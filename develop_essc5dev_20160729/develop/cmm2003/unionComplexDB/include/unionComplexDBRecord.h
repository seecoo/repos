//	Author: ChenJiaMei
//	Date: 2008-8-8

#ifndef _record_
#define _record_

#include "unionComplexDBObjectDef.h"
/*
���ܣ�����һ�������ʵ��
���������
	idOfObject������ID
	record������ʵ����ֵ����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
	lenOfRecord��record�ĳ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionInsertObjectRecord(TUnionIDOfObject idOfObject,char *record,int lenOfRecord);

/*
���ܣ����ݹؼ���ɾ��һ�������ʵ��
���������
	idOfObject������ID
	primaryKey������ʵ���Ĺؼ��֣���ʽΪ"��ֵ��1=��ֵ|��ֵ��2=��ֵ|����ֵ��N=��ֵ|"
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionDeleteUniqueObjectRecord(TUnionIDOfObject idOfObject,char *primaryKey);

/*
���ܣ��޸�һ�������ʵ��
���������
	idOfObject������ID
	primaryKey������ʵ���Ĺؼ��֣���ʽΪ"��ֵ��1=��ֵ|��ֵ��2=��ֵ|����ֵ��N=��ֵ|"
	record��Ҫ�޸ĵ�ʵ����ֵ����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"��ֻ����Ҫ�޸ĵ���
	lenOfRecord��record�ĳ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionUpdateUniqueObjectRecord(TUnionIDOfObject idOfObject,char *primaryKey,char *record,int lenOfRecord);

/*
���ܣ����ݹؼ��ֲ�ѯһ�������ʵ��
���������
	idOfObject������ID
	primaryKey������ʵ���Ĺؼ��֣���ʽΪ"��ֵ��1=��ֵ|��ֵ��2=��ֵ|����ֵ��N=��ֵ|"
	sizeOfRecord�����ܲ�ѯ��¼��record�Ĵ�С
���������
	record�����ҳ����ļ�¼����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
����ֵ��
	>=0���ɹ������ؼ�¼�ĳ���
	<0��ʧ�ܣ�������
*/
int UnionSelectUniqueObjectRecordByPrimaryKey(TUnionIDOfObject idOfObject,char *primaryKey,char *record,int sizeOfRecord);

/*
���ܣ����ݹؼ��ֲ�ѯһ�������ʵ����ֻѡ��ָ������
���������
	idOfObject������ID
	primaryKey������ʵ���Ĺؼ��֣���ʽΪ"��ֵ��1=��ֵ|��ֵ��2=��ֵ|����ֵ��N=��ֵ|"
	sizeOfRecord�����ܲ�ѯ��¼��record�Ĵ�С
	fldGrp		Ҫѡ�������嵥��ÿ����֮����,�ָ���
	lenOfFldGrp	���嵥�ĳ���
���������
	record�����ҳ����ļ�¼����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
����ֵ��
	>=0���ɹ������ؼ�¼�ĳ���
	<0��ʧ�ܣ�������
*/
int UnionSelectObjectFldGrpByPrimaryKey(TUnionIDOfObject idOfObject,char *primaryKey,char *fldGrp,int lenOfFldGrp,char *record,int sizeOfRecord);

/*
���ܣ�����Ψһֵ��ѯһ�������ʵ��
���������
	idOfObject������ID
	uniqueKey������ʵ����Ψһֵ����ʽΪ"Ψһֵ��1=��ֵ|Ψһֵ��2=��ֵ|��Ψһֵ��N=��ֵ|"
	sizeOfRecord�����ܲ�ѯ��¼��record�Ĵ�С
���������
	record�����ҳ����ļ�¼����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
����ֵ��
	>=0���ɹ������ؼ�¼�ĳ���
	<0��ʧ�ܣ�������
*/
int UnionSelectUniqueObjectRecordByUniqueKey(TUnionIDOfObject idOfObject,char *uniqueKey,char *record,int sizeOfRecord);

/*
���ܣ�����ɾ��һ�������ʵ��
���������
	idOfObject������ID
	condition��ɾ����������ʽΪ"��1=��ֵ|��2=��ֵ|��3=��ֵ|����N=��ֵ|"
���������
	��
����ֵ��
	>0���ɹ������ر�ɾ����ʵ����Ŀ
	<0��ʧ�ܣ�������
*/
int UnionBatchDeleteObjectRecord(TUnionIDOfObject idOfObject,char *condition);

/*
���ܣ������޸�һ�������ʵ��
���������
	idOfObject������ID
	condition���޸���������ʽΪ"��1=��ֵ|��2=��ֵ|��3=��ֵ|����N=��ֵ|"
	record��Ҫ�޸ĵ�ʵ����ֵ����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ|"��ֻ����Ҫ�޸ĵ���
	lenOfRecord��record�ĳ���
���������
	��
����ֵ��
	>=0���ɹ������ر��޸ĵ�ʵ����Ŀ
	<0��ʧ�ܣ�������
*/
int UnionBatchUpdateObjectRecord(TUnionIDOfObject idOfObject,char *condition,char *record,int lenOfRecord);

/*
���ܣ�������ѯһ�������ʵ��
���������
	idOfObject������ID
	condition����ѯ��������ʽΪ"��1=��ֵ|��2=��ֵ|��3=��ֵ|����N=��ֵ|"
���������
	fileName���洢�˲�ѯ������ʵ����ֵ���ļ���
����ֵ��
	>=0���ɹ������ز�ѯ���ļ�¼����Ŀ
	<0��ʧ�ܣ�������
*/
int UnionBatchSelectObjectRecord(TUnionIDOfObject idOfObject,char *condition,char *fileName);

/*
���ܣ�ƴװһ���ؼ���������
���������
	idOfObject��	Ҫ���Ķ���ID	
	primaryKeyValueList	�ؼ�ֵ�ִ�,������ֵ��.�ֿ�
	lenOfValueList		�ؼ�ֵ�ִ�����
	sizeOfBuf	�ؼ����嵥�Ĵ�С
���������
	recStr		�ؼ���������
����ֵ��
	0��	�ؼ����������ĳ���
	<0��	ʧ�ܣ����ش�����
*/
int UnionFormObjectPrimarKeyCondition(TUnionIDOfObject idOfObject,char *primaryKeyValueList,int lenOfValueList,char *recStr,int sizeOfBuf);

/*
���ܣ�
	��һ����¼��ƴװ�ؼ���
���������
	idOfObject��		����ID
	record��		����ʵ����ֵ����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
	lenOfRecord��		record�ĳ���
	sizeOfPrimaryKey	�ؼ�ֵ�������С
���������
	primaryKey		�ؼ�ֵ������.�ֿ�
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionFormPrimaryKeyStrOfSpecObjectRec(TUnionIDOfObject idOfObject,char *record,int lenOfRecord,char *primaryKey,int sizeOfPrimaryKey);

#endif
