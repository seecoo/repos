//	Wolfgang Wang
//	2003/09/09

// 2007/12/11,增加了writinglockdb和readinglockdb两个命令，这两个命令仅用于测试目的

#define _UnionTask_3_x_
#define _UnionDesKeyDB_2_x_
#define _UnionLogMDL_3_x_

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <time.h>
#include <signal.h>

#ifndef _UnionDesKey_3_x_
#define _UnionDesKey_3_x_
#endif

#include "UnionLog.h"
#include "unionCommand.h"
#include "unionVersion.h"
#include "UnionTask.h"
#include "unionModule.h"

#include "unionDesKeyDB.h"

TUnionDesKey		gdesKey;

PUnionTaskInstance	ptaskInstance = NULL;

void UnionSignalExit(int n);

void UnionSignalExit(int n)
{
	UnionTaskActionBeforeExit();
}

int UnionTaskActionBeforeExit()
{
	UnionDisconnectDesKeyDB();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);	//return(exit(0));
}

int UnionHelp()
{
	printf("Usage:: %s command\n",UnionGetApplicationName());
	printf(" Command\n");
	printf("	create	maxKeyNum  Create a des key db\n");
	printf("	drop	Drop a des key db\n");
	printf("	reload  reload desKeyDB into shared memory\n");
	printf("	connect Connect to desKeyDB\n");
	printf("	mirror\n");
	printf("	maxkeynum newMaxKeyNum change maxKeyNum of the des key db\n");
	printf("	insert|insertanyway	fullKeyName keyType keyLength container keyValue keyCheckValue\n");
	printf("	update	fullKeyName keyValue keyCheckValue\n");
	printf("	delete	fullKeyName Delete a des key\n");
	printf("	deleteallofapp	idOfApp Delete all des keys of app\n");
	printf("	print	fullKeyName Print a des key\n");
	printf("	read	fullKeyName read a des key\n");
	printf("	use	fullKeyName use a des key\n");
	printf("	all 	Print all des keys\n");
	printf("	allvalue 	Print all des key value\n");
	printf("	app 	print all des keys of application\n");
	printf("	owner	print all des keys of owner\n");
	printf("	name	print all des keys of name\n");
	printf("	pcontainer 	print all des keys of container\n");
	printf("	like	print all des keys like {app,owner,name}\n");
	printf("	status 	Print keydb status\n");
	printf("	edit	edit a des key\n");
	printf("	length fullKeyName newKeyLength\n");
	printf("	activeDate fullKeyName newActiveDate\n");
	printf("	maxEffectiveDays fullKeyName newMaxEffectiveDays\n");
	printf("	maxUseTimes fullKeyName newMaxUseTimes\n");
	printf("	useoldver fullKeyName\n");
	printf("	unuseoldver fullKeyName\n");
	printf("	window	fullKeyName newWindowBetweenKeyVer\n");
	printf("	container containerName\n");
	printf("	unlockdb unlock KeyDB\n");
	printf("	unlockallkey unlock all locked key\n");
	printf("	alllockedkey print all the locked key\n");
	printf("	outdate [days] Print all keys which are outofdate or will be outofdate after days.\n");
	printf("	outtimes [times] Print all keys which exceeded maxUseTimes or will exceed after times.\n");
	printf("	recsize	print size of a des key record\n");
	printf("	loop looptimes fullKeyName  Loop testing\n");

	EditHelp();
	
	return(0);
}

int EditHelp()
{
	printf(" edit commands\n");
	printf("	insert	Insert des key\n");
	printf("	delete	Delete des key\n");
	printf("	updateattr update des key attr\n");
	printf("	updatevalue update des key value\n");	
	printf("	print	print des key\n");
	printf("	read	read des key\n");
	printf("	use	use des key\n");
	printf("	all	Print all des keys\n");
	printf("	allvalue	Print all des key value\n");
	printf("	app 	print all des keys of application\n");
	printf("	owner	print all des keys of owner\n");
	printf("	name	print all des keys of name\n");
	printf("	pcontainer	print all des keys of container\n");
	printf("	like	print all des keys like {app,owner,name}\n");
	printf("	status	print keydb status\n");	
	printf("	length	Input des key length\n");
	printf("	type	Input des key type\n");
	printf("	value	Input des key value\n");
	printf("	checkvalue Input des key check value\n");
	printf("	activedate Input des key active date\n");
	printf("	maxEffectiveDays Input des key maxEffectiveDays\n");
	printf("	maxUseTimes Input des key newMaxUseTimes\n");
	printf("	window	Input des key newWindowBetweenKeyVer\n");
	printf("	container Input des key container\n");
	printf("	fullname Input des key fullname\n");
	printf("	useoldver\n");
	printf("	unuseoldver\n");
	printf("	outdate print outofdate desKeys.\n");
	printf("	outtimes print outoftimes desKeys.\n");
	printf("	unlockallkey unlock all locked key\n");
	printf("	alllockedkey print all the locked key\n");
	printf("	lock lock current key\n");
	return(0);
}


//int CreateKeyDB()
int CreateDesKeyDB(int argc,char *argv[])
{
	int	ret;
	long	gmaxKeyNum;
	
	if (argc > 0)
		gmaxKeyNum = atol(argv[0]);
	else
	{
loopUnionInput2:
		if ((gmaxKeyNum = atoi(UnionInput("Input max num of keys::"))) <= 0)
		{
			goto loopUnionInput2;
		}
	}
	
	if (UnionConfirm("Create DesKeyDB of maxKeyNum = [%ld]?",gmaxKeyNum))
	{
		if ((ret = UnionCreateDesKeyDB(gmaxKeyNum)) < 0)
			printf("UnionCreateDesKeyDB Error! ret = [%d]\n",ret);
		else
			printf("UnionCreateDesKeyDB OK!\n");
	}
	return(UnionTaskActionBeforeExit());
}

//int DropKeyDB()
int DropDesKeyDB()
{
	int	ret;
	
	if (UnionConfirm("Drop DesKeyDB?"))
	{
		if ((ret = UnionDeleteDesKeyDB()) < 0)
			printf("UnionDeleteDesKeyDB Error! ret = [%d]\n",ret);
		else
			printf("UnionDeleteDesKeyDB OK!\n");
	}
	return(UnionTaskActionBeforeExit());
}

int InsertDesKey(int argc,char *argv[],short confirmRequired)
{
	int		ret;
	
	if (argc > 0)
	{
		if (!UnionIsValidDesKeyFullName(argv[0]))
		{
			printf("Invalid full name [%s]\n",argv[0]);
			return(UnionTaskActionBeforeExit());
		}
		strcpy(gdesKey.fullName,argv[0]);
	}
	
	if (argc > 1)
	{
		if (!UnionIsValidDesKeyType(UnionConvertDesKeyType(argv[1])))
		{
			printf("Invalid des key type [%s]!\n",argv[1]);
			return(UnionTaskActionBeforeExit());
		}
		gdesKey.type = UnionConvertDesKeyType(argv[1]);
	}
	if (argc > 2)
	{
		if ((gdesKey.length = UnionConvertDesKeyLength(atoi(argv[2]))) < 0)
		{
			printf("Invalid des key length [%s]!\n",argv[2]);
			return(UnionTaskActionBeforeExit());
		}
	}
	if (argc > 3)
	{
		if (strlen(argv[3]) >= sizeof(gdesKey.container))
		{
			printf("Invalid container length [%s]!\n",argv[3]);
			return(UnionTaskActionBeforeExit());
		}
		strcpy(gdesKey.container,argv[3]);
	}
	
	if (argc > 4)
	{
		if (!UnionIsValidDesKeyCryptogram(argv[4]))
		{
			printf("Invalid des key cryptogram [%s]!\n",argv[4]);
			return(UnionTaskActionBeforeExit());
		}
		strcpy(gdesKey.value,argv[4]);
	}
	if (argc > 5)
	{
		if (!UnionIsValidDesKeyCheckValue(argv[5]))
		{
			printf("Invalid des key check value [%s]!\n",argv[5]);
			return(UnionTaskActionBeforeExit());
		}
		strcpy(gdesKey.checkValue,argv[5]);
	}
	if (argc > 6)
	{
		if (strlen(argv[6]) != 8)
		{
			printf("Invalid des key active date [%s]!\n",argv[6]);
			return(UnionTaskActionBeforeExit());
		}
		strcpy(gdesKey.activeDate,argv[6]);
	}
	if (argc > 7)
	{
		gdesKey.maxEffectiveDays = atol(argv[7]);
	}
	
	if (argc > 0)
	{
		if (confirmRequired)
		{
			UnionPrintDesKey(&gdesKey);
			if (!UnionConfirm("Are you sure of inserting this des key?"))
				return(UnionTaskActionBeforeExit());
		}
		if ((ret = UnionInsertDesKeyIntoKeyDB(&gdesKey)) < 0)
			printf("UnionInsertDesKeyIntoKeyDB %s error! ret = [%d]\n",gdesKey.fullName,ret);
		else
			printf("UnionInsertDesKeyIntoKeyDB %s OK!\n",gdesKey.fullName);
	}
	return(UnionTaskActionBeforeExit());
}

