//	Author:		Wolfgang Wang
//	Copyright:	Union Tech. Guangzhou
//	Date:		2004/11/20

// 2007/8/7 ��keyDBBackuper.20041120.c����������

// 2007/11/28 ****
/*
// 2007/11/28����
// ��ָ���ı��ݷ������Ƚ���Կֵ
int UnionCompareDesKeyValueWithSpecBrother(char *ipAddr,char *fullKeyName,char *keyByLmk);
// 2007/11/28����
// �����еı��ݷ������Ƚ���Կֵ
int UnionCompareDesKeyValueWithBrothers(char *fullKeyName,char *keyByLmk);
*/

// 2007/11/30 �޸���ͬ����Կʱ����ʾ��ԭ��ֻ��ʾͬ������̨����������������ͬʱ��ʾͬ����������Կ���ơ�

// 2007/12/11 �޸�
/* UnionSynchronizeKeyDBOperationToSpecBrother:
	1���������뱸�ݷ�������ͨѶ����ʧ��ʱ������̨������״̬Ϊ���״̬
	2�����ӱ��ݷ�����������Ӧ�����޸�֮ǰ�ķ����ν��գ���Ϊ1�ν��ա�
   UnionReadSynchronizingKeyDBOperation:
   	1������������������˳�ʱ���ơ�
   ���ӣ�
   UnionMaintainStatusOfKeyDBBrothers
   	��鱸�ݷ�������״̬
   UnionCountAllUnsameDesKeyBrothers
	ͳ��������Կֵ����ͬ�ı��ݷ�����������ֵ�ǲ�ͬ���ı��ݷ�����������
   UnionIsCreateSckToKeyDBBackuperFailure
	�ж��Ƿ������뱸�ݷ�������������ʧ�ܵ����
   UnionResetCreateSckToKeyDBBackuperFailure
   	���뱸�ݷ�������������ʧ�ܴ���Ϊ0
*/

// 2007/12/25�޸�
/*
	�޸��� UnionSynchronizeKeyDBOperationToSpecBrother ������������е����� UnionSpierKeySynchronizeResultNowait
		�����Կͬ��������
	�޸��� UnionSynchronizeKeyDBOperation,�޸�֮ǰ�������Ƿ�ͬ����Կ���жϣ������ж��Ƿ�����ȱʡ������֮�����ڸ�Ϊ��
		�ж��Ƿ�����ȱʡ������֮ǰ��
*/

/* 2007/12/27�޸�
	�ڳ����������˱��뿪��ѡ��_LINUX_�����ڿ�����linux�£�����sigsetjmp��siglongjmp��������������ϵͳ�£��Ե���setjmp/longjmp
*/
#ifndef _keyDBBackuper_2_x_
#define _keyDBBackuper_2_x_
#endif

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <setjmp.h>	// 2007/12/11 ����

#include "unionModule.h"
#include "UnionStr.h"
#define _UnionEnv_3_x_
#include "UnionEnv.h"
#include "unionDesKey.h"
#include "unionDesKeyDBLog.h"
#include "unionPK.h"

#include "keyDBBackuper.h"
#include "unionErrCode.h"
#include "UnionLog.h"

PUnionSharedMemoryModule	pgunionKeyDBBackupServerMDL = NULL;
PUnionKeyDBBackupServer		pgunionKeyDBBackupServer = NULL;

int				gisKeyDBBackupServer = 0;
//int				gsckConnToBrotherBackupServer = -1;
char				gunionDefaultDBBackuperBrother[15+1] = "";
int				gunionIsDefaultDBBackuperBrotherUsed = 0;

jmp_buf				gsjl06CommJmpEnv;	// 2007/12/11����
void UnionDealKeyDBBackuperTimeout();			//2007/12/11����
int				gunionIsCreateSckToKeyDBBackuperFailure = 0;	// 2007/12/11����

// 2007/12/11����
// �ж��Ƿ������뱸�ݷ�������������ʧ�ܵ����
int UnionIsCreateSckToKeyDBBackuperFailure()
{
	return(gunionIsCreateSckToKeyDBBackuperFailure);
}

// 2007/12/11 ����
// ���뱸�ݷ�������������ʧ�ܴ���Ϊ0
void UnionResetCreateSckToKeyDBBackuperFailure()
{
	gunionIsCreateSckToKeyDBBackuperFailure = 0;
}

// 2007/8/8 ����
int UnionSetDefaultDBBackuperBrother(char *ipAddr)
{
	gunionIsDefaultDBBackuperBrotherUsed = 0;
	memset(gunionDefaultDBBackuperBrother,0,sizeof(gunionDefaultDBBackuperBrother));
	if (!UnionIsValidIPAddrStr(ipAddr))
	{
		UnionUserErrLog("in UnionSetDefaultDBBackuperBrother:: invalid ipAddr [%s]\n",ipAddr);
		return(errCodeInvalidIPAddr);
	}
	gunionIsDefaultDBBackuperBrotherUsed = 1;
	strcpy(gunionDefaultDBBackuperBrother,ipAddr);
	return(0);
}

PUnionKeyDBBackuper UnionFindDBBackuperBrother(char *ipAddr)
{
	int	i;
	int	ret;
	
	if ((ret = UnionConnectKeyDBBackupServer()) < 0)
	{
		UnionUserErrLog("in UnionFindDBBackuperBrother:: UnionConnectKeyDBBackupServer!\n");
		return(NULL);
	}
	
	for (i = 0; i < pgunionKeyDBBackupServer->realNum; i++)
	{
		if (strcmp(pgunionKeyDBBackupServer->brother[i].ipAddr,ipAddr) == 0)
			return(&(pgunionKeyDBBackupServer->brother[i]));
	}
	return(NULL);
}	

