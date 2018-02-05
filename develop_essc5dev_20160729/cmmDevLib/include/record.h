// Author:	ChenJiaMei
// Date:	2008-7-16

#ifndef _record_
#define _record_

/*
���ܣ�	��һ���ؼ��ֶ��崮�ж��������ֶ�
���������
	defStr		�ؼ��ֶ��崮
	lenOfDefStr	�ؼ��ֶ��崮����
        maxFldNum       �ؼ�������Ĵ�С
���������
	primaryKeyGrp	�ؼ�������
����ֵ��
	>=0��		�ؼ��ְ���������Ŀ
	<0��		�������
*/
int UnionReadPrimaryKeyFldGrpFromDefStr(char *defStr,int lenOfDefStr,char primaryKeyGrp[][128+1],int maxFldNum);

// ���ݱ�񣬼�¼��������ü�¼�ļ�������
int UnionGetRecordFileNameFromCondition(char *tableName,char *condition,int lenOfCondition,char *fileName,char *keyStr,int sizeOfKeyStr);

/*
���ܣ�	�Ӽ�¼��Ϣ�ļ��ж����ؼ��ָ��ֶ�
���������
	tableName	����
        maxFldNum       �ؼ�������Ĵ�С
���������
	primaryKeyGrp	�ؼ�������
����ֵ��
	>=0��		�ؼ��ְ���������Ŀ
	<0��		�������
*/
int UnionReadPrimaryKeyFldGrpFromRecInfoFile(char *tableName,char primaryKeyGrp[][128+1],int maxFldNum);

// ���ݱ�񣬼�¼������ü�¼�ļ�������
int UnionGetRecordFileName(char *tableName,char *recNullFileName,char *fileName);

// �ж��Ƿ���ڼ�¼�ļ�
int UnionExistsRecordFile(char *recordFileName);

// ���ݼ�¼�ͼ�ֵ��ɼ�¼���ļ�����
int UnionFormRecordFileName(char *tableName,char *record,int lenOfRecord,char *fileName,int sizeOfFileName);
/*
���ܣ��ڱ��в���һ����¼
���������
	tableName������
	record��Ҫ����ļ�¼����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
	lenOfRecord����¼�ĳ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ��
*/
int UnionInsertRecord(char *tableName,char *record,int lenOfRecord);

/*
���ܣ�ɾ�����е�һ����¼
���������
	tableName������
	key��Ҫɾ����¼�ļ�ֵ����ʽΪ"�ؼ�����1=��ֵ|�ؼ�����2=��ֵ|��|�ؼ�����N=��ֵ"
	lenOfKey����ֵ�ĳ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ��
*/
int UnionDeleteRecord(char *tableName,char *key,int lenOfKey);

/*
���ܣ��޸ı��е�һ����¼���ؼ��ֲ����޸�
���������
	tableName������
	record��Ҫ�޸ĵļ�¼����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"��
		������ؼ������ҹؼ��������޸ģ����������ֻ����Ҫ�޸ĵ���
	lenOfRecord����¼�ĳ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ��
*/
int UnionUpdateRecord(char *tableName,char *record,int lenOfRecord);

/*
���ܣ����ݹؼ��ֲ�ѯһ����¼
���������
	tableName������
	key����¼�ؼ��֣���ʽΪ"�ؼ�����1=��ֵ|�ؼ�����2=��ֵ|��|�ؼ�����N=��ֵ"
	lenOfKey���ؼ��ֵĳ���
        sizeOfRecord    ��¼����Ĵ�С
���������
	record�����ҳ��ļ�¼
����ֵ��
	>=0���ɹ������ؼ�¼�ĳ���
	<0��ʧ��
*/
int UnionSelectRecord(char *tableName,char *key,int lenOfKey,char *record,int sizeOfRecord);

/*
���ܣ������м�¼д�뵽�ļ���
���������
	tableName������
	fileName	д�뵽���ļ�����
���������
	��
����ֵ��
	>=0��		��¼��
	<0��		�������
*/
int UnionPrintAllRecordToFile(char *tableName,char *condition,char *fileName);

#endif
