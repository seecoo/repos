//	Wolfgang Wang
//	2004/6/17

#ifndef _UnionLisence_
#define _UnionLisence_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define conMDLNameOfUnionLisence	"UnionLisenceMDL"

typedef struct
{
	// ����Ϊ�̶�����
	char	userName[40+1];		// ��Ʒ���
	char	serialNumber[16+1];	// ��Ʒ�����к�
	char	lisenceData[16+1];	// ��Ʒ����֤��
	long	effectiveDays;		// ��Ʒ��Чʹ������
	long	lisenceTime;		// �䲼��Ʒ�����ʹ������
	// ����Ϊ��̬����
	time_t	registerTime;		// ��Ʒ�ǼǵĿ�ʼʹ������
	char	lisenceCode[16+1];	// ��Ʒ����֤�룬��������������
} TUnionLisence;
typedef TUnionLisence	*PUnionLisence;

int UnionGetLisenceKey(char *key);
int UnionGenerateLisenceCode(PUnionLisence plisence,char *lisenceCode);
int UnionCreateLisenceFile();
int UnionVerifyLisenceCode();
int UnionPrintLisence();

int UnionGetFixedLisence(PUnionLisence plisence);

int UnionConnectLisenceModule();
int UnionDisconnectLisenceModule();
int UnionRemoveLisenceModule();
int UnionReloadLisenceModule();
int UnionVerifyLisenceCode();


// ��ð汾�Ǽ��ļ�������
int UnionGetCopyrightRegistryFileName(char *fileName);

/*
����
	�����������ݺ����к�������Ȩ��
�������
	inputData	�������ݣ����Ϊ��ָ�룬��ȡunionREC.CFG�е�nameOfMyself��ֵ
	serialNumber	���кţ����Ϊ��ָ�룬��ȡunionREC.CFG�е����кŵ�ֵ
	isVerify	�ǲ�����֤
�������
	key		���ɵ���Ȩ��
����ֵ
	>=0		�ɹ������ɵ���Ȩ�볤��
	<0		�������
*/
int UnionGenerateLisenceKey(char *inputData,char *serialNumber,char *key,int isVerify);

/*
����
	�����������ݺ����к�������Ȩ��
�������
	inputData	�������ݣ����Ϊ��ָ�룬��ȡunionREC.CFG�е�nameOfMyself��ֵ
	conSerialNumberStr	���кţ����Ϊ��ָ�룬��ȡunionREC.CFG�е����кŵ�ֵ
	isVerify	�ǲ�����֤
�������
	key		���ɵ���Ȩ��
����ֵ
	>=0		�ɹ������ɵ���Ȩ�볤��
	<0		�������
*/
int UnionGenerateLisenceKeyUseSpecInputData(char *inputData,char *serialNumber,char *key,int isVerify);
/*
����
	�����������ݺ����к��Լ���Ȩ��У����Ȩ��
�������
	inputData	�������ݣ����Ϊ��ָ�룬��ȡunionREC.CFG�е�nameOfMyself��ֵ
	conSerialNumberStr	���кţ����Ϊ��ָ�룬��ȡunionREC.CFG�е����кŵ�ֵ
	key		Ҫ��У����Ȩ�룬���Ϊ��ָ�룬��ȡunionREC.CFG�е�liscenceCode
�������
	��
����ֵ
	>0		У��ɹ�
	=0		У��ʧ��
	<0		�������
*/
int UnionVerifyLisenceKeyUseSpecInputData(char *inputData,char *serialNumber,char *key);

/*
����
	�����Ʒ���ݺ����к��Լ���Ȩ��
�������
	inputData	��������
	conSerialNumberStr	���к�
	key		Ҫ��У����Ȩ��
�������
	��
����ֵ
	>0		У��ɹ�
	=0		У��ʧ��
	<0		�������
*/
int UnionInputLisenceKeyUseSpecInputData(char *inputData,char *serialNumber,char *key);

/*
����
	�洢��Ʒ���ݺ����к��Լ���Ȩ��
�������
	inputData	��������
	serialNumber	���к�
	key		Ҫ��У����Ȩ��
�������
	��
����ֵ
	>0		У��ɹ�
	=0		У��ʧ��
	<0		�������
*/
int UnionStoreLisenceInfo(char *inputData,char *serialNumber,char *key);

/*
����
	��ȡ��Ʒ���ݺ����к��Լ���Ȩ��
�������
	��
�������
	inputData	��������
	serialNumber	���к�
	key		Ҫ��У����Ȩ��
����ֵ
	>0		У��ɹ�
	=0		У��ʧ��
	<0		�������
*/
int UnionReadLiscenceInfo(char *inputData,char *serialNumber,char *key);

/*
����
	У���Ʒ��Ȩ��Ϣ
�������
	��
�������
	��
����ֵ
	>0		У��ɹ�
	=0		У��ʧ��
	<0		�������
*/
int UnionVerifyLisenceInfo();

/*
����
	�����������ݺ����к�����ǰ16λ��Ȩ��
�������
	inputData	�������ݣ����Ϊ��ָ�룬��ȡunionREC.CFG�е�nameOfMyself��ֵ
	serialNumber	���кţ����Ϊ��ָ�룬��ȡunionREC.CFG�е����кŵ�ֵ
	isVerify	�ǲ�����֤
�������
	key		���ɵ���Ȩ��
����ֵ
	>=0		�ɹ������ɵ���Ȩ�볤��
	<0		�������
*/
int UnionGenerateFrontLisenceKey(char *inputData,char *serialNumber,char *key,int isVerify);

