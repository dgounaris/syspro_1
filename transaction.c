#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "transaction.h"
#include "service.h"

TNode* addTNode(TNode** tNode, int id, char* wIdSend, char* wIdRec, int amount, char* date, char* time) {
	TNode* prev = NULL;
	TNode* curr = *tNode;
	while (curr != NULL) {
		prev = curr;
		curr = curr->next;
	}
	if (prev == NULL) {
		*tNode = malloc(sizeof(TNode));
		(*tNode)->id = id;
		(*tNode)->wIdSend = malloc(255*sizeof(char));
		strcpy((*tNode)->wIdSend, wIdSend);
		(*tNode)->wIdRec = malloc(255*sizeof(char));
		strcpy((*tNode)->wIdRec, wIdRec);
		(*tNode)->amount = amount;
		(*tNode)->date = malloc(255*sizeof(char));
		strcpy((*tNode)->date, date);
		(*tNode)->time = malloc(255*sizeof(char));
		strcpy((*tNode)->time, time);
		(*tNode)->next = NULL;
		return *tNode;
	} else {
		curr = malloc(sizeof(TNode));
		curr->id = id;
		curr->wIdSend = malloc(255*sizeof(char));
		strcpy(curr->wIdSend, wIdSend);
		curr->wIdRec = malloc(255*sizeof(char));
		strcpy(curr->wIdRec, wIdRec);
		curr->amount = amount;
		curr->date = malloc(255*sizeof(char));
		strcpy(curr->date, date);
		curr->time = malloc(255*sizeof(char));
		strcpy(curr->time, time);
		curr->next = NULL;
		prev->next = curr;
		return curr;
	}
}

void deleteTNode(TNode* tNode) {
	if (tNode->next != NULL) {
		deleteTNode(tNode->next);
	}
	free(tNode->wIdSend);
	free(tNode->wIdRec);
	free(tNode->date);
	free(tNode->time);
	free(tNode);
}

void initTHash(THash* tHash, int size, int bSize) {
	bSize = floor(bSize/(double)sizeof(TBNode)) + 1;
	tHash->buckets = malloc(size*sizeof(TBucket));
	int i;
	for (i=0;i<size;i++) {
		initTBucket(&((*tHash).buckets[i]), bSize);
	}
	tHash->size = size;
}

void initTBucket(TBucket* tb, int bSize) {
	tb->bucketData = malloc(bSize*sizeof(TBNode));
	int i;
	for (i=0;i<bSize;i++) {
		(*tb).bucketData[i].wId = NULL;
	}
	tb->bSize = bSize;
	tb->next = NULL;
}

void deleteTHash(THash* tHash, int size) {
	int i;
	for (i=0;i<size;i++) {
		deleteTBucket((*tHash).buckets[i]);
	}
	free(tHash->buckets);
}

void deleteTBucket(TBucket tBucket) {
	if (tBucket.next != NULL) {
		deleteTBucket(*(tBucket.next));
		free(tBucket.next);
	}
	free(tBucket.bucketData);
}

void addTNodeH(THash* tHash, TNode* tNode, int hashBy) {
	int hv;
	char* hashV;
	if (hashBy == 0) {
		hashV = tNode->wIdSend;
		hv = getHash(tNode->wIdSend, tHash->size);
	} else {
		hashV = tNode->wIdRec;
		hv = getHash(tNode->wIdRec, tHash->size);
	}
	TBucket* tb = &((*tHash).buckets[hv]);
	int i;
	while (1) {
		for (i=0; i < tb->bSize; i++) {
			if ((*tb).bucketData[i].wId == NULL) {
				(*tb).bucketData[i].transactions = tNode;
				(*tb).bucketData[i].wId = hashV;
				return;
			} else if (strcmp((*tb).bucketData[i].wId, hashV) == 0) {
				(*tb).bucketData[i].transactions = tNode; //theoretically will be the same as previously, otherwise possible memory leak!
				//memory leak due to potentially loose tNode*
				return;
			}
		}
		if (tb->next == NULL) {
			tb->next = malloc(sizeof(TBucket));
			initTBucket(tb->next, tb->bSize);
		}
		tb = tb->next;
	}
}

int getHash(char* wId, int size) {
	int strsize = strlen(wId);
	int hash = 7;
	int i;
	for (i = 0; i < strsize; i++) {
		hash = hash*31 + wId[i];
	}
	return abs(hash % size);
}

TNode* getTNodeH(THash* tHash, char* hashV) {
	int hv = getHash(hashV, tHash->size);
	int i;
	TBucket* tb = &((*tHash).buckets[hv]);
	while(1) {
		for (i=0; i < tb->bSize; i++) {
			if ((*tb).bucketData[i].wId == NULL) {
				continue;
			} else if (strcmp((*tb).bucketData[i].wId, hashV) == 0) {
				return (*tb).bucketData[i].transactions;
			}
		}
		if (tb->next != NULL) {
			tb = tb->next;
		} else {
			return NULL;
		}
	}
}

int countTSumS(TNode* tNode, char* t1, char* t2) {
	if (tNode == NULL) {
		return 0;
	} else if (strcmp(tNode->time, t1) >= 0 && strcmp(tNode->time, t2) <= 0) {
		return tNode->amount + countTSumS(tNode->next, t1, t2);
	} else {
		return countTSumS(tNode->next, t1, t2);
	}
}

int countTSumC(TNode* tNode, char* t1, char* y1, char* t2, char* y2) {
	char* totaly1 = malloc(20*sizeof(char));
	char* totaly2 = malloc(20*sizeof(char));
	strcpy(totaly1, "01-01-");
	strcpy(totaly2, "01-01-");
	strcat(totaly1, y1);
	strcat(totaly2, y2);
	if (tNode == NULL) {
		free(totaly1);
		free(totaly2);
		return 0;
	} else if (datecmp(tNode->date, totaly1) >= 0 && datecmp(tNode->date, totaly2) <= 0
				&& timecmp(tNode->time, t1) >= 0 && timecmp(tNode->time, t2) <= 0) {
		free(totaly1);
		free(totaly2);
		return tNode->amount + countTSumC(tNode->next, t1, y1, t2, y2);
	} else {
		free(totaly1);
		free(totaly2);
		return countTSumC(tNode->next, t1, y1, t2, y2);
	}
}

