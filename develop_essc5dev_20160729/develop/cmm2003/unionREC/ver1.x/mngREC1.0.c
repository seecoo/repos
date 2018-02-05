//	Author:		Wolfgang Wang
//	Date:		2001/08/29
//	Version:	2.0

#define _UnionLogMDL_3_x_
#define _UnionTask_3_x_

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "UnionLog.h"
#include "unionCommand.h"
#include "unionVersion.h"
#include "UnionTask.h"

#include "unionREC.h"

PUnionTaskInstance	ptaskInstance = NULL;
TUnionRECVarType	gvarType = conString;

int UnionHelp()
{
	printf("Usage:: %s command\n",UnionGetApplicationName());
	printf(" Where command as:\n");
	printf("  -reload 	reload the running enviroment configuration\n");
	printf("  -print 	print the running enviroment configuration\n");
	printf("  -mirror	mirror the running enviroment configuration\n");
	printf("  -read varName ...\n");
	printf("  -RELOADANYWAY reload any way!\n");
	printf("  -update varName value\n");
	printf("  -testread	test the read functions\n");
	printf("  -testupdate	test the update functions\n");
	return(0);
}

int UnionTaskActionBeforeExit()
{
	UnionDisconnectREC();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	//return(exit(0));
	exit(0);
}

int TestReadFunctions()
{
	char	*p;
	char	varName[80];
	
loop:
	p = UnionInput("Input type/varName/exit/quit::");
	
	memset(varName,0,sizeof(varName));
	strcpy(varName,p);
	if (UnionIsQuit(p))
		return(UnionTaskActionBeforeExit());
	
	if (strcasecmp(p,"INT") == 0)
	{
		gvarType = conInt;
		goto loop;
	}
	if (strcasecmp(p,"SHORT") == 0)
	{
		gvarType = conShort;
		goto loop;
	}
	if (strcasecmp(p,"LONG") == 0)
	{
		gvarType = conLong;
		goto loop;
	}
	if (strcasecmp(p,"CHAR") == 0)
	{
		gvarType = conChar;
		goto loop;
	}
	if (strcasecmp(p,"STRING") == 0)
	{
		gvarType = conString;
		goto loop;
	}
	if (strcasecmp(p,"DOUBLE") == 0)
	{
		gvarType = conDouble;
		goto loop;
	}
	
	switch (gvarType)
	{
		case	conInt:
			printf("[%s] = [%d]\n",varName,UnionReadIntTypeRECVar(varName));
			break;
		case	conShort:
			printf("[%s] = [%d]\n",varName,UnionReadShortTypeRECVar(varName));
			break;
		case	conLong:
			printf("[%s] = [%ld]\n",varName,UnionReadLongTypeRECVar(varName));
			break;
		case	conChar:
			printf("[%s] = [%c]\n",varName,UnionReadCharTypeRECVar(varName));
			break;
		case	conString:
			printf("[%s] = [%s]\n",varName,UnionReadStringTypeRECVar(varName));
			break;
		case	conDouble:
			printf("[%s] = [%-12.2f]\n",varName,UnionReadDoubleTypeRECVar(varName));
			break;
		default:
			printf("Set varType first!\n");
			break;
	}
	goto loop;
}

int TestUpdateFunctions()
{
	char	*p;
	char	varName[80];
	PUnionRECVar	pvar;
	int	ret;
loop:
	p = UnionInput("Input varName/exit/quit::");
	
	memset(varName,0,sizeof(varName));
	strcpy(varName,p);
	if (UnionIsQuit(p))
		return(UnionTaskActionBeforeExit());
	if ((pvar = UnionReadRECVar(varName)) == NULL)
	{
		printf("UnionReadRECVar [%s] error!\n",varName);
		goto loop;
	}
	UnionPrintRECVarToFile(pvar,stdout);
	if (!UnionConfirm("Are you sure of update this RECVar?"))
		goto loop;
	p = UnionInput("Input New Value for [%s]::",varName);
	switch (pvar->type)
	{
		case	conInt:
			pvar->value.intValue = atoi(p);
			ret = UnionUpdateRECVar(varName,(unsigned char *)(&(pvar->value.intValue)));
			break;
		case	conShort:
			pvar->value.shortValue = atoi(p);
			ret = UnionUpdateRECVar(varName,(unsigned char *)(&(pvar->value.shortValue)));
			break;
		case	conLong:
			pvar->value.longValue = atol(p);
			ret = UnionUpdateRECVar(varName,(unsigned char *)(&(pvar->value.longValue)));
			break;
		case	conChar:
			pvar->value.charValue = *p;
			ret = UnionUpdateRECVar(varName,(unsigned char *)(&(pvar->value.charValue)));
			break;
		case	conString:
			ret = UnionUpdateRECVar(varName,(unsigned char *)p);
			break;
		case	conDouble:
			pvar->value.doubleValue = atof(p);
			ret = UnionUpdateRECVar(varName,(unsigned char *)(&(pvar->value.doubleValue)));
			break;
		default:
			printf("Invalid varType first!\n");
			goto loop;
	}
	if (ret < 0)
		printf("UnionUpdateRECVar [%s] Error! ret = [%d]\n",varName,ret);
	else
		printf("UnionUpdateRECVar [%s] OK!\n",varName);
	
	goto loop;
}

int Read(int argc,char *argv[])
{
	char	*p;
	char	varName[80];
	int	i;
	PUnionRECVar	pvar;
	
	for (i = 0; i < argc; i++)
	{
		if ((pvar = UnionReadRECVar(argv[i])) == NULL)
			printf("UnionReadRECVar [%s] Error!\n",argv[i]);
		else
			UnionPrintRECVarToFile(pvar,stdout);
	}
	if (argc > 0)
		return(UnionTaskActionBeforeExit());
	
loop:
	p = UnionInput("\nInput varName/exit/quit::");
	
	memset(varName,0,sizeof(varName));
	strcpy(varName,p);
	if (UnionIsQuit(p))
		return(UnionTaskActionBeforeExit());
	
	if ((pvar = UnionReadRECVar(varName)) == NULL)
		printf("UnionReadRECVar [%s] Error!\n",varName);
	else
		UnionPrintRECVarToFile(pvar,stdout);

	goto loop;
}

