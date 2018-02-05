//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#include <stdio.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionStructDef.h"
#include "UnionStr.h"

/* ��һ���ַ����ж�ȡ�ṹ�����Ķ���
�������
	str		�������崮
	lenOfStr	�������崮�ĳ���
�������
	pdeclareDef	��������������
����ֵ��
	>=0 	������������Ŀ
	<0	�������	
	
*/
int UnionReadStructDeclareDefFromStr(char *str,int lenOfStr,PUnionStructDeclareDef pdeclareDef)
{
	int	ret;

	if ((str == NULL) || (lenOfStr <= 0))
		return(0);
	memset(pdeclareDef,0,sizeof(*pdeclareDef));
	if (pdeclareDef == NULL)
	{
		UnionUserErrLog("in UnionReadStructDeclareDefFromStr:: pdeclareDef is null!\n");
		return(errCodeParameter);
	}
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conStructDefTagTypeDefName,pdeclareDef->typeDefName,sizeof(pdeclareDef->typeDefName))) < 0)		
	{
		UnionUserErrLog("in UnionReadStructDeclareDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]\n",conStructDefTagTypeDefName,str);
		return(ret);
	}
	UnionReadRecFldFromRecStr(str,lenOfStr,conStructDefTagStructName,pdeclareDef->structName,sizeof(pdeclareDef->structName));
	UnionReadRecFldFromRecStr(str,lenOfStr,conStructDefTagRemark,pdeclareDef->remark,sizeof(pdeclareDef->remark));
	return(1);
}

/* ��ָ���ṹ������ͷ��ӡ���ļ���
�������
	pdef		��������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ȷ
	<0	�������	
	
*/
int UnionPrintStructHeaderDeclareDefToFp(PUnionStructDeclareDef pdef,FILE *fp)
{
	FILE		*outFp;
	
	if (fp == NULL)
		outFp = stdout;
	else
		outFp = fp;
		
	if (pdef == NULL)
		return(0);

	if (strlen(pdef->remark) > 0)
		fprintf(outFp,"// %s\n", pdef->remark);
	fprintf(outFp,"typedef struct");
	if (strlen(pdef->structName) != 0)
		fprintf(outFp," %s",pdef->structName);
	return(0);
}

/* ��ָ���ṹ������β��ӡ���ļ���
�������
	pdef		��������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ȷ
	<0	�������	
	
*/
int UnionPrintStructTailDeclareDefToFp(PUnionStructDeclareDef pdef,FILE *fp)
{
	FILE		*outFp;
	
	if (fp == NULL)
		outFp = stdout;
	else
		outFp = fp;
		
	if (pdef == NULL)
		return(0);

	fprintf(outFp,"} %s;\n",pdef->typeDefName);
	//fprintf(outFp,"typedef %s *%s;\n",pdef->typeDefName,pdef->typeDefName);
	return(0);
}

/* ��ָ���ṹ�Ķ����ӡ���ļ���
�������
	pdef	�ṹ����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintStructDefToFp(PUnionStructDef pdef,FILE *fp)
{
	int		fldNum;
	FILE		*outFp;
	
	if (fp == NULL)
		outFp = stdout;
	else
		outFp = fp;
	UnionPrintStructHeaderDeclareDefToFp(&(pdef->declareDef),outFp);
	if (pdef->fldNum <= 0)
	{
		fprintf(outFp," %s;\n",pdef->declareDef.typeDefName);
		return(0);
	}
	fprintf(outFp,"\n{\n");
	for (fldNum = 0; fldNum < pdef->fldNum; fldNum++)
	{
		fprintf(outFp,"        ");
		UnionPrintVarDefToFp(&(pdef->fldGrp[fldNum]),1,outFp);
		fprintf(outFp,";");
		if (strlen(pdef->fldGrp[fldNum].remark) != 0)
			fprintf(outFp,"  // %s",pdef->fldGrp[fldNum].remark);
		fprintf(outFp,"\n");
	}
	UnionPrintStructTailDeclareDefToFp(&(pdef->declareDef),outFp);
}

/* ��ָ���ļ��ж���Ľṹ��ӡ���ļ���
�������
	fileName	�ļ�����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintStructDefInFileToFp(char *fileName,FILE *fp)
{
	TUnionStructDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadStructDefFromSpecFile(fileName,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintStructDefInFileToFp:: UnionReadStructDefFromSpecFile!\n");
		return(ret);
	}
	return(UnionPrintStructDefToFp(&def,fp));
}
	
/* ��ָ���ļ��ж���Ľṹ��ӡ����Ļ��
�������
	fileName	�ļ�����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputStructDefInFile(char *fileName)
{
	return(UnionPrintStructDefInFileToFp(fileName,stdout));

}

/* ��ָ�����ƵĽṹ����������ļ���
�������
	nameOfType	�ṹ����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintSpecStructDefToFp(char *nameOfType,FILE *fp)
{
	TUnionStructDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadStructDefFromDefaultDefFile(nameOfType,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintSpecStructDefToFp:: UnionReadStructDefFromDefaultDefFile!\n");
		return(ret);
	}
	return(UnionPrintStructDefToFp(&def,fp));
}
	
/* ��ָ���ļ��ж���Ľṹ��ӡ����Ļ��
�������
	nameOfType	�ṹ����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputSpecStructDef(char *nameOfType)
{
	return(UnionPrintSpecStructDefToFp(nameOfType,stdout));

}

/* ��ָ���ṹ�Ķ����Զ����ʽ��ӡ���ļ���
�������
	pdef	�ṹ����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintStructDefToFpInDefFormat(PUnionStructDef pdef,FILE *fp)
{
	int		fldNum;
	FILE		*outFp = stdout;
	
	if (fp != NULL)
		outFp = fp;

	fprintf(outFp,"%s=%s|%s=%s|%s=%s|\n",conStructDefTagTypeDefName,pdef->declareDef.typeDefName,conStructDefTagStructName,pdef->declareDef.structName,conStructDefTagRemark,pdef->declareDef.remark);
	fprintf(outFp,"\n");
	for (fldNum = 0; fldNum < pdef->fldNum; fldNum++)
	{
		UnionPrintVarDefToFp(&(pdef->fldGrp[fldNum]),0,outFp);
		fprintf(outFp,"\n");
	}
	return(0);
}