int UnionSetSpecDBBackuperBrotherStatus(char *ipAddr,int status)
{
	PUnionKeyDBBackuper	pbrother;
	
	if ((pbrother = UnionFindDBBackuperBrother(ipAddr)) == NULL)
	{
		UnionUserErrLog("in UnionSetSpecDBBackuperBrotherStatus:: UnionFindDBBackuperBrother [%s]!\n",ipAddr);
		return(errCodeKeyDBBackuperMDL_BrotherNotDefined);
	}
	pbrother->active = status;
	return(0);
}

int UnionSetSpecDBBackuperBrotherActive(char *ipAddr)
{
	return(UnionSetSpecDBBackuperBrotherStatus(ipAddr,1));
}

int UnionSetSpecDBBackuperBrotherInactive(char *ipAddr)
{
	return(UnionSetSpecDBBackuperBrotherStatus(ipAddr,0));
}

int UnionSynchronizeKeyDBOperationToSpecBrother(char *ipAddr,TUnionKeyDBOperation operation,char keyDBType,unsigned char *keyRec,int sizeOfKeyRec)
{
	int			ret;
	int			sck = -1;
	unsigned char		tmpBuf[512+1];
	int			len;
	TUnionKeyDBBackuperRecordHeader	header;
	PUnionKeyDBBackuper	pbrother;
	char			fullName[100+1];
	
	if ((gisKeyDBBackupServer) || (keyRec == NULL) || (sizeOfKeyRec <= 0))
		return(0);

	if ((pbrother = UnionFindDBBackuperBrother(ipAddr)) == NULL)
	{
		UnionUserErrLog("in UnionSynchronizeKeyDBOperationToSpecBrother:: UnionFindDBBackuperBrother [%s]!\n",ipAddr);
		return(errCodeKeyDBBackuperMDL_BrotherNotDefined);
	}
	
	// 2007/12/25������δ���Ӻ��Ƶ��˴�
	memset(fullName,0,sizeof(fullName));
	switch (keyDBType)
	{
		case	conIsDesKeyDB:
			memcpy(fullName,((PUnionDesKey)keyRec)->fullName,sizeof(((PUnionDesKey)keyRec)->fullName));
			break;
		case	conIsPKDB:
			memcpy(fullName,((PUnionPK)keyRec)->fullName,sizeof(((PUnionPK)keyRec)->fullName));
			break;
		default:
			break;
	}
	// 2007/12/25���ƶ�����
	
	// 2007/12/11 �������´���
	if (!pbrother->active)	// �����ݵ����������
	{	
		ret = errCodeKeyDBBackuperMDL_BrotherNotActive; // 2007/12/25����
		goto synchronizeFailure;
	}
	// 2007/12/11 ���ӽ�����
	
	if ((sck = UnionCreateSocketClient(pbrother->ipAddr,pbrother->port)) < 0)
	{
		UnionUserErrLog("in UnionSynchronizeKeyDBOperationToSpecBrother:: UnionCreateSocketClient [%s] [%d]!\n",
				pbrother->ipAddr,pbrother->port);
		ret = sck;
		// 2007/12/11 �������´���
		pbrother->active = 0;
		if (ret == errCodeSocketMDL_Timeout)
			gunionIsCreateSckToKeyDBBackuperFailure = 1;
		// 2007/12/11 ���ӽ�����
		goto synchronizeFailure;
	}

	memset(tmpBuf,0,sizeof(tmpBuf));
	tmpBuf[0] = (sizeOfKeyRec + sizeof(header))/256;
	tmpBuf[1] = (sizeOfKeyRec + sizeof(header))%256;
	memset(&header,0,sizeof(header));
	header.keyDBType = keyDBType;
	header.operation = operation;
	header.lenOfKeyRec = sizeOfKeyRec;
	memcpy(tmpBuf+2,&header,sizeof(header));
	if ((ret = UnionSendToSocket(sck,tmpBuf,2+sizeof(header))) < 0)
	{
		UnionUserErrLog("in UnionSynchronizeKeyDBOperationToSpecBrother:: first UnionSendToSocket!\n");
		goto synchronizeFailure;
	}
	if ((ret = UnionSendToSocket(sck,keyRec,sizeOfKeyRec)) < 0)
	{
		UnionUserErrLog("in UnionSynchronizeKeyDBOperationToSpecBrother:: first UnionSendToSocket!\n");
		goto synchronizeFailure;
	}
	// 2007/12/11����д�����´��룬δ��д֮ǰ�����Ƚ��ճ��ȣ��ٽ������ݡ���д֮�󣬸�Ϊһ�ν���
	if ((ret = UnionReceiveFromSocket(sck,tmpBuf,sizeof(tmpBuf))) < 4)
	{
		UnionUserErrLog("in UnionSynchronizeKeyDBOperationToSpecBrother::UnionReceiveFromSocketUntilLen Error!\n");
		if (ret >= 0)
			ret = errCodeSocketMDL_ReceiveLen;
		goto synchronizeFailure;
	}
	if ((len = tmpBuf[0] * 256 + tmpBuf[1]) != ret - 2)	// �жϽ��յĳ����Ƿ���ȷ
	{
		UnionUserErrLog("in UnionSynchronizeKeyDBOperationToSpecBrother::answer len [%d] != real send [%d]!\n",len,ret-2);
		ret = errCodeSocketMDL_ReceiveLen;
		goto synchronizeFailure;
	}
	if (strncmp((char *)tmpBuf+2,"00",2) != 0)
	{
		tmpBuf[2+2] = 0;
		UnionUserErrLog("in UnionSynchronizeKeyDBOperationToSpecBrother::answer from backup server = [%s] Error!\n",tmpBuf+2);
		ret = errCodeKeyDBBackuperMDL_BrotherReturnFailure;
		goto synchronizeFailure;
	}
	// 2007/12/11����д����
	ret = 0;
synchronizeFailure:
	// 2007/11/30 �޸�������Σ�wolfgang wang
	if (ret != 0)
	{
		if (operation == conCompareKeyValue)
			UnionUserErrLog("check to [%15s] [%d] Err:: [%s]!\n",pbrother->ipAddr,pbrother->port,fullName);
		else
			UnionUserErrLog("synchronize to [%15s] [%d] Err:: [%s]!\n",pbrother->ipAddr,pbrother->port,fullName);
	}		
	else
	{
		if (operation == conCompareKeyValue)
			UnionAuditLog("check to [%15s] [%d] OK :: [%s]!\n",pbrother->ipAddr,pbrother->port,fullName);
		else
			UnionAuditLog("synchronize to [%15s] [%d] OK :: [%s]!\n",pbrother->ipAddr,pbrother->port,fullName);
	}
	// 2007/11/30 �޸Ľ���
	if (sck >= 0)
		UnionCloseSocket(sck);
	// 2007/12/25���ӣ����ͬ������
	UnionSpierKeySynchronizeResultNowait(pbrother->ipAddr,pbrother->port,fullName,operation,ret);
	// 2007/12/25���ӽ���
	return(ret);
}
// 20070808 �������

