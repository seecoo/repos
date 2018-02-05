//	Wolfgang Wang
//	2003/1/19

#ifndef _UnionCopyrightRegistry_
#define _UnionCopyrightRegistry_

#include <stdio.h>
#include <string.h>

typedef struct
{
	char	abbrName[20+1];		// 产品简称
	char	primaryName[80+1];	// 产品主名称
	char	minorName[80+1];	// 产品次名称
	char	version[20+1];		// 产品版本号
	char	userName[80+1];		// 用户名称，即产品使用者名称
	int	userNumber;		// 支持的最大用户数
	char	serialNumber[16+1];	// 产品的序列号
	char	verificationCode[16+1];	// 产品的验证码
	char	lisenceData[16+1];	// 产品的验证数据
	char	fixedLisenceData[64+1];	// 产品的固化验证数据
	char	registerDateTime[14+1];	// 生成日期
} TUnionCopyrightRegistry;
typedef TUnionCopyrightRegistry	*PUnionCopyrightRegistry;

int UnionVerifyCopyrightRegistryUserNumber(int userNumber);
int UnionInitCopyrightRegistryStaticData(PUnionCopyrightRegistry pCopyrightRegistry,char *abbrProductName);
int UnionOrganizeCopyrightRegistryDynamicData(PUnionCopyrightRegistry pCopyrightRegistry);

int UnionStoreCopyrightRegistry(PUnionCopyrightRegistry pCopyrightRegistry,char *nameOfProduct);
int UnionGenerateCopyrightRegistry(PUnionCopyrightRegistry pCopyrightRegistry);
int UnionVerifyCopyrightRegistry(PUnionCopyrightRegistry pCopyrightRegistry);

int UnionReadUserCopyrightRegistry(PUnionCopyrightRegistry pCopyrightRegistry);

int UnionVerifyUserCopyrightRegistry(char *abbrProductName);
int UnionGenerateUserCopyrightRegistry(char *abbrProductName);

#ifdef _UnionCopyrightRegistry_2_x_
int UnionPrintCopyrightRegistry(PUnionCopyrightRegistry pCopyrightRegistry);
#endif


#endif
