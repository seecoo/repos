//	Author:		Wolfgang Wang
//	Copyright:	Union Tech. Guangzhou
//	Date:		2003/09/30
//	Version:	1.0

#define _UnionLogMDL_3_x_	
#define _UnionEnv_3_x_	

#include <stdio.h>
#include <string.h>

#include "UnionStr.h"

#include "sjl06.h"
#include "sjl06Grp.h"
#include "unionMenu.h"
#include "unionCommand.h"
#include "unionModule.h"
#include "UnionEnv.h"

#include "sjl06SharedKeySpace.h"
#include "unionErrCode.h"
#include "UnionLog.h"

PUnionSharedMemoryModule	pgsjl06SharedKeySpaceMDL = NULL;
PUnionSJL06SharedKeySpace	pgsjl06SharedKeySpace = NULL;
PUnionSJL06SharedKeyPos		pgsjl06SharedKeyPos = NULL;

int UnionIsSharedKeyPosSpaceConnected()
{
	if ((pgsjl06SharedKeyPos == NULL) || (pgsjl06SharedKeySpace == NULL) || (pgsjl06SharedKeySpaceMDL == NULL))
		return(0);
	else
		return(1);
}

int UnionGetNameOfSharedKeyPosSpaceDef(char *fileName)
{
	sprintf(fileName,"%s/sjl06SharedKeySpace.Def",getenv("UNIONETC"));			
	return(0);
}
	
int UnionGetMaxNumOfSJL06SharedKeyPos()
{
	int		ret;
	char		fileName[512];
	char		*p;
	int		maxNumOfSharedKeyPos;

	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfSharedKeyPosSpaceDef(fileName);
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionGetMaxNumOfSJL06SharedKeyPos:: UnionInitEnvi [%s]!\n",fileName);
		return(ret);
	}

	if ((p = UnionGetEnviVarByName("maxNumOfSharedKeyPos")) == NULL)
	{
		UnionUserErrLog("in UnionGetMaxNumOfSJL06SharedKeyPos:: UnionGetEnviVarByName for [%s]\n!","maxNumOfSharedKeyPos");
		goto abnormalExit;
	}
	maxNumOfSharedKeyPos = atoi(p);
	
	UnionClearEnvi();
	return(maxNumOfSharedKeyPos);

abnormalExit:	
	UnionClearEnvi();
	return(errCodeSJL06MDL_SharedKeySpaceConfFile);
}

// Module Layer Functions
int UnionConnectSJL06SharedKeySpace()
{
	int	index;
	int	maxNumOfSharedKeyPos;
	
	if (UnionIsSharedKeyPosSpaceConnected())	// 已经连接
		return(0);
	
	if ((maxNumOfSharedKeyPos = UnionGetMaxNumOfSJL06SharedKeyPos()) <= 0)
	{
		UnionUserErrLog("in UnionConnectSJL06SharedKeySpace:: UnionGetMaxNumOfSJL06SharedKeyPos!\n");
		return(maxNumOfSharedKeyPos);
	}
	
	if ((pgsjl06SharedKeySpaceMDL = UnionConnectSharedMemoryModule(conMDLNameOfSJL06SharedKeySpace,
			sizeof(TUnionSJL06SharedKeySpace) + sizeof(TUnionSJL06SharedKeyPos) * maxNumOfSharedKeyPos)) == NULL)
	{
		UnionUserErrLog("in UnionConnectSJL06SharedKeySpace:: UnionConnectSharedMemoryModule!\n");
		return(errCodeSharedMemoryModule);
	}

	if ((pgsjl06SharedKeySpace = (PUnionSJL06SharedKeySpace)UnionGetAddrOfSharedMemoryModuleUserSpace(pgsjl06SharedKeySpaceMDL)) == NULL)
	{
		UnionUserErrLog("in UnionConnectSJL06SharedKeySpace:: PUnionSJL06SharedKeySpace!\n");
		return(errCodeSharedMemoryModule);
	}
	
	if ((pgsjl06SharedKeySpace->psjl06SharedKeyPos = (PUnionSJL06SharedKeyPos)(
		(unsigned char *)pgsjl06SharedKeySpace + sizeof(*pgsjl06SharedKeySpace))) == NULL)
	{
		UnionUserErrLog("in UnionConnectSJL06SharedKeySpace:: PUnionSJL06SharedKeyPos!\n");
		return(errCodeSharedMemoryModule);
	}
	
	pgsjl06SharedKeyPos = pgsjl06SharedKeySpace->psjl06SharedKeyPos;
	
	if (UnionIsNewCreatedSharedMemoryModule(pgsjl06SharedKeySpaceMDL))
	{
		pgsjl06SharedKeySpace->maxNumOfSharedKeyPos = maxNumOfSharedKeyPos;
		pgsjl06SharedKeySpace->currentNumOfSharedKeyPos = 0;
		return(UnionReloadSJL06SharedKeyPosSpace());
	}
	else
		return(0);
}

