/*
 * main.c
 *
 *  Created on: 2017Äê8ÔÂ23ÈÕ
 *      Author: Gift
 */

#include <msgCom.h>
#include <stdio.h>
#include "./container/hashTable.h"

#include "msgServer.h"
DEFINE_HASHTABLE(ht, 10);

unsigned int msg = 100;
unsigned int msg2 = 101;
unsigned int msg3 = 123;

bool mc1(T_MSG_SN msgType, unsigned char *pMsg, unsigned int msgLen, void *pProcParam) {
	printf("ms1 : %d \r\n", *((unsigned int *)pProcParam));
	return false;
}

bool mc2(T_MSG_SN msgType, unsigned char *pMsg, unsigned int msgLen, void *pProcParam) {
	printf("ms2 : %d \r\n", *((unsigned int *)pProcParam));
#if 1
	return false;
#else
	return true;
#endif
}

bool mc3(T_MSG_SN msgType, unsigned char *pMsg, unsigned int msgLen, void *pProcParam) {
	printf("ms3 : %d \r\n", *((unsigned int *)pProcParam));
	return false;
}

bool tSend(T_MSG_SN msgType) {

	printf("tSend !!!!!!  %d  \r\n", msgType);

	return false;
}

void tRecvHello(P_MSG_COM_PORT_INFO pp) {
	MSG_HELLO_PKT pkt;

	pkt.msgNum = 2;
	pkt.msgTable[0] = msg3;
	pkt.msgTable[1] = msg;

	msgComPortRecvCallback(MSG_COM_HELLO, (unsigned char *)&pkt, 100, pp);
}

int main() {

	printf("This is a test interface \r\n");

	printf("%d \r\n", HASH_SIZE(ht));

	P_MSG_SERVER ps = newMsgServer();
	P_MSG_COM pc = newMsgCom(ps);
	P_MSG_COM_PORT_INFO pp = bindMsgComPort(pc, (MSG_COM_SEND_FUNC)tSend, NULL);

	msgSubscribe(ps, msg, (T_MSG_CALLBACK_FUNC)mc1, &msg, 1, true, true);
	msgSubscribe(ps, msg, (T_MSG_CALLBACK_FUNC)mc2, &msg, 0, true, true);
	msgSubscribe(ps, msg2, (T_MSG_CALLBACK_FUNC)mc3, &msg2, 6, true, true);
	msgPush(ps, msg, NULL, 0);
	msgPush(ps, msg2, NULL, 0);
/*	msgUnsubscribe(ps, msg, (T_MSG_CALLBACK_FUNC)mc2, &msg);
	msgPush(ps, msg, NULL, 0);
	msgUnsubscribe(ps, msg, (T_MSG_CALLBACK_FUNC)mc1, &msg);
	msgPush(ps, msg, NULL, 0);*/

	tRecvHello(pp);
	msgPush(ps, msg3, NULL, 0);
	msgPush(ps, msg, NULL, 0);

	return 0;
}
