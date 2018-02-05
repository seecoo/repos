// Wolfgang Wang
// 2008/11/03

#ifndef _unionLockMachinism_
#define _unionLockMachinism_

// �򿪹ؼ���������
/*
�������
	��
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionOpenKeyWordLockMachinism();

// �رչؼ���������
/*
�������
	��
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionCloseKeyWordLockMachinism();

// ��һ���ؼ��ּ���
/*
�������
	key		�ؼ���
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionLockKeyWord(char *key);

// ��һ���ؼ��ֽ���
/*
�������
	key		�ؼ���
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionUnlockKeyWord(char *key);

// �����йؼ��ֽ���
/*
�������
	��
�������
	��
����ֵ
	�ɹ�		>=0������������
	ʧ��		<0,������
*/
int UnionUnlockAllKeyWord();

// ��ʾ������ס�˵Ĺؼ���
/*
�������
	��
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionPrintAllLockedKeyWordToFile(FILE *fp);


#endif
