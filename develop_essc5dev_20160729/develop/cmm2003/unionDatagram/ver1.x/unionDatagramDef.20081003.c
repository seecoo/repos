// wolfang wang
// 2008/10/3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "unionErrCode.h"
#include "unionDatagramDef.h"
#include "UnionLog.h"

/*
����	
	��һ�����ݴ��ж�ȡһ�����ĵ�ͷ����
�������
	str		���崮
	lenOfStr	������
�������
	serviceID	����ͷ�еķ������
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadDatagramHeaderFromStr(char *str,int lenOfStr,char *serviceID)
{
	int	ret;
	
	// �������
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conDatagramTagServiceID,serviceID,100)) < 0)
	{
		UnionUserErrLog("in UnionReadDatagramHeaderFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]!\n",conDatagramTagServiceID,str);
		return(ret);
	}
	return(ret);
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
void UnionGetFileNameOfDatagramDef(char *appID,char *datagramID,char *fileName)
{
	sprintf(fileName,"%s/datagram/%s/%s.def",getenv("UNIONDESIGNDIR"),appID,datagramID);
}

/*
����	
	��һ�����ݴ��ж�ȡһ����������
�������
	str		���崮
	lenOfStr	������
�������
	pdef		�����ı�������
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadDatagramFldDefFromStr(char *str,int lenOfStr,PUnionDatagramFldDef pdef)
{
	int	ret;
	
	if (pdef == NULL)
		return(errCodeParameter);
	memset(pdef,0,sizeof(*pdef));
	// ���ʶ
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conDatagramFldDefTagFldID,pdef->fldID,sizeof(pdef->fldID))) < 0)
	{
		UnionUserErrLog("in UnionReadDatagramFldDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]!\n",conDatagramFldDefTagFldID,str);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadDatagramFldDefFromStr:: conDatagramFldDefTagFldID [%s]! not defined!\n",conDatagramFldDefTagFldID);
		return(errCodeCDPMDL_DatagramFldTagNotDefined);
	}
	// ����
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conDatagramFldDefTagAlais,pdef->alais,sizeof(pdef->alais))) < 0)
	{
		UnionUserErrLog("in UnionReadDatagramFldDefFromStr:: UnionReadRecFldFromRecStr [%s]  from [%s]!\n",conDatagramFldDefTagAlais,str);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadDatagramFldDefFromStr:: conDatagramFldDefTagFldID [%s]! not defined!\n",conDatagramFldDefTagFldID);
		return(errCodeCDPMDL_DatagramFldAlaisNotDefined);
	}
	// ����
	if ((ret = UnionReadIntTypeRecFldFromRecStr(str,lenOfStr,conDatagramFldDefTagLen,&(pdef->len))) < 0)
		pdef->len = 2048;
	if (pdef->len == 0)
		pdef->len = 2048;
	// ��ֵ����
	UnionReadRecFldFromRecStr(str,lenOfStr,conDatagramFldDefTagFldValueID,pdef->valueID,sizeof(pdef->valueID));
	// ��˵��
	UnionReadRecFldFromRecStr(str,lenOfStr,conDatagramFldDefTagRemark,pdef->remark,sizeof(pdef->remark));
	// �������ʶ
	if ((ret = UnionReadIntTypeRecFldFromRecStr(str,lenOfStr,conDatagramFldDefTagIsRequest,&(pdef->isRequest))) < 0)
		pdef->isRequest = 1;
	UnionReadIntTypeRecFldFromRecStr(str,lenOfStr,conDatagramFldDefTagOptional,&(pdef->optional));
	return(0);
}

/*
����	
	�ӱ��ж�ȡһ�����ĵĶ���
�������
	appID		���ĵ����
	datagramID	���ĵı�ʶ
�������
	pdef		�����ı��Ķ���
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadSpecDatagramDefFromFile(char *appID,char *datagramID,PUnionDatagramDef pdef)
{
	char			fileName[256+1];
	FILE			*fp;
	int			lineLen;
	char			lineStr[1024+1];
	int			lineNum = 0;
	int			ret;
	
	if ((appID == NULL) || (datagramID == NULL) || (pdef == NULL))
	{
		UnionUserErrLog("in UnionReadSpecDatagramDefFromFile:: null parameter!\n");
		return(errCodeParameter);
	}
	
	// �򿪶����ļ�
	memset(fileName,0,sizeof(fileName));
	UnionGetFileNameOfDatagramDef(appID,datagramID,fileName);
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadSpecDatagramDefFromFile:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	// ���ļ��ж�ȡ����
	memset(pdef,0,sizeof(*pdef));
	while (!feof(fp))
	{
		if (pdef->fldNum >= conMaxNumOfFldPerDatagram)
		{
			UnionUserErrLog("in UnionReadSpecDatagramDefFromFile:: too many datagram fld [%d] defined!\n",pdef->fldNum);
			ret = errCodeCDPMDL_TooManyRequestDatagramFld;
			goto errExit;
		}
		memset(lineStr,0,sizeof(lineStr));
		if ((lineLen = UnionReadOneDataLineFromTxtFile(fp,lineStr,sizeof(lineStr))) < 0)
		{
			if (lineLen == errCodeFileEnd)
				continue;
			UnionUserErrLog("in UnionReadSpecDatagramDefFromFile:: UnionReadOneDataLineFromTxtFile! lineNum = [%04d] ret = [%d]\n",lineNum,lineLen);
			ret = lineLen;
			goto errExit;
		}
		lineNum++;
		if (lineLen == 0)	// ����
			continue;
		if (strncmp(lineStr,conDatagramTagHeader,strlen(conDatagramTagHeader)) == 0)
		{
			// ����ͷ
			if ((ret = UnionReadDatagramHeaderFromStr(lineStr+strlen(conDatagramTagHeader),lineLen-strlen(conDatagramTagHeader),pdef->serviceID)) < 0)
			{
				UnionUserErrLog("in UnionReadSpecDatagramDefFromFile:: UnionReadDatagramHeaderFromStr from [%s]!\n",lineStr+strlen(conDatagramTagHeader));
				goto errExit;
			}
			continue;
		}			
		// ����
		if ((ret = UnionReadDatagramFldDefFromStr(lineStr,lineLen,&(pdef->fldGrp[pdef->fldNum]))) < 0)
		{
			UnionUserErrLog("in UnionReadSpecDatagramDefFromFile:: UnionReadDatagramFldDefFromStr [%s]! lineNum = [%04d]\n",lineStr,lineNum);
			goto errExit;
		}
		pdef->fldNum += 1;
	}
	ret = 0;
	strcpy(pdef->appID,appID);
	strcpy(pdef->datagramID,datagramID);
	if (strlen(pdef->serviceID) == 0)
	{
		UnionUserErrLog("in UnionReadSpecDatagramDefFromFile:: header defined error!\n");
		ret = errCodeCDPMDL_DatagramHeaderDefinedError;
	}
errExit:
	fclose(fp);
	return(ret);
}

/*
����	
	��ӡһ�����ĵ�����
�������
	pdef		Ҫ��ӡ�ı�������
	fp		������ļ�ָ��
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintDatagramFldDefToFp(PUnionDatagramFldDef pdef,FILE *fp)
{
	FILE	*outFp;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp == NULL)
		outFp = stdout;
	else
		outFp = fp;
	
	//fprintf(outFp,"isRequest=%d|fldID=%s|valueID=%s|remark=%s|\n",pdef->isRequest,pdef->fldID,pdef->valueID,pdef->remark);
	fprintf(outFp,"%24s %16s %4d %16s",pdef->fldID,pdef->alais,pdef->len,pdef->valueID);
	if (pdef->optional)
		fprintf(outFp," ��ѡ");
	else
		fprintf(outFp," ����");
	fprintf(outFp," %s\n",pdef->remark);
	return(0);
}

/*
����	
	��ӡһ�����ĵĶ���
�������
	pdef		Ҫ��ӡ�ı��Ķ���
	fp		������ļ�ָ��
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintDatagramDefToFp(PUnionDatagramDef pdef,FILE *fp)
{
	FILE	*outFp = stdout;
	int	fldNum;
	int	num = 0;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp != NULL)
		outFp = fp;
	
	//fprintf(outFp,"appID=%s|datagramID=%s|\n",pdef->appID,pdef->datagramID);
	fprintf(outFp,"������;::%s ���Ĺ���::%s\n",pdef->appID,pdef->datagramID);
	//fprintf(outFp,"idOfApp=%s|serviceID=%s|\n",pdef->idOfAppInDatagram,pdef->serviceID);
	fprintf(outFp,"�������::%s\n",pdef->serviceID);
	//fprintf(outFp,"fldNum=%d\n",pdef->fldNum);
	fprintf(outFp,"%24s %16s %4s %16s %4s %s\n","���ʶ","����","����","ȡֵ����","��ѡ","˵��");
	fprintf(outFp,"��������::\n");
	for (fldNum = 0,num = 0; fldNum < pdef->fldNum; fldNum++)
	{
		if (!pdef->fldGrp[fldNum].isRequest)
			continue;
		UnionPrintDatagramFldDefToFp(&(pdef->fldGrp[fldNum]),outFp);
		num++;
	}
	if (num == 0)
		fprintf(outFp,"��\n");
	fprintf(outFp,"��Ӧ����::\n");
	for (fldNum = 0,num = 0; fldNum < pdef->fldNum; fldNum++)
	{
		if (pdef->fldGrp[fldNum].isRequest)
			continue;
		UnionPrintDatagramFldDefToFp(&(pdef->fldGrp[fldNum]),outFp);
		num++;
	}
	if (num == 0)
		fprintf(outFp,"��\n");	
	return(0);
}

/*
����	
	��ӡһ�����ĵĶ���
�������
	appID		������;��ʶ
	datagramID	���ĵ����
	fp		������ļ�ָ��
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintSpecDatagramDefToFp(char *appID,char *datagramID,FILE *fp)
{
	TUnionDatagramDef	def;
	int			ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadSpecDatagramDefFromFile(appID,datagramID,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintSpecDatagramDefToFp:: UnionReadSpecDatagramDefFromFile!\n");
		return(ret);
	}
	return(UnionPrintDatagramDefToFp(&def,fp));
}

/*
����	
	��ӡһ�����ĵĶ��嵽ָ���ļ�
�������
	appID		������;��ʶ
	datagramID	���ĵ����
	fileName	�ļ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintSpecDatagramDefToSpecFile(char *appID,char *datagramID,char *fileName)
{
	FILE	*fp = stdout;
	int	ret;
	
	if ((fileName != NULL) && (strlen(fileName) != 0) && (strcmp(fileName,"null") != 0))
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionPrintSpecDatagramDefToSpecFile:: fopen [%s]!\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	if ((ret = UnionPrintSpecDatagramDefToFp(appID,datagramID,fp)) < 0)
	{
		UnionUserErrLog("in UnionPrintSpecDatagramDefToSpecFile:: UnionPrintSpecDatagramDefToFp!\n");
	}
	if (fp != stdout)
		fclose(fp);
	return(ret);	
}

