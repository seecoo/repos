// Author:	������
// Date:	2010-6-2

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>

#include "UnionStr.h"
#include "unionREC.h"
#include "UnionLog.h"
#include "unionErrCode.h"

#include "mngSvrAutoAppOnlineLogWriter.h"

/*
����
	ƴװ��ָ�������γɵļ�¼��
���������
	pwriterDef	��������
	methodTag	������ʶ
	sizeOfBuf	��������С
�������
	recStr		��¼��
����ֵ
	>=0	�ɹ�����¼���ĳ���
	<0	������
*/
int UnionFormAutoAppOnlineLogRecStr(PUnionAutoAppOnlineLogWriter pwriterDef,int methodTag,char *recStr,int sizeOfBuf)
{
	int				index;
	int				ret;
	char				tmpBuf[4196+1];
	PUnionAutoAppOnlineLogFldDef	pfldDef;
	int				lenOfRecStr = 0;
	char				method[128+1];
	int				fldLen;
	
	if (pwriterDef == NULL)
		return(errCodeParameter);
	
	for (index = 0; index < pwriterDef->fldNum; index++)
	{
		pfldDef = &(pwriterDef->desTableFldGrp[index]);
		if (pfldDef->methodTag != methodTag)
			continue;
		// ��ȡ��ֵ
		memset(tmpBuf,0,sizeof(tmpBuf));
		switch (pfldDef->methodTag)
		{
			case	conAutoAppOnlineLogFldMethodReadReqStrFld:
				fldLen = UnionReadFldFromCurrentMngSvrClientReqStr(pfldDef->oriFldName,tmpBuf,sizeof(tmpBuf));
				break;
			case	conAutoAppOnlineLogFldMethodReadResStrFld:
				fldLen = UnionReadFldFromCurrentMngSvrClientResStr(pfldDef->oriFldName,tmpBuf,sizeof(tmpBuf));
				break;
			default:
				continue;
		}				
		if (fldLen < 0)
		{
			UnionUserErrLog("in UnionFormAutoAppOnlineLogRecStr:: UnionReadValueUsingSpecMethod method = [%d] fldName = [%s]\n",pfldDef->methodTag,pfldDef->oriFldName);
			if (methodTag == conAutoAppOnlineLogFldMethodReadReqStrFld)
				return(fldLen);
			else
				continue;
		}
		if ((ret = UnionPutRecFldIntoRecStr(pfldDef->desFldName,tmpBuf,fldLen,recStr+lenOfRecStr,sizeOfBuf-lenOfRecStr)) < 0)
		{
			UnionUserErrLog("in UnionFormAutoAppOnlineLogRecStr:: UnionPutRecFldIntoRecStr fldName = [%s] [%04d][%s]\n",pfldDef->desFldName,fldLen,tmpBuf);
			return(ret);
		}
		lenOfRecStr += ret;
	}
	return(lenOfRecStr);
}

/*
����
	ƴװ��¼����
���������
	pwriterDef	��������
	sizeOfBuf	��������С
�������
	recStr		��¼��
����ֵ
	>=0	�ɹ�����¼���ĳ���
	<0	������
*/
int UnionFormAutoAppOnlineLogRecStrForRequest(PUnionAutoAppOnlineLogWriter pwriterDef,char *recStr,int sizeOfBuf)
{
	return(UnionFormAutoAppOnlineLogRecStr(pwriterDef,conAutoAppOnlineLogFldMethodReadReqStrFld,recStr,sizeOfBuf));
}

/*
����
	ƴװ��¼��Ӧ��
���������
	pwriterDef	��������
	sizeOfBuf	��������С
�������
	recStr		��¼��
����ֵ
	>=0	�ɹ�����¼���ĳ���
	<0	������
*/
int UnionFormAutoAppOnlineLogRecStrForResponse(PUnionAutoAppOnlineLogWriter pwriterDef,char *recStr,int sizeOfBuf)
{
	return(UnionFormAutoAppOnlineLogRecStr(pwriterDef,conAutoAppOnlineLogFldMethodReadResStrFld,recStr,sizeOfBuf));
}

