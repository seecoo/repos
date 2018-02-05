// Author:	ChenJiaMei
// Date:	2008-7-16

#ifndef _database_
#define _database_

// ��õ�ǰ���ݿ������
char *UnionGetCurrentDatabaseName();

// ������ݿ��Ŀ¼
int UnionGetDatabaseDir(char *databaseName,char *dir);

/*
���ܣ�����һ�����ݿ�
���������
	databaseName�����ݿ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ��
*/
int UnionCreateDatabase(char *databaseName);

/*
���ܣ�ɾ��һ�����ݿ�
���������
	databaseName�����ݿ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ��
*/
int UnionDropDatabase(char *databaseName);

/*
���ܣ��ж�һ�����ݿ��Ƿ����
���������
	databaseName�����ݿ���
���������
	��
����ֵ��
	>0�����ݿ����
	0�����ݿⲻ����
	<0��ʧ��
*/
int UnionExistDatabase(char *databaseName);

/*
���ܣ����������ݿ������
���������
	databaseName�����ݿ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ��
*/
int UnionConnectDatabase(char *databaseName);
/*
���ܣ��Ͽ������ݿ������
���������
	��
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ��
*/
int UnionDisconnectDatabase();

#endif
