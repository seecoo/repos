//	Wolfgang Wang, 2006/8/9

#ifndef _unionErrCodeTranslater_
#define _unionErrCodeTranslater_

#define conConfFileNameOfErrCodeTranslater	"errCodeTranslater.Def"
#define conMDLNameOfErrCodeTranslater		"errCodeTranslaterMDL"

#ifdef _maxErrCodeNum_1024_
#define conMaxNumOfErrCode	1024
#else
#define conMaxNumOfErrCode	512
#endif

typedef struct
{
	int	errCode;	// 错误码
#ifdef _REMARK_128_
	char	remark[128+1];	// 错误描述
#else
	char	remark[40+1];	// 错误描述
#endif
#ifdef _unionErrCodeTranslater_2_x_
	long	alaisCode;	// 错误码的另一个值
#endif
} TUnionErrCode;
typedef TUnionErrCode	*PUnionErrCode;
typedef struct
{
	int		realNum;
	TUnionErrCode	errCodeTBL[conMaxNumOfErrCode];
} TUnionErrCodeTranslater;
typedef TUnionErrCodeTranslater	*PUnionErrCodeTranslater;

PUnionErrCode UnionFindErrCode(int errCode);

// 2008/12/1，王纯军
long UnionFindAlaisCodeOfErrCode(int errCode);

int UnionGetNameOfErrCodeTranslater(char *fileName);

int UnionIsErrCodeTranslaterConnected();

int UnionGetMaxNumOfErrCode();

int UnionConnectErrCodeTranslater();

int UnionDisconnectErrCodeTranslater();

int UnionRemoveErrCodeTranslater();

int UnionReloadErrCodeTranslater();

int UnionPrintErrCodeTranslaterToFile(FILE *fp);
int UnionPrintErrCodeTranslaterToSpecFile(char *fileName);

int UnionPrintErrCodeToFile(PUnionErrCode errCodeRec,FILE *fp);

char *UnionFindRemarkOfErrCode(int ErrCode);

int UnionPrintRecOfErrCodeToFile(int ErrCode,FILE *fp);

int UnionPrintAllErrCodeOfRemarkToFile(char *remark,FILE *fp);

int UnionTranslateErrCodeIntoRemark(int ErrCode,char *remark,int sizeOfRemark);

// 20060825 增加
int UnionExistErrCodeDef(char *name);

int UnionInsertErrCode(int errCode,char *remark);

// 20060825 增加
int UnionDeleteErrCodeDef(int errCode);

// 20060825 增加
int UnionUpdateErrCodeDef(int errCode,char *remark);

// wangk add 2009-9-24
int UnionPrintErrCodeInRecStrToFile(PUnionErrCode errCodeRec,FILE *fp);

int UnionPrintErrCodeTranslaterInRecStrToFile(FILE *fp);

int UnionPrintErrCodeTranslaterInRecStrToSpecFile(char *fileName);
// wangk add end 2009-9-24

#endif
