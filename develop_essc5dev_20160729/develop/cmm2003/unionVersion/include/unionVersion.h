//	Author:		Wolfgang Wang
//	Date:		2003/09/19
//	Version:	1.0


// 2004/11/25， 增加了函数
//	UnionReadPredefinedProductEffectiveDays
//	UnionIsProductStillEffective
//	UnionGetStillEffectiveDaysOfProduct

#ifndef _UnionVersion_
#define _UnionVersion_

#include <stdio.h>

typedef struct
{
	char	name[80+1];	// 产品名称
	char	version[20+1];	// 版本号
//	char	date[14+1];	// 生成时间
//	char	author[40+1];	// 生成人
} TUnionVersion;
typedef TUnionVersion		*PUnionVersion;

int UnionSetProductName(char *name);
int UnionSetProductVersionNumber(char *version);
//int UnionSetProductDate(char *date);
//int UnionSetProductAuthor(char *author);

int UnionReadProductName(char *name);
int UnionReadProductVersionNumber(char *version);
//int UnionReadProductDate(char *date);
//int UnionReadProductAuthor(char *author);

//int UnionSetProductVersion(char *name,char *version,char *author,char *date);
int UnionSetProductVersion(char *name,char *version);
int UnionPrintProductVersionToFile(FILE *fp);
char *UnionReadPredfinedProductVersionNumber();

int UnionResetProductVersionNumber();

char *UnionGetApplicationSystemName();

int UnionSetApplicationName(char *appName);
char *UnionGetApplicationName();

char *UnionGetProductGeneratedTime();

long UnionReadPredefinedProductEffectiveDays();	// Added by Wolfang Wang, 2004/11/25

int UnionIsProductStillEffective();	// Added by Wolfang Wang, 2004/11/25

// Added by Wolfang Wang, 2004/11/25	
long UnionGetStillEffectiveDaysOfProduct();

char *UnionGetDataBaseType();

#endif
