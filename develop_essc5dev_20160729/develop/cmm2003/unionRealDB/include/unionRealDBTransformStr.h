#ifndef _realDBTransformStr_
#define _realDBTransformStr_

#include "unionComplexDBCommon.h"

typedef enum
{
	conTransformTypeInsert = 0,		// ����ֵ��
	conTransformTypeUpdate = 1,		// ����ֵ��
	conTransformTypeCondition = 2,		// ������
} TUnionTransformType;
typedef TUnionTransformType	*PUnionTransformType;

typedef struct
{
	char str[1024];	// SQL����ֳ������ַ���
	int iType;		// �ַ�������
					// 1���ؼ���"SELECT UPDATE DELETE AS SUM COUNT DISTINCT FROM WHERE ORDER GROUP BY HAVING AND OR"�� 
					// 2����ʶ�����������������Զ��������
					// 3����ֵ
					// 4����''��""���ַ���
					// 5������ָ����',','(',')','.'
					// 6: �������"=,+,-,*,/,<,>,<=,>=,<>,!="
} TUnionSQLAnalyseRes;
typedef TUnionSQLAnalyseRes   *PUnionSQLAnalyseRes;

/*
���ܣ���һ����¼��ת��ΪSQL���ַ���
���������
	recStr��	�����ַ���
	lenOfRecStr��	recStr�ĳ���
	type��		SQL������
	sizeOfBuf��	sqlStr�Ĵ洢��С
���������
	sqlStr��	����SQL��ֵ��
����ֵ��
	>=0��		�ɹ�
	<0��		ʧ�ܣ�������
*/
int UnionIsSQLString(char *recStr,int lenOfRecStr,char *sqlStr,int sizeOfBuf);

/*
���ܣ�
	��һ���������е������滻Ϊ���е�����
���������
	resName		����
	oriSQLStr	ԭ��
���������
	desSQLStr	Ŀ�괮
����ֵ��
	>=0��		Ŀ�괮�ĳ���
	<0��		ʧ�ܣ�������
*/
int UnionConvertFldDefNameInSQLStrIntoDBFldNameOfSpecObject(char *resName,char *oriSQLStr,char *desSQLStr);

/*
���ܣ���һ����¼��ת��Ϊ����SQL��ֵ��
���������
	recStr��	�����ַ���
	lenOfRecStr��	recStr�ĳ���
	sizeOfBuf��	sqlStr�Ĵ洢��С
���������
	sqlStr��	����SQL��ֵ��
����ֵ��
	>=0��		�ɹ�
	<0��		ʧ�ܣ�������
*/
int UnionTransformRecStrToSQLStr(char *tblName,char *recStr,int lenOfRecStr,TUnionTransformType type,char *sqlStr,int sizeOfBuf);

/*
���ܣ���һ����¼����ȡ������
���������
	recStr��	�����ַ���
	lenOfRecStr��	recStr�ĳ���
���������
	oper��		������
����ֵ��
	>=0��		�ɹ�
	<0��		ʧ�ܣ�������
*/
int UnionGetOperatorFromRecStr(char *recStr,int lenOfRecStr,char *oper);

/*
���ܣ�
��ʽ����ֵ
���������
pobject��	ָ��������ָ��
fldName		������
���������
formatFldVal	��ʽ�������ֵ
����ֵ��
>=0��	����
<0��	ʧ�ܣ����ش�����
*/
int UnionGetFormatFldValueOfSpecFldFromObjectDef(PUnionObject pobject,char *fldName,char *fldValue,char *formatFldVal);

/*
���ܣ�
��SQL������ʷ�����
���������
strSQL��	SQL���
iMaxSQLAnalyseResNum��	�ʷ��������ĵ���������
���������
tSQLAnalyseRes��	�ʷ��������
����ֵ��
>=0��	�ɹ����ʷ��������ĵ��ʸ���
<0��	ʧ�ܣ����ش�����
*/
int UnionAnalyzeSQL(char *strSQL, TUnionSQLAnalyseRes tSQLAnalyseRes[], int iMaxSQLAnalyseResNum);

/*
���ܣ�
��ǰ̨�����SQL��������ת��Ϊ�������ݿ�����
���������
strSQL��		SQL���
strTableName��	����
iSizeOfDestSQL�����ת�����SQL��仺�����Ĵ�С
���������
strDestSQL��	ת�����SQL���
����ֵ��
>=0��	�ɹ���ת����SQL���ĳ���
<0��	ʧ�ܣ����ش�����
*/
int UnionTransSQL(char *strSQL, char *strTableName, char *strDestSQL, int iSizeOfDestSQL);

int UnionTransRecStrFileToSpecFormatFile(char *oriFile, char *desFile, char speratorTag);

#endif
