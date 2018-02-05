// Author:	Wolfgang Wang
// Date:	2005/06/07

#define _UnionDesKeyDB_2_x_
#define _UnionLogMDL_3_x_

#include <stdio.h>
#include <string.h>

#ifndef _UnionDesKey_3_x_
#define _UnionDesKey_3_x_
#endif
#include "unionDesKeyDB.h"

#include "sjl06.h"
#include "UnionStr.h"
#include "UnionSocket.h"
#include "UnionTask.h"
#include "unionCommand.h"
#include "UnionLog.h"

extern PUnionDesKeyDB			pgunionDesKeyDB;
extern PUnionDesKey			pgunionDesKey;

PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	UnionDisconnectDesKeyDB();
	exit(0);
}


int UnionHelp()
{
	printf("Usage:: %s ipAddrOfHsm zmkValue\n",UnionGetApplicationName());
	return(0);
}

int OutputAllKeysFromDesKeyDB(char *ipAddrOfHsm,char *zmkValue,char *fileName)
{
	int	ret;
	long	i;
	long	successNum = 0;
	FILE	*fp = NULL;
	TUnionDesKey	zmk,key;
	int	hsmHDL = -1;
	TUnionSJL06	sjl06;
	char	errCode[2+1];
	
	memset(&sjl06,0,sizeof(sjl06));
	if (!UnionIsValidIPAddrStr(ipAddrOfHsm))
	{
		UnionUserErrLog("in OutputAllKeysFromDesKeyDB:: UnionIsValidIPAddrStr [%s]!\n",ipAddrOfHsm);
		goto abnormalExit;
	}	
	strcpy(sjl06.staticAttr.ipAddr,ipAddrOfHsm);
	if ((ret = UnionSelectSJL06Rec(&sjl06)) < 0)
	{
		UnionUserErrLog("in OutputAllKeysFromDesKeyDB:: UnionSelectSJL06Rec [%s]!\n",ipAddrOfHsm);
		goto abnormalExit;
	}	
	if ((hsmHDL = UnionCreateSocketClient(sjl06.staticAttr.ipAddr,sjl06.staticAttr.port)) < 0)
	{
		UnionUserErrLog("in OutputAllKeysFromDesKeyDB:: UnionCreateSocketClient [%s]!\n",ipAddrOfHsm);
		goto abnormalExit;
	}	
	
	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in OutputAllKeysFromDesKeyDB:: UnionConnectDesKeyDB!\n");
		goto abnormalExit;
	}
	
	memset(&zmk,0,sizeof(zmk));
	if ((ret = UnionFormDefaultDesKey(&zmk,"99.tmp.zmk",zmkValue,conZMK,"")) < 0)
	{
		UnionUserErrLog("in OutputAllKeysFromDesKeyDB:: UnionFormDefaultDesKey [%s] error!\n",zmkValue);
		goto abnormalExit;
	}
	if ((fp = fopen(fileName,"w")) == NULL)	
	{
		UnionUserErrLog("in OutputAllKeysFromDesKeyDB:: fopen [%s]!\n",fileName);
		goto abnormalExit;
	}
	for (i = 0; i < pgunionDesKeyDB->num; i++)
	{
		memcpy(&key,(pgunionDesKey+i),sizeof(key));
		memset(errCode,0,sizeof(errCode));
		if ((ret = UnionTranslateKeyUnderLMKToZMK(hsmHDL,&sjl06,&zmk,&key,errCode)) < 0)
			printf("Failure:: %40s\n",key.fullName);
		else
		{
			if (strncmp(errCode,"00",2) != 0)
				printf("Failure:: %40s %s\n",key.fullName,errCode);
			else
			{
				printf("Success:: %40s\n",key.fullName);
				fprintf(fp,"%40s %48s %16s\n",key.fullName,key.value,key.checkValue);
				successNum++;
			}
		}
	}
	printf("successNum = [%ld]\n",successNum);
	printf("totalNum =   [%ld]\n",pgunionDesKeyDB->num);
	fclose(fp);
	UnionCloseSocket(hsmHDL);
	return(0);
abnormalExit:
	if (fp)
		fclose(fp);
	if (hsmHDL >= 0)
		UnionCloseSocket(hsmHDL);
	return(-1);
}


int main(int argc,char *argv[])
{
	int	ret;
	char	ipAddrOfHsm[100];
	char	zmkValue[100];
	char	*p;
	char	fileName[512];
	char	datetime[100];
	
	UnionSetApplicationName(argv[0]);
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
	
	memset(ipAddrOfHsm,0,sizeof(ipAddrOfHsm));
	if (argc >= 2)
		strcpy(ipAddrOfHsm,argv[1]);
inputIPAddrOfHsm:
	if (strlen(ipAddrOfHsm) == 0)
	{
		if (UnionIsQuit(p=UnionInput("请输入密码机的IP地址(exit退出)::")))
			return(UnionTaskActionBeforeExit());
		strcpy(ipAddrOfHsm,p);
	}
	if (!UnionIsValidIPAddrStr(ipAddrOfHsm))
	{
		printf("非法的密码机IP地址，请重输!\n");
		memset(ipAddrOfHsm,0,sizeof(ipAddrOfHsm));
		goto inputIPAddrOfHsm;
	}
	
	memset(zmkValue,0,sizeof(zmkValue));
	if (argc >= 3)
		strcpy(zmkValue,argv[2]);
inputZMKValue:
	if (strlen(zmkValue) == 0)
	{
		if (UnionIsQuit(p=UnionInput("请输入ZMK的密文(exit退出)::")))
			return(UnionTaskActionBeforeExit());
		strcpy(zmkValue,p);
	}
	if (!UnionIsValidDesKeyCryptogram(zmkValue))
	{
		printf("非法的ZMK密文，请重输!\n");
		memset(zmkValue,0,sizeof(zmkValue));
		goto inputZMKValue;
	}

	memset(datetime,0,sizeof(datetime));
	UnionGetFullSystemDate(datetime);
	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/tmp/desKeyDB.output.%s.txt",getenv("HOME"),datetime);
	if ((ret = OutputAllKeysFromDesKeyDB(ipAddrOfHsm,zmkValue,fileName)) < 0)
		printf("OutputAllKeysFromDesKeyDB Failure!\n");
	else
	{
		printf("OutputAllKeysFromDesKeyDB OK!\n");
		printf("keys stored in file [%s]\n",fileName);
	}
		
	return(UnionTaskActionBeforeExit());
}