int UnionDisconnectSJL06SharedKeySpace()
{
	pgsjl06SharedKeySpace = NULL;

	return(UnionDisconnectShareModule(pgsjl06SharedKeySpaceMDL));
}

int UnionRemoveSJL06SharedKeySpace()
{
	pgsjl06SharedKeySpace = NULL;
	
	return(UnionRemoveSharedMemoryModule(conMDLNameOfSJL06SharedKeySpace));
}

int UnionLockSJL06SharedKeyPos(PUnionSJL06SharedKeyPos	pkeyPos)
{
	if (pkeyPos == NULL)
		return(0);
	if (!pkeyPos->locked)
		return(pkeyPos->locked = 1);
	else
		return(0);
}

int UnionUnlockSJL06SharedKeyPos(PUnionSJL06SharedKeyPos pkeyPos)
{
	if (pkeyPos == NULL)
		return(errCodeParameter);
	else
		return(pkeyPos->locked = 0);
}

int UnionAddSJL06SharedKeyPos(char *hsmGrp,char *name,long keyPos)
{
	long			i;
	PUnionSJL06SharedKeyPos	pkeyPos;
	int			ret;
	
	if ((ret = UnionConnectSJL06SharedKeySpace()) < 0)
	{
		UnionUserErrLog("in UnionAddSJL06SharedKeyPos::UnionAddSJL06SharedKeyPos!\n");
		return(ret);
	}
	
	for (i = 0; i < pgsjl06SharedKeySpace->currentNumOfSharedKeyPos; i++)
	{
		pkeyPos = pgsjl06SharedKeyPos + i;
		if ((strcmp(pkeyPos->hsmGrpID,hsmGrp) != 0) || (strcmp(pkeyPos->name,name) != 0) || (pkeyPos->keyPos != keyPos))
			continue;
		UnionUserErrLog("in UnionAddSJL06SharedKeyPos:: [%s][%s][%ld] already exists!\n",hsmGrp,name,keyPos);
		return(0);
	}
	if (pgsjl06SharedKeySpace->currentNumOfSharedKeyPos == pgsjl06SharedKeySpace->maxNumOfSharedKeyPos)
	{
		UnionUserErrLog("in UnionAddSJL06SharedKeyPos::table is full!\n");
		return(errCodeSJL06MDL_SharedKeySpaceFull);
	}
	pkeyPos = pgsjl06SharedKeyPos + pgsjl06SharedKeySpace->currentNumOfSharedKeyPos;
	pkeyPos->locked = 1;
	++pgsjl06SharedKeySpace->currentNumOfSharedKeyPos;
	strcpy(pkeyPos->hsmGrpID,hsmGrp);
	strcpy(pkeyPos->name,name);
	pkeyPos->keyPos = keyPos;
	pkeyPos->locked = 0;
	return(0);
}

long UnionLockUseAvailableSJL06SharedKeyPos(char *hsmGrp,char *name)
{
	long			i;
	PUnionSJL06SharedKeyPos	pkeyPos;
	int			j;
	int			ret;
	
	if ((ret = UnionConnectSJL06SharedKeySpace()) < 0)
	{
		UnionUserErrLog("in UnionLockUseAvailableSJL06SharedKeyPos::UnionAddSJL06SharedKeyPos!\n");
		return(ret);
	}
	
	for (j = 0; j < 100; j++)
	{	
		for (i = 0; i < pgsjl06SharedKeySpace->currentNumOfSharedKeyPos; i++)
		{
			pkeyPos = pgsjl06SharedKeyPos + i;
			if ((strcmp(pkeyPos->hsmGrpID,hsmGrp) != 0) || (strcmp(pkeyPos->name,name) != 0) || (pkeyPos->locked))
				continue;
			if (UnionLockSJL06SharedKeyPos(pkeyPos))
				return(pkeyPos->keyPos);
			continue;
		}
		usleep(100);
	}
	UnionUserErrLog("in UnionLockUseAvailableSJL06SharedKeyPos:: no avaible shared key pos for [%s][%s]\n",hsmGrp,name);
	return(errCodeSJL06MDL_NoAvaliableSharedKeySpacePos);
}

