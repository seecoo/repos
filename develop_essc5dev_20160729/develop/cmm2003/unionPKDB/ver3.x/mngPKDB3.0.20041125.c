//	Wolfgang Wang
//	2004/11/25

// 2004/11/25在 mngPKDB.20041111.c 和mngPKDB3.0.20041125.c基础上生成


#ifndef _UnionPK_3_x_
#define _UnionPK_3_x_
#endif

#define _UnionTask_3_x_
#define _UnionLogMDL_3_x_

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <time.h>

#include "UnionLog.h"
#include "unionCommand.h"
#include "unionVersion.h"
#include "UnionTask.h"
#include "unionModule.h"

#include "unionPKDB.h"

TUnionPK		gpk;

PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	UnionDisconnectPKDB();
	//return(exit(0));
	exit(0);
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
	printf("	insert	fullKeyName keyType keyLength keyValue keyCheckValue\n");
	printf("	update	fullKeyName keyValue keyCheckValue\n");
	printf("	delete	fullKeyName Delete a des key\n");
	printf("	print	fullKeyName Print a des key\n");
	printf("	read	fullKeyName read a des key\n");
	printf("	use	fullKeyName use a des key\n");
	printf("	all 	Print all des keys\n");
	printf("	app 	print all des keys of application\n");
	printf("	owner	print all des keys of owner\n");
	printf("	name	print all des keys of name\n");
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
	printf("	outdate [days] Print all keys which are outofdate or will be outofdate after days.\n");
	printf("	outtimes [times] Print all keys which exceeded maxUseTimes or will exceed after times.\n");
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
	printf("	app 	print all des keys of application\n");
	printf("	owner	print all des keys of owner\n");
	printf("	name	print all des keys of name\n");
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
	return(0);
}



//int CreateKeyDB()
int CreatePKDB(int argc,char *argv[])
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
	
	if (UnionConfirm("Create PKDB of maxKeyNum = [%ld]?",gmaxKeyNum))
	{
		if ((ret = UnionCreatePKDB(gmaxKeyNum)) < 0)
			printf("UnionCreatePKDB Error! ret = [%d]\n",ret);
		else
			printf("UnionCreatePKDB OK!\n");
	}
	return(UnionTaskActionBeforeExit());
}

//int DropKeyDB()
int DropPKDB()
{
	int	ret;
	
	if (UnionConfirm("Drop PKDB?"))
	{
		if ((ret = UnionDeletePKDB()) < 0)
			printf("UnionDeletePKDB Error! ret = [%d]\n",ret);
		else
			printf("UnionDeletePKDB OK!\n");
	}
	return(UnionTaskActionBeforeExit());
}

int InsertPK(int argc,char *argv[],short confirmRequired)
{
	int		ret;
	
	if (argc > 0)
	{
		if (!UnionIsValidPKFullName(argv[0]))
		{
			printf("Invalid full name [%s]\n",argv[0]);
			return(UnionTaskActionBeforeExit());
		}
		strcpy(gpk.fullName,argv[0]);
	}
	
	if (argc > 1)
	{
		if (!UnionIsValidPKType(UnionConvertPKType(argv[1])))
		{
			printf("Invalid des key type [%s]!\n",argv[1]);
			return(UnionTaskActionBeforeExit());
		}
		gpk.type = UnionConvertPKType(argv[1]);
	}
	if (argc > 2)
	{
		if ((gpk.length = UnionConvertPKLength(atoi(argv[2]))) < 0)
		{
			printf("Invalid des key length [%s]!\n",argv[2]);
			return(UnionTaskActionBeforeExit());
		}
	}
	if (argc > 3)
	{
		if (!UnionIsValidPKValue(argv[3]))
		{
			printf("Invalid des key cryptogram [%s]!\n",argv[3]);
			return(UnionTaskActionBeforeExit());
		}
		strcpy(gpk.value,argv[3]);
	}
	if (argc > 4)
	{
		if (!UnionIsValidPKCheckValue(argv[4]))
		{
			printf("Invalid des key check value [%s]!\n",argv[4]);
			return(UnionTaskActionBeforeExit());
		}
		strcpy(gpk.checkValue,argv[4]);
	}
	if (argc > 5)
	{
		if (strlen(argv[5]) != 8)
		{
			printf("Invalid des key active date [%s]!\n",argv[5]);
			return(UnionTaskActionBeforeExit());
		}
		strcpy(gpk.activeDate,argv[5]);
	}
	if (argc > 6)
	{
		gpk.maxEffectiveDays = atol(argv[6]);
	}
	
	if (argc > 0)
	{
		if (confirmRequired)
		{
			UnionPrintPK(&gpk);
			if (!UnionConfirm("Are you sure of inserting this des key?"))
				return(UnionTaskActionBeforeExit());
		}
		if ((ret = UnionInsertPKIntoKeyDB(&gpk)) < 0)
			printf("UnionInsertPKIntoKeyDB %s error! ret = [%d]\n",gpk.fullName,ret);
		else
			printf("UnionInsertPKIntoKeyDB %s OK!\n",gpk.fullName);
	}
	return(UnionTaskActionBeforeExit());
}

int DeletePK(int argc,char *argv[],short confirmRequired)
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
		if ((ret = UnionDeletePKFromKeyDB(argv[i])) < 0)
			printf("UnionDeletePKFromKeyDB [%s] Error! ret = [%d]\n",argv[i],ret);
		else
			printf("UnionDeletePKFromKeyDB [%s] OK!\n",argv[i]);
	}
	return(UnionTaskActionBeforeExit());
}	

