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

#include "sjl06.h"

#include "unionDesKeyDB.h"


TUnionDesKey		gdesKey;
char			gsuffix[40];
char			gprefix[40];
char			gapp[40],gname[40];
long			gmin=1,gmax=1,gstep=1,gchars=4;
int			gcomponentNum = 2;
TUnionSJL06		gsjl06;
char			gfullKeyName[100];
char			gparam[20][80];

PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	UnionDisconnectDesKeyDB();
	exit(0);	//return(exit(0));
}

int UnionHelp()
{
	printf("Usage:: %s\n",UnionGetApplicationName());
	printf(" this executable is used to insert keys batchlly.");

	EditHelp();
	
	return(0);
}

int EditHelp()
{
	printf(" edit commands\n");
	printf("	insert	batch insert keys\n");
	printf("	delete	batch insert keys\n");
	printf("	update  batch update keys\n");
	printf("	print	batch print keys\n");
	printf("	generate batch generate key values\n");
	printf("	\n");
	printf("	app	set application id\n");
	printf("	name	set key name\n");
	printf("	type	set key type\n");
	printf("	length	set key length\n");
	printf("	activeDate set active date\n");
	printf("	passiveDate set passive date\n");
	printf("	prefix	set owner prefix\n");
	printf("	suffix	set owner suffix\n");
	printf("	min	set min owner\n");
	printf("	max	set max owner\n");
	printf("	step	set step of two owners\n");
	printf("	chars	set how many char positions the middle part occupies\n");
	printf("	comnum	set component num\n");
	printf("	par	set the specified parameter\n");
	return(0);
}

int main(int argc,char **argv)
{
	int	ret;
	
	UnionSetApplicationName(argv[0]);
		
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
		
	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		printf("in main:: UnionConnectDesKeyDB Error! ret = [%d]\n",ret);
		UnionTaskActionBeforeExit();
		return(ret);
	}

	return(BatchEditDesKeys());
}


