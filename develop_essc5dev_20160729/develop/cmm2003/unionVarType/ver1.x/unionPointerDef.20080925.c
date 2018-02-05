//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#include <stdio.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionPointerDef.h"
#include "UnionStr.h"
#include "unionDefaultVarTypeDefDir.h"


/* ��ȱʡ�ļ���ָ�����Ƶ�ָ��Ķ���
�������
	nameOfType	ָ�����͵�����
�������
	typeNameOfPointer	ָ�����͵�����
����ֵ��
	>=0 	�ɹ�
	<0	�������	
	
*/
int UnionReadTypeOfPointerDefFromDefaultDefFile(char *nameOfType,char *typeNameOfPointer)
{
	TUnionPointerDef	def;
	int			ret;
	
	if ((ret = UnionReadPointerDefFromDefaultDefFile(nameOfType,&def)) < 0)
	{
		UnionUserErrLog("in UnionReadTypeOfPointerDefFromDefaultDefFile:: UnionReadPointerDefFromDefaultDefFile!\n");
		return(ret);
	}
	strcpy(typeNameOfPointer,def.nameOfType);
	return(0);
}

/*
����	
	��һ������ڴ��ж�ȡһ��ָ�����Ͷ���
�������
	str		���崮
	lenOfStr	���崮�ĳ���
�������
	pdef		ָ�����Ͷ���
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadPointerDefFromStr(char *str,int lenOfStr,PUnionPointerDef pdef)
{
	int			ret;
	
	if ((str == NULL) || (pdef == NULL))
	{
		UnionUserErrLog("in UnionReadPointerDefFromStr:: null parameter!\n");
		return(errCodeParameter);
	}
	
	memset(pdef,0,sizeof(*pdef));
	// ָ�����͵����Ͷ���
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conPointerDefTagNameOfType,pdef->nameOfType,sizeof(pdef->nameOfType))) < 0)
	{
		UnionUserErrLog("in UnionReadPointerDefFromStr:: UnionReadRecFldFromRecStr [%s]!\n",conPointerDefTagNameOfType);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadVarDeclareTypeDefFromStr:: name must be defined!\n");
		return(errCodeCDPMDL_VarTypeNotDefined);
	}
	// ָ����������
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conPointerDefTagVarName,pdef->name,sizeof(pdef->name))) < 0)
	{
		UnionUserErrLog("in UnionReadPointerDefFromStr:: UnionReadRecFldFromRecStr [%s]!\n",conPointerDefTagVarName);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadVarDeclareTypeDefFromStr:: name must be defined!\n");
		return(errCodeCDPMDL_VarNameNotDefined);
	}
	// ָ������˵��
	UnionReadRecFldFromRecStr(str,lenOfStr,conPointerDefTagRemark,pdef->remark,sizeof(pdef->remark));
	return(0);
}

/* ��ָ��ָ��Ķ����ӡ���ļ���
�������
	pdef	ָ�붨��
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintPointerDefToFp(PUnionPointerDef pdef,FILE *fp)
{
	FILE	*outFp;
	int	index;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp == NULL)
		outFp = stdout;
	else
		outFp = fp;
	if (strlen(pdef->remark) != 0)
		fprintf(outFp,"// %s\n",pdef->remark);
	fprintf(outFp,"typedef %s *%s;\n",pdef->nameOfType,pdef->name);
	return(0);
}

/* ��ָ��ָ��Ķ����ӡ�������ļ���
�������
	pdef	ָ�붨��
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintPointerDefToFpInDefFormat(PUnionPointerDef pdef,FILE *fp)
{
	FILE	*outFp;
	int	index;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp == NULL)
		outFp = stdout;
	else
		outFp = fp;
	fprintf(outFp,"%s=%s|%s=%s|%s=%s|\n",conPointerDefTagVarName,pdef->name,conPointerDefTagNameOfType,pdef->nameOfType,conPointerDefTagRemark,pdef->remark);
	return(0);
}

/* ��ָ���ļ��ж����ָ���ӡ���ļ���
�������
	fileName	�ļ�����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintPointerDefInFileToFp(char *fileName,FILE *fp)
{
	TUnionPointerDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadPointerDefFromSpecFile(fileName,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintPointerDefInFileToFp:: UnionReadPointerDefFromSpecFile!\n");
		return(ret);
	}
	return(UnionPrintPointerDefToFp(&def,fp));
}
	
/* ��ָ���ļ��ж����ָ���ӡ����Ļ��
�������
	pdef	ָ�붨��
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputPointerDefInFile(char *fileName)
{
	return(UnionPrintPointerDefInFileToFp(fileName,stdout));

}

/* ��ָ�����Ƶ�ָ�����Ͷ���������ļ���
�������
	nameOfType	ָ����������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintSpecPointerDefToFp(char *nameOfType,FILE *fp)
{
	TUnionPointerDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadPointerDefFromDefaultDefFile(nameOfType,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintSpecPointerDefToFp:: UnionReadPointerDefFromDefaultDefFile!\n");
		return(ret);
	}
	return(UnionPrintPointerDefToFp(&def,fp));
}
	
/* ��ָ���ļ��ж����ָ�����ʹ�ӡ����Ļ��
�������
	nameOfType	ָ����������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputSpecPointerDef(char *nameOfType)
{
	return(UnionPrintSpecPointerDefToFp(nameOfType,stdout));

}

