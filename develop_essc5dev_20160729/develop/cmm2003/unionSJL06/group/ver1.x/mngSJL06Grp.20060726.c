//	Author:	Wolfgang Wang
//	Date:	2001/10/22
// 	Version	1.0
// 2010-11-30, huangbaoxin,	增加-clear Index功能, 清除共享内存中对应的索引的加密机数据
//				增加-change Index IpAddr, 手动修改内存中指定索引的ip地址

#define _UnionTask_3_x_
#define _UnionLogMDL_3_x_       


#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "sjl06.h"
#include "sjl06Grp.h"
#include "unionCommand.h"
#include "UnionLog.h"

#include "UnionTask.h"

PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
	UnionDisconnectWorkingSJL06MDL();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	//return(exit(0));
	exit(0);
}

int UnionHelp()
{
	printf("Usage:: %s Command Arguments\n",UnionGetApplicationName());
	printf("Where:: Command as follow:\n");
	printf(" -add IPAddr ...\n");
	printf("	add hsms into the hsm-working-spool.\n");
	printf(" -delete IPAddr ....\n");
	printf("	Delete hsms from the the hsm-working-spool.\n");
	printf(" -backup IPAddr ....\n");
	printf("	Set a hsms backup in hsm-working-spool.\n");
	printf(" -abnormal IPAddr ....\n");
	printf("	Set hsms abnormal in hsm-working-spool.\n");
	printf(" -online IPAddr ....\n");
	printf("	Set hsm online in hsm-working-spool.\n");
	printf(" -reset IPAddr ....\n");
	printf("	Reset normal hsm command times.\n");
	printf(" -resetall\n");
	printf("	Reset normal hsm command times for all hsms.\n");
	printf(" -print IPAddr ...\n");
	printf("	Print status of hsms in hsm-working-spool.\n");
	printf(" -printall\n");
	printf("	Print status of all Hsms in hsm-working-spool.\n");
	printf(" -check seconds\n");
	printf("	Check status of Hsms in hsm-working-spool.\n");
	printf(" -checkgroup hsmGrpID Seconds\n");
	printf("	Check status of Hsm Group in hsm-working-spool.\n");
	printf(" -killhsmtask Seconds\n");
	printf("	kill hsmtask for abnormal hsms in hsm-working-spool.\n");
	printf(" -reload\n");
	printf("	reload all hsms registered in $UNIONETC/HsmCfg/hsmGrp.conf into hsm-working-spool.\n");
	printf(" -clear Index...\n");
	printf("	clear hsm info by index in share memory.\n");
	printf(" -change Index IpAddr\n");
	printf("	change IpAddr by index in share memory.\n");	
	
	return(0);
}

int main(int argc,char **argv)
{
	int	ret;

	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		return(UnionHelp());
		
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());

	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if ((ret = UnionConnectWorkingSJL06MDL()) < 0)
	{
		printf("UnionConnectWorkingSJL06MDL Error! ret = [%d]\n",ret);
		return(ret);
	}
		
	if (strcasecmp(argv[1],"-ADD") == 0)
		return(AddHsm(argc-2,&argv[2]));
		
	if (strcasecmp(argv[1],"-DELETE") == 0)
		return(DeleteHsm(argc-2,&argv[2]));
	
	if (strcasecmp(argv[1],"-ONLINE") == 0)
		return(SetHsmOnline(argc-2,&argv[2]));
	
	if (strcasecmp(argv[1],"-RESET") == 0)
		return(ResetHsm(argc-2,&argv[2]));
	
	if (strcasecmp(argv[1],"-RESETALL") == 0)
		return(ResetAllHsm());
	
	if (strcasecmp(argv[1],"-BACKUP") == 0)
		return(SetHsmColdBackup(argc-2,&argv[2]));
	
	if (strcasecmp(argv[1],"-ABNORMAL") == 0)
		return(SetHsmAbnormal(argc-2,&argv[2]));

	if (strcasecmp(argv[1],"-PRINT") == 0)
		return(PrintHsm(argc-2,&argv[2]));
	
	if (strcasecmp(argv[1],"-PRINTALL") == 0)
		return(PrintAllHsm());

	if (strcasecmp(argv[1],"-CHECK") == 0)
	{
		if (argc < 2)
			return(CheckHsmGrp(60));
		else
			return(CheckHsmGrp(atoi(argv[2])));
	}
	if (strcasecmp(argv[1],"-KILLHSMTASK") == 0)
	{
		if (argc < 2)
			return(KillHsmTask(60));
		else
			return(KillHsmTask(atoi(argv[2])));
	}
	if (strcasecmp(argv[1],"-CHECKGROUP") == 0)
	{
		return(KillTaskWhenNoWorkingHsmInGroup(argc-2,&argv[2]));
	}
	if (strcasecmp(argv[1],"-RELOADANYWAY") == 0)
	{
		if ((ret = UnionAutoLoadSJL06IntoSJL06MDL()) < 0)
			printf("UnionAutoLoadSJL06IntoSJL06MDL failure!\n");
		else
			printf("UnionAutoLoadSJL06IntoSJL06MDL OK!\n");
		return(UnionTaskActionBeforeExit());
	}
	if (strcasecmp(argv[1],"-RELOAD") == 0)
	{
		if (!UnionConfirm("Are you sure of reload all hsms?"))
			return(UnionTaskActionBeforeExit());
		if ((ret = UnionAutoLoadSJL06IntoSJL06MDL()) < 0)
			printf("UnionAutoLoadSJL06IntoSJL06MDL failure!\n");
		else
			printf("UnionAutoLoadSJL06IntoSJL06MDL OK!\n");
		return(UnionTaskActionBeforeExit());
	}
	// Add By HuangBaoxin, 2010-11-3
	if (strcasecmp(argv[1],"-CLEAR") == 0)
		return(ClearHsm(argc-2, &argv[2]));
	if (strcasecmp(argv[1],"-CHANGE") == 0)
	{
		if (argc != 4)
			return UnionHelp();
			
		return(ChangeIpAddr(argv[2], argv[3]));
	}
	// Add End
		
	UnionHelp();

	return(UnionTaskActionBeforeExit());
}

