//	Author:		Wolfgang Wang
//	Date:		2001/08/29
//	Version:	2.0

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
#include <unistd.h>

#include "UnionLog.h"
#include "unionCommand.h"
#include "unionVersion.h"
#include "UnionTask.h"
#include "unionLockTBL.h"

PUnionTaskInstance	ptaskInstance = NULL;
PUnionLockTBL		pgunionLockTBL;
int UnionHelp()
{
	printf("Usage:: %s command\n",UnionGetApplicationName());
	printf(" Where command as:\n");
	printf("  -reload 		reload the unionLockTBL\n");
	printf("  -initial 		initial the unionLockTBL\n");
	printf("  -remove		remove the unionLockTBL\n");
	printf("     --print all locks!!--\n");
	printf("  -printAllLocks 	print all unionLockTBL of AllLock\n");
	printf("  -printAllResLocks 	print all unionLockTBL of AllResLock\n");
	printf("  -printAllRecLocks 	print all unionLockTBL of AllRecLock\n");
	printf("     --free all locks!!--\n");		
	printf("  -freeWriteLocks 	free all unionLockTBL of AllWriteLock\n");
	printf("  -freeReadLocks 	free all unionLockTBL of AllReadLock\n");
	printf("  -freeResWriteLocks 	free all unionLockTBL of AllResWriteLock\n");
	printf("  -freeResReadLocks 	free all unionLockTBL of AllResReadLock\n");
	printf("  -freeRecWriteLocks 	free all unionLockTBL of AllRecWriteLock\n");
	printf("  -freeRecReadLocks 	free all unionLockTBL of AllRecReadLock\n");	
	printf("     --free a lock!!--\n");
	printf("  -freeResWriteLock [resName] 	free appoint unionLockTBL of AllResWriteLock\n");
	printf("  -freeResReadLock  [resName] 	free appoint unionLockTBL of AllResReadLock\n");
	printf("  -freeRecWriteLock [resName] [keyWord]	free appoint AllRecWriteLock\n");
	printf("  -freeRecReadLock  [resName] [keyWord]	free appoint AllRecReadLock\n");
	printf("     --block apply lock!!--\n");
	printf("  -applyResWriteLock [resName] block apply the Writing Lock of Resource\n");
	printf("  -applyResReadLock  [resName] block apply the Reading Lock of Resource\n");
	printf("  -applyRecWriteLock [resName] [keyWord] block apply the Writing Lock of Record\n");
	printf("  -applyRecReadLock  [resName] [keyWord] block apply the Reading Lock of Record\n");
	printf("     --unblock apply lock!!--\n");
	printf("  -ubApplyResWriteLock [resName] [timeout] unblock apply the Writing Lock of Resource\n");
	printf("  -ubApplyResReadLock  [resName] [timeout] unblock apply the Reading Lock of Resource\n");
	printf("  -ubApplyRecWriteLock [resName] [keyWord] [timeout] unblock apply Writing Lock of Record\n");
	printf("  -ubApplyRecReadLock  [resName] [keyWord] [timeout] unblock apply Reading Lock of Record\n");
	printf("  -clear [maxLockedSeconds] [interval]	clear dead locks\n");
	return(0);
}

int UnionTaskActionBeforeExit()
{
	UnionDisconnectLockTBL();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	//return(exit(0));
	exit(0);
}

int PrintAllRecLocks(FILE *fp)
{
	int	ret;
	if ((ret = UnionPrintAllRecLockOfSpecResToFile("",fp)) < 0)
	{
		printf("in PrintAllRecLocks:: UnionPrintAllRecLockOfSpecResToFile error! ret = [%d]\n",ret);
		return (ret);
	}
	return (0);
}

int PrintAllLocks(FILE *fp)
{
	int	ret;
	
	if ((ret = UnionPrintAllResLockToFile(fp)) < 0)
	{
		printf("in PrintAllLocks:: UnionPrintAllResLockToFile error! ret = [%d]\n",ret);
		return (ret);
	}
	if ((ret = PrintAllRecLocks(fp)) < 0)
	{
		printf("in PrintAllLocks:: PrintAllRecLocks error! ret = [%d]\n",ret);
		return (ret);
	}
	
	return (0);
}

int PrintAllResLocks(FILE *fp)
{
	int	ret;
	if ((ret = UnionPrintAllResLockToFile(fp)) < 0)
	{
		printf("in PrintAllResLocks:: UnionPrintAllLockToFile error! ret = [%d]\n",ret);
		return (ret);
	}
	return (0);
}

