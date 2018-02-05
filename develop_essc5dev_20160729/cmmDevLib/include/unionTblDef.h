//	Wolfgang Wang
//	2003/10/10
//	Version 1.0

#ifndef _UnionTblDef_h_
#define _UnionTblDef_h_

#include <stdio.h>

#define conMDLNameOfTblDef	"UnionTblDefMDL"

typedef struct
{
	char				name[40+1];		// ������
	int				type;			// ������
} TUnionTblFldDef;
typedef TUnionTblFldDef			*PUnionTblFldDef;

typedef struct
{
	unsigned int			fldNum;			// �����е�����Ŀ
	char				fldNameGrp[8][40+1];	// �����е�������
} TUnionTblFldGrp;
typedef TUnionTblFldGrp			*PUnionTblFldGrp;

typedef struct
{
	char				tableName[40+1];	// ��������
	unsigned int			fldNum;			// ������Ŀ
	TUnionTblFldDef			fldDefGrp[64];		// ����
	TUnionTblFldGrp			primaryKey;		// �ؼ��ֶ���
	char				dbSelectFieldList[512];	// �ؼ��ֶ���
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