int AddHsm(int argc,char **argv)
{
	int		ret;
	int		i;
	char		*p;

	for (i = 0; i < argc ; i++)
	{
		if ((ret = UnionAddWorkingSJL06(argv[i])) < 0)
			printf("UnionAddWorkingSJL06 [%s] Error! ret = [%d]\n",argv[i],ret);
		else
			printf("UnionAddWorkingSJL06 [%s] OK!\n",argv[i]);
	}
	if (argc > 0)
		return(UnionTaskActionBeforeExit());

looping:
	if (UnionIsQuit(p=UnionInput("Add>Input IPAddr or Exit/Quit to Eixt>")))
		return(UnionTaskActionBeforeExit());
	if ((ret = UnionAddWorkingSJL06(p)) < 0)
		printf("UnionAddWorkingSJL06 [%s] Error! ret = [%d]\n",p,ret);
	else
		printf("UnionAddWorkingSJL06 [%s] OK!\n",p);
	goto looping;	
}

int DeleteHsm(int argc,char **argv)
{
	int	i;
	int	ret;
	char	*p;

	for (i = 0; i < argc; i++)
	{
		if ((ret = UnionDeleteWorkingSJL06(argv[i])) < 0)
			printf("UnionDeleteWorkingSJL06 [%s] Error! ret = [%d]\n",argv[i],ret);
		else
			printf("UnionDeleteWorkingSJL06 [%s] OK!\n",argv[i]);
	}
	
	if (argc > 0)
		return(UnionTaskActionBeforeExit());

looping:
	if (UnionIsQuit(p=UnionInput("Delete>Input IPAddr or Exit/Quit to Eixt>")))
		return(UnionTaskActionBeforeExit());
	if ((ret = UnionDeleteWorkingSJL06(p)) < 0)
		printf("UnionDeleteWorkingSJL06 [%s] Error! ret = [%d]\n",p,ret);
	else
		printf("UnionDeleteWorkingSJL06 [%s] OK!\n",p);
	goto looping;
}

int SetHsmOnline(int argc,char **argv)
{	
	PUnionSJL06	pSJL06;
	int		ret;
	int		i;
	char		*p;
	
	for (i = 0; i < argc ; i++)
	{
		if ((pSJL06 = UnionFindWorkingSJL06(argv[i])) == NULL)	// 还没在working-spool注册
		{
			printf("UnionFindWorkingSJL06 [%s] Error!\n",argv[i]);
			continue;
		}
		if ((ret = UnionSetWorkingSJL06Online(pSJL06)) < 0)
			printf("UnionSetWorkingSJL06Online [%s] Error! ret = [%d]\n",argv[i],ret);
		else
			printf("UnionSetWorkingSJL06Online [%s] OK!\n",argv[i]);
	}
	
	if (argc > 0)
		return(UnionTaskActionBeforeExit());

looping:
	if (UnionIsQuit(p=UnionInput("SetOnline>Input IPAddr or Exit/Quit to Eixt>")))
		return(UnionTaskActionBeforeExit());
	if ((pSJL06 = UnionFindWorkingSJL06(p)) == NULL)	// 还没在working-spool注册
	{
		printf("UnionFindWorkingSJL06 [%s] Error!\n",p);
		goto looping;
	}
	if ((ret = UnionSetWorkingSJL06Online(pSJL06)) < 0)
		printf("UnionSetWorkingSJL06Online [%s] Error! ret = [%d]\n",p,ret);
	else
		printf("UnionSetWorkingSJL06Online [%s] OK!\n",p);
	goto looping;
}

