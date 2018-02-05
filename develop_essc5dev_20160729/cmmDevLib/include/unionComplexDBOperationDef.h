// 2008/7/26
// Wolfang Wang

#ifndef _unionComplexDBOperationDef_
#define _unionComplexDBOperationDef_

/*
����	
	��һ������ڴ��ж�ȡһ�����ݿ��������
�������
	str		���崮
	lenOfStr	���崮�ĳ���
	sizeOfTblName	��������Ĵ�С
	sizeOfCondition	������������Ĵ�С
�������
	tblName		����
	operationID	������ʶ
	condition	��������
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadComplexDBOperationDefFromStr(char *str,int lenOfStr,char *tblName,int sizeOfTblName,int *operationID,char *operationCondition,int sizeOfCondition);

/*
����	
	��һ������ڴ��ж�ȡһ�����ݿ�ѡ���������
�������
	str		���崮
	lenOfStr	���崮�ĳ���
	sizeOfTblName	��������Ĵ�С
	sizeOfOperationDef	������������Ĵ�С
	sizeOfFldList	���嵥����Ĵ�С
�������
	tblName		����
	operationID	������ʶ
	condition	��������
	fldList		Ҫѡ������嵥
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadComplexDBSelectOperationDefFromStr(char *str,int lenOfStr,char *tblName,int sizeOfTblName,int *operationID,char *condition,int sizeOfOperationDef,char *fldList,int sizeOfFldList);

#endif

