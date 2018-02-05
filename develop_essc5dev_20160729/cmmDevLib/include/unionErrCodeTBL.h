// Wolfgang Wang
// 2005/08/04

#ifndef _unionErrCodeTBL_
#define _unionErrCodeTBL_

#include "unionErrCode.h"

#ifdef _1024ErrCode_
#define conMaxNumOfSoftErrCode			1024
#else
#ifdef _128ErrCode_
#define conMaxNumOfSoftErrCode			128
#else
#define conMaxNumOfSoftErrCode			512
#endif
#endif

#define conMDLNameOfSoftErrCodeTBL		"softErrCodeTBL"
#define conConfFileNameOfSoftErrCodeTBL		"softErrCode.Def"

// ESSC错误码和Client错误码是多对一关系，即多个ESSC错误码可能对应同一个Client错误码。
// 一个ESSC错误码只能对应一个Client错误码。

typedef struct
{
	int			softErrCode;
#ifdef _2LenCliErrCode_
	char			cliErrCode[2+1];
#endif
#ifdef _3LenCliErrCode_
	char			cliErrCode[3+1];
#endif
#ifdef _4LenCliErrCode_
	char			cliErrCode[4+1];
#endif
#ifdef _5LenCliErrCode_
	char			cliErrCode[5+1];
#endif
#ifdef _6LenCliErrCode_
	char			cliErrCode[6+1];
#endif
#ifdef _7LenCliErrCode_
	char			cliErrCode[7+1];
#endif
#ifdef _8LenCliErrCode_
	char			cliErrCode[8+1];
#endif
#ifdef _9LenCliErrCode_
	char			cliErrCode[9+1];
#endif
#ifdef _10LenCliErrCode_
	char			cliErrCode[10+1];
#endif
#ifdef _withSoftErrCodeRemark_
	char			remark[40+1];
#endif
} TUnionSoftErrCode;
typedef TUnionSoftErrCode	*PUnionSoftErrCode;

typedef struct
{
	int			realNum;
	int			defaultSoftErrCode;		// 当CliErrCode未定义对应的softErrCode时，取该值
#ifdef _2LenCliErrCode_
	char			defaultCliErrCode[2+1];		// 当ESSC定义的错误码没有对应的Client错误码时，取该值
#endif
#ifdef _3LenCliErrCode_
	char			defaultCliErrCode[3+1];		// 当ESSC定义的错误码没有对应的Client错误码时，取该值
#endif
#ifdef _4LenCliErrCode_
	char			defaultCliErrCode[4+1];		// 当ESSC定义的错误码没有对应的Client错误码时，取该值
#endif
#ifdef _5LenCliErrCode_
	char			defaultCliErrCode[5+1];		// 当ESSC定义的错误码没有对应的Client错误码时，取该值
#endif
#ifdef _6LenCliErrCode_
	char			defaultCliErrCode[6+1];		// 当ESSC定义的错误码没有对应的Client错误码时，取该值
#endif
#ifdef _7LenCliErrCode_
	char			defaultCliErrCode[7+1];		// 当ESSC定义的错误码没有对应的Client错误码时，取该值
#endif
#ifdef _8LenCliErrCode_
	char			defaultCliErrCode[8+1];		// 当ESSC定义的错误码没有对应的Client错误码时，取该值
#endif
#ifdef _9LenCliErrCode_
	char			defaultCliErrCode[9+1];		// 当ESSC定义的错误码没有对应的Client错误码时，取该值
#endif
#ifdef _10LenCliErrCode_
	char			defaultCliErrCode[10+1];		// 当ESSC定义的错误码没有对应的Client错误码时，取该值
#endif
	TUnionSoftErrCode	rec[conMaxNumOfSoftErrCode];	// 域的值
} TUnionSofterrCodeTBL;
typedef TUnionSofterrCodeTBL	*PUnionSofterrCodeTBL;

int UnionGetNameOfSoftErrCodeTBL(char *fileName);

int UnionIsSoftErrCodeTBLConnected();

int UnionGetMaxNumOfSoftErrCode();

int UnionConnectSoftErrCodeTBL();
	
int UnionDisconnectSoftErrCodeTBL();

int UnionRemoveSoftErrCodeTBL();

int UnionReloadSoftErrCodeTBL();

int UnionPrintSoftErrCodeTBLToFile(FILE *fp);

int UnionPrintSoftErrCodeTBLToSpecFile(char *fileName);

int UnionPrintSoftErrCodeToFile(PUnionSoftErrCode pylqz_cliErrCode,FILE *fp);

PUnionSoftErrCode UnionFindSoftErrCode(char *errCode);

char *UnionFindCliErrCodeOfSoftErrCode(int softErrCode);

int UnionFindSoftErrCodeOfCliErrCode(char *errCode);

int UnionPrintRecOfSoftErrCodeToFile(int softErrCode,FILE *fp);

int UnionPrintAllSoftErrCodeOfCliErrCodeToFile(char *cliErrCode,FILE *fp);

int UnionTranslateSoftErrCodeIntoCliErrCode(int softErrCode,char *cliErrCode);

int UnionExistSoftErrCodeDef(char *name);

int UnionInsertSoftErrCode(int softErrCode,char *cliErrCode,char *remark);

// 20060825 增加
int UnionDeleteSoftErrCodeDef(int softErrCode);

// 20060825 增加
int UnionUpdateSoftErrCodeDef(int softErrCode,char *cliErrCode,char *remark);

// wangk add 2009-9-24
int UnionPrintSoftErrCodeInRecStrToFile(PUnionSoftErrCode pylqz_cliErrCode,FILE *fp);

int UnionPrintSoftErrCodeTBLInRecStrToFile(FILE *fp);

int UnionPrintSoftErrCodeTBLInRecStrToSpecFile(char *fileName);
// wangk add end 2009-9-24

#endif
