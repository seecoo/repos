//	Wolfgang Wang
//	2008/2/25

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include "unionCommand.h"
#include "unionErrCode.h"
#include "unionComplexDBRecord.h"
#include "unionComplexDBDataTransfer.h"
#include "unionGenSQLFromTBLDefList.h"
#include "unionRecFile.h"
#include "unionREC.h"

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_			// 选用3.x版本的Task模块
#endif
#include "UnionTask.h"

#include "UnionStr.h"
#include "UnionLog.h"

PUnionTaskInstance	ptaskInstance = NULL;
int			gunionResId = 44;			

int UnionTaskActionBeforeExit()
{
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

int UnionHelp()
{
	printf("Usage:: %s [command]\n",UnionGetApplicationName());
	printf("where command as follows:\n");
	printf(" NULL params::          export one table's datas from DB2 to txt\n");
	printf(" objectName             export one table's datas from DB2 to txt\n");
	printf(" -backupeveryday        -- backup key db every at time config by param \"timeOfBackupKeyDB\" in unionREC table\n");
	printf(" -atonceBackup          -- execute backup script at once\n");
	printf(" -help/-?               -- get more info\n");
	printf("\n");
	return(0);
}

int main(int argc,char *argv[])
{
	int	ret;
	int	index;
	
	
	UnionSetApplicationName(argv[0]);
	UnionSetIsOutPutInPutDataTools(1);
	
	if ((argc == 1) || (strstr(argv[1],"-") == NULL))
	{
		if ((ret = UnionConnectDatabase()) < 0)
		{
			printf("UnionConnectDatabase failure! ret = [%d]\n",ret);
			return(ret);
		}
		
		if (argc == 1)
			UnionOutPutRecDataFromDB(NULL);
		else if (argc > 1)
		{
			for (index = 1; index < argc; index++)
			{
				UnionOutPutRecDataFromDB(argv[index]);
			}
		}
		UnionCloseDatabase();
		return(0);
	}
	
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);
	
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,argv[0])) == NULL)
	{
		printf("in %s:: UnionCreateTaskInstance [%s] Error!\n",UnionGetApplicationName(),UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}
	
	if (strcasecmp(argv[1],"-BACKUPEVERYDAY") == 0)
		UnionBackupKeyDBEveryDay();
	else if (strcasecmp(argv[1],"-ATONCEBACKUP") == 0)
	{
		if (UnionConfirm("Are you backupKeyDB at once?"))
			UnionAtOnceBackupKeyDB();
	}
	else if ((strcasecmp(argv[1],"-HELP") == 0) || (strcasecmp(argv[1],"-?") == 0))
		UnionHelp();
	else
		UnionHelp();
	return(UnionTaskActionBeforeExit());
}

