#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "transaction.h"
#include "bc_transaction.h"
#include "wallet.h"
#include "service.h"

int parseArgs(int argc, char* argv[], char** bcBFile, char** tFile, int* bcVal, int* shSize, int* rhSize, int* bSize) {
	int nextArg;
	int bcBFlag = 1; int tFlag = 1; int bcFlag = 1; int shFlag = 1; int rhFlag = 1; int bFlag = 1;
	for (nextArg = 1; nextArg < argc; nextArg++) {
		if (strcmp(argv[nextArg], "-a") == 0 && bcBFlag) {
			if (++nextArg < argc) {
				*bcBFile = argv[nextArg];
				bcBFlag = 0;
			} else {
				fprintf(stderr, "Error: Bad parameters format\n");
				return -1;
			}
		} else if (strcmp(argv[nextArg], "-t") == 0 && tFlag) {
			if (++nextArg < argc) {
				*tFile = argv[nextArg];
				tFlag = 0;
			} else {
				fprintf(stderr, "Error: Bad parameters format\n");
				return -1;
			}
		} else if (strcmp(argv[nextArg], "-v") == 0 && bcFlag) {
			if (++nextArg < argc) {
				*bcVal = atoi(argv[nextArg]);
				bcFlag = 0;
			} else {
				fprintf(stderr, "Error: Bad parameters format\n");
				return -1;
			}
		} else if (strcmp(argv[nextArg], "-h1") == 0 && shFlag) {
			if (++nextArg < argc) {
				*shSize = atoi(argv[nextArg]);
				shFlag = 0;
			} else {
				fprintf(stderr, "Error: Bad parameters format\n");
				return -1;
			}
		} else if (strcmp(argv[nextArg], "-h2") == 0 && rhFlag) {
			if (++nextArg < argc) {
				*rhSize = atoi(argv[nextArg]);
				rhFlag = 0;
			} else {
				fprintf(stderr, "Error: Bad parameters format\n");
				return -1;
			}
		} else if (strcmp(argv[nextArg], "-b") == 0 && bFlag) {
			if (++nextArg < argc) {
				*bSize = atoi(argv[nextArg]);
				bFlag = 0;
			} else {
				fprintf(stderr, "Error: Bad parameters format\n");
				return -1;
			}
		}
	}
	return 0;
}

void readBcBFile(char* bcBFile, WHash* wHash, BCTHash* bctHash, int bcVal) {
	FILE* fp;
	char buff[255];
	int bid;
	fp = fopen(bcBFile, "r");
	if (fp == NULL) {
		fprintf(stderr, "Error: Could not open file %s\n", bcBFile);
		exit(11);
	}
	int rs = fscanf(fp, "%s", buff);
	if (rs == -1) {
		return;
	}
	char c;
	int balance = 0;
	BCList* bcList = NULL;
	while ((c = fgetc(fp)) != EOF) {
		if (c == ' ') {
			fscanf(fp, "%d", &bid);
			BCTNode* bctNode = NULL;
			addBCTNode(&bctNode, buff, NULL, bcVal, -1);
			addBCTHNode(bctHash, bid, bctNode);
			addBCList(&bcList, bid);
			balance += bcVal;
		} else {
			if (bcList == NULL) {
				printf("WARN: null bclist on %s\n", buff);
			}
			addWHNode(wHash, buff, bcList, balance);
			balance = 0;
			bcList = NULL;
			int i = fscanf(fp, "%s", buff);
			if (i == -1) {
				break;
			}
		}
	}
}

void readTFile(char* tFile, WHash* wHash, BCTHash* bctHash, THash* sendHash, THash* recHash) {
	FILE* fp;
	char buffs[255];
	char buffr[255];
	char buffd[255];
	char bufft[255];
	int tid;
	int amount;
	fp = fopen(tFile, "r");
	if (fp == NULL) {
		fprintf(stderr, "Error: Could not open file %s\n", tFile);
		exit(12);
	}
	while(1) {
		int rs = fscanf(fp, "%d %s %s %d %s %s", &tid, buffs, buffr, &amount, buffd, bufft);
		if (rs == -1) {
			return;
		}
		doTransaction(tid, buffs, buffr, amount, buffd, bufft, wHash, bctHash, sendHash, recHash, 1);
	}
}

void readVFile(char* vFile, WHash* wHash, BCTHash* bctHash, THash* sendHash, THash* recHash) {
	FILE* fp;
	fp = fopen(vFile, "r");
	if (fp == NULL) {
		fprintf(stderr, "Error: Could not open file %s\n", vFile);
	}
	char buffs[255];
	char buffr[255];
	char buffd[255];
	char bufft[255];
	int tid;
	int amount;
	while(1) {
		int rs = fscanf(fp, "%d %s %s %d %s %s%*[; \t\n]", &tid, buffs, buffr, &amount, buffd, bufft);
		//remove last ;
		int tsize = strlen(bufft);
		bufft[tsize-1] = '\0';
		if (rs == -1) {
			return;
		}
		doTransaction(tid, buffs, buffr, amount, buffd, bufft, wHash, bctHash, sendHash, recHash, 0);
	}
}

