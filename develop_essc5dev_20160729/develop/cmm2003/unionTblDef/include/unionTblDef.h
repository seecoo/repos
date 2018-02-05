//	Wolfgang Wang
//	2003/10/10
//	Version 1.0

#ifndef _UnionTblDef_h_
#define _UnionTblDef_h_

#include <stdio.h>

#define conMDLNameOfTblDef	"UnionTblDefMDL"

typedef struct
{
	char				name[40+1];		// 域名称
	int				type;			// 域类型
} TUnionTblFldDef;
typedef TUnionTblFldDef			*PUnionTblFldDef;

typedef struct
{
	unsigned int			fldNum;			// 域组中的域数目
	char				fldNameGrp[8][40+1];	// 域组中的域名称
} TUnionTblFldGrp;
typedef TUnionTblFldGrp			*PUnionTblFldGrp;

typedef struct
{
	char				tableName[40+1];	// 对象名称
	unsigned int			fldNum;			// 域定义数目
	TUnionTblFldDef			fldDefGrp[64];		// 域定义
	TUnionTblFldGrp			primaryKey;		// 关键字定义
	char				dbSelectFieldList[512];	// 关键字定义
} TUnionTblDefRec;
typedef TUnionTblDefRec		*PUnionTblDefRec;

typedef struct
{
	PUnionTblDefRec			pTBL;
	unsigned int			maxNum;
	unsigned int			realNum;
} TUnionTblDef;
typedef TUnionTblDef		*PUnionTblDef;

int UnionGetCurrentTblDefNum();
PUnionTblDef UnionGetCurrentTblDefGrp();
int UnionIsTblDefConnected();
int UnionConnectTblDef();
int UnionDisconnectTblDef();
int UnionRemoveTblDef();
int UnionPrintTblDefRecToFile(PUnionTblDefRec prec, FILE *fp);
int UnionPrintTblDefToFile(FILE *fp);
PUnionTblDefRec UnionReadTblDefRec(char *tableName);
int UnionInsertTblDefRec(PUnionTblDefRec prec);
char *UnionGetTableSelectFieldFromImage(char *tableName);
int UnionReadTablePrimaryKeyFromImage(char *tableName, char *primaryKeyList);
int UnionPrintDetailTblDefToFile(FILE *fp, char *tableName);

#endif 			