int SetHsmAbnormal(int argc,char **argv)
{	
	PUnionSJL06	pSJL06;
	int		ret;
	int		i;
	char		*p;
	
	for (i = 0; i < argc ; i++)
	{
		if ((pSJL06 = UnionFindWorkingSJL06(argv[i])) == NULL)	// 还没在working-spool注册
		{
			printf("UnionFindWorkingSJL06 [%s] Error!\n",argv[i]);
			continue;
		}
		if ((ret = UnionSetWorkingSJL06Abnormal(pSJL06)) < 0)
			printf("UnionSetWorkingSJL06Abnormal [%s] Error! ret = [%d]\n",argv[i],ret);
		else
			printf("UnionSetWorkingSJL06Abnormal [%s] OK!\n",argv[i]);
	}
	
	if (argc > 0)
		return(UnionTaskActionBeforeExit());

looping:
	if (UnionIsQuit(p=UnionInput("SetAbnormal>Input IPAddr or Exit/Quit to Eixt>")))
		return(UnionTaskActionBeforeExit());
	if ((pSJL06 = UnionFindWorkingSJL06(p)) == NULL)	// 还没在working-spool注册
	{
		printf("UnionFindWorkingSJL06 [%s] Error!\n",p);
		goto looping;
	}
	if ((ret = UnionSetWorkingSJL06Abnormal(pSJL06)) < 0)
		printf("UnionSetWorkingSJL06Abnormal [%s] Error! ret = [%d]\n",p,ret);
	else
		printf("UnionSetWorkingSJL06Abnormal [%s] OK!\n",p);
	goto looping;
}

int SetHsmColdBackup(int argc,char **argv)
{	
	PUnionSJL06	pSJL06;
	int		ret;
	int		i;
	char		*p;

	for (i = 0; i < argc ; i++)
	{
		if ((pSJL06 = UnionFindWorkingSJL06(argv[i])) == NULL)	// 还没在working-spool注册
		{
			printf("UnionFindWorkingSJL06 [%s] Error!\n",argv[i]);
			continue;
		}
		if ((ret = UnionSetWorkingSJL06ColdBackup(pSJL06)) < 0)
			printf("UnionSetWorkingSJL06ColdBackup [%s] Error! ret = [%d]\n",argv[i],ret);
		else
			printf("UnionSetWorkingSJL06ColdBackup [%s] OK!\n",argv[i]);
	}
	
	if (argc > 0)
		return(UnionTaskActionBeforeExit());

looping:
	if (UnionIsQuit(p=UnionInput("SetBackup>Input IPAddr or Exit/Quit to Eixt>")))
		return(UnionTaskActionBeforeExit());
	if ((pSJL06 = UnionFindWorkingSJL06(p)) == NULL)	// 还没在working-spool注册
	{
		printf("UnionFindWorkingSJL06 [%s] Error!\n",p);
		goto looping;
	}
	if ((ret = UnionSetWorkingSJL06ColdBackup(pSJL06)) < 0)
		printf("UnionSetWorkingSJL06ColdBackup [%s] Error! ret = [%d]\n",p,ret);
	else
		printf("UnionSetWorkingSJL06ColdBackup [%s] OK!\n",p);
	goto looping;
}

int PrintHsm(int argc,char **argv)
{	
	int		ret;
	int		i;
	char		*p;
	
	for (i = 0; i < argc ; i++)
	{
		ret = UnionPrintWorkingSJL06(argv[i]);
	}
	
	if (argc > 0)
		return(UnionTaskActionBeforeExit());

looping:
	if (UnionIsQuit(p=UnionInput("Print>Input IPAddr or Exit/Quit to Eixt>")))
		return(UnionTaskActionBeforeExit());
	if ((ret = UnionPrintWorkingSJL06(p)) < 0)
		printf("UnionPrintWorkingSJL06 [%s] Error! ret = [%d]\n",p,ret);
	else
		printf("UnionPrintWorkingSJL06 [%s] OK!\n",p);
	goto looping;	
}

int PrintAllHsm()
{	
	return(UnionPrintAllWorkingSJL06());
}

int CheckHsmGrp(int seconds)
{
	if (seconds <= 0)
		seconds = 60;
	
	if (UnionCreateProcess() > 0)
		return(UnionTaskActionBeforeExit());

	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s check %d",UnionGetApplicationName(),seconds)) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	for (;;)
	{
#ifdef _checkHsmCmdGrp_
		UnionCheckStatusOfWorkingSJL06WithCmdGrp();
#else
		UnionCheckStatusOfWorkingSJL06();
#endif
		sleep(seconds);
	}
	
}

