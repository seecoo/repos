//	Wolfgang Wang
//	2003/09/09
// 	Version	1.0

#define _UnionTask_3_x_

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "sjl06KeySpace.h"
#include "UnionStr.h"
#include "unionCommand.h"

char			hsmGrp[50];
char			name[50];
long			num;			

#include "UnionTask.h"

PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
	UnionDisconnectSJL06SharedKeySpace();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	//return(exit(0));
	exit(0);
}

int UnionHelp()
{
	printf("Usage:: %s Command\n",UnionGetApplicationName());
	printf(" Where Command\n");
	printf(" reloadall\n");
	printf(" reload hsmGrp spaceName\n");
	printf(" print\n");
	printf(" clear\n");
	printf(" unlockall\n");
	printf(" test functions\n");
	printf(" setsharedkeypos hsmGrp spaceName shareKeyPosNum\n");
	return(0);
}

int main(int argc,char **argv)
{
	int	ret;
	
	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		return(UnionHelp());
		
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"mngSharedKeySpace")) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
	
	if ((ret = UnionConnectSJL06SharedKeySpace()) < 0)
	{
		printf("UnionConnectSJL06SharedKeySpace Error! ret = [%d]\n",ret);
		return(UnionTaskActionBeforeExit());
	}
	
	if (strcasecmp(argv[1],"RELOADALL") == 0)
	{
		if ((ret = UnionReloadSJL06SharedKeyPosSpace()) < 0)
			printf("UnionReloadSJL06SharedKeyPosSpace Error! ret = [%d]\n",ret);
		else
			printf("UnionReloadSJL06SharedKeyPosSpace OK!\n");
		return(UnionTaskActionBeforeExit());
	}
	
	if (strcasecmp(argv[1],"CLEAR") == 0)
	{
		if ((ret = UnionClearSJL06SharedKeyPos()) < 0)
			printf("UnionClearSJL06SharedKeyPos Error! ret = [%d]\n",ret);
		else
			printf("UnionClearSJL06SharedKeyPos OK!\n");
		return(UnionTaskActionBeforeExit());
	}
	
	if (strcasecmp(argv[1],"UNLOCKALL") == 0)
	{
		if ((ret = UnionForceUnlockAllSJL06SharedKeyPos()) < 0)
			printf("UnionForceUnlockAllSJL06SharedKeyPos Error! ret = [%d]\n",ret);
		else
			printf("UnionForceUnlockAllSJL06SharedKeyPos OK!\n");
		return(UnionTaskActionBeforeExit());
	}
	
	if (strcasecmp(argv[1],"PRINT") == 0)
	{
		if ((ret = UnionPrintSJL06SharedKeySpaceToFile(stdout)) < 0)
			printf("UnionPrintSJL06SharedKeySpaceToFile Error! ret = [%d]\n",ret);
		return(UnionTaskActionBeforeExit());
	}
	
	if (strcasecmp(argv[1],"TEST") == 0)
		return(TestFun());

	if (strcasecmp(argv[1],"SETSHAREDKEYPOS") == 0)
		return(SetSharedKeyPos(argc-2,&argv[2]));
	
	if (strcasecmp(argv[1],"RELOAD") == 0)
		return(Reload(argc-2,&argv[2]));
	
	UnionHelp();
	
	return(UnionTaskActionBeforeExit());
}

//Create
int TestFun(int argc,char *argv[])
{
	char	*p;
	int	ret;
	long	keyPos;
	
	strcpy(hsmGrp,"001");
	strcpy(name,"JK");
	
loop:
	printf("\nhsmGrpID	[%s]\n",hsmGrp);
	printf("keySpaceName	[%s]\n",name);
	if (UnionIsQuit(p = UnionInput("TestFun>(hsmGrpID/JK/RSA/IC/Use/Unuse/exit)>")))
		return(UnionTaskActionBeforeExit());
	
	if (strcasecmp("USE",p) == 0)
		goto loopLockUse;
	if (strcasecmp("UNUSE",p) == 0)
		goto loopUnlock;
	if ((strcasecmp(p,conSJL06JKKeySpaceName) == 0) || (strcasecmp(p,conSJL06ICKeySpaceName) == 0) || (strcasecmp(p,conSJL06RSAKeySpaceName) == 0))
	{
		memset(name,0,sizeof(name));
		strcpy(name,p);
		goto loop;
	}
	memset(hsmGrp,0,sizeof(hsmGrp));
	strcpy(hsmGrp,p);
	goto loop;
		
loopLockUse:
	if ((keyPos = UnionLockUseAvailableSJL06SharedKeyPos(hsmGrp,name)) < 0)
		printf("UnionLockUseAvailableSJL06SharedKeyPos Error! ret = [%d]\n",ret);
	else
		printf("UnionLockUseAvailableSJL06SharedKeyPos OK! [%s.%s][%ld]\n\n",hsmGrp,name,keyPos);
	goto loop;
loopUnlock:
	if (UnionIsQuit(p=UnionInput("Input Key Pos to Unlock::")))
		return(UnionTaskActionBeforeExit());
	keyPos = atol(p);
	if ((ret = UnionFreeLockUsedSJL06SharedKeyPos(hsmGrp,name,keyPos)) < 0)
		printf("UnionFreeLockUsedSJL06SharedKeyPos Error! ret = [%d]\n",ret);
	else
		printf("UnionFreeLockUsedSJL06SharedKeyPos OK! [%s.%s][%ld]\n\n",hsmGrp,name,keyPos);
	goto loop;

}

