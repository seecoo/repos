// Copyright:	Union Tech.
// Author:	������
// Date:	2013/1/8
// Version:	1.0

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <dlfcn.h>
#include <time.h>

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif
#define _realBaseDB_2_x_

#include "UnionTask.h"
#include "unionVersion.h"
#include "unionMDLID.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "unionRealDBCommon.h"
#include "unionCommand.h"
#include "UnionStr.h"
#include "unionDataSync.h"
#include "clusterSyn.h"
#include "unionREC.h"

#define defSleepSeconds		60

int	gunionLocalClusterNo;
char	gunionLocalHostName[64+1];

PUnionTaskInstance	ptaskInstance = NULL;

int (*UnionFunDLDaemonTask)(char *parameter);

typedef struct
{
	int	isScript;
	char	dlOrScriptName[128+1];
	char	dlName[256+1];
	char	taskID[40+1];
	char	cliTaskName[128];
	char	*parameter;
}TUnionDaemonTask;
typedef TUnionDaemonTask	*PUnionDaemonTask;

#define DAEMONTASK_NUM	128

int UnionTaskActionBeforeExit()
{
        alarm(0);

        //UnionCloseDatabase();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

int UnionHelp()
{
	printf("Usage:: %s\n",UnionGetApplicationName());
	return(0);
}

// ����ػ������Ƿ�����ִ��
// taskType = 1 ��������taskType = 0 �������
int UnionCheckDaemonTaskIsRunning(int taskType,char *taskID,char *cliTaskName,char *lastRunStartTime,char *lastRunningTime,char *lastRunEndTime,char *lastRunHost,int strategy)
{
	int	ret;
	int	len = 0;
	long	intervalTime = 0;
	int	runFlag = 0;
	char	sql[512+1];
	
	if (taskType == 0)
	{
		if (UnionExistsTaskClassOfName(cliTaskName) > 0)
			return(1);
		else
			return(0);
	}
	
	// ����ִ��
	if ((strlen(lastRunStartTime) > 0) && (strlen(lastRunEndTime) == 0))
	{
		if (strcmp(lastRunHost,gunionLocalHostName) == 0)	// ����ִ��
		{
			// ����ӽ����Ƿ����
			if (UnionExistsTaskOfName(cliTaskName) > 0)
			{
				runFlag = 1;
				len = sprintf(sql,"update daemonTask set lastRunningTime = '%s' where taskID = '%s'",
					UnionGetCurrentFullSystemDateTime(),taskID);
			}
			else
			{	
				runFlag = 0;
				len = sprintf(sql,"update daemonTask set lastRunResult = 0,lastRunEndTime = '%s', remark = '%s' where taskID = '%s'",
					UnionGetCurrentFullSystemDateTime(),"�����쳣�˳���δִ�����",taskID);
			}
			
			
			sql[len] = 0;
			if ((ret = UnionExecRealDBSql(sql)) < 0)
			{
				UnionUserErrLog("in UnionCheckDaemonTaskIsRunning:: UnionExecRealDBSql ret = [%d] sql = [%s]!\n",ret,sql);
			}
			return(runFlag);

		}
		else	// �Ǳ���ִ��
		{
			if (strategy)	// �Ƕ�ʱ����
			{
				UnionCalcuSecondsPassedAfterSpecTime(lastRunningTime,&intervalTime);
				if (intervalTime > 60 * 60)	// ����һ��Сʱ
					return(0);
				else
					return(1);
			}
		}
	}
	return(0);
}

int main(int argc,char *argv[])
{
	int	i;
	int	ret;
	int	len;
	int	onceFlag = 0;
	long	iRunTime;
	long	intervalTime;
	int	runResult = 0;
	int	childPid;
	int	sleepSeconds = 0;
	int	num;
	int	strategy;
	int	taskType;
	TUnionDaemonTask	daemonTask[DAEMONTASK_NUM];
	char	tmpBuf[128+1];
	char	lastRunHost[64+1];
	char	lastRunStartTime[14+1];
	char	lastRunningTime[14+1];
	char	lastRunEndTime[14+1];
	char	strRunTime[14+1];
	char	sql[1024+1];
	char	systemCmd[512+1];
	char	remark[128+1];
	void	*handle = NULL;
	char	*parameter = NULL;
	int	taskNum = 0;
	int	numOfCluster = 0;
	int	isDelayed = 1;
	int	delayTime = 0;
	int	randNum = 3;

	UnionSetApplicationName(argv[0]);
	
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);
	
	if (UnionExistsAnotherTaskOfName(UnionGetApplicationName()))
	{
		UnionPrintf("another task of name [%s] already exists!\n",UnionGetApplicationName());
		return(0);
	}
		
	if (UnionCreateProcess() > 0)
		return(0);

	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		UnionPrintf("in %s:: UnionCreateTaskInstance Error!\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}
	
	// ������Ⱥ���
	gunionLocalClusterNo = UnionGetClusterNoOfLocalHost();

	// ��ȡ��Ⱥ����
	if ((numOfCluster = UnionReadIntTypeRECVar("numOfCluster")) < 0)
	{
		UnionLog("in %s:: UnionReadIntTypeRECVar[numOfCluster]!\n",UnionGetApplicationName());
		numOfCluster = 1;	
	}
	
	// ������
	memset(tmpBuf,0,sizeof(tmpBuf));
	gethostname(tmpBuf,sizeof(tmpBuf));
	snprintf(gunionLocalHostName,sizeof(gunionLocalHostName),"��Ⱥ���[%d],������[%s]",gunionLocalClusterNo,tmpBuf);
	
	UnionSetDataSyncIsWaitResult();
	while(1)
	{
		isDelayed = 1;
		if (sleepSeconds > 0)
		{
			sleep(sleepSeconds);
			if (sleepSeconds < defSleepSeconds)
				sleepSeconds = defSleepSeconds;
		}
		else
			sleepSeconds = defSleepSeconds;
delayedOperate:
		snprintf(sql,sizeof(sql),"select * from daemonTask where enabled = 1");
		if ((num = UnionSelectRealDBRecord(sql,0,0)) < 0)
		{
			UnionUserErrLog("in %s:: UnionSelectRealDBRecord[%s]!\n",UnionGetApplicationName(),sql);
			break;
		}

		taskNum = 0;
		for (i = 0; i < num; i++)
		{
			UnionLocateXMLPackage("detail", i + 1);
			
			if ((ret = UnionReadXMLPackageValue("taskID",daemonTask[taskNum].taskID, sizeof(daemonTask[taskNum].taskID))) < 0)
			{
				UnionUserErrLog("in %s:: UnionReadXMLPackageValue[%s]!\n",UnionGetApplicationName(),"taskID");
				break;
			}
			daemonTask[taskNum].taskID[ret] = 0;

			snprintf(daemonTask[taskNum].cliTaskName,sizeof(daemonTask[taskNum].cliTaskName),"%s %s",UnionGetApplicationName(),daemonTask[taskNum].taskID);
			
			if ((ret = UnionReadXMLPackageValue("dlOrScriptName",daemonTask[taskNum].dlOrScriptName, sizeof(daemonTask[taskNum].dlOrScriptName))) < 0)
			{
				UnionUserErrLog("in %s:: UnionReadXMLPackageValue[%s]!\n",UnionGetApplicationName(),"dlOrScriptName");
				break;
			}
			daemonTask[taskNum].dlOrScriptName[ret] = 0;
			
			// ��.so��.sh����ִ��
			len = strlen(daemonTask[taskNum].dlOrScriptName);
			if ((strcmp(daemonTask[taskNum].dlOrScriptName + len - 3,".so") == 0) || (strstr(daemonTask[taskNum].dlOrScriptName,".so ") != NULL))
			{
				daemonTask[taskNum].isScript = 0;
				if ((parameter = strstr(daemonTask[taskNum].dlOrScriptName,".so ")) != NULL)
				{
					*(parameter + 3) = 0;
					parameter += 4;
				}
				daemonTask[taskNum].parameter = parameter;
				
				if (getenv("UNIONDLDIR") != NULL)
					ret = snprintf(daemonTask[taskNum].dlName,sizeof(daemonTask[taskNum].dlName),"%s/%s",getenv("UNIONDLDIR"),daemonTask[taskNum].dlOrScriptName);
				else
					ret = snprintf(daemonTask[taskNum].dlName,sizeof(daemonTask[taskNum].dlName),"%s/bin/%s",getenv("UNIONREC"),daemonTask[taskNum].dlOrScriptName);

				daemonTask[taskNum].dlName[ret] = 0;
				
			}
			else if ((strcmp(daemonTask[taskNum].dlOrScriptName + len - 3,".sh") == 0) || (strstr(daemonTask[taskNum].dlOrScriptName,".sh ") != NULL))
				daemonTask[taskNum].isScript = 1;
			else
			{
				UnionUserErrLog("in %s:: dlOrScriptName[%s] is not .so or .sh!\n",UnionGetApplicationName(),daemonTask[taskNum].dlOrScriptName);
				continue;
			}
			
			// ��������
			if ((ret = UnionReadXMLPackageValue("taskType",tmpBuf, sizeof(tmpBuf))) < 0)
			{
				UnionUserErrLog("in %s:: UnionReadXMLPackageValue[%s]!\n",UnionGetApplicationName(),"taskType");
				break;
			}
			taskType = atoi(tmpBuf);
			
			// ����
			if ((ret = UnionReadXMLPackageValue("strategy",tmpBuf, sizeof(tmpBuf))) < 0)
			{
				UnionUserErrLog("in %s:: UnionReadXMLPackageValue[%s]!\n",UnionGetApplicationName(),"tmpBuf");
				break;
			}
			strategy = atoi(tmpBuf);
			
			// ���ִ����ʼʱ��
			if ((ret = UnionReadXMLPackageValue("lastRunStartTime",lastRunStartTime, sizeof(lastRunStartTime))) < 0)
			{
				UnionUserErrLog("in %s:: UnionReadXMLPackageValue[%s]!\n",UnionGetApplicationName(),"lastRunStartTime");
				break;
			}
			
			// ���ִ����ʱ��
			if ((ret = UnionReadXMLPackageValue("lastRunningTime",lastRunningTime, sizeof(lastRunningTime))) < 0)
			{
				UnionUserErrLog("in %s:: UnionReadXMLPackageValue[%s]!\n",UnionGetApplicationName(),"lastRunningTime");
				break;
			}

			// ���ִ�н���ʱ��
			if ((ret = UnionReadXMLPackageValue("lastRunEndTime",lastRunEndTime, sizeof(lastRunEndTime))) < 0)
			{
				UnionUserErrLog("in %s:: UnionReadXMLPackageValue[%s]!\n",UnionGetApplicationName(),"lastRunEndTime");
				break;
			}
			
			// ���ִ������
			if ((ret = UnionReadXMLPackageValue("lastRunHost",lastRunHost, sizeof(lastRunHost))) < 0)
			{
				UnionUserErrLog("in %s:: UnionReadXMLPackageValue[%s]!\n",UnionGetApplicationName(),"lastRunHost");
				break;
			}
			
			// ��ʱִ��һ��
			if ((ret = UnionReadXMLPackageValue("onceFlag",tmpBuf, sizeof(tmpBuf))) < 0)
			{
				UnionUserErrLog("in %s:: UnionReadXMLPackageValue[%s]!\n",UnionGetApplicationName(),"lastRunHost");
				break;
			}
			onceFlag = atoi(tmpBuf);

			if (onceFlag == 0)	// �Ǽ�ʱִ��
			{
				if ((ret = UnionReadXMLPackageValue("runTime",tmpBuf, sizeof(tmpBuf))) < 0)
				{
					UnionUserErrLog("in %s:: UnionReadXMLPackageValue[%s]!\n",UnionGetApplicationName(),"runTime");
					break;
				}
				
				// ��������Ƿ�����ִ��
				if (UnionCheckDaemonTaskIsRunning(taskType,daemonTask[taskNum].taskID,daemonTask[taskNum].cliTaskName,lastRunStartTime,lastRunningTime,lastRunEndTime,lastRunHost,strategy))
				{
					UnionLog("in %s:: taskID[%s] is running!\n",UnionGetApplicationName(),daemonTask[taskNum].taskID);
					continue;
				}
	
				if (strategy == 0)	//	��ʱִ��
				{
					strcpy(strRunTime,tmpBuf);
					if (strlen(strRunTime) != 4)
					{
						UnionUserErrLog("in %s:: runTime[%s] not HHMM,error!\n",UnionGetApplicationName(),strRunTime);
						continue;
					}

					if (strncmp(UnionGetCurrentFullSystemDate(),lastRunStartTime,8) == 0)	// ������ͬ�������Ѿ�ִ��
					{
						continue;
					}
					
					if (strncmp(UnionGetCurrentSystemTime(),strRunTime,4) < 0)	// δ����ָ��ʱ���
					{
						continue;
					}

					// �������ּ�Ⱥ���Ϊ1�ķ�����ȥִ������
					// Ϊ���ⵥ�������ʱ��,�ػ�����ͬʱ��ѯ���ݿ⣬�Ӷ�ͬʱȥִ�е�������,�ʰѼ�Ⱥ�Ŵ���1�������ӳٺ���ִ�С�
					// ���ݼ�Ⱥ������������ӳ�10����
					if ((numOfCluster > 1) && (gunionLocalClusterNo > 1) && (taskType == 1) && (isDelayed == 1))
					{
						srand((unsigned)time(NULL));
						randNum = 3 + (int)(3.0 * rand()/(RAND_MAX));	
						delayTime = 600 * randNum * (gunionLocalClusterNo - 1);
						UnionLog("in %s:: numOfCluster[%d] localClusterNo[%d] delayed time[%d]s!\n",UnionGetApplicationName(),numOfCluster,gunionLocalClusterNo,delayTime);
						sleep(delayTime);
						isDelayed = 0;
						goto delayedOperate;
					}
				}
				else
				{
					// �����ʱ��
					if ((iRunTime = atol(tmpBuf)) > defSleepSeconds)
					{
						UnionCalcuSecondsPassedAfterSpecTime(lastRunEndTime,&intervalTime);
						if (intervalTime < iRunTime)
							continue;
					}
				}
			}
			
			len = sprintf(sql,"update daemonTask set runTimes = runTimes + 1, lastRunStartTime = '%s' ,lastRunningTime = '%s', lastRunEndTime = null,lastRunHost = '%s',onceFlag = 0 where taskID = '%s'",UnionGetCurrentFullSystemDateTime(),UnionGetCurrentFullSystemDateTime(),gunionLocalHostName,daemonTask[taskNum].taskID);
			sql[len] = 0;
			if ((ret = UnionExecRealDBSql(sql)) < 0)
			{
				UnionUserErrLog("in %s:: UnionExecRealDBSql[%s]!\n",UnionGetApplicationName(),sql);
				return(ret);
			}
			taskNum++;			

		}

		// modified 20151012 û��������ʱ���ر����ݿ�
		//UnionCloseDatabase();
		if(taskNum > 0)
		{
			UnionCloseDatabase();
		}
		// end of modification 20151012

		for (i = 0; i < taskNum; i++)
		{	
			if ((childPid = UnionCreateProcess()) == 0)	// ���ӽ���
				goto callChildProcess;
			else
				continue;
		}
	}
	
	return(UnionTaskActionBeforeExit());

	// �ӽ���ִ�����´���
callChildProcess:
	runResult = 0;
	remark[0] = 0;

	// modified 2015-06-09
	//if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",daemonTask[i].cliTaskName)) == NULL)
	if ((ptaskInstance = UnionCreateTaskInstanceForSub(ptaskInstance->pclass->logFileName, UnionTaskActionBeforeExit, "%s", daemonTask[i].cliTaskName)) == NULL)
	{
		UnionUserErrLog("in %s:: UnionCreateTaskInstance Error!\n",daemonTask[i].cliTaskName);
		snprintf(remark,sizeof(remark),"��������ʵ��[%s]ʧ��",daemonTask[i].cliTaskName);
		goto updateDaemonTask;
	}

	
	if (daemonTask[i].isScript)	// �ű�
	{
		snprintf(systemCmd,sizeof(systemCmd),daemonTask[i].dlOrScriptName);
		system(systemCmd);
		runResult = 1;
		goto updateDaemonTask;
	}
	
	if ((handle = dlopen(daemonTask[i].dlName, RTLD_NOW)) == NULL)
	{
		UnionUserErrLog("in %s:: dlopen[%s] error[%s]!\n",daemonTask[i].cliTaskName,daemonTask[i].dlName,dlerror());
		snprintf(remark,sizeof(remark),"dlopen[%s]ʧ��",daemonTask[i].dlName);
		goto updateDaemonTask;
	}
	
	if ((UnionFunDLDaemonTask = dlsym(handle,"UnionDealDaemonTask")) == NULL)
	{
		UnionUserErrLog("in %s:: dlsym[%s] error[%s]!\n",daemonTask[i].cliTaskName,"UnionDealDaemonTask",dlerror());
		snprintf(remark,sizeof(remark),"dlsym����[%s][%s]ʧ��",daemonTask[i].dlName,"UnionDealDaemonTask");
		dlclose(handle);
		goto updateDaemonTask;
	}
	
	if ((ret = (*UnionFunDLDaemonTask)(daemonTask[i].parameter)) < 0)
	{
		UnionUserErrLog("in %s:: UnionFunDLDaemonTask ret = [%d]!\n",daemonTask[i].cliTaskName,ret);
		snprintf(remark,sizeof(remark),"�ػ�����[%s]ִ��ʧ�ܣ�������[%d]",daemonTask[i].dlName,ret);
		dlclose(handle);
		goto updateDaemonTask;
	}
	dlclose(handle);
	
	runResult = 1;

updateDaemonTask:
	
	if (runResult)
		snprintf(sql,sizeof(sql),"update daemonTask set lastRunResult = %d,lastRunningTime = null,lastRunEndTime = '%s', remark = null where taskID = '%s'",runResult,UnionGetCurrentFullSystemDateTime(),daemonTask[i].taskID);
	else
		snprintf(sql,sizeof(sql),"update daemonTask set lastRunResult = %d,lastRunningTime = null,lastRunEndTime = '%s', remark = '%s' where taskID = '%s'",runResult,UnionGetCurrentFullSystemDateTime(),remark,daemonTask[i].taskID);
		
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in %s:: UnionExecRealDBSql[%s]!\n",daemonTask[i].cliTaskName,sql);
		return(UnionTaskActionBeforeExit());
	}
	return(UnionTaskActionBeforeExit());
}
