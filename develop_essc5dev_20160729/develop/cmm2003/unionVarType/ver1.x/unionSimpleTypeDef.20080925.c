//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#include <stdio.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionVarTypeDef.h"
#include "unionSimpleTypeDef.h"
#include "UnionStr.h"
#include "unionDefaultVarTypeDefDir.h"

/*
����	
	���ָ�������Ͷ�Ӧ��c���Ե�����
�������
	nameOfType	��������
�������
	��
����ֵ
	�������͵��ڲ���ʶ
*/
int UnionGetFinalTypeTagOfSpecNameOfSimpleType(char *nameOfType)
{
	char			finalNameOfType[256+1];
	int			ret;
	
	memset(finalNameOfType,0,sizeof(finalNameOfType));
	if ((ret = UnionGetFinalTypeNameOfSpecNameOfType(nameOfType,finalNameOfType)) < 0)
	{
		UnionUserErrLog("in UnionGetFinalTypeTagOfSpecNameOfSimpleType:: UnionGetFinalTypeNameOfSpecNameOfType! [%s]\n",nameOfType);
		return(ret);
	}
	return(UnionGetTypeTagOfSpecNameOfType(finalNameOfType));
}

/*
����	
	���ָ�������Ͷ�Ӧ����������
�������
	oriNameOfType	��������
�������
	finalNameOfType	������������
����ֵ
	>= 0		�ɹ�
	<0		�������
*/
int UnionGetFinalTypeNameOfSpecNameOfSimpleType(char *oriNameOfType,char *finalNameOfType)
{
	TUnionSimpleTypeDef	def;
	char			fileName[256+1];
	int			ret;
	
	if (UnionGetTypeTagOfSpecNameOfType(oriNameOfType) != conVarTypeTagSimpleType)
	{
		strcpy(finalNameOfType,oriNameOfType);
		return(0);
	}
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadSimpleTypeDefFromDefaultDefFile(oriNameOfType,&def)) < 0)
	{
		UnionUserErrLog("in UnionGetFinalTypeNameOfSpecNameOfSimpleType:: UnionReadSimpleTypeDefFromDefaultDefFile!\n");
		return(ret);
	}
	return(UnionGetFinalTypeNameOfSpecNameOfType(def.nameOfType,finalNameOfType));
}

/*
����	
	��һ������ڴ��ж�ȡһ�����������Ͷ���
�������
	str		���崮
	lenOfStr	���崮�ĳ���
�������
	pdef		���������Ͷ���
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadSimpleTypeDefFromStr(char *str,int lenOfStr,PUnionSimpleTypeDef pdef)
{
	int			ret;
	
	if ((str == NULL) || (pdef == NULL))
	{
		UnionUserErrLog("in UnionReadSimpleTypeDefFromStr:: null parameter!\n");
		return(errCodeParameter);
	}
	
	memset(pdef,0,sizeof(*pdef));
	// ���������͵����Ͷ���
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conSimpleTypeDefTagNameOfType,pdef->nameOfType,sizeof(pdef->nameOfType))) < 0)
	{
		UnionUserErrLog("in UnionReadSimpleTypeDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]!\n",conSimpleTypeDefTagNameOfType,str);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadVarDeclareTypeDefFromStr:: name must be defined!\n");
		return(errCodeCDPMDL_VarTypeNotDefined);
	}
	// ��������������
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conSimpleTypeDefTagVarName,pdef->name,sizeof(pdef->name))) < 0)
	{
		UnionUserErrLog("in UnionReadSimpleTypeDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]!\n",conSimpleTypeDefTagVarName,str);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadVarDeclareTypeDefFromStr:: name must be defined!\n");
		return(errCodeCDPMDL_VarNameNotDefined);
	}
	// ����������˵��
	UnionReadRecFldFromRecStr(str,lenOfStr,conSimpleTypeDefTagRemark,pdef->remark,sizeof(pdef->remark));
	return(0);
}

/* ��ָ�������͵Ķ����ӡ���ļ���
�������
	pdef	�����Ͷ���
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintSimpleTypeDefToFp(PUnionSimpleTypeDef pdef,FILE *fp)
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
	fprintf(outFp,"typedef %s %s;\n",pdef->nameOfType,pdef->name);
	return(0);
}

/* ��ָ�������͵Ķ����Զ����ʽ��ӡ���ļ���
�������
	pdef	�����Ͷ���
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintSimpleTypeDefToFpInDefFormat(PUnionSimpleTypeDef pdef,FILE *fp)
{
	FILE	*outFp;
	int	index;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp == NULL)
		outFp = stdout;
	else
		outFp = fp;
	//if (strlen(pdef->remark) != 0)
	//	fprintf(outFp,"// %s\n",pdef->remark);
	fprintf(outFp,"%s=%s|%s=%s|%s=%s|\n",conSimpleTypeDefTagVarName,pdef->name,conSimpleTypeDefTagNameOfType,pdef->nameOfType,conSimpleTypeDefTagRemark,pdef->remark);
	return(0);
}

/* ��ָ���ļ��ж���ļ����ʹ�ӡ���ļ���
�������
	fileName	�ļ�����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintSimpleTypeDefInFileToFp(char *fileName,FILE *fp)
{
	TUnionSimpleTypeDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadSimpleTypeDefFromSpecFile(fileName,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintSimpleTypeDefInFileToFp:: UnionReadSimpleTypeDefFromSpecFile!\n");
		return(ret);
	}
	return(UnionPrintSimpleTypeDefToFp(&def,fp));
}
	
/* ��ָ���ļ��ж���ļ����ʹ�ӡ����Ļ��
�������
	pdef	�����Ͷ���
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputSimpleTypeDefInFile(char *fileName)
{
	return(UnionPrintSimpleTypeDefInFileToFp(fileName,stdout));

}

/* ��ָ�����Ƶļ����Ͷ���������ļ���
�������
	nameOfType	����������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintSpecSimpleTypeDefToFp(char *nameOfType,FILE *fp)
{
	TUnionSimpleTypeDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadSimpleTypeDefFromDefaultDefFile(nameOfType,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintSpecSimpleTypeDefToFp:: UnionReadSimpleTypeDefFromDefaultDefFile!\n");
		return(ret);
	}
	return(UnionPrintSimpleTypeDefToFp(&def,fp));
}
	
/* ��ָ���ļ��ж���ļ����ʹ�ӡ����Ļ��
�������
	nameOfType	����������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputSpecSimpleTypeDef(char *nameOfType)
{
	return(UnionPrintSpecSimpleTypeDefToFp(nameOfType,stdout));

}