int UnionIsKeyDBBackupServer()
{
	return(gisKeyDBBackupServer);
}

int UnionSetAsKeyDBBackupServer()
{
	gisKeyDBBackupServer = 1;
	return(0);
}

int UnionSetAsNonKeyDBBackupServer()
{
	gisKeyDBBackupServer = 0;
	return(0);
}

int UnionGetNameOfKeyDBBackupServer(char *fileName)
{
	sprintf(fileName,"%s/unionKeyDBBackupServer.Def",getenv("UNIONETC"));
	return(0);
}

int UnionIsKeyDBBackupServerConnected()
{
	// 2007/10/26�޸�
	if ((pgunionKeyDBBackupServer == NULL) || (!UnionIsSharedMemoryInited(conMDLNameOfUnionKeyDBBackupServer)))
	//if ((pgunionKeyDBBackupServerMDL == NULL) || (pgunionKeyDBBackupServer == NULL))
		return(0);
	else
		return(1);
}

// Module Layer Functions
int UnionConnectKeyDBBackupServer()
{
	int	ret;
	
	if (UnionIsKeyDBBackupServerConnected())
		return(0);
		
	if ((pgunionKeyDBBackupServerMDL = UnionConnectSharedMemoryModule(conMDLNameOfUnionKeyDBBackupServer,
			sizeof(TUnionKeyDBBackupServer))
			) == NULL)
	{
		UnionUserErrLog("in UnionConnectKeyDBBackupServer:: UnionConnectSharedMemoryModule!\n");
		return(errCodeSharedMemoryModule);
	}
	if ((pgunionKeyDBBackupServer = (PUnionKeyDBBackupServer)UnionGetAddrOfSharedMemoryModuleUserSpace(pgunionKeyDBBackupServerMDL)) == NULL)
	{
		UnionUserErrLog("in UnionConnectKeyDBBackupServer:: PUnionKeyDBBackupServer!\n");
		UnionRemoveKeyDBBackupServerInMemory();
		return(errCodeSharedMemoryModule);
	}
	
	if (UnionIsNewCreatedSharedMemoryModule(pgunionKeyDBBackupServerMDL))
	{
		memset(pgunionKeyDBBackupServer,0,sizeof(*pgunionKeyDBBackupServer));
		if ((ret = UnionLoadKeyDBBackupServerIntoMemory()) < 0)
		{
			UnionUserErrLog("in UnionConnectKeyDBBackupServer:: UnionLoadKeyDBBackupServerIntoMemory!\n");
			UnionRemoveKeyDBBackupServerInMemory();
		}
		return(ret);
	}
	else
		return(0);
}

int UnionDisconnectKeyDBBackupServer()
{
	pgunionKeyDBBackupServer = NULL;
	
	return(UnionDisconnectShareModule(pgunionKeyDBBackupServerMDL));
}

int UnionRemoveKeyDBBackupServerInMemory()
{
	UnionDisconnectKeyDBBackupServer();
	return(UnionRemoveSharedMemoryModule(conMDLNameOfUnionKeyDBBackupServer));
}