int BatchEditDesKeys()
{
	char	*p;
	int	ret;
	char	type[10];
	char	choice[100];
	//TUnionDesKey	defaultKey;
		
	UnionFormDefaultDesKey(&gdesKey,"00.00.00","",conZMK,"");
	//memset(gdesKey.fullName,0,sizeof(gdesKey.fullName));
	gdesKey.length = con128BitsDesKey;
	gdesKey.type = conZMK;
	memset(gapp,0,sizeof(gapp));
	strcpy(gapp,"01");
	memset(gname,0,sizeof(gname));
	strcpy(gname,"zmk");
	memset(gsuffix,0,sizeof(gsuffix));
	memset(gprefix,0,sizeof(gprefix));
	UnionGetFullSystemDate(gdesKey.activeDate);
	strcpy(gdesKey.passiveDate,"21000101");
	goto loopEdit;
waitLoop:
	printf("按回车键继续...");
	getchar();
	getchar();
	//getchar();
loopEdit:
	//p = UnionInput("\n\nCommand>");
	
	memset(type,0,sizeof(type));
	UnionGetNameOfDesKeyType(gdesKey.type,type);
	system("clear");
	printf("01:插入  02:删除  03:更新  04:显示  05:生成\n");
	printf("\n");
	printf("11:app      设置应用编号,当前值::%s\n",gapp);
	printf("12:name     设置密钥名称,当前值::%s\n",gname);
	printf("13:type     设置密钥类型,当前值::%s\n",type);
	printf("14:length   设置密钥长度,当前值::%d\n",(gdesKey.length+1)*64);
	printf("\n");
	printf("21:prefix   设置属主前缀,当前值::%s\n",gprefix);
	printf("22:suffix   设置属主后缀,当前值::%s\n",gsuffix);
	printf("23:min      设置起始偏移,当前值::%ld\n",gmin);
	printf("24:max      设置最大偏移,当前值::%ld\n",gmax);
	printf("25:chars    设置偏移位数,当前值::%ld\n",gchars);
	printf("26:step     设置偏移步长,当前值::%ld\n",gstep);
	printf("\n");
	printf("31:comnum   设置密钥成分数量,当前值::%d\n",gcomponentNum);
	printf("\n");
	printf("41:par      设置打印参数\n");	
#ifdef _GangdongDevelopmentBank_
	printf("42:         设置行所号::%s\n",gparam[0]);
	printf("43:         设置机器号::%s\n",gparam[1]);
#endif
	printf("\n");
	printf("密码机IP地址 %s\n",gsjl06.staticAttr.ipAddr);
	printf("\n");
	printf("请选择::");
	scanf("%s",choice);
	system("clear");

	p = choice;
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
		strcpy(gfullKeyName,p);
		gmax = gmin;
		memset(gsuffix,0,sizeof(gsuffix));
		memset(gprefix,0,sizeof(gprefix));
		goto waitLoop;
	}
	else if (UnionIsValidIPAddrStr(p))
	{
		memset(&gsjl06,0,sizeof(gsjl06));
		strcpy(gsjl06.staticAttr.ipAddr,p);
		if ((ret = UnionSelectSJL06Rec(&gsjl06)) < 0)
		{
			printf("UnionSelectSJL06Rec [%s] Error\n",p);
		}
		else
			gsjl06.dynamicAttr.status = conOnlineSJL06;
	}
	else if (UnionIsQuit(p))
		return(UnionTaskActionBeforeExit());
	else if (strcmp(p,"COMNUM") == 0)
		EditComponentNum();
	else if (strcmp(p,"GENERATE") == 0)
	{
		EditGenerate();
		goto waitLoop;
	}
	else if (strcmp(p,"INSERT") == 0)
	{
		EditInsert();
		goto waitLoop;
	}
	else if (strcmp(p,"DELETE") == 0)
	{
		EditDelete();
		goto waitLoop;
	}
	else if (strcmp(p,"UPDATE") == 0)
	{
		EditUpdate();
		goto waitLoop;
	}
	else if (strcmp(p,"PRINT") == 0)
	{
		EditPrint();
		goto waitLoop;
	}
	else if (strcmp(p,"PAR") == 0)
		EditInputPar();
	else if (strcmp(p,"LENGTH") == 0)
		UnionInputDesKeyLength();
	else if (strcmp(p,"TYPE") == 0)
		UnionInputDesKeyType();
	else if (strcmp(p,"APP") == 0)
		UnionInputApp();
	else if (strcmp(p,"NAME") == 0)
		UnionInputName();
	else if (strcmp(p,"SUFFIX") == 0)
		UnionInputSuffix();
	else if (strcmp(p,"PREFIX") == 0)
		UnionInputPrefix();
	else if (strcmp(p,"VALUE") == 0)
		UnionInputDesKeyValue();
	else if (strcmp(p,"CHECKVALUE") == 0)
		UnionInputDesKeyCheckValue();
	else if (strcmp(p,"ACTIVEDATE") == 0)
		UnionInputDesKeyActiveDate();
	else if (strcmp(p,"PASSIVEDATE") == 0)
		UnionInputDesKeyPassiveDate();
	else if (strcmp(p,"MAX") == 0)
		UnionInputMaxOwnerLimit();
	else if (strcmp(p,"MIN") == 0)
		UnionInputMinOwnerLimit();
	else if (strcmp(p,"STEP") == 0)
		UnionInputOwnerStep();
	else if (strcmp(p,"CHARS") == 0)
		UnionInputOwnerChars();
	else if (strcmp(p,"HELP") == 0)
		EditHelp();
	else 
	{
		if ((strlen(choice) == 2) && isdigit(choice[0]) && isdigit(choice[1]))
		{
			switch (atoi(p))
			{
				case	31:
					EditComponentNum();
					break;
				case	1:
					EditInsert();
					goto waitLoop;
				case	2:
					EditDelete();
					goto waitLoop;
				case	3:
					EditUpdate();
					goto waitLoop;
				case	4:
					EditPrint();
					goto waitLoop;
				case	5:
					EditGenerate();
					goto waitLoop;
				case	11:
					UnionInputApp();
					break;
				case	12:
					UnionInputName();
					break;
				case	13:
					UnionInputDesKeyType();
					break;
				case	14:
					UnionInputDesKeyLength();
					break;
				case	21:
					UnionInputPrefix();
					break;
				case	22:
					UnionInputSuffix();
					break;
				case	23:
					UnionInputMinOwnerLimit();
					break;
				case	24:
					UnionInputMaxOwnerLimit();
					break;
				case	25:
					UnionInputOwnerChars();
					break;
				case	26:
					UnionInputOwnerStep();
					break;
				case	41:
					EditInputPar();
					break;
#ifdef _GangdongDevelopmentBank_
				case	42:
					GDBInputBankNo();
					break;
				case	43:
					GDBInputMachinceNo();
#endif
			}
		}
	}
	goto loopEdit;
}