int DeleteDesKey(int argc,char *argv[],short confirmRequired)
{
	int	ret;
	int	i;
	
	for (i = 0; i < argc; i++)
	{	
		if (confirmRequired)
		{
			if (!UnionConfirm("Are you sure of deleting des key named [%s]?",argv[i]))
				continue;
		}
		if ((ret = UnionDeleteDesKeyFromKeyDB(argv[i])) < 0)
			printf("UnionDeleteDesKeyFromKeyDB [%s] Error! ret = [%d]\n",argv[i],ret);
		else
			printf("UnionDeleteDesKeyFromKeyDB [%s] OK!\n",argv[i]);
	}
	return(UnionTaskActionBeforeExit());
}	

int PrintDesKey(int argc,char *argv[])
{
	int	ret;
	int	i;
	
	for (i = 0; i < argc; i++)
	{
		if (!UnionIsValidDesKeyFullName(argv[i]))
		{
			printf("Invalid fullName [%s]!\n");
			continue;
		}
		memset(&gdesKey,0,sizeof(gdesKey));
		strcpy(gdesKey.fullName,argv[i]);
		if ((ret = UnionReadDesKeyFromKeyDBAnyway(&gdesKey)) < 0)
			printf("UnionReadDesKeyFromKeyDBAnyway [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
		else
			UnionPrintDesKey(&gdesKey);
	}
	
	return(UnionTaskActionBeforeExit());
}

int ReadDesKey(int argc,char *argv[])
{
	int	ret;
	int	i;
	
	for (i = 0; i < argc; i++)
	{
		if (!UnionIsValidDesKeyFullName(argv[i]))
		{
			printf("Invalid fullName [%s]!\n");
			continue;
		}
		memset(&gdesKey,0,sizeof(gdesKey));
		strcpy(gdesKey.fullName,argv[i]);
		if ((ret = UnionReadDesKeyFromKeyDBAnyway(&gdesKey)) < 0)
			printf("UnionReadDesKeyFromKeyDBAnyway [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
		else
			UnionPrintDesKey(&gdesKey);
	}
	
	return(UnionTaskActionBeforeExit());
}

int UseDesKey(int argc,char *argv[])
{
	int	ret;
	int	i;
	
	for (i = 0; i < argc; i++)
	{
		if (!UnionIsValidDesKeyFullName(argv[i]))
		{
			printf("Invalid fullName [%s]!\n");
			continue;
		}
		memset(&gdesKey,0,sizeof(gdesKey));
		strcpy(gdesKey.fullName,argv[i]);
		if ((ret = UnionUseDesKeyFromKeyDB(&gdesKey)) < 0)
			printf("UnionUseDesKeyFromKeyDB [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
		else
			UnionPrintDesKey(&gdesKey);
	}
	
	return(UnionTaskActionBeforeExit());
}

int PrintAllDesKeyForApp(int argc,char *argv[])
{
	int	ret;
	int	i;
	
	if (argc <= 0)
	{
		UnionPrintDesKeyExpectedToFile(UnionInput("Input AppID::"),"","",stdout);
	}
	for (i = 0; i < argc; i++)
		UnionPrintDesKeyExpectedToFile(argv[i],"","",stdout);
			
	return(UnionTaskActionBeforeExit());
}

int PrintAllDesKeyForContainer(int argc,char *argv[])
{
	int	ret;
	int	i;
	
	if (argc <= 0)
	{
		UnionPrintDesKeyOfContainerInKeyDBToFile(UnionInput("Input Container::"),stdout);
	}
	for (i = 0; i < argc; i++)
		UnionPrintDesKeyOfContainerInKeyDBToFile(argv[i],stdout);
			
	return(UnionTaskActionBeforeExit());
}

int PrintAllDesKeyForOwner(int argc,char *argv[])
{
	int	ret;
	int	i;
	
	if (argc <= 0)
	{
		UnionPrintDesKeyExpectedToFile("",UnionInput("Input Owner::"),"",stdout);
	}
	for (i = 0; i < argc; i++)
		UnionPrintDesKeyExpectedToFile("",argv[i],"",stdout);
			
	return(UnionTaskActionBeforeExit());
}

int PrintAllDesKeyForName(int argc,char *argv[])
{
	int	ret;
	int	i;
	
	if (argc <= 0)
	{
		UnionPrintDesKeyExpectedToFile("","",UnionInput("Input Name::"),stdout);
	}

	for (i = 0; i < argc; i++)
		UnionPrintDesKeyExpectedToFile("","",argv[i],stdout);
			
	return(UnionTaskActionBeforeExit());
}

int PrintAllDesKeyExpected(int argc,char *argv[])
{
	int	ret;
	char	tmpBuf1[128];
	
	if (argc <= 0)
	{
		memset(tmpBuf1,0,sizeof(tmpBuf1));
		strcpy(tmpBuf1,UnionInput("Input AppID::"));
		UnionPrintDesKeyExpectedToFile(tmpBuf1,UnionInput("Input Owner::"),"",stdout);
	}
	else if (argc == 1)
		UnionPrintDesKeyExpectedToFile(argv[0],"","",stdout);
	else if (argc == 2)
		UnionPrintDesKeyExpectedToFile(argv[0],argv[1],"",stdout);
	else if (argc >= 3)
		UnionPrintDesKeyExpectedToFile(argv[0],argv[1],argv[2],stdout);
	return(UnionTaskActionBeforeExit());
}

int LoopTest(int argc,char *argv[])
{
	long	loopTimes;
	long	i;
	time_t	start,finish;
	int	ret;
	char	*p;

	if (argc > 0)
		loopTimes = atol(argv[0]);
	else
		loopTimes = atol(UnionInput("\nUnionInput LoopTimes::"));
	if (argc > 1)
	{
		if (!UnionIsValidDesKeyFullName(argv[1]))
		{
			printf("Invalid full name [%s]\n",argv[1]);
			return(UnionTaskActionBeforeExit());
		}
		strcpy(gdesKey.fullName,argv[1]);
	}
	else
	{
		if (!UnionIsValidDesKeyFullName(p = UnionInput("\nDesKeyFullName::")))
		{
			printf("Invalid full name [%s]\n",p);
			return(UnionTaskActionBeforeExit());
		}
		strcpy(gdesKey.fullName,p);
	}
		
	if (!UnionConfirm("Are you sure to loop test [%ld] times?\n",loopTimes))
		return(UnionTaskActionBeforeExit());
		
	time(&start);
	for (i = 0; i < loopTimes; i++)
	{
		if ((ret = UnionReadDesKeyFromKeyDBAnyway(&gdesKey)) < 0)
		{
			printf("UnionReadDesKeyFromKeyDBAnyway [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
			return(ret);
		}
		if (i % 100000 == 0)
		{
			time(&finish);
			printf("[%012d] times used [%012ld]!\n",i,finish-start);
		}
	}
	time(&finish);
	printf("TotalTimes Used [%012ld] for [%012ld].\n",finish-start,loopTimes);
	if (finish - start > 0)
		printf("Times/Seconds = [%012ld]\n",loopTimes/(finish-start));
	return(UnionTaskActionBeforeExit());
}

int EditDesKey()
{
	char	*p;
	int	ret;
	//TUnionDesKey	defaultKey;
		
	UnionFormDefaultDesKey(&gdesKey,"00.null.00","",conZPK,"");
loopEdit:
	p = UnionInput("\n\nCommand>");
	if (UnionIsValidDesKeyFullName(p))
	{
		strcpy(gdesKey.fullName,p);
		if ((ret = UnionReadDesKeyFromKeyDBAnyway(&gdesKey)) < 0)
		{
			printf("%s not exists!\n",gdesKey.fullName);
			memset(gdesKey.value,0,sizeof(gdesKey.value));
			memset(gdesKey.checkValue,0,sizeof(gdesKey.checkValue));
			memset(gdesKey.oldValue,0,sizeof(gdesKey.oldValue));
			memset(gdesKey.oldCheckValue,0,sizeof(gdesKey.oldCheckValue));
		}			
		else
			UnionPrintDesKey(&gdesKey);			
		goto loopEdit;
	}
	if (UnionIsQuit(p))
		return(UnionTaskActionBeforeExit());
	if (strcasecmp(p,"INSERT") == 0)
		EditInsert();
	if (strcasecmp(p,"DELETE") == 0)
		EditDelete();
	if (strcasecmp(p,"UPDATEVALUE") == 0)
		EditUpdateValue();
	if (strcasecmp(p,"UPDATEATTR") == 0)
		EditUpdate();
	if (strcasecmp(p,"PRINT") == 0)
		EditPrint();
	if (strcasecmp(p,"READ") == 0)
		EditRead();
	if (strcasecmp(p,"USE") == 0)
		EditUse();
	if (strcasecmp(p,"ALL") == 0)
		EditPrintAllKeys();
	if (strcasecmp(p,"ALLVALUE") == 0)
		EditPrintAllKeysValue();
	if (strcasecmp(p,"STATUS") == 0)
		EditPrintStatus();
	if (strcasecmp(p,"LENGTH") == 0)
		UnionInputDesKeyLength();
	if (strcasecmp(p,"TYPE") == 0)
		UnionInputDesKeyType();
	if (strcasecmp(p,"VALUE") == 0)
		UnionInputDesKeyValue();
	if (strcasecmp(p,"CHECKVALUE") == 0)
		UnionInputDesKeyCheckValue();
	if (strcasecmp(p,"ACTIVEDATE") == 0)
		UnionInputDesKeyActiveDate();
	if (strcasecmp(p,"MAXEFFECTIVEDAYS") == 0)
		UnionInputDesKeyMaxEffectiveDays();
	if (strcasecmp(p,"MAXUSETIMES") == 0)
		UnionInputDesKeyMaxUseTimes();
	if (strcasecmp(p,"WINDOW") == 0)
		UnionInputDesKeyWindowBetweenKeyVer();
	if (strcasecmp(p,"CONTAINER") == 0)
		UnionInputDesKeyContainer();
	if (strcasecmp(p,"USEOLDKEY") == 0)
	{
		gdesKey.oldVerEffective = 1;
		printf("set old version key effective!\n");
	}
	if (strcasecmp(p,"UNUSEOLDKEY") == 0)
	{
		gdesKey.oldVerEffective = 0;
		printf("set old version key ineffective!\n");
	}
	if (strcasecmp(p,"FULLNAME") == 0)
		UnionInputDesKeyFullName();
	if (strcasecmp(p,"APP") == 0)
		EditPrintDesKeyOfApp();
	if (strcasecmp(p,"PCONTAINER") == 0)
		EditPrintDesKeyOfContainer();
	if (strcasecmp(p,"OWNER") == 0)
		EditPrintDesKeyOfOwner();
	if (strcasecmp(p,"NAME") == 0)
		EditPrintDesKeyOfName();
	if (strcasecmp(p,"LIKE") == 0)
		EditPrintDesKeyOfExpected();
	if (strcasecmp(p,"HELP") == 0)
		EditHelp();
	if (strcasecmp(p,"OUTDATE") == 0)
		EditOutofDate();
	if (strcasecmp(p,"OUTTIMES") == 0)
		EditOutofTimes();
	if (strcasecmp(p,"UNLOCKALLKEY") == 0)
	{
		if ((ret = UnionUnlockAllLockedDesKeyInDesKeyDB()) < 0)
			printf("UnionUnlockAllLockedDesKeyInDesKeyDB Failure! ret = [%d]\n",ret);
		else
			printf("UnionUnlockAllLockedDesKeyInDesKeyDB OK!\n");
	}
	if (strcasecmp(p,"ALLLOCKEDKEY") == 0)
	{
		if ((ret = UnionPrintAllLockedDesKeyInDesKeyDBToFile(stdout)) < 0)
			printf("UnionPrintAllLockedDesKeyInDesKeyDBToFile Failure! ret = [%d]\n",ret);
	}
	if (strcasecmp(p,"LOCK") == 0)	// 2008/5/15增加
	{
		if (UnionConfirm("Are you sure to lock [%s]",gdesKey.fullName))
		{
			if ((ret = UnionLockDesKeyInDesKeyDB(gdesKey.fullName)) < 0)
				printf("UnionLockDesKeyInDesKeyDB [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
			else
				printf("UnionLockDesKeyInDesKeyDB [%s] OK!\n",gdesKey.fullName);
		}
	}
	goto loopEdit;
}
		
int EditOutofDate()
{
	int	ret;
	char	*p;
	long	days;
	
loop:
	if (UnionIsQuit(p = UnionInput("Input days after which the des keys will be outofdate,\ninput -1 to display all outofdate des keys.\ndays(exit or quit to exit)::\n")))
		return(0);
	if ((days = atol(p)) < 0)	
	{
		if ((ret = UnionPrintDesKeyOutofDateToFile(stdout)) < 0)
			printf("UnionPrintDesKeyOutofDateToFile Error! ret = [%d]\n",ret);
	}
	else
	{
		if ((ret = UnionPrintDesKeyNearOutofDateToFile(days,stdout)) < 0)
			printf("UnionPrintDesKeyNearOutofDateToFile Error! ret = [%d]\n",ret);
	}
	goto loop;
}

int EditOutofTimes()
{
	int	ret;
	char	*p;
	long	times;
	
loop:
	if (UnionIsQuit(p = UnionInput("Input times after which the des keys will be outofdate,\ninput -1 to display all outofdate des keys.\ntimes(exit or quit to exit)::\n")))
		return(0);
	if ((times = atol(p)) < 0)	
	{
		if ((ret = UnionPrintDesKeyOutofDateToFile(stdout)) < 0)
			printf("UnionPrintDesKeyOutofDateToFile Error! ret = [%d]\n",ret);
	}
	else
	{
		if ((ret = UnionPrintDesKeyNearOutofMaxUseTimesToFile(times,stdout)) < 0)
			printf("UnionPrintDesKeyNearOutofMaxUseTimesToFile Error! ret = [%d]\n",ret);
	}
	goto loop;
}

int EditInsert()
{
	int	ret;
	
	UnionPrintDesKey(&gdesKey);
	if (!UnionIsValidDesKey(&gdesKey))
	{
		printf("Des Key Invalid!\n");
		return(-1);
	}
	if (!UnionConfirm("Are you sure to add this des key?"))
		return(0);
	if ((ret = UnionInsertDesKeyIntoKeyDB(&gdesKey)) < 0)
		printf("UnionInsertDesKeyIntoKeyDB [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
	else
		printf("UnionInsertDesKeyIntoKeyDB [%s] OK!\n",gdesKey.fullName);
	return(ret);
}

int EditDelete()
{
	int	ret;
	
	UnionPrintDesKey(&gdesKey);
	if (!UnionConfirm("Are you sure to delete this des key?"))
		return(0);
	if ((ret = UnionDeleteDesKeyFromKeyDB(gdesKey.fullName)) < 0)
		printf("UnionDeleteDesKeyFromKeyDB [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
	else
		printf("UnionDeleteDesKeyFromKeyDB [%s] OK!\n",gdesKey.fullName);
	return(ret);
}	
	
int EditUpdate()
{
	int	ret;
	
	UnionPrintDesKey(&gdesKey);
	if (!UnionIsValidDesKey(&gdesKey))
	{
		printf("Des Key Invalid!\n");
		return(-1);
	}
	if (!UnionConfirm("Are you sure to update this des key?"))
		return(0);
	if ((ret = UnionUpdateDesKeyAttrInKeyDB(&gdesKey)) < 0)
		printf("UnionUpdateDesKeyAttrInKeyDB [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
	else
		printf("UnionUpdateDesKeyAttrInKeyDB [%s] OK!\n",gdesKey.fullName);
	return(ret);
}

int EditUpdateValue()
{
	int	ret;
	
	UnionPrintDesKey(&gdesKey);
	if (!UnionIsValidDesKey(&gdesKey))
	{
		printf("Des Key Invalid!\n");
		return(-1);
	}
	if (!UnionConfirm("Are you sure to update value of this des key?"))
		return(0);
	if ((ret = UnionUpdateDesKeyValueInKeyDB(gdesKey.fullName,gdesKey.value,gdesKey.checkValue)) < 0)
		printf("UnionUpdateDesKeyValueInKeyDB [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
	else
		printf("UnionUpdateDesKeyValueInKeyDB [%s] OK!\n",gdesKey.fullName);
	return(ret);
}

int EditPrint()
{
	int	ret;
	
	if ((ret = UnionReadDesKeyFromKeyDBAnyway(&gdesKey)) < 0)
		printf("UnionReadDesKeyFromKeyDBAnyway [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
	else
		UnionPrintDesKey(&gdesKey);
	return(ret);
}

int EditRead()
{
	int	ret;
	
	if ((ret = UnionReadDesKeyFromKeyDBAnyway(&gdesKey)) < 0)
		printf("UnionReadDesKeyFromKeyDBAnyway [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
	else
		UnionPrintDesKey(&gdesKey);
	return(ret);
}

int EditUse()
{
	int	ret;
	
	if ((ret = UnionUseDesKeyFromKeyDB(&gdesKey)) < 0)
		printf("UnionUseDesKeyFromKeyDB [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
	else
		UnionPrintDesKey(&gdesKey);
	return(ret);
}


int EditPrintStatus()
{
	int	ret;
	
	if ((ret = UnionPrintStatusOfDesKeyDBToFile(stdout)) < 0)
		printf("UnionReadDesKeyFromKeyDBAnyway Error! ret = [%d]\n",ret);
	return(ret);
}

int EditPrintAllKeys()
{
	int	ret;
	
	if ((ret = UnionPrintAllDesKeysInKeyDBToFile(stdout)) < 0)
		printf("UnionPrintAllDesKeysInKeyDBToFile Error! ret = [%d]\n",ret);
	return(ret);
}

// Mary add, 20080520
int EditPrintAllKeysValue()
{
	int	ret;
	
	if ((ret = UnionPrintAllDesKeyValueToFile(stdout)) < 0)
		printf("UnionPrintAllDesKeyValueToFile Error! ret = [%d]\n",ret);
	return(ret);
}

int EditPrintDesKeyOfApp()
{
	int	ret;
	char	*p;
	char	tmpBuf[128];

loop:	
	memset(tmpBuf,0,sizeof(tmpBuf));
	p = UnionInput("Input AppID::");
	strcpy(tmpBuf,p);
	if (UnionIsQuit(p))
		return(0);
	UnionPrintDesKeyExpectedToFile(tmpBuf,"","",stdout);
	goto loop;
}

int EditPrintDesKeyOfContainer()
{
	int	ret;
	char	*p;
	char	tmpBuf[128];

loop:	
	memset(tmpBuf,0,sizeof(tmpBuf));
	p = UnionInput("Input Container::");
	strcpy(tmpBuf,p);
	if (UnionIsQuit(p))
		return(0);
	UnionPrintDesKeyOfContainerInKeyDBToFile(tmpBuf,stdout);
	goto loop;
}

int EditPrintDesKeyOfOwner()
{
	int	ret;
	char	*p;
	char	tmpBuf[128];

loop:	
	memset(tmpBuf,0,sizeof(tmpBuf));
	p = UnionInput("Input Owner::");
	strcpy(tmpBuf,p);
	if (UnionIsQuit(p))
		return(0);
	UnionPrintDesKeyExpectedToFile("",tmpBuf,"",stdout);
	goto loop;
}


int EditPrintDesKeyOfName()
{
	int	ret;
	char	*p;
	char	tmpBuf[128];

loop:	
	memset(tmpBuf,0,sizeof(tmpBuf));
	p = UnionInput("Input Name::");
	strcpy(tmpBuf,p);
	if (UnionIsQuit(p))
		return(0);
	UnionPrintDesKeyExpectedToFile("","",tmpBuf,stdout);
	goto loop;
}

int EditPrintDesKeyOfExpected()
{
	int	ret;
	char	*p;
	char	tmpBuf1[128];
	char	tmpBuf2[128];

loop:	
	memset(tmpBuf1,0,sizeof(tmpBuf1));
	p = UnionInput("Input AppID::");
	strcpy(tmpBuf1,p);
	if (UnionIsQuit(p))
		return(0);
	p = UnionInput("Input Owner::");
	memset(tmpBuf2,0,sizeof(tmpBuf2));
	strcpy(tmpBuf2,p);
	if (UnionIsQuit(p))
		return(0);
	UnionPrintDesKeyExpectedToFile(tmpBuf1,tmpBuf2,"",stdout);
	goto loop;
}

int UnionInputDesKeyLength()
{
	char *param;
	
loopUnionInput:
	param = UnionInput("UnionInput Des Key Length:(64/128/192/quit)");
	UnionToUpperCase(param);
	if (strcasecmp(param,"QUIT") == 0)
		return(0);

	if (strcasecmp(param,"64") == 0)
		gdesKey.length = con64BitsDesKey;
	else if (strcasecmp(param,"128") == 0)
		gdesKey.length = con128BitsDesKey;
	else if (strcasecmp(param,"192") == 0)
		gdesKey.length = con192BitsDesKey;
	else
	{
		printf("Invalid Des Key Length [%s]!\n",param);
		goto loopUnionInput;
	}
	
	return(0);
}

int UnionInputDesKeyValue()
{
	char	*param;
	
loopUnionInput:
	switch (gdesKey.length)
	{
		case	con64BitsDesKey:
			param = UnionInput("UnionInput 64 Bits DesKey,quit to exit\n++++++++++++++++\n");
			break;
		case	con128BitsDesKey:
			param = UnionInput("UnionInput 128 Bits DesKey,quit to exit\n++++++++++++++++++++++++++++++++\n");
			break;
		case	con192BitsDesKey:
			param = UnionInput("UnionInput 192 Bits DesKey,quit to exit\n++++++++++++++++++++++++++++++++++++++++++++++++\n");
			break;
		default:
			printf("Invalid length of the key! [%d]\n",gdesKey.length);
			return(-1);
	}
	
	UnionToUpperCase(param);
	if (strcasecmp(param,"QUIT") == 0)
		return(0);
		
	if (!UnionIsValidDesKeyCryptogram(param))
	{
		printf("invalid key cryptogram [%s]!\n",param);
		goto loopUnionInput;
	}
	else
		strcpy(gdesKey.value,param);
	
	return(0);
}

int UnionInputDesKeyCheckValue()
{
	char	*param;
loopUnionInput:
	param = UnionInput("UnionInput the checkvalue\n++++++++++++++++\n");
	if (!UnionIsValidDesKeyCheckValue(param))
	{
		printf("Invalid checkvalue!\n");
		goto loopUnionInput;
	}
	
	strcpy(gdesKey.checkValue,param);
	return(0);
}

int UnionInputDesKeyType()
{
	char	*param;

loopUnionInput:
	param = UnionInput("UnionInput Key Type,quit to exit\n");
	UnionToUpperCase(param);
	if (strcasecmp(param,"QUIT") == 0)
		return(0);
	else if (strcasecmp(param,"ZPK") == 0)
		gdesKey.type = conZPK;
	else if (strcasecmp(param,"ZAK") == 0)
		gdesKey.type = conZAK;
	else if (strcasecmp(param,"ZMK") == 0)
		gdesKey.type = conZMK;
	else if (strcasecmp(param,"TMK") == 0)
		gdesKey.type = conTMK;
	else if (strcasecmp(param,"TPK") == 0)
		gdesKey.type = conTPK;
	else if (strcasecmp(param,"TAK") == 0)
		gdesKey.type = conTAK;
	else if (strcasecmp(param,"PVK") == 0)
		gdesKey.type = conPVK;
	else if (strcasecmp(param,"CVK") == 0)
		gdesKey.type = conCVK;
	else if (strcasecmp(param,"ZEK") == 0)
		gdesKey.type = conZEK;
	else if (strcasecmp(param,"WWK") == 0)
		gdesKey.type = conWWK;
	else if (strcasecmp(param,"BDK") == 0)
		gdesKey.type = conBDK;
	else if (strcasecmp(param,"EDK") == 0)
		gdesKey.type = conEDK;
	else if (strcasecmp(param,"USER") == 0)
		gdesKey.type = conSelfDefinedKey;
	else if (strcasecmp(param,"MKAC") == 0)	//Add By Huangbx, 20100518
		gdesKey.type = conMKAC;
	else if (strcasecmp(param,"MKSMC") == 0)	//Add By Huangbx, 20100518
		gdesKey.type = conMKSMC;
	else
	{
		printf("Invalid KeyType [%s]!\n",param);
		goto loopUnionInput;
	}
		
	return(0);
}

int UnionInputDesKeyActiveDate()
{
	char	*param;

	if (UnionConfirm("Use current system date as the default effective date?"))
	{
		UnionGetFullSystemDate(gdesKey.activeDate);
		return(0);
	}
loopUnionInput:
	param = UnionInput("UnionInput effective date(YYYYMMDD)::");
	if (strlen(param) != 8)
	{
		printf("Invalid date!\n");
		goto loopUnionInput;
	}
	strcpy(gdesKey.activeDate,param);
	return(0);
}

int UnionInputDesKeyMaxEffectiveDays()
{
	char	*param;

loopUnionInput:
	if (UnionIsQuit(param = UnionInput("UnionInput maxEffectiveDays::")))
		return(0);
	gdesKey.maxEffectiveDays = atol(param);
	return(0);
}

int UnionInputDesKeyMaxUseTimes()
{
	char	*param;

loopUnionInput:
	if (UnionIsQuit(param = UnionInput("UnionInput maxUseTimes::")))
		return(0);
	gdesKey.maxUseTimes = atol(param);
	return(0);
}

int UnionInputDesKeyWindowBetweenKeyVer()
{
	char	*param;

loopUnionInput:
	if (UnionIsQuit(param = UnionInput("UnionInput windowBetweenKeyVer::")))
		return(0);
	gdesKey.windowBetweenKeyVer = atol(param);
	return(0);
}

int UnionInputDesKeyContainer()
{
	char	*param;
	char	tmpBuf[100];
	
loopUnionInput:
	memset(tmpBuf,0,sizeof(tmpBuf));
	param = UnionInput("UnionInput container::");
	if (strlen(param) >= sizeof(gdesKey.container))
	{
		printf("too long container name!\n");
		goto loopUnionInput;
	}
	strcpy(tmpBuf,param);
	if (UnionIsQuit(param))
		return(0);
	strcpy(gdesKey.container,tmpBuf);
	return(0);
}

int UnionInputDesKeyFullName()
{
	char *param;

loopUnionInput:	
	if (!UnionIsValidDesKeyFullName(param = UnionInput("UnionInput full name of deskey like keyApp.owner.keyName\n")))
	{
		if (UnionIsQuit(param))
			return(0);
		printf("invalid fullName!\n");
		goto loopUnionInput;
	}
	
	strcpy(gdesKey.fullName,param);
	return(0);
}
	
int ChangeMaxKeyNumInDesKeyDB(int argc,char *argv[])
{
	int	ret;
	long	gmaxKeyNum;
	
	if (argc > 0)
		gmaxKeyNum = atol(argv[0]);
	else
	{
loopUnionInput2:
		if ((gmaxKeyNum = atoi(UnionInput("Input max num of keys::"))) <= 0)
		{
			goto loopUnionInput2;
		}
	}
	
	if (UnionConfirm("Change Size Of DesKeyDB of maxKeyNum = [%ld]?",gmaxKeyNum))
	{
		if ((ret = UnionChangeSizeOfDesKeyDB(gmaxKeyNum)) < 0)
			printf("UnionChangeSizeOfDesKeyDB Error! ret = [%d]\n",ret);
		else
			printf("UnionChangeSizeOfDesKeyDB OK!\n");
	}
	return(UnionTaskActionBeforeExit());
}

int UpdateDesKey(int argc,char *argv[])
{
	int		ret;
	
	if (argc > 0)
	{
		if (!UnionIsValidDesKeyFullName(argv[0]))
		{
			printf("Invalid full name [%s]\n",argv[0]);
			return(UnionTaskActionBeforeExit());
		}
		strcpy(gdesKey.fullName,argv[0]);
		if ((ret = UnionReadDesKeyFromKeyDBAnyway(&gdesKey)) < 0)
			printf("UnionReadDesKeyFromKeyDBAnyway [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
		else
		{
			printf("Old key::\n");
			UnionPrintDesKey(&gdesKey);
		}
	}

	if (argc > 1)
	{
		if (!UnionIsValidDesKeyCryptogram(argv[1]))
		{
			printf("Invalid des key cryptogram [%s]!\n",argv[1]);
			return(UnionTaskActionBeforeExit());
		}
		strcpy(gdesKey.value,argv[1]);
		//printf("[%s][%s]\n",gdesKey.value,argv[1]);
	}
	if (argc > 2)
	{
		if (!UnionIsValidDesKeyCheckValue(argv[2]))
		{
			printf("Invalid des key check value [%s]!\n",argv[2]);
			return(UnionTaskActionBeforeExit());
		}
		strcpy(gdesKey.checkValue,argv[2]);
	}
	if (argc > 0)
	{
		printf("\n\nNew Key::\n");
		UnionPrintDesKey(&gdesKey);
		if (!UnionConfirm("Are you sure of updating this key?"))
			return(UnionTaskActionBeforeExit());
		if ((ret = UnionUpdateDesKeyValueInKeyDB(gdesKey.fullName,gdesKey.value,gdesKey.checkValue)) < 0)
			printf("UnionUpdateDesKeyValueInKeyDB [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
		else
			printf("UnionUpdateDesKeyValueInKeyDB [%s] OK!\n",gdesKey.fullName);
	}
	return(UnionTaskActionBeforeExit());
}

int UpdateDesKeyLength(int argc,char *argv[])
{
	int		ret;
	
	if (argc < 2)
	{
		printf("Usage:: %s fullKeyName newKeyLength\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}
	
	if (!UnionIsValidDesKeyFullName(argv[0]))
	{
		printf("Invalid full name [%s]\n",argv[0]);
		return(UnionTaskActionBeforeExit());
	}
	strcpy(gdesKey.fullName,argv[0]);
	if ((ret = UnionReadDesKeyFromKeyDBAnyway(&gdesKey)) < 0)
	{
		printf("UnionReadDesKeyFromKeyDBAnyway [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
		return(UnionTaskActionBeforeExit());
	}
	printf("Old key::\n");
	UnionPrintDesKey(&gdesKey);

	if ((gdesKey.length = UnionConvertDesKeyLength(atoi(argv[1]))) < 0)
	{
		printf("Invalid des key length [%s]\n",argv[1]);
		return(UnionTaskActionBeforeExit());
	}
	printf("\n\nNew Key::\n");
	UnionPrintDesKey(&gdesKey);
	if (!UnionConfirm("Are you sure of updating this key?"))
		return(UnionTaskActionBeforeExit());
	if ((ret = UnionUpdateDesKeyAttrInKeyDB(&gdesKey)) < 0)
		printf("UnionUpdateDesKeyAttrInKeyDB [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
	else
		printf("UnionUpdateDesKeyAttrInKeyDB [%s] OK!\n",gdesKey.fullName);
	return(UnionTaskActionBeforeExit());
}


int UpdateDesKeyActiveDate(int argc,char *argv[])
{
	int		ret;
	
	if (argc < 2)
	{
		printf("Usage:: %s fullKeyName newActiveDate\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}
	
	if (!UnionIsValidDesKeyFullName(argv[0]))
	{
		printf("Invalid full name [%s]\n",argv[0]);
		return(UnionTaskActionBeforeExit());
	}
	strcpy(gdesKey.fullName,argv[0]);
	if ((ret = UnionReadDesKeyFromKeyDBAnyway(&gdesKey)) < 0)
	{
		printf("UnionReadDesKeyFromKeyDBAnyway [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
		return(UnionTaskActionBeforeExit());
	}
	printf("Old key::\n");
	UnionPrintDesKey(&gdesKey);

	if (strlen(argv[1]) != 8)
	{
		printf("Invalid activeDate [%s]\n",argv[1]);
		return(UnionTaskActionBeforeExit());
	}
	strcpy(gdesKey.activeDate,argv[1]);
	printf("\n\nNew Key::\n");
	UnionPrintDesKey(&gdesKey);
	if (!UnionConfirm("Are you sure of updating this key?"))
		return(UnionTaskActionBeforeExit());
	if ((ret = UnionUpdateDesKeyAttrInKeyDB(&gdesKey)) < 0)
		printf("UnionUpdateDesKeyAttrInKeyDB [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
	else
		printf("UnionUpdateDesKeyAttrInKeyDB [%s] OK!\n",gdesKey.fullName);
	return(UnionTaskActionBeforeExit());
}

int UpdateDesKeyMaxEffectiveDays(int argc,char *argv[])
{
	int		ret;
	
	if (argc < 2)
	{
		printf("Usage:: %s fullKeyName newMaxEffectiveDays\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}
	
	if (!UnionIsValidDesKeyFullName(argv[0]))
	{
		printf("Invalid full name [%s]\n",argv[0]);
		return(UnionTaskActionBeforeExit());
	}
	strcpy(gdesKey.fullName,argv[0]);
	if ((ret = UnionReadDesKeyFromKeyDBAnyway(&gdesKey)) < 0)
	{
		printf("UnionReadDesKeyFromKeyDBAnyway [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
		return(UnionTaskActionBeforeExit());
	}
	printf("Old key::\n");
	UnionPrintDesKey(&gdesKey);

	gdesKey.maxEffectiveDays = atol(argv[1]);
	printf("\n\nNew Key::\n");
	UnionPrintDesKey(&gdesKey);
	if (!UnionConfirm("Are you sure of updating this key?"))
		return(UnionTaskActionBeforeExit());
	if ((ret = UnionUpdateDesKeyAttrInKeyDB(&gdesKey)) < 0)
		printf("UnionUpdateDesKeyAttrInKeyDB [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
	else
		printf("UnionUpdateDesKeyAttrInKeyDB [%s] OK!\n",gdesKey.fullName);
	return(UnionTaskActionBeforeExit());
}

int UpdateDesKeyMaxUseTimes(int argc,char *argv[])
{
	int		ret;
	
	if (argc < 2)
	{
		printf("Usage:: %s fullKeyName newMaxUseTimes\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}
	
	if (!UnionIsValidDesKeyFullName(argv[0]))
	{
		printf("Invalid full name [%s]\n",argv[0]);
		return(UnionTaskActionBeforeExit());
	}
	strcpy(gdesKey.fullName,argv[0]);
	if ((ret = UnionReadDesKeyFromKeyDBAnyway(&gdesKey)) < 0)
	{
		printf("UnionReadDesKeyFromKeyDBAnyway [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
		return(UnionTaskActionBeforeExit());
	}
	printf("Old key::\n");
	UnionPrintDesKey(&gdesKey);

	gdesKey.maxUseTimes = atol(argv[1]);
	printf("\n\nNew Key::\n");
	UnionPrintDesKey(&gdesKey);
	if (!UnionConfirm("Are you sure of updating this key?"))
		return(UnionTaskActionBeforeExit());
	if ((ret = UnionUpdateDesKeyAttrInKeyDB(&gdesKey)) < 0)
		printf("UnionUpdateDesKeyAttrInKeyDB [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
	else
		printf("UnionUpdateDesKeyAttrInKeyDB [%s] OK!\n",gdesKey.fullName);
	return(UnionTaskActionBeforeExit());
}

int UpdateDesKeyWindowBetweenKeyVer(int argc,char *argv[])
{
	int		ret;
	
	if (argc < 2)
	{
		printf("Usage:: %s fullKeyName newWindowBetweenKeyVer\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}
	
	if (!UnionIsValidDesKeyFullName(argv[0]))
	{
		printf("Invalid full name [%s]\n",argv[0]);
		return(UnionTaskActionBeforeExit());
	}
	strcpy(gdesKey.fullName,argv[0]);
	if ((ret = UnionReadDesKeyFromKeyDBAnyway(&gdesKey)) < 0)
	{
		printf("UnionReadDesKeyFromKeyDBAnyway [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
		return(UnionTaskActionBeforeExit());
	}
	printf("Old key::\n");
	UnionPrintDesKey(&gdesKey);

	gdesKey.windowBetweenKeyVer = atol(argv[1]);
	printf("\n\nNew Key::\n");
	UnionPrintDesKey(&gdesKey);
	if (!UnionConfirm("Are you sure of updating this key?"))
		return(UnionTaskActionBeforeExit());
	if ((ret = UnionUpdateDesKeyAttrInKeyDB(&gdesKey)) < 0)
		printf("UnionUpdateDesKeyAttrInKeyDB [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
	else
		printf("UnionUpdateDesKeyAttrInKeyDB [%s] OK!\n",gdesKey.fullName);
	return(UnionTaskActionBeforeExit());
}

int UpdateDesKeyContainer(int argc,char *argv[])
{
	int		ret;
	
	if (argc < 2)
	{
		printf("Usage:: %s fullKeyName newContainer\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}
	
	if (!UnionIsValidDesKeyFullName(argv[0]))
	{
		printf("Invalid full name [%s]\n",argv[0]);
		return(UnionTaskActionBeforeExit());
	}
	strcpy(gdesKey.fullName,argv[0]);
	if ((ret = UnionReadDesKeyFromKeyDBAnyway(&gdesKey)) < 0)
	{
		printf("UnionReadDesKeyFromKeyDBAnyway [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
		return(UnionTaskActionBeforeExit());
	}
	printf("Old key::\n");
	UnionPrintDesKey(&gdesKey);

	if (strlen(argv[1]) >= sizeof(gdesKey.container))
	{
		printf("container name too long!\n");
		return(UnionTaskActionBeforeExit());
	}
	strcpy(gdesKey.container,argv[1]);
	printf("\n\nNew Key::\n");
	UnionPrintDesKey(&gdesKey);
	if (!UnionConfirm("Are you sure of updating this key?"))
		return(UnionTaskActionBeforeExit());
	if ((ret = UnionUpdateDesKeyAttrInKeyDB(&gdesKey)) < 0)
		printf("UnionUpdateDesKeyAttrInKeyDB [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
	else
		printf("UnionUpdateDesKeyAttrInKeyDB [%s] OK!\n",gdesKey.fullName);
	return(UnionTaskActionBeforeExit());
}

int UpdateDesKeyUseOldVerKey(int argc,char *argv[])
{
	int		ret;
	
	if (argc < 1)
	{
		printf("Usage:: %s fullKeyName\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}
	
	if (!UnionIsValidDesKeyFullName(argv[0]))
	{
		printf("Invalid full name [%s]\n",argv[0]);
		return(UnionTaskActionBeforeExit());
	}
	strcpy(gdesKey.fullName,argv[0]);
	if ((ret = UnionReadDesKeyFromKeyDBAnyway(&gdesKey)) < 0)
	{
		printf("UnionReadDesKeyFromKeyDBAnyway [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
		return(UnionTaskActionBeforeExit());
	}
	printf("Old key::\n");
	UnionPrintDesKey(&gdesKey);

	gdesKey.oldVerEffective = 1;
	printf("\n\nNew Key::\n");
	UnionPrintDesKey(&gdesKey);
	if (!UnionConfirm("Are you sure of updating this key?"))
		return(UnionTaskActionBeforeExit());
	if ((ret = UnionUpdateDesKeyAttrInKeyDB(&gdesKey)) < 0)
		printf("UnionUpdateDesKeyAttrInKeyDB [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
	else
		printf("UnionUpdateDesKeyAttrInKeyDB [%s] OK!\n",gdesKey.fullName);
	return(UnionTaskActionBeforeExit());
}

int UpdateDesKeyUnuseOldVerKey(int argc,char *argv[])
{
	int		ret;
	
	if (argc < 1)
	{
		printf("Usage:: %s fullKeyName\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}
	
	if (!UnionIsValidDesKeyFullName(argv[0]))
	{
		printf("Invalid full name [%s]\n",argv[0]);
		return(UnionTaskActionBeforeExit());
	}
	strcpy(gdesKey.fullName,argv[0]);
	if ((ret = UnionReadDesKeyFromKeyDBAnyway(&gdesKey)) < 0)
	{
		printf("UnionReadDesKeyFromKeyDBAnyway [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
		return(UnionTaskActionBeforeExit());
	}
	printf("Old key::\n");
	UnionPrintDesKey(&gdesKey);

	gdesKey.oldVerEffective = 0;
	printf("\n\nNew Key::\n");
	UnionPrintDesKey(&gdesKey);
	if (!UnionConfirm("Are you sure of updating this key?"))
		return(UnionTaskActionBeforeExit());
	if ((ret = UnionUpdateDesKeyAttrInKeyDB(&gdesKey)) < 0)
		printf("UnionUpdateDesKeyAttrInKeyDB [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
	else
		printf("UnionUpdateDesKeyAttrInKeyDB [%s] OK!\n",gdesKey.fullName);
	return(UnionTaskActionBeforeExit());
}


int InsertTestKeys()
{
	int		ret;
	long		num,realNum;
	long		i;
	char		keyName[40+1];
	char		*ptr;
	TUnionDesKey	desKey;
	time_t		start,finish;
	
	if (UnionIsQuit(ptr=UnionInput("请输入测试密钥的数目::")))
		return(UnionTaskActionBeforeExit());
	num = atol(ptr);
	if (!UnionConfirm("确认插入%ld个测试密钥吗",num))
		return(UnionTaskActionBeforeExit());
	
	time(&start);
	for (realNum = 0,i = 0; realNum < num;i++)
	{
		sprintf(keyName,"TE.%08ld.zak",i);
		// 依次是密钥指针，密钥名，密钥值（LMK加密），密钥类型，密钥校验值
		UnionFormDefaultDesKey(&desKey,keyName,"",conZAK,"");
		if ((ret = UnionInsertDesKeyIntoKeyDB(&desKey)) < 0)
			continue;
		printf("Insert [%s] OK!\n",desKey.fullName);
		realNum++;
		if (realNum % 100 == 0)
		{
			time(&finish);
			printf("timeUsed = [%06ld] for insert [%08ld] keys\n",finish-start,realNum);
		}
	}
	printf("Insert [%ld] keys!\n",num);
	return(UnionTaskActionBeforeExit());
}

int main(int argc,char **argv)
{
	int	ret;
	int	i;
	
	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		return(UnionHelp());
		
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
	
	// 2011-03-23 Add By Huangbx
	signal(SIGSTOP,UnionSignalExit);
	signal(SIGTSTP,UnionSignalExit);
	
	/***
	// 2010-6-5 王纯军增加
	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		printf("in main:: UnionConnectDesKeyDB! ret = [%d]\n",ret);
		return(ret);
	}
	***/

	memset(&gdesKey,0,sizeof(gdesKey));
	UnionFormDefaultDesKey(&gdesKey,"00.null.00","",conZPK,"");
	
	// 2008/7/18 增加
	if (strcasecmp(argv[1],"OUTPUT") == 0)
	{
		if (argc > 2)
			printf("导出密钥数量=[%d],导出文件名称[%s]\n",UnionOutputAllDesKeyToRecStrFile(argv[2]),argv[2]);
		return(UnionTaskActionBeforeExit());
	}
	// 2008/7/18 增加
	if (strcasecmp(argv[1],"INPUT") == 0)
	{
		if (argc > 2)
			printf("从文件[%s]导入密钥数量=[%d]\n",argv[2],UnionInputAllDesKeyFromRecStrFile(argv[2]));
		return(UnionTaskActionBeforeExit());
	}
	// 2007/12/11增加
	if (strcasecmp(argv[1],"WRITINGLOCKDB") == 0)
	{
		if ((ret = UnionWritingLockDesKeyTBL()) < 0)
			printf("UnionWritingLockDesKeyTBL Failure! ret = [%d]\n",ret);
		else
			printf("UnionWritingLockDesKeyTBL OK!\n");
		return(UnionTaskActionBeforeExit());
	}
	if (strcasecmp(argv[1],"READINGLOCKDB") == 0)
	{
		if ((ret = UnionReadingLockDesKeyTBL()) < 0)
			printf("UnionReadingLockDesKeyTBL Failure! ret = [%d]\n",ret);
		else
			printf("UnionReadingLockDesKeyTBL OK!\n");
		return(UnionTaskActionBeforeExit());
	}
	// 2007/12/11增加结束
	if (strcasecmp(argv[1],"UNLOCKDB") == 0)
	{
		if ((ret = UnionUnlockDesKeyDB()) < 0)
			printf("UnionUnlockDesKeyDB Failure! ret = [%d]\n",ret);
		else
			printf("UnionUnlockDesKeyDB OK!\n");
		return(UnionTaskActionBeforeExit());
	}
	if (strcasecmp(argv[1],"UNLOCKALLKEY") == 0)
	{
		if ((ret = UnionUnlockAllLockedDesKeyInDesKeyDB()) < 0)
			printf("UnionUnlockAllLockedDesKeyInDesKeyDB Failure! ret = [%d]\n",ret);
		else
			printf("UnionUnlockAllLockedDesKeyInDesKeyDB OK!\n");
		return(UnionTaskActionBeforeExit());
	}
	if (strcasecmp(argv[1],"ALLLOCKEDKEY") == 0)
	{
		if ((ret = UnionPrintAllLockedDesKeyInDesKeyDBToFile(stdout)) < 0)
			printf("UnionPrintAllLockedDesKeyInDesKeyDBToFile Failure! ret = [%d]\n",ret);
		return(UnionTaskActionBeforeExit());
	}
	if ((strcasecmp(argv[1],"CREATE") == 0) || (strcasecmp(argv[1],"-CREATE") == 0))
		return(CreateDesKeyDB(argc-2,&argv[2]));
	if ((strcasecmp(argv[1],"DROP") == 0) || (strcasecmp(argv[1],"-DROP") == 0))
		return(DropDesKeyDB());
	if ((strcasecmp(argv[1],"MAXKEYNUM") == 0) || (strcasecmp(argv[1],"-MAXKEYNUM") == 0))
		return(ChangeMaxKeyNumInDesKeyDB(argc-2,&argv[2]));
	if ((strcasecmp(argv[1],"CONNECT") == 0) || (strcasecmp(argv[1],"-CONNECT") == 0))
	{
		if ((ret = UnionConnectDesKeyDB()) < 0)
		{
			printf("in main:: UnionConnectDesKeyDB Error! ret = [%d]\n",ret);
			return(ret);
		}
		else
			printf("UnionConnectDesKeyDB OK!\n");
		return(UnionTaskActionBeforeExit());
	}
	/*	
	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		printf("in main:: UnionConnectDesKeyDB Error! ret = [%d]\n",ret);
		UnionTaskActionBeforeExit();
		return(ret);
	}
	*/
	if ((strcasecmp(argv[1],"MIRROR") == 0) || (strcasecmp(argv[1],"-MIRROR") == 0))
	{
		if ((ret = UnionMirrorDesKeyDBIntoDiskArtifically()) < 0)
			printf("UnionMirrorDesKeyDBIntoDiskArtifically Error! ret = [%d]\n",ret);
		else
			printf("UnionMirrorDesKeyDBIntoDiskArtifically OK!\n");
		UnionTaskActionBeforeExit();
		return(ret);
	}
	
	if ((strcasecmp(argv[1],"RELOAD") == 0) || (strcasecmp(argv[1],"-RELOAD") == 0))
	{
		if (UnionConfirm("Are you sure of reload desKeyDB?"))
		{
			if ((ret = UnionLoadDesKeyDBIntoMemory()) < 0)
				printf("UnionLoadDesKeyDBIntoMemory Error! ret = [%d]\n",ret);
			else
				printf("UnionLoadDesKeyDBIntoMemory OK!\n");
		}
		UnionTaskActionBeforeExit();
		return(ret);
	}
	if ((strcasecmp(argv[1],"RELOADANYWAY") == 0) || (strcasecmp(argv[1],"-RELOADANYWAY") == 0))
	{
		if ((ret = UnionLoadDesKeyDBIntoMemory()) < 0)
			printf("UnionLoadDesKeyDBIntoMemory Error! ret = [%d]\n",ret);
		else
			printf("UnionLoadDesKeyDBIntoMemory OK!\n");
		UnionTaskActionBeforeExit();
		return(ret);
	}
	if ((strcasecmp(argv[1],"INSERT") == 0) || (strcasecmp(argv[1],"-INSERT") == 0))
		return(InsertDesKey(argc-2,&argv[2],1));
	if ((strcasecmp(argv[1],"INSERTANYWAY") == 0) || (strcasecmp(argv[1],"-INSERTANYWAY") == 0))
		return(InsertDesKey(argc-2,&argv[2],0));
	if ((strcasecmp(argv[1],"UPDATE") == 0) || (strcasecmp(argv[1],"-UPDATE") == 0))
		return(UpdateDesKey(argc-2,&argv[2]));
	if ((strcasecmp(argv[1],"DELETE") == 0) || (strcasecmp(argv[1],"-DELETE") == 0))
		return(DeleteDesKey(argc-2,&argv[2],1));
	if ((strcasecmp(argv[1],"DELETEALLOFAPP") == 0) || (strcasecmp(argv[1],"-DELETEALLOFAPP") == 0))
	{
		for (i = 2; i < argc; i++)
		{
			if (!UnionConfirm("Are you sure of deleting all keys of app [%s]",argv[i]))
				continue;
			if ((ret = UnionDeleteAllDesKeyOfApp(argv[i])) < 0)
				printf("UnionDeleteAllDesKeyOfApp [%s] failure! ret = [%d]\n",argv[i],ret);
			else
				printf("UnionDeleteAllDesKeyOfApp [%s] OK! [%04d] keys deleted!\n",argv[i],ret);
		}
		return(UnionTaskActionBeforeExit());
	}	
	if ((strcasecmp(argv[1],"DELETEANYWAY") == 0) || (strcasecmp(argv[1],"-DELETEANYWAY") == 0))
		return(DeleteDesKey(argc-2,&argv[2],0));
	if ((strcasecmp(argv[1],"PRINT") == 0) || (strcasecmp(argv[1],"-PRINT") == 0))
		return(PrintDesKey(argc-2,&argv[2]));
	if ((strcasecmp(argv[1],"READ") == 0) || (strcasecmp(argv[1],"-READ") == 0))
		return(ReadDesKey(argc-2,&argv[2]));
	if ((strcasecmp(argv[1],"USE") == 0) || (strcasecmp(argv[1],"-USE") == 0))
		return(UseDesKey(argc-2,&argv[2]));
	if ((strcasecmp(argv[1],"ALL") == 0) || (strcasecmp(argv[1],"-ALL") == 0))
	{
		if ((ret = UnionPrintAllDesKeysInKeyDBToFile(stdout)) < 0)
			printf("UnionPrintAllDesKeysInKeyDBToFile Error! ret = [%d]\n",ret);
		return(UnionTaskActionBeforeExit());
	}
	if ((strcasecmp(argv[1],"ALLVALUE") == 0) || (strcasecmp(argv[1],"-ALLVALUE") == 0))
	{
		if ((ret = UnionPrintAllDesKeyValueToFile(stdout)) < 0)
			printf("UnionPrintAllDesKeyValueToFile Error! ret = [%d]\n",ret);
		return(UnionTaskActionBeforeExit());
	}
	if ((strcasecmp(argv[1],"PCONTAINER") == 0) || (strcasecmp(argv[1],"-PCONTAINER") == 0))
		return(PrintAllDesKeyForContainer(argc-2,&argv[2]));
	if ((strcasecmp(argv[1],"APP") == 0) || (strcasecmp(argv[1],"-APP") == 0))
		return(PrintAllDesKeyForApp(argc-2,&argv[2]));
	if ((strcasecmp(argv[1],"OWNER") == 0) || (strcasecmp(argv[1],"-OWNER") == 0))
		return(PrintAllDesKeyForOwner(argc-2,&argv[2]));
	if ((strcasecmp(argv[1],"NAME") == 0) || (strcasecmp(argv[1],"-NAME") == 0))
		return(PrintAllDesKeyForName(argc-2,&argv[2]));
	if ((strcasecmp(argv[1],"LIKE") == 0) || (strcasecmp(argv[1],"-LIKE") == 0))
		return(PrintAllDesKeyExpected(argc-2,&argv[2]));
	if ((strcasecmp(argv[1],"LENGTH") == 0) || (strcasecmp(argv[1],"-LENGTH") == 0))
		return(UpdateDesKeyLength(argc-2,&argv[2]));
	if ((strcasecmp(argv[1],"ACTIVEDATE") == 0) || (strcasecmp(argv[1],"-ACTIVEDATE") == 0))
		return(UpdateDesKeyActiveDate(argc-2,&argv[2]));
	if ((strcasecmp(argv[1],"MAXEFFECTIVEDAYS") == 0) || (strcasecmp(argv[1],"-MAXEFFECTIVEDAYS") == 0))
		return(UpdateDesKeyMaxEffectiveDays(argc-2,&argv[2]));
	if ((strcasecmp(argv[1],"MAXUSETIMES") == 0) || (strcasecmp(argv[1],"-MAXUSETIMES") == 0))
		return(UpdateDesKeyMaxUseTimes(argc-2,&argv[2]));
	if ((strcasecmp(argv[1],"WINDOW") == 0) || (strcasecmp(argv[1],"-WINDOW") == 0))
		return(UpdateDesKeyWindowBetweenKeyVer(argc-2,&argv[2]));
	if ((strcasecmp(argv[1],"CONTAINER") == 0) || (strcasecmp(argv[1],"-CONTAINER") == 0))
		return(UpdateDesKeyContainer(argc-2,&argv[2]));
	if ((strcasecmp(argv[1],"USEOLDVER") == 0) || (strcasecmp(argv[1],"-USEOLDVER") == 0))
		return(UpdateDesKeyUseOldVerKey(argc-2,&argv[2]));
	if ((strcasecmp(argv[1],"UNUSEOLDVER") == 0) || (strcasecmp(argv[1],"-UNUSEOLDVER") == 0))
		return(UpdateDesKeyUnuseOldVerKey(argc-2,&argv[2]));
		
	if ((strcasecmp(argv[1],"STATUS") == 0) || (strcasecmp(argv[1],"-STATUS") == 0))
	{
		if ((ret = UnionPrintStatusOfDesKeyDBToFile(stdout)) < 0)
			printf("UnionPrintStatusOfDesKeyDBToFile Error! ret = [%d]\n",ret);
		return(UnionTaskActionBeforeExit());
	}
	if (strcasecmp(argv[1],"LOOP") == 0)
		return(LoopTest(argc-2,&argv[2]));
	if ((strcasecmp(argv[1],"EDIT") == 0) || (strcasecmp(argv[1],"-EDIT") == 0))
		return(EditDesKey());
	if (strcasecmp(argv[1],"INSERTTEST") == 0)
		return(InsertTestKeys());
	if ((strcasecmp(argv[1],"OUTDATE") == 0) || (strcasecmp(argv[1],"-OUTDATE") == 0))
	{
		if (argc == 2)
		{
			if ((ret = UnionPrintDesKeyOutofDateToFile(stdout)) < 0)
				printf("UnionPrintDesKeyOutofDateToFile Error! ret = [%d]\n",ret);
		}
		else
		{
			if ((ret = UnionPrintDesKeyNearOutofDateToFile(atol(argv[2]),stdout)) < 0)
				printf("UnionPrintDesKeyNearOutofDateToFile Error! ret = [%d]\n",ret);
		}
		return(UnionTaskActionBeforeExit());
	}
	if ((strcasecmp(argv[1],"RECSIZE") == 0) || (strcasecmp(argv[1],"-RECSIZE") == 0))
	{
		printf("size of a deskey = [%04d]\n",sizeof(gdesKey));
		return(UnionTaskActionBeforeExit());
	}		
	if ((strcasecmp(argv[1],"OUTTIMES") == 0) || (strcasecmp(argv[1],"-OUTTIMES") == 0))
	{
		if (argc == 2)
		{
			if ((ret = UnionPrintDesKeyOutofDateToFile(stdout)) < 0)
				printf("UnionPrintDesKeyOutofDateToFile Error! ret = [%d]\n",ret);
		}
		else
		{
			if ((ret = UnionPrintDesKeyNearOutofMaxUseTimesToFile(atol(argv[2]),stdout)) < 0)
				printf("UnionPrintDesKeyNearOutofMaxUseTimesToFile Error! ret = [%d]\n",ret);
		}
		return(UnionTaskActionBeforeExit());
	}
			
	UnionHelp();
	return(UnionTaskActionBeforeExit());
}