int UnionFreeLockUsedSJL06SharedKeyPos(char *hsmGrp,char *name,long index)
{
	long			i;
	PUnionSJL06SharedKeyPos	pkeyPos;
	int			j;
	int			ret;
	
	if ((ret = UnionConnectSJL06SharedKeySpace()) < 0)
	{
		UnionUserErrLog("in UnionFreeLockUsedSJL06SharedKeyPos::UnionAddSJL06SharedKeyPos!\n");
		return(ret);
	}
	
	for (i = 0; i < pgsjl06SharedKeySpace->currentNumOfSharedKeyPos; i++)
	{
		pkeyPos = pgsjl06SharedKeyPos + i;
		if ((strcmp(pkeyPos->hsmGrpID,hsmGrp) != 0) || (strcmp(pkeyPos->name,name) != 0) || (pkeyPos->keyPos != index))
			continue;
		pkeyPos->locked = 0;
		return(0);
	}
	UnionUserErrLog("in UnionFreeLockUsedSJL06SharedKeyPos:: [%s][%s][%ld] not exists\n",hsmGrp,name,index);
	return(errCodeSJL06MDL_NotSharedKeySpacePos);
}

int UnionSetSJL06SharedKeyPosSpace(char *hsmGrp,char *name,long num)
{
	long			keyPos,realNum;
	PUnionSJL06SharedKeyPos	pkeyPos;
	PUnionSJL06KeyPosSpace	pkeySpace;
	int			ret;
	
	if ((ret = UnionConnectSJL06SharedKeySpace()) < 0)
	{
		UnionUserErrLog("in UnionSetSJL06SharedKeyPosSpace::UnionAddSJL06SharedKeyPos!\n");
		return(ret);
	}
	
	if ((pkeySpace = UnionConnectKeyPosSpace(hsmGrp,name)) == NULL)
	{
		UnionUserErrLog("in UnionSetSJL06SharedKeyPosSpace:: UnionConnectKeyPosSpace [%s][%s]\n",hsmGrp,name);
		return(errCodeSJL06MDL_SharedKeySpaceNotConnected);
	}
	for (realNum = 0; realNum < num;)
	{
		if ((keyPos = UnionSelectAvailableKeyPos(pkeySpace)) < 0)
		{
			UnionUserErrLog("in UnionSetSJL06SharedKeyPosSpace:: UnionSelectAvailableKeyPos [%s][%s]!\n",hsmGrp,name);
			break;
		}
		if ((ret = UnionUseKeyPos(pkeySpace,keyPos,conKeyPosUserNameOfSharedKeySpace)) < 0)
		{
			UnionUserErrLog("in UnionSetSJL06SharedKeyPosSpace:: UnionUseKeyPos [%s][%s][%ld]!\n",hsmGrp,name,keyPos);
			continue;
		}
		realNum++;
	}
	UnionDisconnectKeyPosSpace(pkeySpace);
	return(realNum);
}

int UnionReloadSJL06SharedKeyPosSpace()
{
	int		ret;
	char		fileName[512];
	char		*p;
	int		maxNumOfSharedKeyPos;
	int		i;
	int		maxVarNum;
	char		hsmGrp[3+1];
	char		keySpaceName[3+1];
	int		loadNum = 0;
	
	if ((ret = UnionConnectSJL06SharedKeySpace()) < 0)
	{
		UnionUserErrLog("in UnionReloadSJL06SharedKeyPosSpace::UnionAddSJL06SharedKeyPos!\n");
		return(ret);
	}
	
	/*
	if ((ret = UnionClearSJL06SharedKeyPos()) < 0)
	{
		UnionUserErrLog("in UnionReloadSJL06SharedKeyPosSpace:: UnionClearSJL06SharedKeyPos!\n");
		return(ret);
	}
	*/
		
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfSharedKeyPosSpaceDef(fileName);
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionReloadSJL06SharedKeyPosSpace:: UnionInitEnvi [%s]!\n",fileName);
		return(ret);
	}

	for (i = 0,maxVarNum = UnionGetEnviVarNum(); i < maxVarNum; i++)
	{
		// 读取工作组编号
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,0)) == NULL)
		{
			UnionUserErrLog("in UnionReloadSJL06SharedKeyPosSpace:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,0);
			continue;
		}
		if (strcmp(p,"maxNumOfSharedKeyPos") == 0)
			continue;
		if (strlen(p) != 3)
			continue;
		memset(hsmGrp,0,sizeof(hsmGrp));
		strcpy(hsmGrp,p);
		// 读取密钥空间名称
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,1)) == NULL)
		{
			UnionUserErrLog("in UnionReloadSJL06SharedKeyPosSpace:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,1);
			continue;
		}
		if (strlen(p) > sizeof(keySpaceName) - 1)
			continue;
		memset(keySpaceName,0,sizeof(keySpaceName));
		strcpy(keySpaceName,p);
		if ((ret = UnionLoadSharedKeyPosIntoSJL06ShareKeyPosSpace(hsmGrp,keySpaceName)) < 0)
		{
			UnionUserErrLog("in UnionReloadSJL06SharedKeyPosSpace:: UnionLoadSharedKeyPosIntoSJL06ShareKeyPosSpace! [%s.%s]\n",hsmGrp,keySpaceName);
			continue;
		}
		loadNum += ret;
	}
	
	UnionClearEnvi();
	
	return(loadNum);	
}