int EditComponentNum()
{
	char *p;
	
	if (UnionIsQuit(p=UnionInput("ComponentNum::")))
		return(0);
	gcomponentNum = atoi(p);
	return(0);
}

int UnionInputDesKeyLength()
{
	char *param;
	
loopUnionInput:
	if (UnionIsQuit(param = UnionInput("Des Key Length:(64/128/192/quit)")))
		return(0);
	if (strcmp(param,"64") == 0)
		gdesKey.length = con64BitsDesKey;
	else if (strcmp(param,"128") == 0)
		gdesKey.length = con128BitsDesKey;
	else if (strcmp(param,"192") == 0)
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
	memset(gdesKey.value,0,sizeof(gdesKey.value));
	switch (gdesKey.length)
	{
		case	con64BitsDesKey:
			param = UnionInput("64 Bits DesKey,quit to exit\n++++++++++++++++\n");
			break;
		case	con128BitsDesKey:
			param = UnionInput("128 Bits DesKey,quit to exit\n++++++++++++++++++++++++++++++++\n");
			break;
		case	con192BitsDesKey:
			param = UnionInput("192 Bits DesKey,quit to exit\n++++++++++++++++++++++++++++++++++++++++++++++++\n");
			break;
		default:
			printf("Invalid length of the key! [%d]\n",gdesKey.length);
			return(-1);
	}
	
	if (UnionIsQuit(param))
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

	memset(gdesKey.checkValue,0,sizeof(gdesKey.checkValue));
loopUnionInput:
	if (UnionIsQuit(param = UnionInput("the checkvalue\n++++++++++++++++\n")))
		return(0);
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
	if (UnionIsQuit(param = UnionInput("Key Type,quit to exit\n")))
		return(0);
	else if (strcmp(param,"ZPK") == 0)
		gdesKey.type = conZPK;
	else if (strcmp(param,"ZAK") == 0)
		gdesKey.type = conZAK;
	else if (strcmp(param,"ZMK") == 0)
		gdesKey.type = conZMK;
	else if (strcmp(param,"TMK") == 0)
		gdesKey.type = conTMK;
	else if (strcmp(param,"TPK") == 0)
		gdesKey.type = conTPK;
	else if (strcmp(param,"TAK") == 0)
		gdesKey.type = conTAK;
	else if (strcmp(param,"PVK") == 0)
		gdesKey.type = conPVK;
	else if (strcmp(param,"CVK") == 0)
		gdesKey.type = conCVK;
	else if (strcmp(param,"ZEK") == 0)
		gdesKey.type = conZEK;
	else if (strcmp(param,"WWK") == 0)
		gdesKey.type = conWWK;
	else if (strcmp(param,"BDK") == 0)
		gdesKey.type = conBDK;
	else if (strcmp(param,"USER") == 0)
		gdesKey.type = conSelfDefinedKey;
	else
	{
		printf("Invalid KeyType [%s]!\n",param);
		goto loopUnionInput;
	}
	
	memset(gname,0,sizeof(gname));
	UnionGetNameOfDesKey(gdesKey.type,gname);
	UnionToLowerCase(gname);
		
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
	if (UnionIsQuit(param = UnionInput("effective date(YYYYMMDD)::")))
		return(0);
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
	if (UnionIsQuit(param = UnionInput("passive date(YYYYMMDD)::")))
		return(0);
	if (strlen(param) != 8)
	{
		printf("Invalid date!\n");
		goto loopUnionInput;
	}
	strcpy(gdesKey.passiveDate,param);
	return(0);
}

int UnionInputMaxOwnerLimit()
{
	char	*param;

loopUnionInput:
	if (UnionIsQuit(param = UnionInput("max owner limit::")))
		return(0);
	gmax = atol(param);
	return(0);
}

int UnionInputMinOwnerLimit()
{
	char	*param;

loopUnionInput:
	if (UnionIsQuit(param = UnionInput("min owner limit::")))
		return(0);
	gmin = atol(param);
	return(0);
}