/*
����
	�����������ݺ����к����ɺ�16λ��Ȩ��
�������
	date1	ά����������
	date2	ֹͣʹ������
	isVerify	�ǲ�����֤
�������
	key		���ɵ���Ȩ��
����ֵ
	>=0		�ɹ������ɵ���Ȩ�볤��
	<0		�������
*/
int UnionGenerateAfterLisenceKey(char *date1,char *date2,char *key,int isVerify);

/*
����
	�����������ݺ����к�����������Ȩ��
�������
	inputData	�������ݣ����Ϊ��ָ�룬��ȡunionREC.CFG�е�nameOfMyself��ֵ
	serialNumber	���кţ����Ϊ��ָ�룬��ȡunionREC.CFG�е����кŵ�ֵ
	date1		ά����������
	date2		ʹ�õ���ʱ��
	isVerify	�ǲ�����֤
�������
	key		���ɵ���Ȩ��
����ֵ
	>=0		�ɹ������ɵ���Ȩ�볤��
	<0		�������
*/
int UnionGenerateFinalLisenceKey(char *inputData,char *serialNumber,char *date1,char *date2,char *key,int isVerify);

/*
����
	�����������ݺ����к�����������Ȩ��
�������
	inputData	�������ݣ����Ϊ��ָ�룬��ȡunionREC.CFG�е�nameOfMyself��ֵ
	serialNumber	���кţ����Ϊ��ָ�룬��ȡunionREC.CFG�е����кŵ�ֵ
	date1		ά���������ڣ����Ϊ��ָ�룬��ȡ00000000��ֵ
	date2		ʹ�õ������ڣ����Ϊ��ָ�룬��ȡ00000000��ֵ
	isVerify	�ǲ�����֤
�������
	key		���ɵ���Ȩ��
����ֵ
	>=0		�ɹ������ɵ���Ȩ�볤��
	<0		�������
*/
int UnionGenerateFinalLisenceKeyUseSpecInputData(char *inputData,char *serialNumber,char *date1,char *date2,char *key,int isVerify);

/*
����
	�����������ݺ����к�����ǰ16��Ȩ��
�������
	inputData	�������ݣ����Ϊ��ָ�룬��ȡunionREC.CFG�е�nameOfMyself��ֵ
	serialNumber	���кţ����Ϊ��ָ�룬��ȡunionREC.CFG�е����кŵ�ֵ
	isVerify	�ǲ�����֤
�������
	key		���ɵ���Ȩ��
����ֵ
	>=0		�ɹ������ɵ���Ȩ�볤��
	<0		�������
*/
int UnionGenerateFrontLisenceKeyUseSpecInputData(char *inputData,char *serialNumber,char *key,int isVerify);

/*
����
	�����������ݺ����к��Լ���Ȩ��У����Ȩ��
�������
	inputData	�������ݣ����Ϊ��ָ�룬��ȡunionREC.CFG�е�nameOfMyself��ֵ
	serialNumber	���кţ����Ϊ��ָ�룬��ȡunionREC.CFG�е����кŵ�ֵ
	key		Ҫ��У����Ȩ�룬���Ϊ��ָ�룬��ȡunionREC.CFG�е�liscenceCode
�������
	date1		ά����������
	date2		ʹ�õ�������
����ֵ
	=2		У��ɹ�
	=1		��ά����
	=0		У��ʧ��
	=-1		����ֹʹ������
	<-1		�������
*/
int UnionVerifyFinalLisenceKeyUseSpecInputData(char *inputData,char *serialNumber,char *date1,char *date2,char *key);

/*
����
	�����Ʒ���ݺ����к��Լ���Ȩ��
�������
	inputData	��������
	serialNumber	���к�
	key		Ҫ��У����Ȩ��
�������
	date1		ά����������
	date2		ʹ�õ�������
����ֵ
	>0		У��ɹ�
	=0		У��ʧ��
	<0		�������
*/
int UnionInputFinalLisenceKeyUseSpecInputData(char *inputData,char *serialNumber,char *date1,char *date2,char *key);

/*
����
	�洢��Ʒ���ݺ����к��Լ���Ȩ��
�������
	inputData	��������
	serialNumber	���к�
	key		Ҫ��У����Ȩ��
�������
	��
����ֵ
	>0		У��ɹ�
	=0		У��ʧ��
	<0		�������
*/
int UnionStoreFinalLisenceInfo(char *inputData,char *serialNumber,char *key);

/*
����
	��ȡ��Ʒ���ݺ����к��Լ���Ȩ��
�������
	��
�������
	inputData	��������
	serialNumber	���к�
	key		Ҫ��У����Ȩ��
����ֵ
	>0		У��ɹ�
	=0		У��ʧ��
	<0		�������
*/
int UnionReadFinalLiscenceInfo(char *inputData,char *serialNumber,char *key);

/*
����
	У���Ʒ��Ȩ��Ϣ
�������
	��
�������
	��
����ֵ
	=2		У��ɹ�
	=1		��ά����
	=0		У��ʧ��
	=-1		����ֹʹ������
	<-1		�������
*/
int UnionVerifyFinalLisenceInfo();

#endif