int ResetHsm(int argc,char **argv)
{	
	int		ret;
	int		i;
	char		*p;
	
	for (i = 0; i < argc ; i++)
	{
		if ((ret = UnionResetWorkingSJL06CmdTimes(argv[i])) < 0)
			printf("UnionResetWorkingSJL06CmdTimes [%s] Error! ret = [%d]\n",argv[i],ret);
		else
			printf("UnionResetWorkingSJL06CmdTimes [%s] OK!\n",argv[i]);
	}
	
	if (argc > 0)
		return(UnionTaskActionBeforeExit());

looping:
	if (UnionIsQuit(p=UnionInput("ResetHsm>Input IPAddr or Exit/Quit to Eixt>")))
		return(UnionTaskActionBeforeExit());
	if ((ret = UnionResetWorkingSJL06CmdTimes(p)) < 0)
		printf("UnionResetWorkingSJL06CmdTimes [%s] Error! ret = [%d]\n",p,ret);
	else
		printf("UnionResetWorkingSJL06CmdTimes [%s] OK!\n",p);
	goto looping;
}

int ResetAllHsm()
{
	int	ret;
	
	if ((ret = UnionResetAllWorkingSJL06CmdTimes()) < 0)
		printf("UnionResetAllWorkingSJL06CmdTimes Error! ret = [%d]\n",ret);
	else
		printf("UnionResetAllWorkingSJL06CmdTimes OK!\n");
	
	return(UnionTaskActionBeforeExit());
}
	
int KillTaskWhenNoWorkingHsmInGroup(int argc,char *argv[])
{
	int	seconds = -1;
	char	hsmGrpID[3+1];

	if (UnionCreateProcess() > 0)
		return(UnionTaskActionBeforeExit());
	memset(hsmGrpID,0,sizeof(hsmGrpID));
	if (argc >= 1)
	{
		if (strlen(argv[0]) != 3)
		{
			printf("invalid hsmGrpID [%s]\n",argv[0]);
			return(UnionTaskActionBeforeExit());
		}
		strcpy(hsmGrpID,argv[0]);
	}
	else
		strcpy(hsmGrpID,"001");
	if (argc >= 2)
		seconds = atoi(argv[1]);
	if (seconds <= 0)
		seconds = 60;
			
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s check %s %d",UnionGetApplicationName(),hsmGrpID,seconds)) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	for (;;)
	{	
		if (UnionFindIdleWorkingSJL06(hsmGrpID) == NULL)
		{
			UnionUserErrLog("in UnionSynchTCPIPTaskServer:: KillTaskWhenNoWorkingHsmInGroup [%s]\n",hsmGrpID);
			UnionKillTaskInstanceByAlikeName(hsmGrpID);
		}
		sleep(seconds);
	}
	//return(UnionTaskActionBeforeExit());
}
int KillHsmTask(int seconds)
{
	if (seconds <= 0)
		seconds = 60;
	
	if (UnionCreateProcess() > 0)
		return(UnionTaskActionBeforeExit());

	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s killhsmtask %d",UnionGetApplicationName(),seconds)) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	for (;;)
	{
		UnionKillHsmTaskOfAllAbnormalHsm();
		sleep(seconds);
	}
	
}

// Add By HuangBaoxin, 2010-11-30
int ClearHsm(int argc,char **argv)
{	
	int		ret;
	int		i;
	
	for (i = 0; i < argc ; i++)
	{
		if (!UnionIsDigitStr(argv[i]))
		{
			printf("param [%s] is not a number\n", argv[i]);
			continue;
		}
		if ((ret = UnionClearWorkingSJL06(atoi(argv[i]))) < 0)
			printf("UnionClearWorkingSJL06 [%d] Error! ret = [%d]\n", atoi(argv[i]), ret);
		else
			printf("UnionClearWorkingSJL06 [%d] OK!\n",atoi(argv[i]));
	}
	
	return(UnionTaskActionBeforeExit());
}

int ChangeIpAddr(char *index, char *ipAddr)
{
	int	idx;
	int	ret;
	
	if (!UnionIsDigitStr(index))
	{
		printf("param [%s] is not a number\n", index);
		return(UnionTaskActionBeforeExit());
	}
	
	if ((ret = UnionChangeWorkingSJL06(atoi(index), ipAddr)) < 0)
		printf("UnionChangeWorkingSJL06 [%d] Error! ret = [%d]\n", atoi(index), ret);
	else
		printf("UnionChangeWorkingSJL06 [%d] OK!\n",atoi(index));

	return(UnionTaskActionBeforeExit());
}
// End Add By HuangBaoxin
