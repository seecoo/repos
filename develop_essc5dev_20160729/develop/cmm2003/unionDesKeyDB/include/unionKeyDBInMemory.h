// Author:	Wolfgang Wang
// Date:	2003/8/06

// Version 2.x 2003/09/24,Wolfgang Wang

#ifndef _UnionKeyDBInMemory_
#define _UnionKeyDBInMemory_

#include "unionDesKey.h"

#ifndef _UnionKeyDBInMemory_2_x_
#include "UnionLog.h"

typedef struct
{
	int		idOfKeyDB;	// 用户分配的数据库标识
	int		readUsers;	// 正在读该表的用户数
	int		writeUsers;	// 正在写该表的用户数
	int		connNum;	// 连接数量
	int		num;		// 密钥数量
	int		maxKeyNum;	// 最大的密钥数量
	PUnionDesKey	pkey;		// 密钥表;
} TUnionDesKeyDB;
typedef TUnionDesKeyDB		*PUnionDesKeyDB;

int UnionGetNameOfDesKeyDB(char *fileName,int idOfKeyDB);

int UnionGetMaxKeyNumFromKeyDBFile(int idOfKeyDB);


PUnionDesKeyDB UnionConnectDesKeyDB(int idOfKeyDB);
int UnionDisconnectDesKeyDB(PUnionDesKeyDB);
int UnionRemoveDesKeyDBInMemory(int idOfKeyDB);
int UnionDeleteDesKeyDB(int idOfKeyDB);
int UnionCreateDesKeyDB(int idOfKeyDB,int maxKeyNum);

int UnionLoadDesKeyDBIntoMemory(PUnionDesKeyDB);
int UnionPrintDesKeyDB(PUnionDesKeyDB pkeyDB);
int UnionMirrorDesKeyDBIntoDisk(PUnionDesKeyDB pkeyDB);

int UnionInsertDesKeyIntoKeyDB(PUnionDesKeyDB pkeyDB,PUnionDesKey pkey);
int UnionDeleteDesKeyFromKeyDB(PUnionDesKeyDB pkeyDB,char *fullName);
int UnionReadDesKeyFromKeyDB(PUnionDesKeyDB pkeyDB,PUnionDesKey pkey);
long UnionFindDesKeyPosInKeyDB(PUnionDesKeyDB pkeyDB,char *fullName);
int UnionUpdateDesKeyInKeyDB(PUnionDesKeyDB pkeyDB,PUnionDesKey pkey);

int UnionApplyWritingDesKeyDB(PUnionDesKeyDB pkeyDB);
int UnionReleaseWritingDesKeyDB(PUnionDesKeyDB pkeyDB);
int UnionApplyReadingDesKeyDB(PUnionDesKeyDB pkeyDB);
int UnionReleaseReadingDesKeyDB(PUnionDesKeyDB pkeyDB);

int UnionGetKeyNumOfKeyDB(PUnionDesKeyDB pkeyDB);

#endif // ifndef _UnionKeyDBInMemory_2_x_

#ifdef _UnionKeyDBInMemory_2_x_

#define _UnionLog_3_x_
#include "UnionLog.h"

#define conMDLNameOfUnionDesKeyDB	"MDLOfUnionDesKeyDB"

typedef struct
{
	int		readUsers;	// 正在读该表的用户数
	int		writeUsers;	// 正在写该表的用户数
	int		num;		// 密钥数量
	int		maxKeyNum;	// 最大的密钥数量
	PUnionDesKey	pkey;		// 密钥表;
} TUnionDesKeyDB;
typedef TUnionDesKeyDB		*PUnionDesKeyDB;

int UnionGetNameOfDesKeyDB(char *fileName);

int UnionGetMaxKeyNumFromKeyDBFile();


int UnionConnectDesKeyDB();
int UnionDisconnectDesKeyDB();
int UnionRemoveDesKeyDBInMemory();
int UnionDeleteDesKeyDB();
int UnionCreateDesKeyDB(int maxKeyNum);

int UnionLoadDesKeyDBIntoMemory();
int UnionPrintDesKeyDB();
int UnionPrintDesKeyDBToFile(PUnionDesKeyDB pgunionDesKeyDB,FILE *fp);
int UnionMirrorDesKeyDBIntoDisk(PUnionDesKeyDB pgunionDesKeyDB);

int UnionInsertDesKeyIntoKeyDB(PUnionDesKey pkey);
int UnionDeleteDesKeyFromKeyDB(char *fullName);
int UnionReadDesKeyFromKeyDB(PUnionDesKey pkey);
long UnionFindDesKeyPosInKeyDB(char *fullName);
int UnionUpdateDesKeyInKeyDB(PUnionDesKey pkey);

int UnionApplyWritingDesKeyDB(PUnionDesKeyDB pgunionDesKeyDB);
int UnionReleaseWritingDesKeyDB(PUnionDesKeyDB pgunionDesKeyDB);
int UnionApplyReadingDesKeyDB(PUnionDesKeyDB pgunionDesKeyDB);
int UnionReleaseReadingDesKeyDB(PUnionDesKeyDB pgunionDesKeyDB);

int UnionGetKeyNumOfKeyDB();

#endif // ifdef _UnionKeyDBInMemory_2_x_

#endif
