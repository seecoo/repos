//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#include <stdio.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionArrayDef.h"
#include "UnionStr.h"

/*
����	
	��һ������ڴ��ж�ȡһ���������Ͷ���
�������
	str		���崮
	lenOfStr	���崮�ĳ���
�������
	pdef		�������Ͷ���
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadArrayDefFromStr(char *str,int lenOfStr,PUnionArrayDef pdef)
{
	int			ret;
	int			index;
	char			var[128];
	
	if ((str == NULL) || (pdef == NULL))
	{
		UnionUserErrLog("in UnionReadArrayDefFromStr:: null parameter!\n");
		return(errCodeParameter);
	}
	
	memset(pdef,0,sizeof(*pdef));
	// �������͵����Ͷ���
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conArrayDefTagNameOfType,pdef->nameOfType,sizeof(pdef->nameOfType))) < 0)
	{
		UnionUserErrLog("in UnionReadArrayDefFromStr:: UnionReadRecFldFromRecStr [%s]!\n",conArrayDefTagNameOfType);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadVarDeclareTypeDefFromStr:: name must be defined!\n");
		return(errCodeCDPMDL_VarTypeNotDefined);
	}
	// ������������
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conArrayDefTagVarName,pdef->name,sizeof(pdef->name))) < 0)
	{
		UnionUserErrLog("in UnionReadArrayDefFromStr:: UnionReadRecFldFromRecStr [%s]!\n",conArrayDefTagVarName);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadVarDeclareTypeDefFromStr:: name must be defined!\n");
		return(errCodeCDPMDL_VarNameNotDefined);
	}
	// ��������˵��
	UnionReadRecFldFromRecStr(str,lenOfStr,conArrayDefTagRemark,pdef->remark,sizeof(pdef->remark));
	// �����ά��
	if ((ret = UnionReadIntTypeRecFldFromRecStr(str,lenOfStr,conArrayDefTagDimisionNum,&(pdef->dimisionNum))) < 0)
		pdef->dimisionNum = 0;
	if (pdef->dimisionNum >= conMaxNumOfArrayDimision)
	{
		UnionUserErrLog("in UnionReadArrayDefFromStr:: dimisionNum [%d] is out of range in [%s]!\n",pdef->dimisionNum,str);
		return(ret);
	}
	if (pdef->dimisionNum <= 0)
	{
		UnionUserErrLog("in UnionReadVarDeclareTypeDefFromStr:: dimisionNum must be defined!\n");
		return(errCodeCDPMDL_ArrayDimisionNotDefined);
	}
	for (index = 0; index < pdef->dimisionNum; index++)
	{
		sprintf(var,"%s%02d",conArrayDefTagSizeOfDimision,index+1);
		if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,var,pdef->sizeOfDimision[index],sizeof(pdef->sizeOfDimision[index]))) < 0)
		{
			UnionUserErrLog("in UnionReadVarDeclareTypeDefFromStr:: sizeOfDimision of dimision index [%02d] must be defined!\n",index+1);
			return(errCodeCDPMDL_ArrayDimisionSizeNotDefined);
		}
	}
	return(0);
}

/* ��ָ������Ķ����ӡ���ļ���
�������
	pdef	���鶨��
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintArrayDefToFp(PUnionArrayDef pdef,FILE *fp)
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
	fprintf(outFp,"typedef %s %s ",pdef->nameOfType,pdef->name);
	for (index = 0; index < pdef->dimisionNum; index++)
	{
		if (strlen(pdef->sizeOfDimision[index]) > 0)
			fprintf(outFp,"[%s]",pdef->sizeOfDimision[index]);
		else
			fprintf(outFp,"[]");
	}
	fprintf(outFp,";\n");	
	return(0);
}

/* ��ָ���ļ��ж���������ӡ���ļ���
�������
	fileName	�ļ�����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintArrayDefInFileToFp(char *fileName,FILE *fp)
{
	TUnionArrayDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadArrayDefFromSpecFile(fileName,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintArrayDefInFileToFp:: UnionReadArrayDefFromSpecFile!\n");
		return(ret);
	}
	return(UnionPrintArrayDefToFp(&def,fp));
}
	
/* ��ָ���ļ��ж���������ӡ����Ļ��
�������
	pdef	���鶨��
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputArrayDefInFile(char *fileName)
{
	return(UnionPrintArrayDefInFileToFp(fileName,stdout));

}


/* ��ָ�����Ƶ����鶨��������ļ���
�������
	nameOfType	��������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintSpecArrayDefToFp(char *nameOfType,FILE *fp)
{
	TUnionArrayDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadArrayDefFromDefaultDefFile(nameOfType,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintSpecArrayDefToFp:: UnionReadArrayDefFromDefaultDefFile!\n");
		return(ret);
	}
	return(UnionPrintArrayDefToFp(&def,fp));
}
	
/* ��ָ���ļ��ж���������ӡ����Ļ��
�������
	nameOfType	��������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputSpecArrayDef(char *nameOfType)
{
	return(UnionPrintSpecArrayDefToFp(nameOfType,stdout));

}