int UnionInputOwnerStep()
{
	char	*param;

loopUnionInput:
	if (UnionIsQuit(param = UnionInput("owner step::")))
		return(0);
	gstep = atol(param);
	return(0);
}

int UnionInputOwnerChars()
{
	char	*param;

loopUnionInput:
	if (UnionIsQuit(param = UnionInput("owner chars::")))
		return(0);
	gchars = atol(param);
	return(0);
}

int UnionInputApp()
{
	char	*p;
	strcpy(gapp,p=UnionInput("app::"));
	if (UnionIsQuit(p))
		memset(gapp,0,sizeof(gapp));
	return(0);
}

int UnionInputName()
{
	char	*p;
	strcpy(gname,p=UnionInput("name::"));
	if (UnionIsQuit(p))
		memset(gname,0,sizeof(gname));
	return(0);
}

int UnionInputSuffix()
{
	char	*p;
	strcpy(gsuffix,p=UnionInput("owner suffix::"));
	if (UnionIsQuit(p))
		memset(gsuffix,0,sizeof(gsuffix));
	return(0);
}

int UnionInputPrefix()
{
	char	*p;
	strcpy(gprefix,p=UnionInput("owner prefix::"));
	if (UnionIsQuit(p))
		memset(gprefix,0,sizeof(gprefix));
	return(0);
}


int EditFormFullKeyName(long middlePartOfOwner,char *fullKeyName)
{
	char	tmpKeyName[200];
	
	if ((strlen(gsuffix) == 0) && (strlen(gprefix) == 0))
	{
		strcpy(fullKeyName,gfullKeyName);
		return(0);
	}
	switch (gchars)
	{
		case	1:
			sprintf(tmpKeyName,"%s.%s%ld%s.%s",gapp,gprefix,middlePartOfOwner,gsuffix,gname);
			break;
		case	2:
			sprintf(tmpKeyName,"%s.%s%02ld%s.%s",gapp,gprefix,middlePartOfOwner,gsuffix,gname);
			break;
		case	3:
			sprintf(tmpKeyName,"%s.%s%03ld%s.%s",gapp,gprefix,middlePartOfOwner,gsuffix,gname);
			break;
		case	4:
			sprintf(tmpKeyName,"%s.%s%04ld%s.%s",gapp,gprefix,middlePartOfOwner,gsuffix,gname);
			break;
		case	5:
			sprintf(tmpKeyName,"%s.%s%05ld%s.%s",gapp,gprefix,middlePartOfOwner,gsuffix,gname);
			break;
		case	6:
			sprintf(tmpKeyName,"%s.%s%06ld%s.%s",gapp,gprefix,middlePartOfOwner,gsuffix,gname);
			break;
		case	7:
			sprintf(tmpKeyName,"%s.%s%07ld%s.%s",gapp,gprefix,middlePartOfOwner,gsuffix,gname);
			break;
		case	8:
			sprintf(tmpKeyName,"%s.%s%08ld%s.%s",gapp,gprefix,middlePartOfOwner,gsuffix,gname);
			break;
		case	9:
			sprintf(tmpKeyName,"%s.%s%09ld%s.%s",gapp,gprefix,middlePartOfOwner,gsuffix,gname);
			break;
		case	10:
			sprintf(tmpKeyName,"%s.%s%010ld%s.%s",gapp,gprefix,middlePartOfOwner,gsuffix,gname);
			break;
		case	11:
			sprintf(tmpKeyName,"%s.%s%011ld%s.%s",gapp,gprefix,middlePartOfOwner,gsuffix,gname);
			break;
		case	12:
			sprintf(tmpKeyName,"%s.%s%012ld%s.%s",gapp,gprefix,middlePartOfOwner,gsuffix,gname);
			break;
		case	13:
			sprintf(tmpKeyName,"%s.%s%013ld%s.%s",gapp,gprefix,middlePartOfOwner,gsuffix,gname);
			break;
		case	14:
			sprintf(tmpKeyName,"%s.%s%014ld%s.%s",gapp,gprefix,middlePartOfOwner,gsuffix,gname);
			break;
		case	15:
			sprintf(tmpKeyName,"%s.%s%015ld%s.%s",gapp,gprefix,middlePartOfOwner,gsuffix,gname);
			break;
		case	16:
			sprintf(tmpKeyName,"%s.%s%016ld%s.%s",gapp,gprefix,middlePartOfOwner,gsuffix,gname);
			break;
		case	17:
			sprintf(tmpKeyName,"%s.%s%017ld%s.%s",gapp,gprefix,middlePartOfOwner,gsuffix,gname);
			break;
		case	18:
			sprintf(tmpKeyName,"%s.%s%018ld%s.%s",gapp,gprefix,middlePartOfOwner,gsuffix,gname);
			break;
		case	19:
			sprintf(tmpKeyName,"%s.%s%019ld%s.%s",gapp,gprefix,middlePartOfOwner,gsuffix,gname);
			break;
		case	20:
			sprintf(tmpKeyName,"%s.%s%020ld%s.%s",gapp,gprefix,middlePartOfOwner,gsuffix,gname);
			break;
		default:
			printf("invalid gchars [%ld]\n",gchars);
			return(-1);
	}
	if (UnionIsValidDesKeyFullName(tmpKeyName))
	{
		strcpy(fullKeyName,tmpKeyName);
		return(0);
	}
	else
	{
		printf("invalid fullKeyName [%s]\n",tmpKeyName);
		return(-1);
	}
}

