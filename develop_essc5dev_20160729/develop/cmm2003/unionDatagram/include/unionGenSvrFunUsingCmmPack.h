//	Wolfgang Wang
//	2008/10/08

#ifndef _unionGenSvrFunUsingCmmPack_
#define _unionGenSvrFunUsingCmmPack_

#include "unionDatagramDef.h"

/*
����	
	ƴװ��������
�������
	appID		Ӧ����;
	datagramID	���ı�ʶ
�������
	funName		��������
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateSvrCmmPackFunNameOfService(char *appID,char *datagramID,char *funName);

/*
����	
	ƴװ��������
�������
	appID		Ӧ����;
	datagramID	���ı�ʶ
�������
	funName		��������
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateSvrCmmPackFunNameOfService(char *appID,char *datagramID,char *funName);

/*
����	
	ƴװ��������
�������
	appID		Ӧ����;
	datagramID	���ı�ʶ
	version		����汾
�������
	programName	����ȫ������
	incFileConf	����ͷ�ļ����ļ�����
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateSvrCmmPackProgramNameOfService(char *appID,char *datagramID,char *version,char *incFileConf,char *programName);

/*
����	
	����һ���Ա�������и�ֵ�ĺ���
�������
	funName		��������
	pdef		����ָ��
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateSvrCmmPackFldSetCodes(char *funName,PUnionDatagramDef pdef,FILE *fp);

/*
����	
	����һ���ӱ������ֵ����ĺ���
�������
	funName		��������
	pdef		����ָ��
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateSvrCmmPackFldReadCodes(char *funName,PUnionDatagramDef pdef,FILE *fp);

/*
����	
	������������
�������
	pdef		����ָ��
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateSvrCmmPackVarDeclarionCodes(PUnionDatagramDef pdef,FILE *fp);

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
int UnionGenerateNullCmmPackFunCodes(FILE *fp);

/*
����	
	����һ������˺���
�������
	appID		Ӧ����;
	datagramID	���ı�ʶ
	incConfFileName	���ɵĺ���Ҫʹ�õ�ͷ�ļ�
	fp		�ļ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateSvrFunUsingCmmPackToFp(char *appID,char *datagramID,char *incConfFileName,FILE *fp);

/*
����	
	����һ������˺���
�������
	appID		Ӧ����;
	datagramID	���ı�ʶ
	version		����汾
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateSvrFunUsingCmmPack(char *appID,char *datagramID,char *version);

#endif