int PrintPK(int argc,char *argv[])
{
	int	ret;
	int	i;
	
	for (i = 0; i < argc; i++)
	{
		if (!UnionIsValidPKFullName(argv[i]))
		{
			printf("Invalid fullName [%s]!\n");
			continue;
		}
		memset(&gpk,0,sizeof(gpk));
		strcpy(gpk.fullName,argv[i]);
		if ((ret = UnionReadPKFromKeyDBAnyway(&gpk)) < 0)
			printf("UnionReadPKFromKeyDBAnyway [%s] Error! ret = [%d]\n",gpk.fullName,ret);
		else
			UnionPrintPK(&gpk);
	}
	
	return(UnionTaskActionBeforeExit());
}

int ReadPK(int argc,char *argv[])
{
	int	ret;
	int	i;
	
	for (i = 0; i < argc; i++)
	{
		if (!UnionIsValidPKFullName(argv[i]))
		{
			printf("Invalid fullName [%s]!\n");
			continue;
		}
		memset(&gpk,0,sizeof(gpk));
		strcpy(gpk.fullName,argv[i]);
		if ((ret = UnionReadPKFromKeyDBAnyway(&gpk)) < 0)
			printf("UnionReadPKFromKeyDBAnyway [%s] Error! ret = [%d]\n",gpk.fullName,ret);
		else
			UnionPrintPK(&gpk);
	}
	
	return(UnionTaskActionBeforeExit());
}

int UsePK(int argc,char *argv[])
{
	int	ret;
	int	i;
	
	for (i = 0; i < argc; i++)
	{
		if (!UnionIsValidPKFullName(argv[i]))
		{
			printf("Invalid fullName [%s]!\n");
			continue;
		}
		memset(&gpk,0,sizeof(gpk));
		strcpy(gpk.fullName,argv[i]);
		if ((ret = UnionUsePKFromKeyDB(&gpk)) < 0)
			printf("UnionUsePKFromKeyDB [%s] Error! ret = [%d]\n",gpk.fullName,ret);
		else
			UnionPrintPK(&gpk);
	}
	
	return(UnionTaskActionBeforeExit());
}

int PrintAllPKForApp(int argc,char *argv[])
{
	int	ret;
	int	i;
	
	if (argc <= 0)
	{
		UnionPrintPKExpectedToFile(UnionInput("Input AppID::"),"","",stdout);
	}
	for (i = 0; i < argc; i++)
		UnionPrintPKExpectedToFile(argv[i],"","",stdout);
			
	return(UnionTaskActionBeforeExit());
}

int PrintAllPKForOwner(int argc,char *argv[])
{
	int	ret;
	int	i;
	
	if (argc <= 0)
	{
		UnionPrintPKExpectedToFile("",UnionInput("Input Owner::"),"",stdout);
	}
	for (i = 0; i < argc; i++)
		UnionPrintPKExpectedToFile("",argv[i],"",stdout);
			
	return(UnionTaskActionBeforeExit());
}

int PrintAllPKForName(int argc,char *argv[])
{
	int	ret;
	int	i;
	
	if (argc <= 0)
	{
		UnionPrintPKExpectedToFile("","",UnionInput("Input Name::"),stdout);
	}

	for (i = 0; i < argc; i++)
		UnionPrintPKExpectedToFile("","",argv[i],stdout);
			
	return(UnionTaskActionBeforeExit());
}

