//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#include <stdio.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionVarTypeDef.h"
#include "unionEnumDef.h"
#include "UnionStr.h"

/* ��һ���ַ����ж�ȡö�������Ķ���
�������
	str		�������崮
	lenOfStr	�������崮�ĳ���
�������
	pdeclareDef	��������������
����ֵ��
	>=0 	������������Ŀ
	<0	�������	
	
*/
int UnionReadEnumDeclareDefFromStr(char *str,int lenOfStr,PUnionEnumDeclareDef pdeclareDef)
{
	int	ret;

	if ((str == NULL) || (lenOfStr <= 0))
		return(0);
	memset(pdeclareDef,0,sizeof(*pdeclareDef));
	if (pdeclareDef == NULL)
	{
		UnionUserErrLog("in UnionReadEnumDeclareDefFromStr:: pdeclareDef is null!\n");
		return(errCodeParameter);
	}
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conEnumDefTagTypeDefName,pdeclareDef->name,sizeof(pdeclareDef->name))) < 0)		
	{
		UnionUserErrLog("in UnionReadEnumDeclareDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]\n",conEnumDefTagTypeDefName,str);
		return(ret);
	}
	UnionReadRecFldFromRecStr(str,lenOfStr,conEnumDefTagRemark,pdeclareDef->remark,sizeof(pdeclareDef->remark));
	return(1);
}

/* ��ָ��ö�ٵ�����ͷ��ӡ���ļ���
�������
	pdef		��������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ȷ
	<0	�������	
	
*/
int UnionPrintEnumHeaderDeclareDefToFp(PUnionEnumDeclareDef pdef,FILE *fp)
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
	fprintf(outFp,"typedef enum\n");
	fprintf(outFp,"{\n");
	return(0);
}

/* ��ָ��ö�ٵ�����β��ӡ���ļ���
�������
	pdef		��������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ȷ
	<0	�������	
	
*/
int UnionPrintEnumTailDeclareDefToFp(PUnionEnumDeclareDef pdef,FILE *fp)
{
	FILE		*outFp;
	
	if (fp == NULL)
		outFp = stdout;
	else
		outFp = fp;
		
	if (pdef == NULL)
		return(0);

	fprintf(outFp,"} %s;\n",pdef->name);
	//fprintf(outFp,"typedef %s *%s;\n",pdef->name,pdef->name);
	return(0);
}

/* ��ָ��ö�ٵĶ����ӡ���ļ���
�������
	pdef	ö�ٶ���
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintEnumDefToFp(PUnionEnumDef pdef,FILE *fp)
{
	int		constNum;
	FILE		*outFp;
	
	if (fp == NULL)
		outFp = stdout;
	else
		outFp = fp;
	UnionPrintEnumHeaderDeclareDefToFp(&(pdef->declareDef),outFp);
	for (constNum = 0; constNum < pdef->constNum; constNum++)
	{
		fprintf(outFp,"        ");
		UnionPrintConstDefToFp(&(pdef->constGrp[constNum]),outFp);
		if (strlen(pdef->constGrp[constNum].remark) != 0)
			fprintf(outFp,"  // %s",pdef->constGrp[constNum].remark);
		fprintf(outFp,"\n");
	}
	UnionPrintEnumTailDeclareDefToFp(&(pdef->declareDef),outFp);
}

/* ��ָ���ļ��ж����ö�ٴ�ӡ���ļ���
�������
	fileName	�ļ�����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintEnumDefInFileToFp(char *fileName,FILE *fp)
{
	TUnionEnumDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadEnumDefFromSpecFile(fileName,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintEnumDefInFileToFp:: UnionReadEnumDefFromSpecFile!\n");
		return(ret);
	}
	return(UnionPrintEnumDefToFp(&def,fp));
}
	
/* ��ָ���ļ��ж����ö�ٴ�ӡ����Ļ��
�������
	fileName	�ļ�����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputEnumDefInFile(char *fileName)
{
	return(UnionPrintEnumDefInFileToFp(fileName,stdout));

}

/* ��ָ�����Ƶ�ö�ٶ���������ļ���
�������
	nameOfType	ö������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintSpecEnumDefToFp(char *nameOfType,FILE *fp)
{
	TUnionEnumDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadEnumDefFromDefaultDefFile(nameOfType,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintSpecEnumDefToFp:: UnionReadEnumDefFromDefaultDefFile!\n");
		return(ret);
	}
	return(UnionPrintEnumDefToFp(&def,fp));
}
	
/* ��ָ���ļ��ж����ö�ٴ�ӡ����Ļ��
�������
	nameOfType	ö������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputSpecEnumDef(char *nameOfType)
{
	return(UnionPrintSpecEnumDefToFp(nameOfType,stdout));

}

