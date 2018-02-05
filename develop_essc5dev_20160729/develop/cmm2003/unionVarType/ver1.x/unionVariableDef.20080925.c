// wolfang wang
// 2008/10/3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionVariableDef.h"
#include "unionVarTypeDef.h"
#include "UnionLog.h"

/*
����	
	��һ������ڴ��ж�ȡһ��ȫ�ֱ������Ͷ���
�������
	str		���崮
	lenOfStr	���崮�ĳ���
�������
	pdef		ȫ�ֱ�������
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadVariableDefFromStr(char *str,int lenOfStr,PUnionVariableDef pdef)
{
	int	ret;
	char	type[100];
	
	memset(pdef,0,sizeof(*pdef));
	// ȫ�ֱ�������
	if ((ret = UnionReadVarDefFromStr(str,lenOfStr,&(pdef->varDef))) < 0)
	{
		UnionUserErrLog("in UnionReadVariableDefFromStr:: UnionReadVarDefFromStr from [%s]!\n",str);
		return(ret);
	}
	// ȫ�ֱ���ֵ
	UnionReadRecFldFromRecStr(str,lenOfStr,conVariableDefTagDefaultValue,pdef->defaultValue,sizeof(pdef->defaultValue));
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
int UnionPrintVariableDefToFp(PUnionVariableDef pdef,FILE *fp)
{
	FILE	*outFp = stdout;
	int	ret;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp != NULL)
		outFp = fp;
	if ((ret = UnionPrintVarDefToFp(&(pdef->varDef),1,fp)) < 0)
	{
		UnionUserErrLog("in UnionPrintVariableDefToFp:: UnionPrintVarDefToFp!\n");
		return(ret);
	}
	if (strlen(pdef->defaultValue) > 0)
	{
		switch (UnionConvertCVarDefTypeIntoTag(pdef->varDef.nameOfType))
		{
			case	conVarTypeTagChar:
				if (pdef->varDef.dimisionNum == 0)
					fprintf(outFp," = '%c'",pdef->defaultValue[0]);
				else
					fprintf(outFp," = \"%s\"",pdef->defaultValue);
				break;
			case	conVarTypeTagString:
				fprintf(outFp," = \"%s\"",pdef->defaultValue);
				break;
			case	conVarTypeTagInt:
			case	conVarTypeTagLong:
			case	conVarTypeTagDouble:
			case	conVarTypeTagFloat:
			case	conVarTypeTagFile:
			case	conVarTypeTagPointer:
			case	conVarTypeTagStruct:
			case	conVarTypeTagUnion:
				fprintf(outFp," = %s",pdef->defaultValue);
				break;
			case	conVarTypeTagSimpleType:
				fprintf(outFp," = %s",pdef->defaultValue);
				break;
			default:
				UnionUserErrLog("in UnionPrintVariableDefToFp:: invalid type [%d] having defaultValue [%s]\n",pdef->varDef.nameOfType,pdef->defaultValue);
				break;
		}
	}
	else
	{
		switch (UnionConvertCVarDefTypeIntoTag(pdef->varDef.nameOfType))
		{
			case	conVarTypeTagChar:
				if (pdef->varDef.dimisionNum > 0)
					fprintf(outFp," = \"\"");
				break;
			case	conVarTypeTagString:
				fprintf(outFp," = \"\"");
				break;
			default:
				break;
		}
	}		
	fprintf(outFp,";");
	if (strlen(pdef->varDef.remark) != 0)
		fprintf(outFp,"  // %s",pdef->varDef.remark);
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
int UnionPrintVariableDefInFileToFp(char *fileName,FILE *fp)
{
	TUnionVariableDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadVariableDefFromSpecFile(fileName,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintVariableDefInFileToFp:: UnionReadVariableDefFromSpecFile!\n");
		return(ret);
	}
	return(UnionPrintVariableDefToFp(&def,fp));
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
int UnionOutputVariableDefInFile(char *fileName)
{
	return(UnionPrintVariableDefInFileToFp(fileName,stdout));

}

/* ��ָ�����Ƶ�ȫ�ֱ�������������ļ���
�������
	nameOfVariable	ȫ�ֱ�������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintSpecVariableDefToFp(char *nameOfVariable,FILE *fp)
{
	TUnionVariableDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadVariableDefFromDefaultDefFile(nameOfVariable,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintSpecVariableDefToFp:: UnionReadVariableDefFromDefaultDefFile!\n");
		return(ret);
	}
	return(UnionPrintVariableDefToFp(&def,fp));
}
	
/* ��ָ���ļ��ж����ȫ�ֱ�����ӡ����Ļ��
�������
	nameOfVariable	ȫ�ֱ�������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputSpecVariableDef(char *nameOfVariable)
{
	return(UnionPrintSpecVariableDefToFp(nameOfVariable,stdout));

}

