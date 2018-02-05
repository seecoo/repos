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

// ESSC�������Client�������Ƕ��һ��ϵ�������ESSC��������ܶ�Ӧͬһ��Client�����롣
// һ��ESSC������ֻ�ܶ�Ӧһ��Client�����롣

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
	int			defaultSoftErrCode;		// ��CliErrCodeδ�����Ӧ��softErrCodeʱ��ȡ��ֵ
#ifdef _2LenCliErrCode_
	char			defaultCliErrCode[2+1];		// ��ESSC����Ĵ�����û�ж�Ӧ��Client������ʱ��ȡ��ֵ
#endif
#ifdef _3LenCliErrCode_
	char			defaultCliErrCode[3+1];		// ��ESSC����Ĵ�����û�ж�Ӧ��Client������ʱ��ȡ��ֵ
#endif
#ifdef _4LenCliErrCode_
	char			defaultCliErrCode[4+1];		// ��ESSC����Ĵ�����û�ж�Ӧ��Client������ʱ��ȡ��ֵ
#endif
#ifdef _5LenCliErrCode_
	char			defaultCliErrCode[5+1];		// ��ESSC����Ĵ�����û�ж�Ӧ��Client������ʱ��ȡ��ֵ
#endif
#ifdef _6LenCliErrCode_
	char			defaultCliErrCode[6+1];		// ��ESSC����Ĵ�����û�ж�Ӧ��Client������ʱ��ȡ��ֵ
#endif
#ifdef _7LenCliErrCode_
	char			defaultCliErrCode[7+1];		// ��ESSC����Ĵ�����û�ж�Ӧ��Client������ʱ��ȡ��ֵ
#endif
#ifdef _8LenCliErrCode_
	char			defaultCliErrCode[8+1];		// ��ESSC����Ĵ�����û�ж�Ӧ��Client������ʱ��ȡ��ֵ
#endif
#ifdef _9LenCliErrCode_
	char			defaultCliErrCode[9+1];		// ��ESSC����Ĵ�����û�ж�Ӧ��Client������ʱ��ȡ��ֵ
#endif
#ifdef _10LenCliErrCode_
	char			defaultCliErrCode[10+1];		// ��ESSC����Ĵ�����û�ж�Ӧ��Client������ʱ��ȡ��ֵ
#endif
	TUnionSoftErrCode	rec[conMaxNumOfSoftErrCode];	// ���ֵ
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

// 20060825 ����
int UnionDeleteSoftErrCodeDef(int softErrCode);

// 20060825 ����
int UnionUpdateSoftErrCodeDef(int softErrCode,char *cliErrCode,char *remark);

// wangk add 2009-9-24
int UnionPrintSoftErrCodeInRecStrToFile(PUnionSoftErrCode pylqz_cliErrCode,FILE *fp);

int UnionPrintSoftErrCodeTBLInRecStrToFile(FILE *fp);

int UnionPrintSoftErrCodeTBLInRecStrToSpecFile(char *fileName);
// wangk add end 2009-9-24

#endif