int EditInsert()
{
	long	i;
	int	ret;
	long	successNum=0,failNum=0;
	char	nameOfType[100];
	
	system("clear");
	//UnionPrintDesKey(&gdesKey);
	memset(gdesKey.fullName,0,sizeof(gdesKey.fullName));
	EditFormFullKeyName(gmin,gdesKey.fullName);
	printf("First Key Name:: [%s]\n",gdesKey.fullName);
	memset(gdesKey.fullName,0,sizeof(gdesKey.fullName));
	EditFormFullKeyName(gmax,gdesKey.fullName);
	printf("Last Key Name::  [%s]\n",gdesKey.fullName);
	printf("key length::     [%d]\n",(1+gdesKey.length)*16);
	memset(nameOfType,0,sizeof(nameOfType));
	UnionGetNameOfDesKeyType(gdesKey.type,nameOfType);
	printf("key type::       [%s]\n",nameOfType);
	printf("Step between two keys = [%ld]\n",gstep);
	printf("Middle part of owner occupies [%ld] char positions\n",gchars);
	
	if (!UnionConfirm("Are you sure to batch insert these keys?"))
		return(0);
		
	for (i = gmin; i <= gmax; i = i + gstep)
	{
		if (EditFormFullKeyName(i,gdesKey.fullName) < 0)
		{
			printf("EditFormFullKeyName Error for [%ld]\n",i);
			continue;
		}
		if ((ret = UnionInsertDesKeyIntoKeyDB(&gdesKey)) < 0)
		{
			printf("insert [%s] failure!\n",gdesKey.fullName);
			failNum++;
		}
		else
		{
			printf("insert [%s] ok!\n",gdesKey.fullName);
			successNum++;
		}
	}
	printf("successNum = [%ld]\n",successNum);
	printf("failNum    = [%ld]\n",failNum);
	return(0);
}

int EditDelete()
{
	long	i;
	int	ret;
	long	successNum=0,failNum=0;
	
	system("clear");
	memset(gdesKey.fullName,0,sizeof(gdesKey.fullName));
	EditFormFullKeyName(gmin,gdesKey.fullName);
	printf("First Key Name:: [%s]\n",gdesKey.fullName);
	memset(gdesKey.fullName,0,sizeof(gdesKey.fullName));
	EditFormFullKeyName(gmax,gdesKey.fullName);
	printf("Last Key Name::  [%s]\n",gdesKey.fullName);
	printf("Step between two keys = [%ld]\n",gstep);
	printf("Middle part of owner occupies [%ld] char positions\n",gchars);
	
	if (!UnionConfirm("Are you sure to batch delete these keys?"))
		return(0);
		
	for (i = gmin; i <= gmax; i = i + gstep)
	{
		if (EditFormFullKeyName(i,gdesKey.fullName) < 0)
		{
			printf("EditFormFullKeyName Error for [%ld]\n",i);
			continue;
		}
		if ((ret = UnionDeleteDesKeyFromKeyDB(gdesKey.fullName)) < 0)
		{
			printf("delete [%s] failure!\n",gdesKey.fullName);
			failNum++;
		}
		else
		{
			printf("delete [%s] ok!\n",gdesKey.fullName);
			successNum++;
		}
	}
	printf("successNum = [%ld]\n",successNum);
	printf("failNum    = [%ld]\n",failNum);
	return(0);
}

