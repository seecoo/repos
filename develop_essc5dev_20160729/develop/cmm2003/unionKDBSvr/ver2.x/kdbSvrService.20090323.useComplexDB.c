//	Author:		Wolfgang Wang
//	Date:		2007/8/3

// 2007/11/30���������º���
/*
// ����һ���첽����Կͬ������
int UnionApplyKeySynchronizeServiceNowait(TUnionKeyDBOperation operation,char keyDBType,unsigned char *keyRec,int sizeOfKeyRec);
// ��ȡһ����Կͬ������
int UnionReadKeySynchronizeServiceRequest(TUnionKeyDBOperation *operation,char *keyDBType,unsigned char *keyRec,int sizeOfRecBuf);
*/

/* 2007/12/06���޸������º�����
UnionSetKDBServiceResponse
�޸�֮ǰ����������ǵȴ���ʽ��ȡ��Ϣλ�ã���ȡʧ�ܣ��������ء�
�޸�֮�󣬻�ȡ��Ϣλ��ʧ�ܣ����ظ���ȡ��ֱ����ȡʱ�䳬��ָ��ֵ���ȡ��������ָ��ֵ���ŷ��ء�
*/

/* 2007/12/25����������������
	// ��ȡһ����Կͬ���������
	int UnionReadKeySynchronizeResultSpierRequest
	// ����һ���첽����Կͬ������������
	int UnionSpierKeySynchronizeResultNowait
  �޸��˺�����UnionClearKDBSvrBufRubbish
  	// �������Ӧ������Ϣ֮�⣬�����˶�������������Ĵ���
  		conKDBSvrKeySynchronizeRequest
  		conKDBSvrKeySynchronizeResultSpierRequest
*/

#define _UnionLogMDL_3_x_
#define _UnionEnv_3_x_

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <signal.h>

#include "UnionLog.h"
#include "unionModule.h"
#include "UnionEnv.h"
#include "unionCommand.h"
#include "unionIndexTBL.h"
#include "kdbSvrService.h"
#include "unionErrCode.h"
#include "unionREC.h"

#ifdef _useComplexDB_
#include "unionComplexDBRecord.h"
#include "unionUnionKDBSvrBuf.h"
#endif

PUnionKDBSvrBufHDL		pgunionKDBSvrBufHDL = NULL;
unsigned char			*pgunionKDBSvrBuf = NULL;
PUnionIndexStatusTBL		pgunionKDBSvrStatusTBL = NULL;
PUnionSharedMemoryModule	pgunionKDBSvrBufMDL = NULL;
unsigned int			gunionCurrentKDBRecIndex = 0;
TUnionKDBSvrServiceHeader	gunionKDBSvrServiceHeader;

PUnionKDBSvrServiceHeader UnionGetKDBSvrServiceHeaderOfIndex(int index)
{
	if (UnionConnectKDBSvrBufMDL() < 0)
		return(NULL);
	if ((index < 0) || (index >= pgunionKDBSvrBufHDL->maxNumOfRec))
		return(NULL);
	return((PUnionKDBSvrServiceHeader)(pgunionKDBSvrBuf + index * (sizeof(TUnionKDBSvrServiceHeader) + pgunionKDBSvrBufHDL->maxSizeOfRec)));
}

long UnionFormKDBSvrServiceUniqueKey(PUnionKDBSvrServiceHeader pheader)
{
	if (pheader == NULL)
		return(errCodeParameter);
	//return(pheader->provider * 100 + pheader->index % 100);	// 2009/3/23,ɾ����������
	return(pheader->provider); // 2009/3/23,������������
}

int UnionIsKDBSvrBufMDLConnected()
{
	if ((pgunionKDBSvrBufHDL == NULL) || (pgunionKDBSvrBuf == NULL) || (pgunionKDBSvrStatusTBL == NULL) || (!UnionIsSharedMemoryInited(conMDLNameOfKDBSvrBuf)))
		return(0);
	else
		return(1);
}

#ifndef _useComplexDB_
int UnionGetFileNameOfKDBSvrBufDef(char *fileName)
{
	sprintf(fileName,"%s/unionKDBSvrBuf.Def",getenv("UNIONETC"));
	return(0);
}
#else
int UnionGetFileNameOfKDBSvrBufDef(char *fileName)
{
	sprintf(fileName,"unionKDBSvrBuf");
	return(0);
}
#endif

int UnionGetTotalNumOfKDBSvrBufMDL()
{
	int	ret;
	
	if ((ret = UnionConnectKDBSvrBufMDL()) < 0)
		return(ret);
	return(pgunionKDBSvrBufHDL->maxNumOfRec);
}

int UnionGetTimeoutOfKDBSvrBufMDL()
{
	int	ret;
	
	if ((ret = UnionConnectKDBSvrBufMDL()) < 0)
		return(ret);
	return(pgunionKDBSvrBufHDL->timeout);
}

#ifndef _useComplexDB_
int UnionInitKDBSvrBufDef(PUnionKDBSvrBufHDL pdef)
{
	int		ret;
	char		fileName[512];
	char		*p;

	if (pdef == NULL)
		return(errCodeParameter);
			
	memset(fileName,0,sizeof(fileName));
	UnionGetFileNameOfKDBSvrBufDef(fileName);
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionInitKDBSvrBufDef:: UnionInitEnvi [%s]!\n",fileName);
		return(ret);
	}

	if ((p = UnionGetEnviVarByName("maxNumOfRec")) == NULL)
	{
		UnionUserErrLog("in UnionInitKDBSvrBufDef:: UnionGetEnviVarByName for [%s]\n!","maxNumOfRec");
		goto abnormalExit;
	}
	pdef->maxNumOfRec = atoi(p);
	
	if ((p = UnionGetEnviVarByName("timeout")) == NULL)
	{
		UnionUserErrLog("in UnionInitKDBSvrBufDef:: UnionGetEnviVarByName for [%s]\n!","timeout");
		goto abnormalExit;
	}
	pdef->timeout = atoi(p);
	
	if ((p = UnionGetEnviVarByName("maxSizeOfRec")) == NULL)
	{
		UnionUserErrLog("in UnionInitKDBSvrBufDef:: UnionGetEnviVarByName for [%s]\n!","maxSizeOfRec");
		goto abnormalExit;
	}
	pdef->maxSizeOfRec = atol(p);
	
	UnionClearEnvi();
	return(0);

