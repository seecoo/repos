// Author:	ChenJiaMei
// Date:	2008-7-16

#ifndef _table_
#define _table_

int UnionGetDirOfTable(char *tableName,char *dir);

int UnionGetDirOfTable(char *tableName,char *dir);

/*
���ܣ�����һ�ű�
���������
	tableName������
	primaryKeyStr	��Ĺؼ�������������ؼ����ɶ������ɣ����������֮����'|'�ָ�
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ��
*/
int UnionCreateTable(char *tableName,char *primaryKeyStr);

/*
���ܣ�ɾ��һ�ű�
���������
	tableName������
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ��
*/
int UnionDropTable(char *tableName);

/*
���ܣ��ж�һ�ű��Ƿ����
���������
	tableName������
���������
	��
����ֵ��
	>0�������
	0��������
	<0��ʧ��
*/
int UnionExistTable(char *tableName);

#endif
