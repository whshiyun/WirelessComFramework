/*
 * msgServer.c
 *
 *  Created on: 2017年8月27日
 *      Author: Gift
 */

#include <hashTable.h>
#include <stdio.h>
#include <stdlib.h>
#include "msgServer.h"

static P_MSG_MAP __newMsgMap(T_MSG_SN msg, T_MSG_CALLBACK_FUNC pFunc,
		void *pProcParam, unsigned int priority, bool isPublic,
		bool isSyncMsg, unsigned int subscribePort) {

	P_MSG_MAP pMap = (P_MSG_MAP)malloc(sizeof(MSG_MAP));
	if(NULL == pMap)
		return NULL;

	INIT_HLIST_NODE(&(pMap->hn));
	pMap->msg = msg;
	pMap->pFunc = pFunc;
	pMap->pProcParam = pProcParam;
	pMap->priority = priority;
	pMap->isPublic = isPublic;
	pMap->isSyncMsg = isSyncMsg;
	pMap->subscribePort = subscribePort;

	return pMap;
}

static void __delMsgMap(P_MSG_MAP pMsgMap) {

	if(NULL == pMsgMap)
		return ;

	hash_del(&(pMsgMap->hn));

	free(pMsgMap);
}

static void __delMsgMapLink(HASH_HEAD *pHead) {

	P_MSG_MAP pos = NULL;
	hlist_for_each_entry(pos, pHead, hn) {
		__delMsgMap (pos);
	}
}

P_MSG_SERVER newMsgServer() {

	P_MSG_SERVER pServer = (P_MSG_SERVER)malloc(sizeof(MSG_SERVER));
	if(NULL == pServer)
		return NULL;

	hash_init(pServer->ht);

	pServer->htSize = HASH_SIZE(pServer->ht);

	return pServer;
}

int delMsgServer(P_MSG_SERVER pServer) {

	if(NULL == pServer)
		return 0;

	unsigned int i = 0;
	for(i=0; i<pServer->htSize; i++) {
		__delMsgMapLink(&(pServer->ht[i]));
	}

	return 0;
}

bool msgSubscribeFull (P_MSG_SERVER pServer, T_MSG_SN msg,
		T_MSG_CALLBACK_FUNC pFunc, void *pProcParam, unsigned int priority,
		bool isPublic, bool isSyncMsg, unsigned int subscribePort) {

	P_MSG_MAP pMap = __newMsgMap(msg, pFunc, pProcParam, priority, isPublic, isSyncMsg, subscribePort);
	if(NULL == pMap)
		return false;

	HASH_HEAD *pHead = hash_head_get(pServer->ht, msg);
	if((pHead > &(pServer->ht[pServer->htSize - 1])) || (pHead < &(pServer->ht[0]))) {
		__delMsgMap(pMap);
		return false;
	}

	P_MSG_MAP pos = NULL;
	bool insertFlag = false;
	hlist_for_each_entry(pos, pHead, hn) {

		if((pFunc == pos->pFunc) && (pProcParam == pos->pProcParam)) {
			__delMsgMap(pMap);
			insertFlag = true;
			break;
		}

		if(pos->priority > pMap->priority) {
			hash_add_before((HASH_NODE *)(&(pos->hn)), (HASH_NODE *)(&(pMap->hn)));
			insertFlag = true;
			break;
		}
	}

	if(false == insertFlag) {
		hash_add_tail(pHead, &(pMap->hn));
		insertFlag = true;
	}

	return insertFlag;
}

bool msgSubscribe (P_MSG_SERVER pServer, T_MSG_SN msg, T_MSG_CALLBACK_FUNC pFunc,
		void *pProcParam, unsigned int priority, bool isPublic, bool isSyncMsg) {

	return msgSubscribeFull(pServer, msg, pFunc, pProcParam,
			priority, isPublic, isSyncMsg, MSG_SERVER_LOCAL_SUBSCRIBE_PORT_NUM);
}

bool msgUnsubscribe(P_MSG_SERVER pServer, T_MSG_SN msg, T_MSG_CALLBACK_FUNC pFunc, void *pProcParam) {

	HASH_HEAD *pHead = hash_head_get(pServer->ht, msg);
	if((pHead > &(pServer->ht[pServer->htSize - 1])) || (pHead < &(pServer->ht[0]))) {
		return false;
	}

	P_MSG_MAP pos = NULL, n = NULL;
	hlist_for_each_entry_safe(pos, n, pHead, hn) {
		if((pFunc == pos->pFunc) && (pProcParam == pos->pProcParam)) {
			__delMsgMap(pos);
		}
	}

	return true;
}

bool msgPushFull(P_MSG_SERVER pServer, T_MSG_SN msg, unsigned char *pMsgSubstance,
		unsigned int msgLen, unsigned int sendPortNum) {

	HASH_HEAD *pHead = hash_head_get(pServer->ht, msg);
	if((pHead > &(pServer->ht[pServer->htSize - 1])) || (pHead < &(pServer->ht[0]))) {
		return false;
	}

	P_MSG_MAP pos = NULL, n = NULL;
	hlist_for_each_entry_safe(pos, n, pHead, hn) {
		if(true == pos->isSyncMsg) {
			if(NULL == pos->pFunc)
				continue;
			if(true == pos->pFunc(msg, pMsgSubstance, msgLen, pos->pProcParam, sendPortNum))
				break;
		} else {
			/* 若是异步消息，则单独创建任务执行，
			 * 异步消息执行暂时由各模块自行完成，
			 * 不在该模块中实现，今后可以在该模块中实现
			 * 若在本模块实现，则此处增加异步消息处理流程 */
		}
	}

	return true;
}

bool msgPush(P_MSG_SERVER pServer, T_MSG_SN msg,
		unsigned char *pMsgSubstance, unsigned int msgLen) {

	return msgPushFull(pServer, msg, pMsgSubstance,
			msgLen, MSG_SERVER_LOCAL_SUBSCRIBE_PORT_NUM);
}
















