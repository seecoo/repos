//	Wolfgang Wang
//	2003/09/09

#define _UnionTask_3_x_
#define _UnionDesKeyDB_2_x_
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

#include "unionDesKeyDB.h"

TUnionDesKey		gdesKey;

PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	UnionDisconnectDesKeyDB();
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
	printf("	maxkeynum newMaxKeyNum change maxKeyNum of the des key db\n");
	printf("	insert	fullKeyName keyType keyLength keyValue keyCheckValue\n");
	printf("	update	fullKeyName keyValue keyCheckValue\n");
	printf("	delete	fullKeyName Delete a des key\n");
	printf("	deleteallofapp	idOfApp Delete all des keys of app\n");
	printf("	print	fullKeyName Print a des key\n");
	printf("	read	fullKeyName read a des key\n");
	printf("	all 	Print all des keys\n");
	printf("	app 	print all des keys of application\n");
	printf("	owner	print all des keys of owner\n");
	printf("	name	print all des keys of name\n");
	printf("	like	print all des keys like {app,owner,name}\n");
	printf("	status 	Print keydb status\n");
	printf("	edit	Read a des key\n");
	printf("	length fullKeyName newKeyLength\n");
	printf("	activeDate fullKeyName newActiveDate\n");
	printf("	passiveDate fullKeyName newPassiveDate\n");
	printf("	passive [days] Print all keys which are passive or will be passive after days.\n");
	printf("	loop looptimes fullKeyName  Loop testing\n");

	EditHelp();
	
	return(0);
}

