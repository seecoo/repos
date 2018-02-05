//	Wolfgang Wang
//	2009/4/29

#ifndef _unionTransferComplexDBDef_
#define unionTransferComplexDBDef

#include "unionComplexDBCommon.h"
#include "unionComplexDBObjectDef.h"
#include "unionTableField.h"
#include "unionTableList.h"

/*
����	
	�ж�һ�������Ƿ����
�������
	pfldRec	tableField�ļ�¼����
�������
	��
����ֵ
	errCodeNameIsUsed	ͬ���Ĵ��ڣ������岻ͬ
	1	����
	����	������
*/
int UnionExistSpecTableFieldRec(PUnionTableField pfldRec);

/*
����	
	���һ��������ı�ʶ
�������
	tableName	����
	fldName		������
�������
	fldID		���ʶ
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionFormTableFieldRecFldID(char *tableName,char *fldName,char *fldID);

/*
����	
	��һ��complexDB�������ת��ΪtableField�ļ�¼����
�������
	tableName	����
	pfldDef		complexDB�������
�������
	pfldRec		tableField�ļ�¼����
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionTransferComplexDBFldDefToTableFieldRec(char *tableName,PUnionObjectFldDef pfldDef,PUnionTableField pfldRec);

/*
����	
	��һ��fldGrp����д�뵽�嵥����
�������
	tableName	����
	pfldGrpDef	����
�������
	fldListStr	���嵥��
����ֵ
	>=0	�ɹ�,������
	<0	������
*/
int UnionPutObjectFldGrpDefIntoStr(char *tableName,PUnionObjectFldGrp pfldGrpDef,char *fldListStr);

/*
����	
	������Դ���ƣ�����complexDB�������ԴID��
�������
	resName		��Դ����
�������
	��
����ֵ
	>=0	��ԴID
	<0	������
*/
int UnionReadResIDOfSpecResNameForComplexDB(char *resName);

/*
����	
	��һ��complexDB����Ķ���ת��ΪtableList�ļ�¼����
�������
	
	pobjDef		complexDB����Ķ���
�������
	ptblRec		tableList�ļ�¼����
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionTransferComplexDBObjectDefToTableListRec(PUnionObject pobjDef,PUnionTableList ptblRec);

/*
����	
	��һ��complexDB����Ķ�����뵽tabelList��
�������
	tableName	������
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionInsertComplexDBObjectDefIntoTableList(char *tableName);

#endif
