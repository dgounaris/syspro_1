typedef struct BCList {
	int bcId;
	struct BCList* next;
} BCList;

typedef struct WalletInfo {
	char* wId;
	BCList* bcIds;
	int balance;
} WInfo;

typedef struct walletHashNode {
	WInfo* wInfo;
	struct walletHashNode* next;
} WHNode;

typedef struct walletHash {
	WHNode* table;
} WHash;

void initWHash(WHash* wHash, int size);

void deleteWHash(WHash* wHash, int size);

void deleteWHNode(WHNode whNode);

void deleteWInfo(WInfo wInfo);

void deleteBCList(BCList* bcList);

void addWHNode(WHash* wHash, char* wId, BCList* bcIds, int balance); //hashed by wId

int getWHash(char* wId);

WInfo* getWHNode(WHash* wHash, char* hashV);

void addBCList(BCList** bcList, int val);

BCList* getBCList(BCList* bcList, int bcId);

void removeBCList(BCList** bcList, int bcId);
