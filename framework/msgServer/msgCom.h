/*
 * msgCom.h
 *
 *  Created on: 2017年8月28日
 *      Author: Gift
 */

#ifndef MSGSERVER_MSGCOM_H_
#define MSGSERVER_MSGCOM_H_
#include <stdbool.h>

#include "linkList.h"
#include "msgServer.h"

#define MSG_COM_HELLO	(1) /* msgServer发现消息 */

typedef T_MSG_CALLBACK_FUNC MSG_COM_SEND_FUNC;
typedef T_MSG_CALLBACK_FUNC MSG_COM_RECV_FUNC;

typedef struct __msg_com_port_info {

	LIST_NODE listNode;
	LIST_NODE *pHead;

	MSG_COM_SEND_FUNC sendFunc;
	void *pSendParam;

}MSG_COM_PORT_INFO, *P_MSG_COM_PORT_INFO;

typedef struct __msg_com {

	LIST_NODE comPortListHead;

	P_MSG_SERVER pServer;

}MSG_COM, *P_MSG_COM;

typedef struct __msg_hello_pkt {

	unsigned char locId[MSG_SERVER_LOCAL_ID_LEN];
	unsigned int msgNum;
	T_MSG_SN msgTable[MSG_SERVER_MAX_SUBSCRIBE_NUM];

}__attribute__ ((__packed__)) MSG_HELLO_PKT, *P_MSG_HELLO_PKT;

P_MSG_COM newMsgCom(P_MSG_SERVER pServer);
void delMsgCom(P_MSG_COM pCom);
P_MSG_COM_PORT_INFO bindMsgComPort(P_MSG_COM pCom,
		MSG_COM_SEND_FUNC sendFunc, void *pSendParam);
bool unbindMsgComPort(P_MSG_COM_PORT_INFO pPort);
void msgComPortRecvCallback(T_MSG_SN msgType, unsigned char *pMsg,
		unsigned int msgLen, P_MSG_COM_PORT_INFO pPort);
void msgSendHello (P_MSG_COM pCom);

#endif /* MSGSERVER_MSGCOM_H_ */