int main(int argc, char* argv[]) {
	char* bcBFile = "bcb.txt";
	char* tFile = "t.txt";
	int bcVal = 10;
	int shSize = 512;
	int rhSize = 512;
	int bSize = 10;
	int whSize = 20;
	int bcthSize = 20;
	if (parseArgs(argc, argv, &bcBFile, &tFile, &bcVal, &shSize, &rhSize, &bSize) == -1) {
		exit(13);
	}
	//init structures
	WHash* wHash = malloc(sizeof(WHash));
	initWHash(wHash, whSize);
	BCTHash* bctHash = malloc(sizeof(bctHash));
	initBCTHash(bctHash, bcthSize);
	THash* sendHash = malloc(sizeof(THash));
	initTHash(sendHash, shSize, bSize);
	THash* recHash = malloc(sizeof(THash));
	initTHash(recHash, rhSize, bSize);
	//read
	readBcBFile(bcBFile, wHash, bctHash, bcVal);
	readTFile(tFile, wHash, bctHash, sendHash, recHash);
	//inputs
	size_t buffsize = 2048;
	char* buff = malloc(buffsize*sizeof(char));
	while(1) {
		getline(&buff, &buffsize, stdin);
		char* token;
		token = strtok(buff, " ");
		if (strcmp(token, "/exit\n") == 0) {
			break;
		} else if (strcmp(token, "./walletStatus") == 0) {
			token = strtok(NULL, "\n");
			WInfo* wi = getWHNode(wHash, token);
			if (wi == NULL) {
				printf("WalletId %s does not exist\n", token);
			} else {
				printf("WalletId %s balance %d\n", token, wi->balance);
			}
		} else if (strcmp(token, "/bitCoinStatus") == 0) {
			token = strtok(NULL, "\n");
			int bcid = atoi(token);
			BCTNode* bctNode = getBCTNode(bctHash, bcid);
			if (bctNode == NULL) {
				continue;
			} else {
				printf("%d %d %d\n", bcid, countTransactions(bctNode), getUnspent(bctNode));
			}
		} else if (strcmp(token, "/traceCoin") == 0) {
			token = strtok(NULL, "\n");
			int bcid = atoi(token);
			BCTNode* bctNode = getBCTNode(bctHash, bcid);
			if (bctNode == NULL) {
				continue;
			} else {
				printTransactions(bctNode);
			}
		} else if (strcmp(token, "/findPayments") == 0) {
			token = strtok(NULL, " ");
			TNode* tnode = getTNodeH(sendHash, token);
			if (tnode == NULL) {
				printf("0\n");
				continue;
			} else {
				char* dttoken = strtok(NULL, "\n");
				char* t1 = strtok(dttoken, " ");
				char* t2 = strtok(NULL, " ");
				char* t3 = strtok(NULL, " ");
				char* t4;
				if (t3 != NULL) {
					t4 = strtok(NULL, "\n");
					printf("%d\n", countTSumC(tnode, t1, t2, t3, t4));
				} else {
					printf("%d\n", countTSumS(tnode, t1, t2));
				}
			}
		} else if (strcmp(token, "/findEarnings") == 0) {
			token = strtok(NULL, " ");
			TNode* tnode = getTNodeH(recHash, token);
			if (tnode == NULL) {
				printf("0\n");
				continue;
			} else {
				char* dttoken = strtok(NULL, "\n");
				char* t1 = strtok(dttoken, " ");
				char* t2 = strtok(NULL, " ");
				char* t3 = strtok(NULL, " ");
				char* t4;
				if (t3 != NULL) {
					t4 = strtok(NULL, "\n");
					printf("%d\n", countTSumC(tnode, t1, t2, t3, t4));
				} else {
					printf("%d\n", countTSumS(tnode, t1, t2));
				}
			}
		} else if (strcmp(token, "/requestTransaction") == 0) {
			char* swid = strtok(NULL, " ");
			char* rwid = strtok(NULL, " ");
			int amount = atoi(strtok(NULL, " "));
			char* date = strtok(NULL, " ");
			char* time = strtok(NULL, "\n");
			doTransaction(0, swid, rwid, amount, date, time, wHash, bctHash, sendHash, recHash, 0);
		} else if (strcmp(token, "/requestTransactions") == 0) {
			char* v1 = strtok(NULL, "\n");
			int v1size = strlen(v1);
			if (v1[v1size-1] == ';') {
				//printf("Cmd read\n");
				char** commands = malloc(sizeof(char)*strlen(v1)/6);
				char* com = strtok(v1, ";");
				int index = 0;
				while (com != NULL) {
					commands[index] = com;
					index++;
					com = strtok(NULL, ";");
				}
				int lastindex = index;
				index = 0;
				while (index < lastindex) {
					char* swid = strtok(commands[index], " ");
					char* rwid = strtok(NULL, " ");
					int amount = atoi(strtok(NULL, " "));
					char* date = strtok(NULL, " ");
					char* time = strtok(NULL, " "); //" " is important to remove trailing spaces
					doTransaction(0, swid, rwid, amount, date, time, wHash, bctHash, sendHash, recHash, 0);
					index++;
				}
				free(commands);
			} else {
				printf("infile read from %s\n", v1);
				readVFile(v1, wHash, bctHash, sendHash, recHash);
			}
		} else {
			printf("Unrecognized command\n");
		}
	}
	//cleanup
	deleteWHash(wHash, whSize);
	deleteBCTHash(bctHash, bcthSize);
	deleteTHash(sendHash, shSize);
	deleteTHash(recHash, rhSize);
	return 0;
}
