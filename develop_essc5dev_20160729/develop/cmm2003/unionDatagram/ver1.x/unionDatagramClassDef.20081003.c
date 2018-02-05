// wolfang wang
// 2008/10/3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "unionErrCode.h"
#include "unionDatagramClassDef.h"

#include "UnionLog.h"
/*
����	
	���Ӧ�ñ������Ͷ�����ļ���
�������
	��
�������
	fileName	��õ�Ӧ�ñ������Ͷ�����ļ�����
����ֵ
	��
*/
void UnionGetFileNameOfDatagramClassDef(char *fileName)
{
	sprintf(fileName,"%s/datagram/datagramClass.def",getenv("UNIONDESIGNDIR"));
}

/*
����	
	���Ӧ�ñ������Ͷ�Ӧ�ķ����ģ���ʶ
�������
	appID			Ӧ�ñ�ʶ
�������
	nameOfModuleOfSvr	�����ģ���ʶ
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGetNameOfModuleSvrOfSpecDatagramClass(char *appID,char *nameOfModuleOfSvr)
{
	int	ret;
	TUnionDatagramClassDef	def;
	
	if ((ret = UnionReadDatagramClassDefFromFile(appID,&def)) < 0)
	{
		UnionUserErrLog("in UnionGetNameOfModuleSvrOfSpecDatagramClass:: UnionReadDatagramClassDefFromFile!\n");
		return(ret);
	}
	if (nameOfModuleOfSvr != NULL)
		strcpy(nameOfModuleOfSvr,def.nameOfModuleOfSvr);
	return(0);
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
int UnionReadDatagramClassDefFromStr(char *str,int lenOfStr,PUnionDatagramClassDef pdef)
{
	int	ret;
	char	classID[128+1];
	
	memset(pdef,0,sizeof(*pdef));

	// Ӧ�ñ�ʶ
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conDatagramClassDefTagAppID,pdef->appID,sizeof(pdef->appID))) < 0)
	{
		UnionUserErrLog("in UnionReadDatagramClassDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]!\n",conDatagramClassDefTagAppID,str);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadDatagramClassDefFromStr:: appID must be defined!\n");
		return(errCodeCDPMDL_DatagramClassNotDefined);
	}
	memset(classID,0,sizeof(classID));
	// ��������
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conDatagramClassDefTagClassID,pdef->classID,sizeof(pdef->classID))) < 0)
	{
		UnionUserErrLog("in UnionReadDatagramClassDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]!\n",conDatagramClassDefTagClassID,str);
		return(ret);
	}
	// ˵��
	UnionReadRecFldFromRecStr(str,lenOfStr,conDatagramClassDefTagRemark,pdef->remark,sizeof(pdef->remark));
	// svr��ģ��
	UnionReadRecFldFromRecStr(str,lenOfStr,conDatagramClassDefTagNameOfModuleOfSvr,pdef->nameOfModuleOfSvr,sizeof(pdef->nameOfModuleOfSvr));
	return(0);
}


/* ָ�����Ƶı������ͱ�ʶ�Ķ���
�������
	appID	ָ��������
�������
	pdef	���������ͱ�ʶ����
����ֵ��
	>=0 	���������ͱ�ʶ�Ĵ�С
	<0	�������	
	
*/
int UnionReadDatagramClassDefFromFile(char *appID,PUnionDatagramClassDef pdef)
{
	char	lineStr[1024+1];
	int	lineLen;
	int	ret;
	FILE	*fp;
	char	fileName[256+1];
	
	
	if (pdef == NULL)
	{
		UnionUserErrLog("in UnionReadDatagramClassDefFromFile:: parameter!\n");
		return(errCodeParameter);
	}
	UnionGetFileNameOfDatagramClassDef(fileName);
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadDatagramClassDefFromFile:: fopen [%s] error!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	memset(pdef,0,sizeof(*pdef));
	while (!feof(fp))
	{
		// ���ļ��ж�ȡһ��
		memset(lineStr,0,sizeof(lineStr));
		if ((lineLen = UnionReadOneDataLineFromTxtFile(fp,lineStr,sizeof(lineStr))) < 0)
		{
			if ((lineLen == errCodeEnviMDL_NullLine) || (lineLen == errCodeFileEnd))	// ����
				continue;
			UnionSystemErrLog("in UnionReadDatagramClassDefFromFile:: UnionReadOneDataLineFromTxtFile error in [%s]\n",fileName);
			fclose(fp);
			return(lineLen);
		}
		if (lineLen == 0)	// ����
			continue;
		if ((ret = UnionReadDatagramClassDefFromStr(lineStr,lineLen,pdef)) < 0)
			continue;
		if (strcmp(appID,pdef->appID) == 0)
		{
			fclose(fp);
			return(0);
		}
	}
	fclose(fp);
	UnionUserErrLog("in UnionReadDatagramClassDefFromFile:: [%s] not defined in [%s]\n",appID,fileName);
	return(errCodeCDPMDL_DatagramClassNotDefined);
}

/*
����	
	��ӡһ�����ĵ����Ͷ���
�������
	pdef		Ҫ��ӡ�ı������Ͷ���
	fp		������ļ�ָ��
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintDatagramClassDefToFp(PUnionDatagramClassDef pdef,FILE *fp)
{
	FILE	*outFp;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp == NULL)
		outFp = stdout;
	else
		outFp = fp;
	
	//fprintf(outFp,"appID=%s|classID=%s|remark=%s|\n",pdef->appID,pdef->classID,pdef->remark);
	fprintf(outFp,"%24s %20s %30s %s \n",pdef->appID,pdef->classID,pdef->nameOfModuleOfSvr,pdef->remark);
	return(0);
}

/*
����	
	��ӡһ���������͵Ķ���
�������
	appID		������;��ʶ
	fp		������ļ�ָ��
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintSpecDatagramClassDefToFp(char *appID,FILE *fp)
{
	TUnionDatagramClassDef	def;
	int			ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadDatagramClassDefFromFile(appID,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintSpecDatagramClassDefToFp:: UnionReadDatagramClassDefFromFile!\n");
		return(ret);
	}
	return(UnionPrintDatagramClassDefToFp(&def,fp));
}

/*
����	
	��ӡһ���������͵Ķ���
�������
	appID		������;��ʶ
	fileName	��������ļ�
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintSpecDatagramClassDef(char *appID,char *fileName)
{
	FILE	*fp = stdout;
	int	ret;
	
	if ((fileName != NULL) && (strlen(fileName) != 0) && (strcmp(fileName,"null") != 0))
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionPrintSpecDatagramClassDef:: fopen [%s]!\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	if ((ret = UnionPrintSpecDatagramClassDefToFp(appID,fp)) < 0)
	{
		UnionUserErrLog("in UnionPrintSpecDatagramClassDef:: UnionPrintSpecDatagramClassDefToFp!\n");
	}
	if (fp != stdout)
		fclose(fp);
	return(ret);	
}
