//	Author:		Wolfgang Wang
//	Date:		2004/11/16
//	Version:	1.0

#define _UnionLogMDL_3_x_	
#define _UnionEnv_3_x_	

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "UnionLog.h"
#include "unionModule.h"
#include "UnionEnv.h"
#include "unionCommand.h"

#include "softHsmKeySpace.h"

PUnionSharedMemoryModule		pgunionSoftHsmKeySpaceMDL = NULL;
PUnionSoftHsmKeySpace			pgunionSoftHsmKeySpace = NULL;
PUnionSoftHsmKey			pgunionSoftHsmKeyTBL = NULL;

int UnionGetConfFileNameOfSoftHsmKeySpace(char *fileName)
{
	sprintf(fileName,"%s/unionSoftHsmKeySpace.Def",getenv("UNIONETC"));
	return(0);
}

int UnionConnectSoftHsmKeySpace()
{
	int				num;
	
	if (pgunionSoftHsmKeyTBL != NULL)	// 已经连接
		return(0);
		
	if ((num = UnionGetMaxKeyNumOfSoftHsmKeySpace()) <= 0)
	{
		UnionUserErrLog("in UnionConnectSoftHsmKeySpace:: UnionGetMaxKeyNumOfSoftHsmKeySpace [%d]\n",num);
		return(num);
	}
	
	if ((pgunionSoftHsmKeySpaceMDL = UnionConnectSharedMemoryModule(conMDLNameOfSoftHsmKeySpace,
			sizeof(TUnionSoftHsmKeySpace) + sizeof(TUnionSoftHsmKey) * num)) == NULL)
	{
		UnionUserErrLog("in UnionConnectSoftHsmKeySpace:: UnionConnectSharedMemoryModule!\n");
		return(-1);
	}

	if ((pgunionSoftHsmKeySpace = (PUnionSoftHsmKeySpace)UnionGetAddrOfSharedMemoryModuleUserSpace(pgunionSoftHsmKeySpaceMDL)) == NULL)
	{
		UnionUserErrLog("in UnionConnectSoftHsmKeySpace:: PUnionSoftHsmKeySpace!\n");
		return(-1);
	}
	
	if ((pgunionSoftHsmKeyTBL = (PUnionSoftHsmKey)((unsigned char *)pgunionSoftHsmKeySpace + sizeof(*pgunionSoftHsmKeySpace))) == NULL)
	{
		UnionUserErrLog("in UnionConnectSoftHsmKeySpace:: PUnionSoftHsmKeySpace!\n");
		return(-1);
	}
	//pgunionSoftHsmKeyTBL = pgunionSoftHsmKeySpace->pkeyTBL;
	
	if (UnionIsNewCreatedSharedMemoryModule(pgunionSoftHsmKeySpaceMDL))
	{
		pgunionSoftHsmKeySpace->maxNum = num;
		pgunionSoftHsmKeySpace->realNum = 0;
		return(UnionReloadSoftHsmKeySpace());
	}
	else
		return(0);
}


int UnionDisconnectSoftHsmKeySpace()
{
	pgunionSoftHsmKeyTBL = NULL;
	pgunionSoftHsmKeySpace = NULL;
	return(UnionDisconnectShareModule(pgunionSoftHsmKeySpaceMDL));
}

int UnionRemoveSoftHsmKeySpace()
{
	UnionDisconnectSoftHsmKeySpace();
	return(UnionRemoveSharedMemoryModule(conMDLNameOfSoftHsmKeySpace));
}

int UnionReloadSoftHsmKeySpace()
{
	int			ret;
	char			fileName[512];
	char			*p;
	int			i;
	PUnionSoftHsmKey	pSoftHsmKey;
	
	if ((pgunionSoftHsmKeySpace == NULL) || (pgunionSoftHsmKeyTBL == NULL))
	{
		UnionUserErrLog("in UnionReloadSoftHsmKeySpace:: pgunionSoftHsmKeySpace is NULL!\n");
		return(-1);
	}
	
	memset(fileName,0,sizeof(fileName));
	UnionGetConfFileNameOfSoftHsmKeySpace(fileName);
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionReloadSoftHsmKeySpace:: UnionInitEnvi [%s]!\n",fileName);
		return(ret);
	}

	if ((p = UnionGetEnviVarByName("MaxNumOfKey")) == NULL)
	{
		UnionUserErrLog("in UnionReloadSoftHsmKeySpace:: UnionGetEnviVarByName for [%s]\n!","MaxNumOfKey");
		UnionClearEnvi();
		return(-1);
	}
	pgunionSoftHsmKeySpace->maxNum = atoi(p);
	pgunionSoftHsmKeySpace->realNum = 0;
		
	for (i = 0; (i < UnionGetEnviVarNum()) && (pgunionSoftHsmKeySpace->realNum < pgunionSoftHsmKeySpace->maxNum); i++)
	{
		pSoftHsmKey = pgunionSoftHsmKeyTBL + pgunionSoftHsmKeySpace->realNum;
		memset(pSoftHsmKey,0,sizeof(*pSoftHsmKey));
		// 读取组
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,0)) == NULL)
		{
			UnionUserErrLog("in UnionReloadSoftHsmKeySpace:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,0);
			continue;
		}
		if (strcmp(p,"MaxNumOfKey") == 0)
			continue;
		if (strlen(p) > sizeof(pSoftHsmKey->hsmGrp) - 1)
			continue;
		strcpy(pSoftHsmKey->hsmGrp,p);
		
		// 读取变量名
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,1)) == NULL)
		{
			UnionUserErrLog("in UnionReloadSoftHsmKeySpace:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,1);
			continue;
		}
		if (strlen(p) > sizeof(pSoftHsmKey->name) - 1)
			continue;
		strcpy(pSoftHsmKey->name,p);
		
		// 读取变量值
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,2)) == NULL)
		{
			UnionUserErrLog("in UnionReloadSoftHsmKeySpace:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,2);
			continue;
		}
		if (strlen(p) != 32)
			continue;
		strcpy(pSoftHsmKey->value,p);
		++pgunionSoftHsmKeySpace->realNum;
	}
	
	UnionClearEnvi();
		
	return(0);
}

