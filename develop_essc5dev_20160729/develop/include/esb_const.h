#ifndef ESB_CONST
#define ESB_CONST

#include "esb_md5.h"

/* deled by PengLy 20130829
char dcfs_ESB_Version[20];
char dcfs_ESB_Encoding[20];
*/

#define MAX_FILE_PIECE 512*1024+9

typedef struct stNode {
	struct stNode *stParentNode;/*���ڵ�*/
	struct stNode *stChildNode;/*�ӽڵ�*/
	struct stNode *stCompeerNode;/*ƽ���ڵ�*/
	struct stNodeType *stNodeType;
	char *sNodeName;/*�ڵ�����*/
	char *sNodeTypeValue;/*�ڵ�����ֵ*/
	char *sNodeValue;/*�ڵ�ֵ*/
	int iNodeType;/*�ڵ�����*/
	int iNameLen;/*�ڵ����ֳ���*/
	int iValueLen;/*�ڵ�ֵ����*/	
	int iTypeValueLen;/*�ڵ�����ֵ����*/
	int iNodeDepth;/*�ڵ����*/
	int iNodeWidth;/*�ڵ���*/
} stNode;

typedef struct stField {
	char *name;	/*����*/
	char *type;	/*����*/
	int length;	/*����*/
	int scale;	/*����*/
	char *encryptmode;/*����ģʽ*/
	char *value;	/*ֵ*/
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
	char desBuffer[MAX_FILE_PIECE];	/* add by sfg 2014.7.12, for readCont & writeCont����ʱ��������������������� */
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