int UnionLoadKeyDBBackupServerIntoMemory()
{
	int				ret;
	FILE				*fp;
	char				fileName[256];
	char				*p;
	int				index;
	int				num = 0;
	char				varName[40];
		
	if ((ret = UnionConnectKeyDBBackupServer()) < 0)
	{
		UnionUserErrLog("in UnionLoadKeyDBBackupServerIntoMemory:: UnionConnectKeyDBBackupServer!\n");
		return(ret);
	}
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfKeyDBBackupServer(fileName);
        if ((ret = UnionInitEnvi(fileName)) < 0)
        {
                UnionUserErrLog("in UnionLoadKeyDBBackupServerIntoMemory:: UnionInitEnvi [%s]!\n",fileName);
                return(ret);
        }

	if ((p = UnionGetEnviVarByName("active")) == NULL)
	{
		UnionUserErrLog("in UnionLoadKeyDBBackupServerIntoMemory:: UnionGetEnviVarByName for [%s]\n!","active");
		ret = errCodeKeyDBBackuperMDL_ConfError;
		goto exitNormally;
	}
	pgunionKeyDBBackupServer->active = atoi(p);
	
	memset(&(pgunionKeyDBBackupServer->myself),0,sizeof(pgunionKeyDBBackupServer->myself));
	if ((index = UnionGetVarIndexOfTheVarName("myself")) < 0)
	{
		UnionUserErrLog("in UnionLoadKeyDBBackupServerIntoMemory:: UnionGetEnviVarByName for [%s]\n!","myself");
		ret = errCodeKeyDBBackuperMDL_ConfError;
		goto exitNormally;
	}
	if (!UnionIsValidIPAddrStr(p = UnionGetEnviVarOfTheIndexByIndex(index,1)))
	{
		UnionUserErrLog("in UnionLoadKeyDBBackupServerIntoMemory:: get ipAddr for [myself]!\n");
		ret = errCodeKeyDBBackuperMDL_ConfError;
		goto exitNormally;
	}
	strcpy(pgunionKeyDBBackupServer->myself.ipAddr,p);
	if ((p = UnionGetEnviVarOfTheIndexByIndex(index,2)) == NULL)
	{
		UnionUserErrLog("in UnionLoadKeyDBBackupServerIntoMemory:: get port for [myself]!\n");
		ret = errCodeKeyDBBackuperMDL_ConfError;
		goto exitNormally;
	}
	pgunionKeyDBBackupServer->myself.port = atoi(p);
	UnionLog("in UnionLoadKeyDBBackupServerIntoMemory:: port = [%s]\n",p);
	
	for (num = 0; num < conMaxNumOfKeyDBBackuper; num++)
		memset(&(pgunionKeyDBBackupServer->brother[num]),0,sizeof(pgunionKeyDBBackupServer->brother[num]));
	
	pgunionKeyDBBackupServer->realNum = 0;
	sprintf(varName,"brother");
	if ((index = UnionGetVarIndexOfTheVarName(varName)) >= 0)
	{
		if (!UnionIsValidIPAddrStr(p = UnionGetEnviVarOfTheIndexByIndex(index,1)))
			goto readAll;
		strcpy(pgunionKeyDBBackupServer->brother[pgunionKeyDBBackupServer->realNum].ipAddr,p);
		if ((p = UnionGetEnviVarOfTheIndexByIndex(index,2)) == NULL)
			goto readAll;
		pgunionKeyDBBackupServer->brother[pgunionKeyDBBackupServer->realNum].port = atoi(p);
		if ((p = UnionGetEnviVarOfTheIndexByIndex(index,3)) != NULL)
			pgunionKeyDBBackupServer->brother[pgunionKeyDBBackupServer->realNum].active = atoi(p);
		else
			pgunionKeyDBBackupServer->brother[pgunionKeyDBBackupServer->realNum].active = pgunionKeyDBBackupServer->active;
		pgunionKeyDBBackupServer->realNum += 1;
	}
readAll:
	for (num = 0; (num < conMaxNumOfKeyDBBackuper) && (pgunionKeyDBBackupServer->realNum < conMaxNumOfKeyDBBackuper); num++)
	{
		sprintf(varName,"brother%02d",num);
		if ((index = UnionGetVarIndexOfTheVarName(varName)) < 0)
		{
			//UnionUserErrLog("in UnionLoadKeyDBBackupServerIntoMemory:: UnionGetEnviVarByName for [%s]\n!",varName);
			//ret = errCodeKeyDBBackuperMDL_ConfError;
			//goto exitNormally;
			continue;
		}
		if (!UnionIsValidIPAddrStr(p = UnionGetEnviVarOfTheIndexByIndex(index,1)))
		{
			UnionUserErrLog("in UnionLoadKeyDBBackupServerIntoMemory:: get ipAddr for [%s]!\n",varName);
			//ret = errCodeKeyDBBackuperMDL_ConfError;
			//goto exitNormally;
			continue;
		}
		strcpy(pgunionKeyDBBackupServer->brother[pgunionKeyDBBackupServer->realNum].ipAddr,p);
		if ((p = UnionGetEnviVarOfTheIndexByIndex(index,2)) == NULL)
		{
			UnionUserErrLog("in UnionLoadKeyDBBackupServerIntoMemory:: get port for [%s]!\n",varName);
			//ret = errCodeKeyDBBackuperMDL_ConfError;
			//goto exitNormally;
			continue;
		}
		pgunionKeyDBBackupServer->brother[pgunionKeyDBBackupServer->realNum].port = atoi(p);
		if ((p = UnionGetEnviVarOfTheIndexByIndex(index,3)) != NULL)
			pgunionKeyDBBackupServer->brother[pgunionKeyDBBackupServer->realNum].active = atoi(p);
		else
			pgunionKeyDBBackupServer->brother[pgunionKeyDBBackupServer->realNum].active = pgunionKeyDBBackupServer->active;
		pgunionKeyDBBackupServer->realNum += 1;
		//UnionLog("in UnionLoadKeyDBBackupServerIntoMemory:: port = [%s]\n",p);
	}
	ret = pgunionKeyDBBackupServer->realNum;

exitNormally:
	UnionClearEnvi();
	return(ret);
}			
	
