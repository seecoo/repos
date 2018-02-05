//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2012-05-25

#ifndef _commWithSftService_
#define _commWithSftService_
#include "unionMXML.h"

typedef struct
{
	char	name[128+1];//�ļ���
	int		size;//��С
	int		ctime;//����޸�ʱ��
	char	sclientNo[16+1];
	char	sclientName[32+1];
}TFF000;

typedef struct
{
	char	clientNo[16];
	char	clientName[64+1];
	int		gid;
}TCLIENTINFO;

typedef struct
{
	int		id;
	int		pid;
	char	name[64];
}TGROUP;

typedef struct
{
	char	IDName[64];
	char 	LevelName[64];
	int 	level;
	int 	loginTimes;
	int	lastLoginTime;	
}TUSER;

typedef struct
{
	int		count;
	TGROUP	*group;
}TSF001;

typedef struct
{
	char	transferDir[512+1];//����Ŀ¼
	int		allNum;//ȫ���ļ�����
	int		fileNum;//�����ļ�����
	int		page;//��ǰҳ��
	TFF000	*files;//�ļ���Ϣ
}TSF000;

typedef struct
{
	TGROUP	pGroup;
	int		clientCount;
	int		groupCount;
	TCLIENTINFO	*client;
	TGROUP	*group;
}TSF002;

typedef struct
{
	char 	clientNo[16];
	int 	totalNum;
	TUSER	*pUser;
}TSF007;

typedef struct
{
	int 	port;
	char  	path[32+1];
}TSF003;

typedef struct
{
	int 	result;	
	char  	prefix[32+1];
	char  	infix[32+1];
	char 	suffix[32+1];
}TSF010;;
int UnionCommWithSftService(char *serviceIPAddr,int servicePort,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr);

int UnionCheckResponse(TUnionXMLCtxt *pmlc, char *resMark);

int UnionSftFormHead(char *serviceCode, PUnionXMLCtxt pxmlc);

/*
���ã���ȡF000������
���룺
�����
���أ�
*/
int SftServiceF000(char *ip, int port, char *clientNo, int sort, int page, int pageSize, TSF000  *ts);


/*
���ã���ȡF001������
���룺
�����
���أ�
*/
int SftServiceF001(char *ip, int port, TSF001  *ts);

/*
���ã���ȡF002������
���룺
�����
���أ�
*/
int SftServiceF002(char *ip, int port,char *clientNo, int groupID, TSF002  *ts);


/*
���ã���ȡF003������
���룺
�����
���أ�
*/
int SftServiceF003(char *ip, int port,char *clientNo, TSF003  *ts);


/*
���ã���ȡF004������
���룺
�����
���أ�
*/
int SftServiceF004(char *ip, int port);


/*
���ã���ȡF005������
���룺
�����
���أ�
*/
int SftServiceF005(char *ip, int port,char *clientNo,char *oriName,char *desName,int action );


/*
���ã���ȡF006������
���룺
�����
���أ�
*/
int SftServiceF006(char *ip, int port,char *clientNo,char *fileName,int fileSize,int direction,int status );

/*
���ã���ȡF007������
���룺
�����
���أ�
*/
int SftServiceF007(char *ip, int port,char *clientNo,TSF007 *ts );

/*
���ã���ȡF008������
���룺
�����
���أ�
*/
int SftServiceF008(char *ip, int port,char *clientNo,char *account,char *passwd );


/*
���ã���ȡF009������
���룺
�����
���أ�
*/
int SftServiceF009(char *ip, int port,char *clientNo,int action,char *account,char *passwd,char *newAccount,char *newPasswd );

/*
���ã���ȡF010������
���룺
�����
���أ�
*/
int SftServiceF010(char *ip, int port,char *clientNo,int action,char *fileName,int fileSize,TSF010 *ts );
#endif
