// wolfang wang
// 2008/10/3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionVarDef.h"
#include "unionFunDef.h"
#include "UnionLog.h"

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
int UnionGenerateCallCodesOfFun(int prefixBlankNum,char *retVarName,PUnionFunDef pdef,FILE *fp)
{
	int	index;
	FILE	*outFp = stdout;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp != NULL)
		outFp = fp;
	for (index = 0; index < prefixBlankNum; index++)
		fprintf(outFp," ");
	if ((retVarName != NULL) && (strlen(retVarName) != 0))
		fprintf(outFp,"%s = ",retVarName);
	fprintf(outFp,"%s(",pdef->funName);
	for (index = 0; index < pdef->varNum; index++)
	{
		if (index > 0)
			fprintf(outFp,",");
		fprintf(outFp,"%s",pdef->varGrp[index].varDef.name);
	}
	fprintf(outFp,");\n");
	return(0);
}

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
int UnionPrintFunDefRemarkToFp(PUnionFunDef pdef,FILE *fp)
{
	FILE	*outFp;
	int	index;
	int	varNum;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp == NULL)
		outFp = stdout;
	else
		outFp = fp;
	fprintf(outFp,"/*\n");
	fprintf(outFp,"��������\n        %s\n",pdef->remark);
	fprintf(outFp,"�������\n");
	for (index = 0,varNum = 0; index < pdef->varNum; index++)
	{
		if (pdef->varGrp[index].isOutput)
			continue;
		fprintf(outFp,"       %s %s\n",pdef->varGrp[index].varDef.name,pdef->varGrp[index].varDef.remark);
		varNum++;
	}
	if (varNum == 0)
		fprintf(outFp,"       ��\n");
	fprintf(outFp,"�������\n");
	for (index = 0,varNum = 0; index < pdef->varNum; index++)
	{
		if (!pdef->varGrp[index].isOutput)
			continue;
		fprintf(outFp,"       %s %s\n",pdef->varGrp[index].varDef.name,pdef->varGrp[index].varDef.remark);
		varNum++;
	}
	if (varNum == 0)
		fprintf(outFp,"       ��\n");
	fprintf(outFp,"����ֵ\n");
	UnionLog("***[%s]\n",pdef->returnType.remark);
	fprintf(outFp,"       \n",pdef->returnType.remark);
	fprintf(outFp,"*/\n");
	return(0);
}

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
int UnionPrintFunVarDefToFpOfCPragramWithPrefixBlank(int prefixBlankNum,PUnionFunDef pdef,char *defaultName,int defaultArraySizeWhenSizeNotSet,FILE *fp)
{
	int	index;
	int	ret;
	
	if (pdef == NULL)
		return(errCodeParameter);

	for (index = 0; index < pdef->varNum; index++)
	{
		if ((ret = UnionPrintVarDefToFpOfCProgramWithPrefixBlank(prefixBlankNum,&(pdef->varGrp[index].varDef),defaultName,defaultArraySizeWhenSizeNotSet,fp)) < 0)
		{
			UnionUserErrLog("in UnionPrintFunVarDefToFpOfCPragramWithPrefixBlank:: UnionPrintVarDefToFpOfCProgram! [%s]!\n",pdef->funName);
			return(ret);
		}
	}
	if ((ret = UnionPrintVarDefToFpOfCProgramWithPrefixBlank(prefixBlankNum,&(pdef->returnType),defaultName,defaultArraySizeWhenSizeNotSet,fp)) < 0)
	{
		UnionUserErrLog("in UnionPrintFunVarDefToFpOfCPragramWithPrefixBlank:: UnionPrintVarDefToFpOfCProgram! [%s]!\n",pdef->funName);
		return(ret);
	}
	return(0);
}

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
int UnionPrintFunVarDefToFpOfCPragram(PUnionFunDef pdef,char *defaultName,int defaultArraySizeWhenSizeNotSet,FILE *fp)
{
	return(UnionPrintFunVarDefToFpOfCPragramWithPrefixBlank(0,pdef,defaultName,defaultArraySizeWhenSizeNotSet,fp));
}

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
int UnionPrintFunVarDefWithPrefixToFpOfCPragram(char *prefix,PUnionFunDef pdef,char *defaultName,int defaultArraySizeWhenSizeNotSet,FILE *fp)
{
	int	index;
	int	ret;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if ((prefix == NULL) || (strlen(prefix) == 0))
		return(UnionPrintFunVarDefToFpOfCPragram(pdef,defaultName,defaultArraySizeWhenSizeNotSet,fp));
	for (index = 0; index < pdef->varNum; index++)
	{
		if ((ret = UnionPrintVarDefWithPrefixToFpOfCProgram(prefix,&(pdef->varGrp[index].varDef),defaultName,defaultArraySizeWhenSizeNotSet,fp)) < 0)
		{
			UnionUserErrLog("in UnionPrintFunVarDefWithPrefixToFpOfCPragram:: UnionPrintVarDefWithPrefixToFpOfCProgram! [%s]!\n",pdef->funName);
			return(ret);
		}
	}
	if ((ret = UnionPrintVarDefWithPrefixToFpOfCProgram(prefix,&(pdef->returnType),defaultName,defaultArraySizeWhenSizeNotSet,fp)) < 0)
	{
		UnionUserErrLog("in UnionPrintFunVarDefWithPrefixToFpOfCPragram:: UnionPrintVarDefWithPrefixToFpOfCProgram! [%s]!\n",pdef->funName);
		return(ret);
	}
	return(0);
}

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
int UnionPrintFunDefToFp(PUnionFunDef pdef,int inCProgramFormat,int isDeclaration,FILE *fp)
{
	FILE	*outFp;
	int	index;
	int	varNum;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp == NULL)
		outFp = stdout;
	else
		outFp = fp;
	if (inCProgramFormat)
	{
		UnionPrintFunDefRemarkToFp(pdef,outFp);	// ����˵��
		UnionPrintVarTypeDeclareCProgramFormatToFp(&(pdef->returnType),outFp);	// ����ֵ
		fprintf(outFp,"%s(",pdef->funName); // ��������
		for (index = 0; index < pdef->varNum; index++)
		{
			if (index != 0)
				fprintf(outFp,",");
			UnionPrintVarDefToFp(&(pdef->varGrp[index].varDef),inCProgramFormat,outFp);	// ����
		}
		if (isDeclaration)
			fprintf(outFp,");\n");
		else
			fprintf(outFp,")\n");
	}
	else
	{
		fprintf(outFp,"funName=%s|remark=%s|",pdef->funName,pdef->remark); // ��������
		for (index = 0; index < pdef->varNum; index++)
		{
			fprintf(outFp,"varDef=");
			fprintf(outFp,"isOutput=%d|",pdef->varGrp[index].isOutput);
			UnionPrintVarDefToFp(&(pdef->varGrp[index].varDef),inCProgramFormat,outFp);	// ����
			fprintf(outFp,"\n");
		}
		fprintf(outFp,"returnType=");
		UnionPrintVarDefToFp(&(pdef->returnType),inCProgramFormat,outFp);	// ����ֵ
		fprintf(outFp,"\n");
	}
	return(0);
}
	
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
int UnionPrintFunDefInFileDefToFp(char *fileName,int inCProgramFormat,int isDeclaration,FILE *fp)
{
	TUnionFunDef		def;
	int			ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadFunDefFromSpecFile(fileName,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintFunDefInFileDefToFp:: UnionReadFunDefFromSpecFile!\n");
		return(ret);
	}
	return(UnionPrintFunDefToFp(&def,inCProgramFormat,isDeclaration,fp));
}

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
int UnionOutputFunDefInFileDef(char *fileName,int inCProgramFormat,int isDeclaration)
{
	return(UnionPrintFunDefInFileDefToFp(fileName,inCProgramFormat,isDeclaration,stdout));
}

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
int UnionOutputFunDefInDefaultDefFile(char *funName,int isDeclaration)
{
	TUnionFunDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadFunDefFromDefaultDefFile(funName,&def)) < 0)
	{
		UnionUserErrLog("in UnionOutputFunDefInDefaultDefFile:: UnionReadFunDefFromDefaultDefFile [%s]\n",funName);
		return(ret);
	}
	return(UnionPrintFunDefToFp(&def,1,0,stdout));
}

/* ��ָ�����Ƶĺ�������������ļ���
�������
	funName	��������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintSpecFunDefToFp(char *funName,FILE *fp)
{
	TUnionFunDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadFunDefFromDefaultDefFile(funName,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintSpecFunDefToFp:: UnionReadFunDefFromDefaultDefFile!\n");
		return(ret);
	}
	return(UnionPrintFunDefToFp(&def,1,1,fp));
}
	
/* ��ָ���ļ��ж���ĺ�����ӡ����Ļ��
�������
	funName	��������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputSpecFunDef(char *funName)
{
	return(UnionPrintSpecFunDefToFp(funName,stdout));

}

		