abnormalExit:
	UnionClearEnvi();
	return(errCodeMsgBufMDL_InitDef);
}
#else
int UnionInitKDBSvrBufDef(PUnionKDBSvrBufHDL pdef)
{
	int	ret = 0;
	char szValue[128+1];

	if (pdef == NULL)
		return(errCodeParameter);

	memset(szValue, 0, 129 * sizeof(char));
	if( (ret = UnionReadUnionKDBSvrBufRecFld("maxNumOfRec", "propertyValue", szValue, 129)) < 0 )
	{
		UnionUserErrLog("in UnionInitKDBSvrBufDef:: UnionReadUnionKDBSvrBufRecFld maxNumOfRec\n");
		return (ret);
	}
	pdef->maxNumOfRec = atoi(szValue);

	memset(szValue, 0, 129 * sizeof(char));
	if( (ret = UnionReadUnionKDBSvrBufRecFld("timeout", "propertyValue", szValue, 129)) < 0 )
	{
		UnionUserErrLog("in UnionInitKDBSvrBufDef:: UnionReadUnionKDBSvrBufRecFld timeout\n");
		return (ret);
	}
	pdef->timeout = atoi(szValue);

	memset(szValue, 0, 129 * sizeof(char));
	if( (ret = UnionReadUnionKDBSvrBufRecFld("maxSizeOfRec", "propertyValue", szValue, 129)) < 0 )
	{
		UnionUserErrLog("in UnionInitKDBSvrBufDef:: UnionReadUnionKDBSvrBufRecFld maxSizeOfRec\n");
		return (ret);
	}
	pdef->maxSizeOfRec = atol(szValue);

	return(0);
}
#endif

int UnionReconnectKDBSvrBufMDLAnyway()
{
	PUnionSharedMemoryModule	pmdl;
	int				ret;
	int				maxNumOfRec;
	int				timeout;
	int				maxSizeOfRec;
	TUnionKDBSvrBufHDL		def;
		
	if (UnionIsKDBSvrBufMDLConnected())
		UnionDisconnectKDBSvrBufMDL();

	//UnionAuditLog("in UnionReconnectKDBSvrBufMDLAnyway:: entering ...\n");
	if ((ret = UnionInitKDBSvrBufDef(&def)) < 0)
	{
		UnionUserErrLog("in UnionReconnectKDBSvrBufMDLAnyway:: UnionInitKDBSvrBufDef!\n");
		return(ret);
	}
	
	if ((pgunionKDBSvrBufMDL = UnionConnectSharedMemoryModule(conMDLNameOfKDBSvrBuf,
			sizeof(*pgunionKDBSvrBufHDL) + 
			(sizeof(TUnionKDBSvrServiceHeader) + sizeof(unsigned char) * def.maxSizeOfRec) * def.maxNumOfRec)) == NULL)
	{
		UnionUserErrLog("in UnionReconnectKDBSvrBufMDLAnyway:: UnionConnectSharedMemoryModule!\n");
		return(errCodeSharedMemoryModule);
	}

	if ((pgunionKDBSvrBufHDL = (PUnionKDBSvrBufHDL)UnionGetAddrOfSharedMemoryModuleUserSpace(pgunionKDBSvrBufMDL)) == NULL)
	{
		UnionUserErrLog("in UnionReconnectKDBSvrBufMDLAnyway:: PUnionKDBSvrBufHDL!\n");
		return(errCodeSharedMemoryModule);
	}
	
	if ((pgunionKDBSvrBuf = (unsigned char *)((unsigned char *)pgunionKDBSvrBufHDL + sizeof(*pgunionKDBSvrBufHDL))) == NULL)
	{
		UnionUserErrLog("in UnionReconnectKDBSvrBufMDLAnyway:: unsigned char *!\n");
		return(errCodeSharedMemoryModule);
	}
	
	if (UnionIsNewCreatedSharedMemoryModule(pgunionKDBSvrBufMDL))
	{
		memcpy(pgunionKDBSvrBufHDL,&def,sizeof(def));
	}

	if ((pgunionKDBSvrStatusTBL = UnionConnectIndexStatusTBL(UnionGetUserIDOfSharedMemoryModule(pgunionKDBSvrBufMDL),
		pgunionKDBSvrBufHDL->maxNumOfRec)) == NULL)
	{
		UnionUserErrLog("in UnionReconnectKDBSvrBufMDLAnyway:: UnionConnectIndexStatusTBL!\n");
		return(errCodeMsgBufMDL_ConnectIndexTBL);
	}
	
	if (UnionIsNewCreatedSharedMemoryModule(pgunionKDBSvrBufMDL))
	{
		UnionResetAllKDBRecPos();
	}
	
	return(0);
}


int UnionConnectKDBSvrBufMDL()
{
	int	ret;
	
	if (UnionIsKDBSvrBufMDLConnected())
		return(0);

	if ((ret = UnionReconnectKDBSvrBufMDLAnyway()) < 0)
	{
		UnionUserErrLog("in UnionConnectKDBSvrBufMDL:: UnionReconnectKDBSvrBufMDLAnyway!\n");
		return(ret);
	}
	return(ret);
}	
	
