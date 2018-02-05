//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#include <stdio.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionUnionDef.h"
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
int UnionReadUnionDeclareDefFromStr(char *str,int lenOfStr,PUnionUnionDeclareDef pdeclareDef)
{
	int	ret;

	if ((str == NULL) || (lenOfStr <= 0))
		return(0);
	memset(pdeclareDef,0,sizeof(*pdeclareDef));
	if (pdeclareDef == NULL)
	{
		UnionUserErrLog("in UnionReadUnionDeclareDefFromStr:: pdeclareDef is null!\n");
		return(errCodeParameter);
	}
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,UnionDefTagTypeDefName,pdeclareDef->typeDefName,sizeof(pdeclareDef->typeDefName))) < 0)		
	{
		UnionUserErrLog("in UnionReadUnionDeclareDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]\n",UnionDefTagTypeDefName,str);
		return(ret);
	}
	UnionReadRecFldFromRecStr(str,lenOfStr,UnionDefTagUnionName,pdeclareDef->unionName,sizeof(pdeclareDef->unionName));
	UnionReadRecFldFromRecStr(str,lenOfStr,UnionDefTagRemark,pdeclareDef->remark,sizeof(pdeclareDef->remark));
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
int UnionPrintUnionHeaderDeclareDefToFp(PUnionUnionDeclareDef pdef,FILE *fp)
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
	fprintf(outFp,"typedef union");
	if (strlen(pdef->unionName) != 0)
		fprintf(outFp," %s",pdef->unionName);
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
int UnionPrintUnionTailDeclareDefToFp(PUnionUnionDeclareDef pdef,FILE *fp)
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
int UnionPrintUnionDefToFp(PUnionUnionDef pdef,FILE *fp)
{
	int		fldNum;
	FILE		*outFp;
	
	if (fp == NULL)
		outFp = stdout;
	else
		outFp = fp;
	UnionPrintUnionHeaderDeclareDefToFp(&(pdef->declareDef),outFp);
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
	UnionPrintUnionTailDeclareDefToFp(&(pdef->declareDef),outFp);
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
int UnionPrintUnionDefInFileToFp(char *fileName,FILE *fp)
{
	TUnionUnionDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadUnionDefFromSpecFile(fileName,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintUnionDefInFileToFp:: UnionReadUnionDefFromSpecFile!\n");
		return(ret);
	}
	return(UnionPrintUnionDefToFp(&def,fp));
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
int UnionOutputUnionDefInFile(char *fileName)
{
	return(UnionPrintUnionDefInFileToFp(fileName,stdout));

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
int UnionPrintSpecUnionDefToFp(char *nameOfType,FILE *fp)
{
	TUnionUnionDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadUnionDefFromDefaultDefFile(nameOfType,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintSpecUnionDefToFp:: UnionReadUnionDefFromDefaultDefFile!\n");
		return(ret);
	}
	return(UnionPrintUnionDefToFp(&def,fp));
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
int UnionOutputSpecUnionDef(char *nameOfType)
{
	return(UnionPrintSpecUnionDefToFp(nameOfType,stdout));

}

