/*
 * msgServer.h
 *
 *  Created on: 2017��8��27��
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

/* T_MSG_CALLBACK_FUNC����true������Ҫ����ģ������Ӧ����Ϣ��
 * ������false�����������ĸ���Ϣ��ģ����Լ����������Ϣ
 * ��Ϣ�������У���ֹ������ʱ������������Ϣ�Ĵ�������б���
 * ������ʱ���������轫����Ϣ�Ĵ���ʽ����Ϊ�첽����������������
 * �Ը���Ϣ���д���Ŀǰ�첽�����������Ϣ�Ķ���ģ������ʵ�֣�
 * ���ڿɷ�����Ϣ����ģ����ͳһ���� */
typedef bool (*T_MSG_CALLBACK_FUNC)(T_MSG_SN msgType,
		unsigned char *pMsg, unsigned int msgLen, ...);
		/*void *pProcParam, unsigned int recvPortNum) ;*/

typedef struct __msg_map {

	HASH_NODE hn;

	T_MSG_SN msg;
	T_MSG_CALLBACK_FUNC pFunc;
	void *pProcParam;
	unsigned int priority; /* ֵԽС���ȼ�Խ�� */
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