int Update(int argc,char *argv[])
{
	char	*p = NULL;
	char	varName[80];
	PUnionRECVar	pvar;
	int	ret;
	char	value[256];
	
	if (argc <= 0)
		goto loop;
		
	memset(varName,0,sizeof(varName));
	if (argc > 0)
		strcpy(varName,argv[0]);
	if ((pvar = UnionReadRECVar(varName)) == NULL)
	{
		printf("UnionReadRECVar [%s] Error!\n",varName);
		return(UnionTaskActionBeforeExit());
	}
	if (argc > 1)
		strcpy(value,argv[1]);
	else
		strcpy(value,p = UnionInput("\nInput Value for [%s]::",varName));
	goto update;
		
loop:
	memset(varName,0,sizeof(varName));
	strcpy(varName,p = UnionInput("\nInput varName/exit/quit::"));
	if (UnionIsQuit(p))
		return(UnionTaskActionBeforeExit());
	if ((pvar = UnionReadRECVar(varName)) == NULL)
	{
		printf("UnionReadRECVar [%s] Error!\n",varName);
		goto loop;
	}
	UnionPrintRECVarToFile(pvar,stdout);
	strcpy(value,p = UnionInput("\nInput Value for [%s]::",varName));
update:
	if (UnionIsQuit(p))
		return(UnionTaskActionBeforeExit());
	if (!UnionConfirm("Are you sure of update RECVar [%s] with new value [%s]?",varName,value))
		goto loop;
	switch (pvar->type)
	{
		case	conInt:
			pvar->value.intValue = atoi(value);
			ret = UnionUpdateRECVar(varName,(unsigned char *)(&(pvar->value.intValue)));
			break;
		case	conShort:
			pvar->value.shortValue = atoi(value);
			ret = UnionUpdateRECVar(varName,(unsigned char *)(&(pvar->value.shortValue)));
			break;
		case	conLong:
			pvar->value.longValue = atol(value);
			ret = UnionUpdateRECVar(varName,(unsigned char *)(&(pvar->value.longValue)));
			break;
		case	conChar:
			pvar->value.charValue = value[0];
			ret = UnionUpdateRECVar(varName,(unsigned char *)(&(pvar->value.charValue)));
			break;
		case	conString:
			ret = UnionUpdateRECVar(varName,(unsigned char *)value);
			break;
		case	conDouble:
			pvar->value.doubleValue = atof(value);
			ret = UnionUpdateRECVar(varName,(unsigned char *)(&(pvar->value.doubleValue)));
			break;
		default:
			printf("Invalid varType!\n");
			goto loop;
	}
	if (ret < 0)
		printf("UnionUpdateRECVar [%s] Error! ret = [%d]\n",varName,ret);
	else
		printf("UnionUpdateRECVar [%s] OK!\n",varName);
	if (argc > 0)
		return(UnionTaskActionBeforeExit());
	goto loop;
}

int main(int argc,char *argv[])
{
	int	ret;

	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		return(UnionHelp());
	
	/*	
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"mngREC")) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}
	*/
	
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
	
	if ((ret = UnionConnectREC()) < 0)
	{
		UnionPrintf("in mngREC:: UnionConnectREC Failure!\n");
		return(UnionTaskActionBeforeExit());
	}
	
	if (strcasecmp(argv[1],"-RELOAD") == 0)
	{
		if (!UnionConfirm("确定加载共享内存[参数表]吗?"))
			return(-1);
		if ((ret = UnionReloadREC()) < 0)
			printf("***** %-30s Error!\n","加载共享内存[参数表]");
		else
			printf("***** %-30s OK!\n","加载共享内存[参数表]");
		return(UnionTaskActionBeforeExit());
	}
	if (strcasecmp(argv[1],"-RELOADANYWAY") == 0)
	{
		if ((ret = UnionReloadREC()) < 0)
			printf("***** %-30s Error!\n","加载共享内存[参数表]");
		else
			printf("***** %-30s OK!\n","加载共享内存[参数表]");
		return(UnionTaskActionBeforeExit());
	}

	if (strcasecmp(argv[1],"-PRINT") == 0)
	{
		if ((ret = UnionPrintRECToFile(stdout)) < 0)
			UnionPrintf("in mngREC:: UnionPrintRECToFile Error! ret = [%d]\n",ret);
		return(UnionTaskActionBeforeExit());
	}

	if (strcasecmp(argv[1],"-MIRROR") == 0)
	{
		if (!UnionConfirm("Are you sure of mirror running enviroment configuration?"))
			return(-1);
		if ((ret = UnionMirrorREC()) < 0)
			UnionPrintf("in mngREC:: UnionMirrorREC Error! ret = [%d]\n",ret);
		else
			UnionPrintf("in mngREC:: UnionMirrorREC OK!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (strcasecmp(argv[1],"-TESTREAD") == 0)
		return(TestReadFunctions());

	if (strcasecmp(argv[1],"-TESTUPDATE") == 0)
		return(TestUpdateFunctions());

	if (strcasecmp(argv[1],"-READ") == 0)
		return(Read(argc-2,&argv[2]));
	
	if (strcasecmp(argv[1],"-UPDATE") == 0)
		return(Update(argc-2,&argv[2]));
		
	UnionHelp();
	return(UnionTaskActionBeforeExit());
}