int EditUpdate()
{
	long	i;
	int	ret;
	long	successNum=0,failNum=0;
	char	nameOfType[100];
		
	system("clear");
	//UnionPrintDesKey(&gdesKey);
	memset(gdesKey.fullName,0,sizeof(gdesKey.fullName));
	EditFormFullKeyName(gmin,gdesKey.fullName);
	printf("First Key Name:: [%s]\n",gdesKey.fullName);
	memset(gdesKey.fullName,0,sizeof(gdesKey.fullName));
	EditFormFullKeyName(gmax,gdesKey.fullName);
	printf("Last Key Name::  [%s]\n",gdesKey.fullName);
	printf("key length::     [%d]\n",(gdesKey.length+1)*16);
	memset(nameOfType,0,sizeof(nameOfType));
	UnionGetNameOfDesKeyType(gdesKey.type,nameOfType);
	printf("key type::       [%s]\n",nameOfType);
	printf("Step between two keys = [%ld]\n",gstep);
	printf("Middle part of owner occupies [%ld] char positions\n",gchars);
	
	if (!UnionConfirm("Are you sure to batch update these keys?"))
		return(0);
		
	for (i = gmin; i <= gmax; i = i + gstep)
	{
		if (EditFormFullKeyName(i,gdesKey.fullName) < 0)
		{
			printf("EditFormFullKeyName Error for [%ld]\n",i);
			continue;
		}
		if ((ret = UnionUpdateDesKeyInKeyDB(&gdesKey)) < 0)
		{
			printf("update [%s] failure!\n",gdesKey.fullName);
			failNum++;
		}
		else
		{
			printf("update [%s] ok!\n",gdesKey.fullName);
			successNum++;
		}
	}
	printf("successNum = [%ld]\n",successNum);
	printf("failNum    = [%ld]\n",failNum);
	return(0);
}

int EditPrint()
{
	long	i;
	int	ret;
	long	successNum=0,failNum=0;
	char	nameOfType[40];
		
	system("clear");
	memset(gdesKey.fullName,0,sizeof(gdesKey.fullName));
	EditFormFullKeyName(gmin,gdesKey.fullName);
	printf("First Key Name:: [%s]\n",gdesKey.fullName);
	memset(gdesKey.fullName,0,sizeof(gdesKey.fullName));
	EditFormFullKeyName(gmax,gdesKey.fullName);
	printf("Last Key Name::  [%s]\n",gdesKey.fullName);
	printf("Step between two keys = [%ld]\n",gstep);
	printf("Middle part of owner occupies [%ld] char positions\n",gchars);
	
	if (!UnionConfirm("Are you sure to batch print these keys?"))
		return(0);
		
	for (i = gmin; i <= gmax; i = i + gstep)
	{
		if (EditFormFullKeyName(i,gdesKey.fullName) < 0)
		{
			printf("EditFormFullKeyName Error for [%ld]\n",i);
			continue;
		}
		if ((ret = UnionReadDesKeyFromKeyDBAnyway(&gdesKey)) < 0)
		{
			printf("read [%s] failure!\n",gdesKey.fullName);
			failNum++;
		}
		else
		{
			memset(nameOfType,0,sizeof(nameOfType));
			UnionGetNameOfDesKeyType(gdesKey.type,nameOfType);
			printf("%22s %3s %03d %s %s\n",gdesKey.fullName,nameOfType,(gdesKey.length + 1)*64,
				gdesKey.value,gdesKey.checkValue);
			successNum++;
		}
	}
	printf("successNum = [%ld]\n",successNum);
	printf("failNum    = [%ld]\n",failNum);
	return(0);
}