int PrintAllPKExpected(int argc,char *argv[])
{
	int	ret;
	char	tmpBuf1[128];
	
	if (argc <= 0)
	{
		memset(tmpBuf1,0,sizeof(tmpBuf1));
		strcpy(tmpBuf1,UnionInput("Input AppID::"));
		UnionPrintPKExpectedToFile(tmpBuf1,UnionInput("Input Owner::"),"",stdout);
	}
	else if (argc == 1)
		UnionPrintPKExpectedToFile(argv[0],"","",stdout);
	else if (argc == 2)
		UnionPrintPKExpectedToFile(argv[0],argv[1],"",stdout);
	else if (argc >= 3)
		UnionPrintPKExpectedToFile(argv[0],argv[1],argv[2],stdout);
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
		if (!UnionIsValidPKFullName(argv[1]))
		{
			printf("Invalid full name [%s]\n",argv[1]);
			return(UnionTaskActionBeforeExit());
		}
		strcpy(gpk.fullName,argv[1]);
	}
	else
	{
		if (!UnionIsValidPKFullName(p = UnionInput("\nPKFullName::")))
		{
			printf("Invalid full name [%s]\n",p);
			return(UnionTaskActionBeforeExit());
		}
		strcpy(gpk.fullName,p);
	}
		
	if (!UnionConfirm("Are you sure to loop test [%ld] times?\n",loopTimes))
		return(UnionTaskActionBeforeExit());
		
	time(&start);
	for (i = 0; i < loopTimes; i++)
	{
		if ((ret = UnionReadPKFromKeyDBAnyway(&gpk)) < 0)
		{
			printf("UnionReadPKFromKeyDBAnyway [%s] Error! ret = [%d]\n",gpk.fullName,ret);
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

int EditPK()
{
	char	*p;
	int	ret;	
	UnionFormDefaultPK(&gpk,"00.00.00","",con512RSAPair,"");
loopEdit:
	p = UnionInput("\n\nCommand>");
	if (UnionIsValidPKFullName(p))
	{
		strcpy(gpk.fullName,p);
		if ((ret = UnionReadPKFromKeyDBAnyway(&gpk)) < 0)
		{
			printf("%s not exists!\n",gpk.fullName);
			memset(gpk.value,0,sizeof(gpk.value));
			memset(gpk.checkValue,0,sizeof(gpk.checkValue));
			memset(gpk.oldValue,0,sizeof(gpk.oldValue));
			memset(gpk.oldCheckValue,0,sizeof(gpk.oldCheckValue));
		}			
		else
			UnionPrintPK(&gpk);			
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
	if (strcasecmp(p,"STATUS") == 0)
		EditPrintStatus();
	if (strcasecmp(p,"LENGTH") == 0)
		UnionInputPKLength();
	if (strcasecmp(p,"TYPE") == 0)
		UnionInputPKType();
	if (strcasecmp(p,"VALUE") == 0)
		UnionInputPKValue();
	if (strcasecmp(p,"CHECKVALUE") == 0)
		UnionInputPKCheckValue();
	if (strcasecmp(p,"ACTIVEDATE") == 0)
		UnionInputPKActiveDate();
	if (strcasecmp(p,"MAXEFFECTIVEDAYS") == 0)
		UnionInputPKMaxEffectiveDays();
	if (strcasecmp(p,"MAXUSETIMES") == 0)
		UnionInputPKMaxUseTimes();
	if (strcasecmp(p,"WINDOW") == 0)
		UnionInputPKWindowBetweenKeyVer();
	if (strcasecmp(p,"CONTAINER") == 0)
		UnionInputPKContainer();
	if (strcasecmp(p,"USEOLDKEY") == 0)
	{
		gpk.oldVerEffective = 1;
		printf("set old version key effective!\n");
	}
	if (strcasecmp(p,"UNUSEOLDKEY") == 0)
	{
		gpk.oldVerEffective = 0;
		printf("set old version key ineffective!\n");
	}
	if (strcasecmp(p,"FULLNAME") == 0)
		UnionInputPKFullName();
	if (strcasecmp(p,"APP") == 0)
		EditPrintPKOfApp();
	if (strcasecmp(p,"OWNER") == 0)
		EditPrintPKOfOwner();
	if (strcasecmp(p,"NAME") == 0)
		EditPrintPKOfName();
	if (strcasecmp(p,"LIKE") == 0)
		EditPrintPKOfExpected();
	if (strcasecmp(p,"HELP") == 0)
		EditHelp();
	if (strcasecmp(p,"OUTDATE") == 0)
		EditOutofDate();
	if (strcasecmp(p,"OUTTIMES") == 0)
		EditOutofTimes();
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
		if ((ret = UnionPrintPKOutofDateToFile(stdout)) < 0)
			printf("UnionPrintPKOutofDateToFile Error! ret = [%d]\n",ret);
	}
	else
	{
		if ((ret = UnionPrintPKNearOutofDateToFile(days,stdout)) < 0)
			printf("UnionPrintPKNearOutofDateToFile Error! ret = [%d]\n",ret);
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
		if ((ret = UnionPrintPKOutofDateToFile(stdout)) < 0)
			printf("UnionPrintPKOutofDateToFile Error! ret = [%d]\n",ret);
	}
	else
	{
		if ((ret = UnionPrintPKNearOutofMaxUseTimesToFile(times,stdout)) < 0)
			printf("UnionPrintPKNearOutofMaxUseTimesToFile Error! ret = [%d]\n",ret);
	}
	goto loop;
}

int EditInsert()
{
	int	ret;
	
	UnionPrintPK(&gpk);
	if (!UnionIsValidPK(&gpk))
	{
		printf("Des Key Invalid!\n");
		return(-1);
	}
	if (!UnionConfirm("Are you sure to add this des key?"))
		return(0);
	if ((ret = UnionInsertPKIntoKeyDB(&gpk)) < 0)
		printf("UnionInsertPKIntoKeyDB [%s] Error! ret = [%d]\n",gpk.fullName,ret);
	else
		printf("UnionInsertPKIntoKeyDB [%s] OK!\n",gpk.fullName);
	return(ret);
}

int EditDelete()
{
	int	ret;
	
	UnionPrintPK(&gpk);
	if (!UnionConfirm("Are you sure to delete this des key?"))
		return(0);
	if ((ret = UnionDeletePKFromKeyDB(gpk.fullName)) < 0)
		printf("UnionDeletePKFromKeyDB [%s] Error! ret = [%d]\n",gpk.fullName,ret);
	else
		printf("UnionDeletePKFromKeyDB [%s] OK!\n",gpk.fullName);
	return(ret);
}	
	
int EditUpdate()
{
	int	ret;
	
	UnionPrintPK(&gpk);
	if (!UnionIsValidPK(&gpk))
	{
		printf("Des Key Invalid!\n");
		return(-1);
	}
	if (!UnionConfirm("Are you sure to update this des key?"))
		return(0);
	if ((ret = UnionUpdatePKAttrInKeyDB(&gpk)) < 0)
		printf("UnionUpdatePKAttrInKeyDB [%s] Error! ret = [%d]\n",gpk.fullName,ret);
	else
		printf("UnionUpdatePKAttrInKeyDB [%s] OK!\n",gpk.fullName);
	return(ret);
}

int EditUpdateValue()
{
	int	ret;
	char	tmpBuf[2048+1];
	
	UnionPrintPK(&gpk);
	if (!UnionIsValidPK(&gpk))
	{
		printf("Des Key Invalid!\n");
		return(-1);
	}
	if (!UnionConfirm("Are you sure to update value of this des key?"))
		return(0);
	memset(tmpBuf,0,sizeof(tmpBuf));
	bcdhex_to_aschex(gpk.value,UnionGetAscCharLengthOfPK(gpk.length)/2,tmpBuf);
	if ((ret = UnionUpdatePKValueInKeyDB(gpk.fullName,tmpBuf,gpk.checkValue)) < 0)
		printf("UnionUpdatePKValueInKeyDB [%s] Error! ret = [%d]\n",gpk.fullName,ret);
	else
		printf("UnionUpdatePKValueInKeyDB [%s] OK!\n",gpk.fullName);
	return(ret);
}

int EditPrint()
{
	int	ret;
	
	if ((ret = UnionReadPKFromKeyDBAnyway(&gpk)) < 0)
		printf("UnionReadPKFromKeyDBAnyway [%s] Error! ret = [%d]\n",gpk.fullName,ret);
	else
		UnionPrintPK(&gpk);
	return(ret);
}

int EditRead()
{
	int	ret;
	
	if ((ret = UnionReadPKFromKeyDBAnyway(&gpk)) < 0)
		printf("UnionReadPKFromKeyDBAnyway [%s] Error! ret = [%d]\n",gpk.fullName,ret);
	else
		UnionPrintPK(&gpk);
	return(ret);
}

int EditUse()
{
	int	ret;
	
	if ((ret = UnionUsePKFromKeyDB(&gpk)) < 0)
		printf("UnionUsePKFromKeyDB [%s] Error! ret = [%d]\n",gpk.fullName,ret);
	else
		UnionPrintPK(&gpk);
	return(ret);
}


int EditPrintStatus()
{
	int	ret;
	
	if ((ret = UnionPrintStatusOfPKDBToFile(stdout)) < 0)
		printf("UnionReadPKFromKeyDBAnyway Error! ret = [%d]\n",ret);
	return(ret);
}

int EditPrintAllKeys()
{
	int	ret;
	
	if ((ret = UnionPrintAllPKsInKeyDBToFile(stdout)) < 0)
		printf("UnionPrintAllPKsInKeyDBToFile Error! ret = [%d]\n",ret);
	return(ret);
}

int EditPrintPKOfApp()
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
	UnionPrintPKExpectedToFile(tmpBuf,"","",stdout);
	goto loop;
}

int EditPrintPKOfOwner()
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
	UnionPrintPKExpectedToFile("",tmpBuf,"",stdout);
	goto loop;
}