int FreeAllResWriteLock()
{
	int	ret;
	int	i;
	
	if ((ret = UnionIsLockTBLConnected()) == 0)
	{
		if ((ret = UnionConnectLockTBL()) < 0)
		{
			UnionUserErrLog("in FreeAllResWriteLock:: UnionConnectLockTBL!\n");
			return(ret);
		}
	}
	for (i = 0; i < pgunionLockTBL->maxNum; i++)
	{	
		if (!UnionIsResLevelLockedRec(&(pgunionLockTBL->lockGrp[i])))
			continue;
		
		if ((pgunionLockTBL->lockGrp[i].resName !=NULL) && pgunionLockTBL->lockGrp[i].isWritingLocked == 1)
		{
			printf("%s\n",pgunionLockTBL->lockGrp[i].resName);
			if ((ret = UnionFreeLock(pgunionLockTBL->lockGrp[i].resName,NULL,1) < 0))
			{
				printf("in FreeAllResWriteLock:: UnionFreeLock error! ret = [%d]\n",ret);
				return (ret);
			}
		}
	}
	return (0);
}

int FreeAllRecWriteLock()
{
	int	ret;
	int	i;
	
	if ((ret = UnionIsLockTBLConnected()) == 0)
	{
		if ((ret = UnionConnectLockTBL()) < 0)
		{
			UnionUserErrLog("in FreeAllRecWriteLock:: UnionConnectLockTBL!\n");
			return(ret);
		}
	}
	
	for (i = 0; i < pgunionLockTBL->maxNum; i++)
	{	
		if (!UnionIsRecLevelLockedRec(&(pgunionLockTBL->lockGrp[i])))
			continue;
		if ((pgunionLockTBL->lockGrp[i].resName !=NULL) && (pgunionLockTBL->lockGrp[i].keyWord != NULL) && (pgunionLockTBL->lockGrp[i].isWritingLocked == 1))
		{
			printf("%s %s\n",pgunionLockTBL->lockGrp[i].resName,pgunionLockTBL->lockGrp[i].keyWord);
			if ((ret = UnionFreeLock(pgunionLockTBL->lockGrp[i].resName,pgunionLockTBL->lockGrp[i].keyWord,1) < 0))
			{
				printf("in FreeAllRecWriteLock:: UnionFreeLock error! ret = [%d]\n",ret);
				return (ret);
			}
		}
	}
	return (0);
}

int FreeAllWriteLock()
{
	int	ret;
	
	if ((ret = UnionIsLockTBLConnected()) == 0)
	{
		if ((ret = UnionConnectLockTBL()) < 0)
		{
			UnionUserErrLog("in FreeAllWriteLock:: UnionConnectLockTBL!\n");
			return(ret);
		}
	}
	
	if ((ret = FreeAllResWriteLock()) < 0)
	{
		printf("in FreeAllWriteLock:: FreeAllResWriteLock error! ret = [%d]\n",ret);
		return (ret);
	}
	if ((ret = FreeAllRecWriteLock()) < 0)
	{
		printf("in FreeAllWriteLock:: FreeAllRecWriteLock error! ret = [%d]\n",ret);
		return (ret);
	}
	return (0);
}

int FreeAllResReadLock()
{
	int	ret;
	int	i;
	
	if ((ret = UnionIsLockTBLConnected()) == 0)
	{
		if ((ret = UnionConnectLockTBL()) < 0)
		{
			UnionUserErrLog("in FreeAllResReadLock:: UnionConnectLockTBL!\n");
			return(ret);
		}
	}
	
	for (i = 0; i < pgunionLockTBL->maxNum; i++)
	{	
		if (!UnionIsResLevelLockedRec(&(pgunionLockTBL->lockGrp[i])))
			continue;
		
		if ((pgunionLockTBL->lockGrp[i].resName !=NULL) && pgunionLockTBL->lockGrp[i].isWritingLocked == 0)
		{
			printf("%s\n",pgunionLockTBL->lockGrp[i].resName);
			if ((ret = UnionFreeLock(pgunionLockTBL->lockGrp[i].resName,NULL,0) < 0))
			{
				printf("in FreeAllResReadLock:: UnionFreeLock error! ret = [%d]\n",ret);
				return (ret);
			}
		}
	}
	return (0);
}

