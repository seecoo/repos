// wolfang wang
// 2008/10/3

#ifndef _unionFunDef_
#define _unionFunDef_

#include "unionVarDef.h"

#define conFunDefTagFunName	"funName"
#define conFunDefTagVarDef	"varDef="
#define conFunDefTagIsOutputVar	"isOutput"
#define conFunDefTagReturnType	"returnType="
#define conFunDefTagFunRemark	"remark"

// ������������
typedef struct
{
	TUnionVarDef		varDef;		// ��������
	int			isOutput;	// 1��ʾ���������0��ʾ�������
} TUnionFunParDef;
typedef TUnionFunParDef	*PUnionFunParDef;

// ����һ��������������������Ŀ
#define conMaxNumOfVarPerFun	16
// ��������
typedef struct
{
	char			funName[64+1];	// ��������
	int			varNum;		// ������Ŀ
	TUnionFunParDef		varGrp[conMaxNumOfVarPerFun];	// ������Ŀ
	TUnionVarDef		returnType;	// ����ֵ����
	char			remark[128+1];	// ����˵��
} TUnionFunDef;	
typedef TUnionFunDef		*PUnionFunDef;

/*
����	
	Ϊһ����������һ�ε��ô���
�������
	prefixBlankNum	���ô���ǰ׺�Ŀո���
	retVarName	����ֵ����
	pdef		��������
	fp		������ļ�ָ��
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateCallCodesOfFun(int prefixBlankNum,char *retVarName,PUnionFunDef pdef,FILE *fp);

/*
����	
	��ӡһ�����������˵��
�������
	pdef		Ҫ��ӡ�Ķ���
�������
	fp		������ļ�ָ��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintFunDefRemarkToFp(PUnionFunDef pdef,FILE *fp);

/*
����	
	��ӡһ����������
�������
	pdef			Ҫ��ӡ�ĺ�������
	inCProgramFormat	1��ʹ��c���Զ����ʽ�����0����ʹ��c���Զ������
	isDeclaration		1������������0��������
�������
	fp		������ļ�ָ��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintFunDefToFp(PUnionFunDef pdef,int inCProgramFormat,int isDeclaration,FILE *fp);

/*
����	
	Ϊһ�������Ĳ�������һ���������
�������
	pdef				Ҫ��ӡ�ĺ�������
	defaultName			ȱʡ�����֣����δ�������֣�������������
	defaultArraySizeWhenSizeNotSet	ȱʡ�������С����������Сδ����	
	fp		������ļ�ָ��
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintFunVarDefToFpOfCPragram(PUnionFunDef pdef,char *defaultName,int defaultArraySizeWhenSizeNotSet,FILE *fp);

/*
����	
	Ϊһ�������Ĳ�������һ���������
�������
	prefixBlankNum			������ǰ׺�Ŀո���
	pdef				Ҫ��ӡ�ĺ�������
	defaultName			ȱʡ�����֣����δ�������֣�������������
	defaultArraySizeWhenSizeNotSet	ȱʡ�������С����������Сδ����	
	fp		������ļ�ָ��
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintFunVarDefToFpOfCPragramWithPrefixBlank(int prefixBlankNum,PUnionFunDef pdef,char *defaultName,int defaultArraySizeWhenSizeNotSet,FILE *fp);

/*
����	
	Ϊһ�������Ĳ�������һ���������,�ڱ�������ǰ����ǰ׺
�������
	prefix				����ǰ׺
	pdef				Ҫ��ӡ�ĺ�������
	defaultName			ȱʡ�����֣����δ�������֣�������������
	defaultArraySizeWhenSizeNotSet	ȱʡ�������С����������Сδ����	
	fp		������ļ�ָ��
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintFunVarDefWithPrefixToFpOfCPragram(char *prefix,PUnionFunDef pdef,char *defaultName,int defaultArraySizeWhenSizeNotSet,FILE *fp);

/*
����	
	��ӡһ����������
�������
	fileName		�ļ�����
	inCProgramFormat	1��ʹ��c���Զ����ʽ�����0����ʹ��c���Զ������
	isDeclaration		1������������0��������
�������
	fp		������ļ�ָ��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintFunDefInFileDefToFp(char *fileName,int inCProgramFormat,int isDeclaration,FILE *fp);

/*
����	
	��ӡһ����������
�������
	fileName		�ļ�����
	inCProgramFormat	1��ʹ��c���Զ����ʽ�����0����ʹ��c���Զ������
	isDeclaration		1������������0��������
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionOutputFunDefInFileDef(char *fileName,int inCProgramFormat,int isDeclaration);

/*
����	
	��ӡȱʡ�����ļ��е�һ����������
�������
	funName			��������
	isDeclaration		1������������0��������
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionOutputFunDefInDefaultDefFile(char *funName,int isDeclaration);

/* ��ָ�����Ƶĺ�������������ļ���
�������
	funName	��������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintSpecFunDefToFp(char *funName,FILE *fp);
	
/* ��ָ���ļ��ж���ĺ�����ӡ����Ļ��
�������
	funName	��������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputSpecFunDef(char *funName);

/*
����	
	��һ�������ı����������
�������
	pdef				Ҫ��ӡ�ĺ�������
	specFldOfVar			ȱʡ�����֣����δ�������֣�������������
�������
	��
����ֵ
	>=0		����
	<0		�������
*/
int UnionReadTypeTagOfSpecVarFldOfFun(PUnionFunDef pdef,char *specFldOfVar);

/*
����	
	��һ�������ı����������
�������
	funName				��������
	specFldOfVar			ȱʡ�����֣����δ�������֣�������������
�������
	��
����ֵ
	>=0		����
	<0		�������
*/
int UnionReadTypeTagOfSpecVarFldOfSpecFun(char *funName,char *specFldOfVar);

#endif
