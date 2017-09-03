/*
 * msgServer.h
 *
 *  Created on: 2017年8月27日
 *      Author: Gift
 */

#ifndef MSGSERVER_MSGSERVER_H_
#define MSGSERVER_MSGSERVER_H_

#include <hashTable.h>
#include <stdbool.h>

#define MSG_SERVER_HASH_TABLE_SIZE_BIT		(10)
#define MSG_SERVER_LOCAL_ID_LEN				(6)
#define MSG_SERVER_MAX_SUBSCRIBE_NUM		(256)
#define MSG_SERVER_LOCAL_SUBSCRIBE_PORT_NUM	(0)

typedef unsigned int T_MSG_SN;

/* T_MSG_CALLBACK_FUNC返回true，则不需要其他模块再响应该消息，
 * 若返回false，则其他订阅该消息的模块可以继续处理该消息
 * 消息处理函数中，禁止引入延时操作，若该消息的处理操作中必须
 * 引入延时操作，则需将该消息的处理方式设置为异步处理，再其他任务中
 * 对该消息进行处理，目前异步处理操作由消息的订阅模块自行实现，
 * 后期可放入消息服务模块中统一管理 */
typedef bool (*T_MSG_CALLBACK_FUNC)(T_MSG_SN msgType,
		unsigned char *pMsg, unsigned int msgLen, ...);
		/*void *pProcParam, unsigned int recvPortNum) ;*/

typedef struct __msg_map {

	HASH_NODE hn;

	T_MSG_SN msg;
	T_MSG_CALLBACK_FUNC pFunc;
	void *pProcParam;
	unsigned int priority; /* 值越小优先级越高 */
	bool isPublic;
	bool isSyncMsg;
	unsigned int subscribePort;

}MSG_MAP, *P_MSG_MAP;

typedef struct __msg_server {

	HASH_HEAD ht[(1 << (MSG_SERVER_HASH_TABLE_SIZE_BIT))];
	unsigned int htSize;

}MSG_SERVER, *P_MSG_SERVER;


P_MSG_SERVER newMsgServer();
int delMsgServer(P_MSG_SERVER pServer);
bool msgSubscribeFull (P_MSG_SERVER pServer, T_MSG_SN msg, T_MSG_CALLBACK_FUNC pFunc,
		void *pProcParam, unsigned int priority, bool isPublic, bool isSyncMsg, unsigned int subscribePort);
bool msgSubscribe (P_MSG_SERVER pServer, T_MSG_SN msg, T_MSG_CALLBACK_FUNC pFunc,
		void *pProcParam, unsigned int priority, bool isPublic, bool isSyncMsg);
bool msgUnsubscribe(P_MSG_SERVER pServer, T_MSG_SN msg, T_MSG_CALLBACK_FUNC pFunc, void *pProcParam);
bool msgPushFull(P_MSG_SERVER pServer, T_MSG_SN msg, unsigned char *pMsgSubstance,
		unsigned int msgLen, unsigned int sendPortNum);
bool msgPush(P_MSG_SERVER pServer, T_MSG_SN msg, unsigned char *pMsgSubstance, unsigned int msgLen);

#endif /* MSGSERVER_MSGSERVER_H_ */