/*
����
	��һ��������ȡ��ֵ����
���������
	fldDefStr	��ֵ��
	lenOfFldDefStr	��ֵ������
�������
	pfldDef		��ֵ����
����ֵ
	>=0	�ɹ�,�������������
	<0	������
*/
int UnionInitAutoAppOnlineLogFldWriterFromDefStr(char *fldDefStr,int lenOfFldDefStr,PUnionAutoAppOnlineLogFldDef pfldDef)
{
	int	ret;
	char	fldGrp[3][128];
	int	fldNum;
	int	fldIndex = 0;
	
	if (pfldDef == NULL)
		return(errCodeNullPointer);

	memset(fldGrp,0,sizeof(fldGrp));
	if ((fldNum = UnionSeprateVarStrIntoVarGrp(fldDefStr,lenOfFldDefStr,'.',fldGrp,3)) < 0)
	{
		UnionUserErrLog("in UnionInitAutoAppOnlineLogFldWriterFromDefStr:: UnionSeprateVarStrIntoVarGrp [%04d][%s]!\n",lenOfFldDefStr,fldDefStr);
		return(fldNum);
	}
	if (fldNum == 0)
		return(0);
	fldGrp[0][40] = 0;
	strcpy(pfldDef->desFldName,fldGrp[0]);
	if (fldNum == 1)	// ֻ��������ˮ�������
	{
		pfldDef->methodTag = conAutoAppOnlineLogFldMethodReadReqStrFld;
		strcpy(pfldDef->oriFldName,pfldDef->desFldName);
		return(1);
	}
	
	if (strlen(fldGrp[1]) == 0)	// ����ȱʡ��ȡֵ����
		strcpy(fldGrp[1],"0");
	if (!UnionIsDigitStr(fldGrp[1]))	// �ڶ�������Դ��Ķ���
	{
		if ((strcmp(fldGrp[1],"reqFld") == 0) || (strcmp(fldGrp[1],"mngSvrReqFld") == 0))
			pfldDef->methodTag = conAutoAppOnlineLogFldMethodReadReqStrFld;
		else if ((strcmp(fldGrp[1],"resFld") == 0) || (strcmp(fldGrp[1],"mngSvrResFld") == 0))
			pfldDef->methodTag = conAutoAppOnlineLogFldMethodReadResStrFld;
		else if ((strcmp(fldGrp[1],"tblFld") == 0) || (strcmp(fldGrp[1],"tableFld") == 0))
			pfldDef->methodTag = conAutoAppOnlineLogFldMethodReadTableFld;
		else
		{
			pfldDef->methodTag = conAutoAppOnlineLogFldMethodReadReqStrFld;
			fldGrp[1][40] = 0;
			strcpy(pfldDef->oriFldName,fldGrp[1]);
			return(1);
		}
	}
	else
	{
		// �ڶ�������ȡֵ����
		pfldDef->methodTag = atoi(fldGrp[1]);
	}
	if (fldNum == 2)	// δ����Դ��
	{
		strcpy(pfldDef->oriFldName,pfldDef->desFldName);
		return(1);
	}
	fldGrp[2][40] = 0;
	strcpy(pfldDef->oriFldName,fldGrp[2]);
	return(1);
}

/*
����
	������Ȩ��¼��ʼ���Զ���־������
���������
	serviceID	��������
�������
	pwriterDef	��־����������
����ֵ
	>=0	�ɹ�,���������Ŀ
	<0	������
*/
int UnionInitAutoAppOnlineLogWriterOnAuthRec(PUnionOperationAuthorization pauthRec,PUnionAutoAppOnlineLogWriter pwriterDef)
{
	int	ret;
	int	index;
	char	fldGrp[conMaxFldNumOfAutoAppOnlineLogRec][128];
	int	fldNum;
	
	if ((pauthRec == NULL) || (pwriterDef == NULL))
	{
		UnionUserErrLog("in UnionInitAutoAppOnlineLogWriterOnAuthRec:: null pointer!\n");
		return(errCodeNullPointer);
	}
	if ((strlen(pauthRec->onlineLogTableName) == 0) || (strlen(pauthRec->fldAssignDef) == 0))	// ���������������Զ���ˮ�Ǽ�
	{
		return(0);
	}
	memset(pwriterDef,0,sizeof(*pwriterDef));
	strcpy(pwriterDef->serviceName,pauthRec->serviceName);
	strcpy(pwriterDef->oriTableName,pauthRec->tableName);
	strcpy(pwriterDef->desTableName,pauthRec->onlineLogTableName);
	// �۷���ˮ�������
	memset(fldGrp,0,sizeof(fldGrp));
	if ((fldNum = UnionSeprateVarStrIntoVarGrp(pauthRec->fldAssignDef,strlen(pauthRec->fldAssignDef),',',fldGrp,conMaxFldNumOfAutoAppOnlineLogRec)) < 0)
	{
		UnionUserErrLog("in UnionInitAutoAppOnlineLogWriterOnAuthRec:: UnionSeprateVarStrIntoVarGrp [%s]!\n",pauthRec->fldAssignDef);
		return(fldNum);
	}
	if (fldNum > conMaxFldNumOfAutoAppOnlineLogRec)
		fldNum = conMaxFldNumOfAutoAppOnlineLogRec;
	// ��һ������ĸ�ֵ����
	for (index = 0; index < fldNum; index++)
	{
		if ((ret = UnionInitAutoAppOnlineLogFldWriterFromDefStr(fldGrp[index],strlen(fldGrp[index]),&(pwriterDef->desTableFldGrp[index]))) < 0)
		{
			UnionUserErrLog("in UnionInitAutoAppOnlineLogWriterOnAuthRec:: UnionInitAutoAppOnlineLogFldWriterFromDefStr [%s] of index [%d] of [%s]!\n",fldGrp[index],index,pauthRec->fldAssignDef);
			return(ret);
		}
	}
	pwriterDef->fldNum = fldNum;
	return(fldNum);
}

