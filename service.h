typedef struct WalletInfo WInfo;
typedef struct walletHash WHash;
typedef struct BctHash BCTHash;
typedef struct transactionHash THash;
typedef struct transactionNode TNode;

int valDT(char* date, char* time, int src);

//error code -1, user does not exist
//error code -2, sender doesn't have the amount
//response 1, ok
int valTransaction(int tid, WInfo* sw, WInfo* rw, int amount);

//execute the actual bitcoin transaction on the bc tree
int execBcT(int amount, WInfo* sw, WInfo* rw, BCTHash* bctHash, TNode* sCreated, TNode* rCreated);

//src = 1 file, src = 0 io
int doTransaction(int tid, char* sid, char* rid, int amount, char* date, char* time, WHash* wHash, BCTHash* bctHash, THash* sHash, THash* rHash, int src);

int datecmp(char* d1, char* d2);

int timecmp(char* t1, char* t2);