int UnionBackupKeyDBEveryDay()
{
	int			ret = -1;
	long			now = 0;
	long			timeSpec = 0;
	char			*ptr;
	char			tmpBuf[1024+1];
	int			keySetToday = 0;
	long			tmpInterval = 0;
	char			pCmd[512+1];
	char			backupBuf[1024+1];
	char			*p;
	char			fileName[512+1];
	char			targetFile1[256+1];
	char			targetFile2[256+1];
	char			targetFile3[256+1];
	char			today[8+1];
	char			pathBackup[512+1];
	
	if (UnionCreateProcess() > 0)
	{
		return(UnionTaskActionBeforeExit());
	}
	
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		printf("in UnionBackupKeyDBEveryDay:: UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	while(1)
	{
		// 获取每天备份密钥库的时间
		if (((ptr = UnionReadStringTypeRECVar("timeOfBackupKeyDB")) == NULL)
			|| (strlen(ptr) > 8))
			timeSpec = 3600;
		else
		{
			if (strlen(ptr) == 6)
				timeSpec = atol(ptr);
			else
			{
				strcpy(tmpBuf,ptr);
				timeSpec = atol(tmpBuf+6);
				tmpBuf[5] = 0;
				timeSpec = timeSpec + atol(tmpBuf+3) * 60;
				tmpBuf[2] = 0;
				timeSpec = timeSpec + atol(tmpBuf) * 3600;
			}
		}
		// 获取现在时间
		memset(tmpBuf,0,sizeof(tmpBuf));
		UnionGetSystemTime(tmpBuf);
		now = atol(tmpBuf+4);
		tmpBuf[4] = 0;
		now = now + atol(tmpBuf+2) * 60;
		tmpBuf[2] = 0;
		now = now + atol(tmpBuf) * 3600;
		/*UnionAuditLog("in UnionBackupKeyDBEveryDay:: timeAutoManageRemoteKeys = [%02ld:%02ld:%02ld] now = [%02ld:%02ld:%02ld] [%d]\n",
		timeSpec / 3600,timeSpec % 3600 / 60,timeSpec % 3600 % 60,
		now / 3600,now % 3600 / 60,now % 3600 % 60,
		keySetToday);
		*/
		// 判断是否是备份密钥库时间
		if (((tmpInterval = now - timeSpec) < 0) || (tmpInterval > 3600))
		{
			keySetToday = 0;
			goto sleepNow;
		}
		
		if (keySetToday)	// 当日已经备份密钥
			goto sleepNow;
		                
		// 调用系统指令运行密钥库备份shell脚本
		// 读取脚本在$(UNIONREC)/bin.x/下面的文件名
		if ((p = UnionReadStringTypeRECVar("fileNameOfBackupKeyDB")) == NULL)
		{
			UnionUserErrLog("in UnionBackupKeyDBEveryDay:: \"fileNameOfBackupKeyDB\" is not define on unionREC!\n");
			return(0);
		}
		memset(fileName,0,sizeof(fileName));
		strcpy(fileName,p);
		
		memset(pCmd,0,sizeof(pCmd));
		sprintf(pCmd,"sh %s/bin.x/%s >>%s/backupDB.log 2>&1",getenv("UNIONREC"),fileName,getenv("UNIONLOG"));
		system(pCmd);
		
		memset(targetFile1,0,sizeof(targetFile1));
		memset(targetFile2,0,sizeof(targetFile2));
		memset(targetFile2,0,sizeof(targetFile3));
		memset(today,0,sizeof(today));
		UnionGetFullSystemDate(today);

		if (getenv("DIROFBACKUPDBDATA") == NULL)
		{
			ret = -1;
			UnionUserErrLog("in UnionBackupKeyDBEveryDay:: getenv [DIROFBACKUPDBDATA] is not define on the .profile!ret=[%d]\n",ret);
		}
		else
		{
			sprintf(targetFile1,"%s/%2.2s/dbdata.backup.%s.tar.gz",getenv("DIROFBACKUPDBDATA"),today+4,today+4);
			sprintf(targetFile2,"%s/%2.2s/dbdata.backup.%s.tar.Z",getenv("DIROFBACKUPDBDATA"),today+4,today+4);
			sprintf(targetFile3,"%s/%2.2s/dbdata.backup.%s.tar",getenv("DIROFBACKUPDBDATA"),today+4,today+4);
			if ((access(targetFile1,F_OK) == 0) || (access(targetFile2,F_OK) == 0) || (access(targetFile3,F_OK) == 0))
			{
				ret = 0;
				UnionDebugLog("in UnionBackupKeyDBEveryDay:: backupDB is OK!\n");
			}
			else
			{
				ret = -1;
				UnionUserErrLog("in UnionBackupKeyDBEveryDay:: backup's file is not exist!ret = [%d]\n",ret);
			}
		}
		memset(backupBuf,0,sizeof(backupBuf));
		if (ret != 0)
		{
			sprintf(backupBuf,"applicationName=%s|dateTime=%s|pid=%d|responseCode=%d|remark=%s|",
				UnionGetApplicationName(),UnionGetCurrentFullSystemDateTime(),getpid(),ret,"备份密钥库失败");
		}
		else
		{
			sprintf(backupBuf,"applicationName=%s|dateTime=%s|pid=%d|responseCode=%d|remark=%s|",
				UnionGetApplicationName(),UnionGetCurrentFullSystemDateTime(),getpid(),ret,"备份密钥库成功");
		}
		keySetToday = 1;
		ret =   UnionSendTransInfoToMonitor(gunionResId, backupBuf, strlen(backupBuf));
		if (ret < 0)
		{       
			UnionUserErrLog("in UnionBackupKeyDBEveryDay::UnionSendTransInfoToMonitor!\n");
			continue;
		}
sleepNow:
		if ((tmpInterval = UnionReadLongTypeRECVar("intervalToCheckTimeOfBackupKeyDB")) <= 0)
			tmpInterval = 1800;
		sleep(tmpInterval);
	}

	return(ret);
}

int UnionAtOnceBackupKeyDB()
{
	char	pCmd[128+1];
	char	backupBuf[1024+1];
	int	ret = -1;
	int	ret1 = -1;
	char	*p;
	char	fileName[512+1];
	char	targetFile1[256+1];
	char	targetFile2[256+1];
	char	targetFile3[256+1];
	char	today[8+1];
	
	if ((p = UnionReadStringTypeRECVar("fileNameOfBackupKeyDB")) == NULL)
	{
		printf("in UnionBackupKeyDBEveryDay:: \"fileNameOfBackupKeyDB\" is not define on unionREC!\n");
		return(0);
	}
	memset(fileName,0,sizeof(fileName));
	strcpy(fileName,p);

	memset(pCmd,0,sizeof(pCmd));
	sprintf(pCmd,"sh %s/bin.x/%s >>%s/backupDB.log 2>&1",getenv("UNIONREC"),fileName,getenv("UNIONLOG"));
	system(pCmd);

	memset(targetFile1,0,sizeof(targetFile1));
	memset(targetFile2,0,sizeof(targetFile2));
	memset(targetFile2,0,sizeof(targetFile3));
	memset(today,0,sizeof(today));
	UnionGetFullSystemDate(today);
	if (getenv("DIROFBACKUPDBDATA") == NULL)
	{
		ret = -1;
		UnionUserErrLog("in UnionBackupKeyDBEveryDay:: getenv [DIROFBACKUPDBDATA] is not define on the .profile!ret=[%d]\n",ret);
	}
	else
	{
		sprintf(targetFile1,"%s/%2.2s/dbdata.backup.%s.tar.gz",getenv("DIROFBACKUPDBDATA"),today+4,today+4);
		sprintf(targetFile2,"%s/%2.2s/dbdata.backup.%s.tar.Z",getenv("DIROFBACKUPDBDATA"),today+4,today+4);
		sprintf(targetFile3,"%s/%2.2s/dbdata.backup.%s.tar",getenv("DIROFBACKUPDBDATA"),today+4,today+4);
		if ((access(targetFile1,F_OK) == 0) || (access(targetFile2,F_OK) == 0) || (access(targetFile3,F_OK) == 0))
		{
			ret = 0;
			UnionDebugLog("in UnionBackupKeyDBEveryDay:: backupDB is OK!\n");
		}
		else
		{
			ret = -1;
			UnionUserErrLog("in UnionBackupKeyDBEveryDay:: backup's file is not exist!ret = [%d]\n",ret);
		}
	}
		
	memset(backupBuf,0,sizeof(backupBuf));
	if (ret < 0)
	{
		sprintf(backupBuf,"applicationName=%s|dateTime=%s|pid=%d|responseCode=%d|remark=%s|",
			UnionGetApplicationName(),UnionGetCurrentFullSystemDateTime(),getpid(),ret,"备份密钥库失败");
	}
	else
	{
		sprintf(backupBuf,"applicationName=%s|dateTime=%s|pid=%d|responseCode=%d|remark=%s|",
			UnionGetApplicationName(),UnionGetCurrentFullSystemDateTime(),getpid(),ret,"备份密钥库成功");
	}
	ret1 = UnionSendTransInfoToMonitor(gunionResId, backupBuf, strlen(backupBuf));
	if (ret1 < 0)
	{       
		printf("in UnionBackupKeyDBEveryDay:: send data to monitor is failur,ret=[%d]!\n",ret1);
		return(ret1);
	}
	if (ret < 0)
		printf("[%s] [%d] backup key db error!\n",UnionGetCurrentFullSystemDateTime(),getpid());
	else
		printf("[%s] [%d] backup key db sucsses!\n",UnionGetCurrentFullSystemDateTime(),getpid());

	return(ret);
}

int UnionOutPutRecDataFromDB(char *str)
{
	char	*ptr;
	char	objectName[128+1];
	char	fileName[1024+1];
	int	ret;
	
	if ((str != NULL) && (strlen(str) != 0))
	{
		memset(fileName,0,sizeof(fileName));
		memset(objectName,0,sizeof(objectName));
		strcpy(objectName,str);
		sprintf(fileName,"%s/%s.txt",getenv("UNIONTEMP"),objectName);
		if ((ret = UnionOutputAllRecFromSpecTBL(objectName,"",fileName)) < 0)
			printf("UnionOutputAllRecFromSpecTBL [%s] failure! ret = [%d]\n",objectName,ret);
		else
			printf("UnionOutputAllRecFromSpecTBL [%s] OK! totalRecNum = [%d]\n",objectName,ret);
		return(ret);
	}
loop:
	memset(objectName,0,sizeof(objectName));
	memset(fileName,0,sizeof(fileName));
	
	ptr = UnionInput("\n\n请输入对象的名称(exit退出)::");
	strcpy(objectName,ptr);
	if (UnionIsQuit(ptr))
	{
		return(errCodeUserSelectExit);
	}
	sprintf(fileName,"%s/%s.txt",getenv("UNIONTEMP"),objectName);
	
	if ((ret = UnionOutputAllRecFromSpecTBL(objectName,"",fileName)) < 0)
		printf("UnionOutputAllRecFromSpecTBL [%s] failure! ret = [%d]\n",objectName,ret);
	else
		printf("UnionOutputAllRecFromSpecTBL [%s] OK! totalRecNum = [%d]\n",objectName,ret);
	
	goto loop;
}

