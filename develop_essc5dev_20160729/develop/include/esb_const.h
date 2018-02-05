#ifndef ESB_CONST
#define ESB_CONST

#include "esb_md5.h"

/* deled by PengLy 20130829
char dcfs_ESB_Version[20];
char dcfs_ESB_Encoding[20];
*/

#define MAX_FILE_PIECE 512*1024+9

typedef struct stNode {
	struct stNode *stParentNode;/*父节点*/
	struct stNode *stChildNode;/*子节点*/
	struct stNode *stCompeerNode;/*平级节点*/
	struct stNodeType *stNodeType;
	char *sNodeName;/*节点名字*/
	char *sNodeTypeValue;/*节点属性值*/
	char *sNodeValue;/*节点值*/
	int iNodeType;/*节点类型*/
	int iNameLen;/*节点名字长度*/
	int iValueLen;/*节点值长度*/	
	int iTypeValueLen;/*节点属性值长度*/
	int iNodeDepth;/*节点深度*/
	int iNodeWidth;/*节点广度*/
} stNode;

typedef struct stField {
	char *name;	/*名称*/
	char *type;	/*类型*/
	int length;	/*长度*/
	int scale;	/*精度*/
	char *encryptmode;/*加密模式*/
	char *value;	/*值*/
} stField;

typedef struct stFileBean {
	char clientIP[32];	
	char serverIP[32];	
	char fileName[1024];
	char clientFileName[1024];
	char serverName[80];
	char uid[80];
	char passwd[80];
	int authFlag;
	char fileMsgFlag[80];
	long fileSize;
	int fileIndex;
	int pieceNum;
	int  headLen;
	char headCont[32*1024];
	int contLen;
	char fileCont[MAX_FILE_PIECE];
	int lastPiece;
	char md5[32+1];
	int scrtFlag;
	char desKey[40+1];
	char desBuffer[MAX_FILE_PIECE];	/* add by sfg 2014.7.12, for readCont & writeCont，临时变量，不需与服务器交互 */
} stFileBean;

typedef struct stFile {
	char remoteFileName [1024];
	char localFileName [1024];
	char tmpFileName [1024];
	char cfgFileName [1024];
	FILE * destFile;
	int putFlag;
	int sock;
  char servIp[32];
  int servPort;	
struct Md5_ctx struMd5Context;
} stFile;

typedef struct stKey {
char uid[80];
char macKey [25];
char pinKey [25];
char workDate [9];
long  time;
int  failNum;
} stKey;

#endif

