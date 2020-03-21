#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bc_transaction.h"
#include "transaction.h"

void addBCTNode(BCTNode** bctNode, char* wId, struct transactionNode* transaction, int amount, int side) {
	if (*bctNode == NULL) {
		*bctNode = malloc(sizeof(BCTNode));
		(*bctNode)->wId = malloc(255*sizeof(char));
		strcpy((*bctNode)->wId, wId);
		(*bctNode)->transaction = transaction;
		(*bctNode)->amount = amount;
		(*bctNode)->left = NULL;
		(*bctNode)->right = NULL;
		return;
	}
	BCTNode* newNode = malloc(sizeof(BCTNode));
	newNode->wId = malloc(255*sizeof(char));
	strcpy(newNode->wId, wId);
	newNode->transaction = transaction;
	newNode->amount = amount;
	newNode->left = NULL;
	newNode->right = NULL;
	if (side == -1) {
		(*bctNode)->left = newNode;
	} else {
		(*bctNode)->right = newNode;
	}
}

int sum(BCTNode* bctNode, char* wId) {
	if (bctNode->left == NULL && bctNode->right == NULL && strcmp(bctNode->wId, wId) == 0) {
		return bctNode->amount;
	} else if (bctNode->left != NULL && bctNode->right != NULL) {
		return sum(bctNode->left, wId) + sum(bctNode->right, wId);
	} else if (bctNode->left != NULL) {
		return sum(bctNode->left, wId);
	} else if (bctNode->right != NULL) {
		return sum(bctNode->right, wId);
	} else {
		return 0;
	}
}

void getByWId(BCTNode* bctNode, char* wId, BCTNode** response) {
	if (bctNode->left == NULL && bctNode->right == NULL && strcmp(bctNode->wId, wId) == 0) {
		int i=0;
		while (response[i] != NULL) {
			i++;
		}
		response[i] = bctNode;
	}
	if (bctNode->left != NULL) {
		getByWId(bctNode->left, wId, response);
	}
	if (bctNode->right != NULL) {
		getByWId(bctNode->right, wId, response);
	}
}

int count(BCTNode* bctNode, char* wId) {
	if (bctNode->left == NULL && bctNode->right == NULL && strcmp(bctNode->wId, wId) == 0) {
		return 1;
	} else if (bctNode->left != NULL && bctNode->right != NULL) {
		return count(bctNode->left, wId) + count(bctNode->right, wId);
	} else if (bctNode->left != NULL) {
		return count(bctNode->left, wId);
	} else if (bctNode->right != NULL) {
		return count(bctNode->right, wId);
	} else {
		return 0;
	}
}

void deleteBCTNode(BCTNode* bctNode) {
	if (bctNode->left != NULL) {
		deleteBCTNode(bctNode->left);
	}
	if (bctNode->right != NULL) {
		deleteBCTNode(bctNode->right);
	}
	free(bctNode->wId);
	free(bctNode);
}

void initBCTHash(BCTHash* bctHash, int size) {
	bctHash->table = malloc(size*sizeof(BCTHNode));
	int i;
	for (i=0;i<size;i++) {
		(*bctHash).table[i].bctData = NULL;
		(*bctHash).table[i].next = NULL;
	}
	bctHash->size = size;
}

void deleteBCTHash(BCTHash* bctHash, int size) {
	int i;
	for (i=0;i<size;i++) {
		deleteBCTHNode((*bctHash).table[i]);
	}
	free(bctHash->table);
}

void deleteBCTHNode(BCTHNode bcthNode) {
	if (bcthNode.next != NULL) {
		deleteBCTHNode(*(bcthNode.next));
		free(bcthNode.next);
	}
	if (bcthNode.bctData != NULL && (bcthNode.bctData)->bctNode != NULL) {
		deleteBCTNode((bcthNode.bctData)->bctNode);
	}
	if (bcthNode.bctData != NULL) {
		free(bcthNode.bctData);
	}
}

void addBCTHNode(BCTHash* bctHash, int bcId, BCTNode* bctNode) {
	int hv = getBHash(bcId, bctHash->size);
	BCTHNode* nextNode = &((*bctHash).table[hv]);
	while(1) {
		if (nextNode->bctData == NULL) {
			nextNode->bctData = malloc(sizeof(BCTHNData));
			nextNode->bctData->bcId = bcId;
			nextNode->bctData->bctNode = bctNode;
			return;
		} else if (nextNode->bctData->bcId == bcId) {
			fprintf(stderr, "BCHT Error! Conflicting bitcoin key %d\n", bcId);
			exit(EXIT_FAILURE);
		} else if (nextNode->next != NULL) {
			nextNode = nextNode->next;
		} else {
			nextNode->next = malloc(sizeof(BCTHNode));
			nextNode->next->bctData = NULL;
			nextNode->next->next = NULL;
			nextNode = nextNode->next;
		}
	}
}

int getBHash(int bcId, int size) {
	bcId ^= (bcId << 13);
	bcId ^= (bcId >> 17);
	bcId ^= (bcId << 5);
	return abs(bcId % size);
}

BCTNode* getBCTNode(BCTHash* bctHash, int bcId) {
	int hv = getBHash(bcId, bctHash->size);
	BCTHNode* nextNode = &((*bctHash).table[hv]);
	while(1) {
		if (nextNode->bctData->bcId == bcId) {
			//printf("BHT returning bct with id %d\n", bcId);
			return nextNode->bctData->bctNode;
		} else if (nextNode->next != NULL) {
			//printf("BHT Going to next node\n");
			nextNode = nextNode->next;
		} else {
			printf("BHT Not found bid %d\n", bcId);
			return NULL;
		}
	}
}

int countTransactions(BCTNode* bctNode) {
	if (bctNode->left == NULL) {
		return 0;
	} else if (bctNode->left != NULL && bctNode->right != NULL) {
		return 1 + countTransactions(bctNode->left) + countTransactions(bctNode->right);
	} else if (bctNode->left != NULL) {
		return 1 + countTransactions(bctNode->left);
	}
}

int getUnspent(BCTNode* bctNode) {
	if (bctNode->right == NULL && bctNode->left == NULL) {
		return bctNode->amount;
	} else if (bctNode->right == NULL && bctNode->left != NULL) {
		return 0;
	} else {
		return getUnspent(bctNode->right);
	}
}

void printTransactions(BCTNode* bctNode) {
	if (bctNode->left != NULL) {
		TNode* tnode = bctNode->left->transaction;
		printf("%d %s %s %d %s %s\n", tnode->id, tnode->wIdSend, tnode->wIdRec, tnode->amount, tnode->date, tnode->time);
		printTransactions(bctNode->left);
	}
	if (bctNode->right != NULL) {
		//printTransactions(bctNode->right);
	}
}
