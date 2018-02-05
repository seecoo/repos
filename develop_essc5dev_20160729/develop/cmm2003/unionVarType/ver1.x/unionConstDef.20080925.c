// wolfang wang
// 2008/10/3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionConstDefFile.h"
#include "unionVarTypeDef.h"
#include "UnionLog.h"

/*
����	
	��ȱʡ�����ļ���ȡ����������ֵ
�������
	constName	��������
�������
	constValue	����ֵ
����ֵ
	>=0		����ֵ
	<0		�������
*/
int UnionCalculateIntConstValue(char *constName)
{
	char	constValue[512+1];
	int	ret;
	
	if (UnionIsDigitStr(constName))
		return(atoi(constName));
	memset(constValue,0,sizeof(constValue));
	if ((ret = UnionReadConstValueFromDefaultDefFile(constName,"",constValue)) < 0)
	{
		UnionUserErrLog("in UnionCalculateIntConstValue:: UnionReadConstValueFromDefaultDefFile!\n");
		return(ret);
	}
	if (UnionIsDigitStr(constValue))
		return(atoi(constValue));
	else
	{
		UnionUserErrLog("in UnionCalculateIntConstValue:: [%s] not int const!\n",constName);
		return(errCodeCDPMDL_NotIntConst);
	}
}
	
/*
����	
	��һ������ڴ��ж�ȡһ���������Ͷ���
�������
	str		���崮
	lenOfStr	���崮�ĳ���
�������
	pdef		��������
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadConstDefFromStr(char *str,int lenOfStr,PUnionConstDef pdef)
{
	int	ret;
	char	type[100];
	
	memset(pdef,0,sizeof(*pdef));
	// ��������
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conConstDefTagName,pdef->name,sizeof(pdef->name))) < 0)
	{
		UnionUserErrLog("in UnionReadConstDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]!\n",conConstDefTagName,str);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadConstDefFromStr:: name must be defined!\n");
		return(errCodeCDPMDL_ConstNameNotDefined);
	}
	// ����ֵ
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conConstDefTagValue,pdef->value,sizeof(pdef->value))) < 0)
	{
		UnionUserErrLog("in UnionReadConstDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]!\n",conConstDefTagValue,str);
		return(ret);
	}
	// ��������
	memset(type,0,sizeof(type));
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conConstDefTagType,type,sizeof(type))) < 0)
		pdef->type = conVarTypeTagInt;
	else
		pdef->type = UnionConvertCVarDefTypeIntoTag(type);
	// ˵��
	UnionReadRecFldFromRecStr(str,lenOfStr,conConstDefTagRemark,pdef->remark,sizeof(pdef->remark));
	return(0);
}

/* ��ָ�����ͱ�ʶ�Ķ����ӡ���ļ���
�������
	pdef	���ͱ�ʶ����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintConstDefToFp(PUnionConstDef pdef,FILE *fp)
{
	FILE	*outFp;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp == NULL)
		outFp = stdout;
	else
		outFp = fp;
	
	switch (pdef->type)
	{
		case	conVarTypeTagInt:
			fprintf(outFp,"#define %s %s",pdef->name,pdef->value);
			break;
		case	conVarTypeTagChar:
			if (strlen(pdef->value) == 1)
				fprintf(outFp,"#define %s '%s'",pdef->name,pdef->value);
			else
				fprintf(outFp,"#define %s \"%s\"",pdef->name,pdef->value);
			break;
		case	conVarTypeTagEnum:
			fprintf(outFp,"        %s = %s,",pdef->name,pdef->value);
			break;
		default:
			UnionUserErrLog("in UnionPrintConstDefToFp:: invalid const type [%d]!\n",pdef->type);
			return(errCodeCDPMDL_NoValidConstType);
	}
	if (strlen(pdef->remark))
		fprintf(outFp,"  //%s\n",pdef->remark);
	else
		fprintf(outFp,"\n");
	return(0);
}

/* ��ָ���ļ��ж�������ͱ�ʶ��ӡ���ļ���
�������
	fileName	�ļ�����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintConstDefInFileToFp(char *fileName,FILE *fp)
{
	TUnionConstDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadConstDefFromSpecFile(fileName,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintConstDefInFileToFp:: UnionReadConstDefFromSpecFile!\n");
		return(ret);
	}
	return(UnionPrintConstDefToFp(&def,fp));
}
	
/* ��ָ���ļ��ж�������ͱ�ʶ��ӡ����Ļ��
�������
	pdef	���ͱ�ʶ����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputConstDefInFile(char *fileName)
{
	return(UnionPrintConstDefInFileToFp(fileName,stdout));

}

/* ��ȱʡ�����ļ���ָ�����Ƶĳ����Ķ���
�������
	nameOfConst	��������
�������
	pdef	�����ĳ�������
����ֵ��
	>=0 	�����ĳ����Ĵ�С
	<0	�������	
	
*/
int UnionReadConstDefFromDefaultDefFile(char *nameOfConst,PUnionConstDef pdef)
{
	char	fileName[512+1];
	int	ret;
	
	memset(fileName,0,sizeof(fileName));
	UnionGetDefaultFileNameOfConstDef(nameOfConst,fileName);
	if ((ret = UnionReadConstDefFromSpecFile(fileName,pdef)) < 0)
	{
		UnionUserErrLog("in UnionReadConstDefFromDefaultDefFile:: UnionReadConstDefFromSpecFile [%s] from [%s]\n",nameOfConst,fileName);
		return(ret);
	}
	return(ret);
}

/* ��ָ�����Ƶĳ�������������ļ���
�������
	nameOfConst	��������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintSpecConstDefToFp(char *nameOfConst,FILE *fp)
{
	TUnionConstDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadConstDefFromDefaultDefFile(nameOfConst,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintSpecConstDefToFp:: UnionReadConstDefFromDefaultDefFile!\n");
		return(ret);
	}
	return(UnionPrintConstDefToFp(&def,fp));
}
	
/* ��ָ���ļ��ж���ĳ�����ӡ����Ļ��
�������
	nameOfConst	��������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputSpecConstDef(char *nameOfConst)
{
	return(UnionPrintSpecConstDefToFp(nameOfConst,stdout));

}

