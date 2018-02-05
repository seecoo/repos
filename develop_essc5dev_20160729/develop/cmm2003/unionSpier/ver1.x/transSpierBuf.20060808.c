//	Author:		Wolfgang Wang
//	Date:		2003/11/04
//	Version:	5.0

//	5.1 2005/05/30，在5.0基础上升级
//	修改了	UnionApplyNewMsgIndexOfTransSpierBuf

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
#include "transSpierBuf.h"
#include "unionErrCode.h"
#include "unionREC.h"
#include "UnionStr.h"
#include "unionVersion.h"
#include "commWithTransSpier.h"

PUnionTransSpierBufHDL		pgunionTransSpierBufHDL = NULL;
unsigned char			*pgunionTransSpierBuf = NULL;
PUnionIndexStatusTBL		pgunionTransSpierStatusTBL = NULL;
PUnionSharedMemoryModule	pgunionTransSpierBufMDL = NULL;

int UnionPutTransSpierMsgHeaderIntoStr(PUnionTransSpierMsgHeader pheader,char *str,int sizeOfStr)
{
	int	len = 0;
	int	ret;

#ifdef _transSpier_2_x_	
	if ((ret = UnionPutRecFldIntoRecStr("applicationName",pheader->applicationName,strlen(pheader->applicationName),str+len,sizeOfStr-len)) < 0)
	{
		UnionUserErrLog("in UnionPutTransSpierMsgHeaderIntoStr:: UnionPutRecFldIntoRecStr!\n");
		return(ret);
	}
	len += ret;
	if ((ret = UnionPutRecFldIntoRecStr("dateTime",pheader->dateTime,strlen(pheader->dateTime),str+len,sizeOfStr-len)) < 0)
	{
		UnionUserErrLog("in UnionPutTransSpierMsgHeaderIntoStr:: UnionPutRecFldIntoRecStr!\n");
		return(ret);
	}
	len += ret;
	if ((ret = UnionPutRecFldIntoRecStr("clientIPAddr",pheader->clientIPAddr,strlen(pheader->clientIPAddr),str+len,sizeOfStr-len)) < 0)
	{
		UnionUserErrLog("in UnionPutTransSpierMsgHeaderIntoStr:: UnionPutRecFldIntoRecStr!\n");
		return(ret);
	}
	len += ret;
	if ((ret = UnionPutIntTypeRecFldIntoRecStr("ssn",pheader->ssn,str+len,sizeOfStr-len)) < 0)
	{
		UnionUserErrLog("in UnionPutTransSpierMsgHeaderIntoStr:: UnionPutIntTypeRecFldIntoRecStr!\n");
		return(ret);
	}
	len += ret;
	if ((ret = UnionPutIntTypeRecFldIntoRecStr("pid",pheader->pid,str+len,sizeOfStr-len)) < 0)
	{
		UnionUserErrLog("in UnionPutTransSpierMsgHeaderIntoStr:: UnionPutIntTypeRecFldIntoRecStr!\n");
		return(ret);
	}
	len += ret;
	if ((ret = UnionPutIntTypeRecFldIntoRecStr("clientPort",pheader->clientPort,str+len,sizeOfStr-len)) < 0)
	{
		UnionUserErrLog("in UnionPutTransSpierMsgHeaderIntoStr:: UnionPutIntTypeRecFldIntoRecStr!\n");
		return(ret);
	}
	len += ret;
#endif
	if ((ret = UnionPutIntTypeRecFldIntoRecStr("resID",pheader->resID,str+len,sizeOfStr-len)) < 0)
	{
		UnionUserErrLog("in UnionPutTransSpierMsgHeaderIntoStr:: UnionPutIntTypeRecFldIntoRecStr!\n");
		return(ret);
	}
	len += ret;
	if ((ret = UnionPutIntTypeRecFldIntoRecStr("len",pheader->len,str+len,sizeOfStr-len)) < 0)
	{
		UnionUserErrLog("in UnionPutTransSpierMsgHeaderIntoStr:: UnionPutIntTypeRecFldIntoRecStr!\n");
		return(ret);
	}
	len += ret;
	return(len);
}
	