int UnionPrintSJL06SharedKeySpaceToFile(FILE *fp)
{
	long			i;
	PUnionSJL06SharedKeyPos	pkeyPos;
	int			ret;
	
	if ((ret = UnionConnectSJL06SharedKeySpace()) < 0)
	{
		UnionUserErrLog("in UnionPrintSJL06SharedKeySpaceToFile::UnionAddSJL06SharedKeyPos!\n");
		return(ret);
	}
	
	for (i = 0; i < pgsjl06SharedKeySpace->currentNumOfSharedKeyPos; i++)
	{
		pkeyPos = pgsjl06SharedKeyPos + i;
		fprintf(fp,"[%s.%3s] [%8ld] [%d]\n",pkeyPos->hsmGrpID,pkeyPos->name,pkeyPos->keyPos,pkeyPos->locked);
		if ((i!= 0) && (i % 22 == 0))
		{
			if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit...")))
				break;
		}		
		continue;
	}
	
	fprintf(fp,"%d shared key pos.\n",pgsjl06SharedKeySpace->currentNumOfSharedKeyPos);
	
	return(0);
}

long UnionClearSJL06SharedKeyPos()
{
	long			i;
	PUnionSJL06SharedKeyPos	pkeyPos;
	long			pos = -1;
	int			j;
	int			ret;
	
	if ((ret = UnionConnectSJL06SharedKeySpace()) < 0)
	{
		UnionUserErrLog("in UnionClearSJL06SharedKeyPos::UnionAddSJL06SharedKeyPos!\n");
		return(ret);
	}
		
	if (pgsjl06SharedKeySpace->currentNumOfSharedKeyPos < 0)
		return(errCodeSJL06MDL_SharedKeySpacePosNum);

	for (j = 0; j < 100; j++)
	{
		for (i = 0; i < pgsjl06SharedKeySpace->currentNumOfSharedKeyPos;)
		{
			if (!UnionLockSJL06SharedKeyPos(pkeyPos = pgsjl06SharedKeyPos + i))
			{
				i++;
				continue;
			}
			memcpy(pkeyPos,pgsjl06SharedKeyPos+(pgsjl06SharedKeySpace->currentNumOfSharedKeyPos-1),sizeof(*pkeyPos));
			--pgsjl06SharedKeySpace->currentNumOfSharedKeyPos;
		}
		if (pgsjl06SharedKeySpace->currentNumOfSharedKeyPos == 0)
			return(0);
		usleep(100);
	}
	return(0);
}

long UnionForceUnlockAllSJL06SharedKeyPos()
{
	long			i;
	PUnionSJL06SharedKeyPos	pkeyPos;
	long			pos = -1;
	int			ret;
	
	if ((ret = UnionConnectSJL06SharedKeySpace()) < 0)
	{
		UnionUserErrLog("in UnionForceUnlockAllSJL06SharedKeyPos::UnionAddSJL06SharedKeyPos!\n");
		return(ret);
	}
	
	if (pgsjl06SharedKeySpace->currentNumOfSharedKeyPos < 0)
		return(errCodeSJL06MDL_SharedKeySpacePosNum);

	for (i = 0; i < pgsjl06SharedKeySpace->currentNumOfSharedKeyPos;i++)
	{
		UnionUnlockSJL06SharedKeyPos(pkeyPos = pgsjl06SharedKeyPos + i);
	}
	return(0);
}