int FreeAllRecReadLock()
{
	int	ret;
	int	i;
	
	if ((ret = UnionIsLockTBLConnected()) == 0)
	{
		if ((ret = UnionConnectLockTBL()) < 0)
		{
			UnionUserErrLog("in FreeAllRecReadLock:: UnionConnectLockTBL!\n");
			return(ret);
		}
	}
	
	for (i = 0; i < pgunionLockTBL->maxNum; i++)
	{	
		if (!UnionIsRecLevelLockedRec(&(pgunionLockTBL->lockGrp[i])))
			continue;
		if ((pgunionLockTBL->lockGrp[i].resName !=NULL) && (pgunionLockTBL->lockGrp[i].keyWord != NULL) && pgunionLockTBL->lockGrp[i].isWritingLocked == 0)
		{
			printf("%s %s\n",pgunionLockTBL->lockGrp[i].resName,pgunionLockTBL->lockGrp[i].keyWord);
			if ((ret = UnionFreeLock(pgunionLockTBL->lockGrp[i].resName,pgunionLockTBL->lockGrp[i].keyWord,0) < 0))
			{
				printf("in FreeAllRecReadLock:: UnionFreeLock error! ret = [%d]\n",ret);
				return (ret);
			}
		}
	}
	return (0);
}

int FreeAllReadLock()
{
	int	ret;
	
	if ((ret = UnionIsLockTBLConnected()) == 0)
	{
		if ((ret = UnionConnectLockTBL()) < 0)
		{
			UnionUserErrLog("in FreeAllReadLock:: UnionConnectLockTBL!\n");
			return(ret);
		}
	}
	
	if ((ret = FreeAllResReadLock()) < 0)
	{
		printf("in FreeAllReadLock:: FreeAllResReadLock error! ret = [%d]\n",ret);
		return (ret);
	}
	if ((ret = FreeAllRecReadLock()) < 0)
	{
		printf("in FreeAllReadLock:: FreeAllRecReadLock error! ret = [%d]\n",ret);
		return (ret);
	}
	return (0);
}