int UnionPrintKeyDBBackupServerToFile(FILE *fp)
{
	int	ret;
	long	i;
	
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintKeyDBBackupServerToFile:: fp is NULL pointer!\n");
		return(errCodeParameter);
	}
	if ((ret = UnionConnectKeyDBBackupServer()) < 0)
	{
		UnionUserErrLog("in UnionPrintKeyDBBackupServerToFile:: UnionConnectKeyDBBackupServer!\n");
		return(ret);
	}
	
	fprintf(fp,"KeyDBBackupServer Configuration::\n");
	if (pgunionKeyDBBackupServer->active)
		fprintf(fp," KeyDB Backupping Mechanism Active!\n");
	else
		fprintf(fp," KeyDB Backupping Mechanism Inactive!\n");
	fprintf(fp," myself    %15s %5d\n",pgunionKeyDBBackupServer->myself.ipAddr,pgunionKeyDBBackupServer->myself.port);
	for (i = 0; i < pgunionKeyDBBackupServer->realNum; i++)
		fprintf(fp," brother%02d %15s %5d %d\n",i+1,pgunionKeyDBBackupServer->brother[i].ipAddr,
							pgunionKeyDBBackupServer->brother[i].port,
							pgunionKeyDBBackupServer->brother[i].active);
	fprintf(fp," brothersNum = %02d\n",pgunionKeyDBBackupServer->realNum);
	
	return(0);
}

int UnionPrintKeyDBBackupServer()
{
	int	ret;
	
	if ((ret = UnionConnectKeyDBBackupServer()) < 0)
	{
		UnionUserErrLog("in UnionPrintKeyDBBackupServer:: UnionConnectKeyDBBackupServer!\n");
		return(ret);
	}
	UnionPrintKeyDBBackupServerToFile(stdout);
	UnionPrintSharedMemoryModuleToFile(pgunionKeyDBBackupServerMDL,stdout);
	return(0);
}

int UnionPrintStatusOfKeyDBBackupServerToFile(FILE *fp)
{
	int	ret;
	
	if ((ret = UnionConnectKeyDBBackupServer()) < 0)
	{
		UnionUserErrLog("in UnionPrintStatusOfKeyDBBackupServerToFile:: UnionConnectKeyDBBackupServer!\n");
		return(ret);
	}
	if (fp == NULL)
		return(errCodeParameter);
	return(UnionPrintSharedMemoryModuleToFile(pgunionKeyDBBackupServerMDL,fp));
}

int UnionSetKeyDBBackupServerActive()
{
	int	ret;
	char	fileName[512+1];
		
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfKeyDBBackupServer(fileName);
	if ((ret = UnionUpdateEnviVar(fileName,"active","[1]")) < 0)
	{
		UnionUserErrLog("in UnionSetKeyDBBackupServerActive:: UnionUpdateEnviVar!\n");
		return(ret);
	}

	if ((ret = UnionConnectKeyDBBackupServer()) < 0)
	{
		UnionUserErrLog("in UnionSetKeyDBBackupServerActive:: UnionConnectKeyDBBackupServer!\n");
		return(ret);
	}
	pgunionKeyDBBackupServer->active = 1;
	return(0);
}

int UnionSetKeyDBBackupServerInactive()
{
	int	ret;
	char	fileName[512+1];
		
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfKeyDBBackupServer(fileName);
	if ((ret = UnionUpdateEnviVar(fileName,"active","[0]")) < 0)
	{
		UnionUserErrLog("in UnionSetKeyDBBackupServerInactive:: UnionUpdateEnviVar!\n");
		return(ret);
	}

	if ((ret = UnionConnectKeyDBBackupServer()) < 0)
	{
		UnionUserErrLog("in UnionSetKeyDBBackupServerInactive:: UnionConnectKeyDBBackupServer!\n");
		return(ret);
	}
	pgunionKeyDBBackupServer->active = 0;
	return(0);
}


int UnionSynchronizeKeyDBOperation(TUnionKeyDBOperation operation,char keyDBType,unsigned char *keyRec,int sizeOfKeyRec)
{
	int			ret;
	int			i;
	int			successNum = 0;
	
	if ((gisKeyDBBackupServer) || (keyRec == NULL) || (sizeOfKeyRec <= 0))
		return(0);

	// 2007/12/26�������´���� gunionIsDefaultDBBackuperBrotherUsed�ж����֮���Ƶ��˴�
	// ���ݵ����з�������
	if ((ret = UnionConnectKeyDBBackupServer()) < 0)
	{
		UnionUserErrLog("in UnionSynchronizeKeyDBOperation:: UnionConnectKeyDBBackupServer!\n");
		return(ret);
	}

	// �����ݣ�
	if (!pgunionKeyDBBackupServer->active)
		return(0);
	// 2007/12/26�ƶ�������
	
	if (gunionIsDefaultDBBackuperBrotherUsed)	// ֻ���ݵ�ָ����ȱʡ������
		return(UnionSynchronizeKeyDBOperationToSpecBrother(gunionDefaultDBBackuperBrother,operation,keyDBType,keyRec,sizeOfKeyRec));
		
	for (i = 0; i < pgunionKeyDBBackupServer->realNum; i++)
	{
		// 2007/12/11 ����������Ƶ� UnionSynchronizeKeyDBOperationToSpecBrother ��
		/*
		if (!pgunionKeyDBBackupServer->brother[i].active)	// �����ݵ����������
			continue;
		*/
		// 2007/12/11 �޸Ľ���
		if ((ret = UnionSynchronizeKeyDBOperationToSpecBrother(pgunionKeyDBBackupServer->brother[i].ipAddr,
				operation,keyDBType,keyRec,sizeOfKeyRec)) >= 0)
			successNum++;
	}
	return(ret = successNum);
}

int UnionGetPortOfMineOfKeyDBBackupServer()
{
	int	ret;
	if ((ret = UnionConnectKeyDBBackupServer()) < 0)
	{
		UnionUserErrLog("in UnionGetPortOfMineOfKeyDBBackupServer:: UnionConnectKeyDBBackupServer!\n");
		return(ret);
	}
	return(pgunionKeyDBBackupServer->myself.port);
}

