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
	printf("Usage:: %s ipAddrOfHsm zmkValue fileName\n",UnionGetApplicationName());
	return(0);
}

int InputAllKeysIntoDesKeyDB(char *ipAddrOfHsm,char *zmkValue,char *fileName)
{
	int	ret;
	long	index;
	long	successNum = 0,totalNum=0;
	FILE	*fp = NULL;
	TUnionDesKey	zmk,key;
	int	hsmHDL = -1;
	TUnionSJL06	sjl06;
	char	errCode[2+1];
	char	value[100],checkValue[100],fullKeyName[100];
	
	memset(&sjl06,0,sizeof(sjl06));
	if (!UnionIsValidIPAddrStr(ipAddrOfHsm))
	{
		UnionUserErrLog("in InputAllKeysIntoDesKeyDB:: UnionIsValidIPAddrStr [%s]!\n",ipAddrOfHsm);
		goto abnormalExit;
	}	
	strcpy(sjl06.staticAttr.ipAddr,ipAddrOfHsm);
	if ((ret = UnionSelectSJL06Rec(&sjl06)) < 0)
	{
		UnionUserErrLog("in InputAllKeysIntoDesKeyDB:: UnionSelectSJL06Rec [%s]!\n",ipAddrOfHsm);
		goto abnormalExit;
	}	
	if ((hsmHDL = UnionCreateSocketClient(sjl06.staticAttr.ipAddr,sjl06.staticAttr.port)) < 0)
	{
		UnionUserErrLog("in InputAllKeysIntoDesKeyDB:: UnionCreateSocketClient [%s]!\n",ipAddrOfHsm);
		goto abnormalExit;
	}	
	
	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in InputAllKeysIntoDesKeyDB:: UnionConnectDesKeyDB!\n");
		goto abnormalExit;
	}
	
	memset(&zmk,0,sizeof(zmk));
	if ((ret = UnionFormDefaultDesKey(&zmk,"99.tmp.zmk",zmkValue,conZMK,"")) < 0)
	{
		UnionUserErrLog("in InputAllKeysIntoDesKeyDB:: UnionFormDefaultDesKey [%s] error!\n",zmkValue);
		goto abnormalExit;
	}
	if ((fp = fopen(fileName,"r")) == NULL)	
	{
		UnionUserErrLog("in InputAllKeysIntoDesKeyDB:: fopen [%s]!\n",fileName);
		goto abnormalExit;
	}
	while (!feof(fp))
	{
		memset(fullKeyName,0,sizeof(fullKeyName));
		memset(value,0,sizeof(value));
		memset(checkValue,0,sizeof(checkValue));
		fscanf(fp,"%s%s%s",fullKeyName,value,checkValue);
		if (strlen(fullKeyName) == 0)
			continue;
		totalNum++;
		if ((index = UnionFindDesKeyPosInKeyDB(fullKeyName)) < 0)
		{
			UnionUserErrLog("in InputAllKeysIntoDesKeyDB:: UnionFindDesKeyPosInKeyDB [%s]!\n",fullKeyName);
			printf("Noexist:: %40s\n",fullKeyName);
			continue;
		}
		memcpy(&key,(pgunionDesKey+index),sizeof(key));
		strcpy(key.value,value);
		strcpy(key.checkValue,checkValue);
		memset(errCode,0,sizeof(errCode));
		if ((ret = UnionTranslateKeyUnderZMKToLMK(hsmHDL,&sjl06,&zmk,&key,errCode)) < 0)
		{
			printf("Failure:: %40s\n",key.fullName);
			continue;
		}
		if (strncmp(errCode,"00",2) != 0)
			printf("Failure:: %40s %s\n",key.fullName,errCode);
		else
		{
			if (strncmp(key.checkValue,checkValue,4) != 0)
				printf("Failure:: %40s checkValue error!\n",key.fullName);
			else
			{
				printf("Success:: %40s\n",key.fullName);
				memcpy((pgunionDesKey+index),&key,sizeof(key));
				successNum++;
			}
		}
	}
	printf("successNum = [%ld]\n",successNum);
	printf("totalNum =   [%ld]\n",totalNum);
	if ((ret = UnionMirrorDesKeyDBIntoDisk(pgunionDesKeyDB,pgunionDesKey)) < 0)
		printf("in InputAllKeysIntoDesKeyDB:: UnionMirrorDesKeyDBIntoDisk!\n");
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
	char	fileName[100],fullFileName[512];
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

	memset(fileName,0,sizeof(fileName));
	if (argc >= 4)
		strcpy(fileName,argv[3]);
inputFileName:
	if (strlen(fileName) == 0)
	{
		strcpy(fileName,p=UnionInput("请输入ZMK加密的文件名(exit退出)::"));
		if (UnionIsQuit(p))
			return(UnionTaskActionBeforeExit());
	}
	sprintf(fullFileName,"%s/tmp/%s",getenv("HOME"),fileName);
	if ((ret = InputAllKeysIntoDesKeyDB(ipAddrOfHsm,zmkValue,fullFileName)) < 0)
		printf("InputAllKeysIntoDesKeyDB Failure!\n");
	else
	{
		printf("InputAllKeysIntoDesKeyDB OK!\n");
		printf("keys restored from file [%s]\n",fullFileName);
	}
		
	return(UnionTaskActionBeforeExit());
}
