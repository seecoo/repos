//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2012-05-25

#ifndef _commWithSftService_
#define _commWithSftService_
#include "unionMXML.h"

typedef struct
{
	char	name[128+1];//文件名
	int		size;//大小
	int		ctime;//最后修改时间
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
	char	transferDir[512+1];//传输目录
	int		allNum;//全部文件数量
	int		fileNum;//返回文件数量
	int		page;//当前页数
	TFF000	*files;//文件信息
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
作用：获取F000服务报文
输入：
输出：
返回：
*/
int SftServiceF000(char *ip, int port, char *clientNo, int sort, int page, int pageSize, TSF000  *ts);


/*
作用：获取F001服务报文
输入：
输出：
返回：
*/
int SftServiceF001(char *ip, int port, TSF001  *ts);

/*
作用：获取F002服务报文
输入：
输出：
返回：
*/
int SftServiceF002(char *ip, int port,char *clientNo, int groupID, TSF002  *ts);


/*
作用：获取F003服务报文
输入：
输出：
返回：
*/
int SftServiceF003(char *ip, int port,char *clientNo, TSF003  *ts);


/*
作用：获取F004服务报文
输入：
输出：
返回：
*/
int SftServiceF004(char *ip, int port);


/*
作用：获取F005服务报文
输入：
输出：
返回：
*/
int SftServiceF005(char *ip, int port,char *clientNo,char *oriName,char *desName,int action );


/*
作用：获取F006服务报文
输入：
输出：
返回：
*/
int SftServiceF006(char *ip, int port,char *clientNo,char *fileName,int fileSize,int direction,int status );

/*
作用：获取F007服务报文
输入：
输出：
返回：
*/
int SftServiceF007(char *ip, int port,char *clientNo,TSF007 *ts );

/*
作用：获取F008服务报文
输入：
输出：
返回：
*/
int SftServiceF008(char *ip, int port,char *clientNo,char *account,char *passwd );


/*
作用：获取F009服务报文
输入：
输出：
返回：
*/
int SftServiceF009(char *ip, int port,char *clientNo,int action,char *account,char *passwd,char *newAccount,char *newPasswd );

/*
作用：获取F010服务报文
输入：
输出：
返回：
*/
int SftServiceF010(char *ip, int port,char *clientNo,int action,char *fileName,int fileSize,TSF010 *ts );
#endif