int UnionReadSynchronizingKeyDBOperation(int handle,TUnionKeyDBOperation *operation,char *keyDBType,unsigned char *keyRec,int sizeOfKeyRecBuf)
{
	int			ret;
	int			sck;
	unsigned char		tmpBuf[100];
	int			len;
	TUnionKeyDBBackuperRecordHeader	header;
	int			lenOfReq;

	// �ó�ʱ���ƣ�2007/12/11����
#ifdef _LINUX_
	if (sigsetjmp(gsjl06CommJmpEnv,1) != 0)
#else
	if (setjmp(gsjl06CommJmpEnv) != 0)
#endif
	{
		//alarm(0);
		UnionUserErrLog("in UnionReadSynchronizingKeyDBOperation:: timeout!\n");
		return(errCodeTimeout);
	}
	alarm(1);
	signal(SIGALRM,UnionDealKeyDBBackuperTimeout);
	// �ó�ʱ���ƽ�����2007/12/11���ӽ���
		
	if ((ret = UnionReceiveFromSocketUntilLen(handle,tmpBuf,2)) != 2)
	{
		// 2007/12/11�޸�
		if (ret != 0)
			UnionUserErrLog("in UnionReadSynchronizingKeyDBOperation::UnionReceiveFromSocketUntilLen Error! ret = [%d]\n",ret);
		else
			UnionProgramerLog("in UnionReadSynchronizingKeyDBOperation:: connection closed by peer!\n");
		alarm(0); // 2007/12/11����
		return(ret);
		// 2007/12/11�޸Ľ���
	}
	lenOfReq = tmpBuf[0] * 256 + tmpBuf[1];
	if (lenOfReq > sizeof(header) + sizeOfKeyRecBuf)
	{
		UnionUserErrLog("in UnionReadSynchronizingKeyDBOperation::sizeOfKeyRecBuf [%d] < expected [%d]\n",
				sizeOfKeyRecBuf,lenOfReq-sizeof(header));
		alarm(0); // 2007/12/11����
		return(errCodeParameter);
	}
	memset(&header,0,sizeof(header));
	if ((ret = UnionReceiveFromSocketUntilLen(handle,&header,sizeof(header))) != sizeof(header))
	{
		if (ret != 0)
			UnionUserErrLog("in UnionReadSynchronizingKeyDBOperation:: UnionReceiveFromSocketUntilLen expected [%d] real = [%d]!\n",sizeof(header),ret);
		else
			UnionLog("in UnionReadSynchronizingKeyDBOperation:: Connection Closed by Client!\n");
		alarm(0); // 2007/12/11����
		return(ret);
	}
	if ((ret = UnionReceiveFromSocketUntilLen(handle,keyRec,lenOfReq-sizeof(header))) !=  lenOfReq-sizeof(header))
	{
		if (ret != 0)
			UnionUserErrLog("in UnionReadSynchronizingKeyDBOperation:: UnionReceiveFromSocketUntilLen expected [%d] real = [%d]!\n",lenOfReq-sizeof(header),ret);
		else
			UnionLog("in UnionReadSynchronizingKeyDBOperation:: Connection Closed by Client!\n");
		alarm(0); // 2007/12/11����
		return(ret);
	}
	alarm(0);	// 2007/12/11����
	*operation = header.operation;
	*keyDBType = header.keyDBType;
	if (lenOfReq - sizeof(header) < header.lenOfKeyRec)
	{
		UnionUserErrLog("in UnionReadSynchronizingKeyDBOperation:: lenOfKeyRec [%d] < defined [%d]\n",
			lenOfReq-sizeof(header),header.lenOfKeyRec);
		return(errCodeParameter);
	}
	else
		return(header.lenOfKeyRec);

}

int UnionAnswerKeyDBSynchronizingRequest(int handle,char *errCode)
{
	char	tmpBuf[100];
	int	ret;
	
	tmpBuf[0] = 0x00;
	tmpBuf[1] = 0x02;
	memcpy(tmpBuf+2,errCode,2);
	if ((ret = UnionSendToSocket(handle,tmpBuf,4)) != 4)
	{
		UnionUserErrLog("in UnionAnswerKeyDBSynchronizingRequest:: UnionSendToSocket!\n");
		return(ret);
	}
	return(0);
}

int UnionPrintKeyDBBackupServerToSpecFile(char *fileName)
{
	FILE	*fp;
	int	ret;
	
	if ((fileName == NULL) || (strlen(fileName) == 0))
		fp = stdout;
	else
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionPrintKeyDBBackupServerToSpecFile:: fopen [%s]\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	ret = UnionPrintKeyDBBackupServerToFile(fp);
	if (fp != stdout)
		fclose(fp);
	return(ret);
}


// 20060825 ����
int UnionUpdateLocalKeyDBBackuperDef(char *ipAddr,int port)
{
	int		ret;
	char		fileName[512+1];
		
	if (ipAddr == NULL)
		return(errCodeParameter);
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfKeyDBBackupServer(fileName);
	if ((ret = UnionUpdateEnviVar(fileName,"myself","[%s][%d]",ipAddr,port)) < 0)
	{
		UnionUserErrLog("in UnionUpdateLocalKeyDBBackuperDef:: UnionUpdateEnviVar!\n");
		return(ret);
	}

	// ���¼��ض����ļ�
	if ((ret = UnionLoadKeyDBBackupServerIntoMemory()) < 0)
	{
		UnionUserErrLog("in UnionUpdateLocalKeyDBBackuperDef:: UnionLoadKeyDBBackupServerIntoMemory!\n");
		return(ret);
	}
	
	return(ret);
}