int EditPrintPKOfName()
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
	UnionPrintPKExpectedToFile("","",tmpBuf,stdout);
	goto loop;
}

int EditPrintPKOfExpected()
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
	UnionPrintPKExpectedToFile(tmpBuf1,tmpBuf2,"",stdout);
	goto loop;
}

int UnionInputPKLength()
{
	char *param;
	
loopUnionInput:
	param = UnionInput("UnionInput PK Length:(128/256/512/1024/quit)");
	UnionToUpperCase(param);
	if (strcasecmp(param,"QUIT") == 0)
		return(0);

	if (strcasecmp(param,"128") == 0)
		gpk.length = con512RSAPair;
	else if (strcasecmp(param,"256") == 0)
		gpk.length = con1024RSAPair;
	else if (strcasecmp(param,"512") == 0)
		gpk.length = con2048RSAPair;
	else if (strcasecmp(param,"1024") == 0)
		gpk.length = con4096RSAPair;
	else
	{
		printf("Invalid PK Length [%s]!\n",param);
		goto loopUnionInput;
	}
	
	return(0);
}

int UnionInputPKValue()
{
	char	param[2048+1];
	char	*p;
	int	i;
	
loopUnionInput:
	memset(param,0,sizeof(param));
	for (i = 0; i < UnionGetAscCharLengthOfPK(gpk.length) / 128;)
	{
		if (UnionIsQuit(p = UnionInput("Input the %dth 128 chars of %04d length Public Key::\n",i+1,UnionGetAscCharLengthOfPK(gpk.length))))
			return(0);
		if (strlen(p) != 128)
		{
			printf("length of PK part error!\n");
			continue;
		}
		memcpy(param+i*128,p,128);
		i++;
	}
	if (strlen(param) != UnionGetAscCharLengthOfPK(gpk.length))
	{
		printf("invalid key cryptogram [%s]!\n",param);
		goto loopUnionInput;
	}
	else
	{
		memset(gpk.value,0,sizeof(gpk.value));
		aschex_to_bcdhex(param,UnionGetAscCharLengthOfPK(gpk.length),gpk.value);
	}
	return(0);
}

int UnionInputPKCheckValue()
{
	char	*param;
loopUnionInput:
	param = UnionInput("UnionInput the checkvalue\n++++++++++++++++\n");
	if (!UnionIsValidPKCheckValue(param))
	{
		printf("Invalid checkvalue!\n");
		goto loopUnionInput;
	}
	
	strcpy(gpk.checkValue,param);
	return(0);
}

int UnionInputPKType()
{
	char	*param;

loopUnionInput:
	param = UnionInput("UnionInput Key Type,quit to exit\n");
	UnionToUpperCase(param);
	if (strcasecmp(param,"QUIT") == 0)
		return(0);
	else if (strcasecmp(param,"SIGNATURE") == 0)
		gpk.type = conSignature;
	else if (strcasecmp(param,"ENCRYPTION") == 0)
		gpk.type = conEncryption;
	else if (strcasecmp(param,"USER") == 0)
		gpk.type = conSelfDefinedPK;
	else
	{
		printf("Invalid KeyType [%s]!\n",param);
		goto loopUnionInput;
	}
		
	return(0);
}

int UnionInputPKActiveDate()
{
	char	*param;

	if (UnionConfirm("Use current system date as the default effective date?"))
	{
		UnionGetFullSystemDate(gpk.activeDate);
		return(0);
	}
loopUnionInput:
	param = UnionInput("UnionInput effective date(YYYYMMDD)::");
	if (strlen(param) != 8)
	{
		printf("Invalid date!\n");
		goto loopUnionInput;
	}
	strcpy(gpk.activeDate,param);
	return(0);
}

int UnionInputPKMaxEffectiveDays()
{
	char	*param;

loopUnionInput:
	if (UnionIsQuit(param = UnionInput("UnionInput maxEffectiveDays::")))
		return(0);
	gpk.maxEffectiveDays = atol(param);
	return(0);
}