int Reload(int argc,char *argv[])
{
	int	ret;
	int	i;
	char	*p;
	
	memset(hsmGrp,0,sizeof(hsmGrp));
	memset(name,0,sizeof(name));
	strcpy(name,"JK");
	strcpy(hsmGrp,"001");
	if (argc > 0)
	{
		strcpy(hsmGrp,argv[0]);
	}
	else
		goto loop;
	if (argc > 1)
	{
		strcpy(name,argv[1]);
	}
	else
	{
		if (UnionIsQuit(p = UnionInput("Input KeySpaceName (JK/IC/RSA/Exit)::")))
			return(UnionTaskActionBeforeExit());
		strcpy(name,p);
	}
	goto reload;

loop:
	printf("\nhsmGrp		[%s]\n",hsmGrp);
	printf("keySpaceName	[%s]\n",name);
	if (UnionIsQuit(p = UnionInput("\nReload (hsmGrp/JK/IC/RSA/Reload/Exit)>")))
		return(UnionTaskActionBeforeExit());
	if (strcasecmp(p,"RELOAD") == 0)
		goto reload;
	if ((strcasecmp(p,conSJL06JKKeySpaceName) == 0) || (strcasecmp(p,conSJL06ICKeySpaceName) == 0) || (strcasecmp(p,conSJL06RSAKeySpaceName) == 0))
	{
		memset(name,0,sizeof(name));
		strcpy(name,p);
	}
	else
	{
		memset(hsmGrp,0,sizeof(hsmGrp));
		strcpy(hsmGrp,p);
	}

reload:
	if (UnionConfirm("Reload shared key pos of [%s.%s]?",hsmGrp,name))
	{
		if ((ret = UnionLoadSharedKeyPosIntoSJL06ShareKeyPosSpace(hsmGrp,name)) < 0)
			printf("UnionLoadSharedKeyPosIntoSJL06ShareKeyPosSpace Error! ret = [%d]\n",ret);
		else
			printf("UnionLoadSharedKeyPosIntoSJL06ShareKeyPosSpace OK!\n");
	}
	if (argc > 0)
		return(UnionTaskActionBeforeExit());
	goto loop;
}

int SetSharedKeyPos(int argc,char *argv[])
{
	int	ret;
	int	i;
	char	*p;
	
	memset(hsmGrp,0,sizeof(hsmGrp));
	memset(name,0,sizeof(name));
	strcpy(name,"JK");
	strcpy(hsmGrp,"001");
	num = 2;
	if (argc > 0)
	{
		strcpy(hsmGrp,argv[0]);
	}
	else
		goto loop;
	if (argc > 1)
	{
		strcpy(name,argv[1]);
	}
	else
	{
		if (UnionIsQuit(p = UnionInput("Input KeySpaceName (JK/IC/RSA/Exit)::")))
			return(UnionTaskActionBeforeExit());
		strcpy(name,p);
	}
	if (argc > 2)
		num = atol(argv[2]);
	else
	{
		if (UnionIsQuit(p = UnionInput("Input shared key pos num (quit/exit to exit)::")))
			return(UnionTaskActionBeforeExit());
		num = atol(p);
	}
	
	goto init;

loop:
	printf("\nhsmGrp		[%s]\n",hsmGrp);
	printf("keySpaceName	[%s]\n",name);
	printf("sharedKeyNum	[%ld]\n\n",num);
	
	if (UnionIsQuit(p = UnionInput("\nInit (hsmGrp/JK/IC/RSA/Num/Init/Exit)>")))
		return(UnionTaskActionBeforeExit());
	if (strcasecmp(p,"INIT") == 0)
		goto init;
	if (strcasecmp(p,"NUM") == 0)
	{
		if (UnionIsQuit(p = UnionInput("Input shared key pos num (quit/exit to exit)::")))
			return(UnionTaskActionBeforeExit());
		num = atol(p);
	}
	else if ((strcasecmp(p,conSJL06JKKeySpaceName) == 0) || (strcasecmp(p,conSJL06ICKeySpaceName) == 0) || (strcasecmp(p,conSJL06RSAKeySpaceName) == 0))
	{
		memset(name,0,sizeof(name));
		strcpy(name,p);
	}
	else
	{
		memset(hsmGrp,0,sizeof(hsmGrp));
		strcpy(hsmGrp,p);
	}

init:
	if (UnionConfirm("Init [%ld] shared key pos for [%s.%s]?",num,hsmGrp,name))
	{
		if ((ret = UnionSetSJL06SharedKeyPosSpace(hsmGrp,name,num)) < 0)
			printf("UnionSetSJL06SharedKeyPosSpace Error! ret = [%d]\n",ret);
		else
			printf("UnionSetSJL06SharedKeyPosSpace OK!\n");
	}
	if (argc > 0)
		return(UnionTaskActionBeforeExit());
	goto loop;
}