int UnionPrintSoftHsmKeyToFile(PUnionSoftHsmKey pkey,FILE *fp)
{
	if ((pkey == NULL) || (fp == NULL))
		return(-1);
	fprintf(fp,"[%3s] [%10s] [%32s]\n",pkey->hsmGrp,pkey->name,pkey->value);
	return(0);
}

int UnionPrintSoftHsmKeySpaceToFile(FILE *fp)
{
	int	i;
	
	if ((pgunionSoftHsmKeySpace == NULL) || (pgunionSoftHsmKeyTBL == NULL) || (fp == NULL))
		return(-1);
	for (i = 0; i < pgunionSoftHsmKeySpace->realNum; i++)
	{
		if ((i != 0) && (i % 23 == 0))
		{
			if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
				break;
		}
		UnionPrintSoftHsmKeyToFile((pgunionSoftHsmKeyTBL+i),fp);
	}
	printf("SoftHsmKeyTBLNum = [%d]\n",pgunionSoftHsmKeySpace->realNum);
	if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
		return(0);
	UnionPrintSharedMemoryModuleToFile(pgunionSoftHsmKeySpaceMDL,stdout);
	return(0);
}

int UnionPrintSoftHsmKeySpaceOfSpecifiedHsmGrpToFile(char *hsmGrp,FILE *fp)
{
	int	i;
	int	printNum = 0;
	
	if ((pgunionSoftHsmKeySpace == NULL) || (pgunionSoftHsmKeyTBL == NULL) || (fp == NULL))
		return(-1);
	for (i = 0; i < pgunionSoftHsmKeySpace->realNum; i++)
	{
		if (strcmp((pgunionSoftHsmKeyTBL+i)->hsmGrp,hsmGrp) != 0)
			continue;
		if ((printNum != 0) && (printNum % 23 == 0))
		{
			if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
				break;
		}
		UnionPrintSoftHsmKeyToFile((pgunionSoftHsmKeyTBL+i),fp);
	}
	printf("SoftHsmKeyTBLNum = [%d]\n",printNum);
	if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
		return(0);
	UnionPrintSharedMemoryModuleToFile(pgunionSoftHsmKeySpaceMDL,stdout);
	return(0);
}

int UnionGetMaxKeyNumOfSoftHsmKeySpace()
{
	int		ret;
	char		fileName[512];
	char		*p;
	int		num;
		
	memset(fileName,0,sizeof(fileName));
	UnionGetConfFileNameOfSoftHsmKeySpace(fileName);
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionGetMaxKeyNumOfSoftHsmKeySpace:: UnionInitEnvi [%s]!\n",fileName);
		return(ret);
	}

	if ((p = UnionGetEnviVarByName("MaxNumOfKey")) == NULL)
	{
		UnionUserErrLog("in UnionGetMaxKeyNumOfSoftHsmKeySpace:: UnionGetEnviVarByName for [%s]\n!","MaxNumOfKey");
		num = -1;
	}
	else
		num = atoi(p);
	
	UnionClearEnvi();

	return(num);	
}

PUnionSoftHsmKey UnionReadSoftHsmKey(char *hsmGrp,char *name)
{
	int	i;
	
	if ((pgunionSoftHsmKeySpace == NULL) || (pgunionSoftHsmKeyTBL == NULL))
		return(NULL);
	for (i = 0; i < pgunionSoftHsmKeySpace->realNum; i++)
	{
		if ((strcmp(name,(pgunionSoftHsmKeyTBL+i)->name) == 0) && (strcmp(hsmGrp,(pgunionSoftHsmKeyTBL+i)->hsmGrp) == 0))
			return(pgunionSoftHsmKeyTBL+i);
	}
	return(NULL);
}

char *UnionReadSoftHsmBMK(char *hsmGrp,char *bmkIndex)
{
	char	name[40];
	PUnionSoftHsmKey	pkey;
	
	memset(name,0,sizeof(name));
	sprintf(name,"BMK");
	memcpy(name+3,bmkIndex,3);
	if ((pkey = UnionReadSoftHsmKey(hsmGrp,name)) == NULL)
		return(NULL);
	else
		return(pkey->value);
}

char *UnionReadSoftHsmICCardTransKey(char *hsmGrp,char *version,char *group,char *index)
{
	char	name[40];
	PUnionSoftHsmKey	pkey;
	
	memset(name,0,sizeof(name));
	sprintf(name,"IC");
	memcpy(name+2,version,1);
	memcpy(name+2+1,group,2);
	memcpy(name+2+1+2,index,2);
	if ((pkey = UnionReadSoftHsmKey(hsmGrp,name)) == NULL)
		return(NULL);
	else
		return(pkey->value);
}

char *UnionReadSoftHsmRacalLMK(char *hsmGrp,char *lmkPair)
{
	char	name[40];
	PUnionSoftHsmKey	pkey;
	
	memset(name,0,sizeof(name));
	sprintf(name,"LMK");
	memcpy(name+3,lmkPair,4);
	if ((pkey = UnionReadSoftHsmKey(hsmGrp,name)) == NULL)
		return(NULL);
	else
		return(pkey->value);
}

