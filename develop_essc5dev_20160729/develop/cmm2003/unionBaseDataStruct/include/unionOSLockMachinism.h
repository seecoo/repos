// Wolfgang Wang
// 2008/11/03

#ifndef _unionOsLockMachinism_
#define _unionOsLockMachinism_

// ��ϵͳ�������
/*
�������
	��
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionOpenOSLockMachinism();

// �ر�ϵͳ�������
/*
�������
	��
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionCloseOSLockMachinism();

// ����һ��ϵͳ����
/*
�������
	��
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionApplyOsLock();

// ����ϵͳ����
/*
�������
	��
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionFreeOsLock();

#endif