int UnionReconnectTransSpierBufMDLAnyway()
{
	int				ret;
	TUnionTransSpierBufHDL		def;
		
	if (UnionIsTransSpierBufMDLConnected())
		UnionDisconnectTransSpierBufMDL();

	//UnionAuditLog("in UnionReconnectTransSpierBufMDLAnyway:: entering ...\n");
	if ((ret = UnionInitTransSpierBufDef(&def)) < 0)
	{
		UnionUserErrLog("in UnionReconnectTransSpierBufMDLAnyway:: UnionInitTransSpierBufDef!\n");
		return(ret);
	}
	
	if ((pgunionTransSpierBufMDL = UnionConnectSharedMemoryModule(conMDLNameOfTransSpierBuf,
			sizeof(*pgunionTransSpierBufHDL) + 
			(sizeof(TUnionTransSpierMsgHeader) + sizeof(unsigned char) * def.maxSizeOfMsg) * def.maxNumOfMsg)) == NULL)
	{
		UnionUserErrLog("in UnionReconnectTransSpierBufMDLAnyway:: UnionConnectSharedMemoryModule!\n");
		return(errCodeSharedMemoryModule);
	}

	if ((pgunionTransSpierBufHDL = (PUnionTransSpierBufHDL)UnionGetAddrOfSharedMemoryModuleUserSpace(pgunionTransSpierBufMDL)) == NULL)
	{
		UnionUserErrLog("in UnionReconnectTransSpierBufMDLAnyway:: PUnionTransSpierBufHDL!\n");
		return(errCodeSharedMemoryModule);
	}
	
	if ((pgunionTransSpierBuf = (unsigned char *)((unsigned char *)pgunionTransSpierBufHDL + sizeof(*pgunionTransSpierBufHDL))) == NULL)
	{
		UnionUserErrLog("in UnionReconnectTransSpierBufMDLAnyway:: unsigned char *!\n");
		return(errCodeSharedMemoryModule);
	}
	pgunionTransSpierBufHDL->pmsgBuf = pgunionTransSpierBuf;
	
	if (UnionIsNewCreatedSharedMemoryModule(pgunionTransSpierBufMDL))
	{
		pgunionTransSpierBufHDL->maxSizeOfMsg = def.maxSizeOfMsg;
		pgunionTransSpierBufHDL->maxNumOfMsg = def.maxNumOfMsg;
	}

	if ((pgunionTransSpierStatusTBL = UnionConnectIndexStatusTBL(UnionGetUserIDOfSharedMemoryModule(pgunionTransSpierBufMDL),
		pgunionTransSpierBufHDL->maxNumOfMsg)) == NULL)
	{
		UnionUserErrLog("in UnionReconnectTransSpierBufMDLAnyway:: UnionConnectIndexStatusTBL!\n");
		return(errCodeTransSpierBufMDL_ConnectIndexTBL);
	}
	
	if (UnionIsNewCreatedSharedMemoryModule(pgunionTransSpierBufMDL))
	{
		UnionResetAllTransSpierMsgPos();
	}
	
	//UnionAuditLog("in UnionReconnectTransSpierBufMDLAnyway:: exit ...\n");
	return(0);
}

int UnionIsTransSpierBufMDLConnected()
{
	if ((pgunionTransSpierBufHDL == NULL) || (pgunionTransSpierBuf == NULL) || (pgunionTransSpierStatusTBL == NULL) || (!UnionIsSharedMemoryInited(conMDLNameOfTransSpierBuf)))
		return(0);
	else
		return(1);
}

int UnionGetTotalNumOfTransSpierBufMDL()
{
	int	ret;
	
	if ((ret = UnionConnectTransSpierBufMDL()) < 0)
		return(ret);
	return(pgunionTransSpierBufHDL->maxNumOfMsg);
}

int UnionConnectTransSpierBufMDL()
{
	int	ret;
	
	if (UnionIsTransSpierBufMDLConnected())
		return(0);

	//UnionAuditLog("in UnionConnectTransSpierBufMDL:: entering... !\n");
	if ((ret = UnionReconnectTransSpierBufMDLAnyway()) < 0)
	{
		UnionUserErrLog("in UnionConnectTransSpierBufMDL:: UnionReconnectTransSpierBufMDLAnyway!\n");
		return(ret);
	}
	//UnionAuditLog("in UnionConnectTransSpierBufMDL:: exit... !\n");
	return(ret);
}	
	