int UnionInputPKMaxUseTimes()
{
	char	*param;

loopUnionInput:
	if (UnionIsQuit(param = UnionInput("UnionInput maxUseTimes::")))
		return(0);
	gpk.maxUseTimes = atol(param);
	return(0);
}

int UnionInputPKWindowBetweenKeyVer()
{
	char	*param;

loopUnionInput:
	if (UnionIsQuit(param = UnionInput("UnionInput windowBetweenKeyVer::")))
		return(0);
	gpk.windowBetweenKeyVer = atol(param);
	return(0);
}

int UnionInputPKFullName()
{
	char *param;

loopUnionInput:	
	if (!UnionIsValidPKFullName(param = UnionInput("UnionInput full name of deskey like keyApp.owner.keyName\n")))
	{
		if (UnionIsQuit(param))
			return(0);
		printf("invalid fullName!\n");
		goto loopUnionInput;
	}
	
	strcpy(gpk.fullName,param);
	return(0);
}
	
int ChangeMaxKeyNumInPKDB(int argc,char *argv[])
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
	
	if (UnionConfirm("Change Size Of PKDB of maxKeyNum = [%ld]?",gmaxKeyNum))
	{
		if ((ret = UnionChangeSizeOfPKDB(gmaxKeyNum)) < 0)
			printf("UnionChangeSizeOfPKDB Error! ret = [%d]\n",ret);
		else
			printf("UnionChangeSizeOfPKDB OK!\n");
	}
	return(UnionTaskActionBeforeExit());
}

int UpdatePK(int argc,char *argv[])
{
	int		ret;
	
	if (argc > 0)
	{
		if (!UnionIsValidPKFullName(argv[0]))
		{
			printf("Invalid full name [%s]\n",argv[0]);
			return(UnionTaskActionBeforeExit());
		}
		strcpy(gpk.fullName,argv[0]);
		if ((ret = UnionReadPKFromKeyDBAnyway(&gpk)) < 0)
			printf("UnionReadPKFromKeyDBAnyway [%s] Error! ret = [%d]\n",gpk.fullName,ret);
		else
		{
			printf("Old key::\n");
			UnionPrintPK(&gpk);
		}
	}

	if (argc > 1)
	{
		if (!UnionIsValidPKValue(argv[1]))
		{
			printf("Invalid des key cryptogram [%s]!\n",argv[1]);
			return(UnionTaskActionBeforeExit());
		}
		strcpy(gpk.value,argv[1]);
		//printf("[%s][%s]\n",gpk.value,argv[1]);
	}
	if (argc > 2)
	{
		if (!UnionIsValidPKCheckValue(argv[2]))
		{
			printf("Invalid des key check value [%s]!\n",argv[2]);
			return(UnionTaskActionBeforeExit());
		}
		strcpy(gpk.checkValue,argv[2]);
	}
	if (argc > 0)
	{
		printf("\n\nNew Key::\n");
		UnionPrintPK(&gpk);
		if (!UnionConfirm("Are you sure of updating this key?"))
			return(UnionTaskActionBeforeExit());
		if ((ret = UnionUpdatePKValueInKeyDB(gpk.fullName,gpk.value,gpk.checkValue)) < 0)
			printf("UnionUpdatePKValueInKeyDB [%s] Error! ret = [%d]\n",gpk.fullName,ret);
		else
			printf("UnionUpdatePKValueInKeyDB [%s] OK!\n",gpk.fullName);
	}
	return(UnionTaskActionBeforeExit());
}