int EditGenerate()
{
	long	i;
	int	ret;
	long	successNum=0,failNum=0;
	char	nameOfType[40];
	int	parNum;
	int	keyExists = 0;
	
	system("clear");
	memset(gdesKey.fullName,0,sizeof(gdesKey.fullName));
	EditFormFullKeyName(gmin,gdesKey.fullName);
	printf("First Key Name:: [%s]\n",gdesKey.fullName);
	memset(gdesKey.fullName,0,sizeof(gdesKey.fullName));
	EditFormFullKeyName(gmax,gdesKey.fullName);
	printf("Last Key Name::  [%s]\n",gdesKey.fullName);
	printf("key length::     [%d]\n",(gdesKey.length+1)*16);
	memset(nameOfType,0,sizeof(nameOfType));
	UnionGetNameOfDesKeyType(gdesKey.type,nameOfType);
	printf("key type::       [%s]\n",nameOfType);
#ifdef _GangdongDevelopmentBank_
	printf("bank no::        [%s]\n",gparam[0]);
	printf("machinceno::     [%s]\n",gparam[1]);
	parNum = 2;
#else
	for (i = 0; i < 20; i++)
	{
		if (strlen(gparam[i]) > 0)
			printf("par%02d::        [%s]\n",i,gparam[i]);
		else
		{
			parNum = i;
			break;
		}
	}
#endif	
	printf("Step between two keys = [%ld]\n",gstep);
	printf("Middle part of owner occupies [%ld] char positions\n",gchars);
	printf("Each Key has [%d] Components\n",gcomponentNum);
	if (!UnionConfirm("Are you sure to batch generate these keys use Hsm [%s]?",gsjl06.staticAttr.ipAddr))
		return(0);
	
	for (i = gmin; i <= gmax; i = i + gstep)
	{
		if (EditFormFullKeyName(i,gdesKey.fullName) < 0)
		{
			printf("EditFormFullKeyName Error for [%ld]\n",i);
			continue;
		}
		if ((ret = UnionReadDesKeyFromKeyDBAnyway(&gdesKey)) < 0)
		{
			printf("key [%s] not exists!\n",gdesKey.fullName);
			keyExists = 0;
		}
		else
			keyExists = 1;
		printf("generate [%s] ...\n",gdesKey.fullName);
		if ((ret = UnionGenerateKeyComponent(&gsjl06,&gdesKey,gcomponentNum,parNum,gparam)) < 0)
		{
			printf("generate [%s] failure!\n",gdesKey.fullName);
			failNum++;
			continue;
		}
		else
			printf("generate [%s] ok!\n",gdesKey.fullName,gdesKey.value);
		if (keyExists)
		{
			if ((ret = UnionUpdateDesKeyInKeyDB(&gdesKey)) < 0)
			{
				printf("update [%s] failure!\n",gdesKey.fullName);
				failNum++;
				continue;
			}
		}
		else
		{
			printf("update [%s] ok!\n",gdesKey.fullName);
			successNum++;
		}
	}
	
	printf("successNum = [%ld]\n",successNum);
	printf("failNum    = [%ld]\n",failNum);
	return(0);
}

int EditInputPar()
{
	int	index;
	char	*p;
loopInput:
	if (UnionIsQuit(p = UnionInput("Input par index::")))
		return(0);
	if (((index = atoi(p)) < 0) || (index >= 20))
	{
		printf("par index must be of 0~%d\n",20);
		goto loopInput;
	}
loopInputPar:
	p = UnionInput("Input par[%02d]::\n",index);
	if (strlen(p) > sizeof(gparam[index]))
	{
		printf("par too long!");
		goto loopInputPar;
	}
	strcpy(gparam[index],p);
	if (UnionIsQuit(p))
	{
		memset(gparam[index],0,sizeof(gparam[index]));
		return(0);
	}
	goto loopInput;
}

#ifdef _GangdongDevelopmentBank_
int GDBInputBankNo()
{
	char	*p;
loopInputPar:
	p = UnionInput("Input bankno::\n");
	if (strlen(p) > sizeof(gparam[0]))
	{
		printf("bankno too long!");
		goto loopInputPar;
	}
	strcpy(gparam[0],p);
	if (UnionIsQuit(p))
	{
		memset(gparam[0],0,sizeof(gparam[0]));
		return(0);
	}
	return(0);
}

int GDBInputMachinceNo()
{
	char	*p;
loopInputPar:
	p = UnionInput("Input machineno::\n");
	if (strlen(p) > sizeof(gparam[1]))
	{
		printf("machineno too long!");
		goto loopInputPar;
	}
	strcpy(gparam[1],p);
	if (UnionIsQuit(p))
	{
		memset(gparam[1],0,sizeof(gparam[1]));
		return(0);
	}
	return(0);
}
#endif
