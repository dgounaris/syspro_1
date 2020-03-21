#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wallet.h"

void initWHash(WHash* wHash, int size) {
	wHash->table = malloc(size*sizeof(WHNode));
	int i;
	for (i=0;i<size;i++) {
		(*wHash).table[i].wInfo = NULL;
		(*wHash).table[i].next = NULL;
	}
}

void deleteWHash(WHash* wHash, int size) {
	int i;
	for (i=0;i<size;i++) {
		deleteWHNode((*wHash).table[i]);
	}
	free(wHash->table);
}

void deleteWHNode(WHNode whNode) {
	if (whNode.next != NULL) {
		deleteWHNode(*(whNode.next));
		free(whNode.next);
	}
	if (whNode.wInfo != NULL) {
		deleteWInfo(*(whNode.wInfo));
	}
	free(whNode.wInfo);
}

void deleteWInfo(WInfo wInfo) {
	if (wInfo.wId != NULL) {
		free(wInfo.wId);
	}
	if (wInfo.bcIds == NULL) {
		return;
	}
	deleteBCList(wInfo.bcIds);
}

void deleteBCList(BCList* bcList) {
	if (bcList->next != NULL) {
		deleteBCList(bcList->next);
	}
	if (bcList != NULL) {
		free(bcList);
	}
}

void addWHNode(WHash* wHash, char* wId, BCList* bcIds, int balance) {
	int hv = getWHash(wId);
	WHNode* nextNode = &((*wHash).table[hv]);
	//printf("WRITES %s\n", wId);
	while(1) {
		if (nextNode->wInfo == NULL) {
			nextNode->wInfo = malloc(sizeof(WInfo));
			nextNode->wInfo->wId = malloc(255*sizeof(char));
			strcpy(nextNode->wInfo->wId, wId);
			nextNode->wInfo->bcIds = bcIds;
			nextNode->wInfo->balance = balance;
			printf("WHT Wallet id %s written successfully\n", wId);
			//printf("WHT Written %s %d\n", wId, balance);
			return;
		} else if (strcmp(nextNode->wInfo->wId, wId)==0) {
			fprintf(stderr, "WHT Error: Wallet id %s already exists\n", wId);
			exit(EXIT_FAILURE);
			//nextNode->wInfo->bcIds = bcIds;
			//potential point of mem leak due to lost pointer
			//nextNode->wInfo->balance = balance;
			return;
		} else if (nextNode->next != NULL) {
			nextNode = nextNode->next;
			//printf("WHT Go to next ptr\n");
		} else {
			nextNode->next = malloc(sizeof(WHNode));
			nextNode->next->wInfo = NULL;
			nextNode->next->next = NULL;
			nextNode = nextNode->next;
			//printf("WHT Create next ptr\n");
		}
	}
}

int getWHash(char* wId) {
	return 0;
}

WInfo* getWHNode(WHash* wHash, char* hashV) {
	int hv = getWHash(hashV);
	WHNode* nextNode = &((*wHash).table[hv]);
	while(1) {
		if (strcmp(nextNode->wInfo->wId, hashV)==0) {
			return nextNode->wInfo;
		} else if (nextNode->next != NULL) {
			nextNode = nextNode->next;
		} else {
			return NULL;
		}
	}
}

void addBCList(BCList** bcList, int val) {
	if (*bcList == NULL) {
		*bcList = malloc(sizeof(BCList));
		(*bcList)->bcId = val;
		(*bcList)->next = NULL;
		return;
	}
	BCList* cnode = *bcList;
	while (cnode->next != NULL) {
		cnode = cnode->next;
	}
	cnode->next = malloc(sizeof(BCList));
	cnode->next->bcId = val;
	cnode->next->next = NULL;
}

BCList* getBCList(BCList* bcList, int bcId) {
	if (bcList == NULL) {
		return NULL;
	} else if (bcList->bcId == bcId) {
		return bcList;
	} else if (bcList->next == NULL) {
		return NULL;
	} else {
		return getBCList(bcList->next, bcId);
	}
}

void removeBCList(BCList** bcList, int bcId) {
	BCList* prev = NULL;
	BCList* toDel = (*bcList);
	if (toDel == NULL) {
		return;
	}
	while (toDel->bcId != bcId) {
		if (toDel->next != NULL) {
			prev = toDel;
			toDel = toDel->next;
		} else {
			printf("WBCL Could not find the removed element\n");
			return;
		}
	}
	if (prev == NULL) {
		(*bcList) = (*bcList)->next;
		free(toDel);
	} else {
		prev->next = toDel->next;
		free(toDel);
	}
}
