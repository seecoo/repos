// wolfang wang
// 2008/10/3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionVarDef.h"
#include "UnionLog.h"

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
int UnionReadVarDeclareTypeDefFromStr(char *str,int lenOfStr,PUnionVarDef pdef)
{
	int	ret;
	
	memset(pdef,0,sizeof(*pdef));

	// ��������ǰ׺
	UnionReadRecFldFromRecStr(str,lenOfStr,conVarDefTagTypePrefix,pdef->typePrefix,sizeof(pdef->typePrefix));
	// ��������
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conVarDefTagNameOfType,pdef->nameOfType,sizeof(pdef->nameOfType))) < 0)
	{
		UnionUserErrLog("in UnionReadVarDeclareTypeDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]!\n",conVarDefTagNameOfType,str);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadVarDeclareTypeDefFromStr:: nameOfType must be defined!\n");
		return(errCodeCDPMDL_VarTypeNotDefined);
	}
	// ָ���ʶ
	if ((ret = UnionReadIntTypeRecFldFromRecStr(str,lenOfStr,conVarDefTagIsPointer,&(pdef->isPointer))) < 0)
		pdef->isPointer = 0;
	return(0);
}

/*
����	
	��һ������ڴ��ж�ȡһ����������
�������
	str		���崮
	lenOfStr	���崮�ĳ���
�������
	pdef		��������
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadVarDefFromStr(char *str,int lenOfStr,PUnionVarDef pdef)
{
	int			ret;
	int			index;
	char			var[128];
	
	if ((str == NULL) || (pdef == NULL))
	{
		UnionUserErrLog("in UnionReadVarDefFromStr:: null parameter!\n");
		return(errCodeParameter);
	}
	
	memset(pdef,0,sizeof(*pdef));
	// �������Ͷ���
	if ((ret = UnionReadVarDeclareTypeDefFromStr(str,lenOfStr,pdef)) < 0)
	{
		UnionUserErrLog("in UnionReadVarDefFromStr:: UnionReadVarDeclareTypeDefFromStr str = [%s]!\n",str);
		return(ret);
	}
	// ��������
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conVarDefTagVarName,pdef->name,sizeof(pdef->name))) < 0)
	{
		UnionUserErrLog("in UnionReadVarDefFromStr:: UnionReadRecFldFromRecStr [%s]! str = [%s]\n",conVarDefTagVarName,str);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadVarDeclareTypeDefFromStr:: name must be defined!\n");
		return(errCodeCDPMDL_VarNameNotDefined);
	}
	// ����˵��
	UnionReadRecFldFromRecStr(str,lenOfStr,conVarDefTagRemark,pdef->remark,sizeof(pdef->remark));
	// �����ά��
	if ((ret = UnionReadIntTypeRecFldFromRecStr(str,lenOfStr,conVarDefTagDimisionNum,&(pdef->dimisionNum))) < 0)
		pdef->dimisionNum = 0;
	if (pdef->dimisionNum >= conMaxNumOfArrayDimision)
	{
		UnionUserErrLog("in UnionReadVarDefFromStr:: dimisionNum [%d] is out of range in [%s]!\n",pdef->dimisionNum,str);
		return(ret);
	}
	for (index = 0; index < pdef->dimisionNum; index++)
	{
		sprintf(var,"%s%02d",conVarDefTagSizeOfDimision,index+1);
		UnionReadRecFldFromRecStr(str,lenOfStr,var,pdef->sizeOfDimision[index],sizeof(pdef->sizeOfDimision[index]));
	}
	return(0);
}

/*
����	
	��ӡһ�����������Ͷ���
�������
	pdef			Ҫ��ӡ�ı�������
�������
	fp		������ļ�ָ��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintVarTypeDeclareCProgramFormatToFp(PUnionVarDef pdef,FILE *fp)
{
	FILE	*outFp;
	int	index;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp == NULL)
		outFp = stdout;
	else
		outFp = fp;
	if (strlen(pdef->typePrefix) != 0)
		fprintf(outFp,"%s ",pdef->typePrefix);
	fprintf(outFp,"%s ",pdef->nameOfType);
	if (pdef->isPointer)
		fprintf(outFp,"*");
	return(0);
}

/*
����	
	��ӡһ���������壬�����������Ĵ�Сδ���壬��ʹ��ȱʡ��С
�������
	pdef				Ҫ��ӡ�ı�������
	defaultName			ȱʡ�����֣����δ�������֣�������������
	defaultArraySizeWhenSizeNotSet	ȱʡ�������С����������Сδ����
�������
	fp		������ļ�ָ��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintVarDefSetArraySizeToFp(PUnionVarDef pdef,char *defaultName,int defaultArraySizeWhenSizeNotSet,FILE *fp)
{
	FILE	*outFp = stdout;
	int	index;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp != NULL)
		outFp = fp;
	UnionPrintVarTypeDeclareCProgramFormatToFp(pdef,outFp);
	if (strlen(pdef->name) != 0)
		fprintf(outFp,"%s",pdef->name);
	else if ((defaultName != NULL) && (strlen(defaultName) != 0))
		fprintf(outFp,"%s",defaultName);
	else
		fprintf(outFp,"unnamedVar");
	for (index = 0; index < pdef->dimisionNum; index++)
	{
		if (strlen(pdef->sizeOfDimision[index]) > 0)
			fprintf(outFp,"[%s]",pdef->sizeOfDimision[index]);
		else
		{
			if (defaultArraySizeWhenSizeNotSet > 0)
				fprintf(outFp,"[%d]",defaultArraySizeWhenSizeNotSet);
			else
				fprintf(outFp,"[]");
		}
	}
	return(0);
}

/*
����	
	��ӡһ���������壬�ڱ�����ǰ������ǰ׺�������������Ĵ�Сδ���壬��ʹ��ȱʡ��С
�������
	prefix				����ǰ׺
	pdef				Ҫ��ӡ�ı�������
	defaultName			ȱʡ�����֣����δ�������֣�������������
	defaultArraySizeWhenSizeNotSet	ȱʡ�������С����������Сδ����
�������
	fp		������ļ�ָ��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintVarDefWithPrefixSetArraySizeToFp(char *prefix,PUnionVarDef pdef,char *defaultName,int defaultArraySizeWhenSizeNotSet,FILE *fp)
{
	FILE	*outFp = stdout;
	int	index;
	
	if ((prefix == NULL) || (strlen(prefix) == 0))
		return(UnionPrintVarDefSetArraySizeToFp(pdef,defaultName,defaultArraySizeWhenSizeNotSet,fp));
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp != NULL)
		outFp = fp;
	UnionPrintVarTypeDeclareCProgramFormatToFp(pdef,outFp);
	if (strlen(pdef->name) != 0)
		fprintf(outFp,"%s%s",prefix,pdef->name);
	else if ((defaultName != NULL) && (strlen(defaultName) != 0))
		fprintf(outFp,"%s%s",prefix,defaultName);
	else
		fprintf(outFp,"%sunnamedVar",prefix);
	for (index = 0; index < pdef->dimisionNum; index++)
	{
		if (strlen(pdef->sizeOfDimision[index]) > 0)
			fprintf(outFp,"[%s]",pdef->sizeOfDimision[index]);
		else
		{
			if (defaultArraySizeWhenSizeNotSet > 0)
				fprintf(outFp,"[%d]",defaultArraySizeWhenSizeNotSet);
			else
				fprintf(outFp,"[]");
		}
	}
	return(0);
}

/*
����	
	��ӡһ����������
�������
	pdef			Ҫ��ӡ�ı�������
	inCProgramFormat	1��ʹ��c���Զ����ʽ�����0����ʹ��c���Զ������
�������
	fp		������ļ�ָ��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintVarDefToFp(PUnionVarDef pdef,int inCProgramFormat,FILE *fp)
{
	FILE	*outFp;
	int	index;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp == NULL)
		outFp = stdout;
	else
		outFp = fp;
	if (inCProgramFormat)
	{
		return(UnionPrintVarDefSetArraySizeToFp(pdef,"",-1,outFp));
	}
	else
	{
		fprintf(outFp,"name=%s|nameOfType=%s|typePrefix=%s|isPointer=%d|dimisionNum=%d|remark=%s|",pdef->name,pdef->nameOfType,pdef->typePrefix,pdef->isPointer,pdef->dimisionNum,pdef->remark);
		for (index = 0; index < pdef->dimisionNum; index++)
			fprintf(outFp,"sizeOfDimision%02d=%s|",index+1,pdef->sizeOfDimision[index]);
	}
	
	return(0);
}

/*
����	
	�������Ĳ������壬�Ա����������ʽ��ӡ���ļ���
�������
	prefixBlankNum			������ǰ׺�Ŀո���
	pdef				Ҫ��ӡ�ı�������
	defaultName			ȱʡ�����֣����δ�������֣�������������
	defaultArraySizeWhenSizeNotSet	ȱʡ�������С����������Сδ����
	fp		������ļ�ָ��
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintVarDefToFpOfCProgramWithPrefixBlank(int prefixBlankNum,PUnionVarDef pdef,char *defaultName,int defaultArraySizeWhenSizeNotSet,FILE *fp)
{
	int	ret;
	FILE	*outFp = stdout;
	int	index;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp != NULL)
		outFp = fp;
	
	for (index = 0; index < prefixBlankNum; index++)
		fprintf(outFp," ");
	// ���庯������
	if ((ret = UnionPrintVarDefSetArraySizeToFp(pdef,defaultName,defaultArraySizeWhenSizeNotSet,outFp)) < 0)
	{
		UnionUserErrLog("in UnionPrintVarDefToFpOfCProgramWithPrefixBlank:: UnionPrintVarDefSetArraySizeToFp!\n");
		return(ret);
	}
	fprintf(outFp,";");
	if (strlen(pdef->remark) != 0)
		fprintf(outFp,"  // %s\n",pdef->remark);
	else
		fprintf(outFp,"\n");
	return(0);
}


/*
����	
	�������Ĳ������壬�Ա����������ʽ��ӡ���ļ���
�������
	pdef				Ҫ��ӡ�ı�������
	defaultName			ȱʡ�����֣����δ�������֣�������������
	defaultArraySizeWhenSizeNotSet	ȱʡ�������С����������Сδ����
	fp		������ļ�ָ��
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintVarDefToFpOfCProgram(PUnionVarDef pdef,char *defaultName,int defaultArraySizeWhenSizeNotSet,FILE *fp)
{
	return(UnionPrintVarDefToFpOfCProgramWithPrefixBlank(0,pdef,defaultName,defaultArraySizeWhenSizeNotSet,fp));
}

/*
����	
	�������Ĳ������壬�Ա����������ʽ��ӡ���ļ��У��ڱ���������ǰ��ǰ׺
�������
	prefix				����ǰ׺
	pdef				Ҫ��ӡ�ı�������
	defaultName			ȱʡ�����֣����δ�������֣�������������
	defaultArraySizeWhenSizeNotSet	ȱʡ�������С����������Сδ����
	fp		������ļ�ָ��
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintVarDefWithPrefixToFpOfCProgram(char *prefix,PUnionVarDef pdef,char *defaultName,int defaultArraySizeWhenSizeNotSet,FILE *fp)
{
	int	ret;
	FILE	*outFp = stdout;
	
	if ((prefix == NULL) || (strlen(prefix) == 0))
		return(UnionPrintVarDefToFpOfCProgram(pdef,defaultName,defaultArraySizeWhenSizeNotSet,fp));
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp != NULL)
		outFp = fp;
		
	// ���庯������
	if ((ret = UnionPrintVarDefWithPrefixSetArraySizeToFp(prefix,pdef,defaultName,defaultArraySizeWhenSizeNotSet,outFp)) < 0)
	{
		UnionUserErrLog("in UnionPrintVarDefWithPrefixToFpOfCProgram:: UnionPrintVarDefWithPrefixSetArraySizeToFp!\n");
		return(ret);
	}
	fprintf(outFp,";");
	if (strlen(pdef->remark) != 0)
		fprintf(outFp,"  // %s\n",pdef->remark);
	else
		fprintf(outFp,"\n");
	return(0);
}
	
/*
����	
	��ӡһ����������
�������
	str		���崮
	lenOfStr	���崮�ĳ���
	inCProgramFormat	1��ʹ��c���Զ����ʽ�����0����ʹ��c���Զ������
�������
	fp		������ļ�ָ��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintVarDefInStrDefToFp(char *str,int lenOfStr,int inCProgramFormat,FILE *fp)
{
	TUnionVarDef	def;
	int			ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadVarDefFromStr(str,lenOfStr,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintVarDefInStrDefToFp:: UnionReadVarDefFromStr!\n");
		return(ret);
	}
	return(UnionPrintVarDefToFp(&def,inCProgramFormat,fp));
}

/*
����	
	��ӡһ����������
�������
	str		���崮
	lenOfStr	���崮�ĳ���
	inCProgramFormat	1��ʹ��c���Զ����ʽ�����0����ʹ��c���Զ������
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionOutputVarDefInStrDef(char *str,int lenOfStr,int inCProgramFormat)
{
	return(UnionPrintVarDefInStrDefToFp(str,lenOfStr,inCProgramFormat,stdout));
}