int EditHelp()
{
	printf(" edit commands\n");
	printf("	insert	Insert des key\n");
	printf("	delete	Delete des key\n");
	printf("	update	update des key\n");
	printf("	print	print des key\n");
	printf("	read	read des key\n");
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
	printf("	passivedate Input des key passive date\n");
	printf("	fullname Input des key fullname\n");
	printf("	passive print passive desKeys.\n");
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
		if (!UnionIsValidDesKeyCryptogram(argv[3]))
		{
			printf("Invalid des key cryptogram [%s]!\n",argv[3]);
			return(UnionTaskActionBeforeExit());
		}
		strcpy(gdesKey.value,argv[3]);
	}
	if (argc > 4)
	{
		if (!UnionIsValidDesKeyCheckValue(argv[4]))
		{
			printf("Invalid des key check value [%s]!\n",argv[4]);
			return(UnionTaskActionBeforeExit());
		}
		strcpy(gdesKey.checkValue,argv[4]);
	}
	if (argc > 5)
	{
		if (strlen(argv[5]) != 8)
		{
			printf("Invalid des key active date [%s]!\n",argv[5]);
			return(UnionTaskActionBeforeExit());
		}
		strcpy(gdesKey.activeDate,argv[5]);
	}
	if (argc > 6)
	{
		if (strlen(argv[6]) != 8)
		{
			printf("Invalid des key passive date [%s]!\n",argv[6]);
			return(UnionTaskActionBeforeExit());
		}
		strcpy(gdesKey.passiveDate,argv[6]);
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
	return(ReadDesKey(argc,argv));
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
			printf("UnionReadDesKeyFromKeyDB [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
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
	if (strcasecmp(p,"UPDATE") == 0)
		EditUpdate();
	if (strcasecmp(p,"PRINT") == 0)
		EditPrint();
	if (strcasecmp(p,"READ") == 0)
		EditRead();
	if (strcasecmp(p,"ALL") == 0)
		EditPrintAllKeys();
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
	if (strcasecmp(p,"PASSIVEDATE") == 0)
		UnionInputDesKeyPassiveDate();
	if (strcasecmp(p,"FULLNAME") == 0)
		UnionInputDesKeyFullName();
	if (strcasecmp(p,"APP") == 0)
		EditPrintDesKeyOfApp();
	if (strcasecmp(p,"OWNER") == 0)
		EditPrintDesKeyOfOwner();
	if (strcasecmp(p,"NAME") == 0)
		EditPrintDesKeyOfName();
	if (strcasecmp(p,"LIKE") == 0)
		EditPrintDesKeyOfExpected();
	if (strcasecmp(p,"HELP") == 0)
		EditHelp();
	if (strcasecmp(p,"PASSIVE") == 0)
		EditPassive();
	goto loopEdit;
}
		
int EditPassive()
{
	int	ret;
	char	*p;
	long	days;
	
loop:
	if (UnionIsQuit(p = UnionInput("Input days after which the des keys will be passive,\ninput -1 to display all passive des keys.\ndays(exit or quit to exit)::\n")))
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
	if ((ret = UnionUpdateDesKeyInKeyDB(&gdesKey)) < 0)
		printf("UnionUpdateDesKeyInKeyDB [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
	else
		printf("UnionUpdateDesKeyInKeyDB [%s] OK!\n",gdesKey.fullName);
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
	
	if ((ret = UnionReadDesKeyFromKeyDB(&gdesKey)) < 0)
		printf("UnionReadDesKeyFromKeyDB [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
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
	else if (strcasecmp(param,"USER") == 0)
		gdesKey.type = conSelfDefinedKey;
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

int UnionInputDesKeyPassiveDate()
{
	char	*param;

loopUnionInput:
	param = UnionInput("UnionInput passive date(YYYYMMDD)::");
	if (strlen(param) != 8)
	{
		printf("Invalid date!\n");
		goto loopUnionInput;
	}
	strcpy(gdesKey.passiveDate,param);
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
			printf("UnionReadDesKeyFromKeyDB [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
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
		if ((ret = UnionUpdateDesKeyInKeyDB(&gdesKey)) < 0)
			printf("UnionUpdateDesKeyInKeyDB [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
		else
			printf("UnionUpdateDesKeyInKeyDB [%s] OK!\n",gdesKey.fullName);
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
		printf("UnionReadDesKeyFromKeyDB [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
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
	if ((ret = UnionUpdateDesKeyInKeyDB(&gdesKey)) < 0)
		printf("UnionUpdateDesKeyInKeyDB [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
	else
		printf("UnionUpdateDesKeyInKeyDB [%s] OK!\n",gdesKey.fullName);
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
		printf("UnionReadDesKeyFromKeyDB [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
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
	if ((ret = UnionUpdateDesKeyInKeyDB(&gdesKey)) < 0)
		printf("UnionUpdateDesKeyInKeyDB [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
	else
		printf("UnionUpdateDesKeyInKeyDB [%s] OK!\n",gdesKey.fullName);
	return(UnionTaskActionBeforeExit());
}

int UpdateDesKeyPassiveDate(int argc,char *argv[])
{
	int		ret;
	
	if (argc < 2)
	{
		printf("Usage:: %s fullKeyName newPassiveDate\n",UnionGetApplicationName());
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
		printf("UnionReadDesKeyFromKeyDB [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
		return(UnionTaskActionBeforeExit());
	}
	printf("Old key::\n");
	UnionPrintDesKey(&gdesKey);

	if (strlen(argv[1]) != 8)
	{
		printf("Invalid passiveDate [%s]\n",argv[1]);
		return(UnionTaskActionBeforeExit());
	}
	strcpy(gdesKey.passiveDate,argv[1]);
	printf("\n\nNew Key::\n");
	UnionPrintDesKey(&gdesKey);
	if (!UnionConfirm("Are you sure of updating this key?"))
		return(UnionTaskActionBeforeExit());
	if ((ret = UnionUpdateDesKeyInKeyDB(&gdesKey)) < 0)
		printf("UnionUpdateDesKeyInKeyDB [%s] Error! ret = [%d]\n",gdesKey.fullName,ret);
	else
		printf("UnionUpdateDesKeyInKeyDB [%s] OK!\n",gdesKey.fullName);
	return(UnionTaskActionBeforeExit());
}

int main(int argc,char **argv)
{
	int	ret;
	int	i;
	
	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		return(UnionHelp());
		
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"mngDesKeyDB")) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
		
	memset(&gdesKey,0,sizeof(gdesKey));
	gdesKey.length = con64BitsDesKey;
	gdesKey.type = conZPK;
	//memset(gdesKey.value,'0',sizeof(gdesKey.value)-1);
	//memset(gdesKey.checkValue,'0',sizeof(gdesKey.checkValue)-1);
	UnionGetFullSystemDate(gdesKey.activeDate);
	memcpy(gdesKey.passiveDate,"21000101",8);
	
	if (strcasecmp(argv[1],"CREATE") == 0)
		return(CreateDesKeyDB(argc-2,&argv[2]));
	if (strcasecmp(argv[1],"DROP") == 0)
		return(DropDesKeyDB());
	if (strcasecmp(argv[1],"MAXKEYNUM") == 0)
		return(ChangeMaxKeyNumInDesKeyDB(argc-2,&argv[2]));
	if (strcasecmp(argv[1],"CONNECT") == 0)
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
		
	if (strcasecmp(argv[1],"MIRROR") == 0)
	{
		if ((ret = UnionMirrorDesKeyDBIntoDiskArtifically()) < 0)
			printf("UnionMirrorDesKeyDBIntoDiskArtifically Error! ret = [%d]\n",ret);
		else
			printf("UnionMirrorDesKeyDBIntoDiskArtifically OK!\n");
		UnionTaskActionBeforeExit();
		return(ret);
	}
	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		printf("in main:: UnionConnectDesKeyDB Error! ret = [%d]\n",ret);
		UnionTaskActionBeforeExit();
		return(ret);
	}
	if (strcasecmp(argv[1],"RELOAD") == 0)
	{
		if ((ret = UnionLoadDesKeyDBIntoMemory()) < 0)
			printf("UnionLoadDesKeyDBIntoMemory Error! ret = [%d]\n",ret);
		else
			printf("UnionLoadDesKeyDBIntoMemory OK!\n");
		UnionTaskActionBeforeExit();
		return(ret);
	}
	
	if (strcasecmp(argv[1],"INSERT") == 0)
		return(InsertDesKey(argc-2,&argv[2],1));
	if (strcasecmp(argv[1],"INSERTANYWAY") == 0)
		return(InsertDesKey(argc-2,&argv[2],0));
	if (strcasecmp(argv[1],"UPDATE") == 0)
		return(UpdateDesKey(argc-2,&argv[2]));
	if (strcasecmp(argv[1],"DELETE") == 0)
		return(DeleteDesKey(argc-2,&argv[2],1));
	if (strcasecmp(argv[1],"DELETEALLOFAPP") == 0)
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
	if (strcasecmp(argv[1],"DELETEANYWAY") == 0)
		return(DeleteDesKey(argc-2,&argv[2],0));
	if (strcasecmp(argv[1],"PRINT") == 0)
		return(PrintDesKey(argc-2,&argv[2]));
	if (strcasecmp(argv[1],"READ") == 0)
		return(ReadDesKey(argc-2,&argv[2]));
	if (strcasecmp(argv[1],"ALL") == 0)
	{
		if ((ret = UnionPrintAllDesKeysInKeyDBToFile(stdout)) < 0)
			printf("UnionPrintAllDesKeysInKeyDBToFile Error! ret = [%d]\n",ret);
		return(UnionTaskActionBeforeExit());
	}
	if (strcasecmp(argv[1],"APP") == 0)
		return(PrintAllDesKeyForApp(argc-2,&argv[2]));
	if (strcasecmp(argv[1],"OWNER") == 0)
		return(PrintAllDesKeyForOwner(argc-2,&argv[2]));
	if (strcasecmp(argv[1],"NAME") == 0)
		return(PrintAllDesKeyForName(argc-2,&argv[2]));
	if (strcasecmp(argv[1],"LIKE") == 0)
		return(PrintAllDesKeyExpected(argc-2,&argv[2]));
	if (strcasecmp(argv[1],"LENGTH") == 0)
		return(UpdateDesKeyLength(argc-2,&argv[2]));
	if (strcasecmp(argv[1],"ACTIVEDATE") == 0)
		return(UpdateDesKeyActiveDate(argc-2,&argv[2]));
	if (strcasecmp(argv[1],"PASSIVEDATE") == 0)
		return(UpdateDesKeyPassiveDate(argc-2,&argv[2]));
		
	if (strcasecmp(argv[1],"STATUS") == 0)
	{
		if ((ret = UnionPrintStatusOfDesKeyDBToFile(stdout)) < 0)
			printf("UnionPrintStatusOfDesKeyDBToFile Error! ret = [%d]\n",ret);
		return(UnionTaskActionBeforeExit());
	}
	if (strcasecmp(argv[1],"LOOP") == 0)
		return(LoopTest(argc-2,&argv[2]));
	if (strcasecmp(argv[1],"EDIT") == 0)
		return(EditDesKey());
	
	if (strcasecmp(argv[1],"PASSIVE") == 0)
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
			
	UnionHelp();
	return(UnionTaskActionBeforeExit());
}

