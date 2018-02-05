// Wolfgang Wang
// 2008/11/03

#ifndef _unionDataImageInMemory_
#define _unionDataImageInMemory_

#define conResNameOfDataImageInMemory	"unionDataImageInMemory"

// �ж��Ƿ�һ�������ӳ����ص�ӳ����
/*
�������
	objectName	����		
	key		�ؼ���
�������
	��
����ֵ
	1		����
	0		������
*/
int UnionIsUseDataImageInMemory(char *objectName,char *key);

// ɾ����������ӳ��
/*
�������
	��
�������
	��
����ֵ
	�ɹ�		>=0��ɾ��������ӳ�������
	ʧ��		<0,������
*/
int UnionDeleteAllDataImageInMemory();

// �ж�һ������ӳ���Ƿ����
/*
�������
	key		�ؼ���
�������
	��
����ֵ
	����		1
	������		0
	����		��ֵ
*/
int UnionExistDataImageInMemory(char *key);

// ����һ������ӳ��
/*
�������
	key		�ؼ���
	data		����
	lenOfData	���ݳ���
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionInsertDataImageInMemory(char *key,unsigned char *data,int lenOfData);

// �޸�һ������ӳ��
/*
�������
	key		�ؼ���
	data		����
	lenOfData	���ݳ���
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionModifyDataImageInMemory(char *key,unsigned char *data,int lenOfData);

// ��һ������ӳ��
/*
�������
	key		�ؼ���
	sizeOfBuf	���ݻ���Ĵ�С
�������
	data		����
����ֵ
	�ɹ�		>=0�����������ݳ���
	ʧ��		<0,������
*/
int UnionReadDataImageInMemory(char *key,unsigned char *data,int sizeOfBuf);

// ɾ��һ������ӳ��
/*
�������
	key		�ؼ���
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionDeleteDataImageInMemory(char *key);

// �ж�һ����¼����ӳ���Ƿ����
/*
�������
	objectName	����		
	key		�ؼ���
�������
	��
����ֵ
	����		1
	������		0
	����		��ֵ
*/
int UnionExistRecDataImageInMemory(char *objectName,char *key);

// ����һ����¼������ӳ��
/*
�������
	objectName	����		
	key		�ؼ���
	data		����
	lenOfData	���ݳ���
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionInsertRecDataImageInMemory(char *objectName,char *key,unsigned char *data,int lenOfData);

// �޸�һ����¼������ӳ��
/*
�������
	objectName	����		
	key		�ؼ���
	data		����
	lenOfData	���ݳ���
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionModifyRecDataImageInMemory(char *objectName,char *key,unsigned char *data,int lenOfData);

// ��һ����¼������ӳ��
/*
�������
	key		�ؼ���
	sizeOfBuf	���ݻ���Ĵ�С
�������
	data		����
����ֵ
	�ɹ�		>=0�����������ݳ���
	ʧ��		<0,������
*/
int UnionReadRecDataImageInMemory(char *objectName,char *key,unsigned char *data,int sizeOfBuf);

// ɾ��һ����¼������ӳ��
/*
�������
	objectName	����		
	key		�ؼ���
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionDeleteRecDataImageInMemory(char *objectName,char *key);

// ��ʾ��������ӳ��
/*
�������
	��
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionPrintAllDataImageInMemoryToFile(FILE *fp);

	
// ��ʾ��������
/*
�������
	resID		��Դ��
�������
	��
����ֵ
	>=0		������Ŀ
	<0		������
*/
int UnionSpierAllDataImageInMemory(int resID);

#endif
