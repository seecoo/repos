// wolfang wang
// 2008/10/3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "unionErrCode.h"
#include "unionPackAndFunRelation.h"
#include "UnionLog.h"

/*
����	
	���һ�����ĺͺ�����Ӧ��ϵ��Ӧ�ķ������
�������
	appID		���ĵ����
	datagramID	���ĵı�ʶ
�������
	fileName	��õı��Ķ�����ļ�����
����ֵ
	>= 0	�ɹ�
	<0	�������
*/
int UnionGetServiceIDFromPackAndFunRelationDef(char *funName,char *serviceID)
{
	int	ret;
	TUnionPackAndFunRelation	def;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadSpecPackAndFunRelationFromFile(funName,&def)) < 0)
	{
		UnionUserErrLog("in UnionGetServiceIDFromPackAndFunRelationDef:: UnionReadSpecPackAndFunRelationFromFile!\n");
		return(ret);
	}
	if (serviceID != NULL)
		strcpy(serviceID,def.serviceID);
	return(0);
}


/*
����	
	���һ�����Ķ�����ļ���
�������
	appID		���ĵ����
	datagramID	���ĵı�ʶ
�������
	fileName	��õı��Ķ�����ļ�����
����ֵ
	��
*/
void UnionGetFileNameOfPackAndFunRelationDef(char *funName,char *fileName)
{
	UnionGetDefaultFileNameOfFunDef(funName,fileName);
	return;
}

