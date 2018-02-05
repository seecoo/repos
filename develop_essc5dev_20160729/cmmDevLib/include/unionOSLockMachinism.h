// Wolfgang Wang
// 2008/11/03

#ifndef _unionOsLockMachinism_
#define _unionOsLockMachinism_

#define  SEM_KEY  100168

#ifndef _WIN32_
typedef union
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
} UUnionSemun;
typedef UUnionSemun  *PUnionSemnu;
#endif

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
	resName	��Դ����
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionApplyOsLock(char *resName);

// ����ϵͳ����
/*
�������
	resName	��Դ����
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionFreeOsLock(char *resName);

#endif