int UpdatePKLength(int argc,char *argv[])
{
	int		ret;
	
	if (argc < 2)
	{
		printf("Usage:: %s fullKeyName newKeyLength\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}
	
	if (!UnionIsValidPKFullName(argv[0]))
	{
		printf("Invalid full name [%s]\n",argv[0]);
		return(UnionTaskActionBeforeExit());
	}
	strcpy(gpk.fullName,argv[0]);
	if ((ret = UnionReadPKFromKeyDBAnyway(&gpk)) < 0)
	{
		printf("UnionReadPKFromKeyDBAnyway [%s] Error! ret = [%d]\n",gpk.fullName,ret);
		return(UnionTaskActionBeforeExit());
	}
	printf("Old key::\n");
	UnionPrintPK(&gpk);

	if ((gpk.length = UnionConvertPKLength(atoi(argv[1]))) < 0)
	{
		printf("Invalid des key length [%s]\n",argv[1]);
		return(UnionTaskActionBeforeExit());
	}
	printf("\n\nNew Key::\n");
	UnionPrintPK(&gpk);
	if (!UnionConfirm("Are you sure of updating this key?"))
		return(UnionTaskActionBeforeExit());
	if ((ret = UnionUpdatePKAttrInKeyDB(&gpk)) < 0)
		printf("UnionUpdatePKAttrInKeyDB [%s] Error! ret = [%d]\n",gpk.fullName,ret);
	else
		printf("UnionUpdatePKAttrInKeyDB [%s] OK!\n",gpk.fullName);
	return(UnionTaskActionBeforeExit());
}


int UpdatePKActiveDate(int argc,char *argv[])
{
	int		ret;
	
	if (argc < 2)
	{
		printf("Usage:: %s fullKeyName newActiveDate\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}
	
	if (!UnionIsValidPKFullName(argv[0]))
	{
		printf("Invalid full name [%s]\n",argv[0]);
		return(UnionTaskActionBeforeExit());
	}
	strcpy(gpk.fullName,argv[0]);
	if ((ret = UnionReadPKFromKeyDBAnyway(&gpk)) < 0)
	{
		printf("UnionReadPKFromKeyDBAnyway [%s] Error! ret = [%d]\n",gpk.fullName,ret);
		return(UnionTaskActionBeforeExit());
	}
	printf("Old key::\n");
	UnionPrintPK(&gpk);

	if (strlen(argv[1]) != 8)
	{
		printf("Invalid activeDate [%s]\n",argv[1]);
		return(UnionTaskActionBeforeExit());
	}
	strcpy(gpk.activeDate,argv[1]);
	printf("\n\nNew Key::\n");
	UnionPrintPK(&gpk);
	if (!UnionConfirm("Are you sure of updating this key?"))
		return(UnionTaskActionBeforeExit());
	if ((ret = UnionUpdatePKAttrInKeyDB(&gpk)) < 0)
		printf("UnionUpdatePKAttrInKeyDB [%s] Error! ret = [%d]\n",gpk.fullName,ret);
	else
		printf("UnionUpdatePKAttrInKeyDB [%s] OK!\n",gpk.fullName);
	return(UnionTaskActionBeforeExit());
}

int UpdatePKMaxEffectiveDays(int argc,char *argv[])
{
	int		ret;
	
	if (argc < 2)
	{
		printf("Usage:: %s fullKeyName newMaxEffectiveDays\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}
	
	if (!UnionIsValidPKFullName(argv[0]))
	{
		printf("Invalid full name [%s]\n",argv[0]);
		return(UnionTaskActionBeforeExit());
	}
	strcpy(gpk.fullName,argv[0]);
	if ((ret = UnionReadPKFromKeyDBAnyway(&gpk)) < 0)
	{
		printf("UnionReadPKFromKeyDBAnyway [%s] Error! ret = [%d]\n",gpk.fullName,ret);
		return(UnionTaskActionBeforeExit());
	}
	printf("Old key::\n");
	UnionPrintPK(&gpk);

	gpk.maxEffectiveDays = atol(argv[1]);
	printf("\n\nNew Key::\n");
	UnionPrintPK(&gpk);
	if (!UnionConfirm("Are you sure of updating this key?"))
		return(UnionTaskActionBeforeExit());
	if ((ret = UnionUpdatePKAttrInKeyDB(&gpk)) < 0)
		printf("UnionUpdatePKAttrInKeyDB [%s] Error! ret = [%d]\n",gpk.fullName,ret);
	else
		printf("UnionUpdatePKAttrInKeyDB [%s] OK!\n",gpk.fullName);
	return(UnionTaskActionBeforeExit());
}

int UpdatePKMaxUseTimes(int argc,char *argv[])
{
	int		ret;
	
	if (argc < 2)
	{
		printf("Usage:: %s fullKeyName newMaxUseTimes\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}
	
	if (!UnionIsValidPKFullName(argv[0]))
	{
		printf("Invalid full name [%s]\n",argv[0]);
		return(UnionTaskActionBeforeExit());
	}
	strcpy(gpk.fullName,argv[0]);
	if ((ret = UnionReadPKFromKeyDBAnyway(&gpk)) < 0)
	{
		printf("UnionReadPKFromKeyDBAnyway [%s] Error! ret = [%d]\n",gpk.fullName,ret);
		return(UnionTaskActionBeforeExit());
	}
	printf("Old key::\n");
	UnionPrintPK(&gpk);

	gpk.maxUseTimes = atol(argv[1]);
	printf("\n\nNew Key::\n");
	UnionPrintPK(&gpk);
	if (!UnionConfirm("Are you sure of updating this key?"))
		return(UnionTaskActionBeforeExit());
	if ((ret = UnionUpdatePKAttrInKeyDB(&gpk)) < 0)
		printf("UnionUpdatePKAttrInKeyDB [%s] Error! ret = [%d]\n",gpk.fullName,ret);
	else
		printf("UnionUpdatePKAttrInKeyDB [%s] OK!\n",gpk.fullName);
	return(UnionTaskActionBeforeExit());
}

int UpdatePKWindowBetweenKeyVer(int argc,char *argv[])
{
	int		ret;
	
	if (argc < 2)
	{
		printf("Usage:: %s fullKeyName newWindowBetweenKeyVer\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}
	
	if (!UnionIsValidPKFullName(argv[0]))
	{
		printf("Invalid full name [%s]\n",argv[0]);
		return(UnionTaskActionBeforeExit());
	}
	strcpy(gpk.fullName,argv[0]);
	if ((ret = UnionReadPKFromKeyDBAnyway(&gpk)) < 0)
	{
		printf("UnionReadPKFromKeyDBAnyway [%s] Error! ret = [%d]\n",gpk.fullName,ret);
		return(UnionTaskActionBeforeExit());
	}
	printf("Old key::\n");
	UnionPrintPK(&gpk);

	gpk.windowBetweenKeyVer = atol(argv[1]);
	printf("\n\nNew Key::\n");
	UnionPrintPK(&gpk);
	if (!UnionConfirm("Are you sure of updating this key?"))
		return(UnionTaskActionBeforeExit());
	if ((ret = UnionUpdatePKAttrInKeyDB(&gpk)) < 0)
		printf("UnionUpdatePKAttrInKeyDB [%s] Error! ret = [%d]\n",gpk.fullName,ret);
	else
		printf("UnionUpdatePKAttrInKeyDB [%s] OK!\n",gpk.fullName);
	return(UnionTaskActionBeforeExit());
}

int UpdatePKUseOldVerKey(int argc,char *argv[])
{
	int		ret;
	
	if (argc < 1)
	{
		printf("Usage:: %s fullKeyName\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}
	
	if (!UnionIsValidPKFullName(argv[0]))
	{
		printf("Invalid full name [%s]\n",argv[0]);
		return(UnionTaskActionBeforeExit());
	}
	strcpy(gpk.fullName,argv[0]);
	if ((ret = UnionReadPKFromKeyDBAnyway(&gpk)) < 0)
	{
		printf("UnionReadPKFromKeyDBAnyway [%s] Error! ret = [%d]\n",gpk.fullName,ret);
		return(UnionTaskActionBeforeExit());
	}
	printf("Old key::\n");
	UnionPrintPK(&gpk);

	gpk.oldVerEffective = 1;
	printf("\n\nNew Key::\n");
	UnionPrintPK(&gpk);
	if (!UnionConfirm("Are you sure of updating this key?"))
		return(UnionTaskActionBeforeExit());
	if ((ret = UnionUpdatePKAttrInKeyDB(&gpk)) < 0)
		printf("UnionUpdatePKAttrInKeyDB [%s] Error! ret = [%d]\n",gpk.fullName,ret);
	else
		printf("UnionUpdatePKAttrInKeyDB [%s] OK!\n",gpk.fullName);
	return(UnionTaskActionBeforeExit());
}

int UpdatePKUnuseOldVerKey(int argc,char *argv[])
{
	int		ret;
	
	if (argc < 1)
	{
		printf("Usage:: %s fullKeyName\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}
	
	if (!UnionIsValidPKFullName(argv[0]))
	{
		printf("Invalid full name [%s]\n",argv[0]);
		return(UnionTaskActionBeforeExit());
	}
	strcpy(gpk.fullName,argv[0]);
	if ((ret = UnionReadPKFromKeyDBAnyway(&gpk)) < 0)
	{
		printf("UnionReadPKFromKeyDBAnyway [%s] Error! ret = [%d]\n",gpk.fullName,ret);
		return(UnionTaskActionBeforeExit());
	}
	printf("Old key::\n");
	UnionPrintPK(&gpk);

	gpk.oldVerEffective = 0;
	printf("\n\nNew Key::\n");
	UnionPrintPK(&gpk);
	if (!UnionConfirm("Are you sure of updating this key?"))
		return(UnionTaskActionBeforeExit());
	if ((ret = UnionUpdatePKAttrInKeyDB(&gpk)) < 0)
		printf("UnionUpdatePKAttrInKeyDB [%s] Error! ret = [%d]\n",gpk.fullName,ret);
	else
		printf("UnionUpdatePKAttrInKeyDB [%s] OK!\n",gpk.fullName);
	return(UnionTaskActionBeforeExit());
}

int UnionInputPKContainer()
{
	char	*param;
	char	tmpBuf[100];
	
loopUnionInput:
	memset(tmpBuf,0,sizeof(tmpBuf));
	param = UnionInput("UnionInput container::");
	if (strlen(param) >= sizeof(gpk.container))
	{
		printf("too long container name!\n");
		goto loopUnionInput;
	}
	strcpy(tmpBuf,param);
	if (UnionIsQuit(param))
		return(0);
	strcpy(gpk.container,tmpBuf);
	return(0);
}

int UpdatePKContainer(int argc,char *argv[])
{
	int		ret;
	
	if (argc < 2)
	{
		printf("Usage:: %s fullKeyName newContainer\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}
	
	if (!UnionIsValidPKFullName(argv[0]))
	{
		printf("Invalid full name [%s]\n",argv[0]);
		return(UnionTaskActionBeforeExit());
	}
	strcpy(gpk.fullName,argv[0]);
	if ((ret = UnionReadPKFromKeyDBAnyway(&gpk)) < 0)
	{
		printf("UnionReadPKFromKeyDBAnyway [%s] Error! ret = [%d]\n",gpk.fullName,ret);
		return(UnionTaskActionBeforeExit());
	}
	printf("Old key::\n");
	UnionPrintPK(&gpk);

	if (strlen(argv[1]) >= sizeof(gpk.container))
	{
		printf("container name too long!\n");
		return(UnionTaskActionBeforeExit());
	}
	strcpy(gpk.container,argv[1]);
	printf("\n\nNew Key::\n");
	UnionPrintPK(&gpk);
	if (!UnionConfirm("Are you sure of updating this key?"))
		return(UnionTaskActionBeforeExit());
	if ((ret = UnionUpdatePKAttrInKeyDB(&gpk)) < 0)
		printf("UnionUpdatePKAttrInKeyDB [%s] Error! ret = [%d]\n",gpk.fullName,ret);
	else
		printf("UnionUpdatePKAttrInKeyDB [%s] OK!\n",gpk.fullName);
	return(UnionTaskActionBeforeExit());
}
int main(int argc,char **argv)
{
	int	ret;
	
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
		
	memset(&gpk,0,sizeof(gpk));
	UnionFormDefaultPK(&gpk,"00.00.00","",conSignature,"");
	
	if (strcasecmp(argv[1],"CREATE") == 0)
		return(CreatePKDB(argc-2,&argv[2]));
	if (strcasecmp(argv[1],"DROP") == 0)
		return(DropPKDB());
	if (strcasecmp(argv[1],"MAXKEYNUM") == 0)
		return(ChangeMaxKeyNumInPKDB(argc-2,&argv[2]));
	if (strcasecmp(argv[1],"CONNECT") == 0)
	{
		if ((ret = UnionConnectPKDB()) < 0)
		{
			printf("in main:: UnionConnectPKDB Error! ret = [%d]\n",ret);
			return(ret);
		}
		else
			printf("UnionConnectPKDB OK!\n");
		return(UnionTaskActionBeforeExit());
	}
	if (strcasecmp(argv[1],"MIRROR") == 0)
	{
		if ((ret = UnionMirrorPKDBIntoDiskArtifically()) < 0)
			printf("UnionMirrorPKDBIntoDiskArtifically Error! ret = [%d]\n",ret);
		else
			printf("UnionMirrorPKDBIntoDiskArtifically OK!\n");
		UnionTaskActionBeforeExit();
		return(ret);
	}
		
	/*
	if ((ret = UnionConnectPKDB()) < 0)
	{
		printf("in main:: UnionConnectPKDB Error! ret = [%d]\n",ret);
		UnionTaskActionBeforeExit();
		return(ret);
	}
	*/
	if (strcasecmp(argv[1],"RELOAD") == 0)
	{
		if ((ret = UnionLoadPKDBIntoMemory()) < 0)
			printf("UnionLoadPKDBIntoMemory Error! ret = [%d]\n",ret);
		else
			printf("UnionLoadPKDBIntoMemory OK!\n");
		UnionTaskActionBeforeExit();
		return(ret);
	}
	if (strcasecmp(argv[1],"INSERT") == 0)
		return(InsertPK(argc-2,&argv[2],1));
	if (strcasecmp(argv[1],"INSERTANYWAY") == 0)
		return(InsertPK(argc-2,&argv[2],0));
	if (strcasecmp(argv[1],"UPDATE") == 0)
		return(UpdatePK(argc-2,&argv[2]));
	if (strcasecmp(argv[1],"DELETE") == 0)
		return(DeletePK(argc-2,&argv[2],1));
	if (strcasecmp(argv[1],"DELETEANYWAY") == 0)
		return(DeletePK(argc-2,&argv[2],0));
	if (strcasecmp(argv[1],"PRINT") == 0)
		return(PrintPK(argc-2,&argv[2]));
	if (strcasecmp(argv[1],"READ") == 0)
		return(ReadPK(argc-2,&argv[2]));
	if (strcasecmp(argv[1],"USE") == 0)
		return(UsePK(argc-2,&argv[2]));
	if (strcasecmp(argv[1],"ALL") == 0)
	{
		if ((ret = UnionPrintAllPKsInKeyDBToFile(stdout)) < 0)
			printf("UnionPrintAllPKsInKeyDBToFile Error! ret = [%d]\n",ret);
		return(UnionTaskActionBeforeExit());
	}
	if (strcasecmp(argv[1],"APP") == 0)
		return(PrintAllPKForApp(argc-2,&argv[2]));
	if (strcasecmp(argv[1],"OWNER") == 0)
		return(PrintAllPKForOwner(argc-2,&argv[2]));
	if (strcasecmp(argv[1],"NAME") == 0)
		return(PrintAllPKForName(argc-2,&argv[2]));
	if (strcasecmp(argv[1],"LIKE") == 0)
		return(PrintAllPKExpected(argc-2,&argv[2]));
	if (strcasecmp(argv[1],"LENGTH") == 0)
		return(UpdatePKLength(argc-2,&argv[2]));
	if (strcasecmp(argv[1],"ACTIVEDATE") == 0)
		return(UpdatePKActiveDate(argc-2,&argv[2]));
	if (strcasecmp(argv[1],"MAXEFFECTIVEDAYS") == 0)
		return(UpdatePKMaxEffectiveDays(argc-2,&argv[2]));
	if (strcasecmp(argv[1],"MAXUSETIMES") == 0)
		return(UpdatePKMaxUseTimes(argc-2,&argv[2]));
	if (strcasecmp(argv[1],"WINDOW") == 0)
		return(UpdatePKWindowBetweenKeyVer(argc-2,&argv[2]));
	if (strcasecmp(argv[1],"CONTAINER") == 0)
		return(UpdatePKContainer(argc-2,&argv[2]));
	if (strcasecmp(argv[1],"USEOLDVER") == 0)
		return(UpdatePKUseOldVerKey(argc-2,&argv[2]));
	if (strcasecmp(argv[1],"UNUSEOLDVER") == 0)
		return(UpdatePKUnuseOldVerKey(argc-2,&argv[2]));
		
	if (strcasecmp(argv[1],"STATUS") == 0)
	{
		if ((ret = UnionPrintStatusOfPKDBToFile(stdout)) < 0)
			printf("UnionPrintStatusOfPKDBToFile Error! ret = [%d]\n",ret);
		return(UnionTaskActionBeforeExit());
	}
	if (strcasecmp(argv[1],"LOOP") == 0)
		return(LoopTest(argc-2,&argv[2]));
	if (strcasecmp(argv[1],"EDIT") == 0)
		return(EditPK());
	
	if (strcasecmp(argv[1],"OUTDATE") == 0)
	{
		if (argc == 2)
		{
			if ((ret = UnionPrintPKOutofDateToFile(stdout)) < 0)
				printf("UnionPrintPKOutofDateToFile Error! ret = [%d]\n",ret);
		}
		else
		{
			if ((ret = UnionPrintPKNearOutofDateToFile(atol(argv[2]),stdout)) < 0)
				printf("UnionPrintPKNearOutofDateToFile Error! ret = [%d]\n",ret);
		}
		return(UnionTaskActionBeforeExit());
	}
	if (strcasecmp(argv[1],"OUTTIMES") == 0)
	{
		if (argc == 2)
		{
			if ((ret = UnionPrintPKOutofDateToFile(stdout)) < 0)
				printf("UnionPrintPKOutofDateToFile Error! ret = [%d]\n",ret);
		}
		else
		{
			if ((ret = UnionPrintPKNearOutofMaxUseTimesToFile(atol(argv[2]),stdout)) < 0)
				printf("UnionPrintPKNearOutofMaxUseTimesToFile Error! ret = [%d]\n",ret);
		}
		return(UnionTaskActionBeforeExit());
	}
	if (strcasecmp(argv[1],"RECSIZE") == 0)
	{
		printf("size of a deskey = [%04d]\n",sizeof(gpk));
		return(UnionTaskActionBeforeExit());
	}		
			
	UnionHelp();
	return(UnionTaskActionBeforeExit());
}
