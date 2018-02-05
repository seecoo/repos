#ifndef _unionDesKeyDB_useOracle_
#define _unionDesKeyDB_useOracle_

#include "unionDesKey.h"
#include "unionDesKeyDB.h"

/*
	�������ܣ�
		�������ݿ��û�������
	�����������
	�����������
	�������أ�0
*/
int UnionInitDBOnOracle();

/*
	�������ܣ�
		�������ݿ�
	���������
		timeout����ʱ(��)��>=0��ʱʱ�䣬<0�����ó�ʱ
	�����������
	�������أ�
		0�ɹ���<0ʧ��
*/
int UnionConnectDBOnOracle(int timeout);

/*
	�������ܣ�
		�ύ���񣬶Ͽ�����
	�����������
	�����������
	�������أ�sqlcode
*/
int UnionCommitAndDisconnectOnOracle();

/*
	�������ܣ�
		��¼���ݿ������Ϣ��־
	���������
	���������
	�������أ�
*/
void UnionLogErrMsgOnOracle();

/*
	�������ܣ�
		�ع����񣬶Ͽ�����
	�����������
	�����������
	�������أ�sqlcode
*/
int UnionRollbackAndDisconnectOnOracle();

/*
	�������ܣ�
		deskey��ʼ��ȫ�ֱ���
	���������
	���������
	�������أ�
*/
void UnionInitDesKeyGlobalVar();

/*
	�������ܣ�
		��ӡdeskeyȫ�ֱ���
	���������
	���������
	�������أ�	
*/
void UnionLogDesKeyGlobalVar();

/*
	�������ܣ�
		deskey��ȫ�ֱ�����ֵ
	���������
	���������
	�������أ�
*/
void UnionSetDesKeyGlobalVar(PUnionDesKey pkey);

/*
	�������ܣ�
		ȫ�ֱ�����deskey��ֵ
	���������
	���������
	�������أ�
*/
void UnionSetGlobalVarToDesKey(PUnionDesKey pkey);

/*
	�������ܣ�
		������Կ��
	���������
		pkey����Կ����
	�����������
	�������أ�
		=0�ɹ���<0ʧ��
*/
int UnionInsertDesKeyOnOracle(PUnionDesKey pkey);

/*
	�������ܣ�
		������Կ
	���������
		pkey����Կ����
	���������
	�������أ�
		>0�ɹ���=0δ�ҵ���¼��<0ʧ��
*/
int UnionUpdateDesKeyOnOracle(PUnionDesKey pkey);

/*
	�������ܣ�
		ɾ����Կ
	���������
		pkey����Կ����
	���������
	�������أ�
		>0�ɹ���=0δ�ҵ���¼��<0ʧ��	
*/
int UnionDeleteDesKeyOnOracle(char *fullName);

/*
	�������ܣ�
		��ѯ����ֵ
	���������
		paramName��������
		sizeOfValue: ����paramValue�Ĵ�С
	���������
		paramValue:����ֵ
	�������أ�
		>0��������ĳ��ȣ�=0δ��ѯ�����ݣ�<0ʧ��		
*/
int UnionGetParamValueOnOracle(char *paramName, char *paramValue, int sizeOfValue);

/*
	�������ܣ�
		�������ֵ
	���������
		paramName��������
		paramValue: ����ֵ
		paramRemark: ��ע
	���������
	�������أ�
		=0�ɹ���<0ʧ��		
*/
int UnionInsertParamValueOnOracle(char *paramName, char *paramValue, char *paramRemark);

/*
	�������ܣ�
		���²���ֵ
	���������
		paramName��������
		paramValue: ����ֵ
		paramRemark: ��ע
	���������
	�������أ�
		>0�ɹ���=0δ�ҵ���¼��<0ʧ��		
*/
int UnionUpdateUnionParamOnOracle(char *paramName, char *paramValue, char *paramRemark);

/*
	�������ܣ�
		����Կ���ѯ������Կ�������ڹ����ڴ���
	���������
	���������
	�������أ�
		=0�ɹ���<0ʧ��	
*/
int UnionLoadDesKeyDBIntoMemoryOnOracle();

/*
	�������ܣ�
		��ȡmaxKeyNum
	���������
	���������
	�������أ�
		>0�ɹ�����maxKeyNum��<=0ʧ��	
*/
long UnionGetMaxKeyNumFromKeyDBOnOracle();

/*
	�������ܣ�
		��ȡnum
	���������
	���������
	�������أ�
		>0�ɹ�����num��<=0ʧ��	
*/
long UnionGetNumFromKeyDBOnOracle();

/*
	�������ܣ�
		��ȡLASTUPDATEDTIME
	���������
	���������
	�������أ�
		char* ָ�룬NULLʧ��	
*/
char *UnionGetDesKeyDBLastUpdatedTimeOnOracle();

/*
	�������ܣ�
		����UnionDesKeyParam���UnionDesKeyDB��,���������MAXKEYNUM��NUM��LASTUPDATEDTIME
	���������
	���������
	�������أ�
		=0�ɹ���<0ʧ��	
*/
int UnionCreateDesKeyDBOnOracle(PUnionDesKeyDB pDesKeyDB);

/*
	�������ܣ�
		ɾ��UnionDesKeyParam���UnionDesKeyDB��
	���������
	���������
	�������أ�
		=0�ɹ���<0ʧ��	
*/
int UnionDeleteDesKeyDBOnOracle();


/*
	�������ܣ�
		����num
	���������
	���������
	�������أ�
		=0�ɹ���<0ʧ��	
*/
int UnionUpdateNumFromKeyDBOnOracle();

#endif
