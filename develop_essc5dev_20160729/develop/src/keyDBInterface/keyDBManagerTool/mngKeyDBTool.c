#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif
#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "UnionLog.h"
#include "unionCommand.h"
#include "unionVersion.h"
#include "UnionTask.h"

#include "highCached.h"
#include "unionHighCachedAPI.h"
#include "symmetricKeyDB.h"
#include "asymmetricKeyDB.h"

PUnionTaskInstance	ptaskInstance = NULL;

int UnionHelp()
{
	printf("Usage:: %s command\n",UnionGetApplicationName());
	printf(" Where command as:\n");
	printf("  -reload	reload keyDB\n");
	printf("  -print	print keyDB\n");
	return(0);
}

int UnionTaskActionBeforeExit()
{
	UnionDisconnectHighCached();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	return(0);
}

int PrintSymmetricKeyDB()
{
	int	i;
	int	ret;
	char	*p;
	char	highCachedKey[256];
	char	keyDBStr[4096];
	TUnionSymmetricKeyDB	symmetricKeyDB;

loopPrintSymmetricKeyDB:
	p = UnionInput("\nplease input symmetric key name(exit/quit)>");
	if (UnionIsQuit(p))
		return(0);
		
	if (UnionIsValidSymmetricKeyName(p))
	{
		printf("\n");
		
		UnionGetKeyOfHighCachedForSymmetricKeyDB(p,highCachedKey);
		if ((ret = UnionGetHighCachedValue(highCachedKey,keyDBStr,sizeof(keyDBStr))) <= 0)
		{
			printf("key[%s] is not in high cached\n",p);
			goto loopPrintSymmetricKeyDB;
		}
		
		UnionSymmetricKeyDBStringToStruct(keyDBStr,&symmetricKeyDB);
		printf("[keyName]		[%s]\n",symmetricKeyDB.keyName);
		printf("[keyGroup]		[%s]\n",symmetricKeyDB.keyGroup);
		printf("[algorithmID]		[%d]\n",symmetricKeyDB.algorithmID);
		printf("[keyType]		[%d]\n",symmetricKeyDB.keyType);
		printf("[keyLen]		[%d]\n",symmetricKeyDB.keyLen);
		printf("[inputFlag]		[%d]\n",symmetricKeyDB.inputFlag);
		printf("[outputFlag]		[%d]\n",symmetricKeyDB.outputFlag);
		printf("[effectiveDays]		[%d]\n",symmetricKeyDB.effectiveDays);
		printf("[status]		[%d]\n",symmetricKeyDB.status);
		printf("[oldVersionKeyIsUsed]	[%d]\n",symmetricKeyDB.oldVersionKeyIsUsed);
		printf("[activeDate]		[%s]\n",symmetricKeyDB.activeDate);
		printf("[checkValue]		[%s]\n",symmetricKeyDB.checkValue);
		printf("[oldCheckValue]		[%s]\n",symmetricKeyDB.oldCheckValue);
		printf("[keyUpdateTime]		[%s]\n",symmetricKeyDB.keyUpdateTime);
		printf("[keyApplyPlatform]	[%s]\n",symmetricKeyDB.keyApplyPlatform);
		printf("[keyDistributePlatform]	[%s]\n",symmetricKeyDB.keyDistributePlatform);
		printf("[creatorType]		[%d]\n",symmetricKeyDB.creatorType);
		printf("[creator]		[%s]\n",symmetricKeyDB.creator);
		printf("[createTime]		[%s]\n",symmetricKeyDB.createTime);
		printf("[usingUnit]		[%s]\n",symmetricKeyDB.usingUnit);
		printf("[remark]		[%s]\n",symmetricKeyDB.remark);

		for (i = 0; i < maxNumOfSymmetricKeyValue; i++)
		{
			if (strlen(symmetricKeyDB.keyValue[i].lmkProtectMode) == 0)
			{
				if (i == 0)
				{
					printf("\nno key value!\n");
				}
				break;
			}
			
			printf("\n***key value index[%d]***\n\n",i);
			printf("[lmkProtectMode]	[%s]\n",symmetricKeyDB.keyValue[i].lmkProtectMode);
			printf("[keyValue]		[%s]\n",symmetricKeyDB.keyValue[i].keyValue);
			printf("[oldKeyValue]		[%s]\n",symmetricKeyDB.keyValue[i].oldKeyValue);
		}
	}
	else if (strcasecmp(p,"all") == 0)
	{
		UnionPrintHighCachedKeysToFile(1,stdout,"");
	}
	else
	{
		UnionPrintHighCachedKeysToFile(1,stdout,p);
	}
	goto loopPrintSymmetricKeyDB;
}