int main(int argc,char *argv[])
{
	int	ret;
	long	seconds = 1;
	int	maxLockedSeconds = 60;
	
	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		return(UnionHelp());
	
	if ((UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
	
	if (strcasecmp(argv[1],"-RELOAD") == 0)
	{
		if (!UnionConfirm("确定加载共享内存[锁表]吗?"))
			return(-1);
		if ((ret = UnionReloadLockTBL()) < 0)
			printf("***** %-30s Error!\n","加载共享内存[锁表]");
		else
			printf("***** %-30s OK!\n","加载共享内存[锁表]");
	}
	else if (strcasecmp(argv[1],"-RELOADANYWAY") == 0)
	{
		if ((ret = UnionReloadLockTBL()) < 0)
			printf("***** %-30s Error!\n","加载共享内存[锁表]");
		else
			printf("***** %-30s OK!\n","加载共享内存[锁表]");
	}
	else if (strcasecmp(argv[1],"-INITIAL") == 0)
	{	
		if ((ret = UnionResetLockTBL()) < 0)
			printf("UnionResetLockTBL Error! ret = [%d]\n",ret);
		else
			printf("UnionResetLockTBL OK!\n");
	}
	else if (strcasecmp(argv[1],"-REMOVE") == 0)
	{
		if ((ret = UnionRemoveLockTBL()) < 0)
			printf("UnionRemoveLockTBL error! ret = [%d]\n",ret);
		else
			printf("UnionRemoveLockTBL OK!\n");
	}
	// 显示锁	
	else if (strcasecmp(argv[1],"-PRINTALLLOCKS") == 0)
		PrintAllLocks(stdout);
	else if (strcasecmp(argv[1],"-PRINTALLRESLOCKS") == 0)
		PrintAllResLocks(stdout);			
	else if (strcasecmp(argv[1],"-PRINTALLRECLOCKS") == 0)
		PrintAllRecLocks(stdout);					
	// 释放全部锁
	else if (strcasecmp(argv[1],"-FREEWRITELOCKS") == 0)
		if ((ret = FreeAllWriteLock()) < 0)
		{
			printf("FreeAllWriteLock error! ret = [%d]\n",ret);
		}
		else
			printf("FreeAllWriteLock OK!\n");
	else if (strcasecmp(argv[1],"-FREEREADLOCKS") == 0)
		if ((ret = FreeAllReadLock()) < 0)
		{
			printf("FreeAllReadLock error! ret = [%d]\n",ret);
		}
		else
			printf("FreeAllReadLock OK!\n");
	else if (strcasecmp(argv[1],"-FREERESWRITELOCKS") == 0)
		if ((ret = FreeAllResWriteLock()) < 0)
		{
			printf("FreeAllResWriteLock error! ret = [%d]\n",ret);
		}
		else
			printf("FreeAllResWriteLock OK!\n");
	else if (strcasecmp(argv[1],"-FREERESREADLOCKS") == 0)
		if ((ret = FreeAllResReadLock()) < 0)
		{
			printf("FreeAllResReadLock error! ret = [%d]\n",ret);
		}
		else
			printf("FreeAllResReadLock OK!\n");
	else if (strcasecmp(argv[1],"-FREERECWRITELOCKS") == 0)
		if ((ret = FreeAllRecWriteLock()) < 0)
		{
			printf("FreeAllRecWriteLock error! ret = [%d]\n",ret);
		}
		else
			printf("FreeAllRecWriteLock OK!\n");
	else if (strcasecmp(argv[1],"-FREERECREADLOCKS") == 0)
	{
		if ((ret = FreeAllRecReadLock()) < 0)
		{
			printf("FreeAllRecReadLock error! ret = [%d]\n",ret);
		}
		else
			printf("FreeAllRecReadLock OK!\n");
	}
	// 释放单个锁
	else if (strcasecmp(argv[1],"-FREERESWRITELOCK") == 0)
	{
		if ( NULL== argv[2])
			printf("sorroy!You enter the parameters of a lack of\n");
		else if ((ret = UnionFreeResWritingLockOfSpecRes(argv[2])) < 0)
			printf("UnionFreeResWritingLockOfSpecRes error! ret = [%d]\n",ret);
		else
			printf("UnionFreeResWritingLockOfSpecRes OK!\n");	
	}
	else if (strcasecmp(argv[1],"-FREERESREADLOCK") == 0)
	{
		if ( NULL== argv[2])
			printf("sorroy!You enter the parameters of a lack of\n");
		else if ((ret = UnionFreeResReadingLockOfSpecRes(argv[2])) < 0)
			printf("UnionFreeResReadingLockOfSpecRes error! ret = [%d]\n",ret);
		else
			printf("UnionFreeResReadingLockOfSpecRes OK!\n");	
	}
	else if (strcasecmp(argv[1],"-FREERECWRITELOCK") == 0)
	{
		if ( NULL== argv[2] ||  NULL== argv[3])
			printf("sorroy!You enter the parameters of a lack of\n");
		else if ( (ret = UnionFreeRecWritingLockOfSpecRec(argv[2],argv[3])) < 0)
			printf("UnionFreeRecWritingLockOfSpecRec error! ret = [%d]\n",ret);
		else
			printf("UnionFreeRecWritingLockOfSpecRec OK!\n");
	}
	else if (strcasecmp(argv[1],"-FREERECREADLOCK") == 0)
	{
		if ( NULL== argv[2] ||  NULL== argv[3])
			printf("sorroy!You enter the parameters of a lack of\n");
		else if ((ret = UnionFreeRecReadingLockOfSpecRec(argv[2],argv[3])) < 0)
			printf("UnionFreeRecReadingLockOfSpecRec error! ret = [%d]\n",ret);
		else
			printf("UnionFreeRecReadingLockOfSpecRec OK!\n");
	}
	// 阻塞申请读写锁
	else if (strcasecmp(argv[1],"-APPLYRESWRITELOCK") == 0)
	{
		if ( NULL== argv[2])
			printf("sorroy!You enter the parameters of a lack of:resName\n");
		else if ((ret = UnionApplyResWritingLockOfSpecRes(argv[2])) < 0)
			printf("UnionApplyResWritingLockOfSpecRes error! ret = [%d]\n",ret);
		else
			printf("UnionApplyResWritingLockOfSpecRes OK!\n");
	}
	else if (strcasecmp(argv[1],"-APPLYRESREADLOCK") == 0)
	{
		if ( NULL== argv[2])
			printf("sorroy!You enter the parameters of a lack of\n");
		else if ((ret = UnionApplyResReadingLockOfSpecRes(argv[2])) < 0)
			printf("UnionApplyResReadingLockOfSpecRes error! ret = [%d]\n",ret);
		else
			printf("UnionApplyResReadingLockOfSpecRes OK!\n");
	}
	else if (strcasecmp(argv[1],"-APPLYRECWRITELOCK") == 0)
	{
		if ( NULL== argv[2] ||  NULL== argv[3])
			printf("sorroy!You enter the parameters of a lack of\n");
		else if ((ret = UnionApplyRecWritingLockOfSpecRec(argv[2],argv[3])) < 0)
			printf("UnionApplyRecWritingLockOfSpecRec error! ret = [%d]\n",ret);
		else
			printf("UnionApplyRecWritingLockOfSpecRec OK!\n");
	}
	else if (strcasecmp(argv[1],"-APPLYRECREADLOCK") == 0)
	{
		if ( NULL== argv[2] ||  NULL== argv[3])
			printf("sorroy!You enter the parameters of a lack of\n");
		else if ((ret = UnionApplyRecReadingLockOfSpecRec(argv[2],argv[3])) < 0)
			printf("UnionApplyRecReadingLockOfSpecRec error! ret = [%d]\n",ret);
		else
			printf("UnionApplyRecReadingLockOfSpecRec OK!\n");
	}
	
	// 非阻塞申请读写锁
	else if (strcasecmp(argv[1],"-UBAPPLYRESWRITELOCK") == 0)
	{
		if ( NULL== argv[2] ||  NULL== argv[3])
			printf("Use: -UBAPPLYRESWRITELOCK resName timeout\n");
		else if ((ret = UnionApplyResWritingLockOfSpecResWithTimeout(argv[2],atoi(argv[3]))) < 0)
			printf("UnionApplyResWritingLockOfSpecResWithTimeout error! ret = [%d]\n",ret);
		else
			printf("UnionApplyResWritingLockOfSpecResWithTimeout OK!\n");
	}
	else if (strcasecmp(argv[1],"-UBAPPLYRESREADLOCK") == 0)
	{
		if (NULL == argv[2] ||  NULL== argv[3])
			printf("Use: -UBAPPLYRESREADLOCK resName timeout\n");
		else if ((ret = UnionApplyResReadingLockOfSpecResWithTimeout(argv[2],atoi(argv[3]))) < 0)
			printf("UnionApplyResReadingLockOfSpecResWithTimeout error! ret = [%d]\n",ret);
		else
			printf("UnionApplyResReadingLockOfSpecResWithTimeout OK!\n");
	}
	else if (strcasecmp(argv[1],"-UBAPPLYRECWRITELOCK") == 0)
	{
		if ( NULL== argv[2] ||  NULL== argv[3] || NULL == argv[4])
			printf("sorroy!You enter the parameters of a lack of\n");
		else if ((ret = UnionApplyRecWritingLockOfSpecRecWithTimeout(argv[2],argv[3],atoi(argv[4]))) < 0)
			printf("UnionApplyRecWritingLockOfSpecRecWithTimeout error! ret = [%d]\n",ret);
		else
			printf("UnionApplyRecWritingLockOfSpecRecWithTimeout OK!\n");
	}
	else if (strcasecmp(argv[1],"-UBAPPLYRECREADLOCK") == 0)
	{
		if ( NULL== argv[2] ||  NULL== argv[3] || NULL == argv[4])
			printf("sorroy!You enter the parameters of a lack of\n");
		else if ((ret = UnionApplyRecReadingLockOfSpecRecWithTimeout(argv[2],argv[3],atoi(argv[4]))) < 0)
			printf("UnionApplyRecReadingLockOfSpecRecWithTimeout error! ret = [%d]\n",ret);
		else
			printf("UnionApplyRecReadingLockOfSpecRecWithTimeout OK!\n");
	}
	
	else if (strcasecmp(argv[1],"-CLEAR") == 0)
	{
		if (UnionCreateProcess() == 0)
		{
			if (argc > 2)
				maxLockedSeconds = atoi(argv[2]);
			if (maxLockedSeconds <= 0)
				maxLockedSeconds = 60;
			if (argc > 3)
				seconds = atoi(argv[3]);
			if (seconds <= 0)
				seconds = 1;
			if ((UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s -clear %d %d",UnionGetApplicationName(),maxLockedSeconds,seconds)) == NULL)
			{
				printf("UnionCreateTaskInstance Error!\n");
				return(UnionTaskActionBeforeExit());
			}
			for (;;)
			{
				if ((ret = UnionMaintainingAllDeadWritingLocks(maxLockedSeconds)) < 0)
				{
					UnionUserErrLog("in %s:: UnionMaintainingAllDeadWritingLocks!\n",UnionGetApplicationName());
					break;
				}
				sleep(seconds);
			}
		}
	}
	else
		printf("sorroy! Your input is wrong!\n"); 
		
	return(UnionTaskActionBeforeExit());
}

