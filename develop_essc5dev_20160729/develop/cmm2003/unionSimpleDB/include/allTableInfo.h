// Author:	ChenJiaMei
// Date:	2008-7-16

#ifndef _allTableInfo_
#define _allTableInfo_

// ����ļ�����
int UnionGetAllTableInfoFileName(char *fileName);

/*
���ܣ�����һ���µı���Ϣ�ļ����ļ���ֻ��һ��ֵ"0"
���������
	databaseName�����ݿ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ��
*/
int UnionCreateAllTableInfo();

/*
���ܣ��ڱ���Ϣ�ļ�������һ�ű�����ƣ�ͬʱ�޸ı������
���������
	databaseName��Ҫ���ӵı���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ��
*/
int UnionAddToAllTableInfo(char *tableName);
/*
���ܣ��ӱ���Ϣ�ļ���ɾ��һ�ű�����ƣ�ͬʱ�޸ı������
���������
	databaseName��Ҫɾ���ı���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ��
*/
int UnionDeleteFromAllTableInfo(char *tableName);

/*
���ܣ��ӱ���Ϣ�ļ���ȡһ�����ݿ��б������
���������
	��
���������
	��
����ֵ��
	>=0���ɹ������ر������
	<0��ʧ��
*/
int UnionGetAllTableNumber();

/*
�����еı���Ϣд�뵽�ļ���
*/
int UnionPrintAllTableNameToFile(char *fileName);

#endif