int UnionDisconnectTransSpierBufMDL()
{
	pgunionTransSpierBuf = NULL;
	pgunionTransSpierBufHDL = NULL;
	UnionDisconnectIndexStatusTBL(pgunionTransSpierStatusTBL);
	return(UnionDisconnectShareModule(pgunionTransSpierBufMDL));
}

int UnionRemoveTransSpierBufMDL()
{
	pgunionTransSpierBuf = NULL;
	pgunionTransSpierBufHDL = NULL;
	if (pgunionTransSpierBufMDL != NULL)
	{
		UnionRemoveIndexStatusTBL(UnionGetUserIDOfSharedMemoryModule(pgunionTransSpierBufMDL));
	}
	pgunionTransSpierBufMDL = NULL;
	return(UnionRemoveSharedMemoryModule(conMDLNameOfTransSpierBuf));
}

int UnionReloadTransSpierBufDef()
{
	int				ret;
	
	//UnionAuditLog("in UnionReloadTransSpierBufDef:: UnionConnectTransSpierBufMDL entering... !\n");
	if ((ret = UnionConnectTransSpierBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionReloadTransSpierBufDef:: UnionConnectTransSpierBufMDL!\n");
		return(ret);
	}
	//UnionAuditLog("in UnionReloadTransSpierBufDef:: UnionConnectTransSpierBufMDL exit... !\n");
	return(UnionResetAllTransSpierMsgPos());
}

long UnionBufferTransSpierMsg(char *msg,int lenOfMsg,int resID)
{
	int				index;
	int				ret;
	PUnionTransSpierMsgHeader	pmsgHeader;
	
	if ((ret = UnionConnectTransSpierBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionBufferTransSpierMsg:: UnionConnectTransSpierBufMDL!\n");
		return(ret);
	}	
	if ((msg == NULL) || (lenOfMsg <= 0))
	{
		UnionUserErrLog("in UnionBufferTransSpierMsg:: wrong parameter!\n");
		return(errCodeParameter);
	}
	if (((index = UnionGetAvailableIndexNoWait(pgunionTransSpierStatusTBL)) < 0) || (index >= pgunionTransSpierBufHDL->maxNumOfMsg))
	{
		//UnionUserErrLog("in UnionBufferTransSpierMsg:: UnionGetAvailableIndexNoWait!\n");
		return(index);
	}
	pmsgHeader = (PUnionTransSpierMsgHeader)(pgunionTransSpierBuf + conSizeOfEachTransSpierMsg * index);
	if (lenOfMsg >= pgunionTransSpierBufHDL->maxSizeOfMsg)
		pmsgHeader->len = pgunionTransSpierBufHDL->maxSizeOfMsg-1;
	else
		pmsgHeader->len = lenOfMsg;
#ifdef _transSpier_2_x_
	pmsgHeader->resID = resID;
	strcpy(pmsgHeader->applicationName,UnionGetApplicationName());
	strcpy(pmsgHeader->clientIPAddr,UnionGetClientIPAddrToldTransSpier());
	pmsgHeader->clientPort = UnionGetClientPortToldTransSpier();
	pmsgHeader->pid = getpid();
	pmsgHeader->ssn = UnionGetSpierSSN();
	memset(pmsgHeader->dateTime,0,sizeof(pmsgHeader->dateTime));
	UnionGetFullSystemDateTime(pmsgHeader->dateTime);
#endif
	memcpy(pgunionTransSpierBuf + conSizeOfEachTransSpierMsg * index + sizeof(TUnionTransSpierMsgHeader),msg,pmsgHeader->len);
	return(UnionSetIndexWithUserStatusNoWait(pgunionTransSpierStatusTBL,index,conTransSpierMsgFlag));
}
	
int UnionReadFirstTransSpierMsgUntilSuccess(unsigned char *msg,int sizeOfMsg,int *resID)
{
	return(UnionReadFirstTransSpierMsgWithMsgHeaderUntilSuccess(msg,sizeOfMsg,resID,NULL));
}

int UnionReadFirstTransSpierMsgWithMsgHeaderUntilSuccess(unsigned char *msg,int sizeOfMsg,int *resID,PUnionTransSpierMsgHeader poutputMsgHeader)
{
	int				index;
	int				len;
	PUnionTransSpierMsgHeader 	pmsgHeader;
	int				ret;
		
	if ((ret = UnionConnectTransSpierBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionReadFirstTransSpierMsgWithMsgHeaderUntilSuccess:: UnionConnectTransSpierBufMDL!\n");
		return(ret);
	}	
	if ((msg == NULL) || (sizeOfMsg <= 0))
	{
		UnionUserErrLog("in UnionReadFirstTransSpierMsgWithMsgHeaderUntilSuccess:: wrong parameter!\n");
		return(errCodeParameter);
	}
loopRead:
	if (((index = UnionGetFirstIndexOfUserStatusUntilSuccess(pgunionTransSpierStatusTBL,conTransSpierMsgFlag)) < 0)
		 || (index >= pgunionTransSpierBufHDL->maxNumOfMsg))
	{
		UnionUserErrLog("in UnionReadFirstTransSpierMsgWithMsgHeaderUntilSuccess:: UnionGetFirstIndexOfUserStatus!\n");
		return(index);
	}
	pmsgHeader = (PUnionTransSpierMsgHeader)(pgunionTransSpierBuf + conSizeOfEachTransSpierMsg * index);
	if (poutputMsgHeader != NULL)
		memcpy(poutputMsgHeader,pmsgHeader,sizeof(*pmsgHeader));
	if ((len = pmsgHeader->len) <= 0)
	{
		UnionSetIndexAvailable(pgunionTransSpierStatusTBL,index);
		goto loopRead;
	}
	if (len >= sizeOfMsg)
	{
		len = sizeOfMsg-4;
		memcpy(msg,pgunionTransSpierBuf + conSizeOfEachTransSpierMsg * index + sizeof(TUnionTransSpierMsgHeader),len);
		memset(msg+len,'.',3);
		len = sizeOfMsg - 1;
	}
	else
		memcpy(msg,pgunionTransSpierBuf + conSizeOfEachTransSpierMsg * index + sizeof(TUnionTransSpierMsgHeader),len);
	pmsgHeader->len = -1;
	*resID = pmsgHeader->resID;
	UnionSetIndexAvailable(pgunionTransSpierStatusTBL,index);

	return(len);
}


int UnionPrintAvailablTransSpierBufPosToFile(FILE *fp)
{
	int				index;
	PUnionTransSpierMsgHeader	pmsgHeader;
	int				num;
	int				ret;
	
	if (fp == NULL)
		fp = stdout;

	if ((ret = UnionConnectTransSpierBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionPrintAvailablTransSpierBufPosToFile:: UnionConnectTransSpierBufMDL!\n");
		return(ret);
	}	
	for (index = 0,num = 0; index < pgunionTransSpierBufHDL->maxNumOfMsg; index++)
	{
		if ((pmsgHeader = (PUnionTransSpierMsgHeader)(pgunionTransSpierBuf + conSizeOfEachTransSpierMsg * index)) == NULL)
			return(errCodeTransSpierBufMDL_OutofRange);
		if (pmsgHeader->len > 0)
			continue;
		fprintf(fp,"%06d\n",index);
		num++;
		if ((num % 23 == 0) && (num != 0) && ((fp == stdout) || (fp == stderr)))
		{
			if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit ...")))
				break;
		}
	}
	fprintf(fp,"Total Available Num = [%d]\n",num);
	return(0);
}

int UnionPrintInavailabeTransSpierBufPosToFile(FILE *fp)
{
	int				index;
	PUnionTransSpierMsgHeader	pmsgHeader;
	int				num;
	int				ret;
	
	if (fp == NULL)
		return(errCodeParameter);

	if ((ret = UnionConnectTransSpierBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionPrintInavailabeTransSpierBufPosToFile:: UnionConnectTransSpierBufMDL!\n");
		return(ret);
	}	
	for (index = 0,num = 0; index < pgunionTransSpierBufHDL->maxNumOfMsg; index++)
	{
		if ((pmsgHeader = (PUnionTransSpierMsgHeader)(pgunionTransSpierBuf + conSizeOfEachTransSpierMsg * index)) == NULL)
			return(errCodeTransSpierBufMDL_OutofRange);
		if (pmsgHeader->len <= 0)
			continue;
		fprintf(fp,"%06d %04d %03d\n",index,pmsgHeader->len,pmsgHeader->resID);
		num++;
		if ((num % 22 == 0) && (num != 0) && ((fp == stdout) || (fp == stderr))) 
		{
			if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit ...")))
				break;
		}
	}
	fprintf(fp,"Total Inavailable Num = [%d]\n",num);
	return(0);
}

int UnionPrintTransSpierBufStatusToFile(FILE *fp)
{
	int	ret;
	
	if (fp == NULL)
		return(errCodeParameter);

	if ((ret = UnionConnectTransSpierBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionPrintTransSpierBufStatusToFile:: UnionConnectTransSpierBufMDL!\n");
		return(ret);
	}	
	fprintf(fp,"\n");
	fprintf(fp,"maxSizeOfMsg		[%d]\n",pgunionTransSpierBufHDL->maxSizeOfMsg);
	fprintf(fp,"maxNumOfMsg			[%d]\n",pgunionTransSpierBufHDL->maxNumOfMsg);
	fprintf(fp,"\n");
	if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or quit/exit to exit...")))
		return(0);
	UnionPrintSharedMemoryModuleToFile(pgunionTransSpierBufMDL,fp);
	return(0);
}

int UnionPrintTransSpierBufToFile(FILE *fp)
{
	UnionPrintAvailablTransSpierBufPosToFile(fp);
	UnionPrintInavailabeTransSpierBufPosToFile(fp);
	UnionPrintTransSpierBufStatusToFile(fp);
	return(0);
}

int UnionResetAllTransSpierMsgPos()
{
	int	ret;
	int	index;
	PUnionTransSpierMsgHeader 	pmsgHeader;
	
	if ((ret = UnionConnectTransSpierBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionResetAllTransSpierMsgPos:: UnionConnectTransSpierBufMDL!\n");
		return(ret);
	}
	for (index = 0; index < pgunionTransSpierBufHDL->maxNumOfMsg; index++)
	{
		if ((pmsgHeader = (PUnionTransSpierMsgHeader)(pgunionTransSpierBuf + conSizeOfEachTransSpierMsg * index)) == NULL)
			continue;
		memset(pmsgHeader,0,sizeof(*pmsgHeader));
	}
	return(UnionResetAllIndexAvailable(pgunionTransSpierStatusTBL));
}

// wangk add 2009-9-25

int UnionPrintTransSpierBufStatusInRecStrFormatToFile(FILE *fp)
{
	int	ret;
	
	if (fp == NULL)
		return(errCodeParameter);

	if ((ret = UnionConnectTransSpierBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionPrintTransSpierBufStatusInRecStrFormatToFile:: UnionConnectTransSpierBufMDL!\n");
		return(ret);
	}	

	fprintf(fp,"propertyName=maxSizeOfMsg|propertyValue=%d|\n",pgunionTransSpierBufHDL->maxSizeOfMsg);
	fprintf(fp,"propertyName=maxNumOfMsg|propertyValue=%d|\n",pgunionTransSpierBufHDL->maxNumOfMsg);

	return(0);
}

int UnionPrintTransSpierBufStatusInRecStrFormatToSpecFile(char *fileName)
{
	FILE	*fp;
	int	ret;

	if ((fileName == NULL) || (strlen(fileName) == 0))
	{
		UnionUserErrLog("in UnionPrintTransSpierBufStatusInRecStrFormatToSpecFile:: NULL poionter!\n");
		return(errCodeParameter);
	}

	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionPrintTransSpierBufStatusInRecStrFormatToSpecFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}

	ret = UnionPrintTransSpierBufStatusInRecStrFormatToFile(fp);
	if (fp != stdout)
		fclose(fp);
	return(ret);
}
// wangk add end 2009-9-25