// 20060825 ����
int UnionUpdateRemoteKeyDBBackuperDef(char *ipAddr,int port)
{
	int		ret;
	char		fileName[512+1];
		
	if (ipAddr == NULL)
		return(errCodeParameter);
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfKeyDBBackupServer(fileName);
	if ((ret = UnionUpdateEnviVar(fileName,"brother","[%s][%d]",ipAddr,port)) < 0)
	{
		UnionUserErrLog("in UnionUpdateRemoteKeyDBBackuperDef:: UnionUpdateEnviVar!\n");
		return(ret);
	}

	// ���¼��ض����ļ�
	if ((ret = UnionLoadKeyDBBackupServerIntoMemory()) < 0)
	{
		UnionUserErrLog("in UnionUpdateRemoteKeyDBBackuperDef:: UnionLoadKeyDBBackupServerIntoMemory!\n");
		return(ret);
	}
	
	return(ret);
}

int UnionUpdateSpecRemoteKeyDBBackuperDef(int indexOfBackuper,char *ipAddr,int port,int active)
{
	int		ret;
	char		fileName[512+1];
	char		varName[40+1];
		
	if (ipAddr == NULL)
		return(errCodeParameter);
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfKeyDBBackupServer(fileName);
	sprintf(varName,"brother%02d",indexOfBackuper);
	if ((ret = UnionUpdateEnviVar(fileName,varName,"[%s][%d][%d]",ipAddr,port,active)) < 0)
	{
		UnionUserErrLog("in UnionUpdateRemoteKeyDBBackuperDef:: UnionUpdateEnviVar!\n");
		return(ret);
	}

	// ���¼��ض����ļ�
	if ((ret = UnionLoadKeyDBBackupServerIntoMemory()) < 0)
	{
		UnionUserErrLog("in UnionUpdateRemoteKeyDBBackuperDef:: UnionLoadKeyDBBackupServerIntoMemory!\n");
		return(ret);
	}
	
	return(ret);
}

// 2007/11/28����
// ��ָ���ı��ݷ������Ƚ���Կֵ
int UnionCompareDesKeyValueWithSpecBrother(char *ipAddr,char *fullKeyName,char *keyByLmk)
{
	TUnionDesKey		desKey;
	
	if ((fullKeyName == NULL) || (keyByLmk == NULL))
		return(0);
		
	memset(&desKey,0,sizeof(desKey));
	if (strlen(fullKeyName) >= sizeof(desKey.fullName))
	{
		UnionUserErrLog("in UnionCompareDesKeyValueWithSpecBrother:: fullKeyName [%s] too long!\n",fullKeyName);
		return(errCodeParameter);
	}
	strcpy(desKey.fullName,fullKeyName);
	if (strlen(keyByLmk) >= sizeof(desKey.value))
	{
		UnionUserErrLog("in UnionCompareDesKeyValueWithSpecBrother:: keyByLmk [%s] too long!\n",keyByLmk);
		return(errCodeParameter);
	}
	strcpy(desKey.value,keyByLmk);
	return(UnionSynchronizeKeyDBOperationToSpecBrother(ipAddr,conCompareKeyValue,conIsDesKeyDB,
		(unsigned char *)(&desKey),sizeof(desKey)));
}

// 2007/11/28����
// �����еı��ݷ������Ƚ���Կֵ
int UnionCompareDesKeyValueWithBrothers(char *fullKeyName,char *keyByLmk)
{
	int			ret;
	int			i;
	int			failNum = 0;
	
	if ((gisKeyDBBackupServer) || (fullKeyName == NULL) || (keyByLmk == NULL))
		return(0);

	if ((ret = UnionConnectKeyDBBackupServer()) < 0)
	{
		UnionUserErrLog("in UnionCompareDesKeyValueWithBrothers:: UnionConnectKeyDBBackupServer!\n");
		return(ret);
	}
	
	// ��̨�����������ݵ�����������
	if (!pgunionKeyDBBackupServer->active)
		return(0);

	for (i = 0; i < pgunionKeyDBBackupServer->realNum; i++)
	{
		if (!pgunionKeyDBBackupServer->brother[i].active)	// �����ݵ����������
			continue;
		if ((ret = UnionCompareDesKeyValueWithSpecBrother(pgunionKeyDBBackupServer->brother[i].ipAddr,
				fullKeyName,keyByLmk)) < 0)
		{
			UnionUserErrLog("in UnionCompareDesKeyValueWithBrothers:: [%s] not same as [%s]\n",
					fullKeyName,pgunionKeyDBBackupServer->brother[i].ipAddr);
			failNum++;
		}
	}
	if (failNum > 0)
		return(errCodeDesKeyDBMDL_KeyValueNotSameAsExpected);
	else
		return(0);
}

// 2007/12/11������
void UnionDealKeyDBBackuperTimeout()
{
	//signal(SIGALRM,SIG_IGN);
	UnionUserErrLog("in UnionDealKeyDBBackuperTimeout:: time out!\n");
	//gsjl06CommJmpSet = 1;
#ifdef _LINUX_
	siglongjmp(gsjl06CommJmpEnv,10);
#else
	longjmp(gsjl06CommJmpEnv,10);
#endif
}

