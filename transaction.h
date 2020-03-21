typedef struct transactionNode {
	int id;
	char* wIdSend;
	char* wIdRec;
	int amount;
	char* date;
	char* time;
	struct transactionNode * next;
} TNode;

//returns the newly inserted node
TNode* addTNode(TNode** tNode, int id, char* wIdSend, char* wIdRec, int amount, char* date, char* time);

void deleteTNode(TNode* tNode);

typedef struct transactionBucketNode {
	TNode* transactions;
	char* wId;
} TBNode;

typedef struct transactionBucket {
	TBNode* bucketData;
	int bSize;
	struct transactionBucket* next;
} TBucket;

typedef struct transactionHash {
	int size;
	TBucket* buckets;
} THash;

void initTHash(THash* tHash, int size, int bSize);

void initTBucket(TBucket* tb, int bSize);

void deleteTHash(THash* tHash, int size);

void deleteTBucket(TBucket tBucket);

void addTNodeH(THash* tHash, TNode* tNode, int hashBy); //0 sender, 1 receiver

int getHash(char* wId, int size);

TNode* getTNodeH(THash* tHash, char* hashV);

int countTSumS(TNode* tNode, char* t1, char* t2);

int countTSumC(TNode* tNode, char* t1, char* y1, char* t2, char* y2);
