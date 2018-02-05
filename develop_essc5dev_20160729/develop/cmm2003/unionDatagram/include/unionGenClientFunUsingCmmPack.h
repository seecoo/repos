//	Wolfgang Wang
//	2008/10/08

#ifndef _unionGenClientFunUsingCmmPack_
#define _unionGenClientFunUsingCmmPack_

#include "unionPackAndFunRelation.h"

/*
����	
	���������ĺ���Ӧ���ı�������
�������
	isClientFun	��ʶ�Ƿ��ǿͻ��˺���
�������
	putFldIntVarName	����������
	reaFldFromVarName	��Ӧ��������
����ֵ
	>=0		�ɹ�
	<0		�������
*/
void UnionSetCmmPackReqAndResVarName(int isClientFun,char *putFldIntVarName,char *reaFldFromVarName);

/*
����	
	��ȡһ�����������������ֵ����
�������
	funName		��������
	varValueTag	ֵ����
	inputOrOutput	�Ƕ����뻹�������1�����������2�����������3�Ƿ���ֵ
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadTypeOfSpecVarValueTag(char *funName,char *varValueTag,int inputOrOutput);

/*
����	
	����һ�ζԱ�������и�ֵ�Ĵ���
�������
	isClientFun	��ʶ�Ƿ��ǿͻ��˺���
	funName		��������
	errExit		�����˳��Ķϵ�
	ppackAndFunDef	��ֵ��
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateCmmPackSetCodes(int isClientFun,char *funName,char *errExit,PUnionPackAndFunRelation ppackAndFunDef,FILE *fp);

/*
����	
	����һ���Ա�������и�ֵ�ĺ���
�������
	isClientFun	��ʶ�Ƿ��ǿͻ��˺���
	funName		��������
	errExit		�����˳��Ķϵ�
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateCmmPackFldSetCodesForFun(int isClientFun,char *funName,char *errExit,FILE *fp);

/*
����	
	����һ���ӱ������ֵ����Ĵ���
�������
	isClientFun	��ʶ�Ƿ��ǿͻ��˺���
	funName		��������
	errExit		�����˳��Ķϵ�
	ppackAndFunDef	��ֵ��
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateCmmPackReadCodes(int isClientFun,char *funName,char *errExit,PUnionPackAndFunRelation ppackAndFunDef,FILE *fp);

/*
����	
	����һ���ӱ������ֵ����ĺ���
�������
	isClientFun	��ʶ�Ƿ��ǿͻ��˺���
	funName		��������
	errExit		�����˳��Ķϵ�
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateCmmPackFldReadCodesForFun(int isClientFun,char *funName,char *errExit,FILE *fp);

/*
����	
	������������
�������
	fp		�ļ����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateCmmPackAndFunVarDeclareCodes(FILE *fp);

/*
����	
	�����������ͨѶ�Ĵ���
�������
	fp		�ļ����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateCommWithCmmPackSvrCodes(char *funName,FILE *fp);

/*
����	
	����һ���ͻ��˺���
�������
	funName		��������
	incConfFileName	���ɵĺ���Ҫʹ�õ�ͷ�ļ�
	fp		�ļ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateClientFunUsingCmmPackToFp(char *funName,char *incConfFileName,FILE *fp);

/*
����	
	����һ���ͻ��˺���
�������
	funName		��������
	toNewFile	�Ƿ�д�����ļ���
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateClientFunUsingCmmPack(char *funName,int toNewFile);

/*
����	
	Ϊָ���ĳ����������пͻ���API
�������
	nameOfProgram	��������
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateAllCmmPackClientAPIInSpecProgram(char *nameOfProgram);

/*
����	
	����һ������˺���
�������
	funName		��������
	incConfFileName	���ɵĺ���Ҫʹ�õ�ͷ�ļ�
	fp		�ļ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateServerFunUsingCmmPackToFp(char *funName,char *incConfFileName,FILE *fp);

/*
����	
	����һ������˺���
�������
	funName		��������
	toNewFile	�Ƿ�д�����ļ���
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateServerFunUsingCmmPack(char *funName,int toNewFile);

/*
����	
	Ϊָ���ĳ����������з����API
�������
	nameOfProgram	��������
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateAllCmmPackServerAPIInSpecProgram(char *nameOfProgram);

#endif

