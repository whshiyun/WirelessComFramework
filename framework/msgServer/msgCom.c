/*
 * msgCom.c
 *
 *  Created on: 2017Äê8ÔÂ28ÈÕ
 *      Author: Gift
 */
#include <msgCom.h>
#include <stdlib.h>

#include "linkList.h"

static P_MSG_COM_PORT_INFO __newMsgComPort(P_MSG_COM pCom) {

	if(NULL == pCom)
		return NULL;

	P_MSG_COM_PORT_INFO pPort = (P_MSG_COM_PORT_INFO)malloc(sizeof(MSG_COM_PORT_INFO));
	if(NULL == pPort)
		return NULL;

	pPort->pHead = &(pCom->comPortListHead);
	list_add_tail(pPort->pHead, &(pPort->listNode));


	return pPort;
}

static void __delMsgComPort(P_MSG_COM_PORT_INFO port) {

	if(NULL == port)
		return ;

	list_del(&(port->listNode));

	free(port);
}

static bool __helloPktPro(T_MSG_SN msgType, unsigned char *pMsg,
		unsigned int msgLen, void *pProcParam, unsigned int recvPort) {

	if(NULL == pMsg)
		return true;

	P_MSG_HELLO_PKT pHello = (P_MSG_HELLO_PKT)pMsg;
	P_MSG_COM_PORT_INFO pPort = (P_MSG_COM_PORT_INFO)recvPort;
	P_MSG_COM pCom = list_entry(pPort->pHead, MSG_COM, comPortListHead);
	unsigned int i = 0;

	for(i=0; i<pHello->msgNum; i++) {
		msgSubscribeFull(pCom->pServer, pHello->msgTable[i],
				(T_MSG_CALLBACK_FUNC)pPort->sendFunc, pPort->pSendParam,
				0, false, true, (unsigned int)pPort);
	}

	return true;
}

static unsigned int __createHello(P_MSG_SERVER pS, P_MSG_HELLO_PKT pHello) {

	unsigned int num = 0, i = 0;
	P_MSG_MAP pos = NULL;

	for(i=0; i<pS->htSize; i++) {
		hlist_for_each_entry(pos, &(pS->ht[i]), hn) {
			pHello->msgTable[num] = pos->msg;
			num ++;
		}
	}

	pHello->msgNum = num;

	return (sizeof(MSG_HELLO_PKT) - sizeof(pHello->msgTable) + (num * sizeof(T_MSG_SN)));
}

static void __msgPortSend(P_MSG_COM pCom, T_MSG_SN msgType, unsigned char *pMsg,
		unsigned int msgLen) {

	P_MSG_COM_PORT_INFO pos = NULL;
	list_for_each_entry(pos, &(pCom->comPortListHead), listNode) {
		pos->sendFunc(msgType, pMsg, msgLen, pos->pSendParam);
	}
}

P_MSG_COM newMsgCom(P_MSG_SERVER pServer) {

	P_MSG_COM pCom = (P_MSG_COM)malloc(sizeof(MSG_COM));
	if(NULL == pCom)
		return NULL;

	init_list_node(&(pCom->comPortListHead));

	pCom->pServer = pServer;

	msgSubscribe(pCom->pServer, MSG_COM_HELLO, (T_MSG_CALLBACK_FUNC)__helloPktPro,
			NULL, 0, false, true);

	return pCom;
}

void delMsgCom(P_MSG_COM pCom) {

	if(NULL == pCom)
		return ;

	P_MSG_COM_PORT_INFO pos = NULL;
	list_for_each_entry(pos, &pCom->comPortListHead, listNode) {
		__delMsgComPort (pos);
	}
}

P_MSG_COM_PORT_INFO bindMsgComPort(P_MSG_COM pCom, MSG_COM_SEND_FUNC sendFunc, void *pSendParam) {

	if(NULL == pCom)
		return NULL;

	P_MSG_COM_PORT_INFO pPort = __newMsgComPort(pCom);
	if(NULL == pPort)
		return NULL;

	pPort->sendFunc = sendFunc;
	pPort->pSendParam = pSendParam;

	return pPort;
}

bool unbindMsgComPort(P_MSG_COM_PORT_INFO pPort) {

	if(NULL == pPort)
		return false;

	__delMsgComPort(pPort);

	return true;
}

void msgComPortRecvCallback(T_MSG_SN msgType, unsigned char *pMsg,
		unsigned int msgLen, P_MSG_COM_PORT_INFO pPort) {

	P_MSG_COM pCom = list_entry(pPort->pHead, MSG_COM, comPortListHead);
	msgPushFull(pCom->pServer, msgType, pMsg, msgLen, (unsigned int)pPort);
}

void msgSendHello (P_MSG_COM pCom) {

	static MSG_HELLO_PKT hello;
	unsigned int msgLen = __createHello(pCom->pServer, &hello);

	__msgPortSend(pCom, MSG_COM_HELLO, (unsigned char *)(&hello), msgLen);
}







