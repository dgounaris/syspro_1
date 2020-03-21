#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "transaction.h"
#include "wallet.h"
#include "bc_transaction.h"
#include "service.h"

int valDT(char* mydate, char* mytime, int src) {
	static char* lastdate = NULL;
	static char* lasttime = NULL;
	if (lastdate == NULL) {
		lastdate = malloc(32*sizeof(char));
		strcpy(lastdate, mydate);
	}
	if (lasttime == NULL) {
		lasttime = malloc(32*sizeof(char));
		strcpy(lasttime, mytime);
	}
	if (src != 1) {
		if (datecmp(lastdate, mydate) > 0) {
			return -1;
		} else if (datecmp(lastdate, mydate) < 0) {
			strcpy(lastdate, mydate);
			strcpy(lasttime, mytime);
			return 1;
		} else if (timecmp(lasttime, mytime) > 0) {
			return -1;
		}
	} else if (datecmp(lastdate, mydate) < 0 || (datecmp(lastdate, mydate) == 0 && timecmp(lasttime, mytime) > 0)) {
		strcpy(lastdate, mydate);
		strcpy(lasttime, mytime);
	}
	return 1;
}

int valTransaction(int tid, WInfo* sw, WInfo* rw, int amount) {
	//both users exist
	if (sw == NULL || rw == NULL) {
		return -1;
	}
	//balance is enough
	printf("%s balance validation: %d\n", sw->wId, sw->balance);
	if (sw->balance < amount) {
		return -2;
	}
	return 1;
}

int execBcT(int amount, WInfo* sw, WInfo* rw, BCTHash* bctHash, TNode* sCreated, TNode* rCreated) {
	BCList* bcIds = sw->bcIds;
	sw->balance -= amount;
	rw->balance += amount;
	printf("New %s balance: %d\n", rw->wId, rw->balance);
	do {
		//add the node to rec wallet if not exist
		//here and not before deleting stuff and gets messy
		if (getBCList(rw->bcIds, bcIds->bcId) == NULL) {
			addBCList(&(rw->bcIds), bcIds->bcId);
		}
		//for the currently open bcId, load the node from bch
		BCTNode* bcn = getBCTNode(bctHash, bcIds->bcId);
		//count user leafs and get by wid
		int cwid = count(bcn, sw->wId);
		BCTNode** wIdL = malloc(cwid*sizeof(BCTNode*));
		getByWId(bcn, sw->wId, wIdL);
		//for each leaf, do an inner loop here that passes money
		int currLeaf = 0;
		while (currLeaf < cwid) {
			if ((wIdL[currLeaf])->amount <= amount) {
				addBCTNode(&(wIdL[currLeaf]), rw->wId, rCreated, (wIdL[currLeaf])->amount, -1);
				amount -= (wIdL[currLeaf])->amount;
				if (currLeaf == cwid-1) {
					//if is on last leaf of this bc
					//find and remove bc from bcids
					int bcIdDel = bcIds->bcId;
					bcIds = getBCList(sw->bcIds, bcIds->bcId)->next;
					//the above is required because
					//after removing the bcIds points to deleted area
					removeBCList(&(sw->bcIds), bcIdDel);
				}
			} else if ((wIdL[currLeaf])->amount > amount) {
				addBCTNode(&(wIdL[currLeaf]), rw->wId, rCreated, amount, -1);
				addBCTNode(&(wIdL[currLeaf]), sw->wId, sCreated, (wIdL[currLeaf])->amount - amount, 1);
				amount = 0;
				break;
			}
			currLeaf++;
		}
		free(wIdL);
	} while (amount > 0);
}

