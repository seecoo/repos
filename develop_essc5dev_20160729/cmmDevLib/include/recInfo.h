// Author:	ChenJiaMei
// Date:	2008-7-16

#ifndef _recInfo_
#define _recInfo_

// ��ü�¼��Ϣ�ļ�
int UnionGetFileNameOfRecInfo(char *tableName,char *fileName);

/*
���ܣ�����һ����ļ�¼��Ϣ�ļ����ļ���ֻ�����У���һ��Ϊ��0������2��Ϊ��Ĺؼ���
���������
	tableName����Ӧ�ı���
	keyStr����Ĺؼ�������������ؼ����ɶ������ɣ��������֮����'|'�ָ�
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ��
*/
int UnionCreateRecInfo(char *tableName,char *keyStr);

/*
���ܣ��ڼ�¼��Ϣ�ļ�������һ����¼�ļ�¼�ļ����ƣ�ͬʱ�޸ļ�¼����
���������
	tableName����Ӧ�ı���
	recFileName��Ҫ���ӵļ�¼�ļ�����
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ��
*/
int UnionAddToRecInfo(char *tableName,char *recFileName);

/*
���ܣ��Ӽ�¼��Ϣ�ļ���ɾ��һ����¼�ļ�¼�ļ����ƣ�ͬʱ�޸ļ�¼����
���������
	tableName����Ӧ�ı���
	recFileName��Ҫɾ���ļ�¼�ļ�����
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ��
*/
int UnionDeleteFromRecInfo(char *tableName,char *recFileName);

/*
���ܣ��Ӽ�¼��Ϣ�ļ���ȡһ�ű�ļ�¼����
���������
	tableName����Ӧ�ı���
���������
	��
����ֵ��
	>=0���ɹ������ر�ļ�¼����
	<0��ʧ��
*/
int UnionGetTableRecordNumber(char *tableName);

/*
���ܣ��Ӽ�¼��Ϣ�ļ���ȡһ�ű�ļ�¼�����͹ؼ���
���������
	tableName����Ӧ�ı���
	sizeOfPrimaryKey	�ؼ��ֻ���Ĵ�С
���������
	primaryKey      �ؼ���
����ֵ��
	>=0���ɹ������ر�ļ�¼����
	<0��ʧ��
*/
int UnionGetTableRecInfo(char *tableName,char *primaryKey,int sizeOfPrimaryKey);

/*
���ܣ��Ӽ�¼��Ϣ�ļ���ȡһ�ű�ļ�¼�����͹ؼ���
���������
	tableName����Ӧ�ı���
���������
	primaryKey      �ؼ���
����ֵ��
	>=0��		�ؼ��ֵĳ���
	<0��ʧ��
*/
int UnionGetPrimaryKeyFromTableRecInfo(char *tableName,char *primaryKey,int sizeOfPrimaryKey);

#endif
