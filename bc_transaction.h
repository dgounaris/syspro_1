typedef struct BcTransactionNode {
	char* wId;
	int amount;
	struct transactionNode* transaction;
	struct BcTransactionNode* left;
	struct BcTransactionNode* right;
} BCTNode;

void addBCTNode(BCTNode** bctNode, char* wId, struct transactionNode* transaction, int amount, int side);

int sum(BCTNode* bctNode, char* wId);

void getByWId(BCTNode* bctNode, char* wId, BCTNode** response);

int count(BCTNode* bctNode, char* wId);

void deleteBCTNode(BCTNode* bctNode);

typedef struct BcTHashNodeData {
	int bcId;
	BCTNode* bctNode;
} BCTHNData;

typedef struct BcTHashNode {
	BCTHNData* bctData;
	struct BcTHashNode* next;
} BCTHNode;

typedef struct BctHash {
	int size;
	BCTHNode* table;
} BCTHash;

void initBCTHash(BCTHash* bctHash, int size);

void deleteBCTHash(BCTHash* bctHash, int size);

void deleteBCTHNode(BCTHNode bcthNode);

//throws error when finding existing key
void addBCTHNode(BCTHash* bctHash, int bcId, BCTNode* bctNode);

int getBHash(int bcId, int size);

BCTNode* getBCTNode(BCTHash* bctHash, int bcId);

int countTransactions(BCTNode* bctNode);

int getUnspent(BCTNode* bctNode);

void printTransactions(BCTNode* bctNode);