int UnionDisconnectKDBSvrBufMDL()
{
	pgunionKDBSvrBuf = NULL;
	pgunionKDBSvrBufHDL = NULL;
	UnionDisconnectIndexStatusTBL(pgunionKDBSvrStatusTBL);
	return(UnionDisconnectShareModule(pgunionKDBSvrBufMDL));
}

int UnionRemoveKDBSvrBufMDL()
{
	pgunionKDBSvrBuf = NULL;
	pgunionKDBSvrBufHDL = NULL;
	if (pgunionKDBSvrBufMDL != NULL)
	{
		UnionRemoveIndexStatusTBL(UnionGetUserIDOfSharedMemoryModule(pgunionKDBSvrBufMDL));
	}
	pgunionKDBSvrBufMDL = NULL;
	return(UnionRemoveSharedMemoryModule(conMDLNameOfKDBSvrBuf));
}

int UnionReloadKDBSvrBufDef()
{
	int				ret;
	TUnionKDBSvrBufHDL		def;
	
	if ((ret = UnionConnectKDBSvrBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionReloadKDBSvrBufDef:: UnionConnectKDBSvrBufMDL!\n");
		return(ret);
	}
	return(UnionResetAllKDBRecPos());
}

int UnionApplyKDBService(TUnionKeyDBOperation operation,char keyDBType,unsigned char *keyRec,int sizeOfKeyRec)
{
	int				index;
	int				ret;
	PUnionKDBSvrServiceHeader	pheader;
	char				tmpBuf[512+1];
	time_t				reqTime;
	time_t				now;
	int				timeout;
		
	if ((ret = UnionConnectKDBSvrBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionApplyKDBService:: UnionConnectKDBSvrBufMDL!\n");
		return(ret);
	}	
	if ((keyRec == NULL) || (sizeOfKeyRec <= 0) || (sizeOfKeyRec >= pgunionKDBSvrBufHDL->maxSizeOfRec))
	{
		UnionUserErrLog("in UnionApplyKDBService:: wrong parameter! lenOfRec = [%04d]\n",sizeOfKeyRec);
		return(errCodeParameter);
	}
	// ��ȡ�洢λ��
	if (((index = UnionGetAvailableIndexNoWait(pgunionKDBSvrStatusTBL)) < 0) || (index >= pgunionKDBSvrBufHDL->maxNumOfRec))
	{
		UnionUserErrLog("in UnionApplyKDBService:: UnionGetAvailableIndexNoWait!\n");
		return(index);
	}
	// ��ֵ
	pheader = (PUnionKDBSvrServiceHeader)(pgunionKDBSvrBuf + conSizeOfEachKDBSvrRec * index);
	memset(pheader,0,sizeof(pheader));
	pheader->lenOfRec = sizeOfKeyRec;
	pheader->keyDBType = keyDBType;
	pheader->operation = operation;
	pheader->type = conKDBSvrRequest;
	pheader->applierIsKeyDBBackuper = UnionIsKeyDBBackupServer();
	pheader->index = ++gunionCurrentKDBRecIndex;
	UnionProgramerLog("%c %02d %d %12ld %12ld %7d %6d\n",pheader->keyDBType,pheader->operation,pheader->applierIsKeyDBBackuper,
			pheader->type,pheader->index,
			pheader->provider,pheader->lenOfRec);
	memcpy(pgunionKDBSvrBuf + conSizeOfEachKDBSvrRec * index + sizeof(TUnionKDBSvrServiceHeader),keyRec,pheader->lenOfRec);
	time(&(pheader->time));
	reqTime = pheader->time;
	pheader->provider = getpid();
	memcpy(&gunionKDBSvrServiceHeader,pheader,sizeof(gunionKDBSvrServiceHeader));
	// ���뻺����
	if ((ret = UnionSetIndexWithUserStatusNoWait(pgunionKDBSvrStatusTBL,index,conKDBSvrRequest)) < 0)
	{
		UnionUserErrLog("in UnionApplyKDBService:: UnionSetIndexWithUserStatusNoWait!\n");
		return(ret);
	}
	// ��ȡ��Ӧ
loopReadRes:
	// �ж��Ƿ�ʱ
	time(&now);
	if (now - reqTime >= pgunionKDBSvrBufHDL->timeout)
	{
		if (now - reqTime < pgunionKDBSvrBufHDL->timeout)
			goto loopReadRes;
		UnionUserErrLog("in UnionApplyKDBService:: read response for [%ld] timeout!\n",UnionFormKDBSvrServiceUniqueKey(&gunionKDBSvrServiceHeader));
		return(errCodeTimeout);
	}
	if ((index = UnionGetFirstIndexOfUserStatusUntilTimeout(pgunionKDBSvrStatusTBL,
			UnionFormKDBSvrServiceUniqueKey(&gunionKDBSvrServiceHeader),pgunionKDBSvrBufHDL->timeout - (now - reqTime))) < 0)
	{
		UnionUserErrLog("in UnionApplyKDBService:: UnionGetFirstIndexOfUserStatusUntilTimeout! [%ld]\n",UnionFormKDBSvrServiceUniqueKey(&gunionKDBSvrServiceHeader));
		return(index);
	}
	if (index >= pgunionKDBSvrBufHDL->maxNumOfRec)
	{
		goto loopReadRes;
	}
	pheader = (PUnionKDBSvrServiceHeader)(pgunionKDBSvrBuf + conSizeOfEachKDBSvrRec * index);
	if (reqTime != pheader->time)
	{
		UnionUserErrLog("in UnionApplyKDBService:: response [%ld] not for me! snap = [%ld]\n",UnionFormKDBSvrServiceUniqueKey(pheader),pheader->time-reqTime);
		memset(pheader,0,sizeof(*pheader));	// 2007/12/25����
		UnionSetIndexAvailable(pgunionKDBSvrStatusTBL,index);
		goto loopReadRes;
	}
	ret = pheader->lenOfRec;
	memset(pheader,0,sizeof(*pheader));
	UnionSetIndexAvailable(pgunionKDBSvrStatusTBL,index);
	return(ret);
}

int UnionReadKDBServiceRequest(TUnionKeyDBOperation *operation,char *keyDBType,unsigned char *keyRec,int sizeOfRecBuf)
{
	int				index;
	int				len;
	int				ret;
	PUnionKDBSvrServiceHeader	ptmpHeader;
	time_t				now;
	if ((ret = UnionConnectKDBSvrBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionReadKDBServiceRequest:: UnionConnectKDBSvrBufMDL!\n");
		return(ret);
	}	
	if ((keyRec == NULL) || (sizeOfRecBuf <= 0))
	{
		UnionUserErrLog("in UnionReadKDBServiceRequest:: wrong parameter!\n");
		return(errCodeParameter);
	}
loopRead:
	if ((index = UnionGetFirstIndexOfUserStatusUntilSuccess(pgunionKDBSvrStatusTBL,conKDBSvrRequest)) < 0)
	{
		UnionUserErrLog("in UnionReadKDBServiceRequest:: UnionGetFirstIndexOfUserStatus!\n");
		return(index);
	}
	if (index >= pgunionKDBSvrBufHDL->maxNumOfRec)
		goto loopRead;
	memcpy(&gunionKDBSvrServiceHeader,ptmpHeader = (PUnionKDBSvrServiceHeader)(pgunionKDBSvrBuf + conSizeOfEachKDBSvrRec * index),sizeof(*ptmpHeader));
	UnionProgramerLog("REQ::%c %02d %d %12ld %12ld %7d %7d %6d\n",ptmpHeader->keyDBType,ptmpHeader->operation,
			ptmpHeader->applierIsKeyDBBackuper,ptmpHeader->type,ptmpHeader->index,
			ptmpHeader->provider,ptmpHeader->dealer,ptmpHeader->lenOfRec);
	// �ж������Ƿ�ʱ
	time(&now);
	if (now - ptmpHeader->time >= pgunionKDBSvrBufHDL->timeout)
	{
		UnionUserErrLog("in UnionReadKDBServiceRequest:: service timeout %c %02d %12ld %7d %7d %6d %ld\n",
			ptmpHeader->keyDBType,ptmpHeader->operation,ptmpHeader->index,
			ptmpHeader->provider,ptmpHeader->dealer,ptmpHeader->lenOfRec,now - ptmpHeader->time);
		goto reRead;
	}
	if ((len = ptmpHeader->lenOfRec) <= 0)
	{
		goto reRead;
	}
	if (len >= sizeOfRecBuf)
	{
		UnionUserErrLog("in UnionReadKDBServiceRequest:: lenOfRec = [%04ld] too long! recKey = [%ld][%d]\n",len,
					gunionKDBSvrServiceHeader.index,gunionKDBSvrServiceHeader.provider);
		goto reRead;
	}
	memcpy(keyRec,pgunionKDBSvrBuf + conSizeOfEachKDBSvrRec * index + sizeof(TUnionKDBSvrServiceHeader),len);
	*operation = ptmpHeader->operation;
	*keyDBType = ptmpHeader->keyDBType;
	if (ptmpHeader->applierIsKeyDBBackuper)
		UnionSetAsKeyDBBackupServer();
	else
		UnionSetAsNonKeyDBBackupServer();
	memset(ptmpHeader,0,sizeof(*ptmpHeader));
	UnionSetIndexAvailable(pgunionKDBSvrStatusTBL,index);
	return(len);
reRead:
	memset(ptmpHeader,0,sizeof(*ptmpHeader));
	UnionSetIndexAvailable(pgunionKDBSvrStatusTBL,index);
	goto loopRead;
}

int UnionAnswerKDBService(int errCode)
{
	int				index;
	int				ret;
	PUnionKDBSvrServiceHeader	ptmpHeader;
	char				tmpBuf[512+1];
	time_t				now;
	int				retryTimes = 0;	// �������Դ���
		
	if ((ret = UnionConnectKDBSvrBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionSetKDBServiceResponse:: UnionConnectKDBSvrBufMDL!\n");
		return(ret);
	}
	/*
	if (kill(gunionKDBSvrServiceHeader.provider,0) != 0)
	{
		UnionUserErrLog("in UnionSetKDBServiceResponse:: applier [%d] not exists [%ld] [%c] [%d]\n",
			gunionKDBSvrServiceHeader.provider,gunionKDBSvrServiceHeader.index,
			gunionKDBSvrServiceHeader.keyDBType,gunionKDBSvrServiceHeader.operation);
		return(errCodeTimeout);
	}
	*/
loop:
	// ��ȡ�洢λ��, 2007/12/06�޸Ĵ˶�
	if (((index = UnionGetAvailableIndexNoWait(pgunionKDBSvrStatusTBL)) < 0) || (index >= pgunionKDBSvrBufHDL->maxNumOfRec))
	{
		time(&now);
		if ((now - gunionKDBSvrServiceHeader.time + 1 >= pgunionKDBSvrBufHDL->timeout) || (retryTimes >= 900))
		{
			UnionUserErrLog("in UnionSetKDBServiceResponse:: UnionGetAvailableIndexNoWait!\n");
			return(0-abs(index));
		}
		retryTimes++;
		usleep(10000);
		goto loop;
	}
	// 2007/12/06 �޸Ĵ˶ν���
	// ��ֵ
	ptmpHeader = (PUnionKDBSvrServiceHeader)(pgunionKDBSvrBuf + conSizeOfEachKDBSvrRec * index);
	memcpy(ptmpHeader,&gunionKDBSvrServiceHeader,sizeof(gunionKDBSvrServiceHeader));
	ptmpHeader->lenOfRec = errCode;
	ptmpHeader->dealer = getpid();
	ptmpHeader->type = UnionFormKDBSvrServiceUniqueKey(&gunionKDBSvrServiceHeader);
	UnionProgramerLog("RES::%c %02d %d %12ld %12ld %7d %7d %6d\n",ptmpHeader->keyDBType,ptmpHeader->operation,
			ptmpHeader->applierIsKeyDBBackuper,ptmpHeader->type,ptmpHeader->index,
			ptmpHeader->provider,ptmpHeader->dealer,ptmpHeader->lenOfRec);
	// ���뻺����
	if ((ret = UnionSetIndexWithUserStatusNoWait(pgunionKDBSvrStatusTBL,index,UnionFormKDBSvrServiceUniqueKey(&gunionKDBSvrServiceHeader))) < 0)
	{
		UnionUserErrLog("in UnionSetKDBServiceResponse:: UnionSetIndexWithUserStatusNoWait!\n");
		return(ret);
	}
	return(ret);
}

int UnionResetKDBServiceTimeout(int timeout)
{
	int				ret;
			
	if (timeout <= 0)
	{
		UnionUserErrLog("in UnionResetKDBServiceTimeout:: timeout = [%d] error!\n",timeout);
		return(errCodeParameter);
	}
	if ((ret = UnionConnectKDBSvrBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionResetKDBServiceTimeout:: UnionConnectKDBSvrBufMDL!\n");
		return(ret);
	}
	
	pgunionKDBSvrBufHDL->timeout = timeout;
	
	return(0);
}

int UnionPrintAvailablKDBSvrBufPosToFile(FILE *fp)
{
	int				index;
	PUnionKDBSvrServiceHeader	pheader;
	int				num;
	int				ret;
	
	if (fp == NULL)
		fp = stdout;

	if ((ret = UnionConnectKDBSvrBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionPrintAvailablKDBSvrBufPosToFile:: UnionConnectKDBSvrBufMDL!\n");
		return(ret);
	}	
	for (index = 0,num = 0; index < pgunionKDBSvrBufHDL->maxNumOfRec; index++)
	{
		if ((pheader = (PUnionKDBSvrServiceHeader)(pgunionKDBSvrBuf + conSizeOfEachKDBSvrRec * index)) == NULL)
			return(errCodeOutRange);
		if ((pheader->provider != 0) || (pheader->time != 0))
			continue;
		fprintf(fp,"%6d\n",index);
		num++;
	}
	fprintf(fp,"Total Available Num = [%d]\n",num);
	return(0);
}

int UnionPrintInavailabeKDBSvrBufPosToFile(FILE *fp)
{
	int				index;
	PUnionKDBSvrServiceHeader	pheader;
	int				num;
	int				ret;
	time_t				now;
	
	if (fp == NULL)
		return(errCodeParameter);

	if ((ret = UnionConnectKDBSvrBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionPrintInavailabeKDBSvrBufPosToFile:: UnionConnectKDBSvrBufMDL!\n");
		return(ret);
	}	
	time(&now);
	for (index = 0,num = 0; index < pgunionKDBSvrBufHDL->maxNumOfRec; index++)
	{
		if ((pheader = (PUnionKDBSvrServiceHeader)(pgunionKDBSvrBuf + conSizeOfEachKDBSvrRec * index)) == NULL)
			return(errCodeOutRange);
		if ((pheader->provider == 0) && (pheader->time == 0))
			continue;
		fprintf(fp,"%6d %c %2d %d %6d %6ld %12d %7d %7d\n",index,pheader->keyDBType,pheader->operation,
				pheader->applierIsKeyDBBackuper,pheader->lenOfRec,
				now - pheader->time,pheader->index,pheader->provider,pheader->dealer);
		num++;
		/*
		if ((num % 22 == 0) && (num != 0) && ((fp == stdout) || (fp == stderr))) 
		{
			if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit ...")))
				break;
		}
		*/
	}
	fprintf(fp,"Total Inavailable Num = [%d]\n",num);
	return(0);
}

int UnionPrintKDBSvrBufStatusToFile(FILE *fp)
{
	int	ret;
	
	if (fp == NULL)
		return(errCodeParameter);

	if ((ret = UnionConnectKDBSvrBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionPrintKDBSvrBufStatusToFile:: UnionConnectKDBSvrBufMDL!\n");
		return(ret);
	}	
	fprintf(fp,"\n");
	fprintf(fp,"maxSizeOfRec                [%ld]\n",pgunionKDBSvrBufHDL->maxSizeOfRec);
	fprintf(fp,"maxNumOfRec                 [%d]\n",pgunionKDBSvrBufHDL->maxNumOfRec);
	fprintf(fp,"timeout                     [%d]\n",pgunionKDBSvrBufHDL->timeout);
	fprintf(fp,"\n");
	//if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or quit/exit to exit...")))
	//	return(0);
	UnionPrintSharedMemoryModuleToFile(pgunionKDBSvrBufMDL,fp);
	return(0);
}

int UnionPrintKDBSvrBufToFile(FILE *fp)
{
	UnionPrintAvailablKDBSvrBufPosToFile(fp);
	UnionPrintInavailabeKDBSvrBufPosToFile(fp);
	UnionPrintKDBSvrBufStatusToFile(fp);
	return(0);
}

int UnionResetAllKDBRecPos()
{
	int	ret;
	int	index;
	PUnionKDBSvrServiceHeader 	pheader;
	
	if ((ret = UnionConnectKDBSvrBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionResetAllKDBRecPos:: UnionConnectKDBSvrBufMDL!\n");
		return(ret);
	}
	for (index = 0; index < pgunionKDBSvrBufHDL->maxNumOfRec; index++)
	{
		if ((pheader = (PUnionKDBSvrServiceHeader)(pgunionKDBSvrBuf + conSizeOfEachKDBSvrRec * index)) == NULL)
			continue;
		memset(pheader,0,sizeof(*pheader));
	}
	return(UnionResetAllIndexAvailable(pgunionKDBSvrStatusTBL));
}

int UnionClearKDBSvrBufRubbish()
{
	int				index;
	PUnionKDBSvrServiceHeader	pheader;
	int				num;
	time_t				nowTime;
	int				ret;
	long				status;
		
	if ((ret = UnionConnectKDBSvrBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionClearKDBSvrBufRubbish:: UnionConnectKDBSvrBufMDL!\n");
		return(ret);
	}	
	
	// ���������Ϣ
	time(&nowTime);
	for (index = 0,num = 0; index < pgunionKDBSvrBufHDL->maxNumOfRec; index++)
	{
		if ((pheader = (PUnionKDBSvrServiceHeader)(pgunionKDBSvrBuf + conSizeOfEachKDBSvrRec * index)) == NULL)
			return(errCodeMsgBufMDL_OutofRange);
		if ((pheader->type <= 0) || (pheader->provider <= 0) || (pheader->time <= 0))
			continue;
		if (nowTime - pheader->time < pgunionKDBSvrBufHDL->timeout)
			continue;
		UnionAuditLog("Rubbish::%6d %12ld %c %2d %d %6d %6ld %12d %7d %7d\n",index,pheader->type,pheader->keyDBType,
				pheader->operation,pheader->applierIsKeyDBBackuper,pheader->lenOfRec,
				nowTime - pheader->time,pheader->index,pheader->provider,pheader->dealer);
		if (pheader->type == conKDBSvrRequest)	// �����󣬲����д���
		{
			UnionAuditLog("Rubbish is requet! not clear it!\n");
			continue;
		}
		// 2007/12/25 �޸ģ�����Ӧ֮�⣬�����˶�conKDBSvrKeySynchronizeRequest��conKDBSvrKeySynchronizeResultSpierRequest�Ĵ���
		switch (pheader->type)
		{
			case	conKDBSvrKeySynchronizeRequest:
				status = conKDBSvrKeySynchronizeRequest;
				break;
			case	conKDBSvrKeySynchronizeResultSpierRequest:
				status = conKDBSvrKeySynchronizeResultSpierRequest;
				break;
			default:	// ����Ӧ�����д���
				status = UnionFormKDBSvrServiceUniqueKey(pheader);
				break;
		}
		if ((ret = UnionGetFirstIndexOfUserStatusNoWait(pgunionKDBSvrStatusTBL,status)) < 0)
		{
			UnionAuditLog("UnionGetFirstIndexOfUserStatusNoWait [%ld] failure! ret = [%d]\n",status,ret);
			continue;
		}
		if (ret != index)
		{
			UnionSetIndexWithUserStatus(pgunionKDBSvrStatusTBL,ret,status);
			UnionAuditLog("index = [%04d] != expected [%04d]\n",ret,index);
			continue;
		}
		// 2007/12/25�޸Ľ���
		memset(pheader,0,sizeof(*pheader));
		if ((ret = UnionSetIndexAvailable(pgunionKDBSvrStatusTBL,index)) < 0)
			UnionUserErrLog("in UnionClearKDBSvrBufRubbish:: UnionSetIndexAvailable [%04d]\n",index);
		else
		{
			UnionAuditLog("in UnionClearKDBSvrBufRubbish:: UnionSetIndexAvailable [%04d] OK!\n",index);			
			num++;
		}
			
	}
	if (num > 0)
		UnionAuditLog("in UnionClearKDBSvrBufRubbish:: [%d] Rubbish are cleared\n",num);
	return(0);
}

// 2007/11/30������
// ����һ���첽����Կͬ������
int UnionApplyKeySynchronizeServiceNowait(TUnionKeyDBOperation operation,char keyDBType,unsigned char *keyRec,int sizeOfKeyRec)
{
	int				index;
	int				ret;
	PUnionKDBSvrServiceHeader	pheader;
	char				tmpBuf[512+1];
	time_t				reqTime;
	time_t				now;
	int				timeout;
		
	if ((ret = UnionConnectKDBSvrBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionApplyKeySynchronizeServiceNowait:: UnionConnectKDBSvrBufMDL!\n");
		return(ret);
	}	
	if ((keyRec == NULL) || (sizeOfKeyRec <= 0) || (sizeOfKeyRec >= pgunionKDBSvrBufHDL->maxSizeOfRec))
	{
		UnionUserErrLog("in UnionApplyKeySynchronizeServiceNowait:: wrong parameter! lenOfRec = [%04d]\n",sizeOfKeyRec);
		return(errCodeParameter);
	}
	// ��ȡ�洢λ��
	if (((index = UnionGetAvailableIndexNoWait(pgunionKDBSvrStatusTBL)) < 0) || (index >= pgunionKDBSvrBufHDL->maxNumOfRec))
	{
		UnionUserErrLog("in UnionApplyKeySynchronizeServiceNowait:: UnionGetAvailableIndexNoWait!\n");
		return(index);
	}
	// ��ֵ
	pheader = (PUnionKDBSvrServiceHeader)(pgunionKDBSvrBuf + conSizeOfEachKDBSvrRec * index);
	memset(pheader,0,sizeof(pheader));
	pheader->lenOfRec = sizeOfKeyRec;
	pheader->keyDBType = keyDBType;
	pheader->operation = operation;
	pheader->type = conKDBSvrKeySynchronizeRequest;
	pheader->applierIsKeyDBBackuper = UnionIsKeyDBBackupServer();
	//pheader->index = ++gunionCurrentKDBRecIndex;
	pheader->index = gunionCurrentKDBRecIndex;
	UnionProgramerLog("syncReq::%c %02d %d %12ld %12ld %7d %6d\n",pheader->keyDBType,pheader->operation,pheader->applierIsKeyDBBackuper,
			pheader->type,pheader->index,
			pheader->provider,pheader->lenOfRec);
	memcpy(pgunionKDBSvrBuf + conSizeOfEachKDBSvrRec * index + sizeof(TUnionKDBSvrServiceHeader),keyRec,pheader->lenOfRec);
	time(&(pheader->time));
	reqTime = pheader->time;
	pheader->provider = getpid();
	// ���뻺����
	if ((ret = UnionSetIndexWithUserStatusNoWait(pgunionKDBSvrStatusTBL,index,conKDBSvrKeySynchronizeRequest)) < 0)
	{
		UnionUserErrLog("in UnionApplyKeySynchronizeServiceNowait:: UnionSetIndexWithUserStatusNoWait!\n");
		return(ret);
	}
	return(0);
}

// 2007/11/30������������
// ��ȡһ����Կͬ������
int UnionReadKeySynchronizeServiceRequest(TUnionKeyDBOperation *operation,char *keyDBType,unsigned char *keyRec,int sizeOfRecBuf)
{
	int				index;
	int				len;
	int				ret;
	PUnionKDBSvrServiceHeader	ptmpHeader;
	time_t				now;

	if ((ret = UnionConnectKDBSvrBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionReadKeySynchronizeServiceRequest:: UnionConnectKDBSvrBufMDL!\n");
		return(ret);
	}	
	if ((keyRec == NULL) || (sizeOfRecBuf <= 0))
	{
		UnionUserErrLog("in UnionReadKeySynchronizeServiceRequest:: wrong parameter!\n");
		return(errCodeParameter);
	}

loopRead:
	if ((index = UnionGetFirstIndexOfUserStatusUntilSuccess(pgunionKDBSvrStatusTBL,conKDBSvrKeySynchronizeRequest)) < 0)
	{
		UnionUserErrLog("in UnionReadKeySynchronizeServiceRequest:: UnionGetFirstIndexOfUserStatus!\n");
		return(index);
	}
	if (index >= pgunionKDBSvrBufHDL->maxNumOfRec)
		goto loopRead;
	memcpy(&gunionKDBSvrServiceHeader,ptmpHeader = (PUnionKDBSvrServiceHeader)(pgunionKDBSvrBuf + conSizeOfEachKDBSvrRec * index),sizeof(*ptmpHeader));
	UnionProgramerLog("REQ::%c %02d %d %12ld %12ld %7d %7d %6d\n",ptmpHeader->keyDBType,ptmpHeader->operation,
			ptmpHeader->applierIsKeyDBBackuper,ptmpHeader->type,ptmpHeader->index,
			ptmpHeader->provider,ptmpHeader->dealer,ptmpHeader->lenOfRec);
	// �ж������Ƿ�ʱ
	time(&now);
	if (now - ptmpHeader->time >= pgunionKDBSvrBufHDL->timeout)
	{
		UnionUserErrLog("in UnionReadKeySynchronizeServiceRequest:: service timeout %c %02d %12ld %7d %7d %6d %ld\n",
			ptmpHeader->keyDBType,ptmpHeader->operation,ptmpHeader->index,
			ptmpHeader->provider,ptmpHeader->dealer,ptmpHeader->lenOfRec,now - ptmpHeader->time);
		goto reRead;
	}
	if ((len = ptmpHeader->lenOfRec) <= 0)
	{
		goto reRead;
	}
	if (len >= sizeOfRecBuf)
	{
		UnionUserErrLog("in UnionReadKeySynchronizeServiceRequest:: lenOfRec = [%04ld] too long! recKey = [%ld][%d]\n",len,
					gunionKDBSvrServiceHeader.index,gunionKDBSvrServiceHeader.provider);
		goto reRead;
	}
	memcpy(keyRec,pgunionKDBSvrBuf + conSizeOfEachKDBSvrRec * index + sizeof(TUnionKDBSvrServiceHeader),len);
	*operation = ptmpHeader->operation;
	*keyDBType = ptmpHeader->keyDBType;
	/*
	if (ptmpHeader->applierIsKeyDBBackuper)
		UnionSetAsKeyDBBackupServer();
	else
		UnionSetAsNonKeyDBBackupServer();
	*/
	memset(ptmpHeader,0,sizeof(*ptmpHeader));
	UnionSetIndexAvailable(pgunionKDBSvrStatusTBL,index);
	return(len);
reRead:
	memset(ptmpHeader,0,sizeof(*ptmpHeader));
	UnionSetIndexAvailable(pgunionKDBSvrStatusTBL,index);
	goto loopRead;
}

// 2007/12/25������
// ����һ���첽����Կͬ������������
int UnionSpierKeySynchronizeResultNowait(char *ipAddr,int port,char *fullKeyName,TUnionKeyDBOperation operation,int resCode)
{
	int				index;
	int				ret;
	PUnionKDBSvrServiceHeader	pheader;
	char				tmpBuf[512+1];
	TUnionKeySynchronizeResult	result;
	
	if (operation == conCompareKeyValue)
	{
		if (UnionReadIntTypeRECVar("spyKeyCheckSynchOper") <= 0)	// ����ؼ����Կ�Ĳ���
			return(ret);
	}
	if ((ipAddr == NULL) || (port <= 0) || (fullKeyName == NULL))
	{
		UnionUserErrLog("in UnionSpierKeySynchronizeResultNowait:: wrong parameter!\n");
		return(errCodeParameter);
	}
	if (!UnionExistsTaskOfName(conTaskNameOfKDBSynchronizerSpier))
	{
		UnionAuditLog("in UnionSpierKeySynchronizeResultNowait:: task [%s] not exists, don't spy this result!\n",conTaskNameOfKDBSynchronizerSpier);
		return(0);
	}
	if ( (!UnionIsValidIPAddrStr(ipAddr)) || (strlen(fullKeyName) >= sizeof(result.fullKeyName)) )
	{
		UnionUserErrLog("in UnionSpierKeySynchronizeResultNowait:: ipAddr [%s] error or fullKeyName [%s] too long!\n",ipAddr,fullKeyName);
		return(errCodeParameter);
	}
	memset(&result,0,sizeof(result));
	strcpy(result.fullKeyName,fullKeyName);
	strcpy(result.ipAddr,ipAddr);
	result.port = port;
	result.operation = operation;
	result.resCode = resCode;
	UnionGetSystemDateTime(result.time);
	
	if ((ret = UnionConnectKDBSvrBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionSpierKeySynchronizeResultNowait:: UnionConnectKDBSvrBufMDL!\n");
		return(ret);
	}	
	// ��ȡ�洢λ��
	if (((index = UnionGetAvailableIndexNoWait(pgunionKDBSvrStatusTBL)) < 0) || (index >= pgunionKDBSvrBufHDL->maxNumOfRec))
	{
		UnionUserErrLog("in UnionSpierKeySynchronizeResultNowait:: UnionGetAvailableIndexNoWait!\n");
		return(index);
	}
	// ��ֵ
	pheader = (PUnionKDBSvrServiceHeader)(pgunionKDBSvrBuf + conSizeOfEachKDBSvrRec * index);
	memset(pheader,0,sizeof(pheader));
	pheader->lenOfRec = sizeof(result);
	pheader->keyDBType = -1;
	pheader->operation = operation;
	pheader->type = conKDBSvrKeySynchronizeResultSpierRequest;
	pheader->applierIsKeyDBBackuper = 0;	//UnionIsKeyDBBackupServer();
	pheader->index = gunionCurrentKDBRecIndex;
	//UnionProgramerLog("syncReq::%c %02d %d %12ld %12ld %7d %6d\n",pheader->keyDBType,pheader->operation,pheader->applierIsKeyDBBackuper,
	//		pheader->type,pheader->index,
	//		pheader->provider,pheader->lenOfRec);
	memcpy(pgunionKDBSvrBuf + conSizeOfEachKDBSvrRec * index + sizeof(TUnionKDBSvrServiceHeader),&result,sizeof(result));
	time(&(pheader->time));
	pheader->provider = getpid();
	// ���뻺����
	if ((ret = UnionSetIndexWithUserStatusNoWait(pgunionKDBSvrStatusTBL,index,conKDBSvrKeySynchronizeResultSpierRequest)) < 0)
	{
		UnionUserErrLog("in UnionSpierKeySynchronizeResultNowait:: UnionSetIndexWithUserStatusNoWait!\n");
		return(ret);
	}
	return(0);
}

// 2007/12/25������������
// ��ȡһ����Կͬ���������
int UnionReadKeySynchronizeResultSpierRequest(PUnionKeySynchronizeResult presult)
{
	int				index;
	int				len;
	int				ret;
	PUnionKDBSvrServiceHeader	ptmpHeader;
	time_t				now;
	
	if ((ret = UnionConnectKDBSvrBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionReadKeySynchronizeResultSpierRequest:: UnionConnectKDBSvrBufMDL!\n");
		return(ret);
	}	

loopRead:
	if ((index = UnionGetFirstIndexOfUserStatusUntilSuccess(pgunionKDBSvrStatusTBL,conKDBSvrKeySynchronizeResultSpierRequest)) < 0)
	{
		UnionUserErrLog("in UnionReadKeySynchronizeResultSpierRequest:: UnionGetFirstIndexOfUserStatus!\n");
		return(index);
	}
	if (index >= pgunionKDBSvrBufHDL->maxNumOfRec)
		goto loopRead;
	memcpy(&gunionKDBSvrServiceHeader,ptmpHeader = (PUnionKDBSvrServiceHeader)(pgunionKDBSvrBuf + conSizeOfEachKDBSvrRec * index),sizeof(*ptmpHeader));
	//UnionProgramerLog("REQ::%c %02d %d %12ld %12ld %7d %7d %6d\n",ptmpHeader->keyDBType,ptmpHeader->operation,
	//		ptmpHeader->applierIsKeyDBBackuper,ptmpHeader->type,ptmpHeader->index,
	//		ptmpHeader->provider,ptmpHeader->dealer,ptmpHeader->lenOfRec);
	if ((len = ptmpHeader->lenOfRec) != sizeof(*presult))
	{
		UnionUserErrLog("in UnionReadKeySynchronizeResultSpierRequest:: lenOfRec [%d] != expected [%d] error!\n",
			ptmpHeader->lenOfRec,sizeof(*presult));
		goto reRead;
	}
	if (presult != NULL)
		memcpy(presult,pgunionKDBSvrBuf + conSizeOfEachKDBSvrRec * index + sizeof(TUnionKDBSvrServiceHeader),sizeof(*presult));
	memset(ptmpHeader,0,sizeof(*ptmpHeader));
	UnionSetIndexAvailable(pgunionKDBSvrStatusTBL,index);
	return(0);
reRead:
	memset(ptmpHeader,0,sizeof(*ptmpHeader));
	UnionSetIndexAvailable(pgunionKDBSvrStatusTBL,index);
	goto loopRead;
}