/*
����	
	��һ�����ݴ��ж�ȡһ���������뺯��������Ӧ��ϵ����
�������
	str		���崮
	lenOfStr	������
�������
	pdef		�����ı������뺯��������Ӧ��ϵ����
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadPackFldAndFunVarRelationFromStr(char *str,int lenOfStr,PUnionPackFldAndFunVarRelation pdef)
{
	int	ret;

	if (pdef == NULL)
		return(errCodeParameter);
	memset(pdef,0,sizeof(*pdef));
	// �������ʶ
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conPackFldAndFunVarRelationTagPackFldID,pdef->packFldID,sizeof(pdef->packFldID))) < 0)
	{
		UnionUserErrLog("in UnionReadPackFldAndFunVarRelationFromStr:: UnionReadRecFldFromRecStr [%s]!\n",conPackFldAndFunVarRelationTagPackFldID);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadPackFldAndFunVarRelationFromStr:: conPackFldAndFunVarRelationTagPackFldID [%s]! not defined!\n",conPackFldAndFunVarRelationTagPackFldID);
		return(errCodeCDPMDL_DatagramFldTagNotDefined);
	}
	// ֵ����
	if (UnionReadRecFldFromRecStr(str,lenOfStr,conPackFldAndFunVarRelationTagPackFldType,pdef->fldType,sizeof(pdef->fldType)) < 0)
		strcpy(pdef->fldType,"string");
	// ����ֵ��ʶ
	UnionReadRecFldFromRecStr(str,lenOfStr,conPackFldAndFunVarRelationTagVarValueTag,pdef->varValueTag,sizeof(pdef->varValueTag));
	// ˵��
	UnionReadRecFldFromRecStr(str,lenOfStr,conPackFldAndFunVarRelationTagRemark,pdef->remark,sizeof(pdef->remark));
	// �Ƿ��Ǳ����������������ֵ
	UnionReadIntTypeRecFldFromRecStr(str,lenOfStr,conPackFldAndFunVarRelationTagIsPackToFun,&(pdef->isPackToFun));
	// ���򳤶ȵ�ָʾ������ֵ
	UnionReadRecFldFromRecStr(str,lenOfStr,conPackFldAndFunVarRelationTagVarNameOfReaLen,pdef->varNameOfRealLen,sizeof(pdef->varNameOfRealLen));
	// ���Ƿ��ѡ��
	if (UnionReadIntTypeRecFldFromRecStr(str,lenOfStr,conPackFldAndFunVarRelationTagIsOptional,&(pdef->isOptional)) < 0)
		pdef->isOptional = 0;
	return(0);
}

/*
����	
	�ӱ��ж�ȡһ�������뺯����ת����ϵ
�������
	funName		��������
	datagramID	���ĵı�ʶ
�������
	pdef		�����ı��Ķ���
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadSpecPackAndFunRelationFromFile(char *funName,PUnionPackAndFunRelation pdef)
{
	char			fileName[256+1];
	FILE			*fp;
	int			lineLen;
	char			lineStr[1024+1];
	int			lineNum = 0;
	int			ret;
	
	if ((funName == NULL) || (pdef == NULL))
	{
		UnionUserErrLog("in UnionReadSpecPackAndFunRelationFromFile:: null parameter!\n");
		return(errCodeParameter);
	}
	
	// �򿪶����ļ�
	memset(fileName,0,sizeof(fileName));
	UnionGetFileNameOfPackAndFunRelationDef(funName,fileName);
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadSpecPackAndFunRelationFromFile:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	// ���ļ��ж�ȡ����
	memset(pdef,0,sizeof(*pdef));
	while (!feof(fp))
	{
		if (pdef->relationNum >= conMaxNumOfPackFldAndFunVarRelation)
		{
			UnionUserErrLog("in UnionReadSpecPackAndFunRelationFromFile:: too many datagram fld [%d] defined!\n",pdef->relationNum);
			ret = errCodeCDPMDL_TooManyRequestDatagramFld;
			goto errExit;
		}
		memset(lineStr,0,sizeof(lineStr));
		if ((lineLen = UnionReadOneDataLineFromTxtFile(fp,lineStr,sizeof(lineStr))) < 0)
		{
			if (lineLen == errCodeFileEnd)
				continue;
			UnionUserErrLog("in UnionReadSpecPackAndFunRelationFromFile:: UnionReadOneDataLineFromTxtFile! lineNum = [%04d] ret = [%d]\n",lineNum,lineLen);
			ret = lineLen;
			goto errExit;
		}
		lineNum++;
		if (lineLen == 0)	// ����
			continue;
		if (strncmp(lineStr,conPackAndFunRelatioTagServiceID,strlen(conPackAndFunRelatioTagServiceID)) == 0)
		{
			if ((ret = UnionReadRecFldFromRecStr(lineStr,lineLen,conPackAndFunRelatioTagServiceID,pdef->serviceID,sizeof(pdef->serviceID))) < 0)
			{
				UnionUserErrLog("in UnionReadPackFldAndFunVarRelationFromStr:: UnionReadRecFldFromRecStr [%s]!\n",conPackAndFunRelatioTagServiceID);
				goto errExit;
			}
			continue;
		}
		if (strncmp(lineStr,conPackAndFunRelatioTagFunAndPack,strlen(conPackAndFunRelatioTagFunAndPack)) != 0)
			continue;
		// ����ϵ����
		if ((ret = UnionReadPackFldAndFunVarRelationFromStr(lineStr+strlen(conPackAndFunRelatioTagFunAndPack),lineLen-strlen(conPackAndFunRelatioTagFunAndPack),&(pdef->relationGrp[pdef->relationNum]))) < 0)
		{
			UnionUserErrLog("in UnionReadSpecPackAndFunRelationFromFile:: UnionReadPackFldAndFunVarRelationFromStr [%s]! lineNum = [%04d]\n",lineStr,lineNum);
			goto errExit;
		}
		pdef->relationNum += 1;
	}
	ret = 0;
errExit:
	fclose(fp);
	return(ret);
}

/*
����	
	��ӡһ���������뺯������ת����ϵ
�������
	pdef		Ҫ��ӡ�ı������뺯��������Ӧ��ϵ����
	fp		������ļ�ָ��
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintPackFldAndFunVarRelationToFp(PUnionPackFldAndFunVarRelation pdef,FILE *fp)
{
	FILE	*outFp;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp == NULL)
		outFp = stdout;
	else
		outFp = fp;
	
	fprintf(outFp,"%d %d %30s %30s %10s %s %s\n",pdef->isPackToFun,pdef->isOptional,pdef->packFldID,pdef->varValueTag,pdef->fldType,
			pdef->remark,pdef->varNameOfRealLen);
	return(0);
}

/*
����	
	��ӡһ�������뺯����ת����ϵ
�������
	pdef		Ҫ��ӡ�ı��Ķ���
	fp		������ļ�ָ��
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintPackAndFunRelationToFp(PUnionPackAndFunRelation pdef,FILE *fp)
{
	FILE	*outFp = stdout;
	int	relationNum;
	int	num = 0;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp != NULL)
		outFp = fp;
	
	fprintf(outFp,"���Ĵ���::%s\n",pdef->serviceID);
	fprintf(outFp,"��������::\n");
	for (relationNum = 0,num = 0; relationNum < pdef->relationNum; relationNum++)
	{
		if (pdef->relationGrp[relationNum].isPackToFun)
			continue;
		UnionPrintPackFldAndFunVarRelationToFp(&(pdef->relationGrp[relationNum]),outFp);
		num++;
	}
	if (num == 0)
		fprintf(outFp,"��\n");
	fprintf(outFp,"��Ӧ����::\n");
	for (relationNum = 0,num = 0; relationNum < pdef->relationNum; relationNum++)
	{
		if (!pdef->relationGrp[relationNum].isPackToFun)
			continue;
		UnionPrintPackFldAndFunVarRelationToFp(&(pdef->relationGrp[relationNum]),outFp);
		num++;
	}
	if (num == 0)
		fprintf(outFp,"��\n");	
	return(0);
}

/*
����	
	��ӡһ�������뺯����ת����ϵ
�������
	funName		��������
	fp		������ļ�ָ��
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintSpecPackAndFunRelationToFp(char *funName,FILE *fp)
{
	TUnionPackAndFunRelation	def;
	int			ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadSpecPackAndFunRelationFromFile(funName,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintSpecPackAndFunRelationToFp:: UnionReadSpecPackAndFunRelationFromFile!\n");
		return(ret);
	}
	return(UnionPrintPackAndFunRelationToFp(&def,fp));
}

/*
����	
	��ӡһ�������뺯����ת����ϵ��ָ���ļ�
�������
	funName		��������
	fileName	�ļ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintSpecPackAndFunRelationToSpecFile(char *funName,char *fileName)
{
	FILE	*fp = stdout;
	int	ret;
	
	if ((fileName != NULL) && (strlen(fileName) != 0) && (strcmp(fileName,"null") != 0))
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionPrintSpecPackAndFunRelationToSpecFile:: fopen [%s]!\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	if ((ret = UnionPrintSpecPackAndFunRelationToFp(funName,fp)) < 0)
	{
		UnionUserErrLog("in UnionPrintSpecPackAndFunRelationToSpecFile:: UnionPrintSpecPackAndFunRelationToFp!\n");
	}
	if (fp != stdout)
		fclose(fp);
	return(ret);	
}