int PrintAsymmetricKeyDB()
{
	char	*p;
	int	ret;
	char	highCachedKey[256];
	char	keyDBStr[4096];
	TUnionAsymmetricKeyDB asymmetricKeyDB;
	
loopPrintAsymmetricKeyDB:
	printf("\n*** asymmetric key ***\n\n");
	printf("*** 1:	all key\n");
	printf("*** 2:	search key\n");
	p = UnionInput("\nCommand(exit/quit)>");
	
	if (UnionIsQuit(p))
		return(0);
	else if (strcasecmp(p,"1") == 0)
	{
		UnionPrintHighCachedKeysToFile(1,stdout,"");
	}
	else if (strcasecmp(p,"2") == 0)
	{
		p = UnionInput("\nplease input asymmetric key name(exit/quit)>");
		
				if (UnionIsValidAsymmetricKeyName(p))
				{
					printf("\n");
					UnionGetKeyOfHighCachedForAsymmetricKeyDB(p,highCachedKey);
					if ((ret = UnionGetHighCachedValue(highCachedKey,keyDBStr,sizeof(keyDBStr))) <= 0)
					{
						printf("key[%s] is not in high cached\n",p);
						goto loopPrintAsymmetricKeyDB;
					}
					
					UnionAsymmetricKeyDBStringToStruct(keyDBStr,&asymmetricKeyDB);
					printf("[keyName]		[%s]\n",asymmetricKeyDB.keyName);
					printf("[keyGroup]		[%s]\n",asymmetricKeyDB.keyGroup);
					printf("[algorithmID]		[%d]\n",asymmetricKeyDB.algorithmID);
					printf("[keyType]		[%d]\n",asymmetricKeyDB.keyType);
					printf("[keyLen]		[%d]\n",asymmetricKeyDB.keyLen);
					printf("[pkExponent]		[%d]\n",asymmetricKeyDB.pkExponent);
					printf("[inputFlag]		[%d]\n",asymmetricKeyDB.inputFlag);
					printf("[outputFlag]		[%d]\n",asymmetricKeyDB.outputFlag);
					printf("[effectiveDays]		[%d]\n",asymmetricKeyDB.effectiveDays);
					printf("[status]		[%d]\n",asymmetricKeyDB.status);
					printf("[vkStoreLocation]		[%d]\n",asymmetricKeyDB.vkStoreLocation);
					printf("[oldVersionKeyIsUsed]	[%d]\n",asymmetricKeyDB.oldVersionKeyIsUsed);
					printf("[activeDate]		[%s]\n",asymmetricKeyDB.activeDate);	
					printf("[pkValue]		[%s]\n",asymmetricKeyDB.pkValue);
					printf("[pkCheckValue]		[%s]\n",asymmetricKeyDB.pkCheckValue);
					printf("[oldPKValue]		[%s]\n",asymmetricKeyDB.oldPKValue);
					printf("[oldPKCheckValue]		[%s]\n",asymmetricKeyDB.oldPKCheckValue);
					printf("[vkValue]		[%s]\n",asymmetricKeyDB.vkValue);
					printf("[vkCheckValue]		[%s]\n",asymmetricKeyDB.vkCheckValue);
					printf("[oldVKValue]		[%s]\n",asymmetricKeyDB.oldVKValue);
					printf("[oldVKCheckValue]		[%s]\n",asymmetricKeyDB.oldVKCheckValue);
					printf("[keyUpdateTime]		[%s]\n",asymmetricKeyDB.keyUpdateTime);
					printf("[keyApplyPlatform]	[%s]\n",asymmetricKeyDB.keyApplyPlatform);
					printf("[keyDistributePlatform]	[%s]\n",asymmetricKeyDB.keyDistributePlatform);
					printf("[creatorType]		[%d]\n",asymmetricKeyDB.creatorType);
					printf("[creator]		[%s]\n",asymmetricKeyDB.creator);
					printf("[createTime]		[%s]\n",asymmetricKeyDB.createTime);
					printf("[usingUnit]		[%s]\n",asymmetricKeyDB.usingUnit);
					printf("[remark]		[%s]\n",asymmetricKeyDB.remark);
				}
				else
				{
					printf("key[%s] is not in high cached\n",p);
					printf("\n");
				}
	}
	goto loopPrintAsymmetricKeyDB;
}

int PrintKeyDB()
{
	char	*p;
	
loopPrint:
	printf("\n*** 1:	symmetric  key\n");
	printf("*** 2:	asymmetric key\n");
	p = UnionInput("\nplease choose(exit/quit)>");
	if (UnionIsQuit(p))
		return(UnionTaskActionBeforeExit());
	
	if (strcmp(p,"1") == 0)
		PrintSymmetricKeyDB();
	if (strcmp(p,"2") == 0)
		PrintAsymmetricKeyDB();
	
	goto loopPrint;
}

int main(int argc,char *argv[])
{
	int	ret;
		
	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		return(UnionHelp());
		
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
	
	/*
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}
	*/

	// 检查高速缓存是否开启
	/*if (!UnionIsUseHighCached())
	{
		printf("高速缓存未启用，请先启用高速缓存!\n");
		return(UnionTaskActionBeforeExit());
	}*/

	if (strcasecmp(argv[1],"-RELOAD") == 0)
	{
		if (!UnionConfirm("Are you sure to reload key to memory?"))
		{
			return(-1);
		}
		if((ret = UnionReloadKeyDB()) < 0)
		{
			printf("*****keyDB reload fail!\n");
		}
		else
		{
			printf("*****keyDB reload success!\n");
		}
	}
	else if (strcasecmp(argv[1],"-PRINT") == 0)
	{
		return(PrintKeyDB());
	}
	else
	{
		return(UnionHelp());	
	}
	return(UnionTaskActionBeforeExit());
}
