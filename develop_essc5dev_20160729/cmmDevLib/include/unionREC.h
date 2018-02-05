//	Wolfgang Wang
//	2003/10/10
//	Version 1.0

#ifndef _UnionREC_
#define _UnionREC_

// 2005/06/16 王纯军增加
int UnionUpdateRECVarAsString(char *varName,char *value,int len);

#define conMDLNameOfREC		"UnionRECMDL"

typedef enum
{
	conString,
	conChar,
	conInt,
	conLong,
	conShort,
	conDouble
} TUnionRECVarType;

typedef struct
{
	char			name[40+1];	// 变量名称
	TUnionRECVarType	type;		// 变量类型
	union
	{
		char		charValue;
		char		strValue[40+1];
		long		longValue;
		int		intValue;
		short		shortValue;
		double		doubleValue;
	} value;
} TUnionRECVar;
typedef TUnionRECVar		*PUnionRECVar;

typedef struct
{
	int			maxNum;		// 变量的最大数量
	int			realNum;	// 变量的实际数量
	PUnionRECVar		pvarTBL;	// 变量列表
} TUnionREC;
typedef TUnionREC		*PUnionREC;

// 20060808
int UnionMirrorREC();
PUnionRECVar UnionGetCurrentRECVarGrp();
int UnionGetCurrentRECVarNum();

int UnionGetConfFileNameOfREC(char *fileName);
int UnionConnectREC();
int UnionDisconnectREC();
int UnionRemoveREC();

int UnionReloadREC();
int UnionPrintRECVarToFile(PUnionRECVar pvar,FILE *fp);
int UnionPrintRECToFile(FILE *fp);
int UnionPrintRECToSpecFile(char *fileName);

int UnionGetMaxVarNumOfREC();
PUnionRECVar UnionReadRECVar(char *varName);
int UnionReadIntTypeRECVar(char *varName);
long UnionReadLongTypeRECVar(char *varName);
short UnionReadShortTypeRECVar(char *varName);
double UnionReadDoubleTypeRECVar(char *varName);
char UnionReadCharTypeRECVar(char *varName);
char *UnionReadStringTypeRECVar(char *varName);

int UnionUpdateRECVar(char *varName,unsigned char *value);
int UnionUpdateRECImageValue(char *varName,unsigned char *value);
int UnionIsValidRECVar(PUnionRECVar pvar);

// 2006/8/25
TUnionRECVarType UnionConvertTypeNameIntoRECVarType(char *nameOfType);

// 检查变量是否已在文件中定义了
int UnionExistRECVarInConfFile(char *name);

//2006/8/25 插入一个记录
int UnionInsertRECVar(char *name,char *type,char *value);

// 2006/8/25删除一个记录
int UnionDeleteRECVar(char *name);

// 2006/8/25
int UnionConvertRECVarTypeIntoTypeName(TUnionRECVarType type,char *nameOfType);

// wangk add 2009-9-24
int UnionPrintRECInRecStrFormatToSpecFile(char *fileName);

int UnionPrintRECInRecStrFormatToFile(FILE *fp);

int UnionPrintRECVarInRecStrToFile(PUnionRECVar pvar,FILE *fp);
// wangk add end 2009-9-24

#endif 			