// 2007/12/11����
int UnionMaintainStatusOfKeyDBBrothers()
{
	int			ret;
	int			i;
	int			successNum = 0;
	int			sck;
	
	if ((ret = UnionConnectKeyDBBackupServer()) < 0)
	{
		UnionUserErrLog("in UnionMaintainStatusOfKeyDBBrothers:: UnionConnectKeyDBBackupServer!\n");
		return(ret);
	}
	
	// �����
	if (!pgunionKeyDBBackupServer->active)
		return(0);

	for (i = 0; i < pgunionKeyDBBackupServer->realNum; i++)
	{
		if ((sck = UnionCreateSocketClient(pgunionKeyDBBackupServer->brother[i].ipAddr,
						pgunionKeyDBBackupServer->brother[i].port)) < 0)
		{
			if (sck == errCodeSocketMDL_Timeout)
				gunionIsCreateSckToKeyDBBackuperFailure = 1;
			UnionUserErrLog("in UnionMaintainStatusOfKeyDBBrothers:: UnionCreateSocketClient [%s] [%d]\n",
					pgunionKeyDBBackupServer->brother[i].ipAddr,
					pgunionKeyDBBackupServer->brother[i].port);
			pgunionKeyDBBackupServer->brother[i].active = 0;
			continue;
		}
		if (pgunionKeyDBBackupServer->brother[i].active == 0)
			pgunionKeyDBBackupServer->brother[i].active = 1;
		UnionCloseSocket(sck);
		successNum++;
	}
	return(successNum);
}

// 2007/12/11����
// ͳ��������Կֵ����ͬ�ı��ݷ�����������ֵ�ǲ�ͬ���ı��ݷ�����������
// ipAddr���ǲ�ͬ���ı��ݷ�������IP��ַ��
int UnionCountAllUnsameDesKeyBrothers(char *fullKeyName,char *keyByLmk,char ipAddr[][15+1],int maxNumOfIPAddr)
{
	int			ret;
	int			i;
	int			failNum = 0;
	
	if ((gisKeyDBBackupServer) || (fullKeyName == NULL) || (keyByLmk == NULL))
		return(0);

	if ((ret = UnionConnectKeyDBBackupServer()) < 0)
	{
		UnionUserErrLog("in UnionCountAllUnsameDesKeyBrothers:: UnionConnectKeyDBBackupServer!\n");
		return(ret);
	}
	
	// ��̨�����������ݵ�����������
	if (!pgunionKeyDBBackupServer->active)
		return(0);

	for (i = 0; i < pgunionKeyDBBackupServer->realNum; i++)
	{
		if (!pgunionKeyDBBackupServer->brother[i].active)	// �����ݵ����������
			continue;
		if ((ret = UnionCompareDesKeyValueWithSpecBrother(pgunionKeyDBBackupServer->brother[i].ipAddr,
				fullKeyName,keyByLmk)) < 0)
		{
			//UnionUserErrLog("in UnionCountAllUnsameDesKeyBrothers:: [%s] not same as [%s]\n",
			//		fullKeyName,pgunionKeyDBBackupServer->brother[i].ipAddr);
			if (failNum < maxNumOfIPAddr)
				strcpy(ipAddr[failNum],pgunionKeyDBBackupServer->brother[i].ipAddr);
			failNum++;
		}
	}
	return(failNum);
}
// 2007/11/28����
// ��ָ���ı��ݷ������Ƚ���Կֵ
int UnionComparePKValueWithSpecBrother(char *ipAddr,char *fullKeyName,char *keyByLmk)
{
	TUnionPK		pk;
	
	if ((fullKeyName == NULL) || (keyByLmk == NULL))
		return(0);
		
	memset(&pk,0,sizeof(pk));
	if (strlen(fullKeyName) >= sizeof(pk.fullName))
	{
		UnionUserErrLog("in UnionComparePKValueWithSpecBrother:: fullKeyName [%s] too long!\n",fullKeyName);
		return(errCodeParameter);
	}
	strcpy(pk.fullName,fullKeyName);
	if (strlen(keyByLmk) >= sizeof(pk.value))
	{
		UnionUserErrLog("in UnionComparePKValueWithSpecBrother:: keyByLmk [%s] too long!\n",keyByLmk);
		return(errCodeParameter);
	}
	strcpy(pk.value,keyByLmk);
	return(UnionSynchronizeKeyDBOperationToSpecBrother(ipAddr,conCompareKeyValue,conIsPKDB,
		(unsigned char *)(&pk),sizeof(pk)));
}

// 2007/11/28����
// �����еı��ݷ������Ƚ���Կֵ
int UnionComparePKValueWithBrothers(char *fullKeyName,char *keyByLmk)
{
	int			ret;
	int			i;
	int			failNum = 0;
	
	if ((gisKeyDBBackupServer) || (fullKeyName == NULL) || (keyByLmk == NULL))
		return(0);

	if ((ret = UnionConnectKeyDBBackupServer()) < 0)
	{
		UnionUserErrLog("in UnionComparePKValueWithBrothers:: UnionConnectKeyDBBackupServer!\n");
		return(ret);
	}
	
	// ��̨�����������ݵ�����������
	if (!pgunionKeyDBBackupServer->active)
		return(0);

	for (i = 0; i < pgunionKeyDBBackupServer->realNum; i++)
	{
		if (!pgunionKeyDBBackupServer->brother[i].active)	// �����ݵ����������
			continue;
		if ((ret = UnionComparePKValueWithSpecBrother(pgunionKeyDBBackupServer->brother[i].ipAddr,
				fullKeyName,keyByLmk)) < 0)
		{
			UnionUserErrLog("in UnionComparePKValueWithBrothers:: [%s] not same as [%s]\n",
					fullKeyName,pgunionKeyDBBackupServer->brother[i].ipAddr);
			failNum++;
		}
	}
	if (failNum > 0)
		return(errCodeDesKeyDBMDL_KeyValueNotSameAsExpected);
	else
		return(0);
}