int doTransaction(int torigid, char* sid, char* rid, int amount, char* mydate, char* mytime, WHash* wHash, BCTHash* bctHash, THash* sHash, THash* rHash, int src) {
	static int tid = 0;
	if (torigid == 0) {
		tid++;
	} else {
		tid = torigid;
	}
	WInfo* sw = getWHNode(wHash, sid);
	WInfo* rw = getWHNode(wHash, rid);
	int validated = valTransaction(tid, sw, rw, amount);
	if (validated == -1) {
		fprintf(stderr, "Transaction error: User %s does not exist\n", sid);
		return -1;
	} else if (validated == -2) {
		fprintf(stderr, "Transaction error: Insufficient balance\n");
		return -2;
	}
	if (mydate == NULL && mytime == NULL) {
		time_t currtime;
		struct tm* timeinfo;
		time(&currtime);
		timeinfo = localtime(&currtime);
		char newdate[32];
		sprintf(newdate, "%d-%d-%d", timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900);
		char newtime[32];
		sprintf(newtime, "%d:%d", timeinfo->tm_hour, timeinfo->tm_min);
		mydate = malloc(32*sizeof(char));
		strcpy(mydate, newdate);
		mytime = malloc(32*sizeof(char));
		strcpy(mytime, newtime);
	}
	int tvalidated = valDT(mydate, mytime, src);
	if (tvalidated != 1) {
		printf("Transaction error: Time is incorrect\n");
		return 0;
	}
	//sender
	TNode* sNode = getTNodeH(sHash, sid);
	TNode* sCreated = addTNode(&sNode, tid, sid, rid, amount, mydate, mytime);
	addTNodeH(sHash, sNode, 0);
	//receiver
	TNode* rNode = getTNodeH(rHash, rid);
	TNode* rCreated = addTNode(&rNode, tid, sid, rid, amount, mydate, mytime);
	addTNodeH(rHash, rNode, 1);
	execBcT(amount, sw, rw, bctHash, sCreated, rCreated);
	return 1;
}

int datecmp(char* dorig1, char* dorig2) {
	if ((dorig1 == NULL || strlen(dorig1) == 0) && (dorig2 == NULL || strlen(dorig2) == 0)) return 0;
	if (dorig1 == NULL || strlen(dorig2) == 0) return -1;
	if (dorig2 == NULL || strlen(dorig2) == 0) return 1;
	char* d1 = malloc(20*sizeof(char));
	strcpy(d1, dorig1);
	char* d2 = malloc(20*sizeof(char));
	strcpy(d2, dorig2);
	int dd1 = atoi(strtok(d1, "-"));
	int mm1 = atoi(strtok(NULL, "-"));
	int yy1 = atoi(strtok(NULL, "-"));
	int dd2 = atoi(strtok(d2, "-"));
	int mm2 = atoi(strtok(NULL, "-"));
	int yy2 = atoi(strtok(NULL, "-"));
	free(d1); free(d2);
	if (yy1 > yy2) return 1;
	else if (yy1 == yy2 && mm1 > mm2) return 1;
	else if (yy1 == yy2 && mm1 == mm2 && dd1 > dd2) return 1;
	else if (yy1 == yy2 && mm1 == mm2 && dd1 == dd2) return 0;
	else return -1;
}

int timecmp(char* torig1, char* torig2) {
	if ((torig1 == NULL || strlen(torig1) == 0) && (torig2 == NULL || strlen(torig2) == 0)) return 0;
	if (torig1 == NULL || strlen(torig1) == 0) return -1;
	if (torig2 == NULL || strlen(torig2) == 0) return 1;
	char* t1 = malloc(20*sizeof(char));
	strcpy(t1, torig1);
	char* t2 = malloc(20*sizeof(char));
	strcpy(t2, torig2);
	int hh1 = atoi(strtok(t1, ":"));
	int mm1 = atoi(strtok(NULL, ":"));
	int hh2 = atoi(strtok(t2, ":"));
	int mm2 = atoi(strtok(NULL, ":"));
	free(t1); free(t2);
	if (hh1 > hh2) return 1;
	else if (hh1 == hh2 && mm1 > mm2) return 1;
	else if (hh1 == hh2 && mm1 == mm2) return 0;
	else return -1;
}
