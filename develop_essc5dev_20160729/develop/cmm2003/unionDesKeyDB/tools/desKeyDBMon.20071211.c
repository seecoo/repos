//	Author:		Wolfgang Wang
//	Date:		2003/09/22

// 20071211在desKeyDBMon.20070925.c基础上升级成本程序

#define _UnionTask_3_x_	
#define _UnionLogMDL_3_x_

#include <stdio.h>
#ifdef _AIX_
#include <cur00.h>
#else
#include <curses.h>
#endif
#include <signal.h>
#include <time.h>

#include "UnionTask.h"
#include "UnionLog.h"
#include "unionDesKeyDB.h"
#include "unionVersion.h"

#include "unionMonitor.h"

extern PUnionDesKeyDB			pgunionDesKeyDB;
extern PUnionDesKey			pgunionDesKey;

extern WINDOW				*pgContentWin;

PUnionTaskInstance			ptaskInstance = NULL;
#define	conMaxNumOfInfoPerWin		(gLineOfWin-2-1)


int UnionTaskActionBeforeExit()
{
	UnionKillMonitorTwinProcess();
	CloseTaskMonitor();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	//return(exit(0));
	exit(0);
}

int RefreshDesKeyDBWin(int interval)
{
	int		i,j;
	char		Buf[128];
	int		keyNum = 0;
	int		len;
	long		minStillEffecitveDays;
	PUnionDesKey	pkey;
	long		days1,days2;
	time_t		now;	
	long		usedDays,maxAvailableDays;
	int		keyNearOutdate = 0;
	int		keyIsLocked = 0;
	
	if (pgContentWin == NULL)
		return(-1);
	
	if (pgunionDesKey == NULL)
		return(-1);
	
	if ((minStillEffecitveDays = UnionReadLongTypeRECVar("minActiveDaysOfKey")) <= 0)
		minStillEffecitveDays = 30;

	for (keyNum = 0,i = 0;;)
	{
		memset(Buf,0,sizeof(Buf));
		memset(Buf,' ',gColOfWin - 4);
		for (j = 1; j <= gLineOfWin - 2; j++)
		{
			wmove(pgContentWin,j,2);
			waddstr(pgContentWin,Buf);
		}
		memset(Buf,0,sizeof(Buf));
		sprintf(Buf,"第%04d页%22s %8s %2s %12s %16s",
			i / (gLineOfWin - 2) + 1,
			"    密   钥   名   称    ",
			"有效天数","锁","可 用 次 数","    使用次数");
		wmove(pgContentWin,(i % (gLineOfWin - 2)) + 1,2);
		waddstr(pgContentWin,Buf);
		++i;
		for (;keyNum < pgunionDesKeyDB->num; keyNum++)
		{
			keyNearOutdate = 0;
			keyIsLocked = 0;
			pkey = pgunionDesKey+keyNum;
			// 从有效期计算有效天数
			if ((usedDays = UnionDecideDaysBeforeToday(pkey->activeDate)) < 0)	// 密钥还没有生效
				goto warningThisKey;
			if ((days1 = pkey->maxEffectiveDays - usedDays) <= 0) // 已经失效
			{
				keyNearOutdate = 1;
				goto warningThisKey;
			}
			// 从使用次数计算有效天数
			if (pkey->maxUseTimes <= 0)
			{
				days2 = days1;
				goto compare;
			}
			if (pkey->useTimes >= pkey->maxUseTimes)
			{
				keyNearOutdate = 1;
				days1 = 0;
				goto warningThisKey;
			}
			if (pkey->useTimes <= 0)
			{
				days2 = days1;
				goto compare;
			}
			if (usedDays != 0)
				maxAvailableDays = (long)((double)pkey->maxUseTimes / (double)pkey->useTimes * (double)usedDays);
			else
				maxAvailableDays = (long)((double)pkey->maxUseTimes / (double)pkey->useTimes);
			if (maxAvailableDays < 0)
				days2 = days1;
			else
				days2 = maxAvailableDays - usedDays;
			//UnionLog("%s %ld %ld %ld %ld\n",pkey->fullName,maxAvailableDays,
			//		pkey->maxUseTimes,pkey->useTimes,usedDays);
			// 判断是否已临近失效
compare:
			if (days1 > days2)
				days1 = days2;
			if (days1 > minStillEffecitveDays)	// 未临近失效
				goto warningThisKey;
			// 临近失效
			keyNearOutdate = 1;
warningThisKey:
			//UnionProgramerLog("%s %s %ld\n",pkey->fullName,pkey->activeDate,usedDays);
#ifdef _UnionDesKey_4_x_
			keyIsLocked = pkey->isWritingLocked;
#endif
			if ((!keyIsLocked) && (!keyNearOutdate))
				continue;
			memset(Buf,0,sizeof(Buf));
			if (pkey->maxUseTimes > 0)
				sprintf(Buf,"%30s %9ld %4d %12ld %16ld",pkey->fullName,days1,keyIsLocked,
						pkey->maxUseTimes - pkey->useTimes,pkey->useTimes);
			else
				sprintf(Buf,"%30s %9ld %4d %12s %16ld",pkey->fullName,days1,keyIsLocked,
						"X",pkey->useTimes);
			wmove(pgContentWin,(i % (gLineOfWin - 2))+1,2);
			waddstr(pgContentWin,Buf);
			++i;
			if ((i % (gLineOfWin - 2) == 0) && (i != 0))
			{
				keyNum++;
				break;
			}
		}
		wrefresh(pgContentWin);
		if (keyNum < pgunionDesKeyDB->num)
		{
			if (interval < 10)
				sleep(10);
			else
				sleep(interval);
		}
		else
		{
			if (i <= gLineOfWin - 2)
				break;
			if (interval < 10)
				sleep(10);
			else
				sleep(interval);
			break;
		}
	}
			
	return(0);
}

int RefreshCheckBrothersWin(int interval)
{
	int		i;
	char		Buf[conMaxNumOfInfoPerWin][128+1];
	int		keyNum = 0;
	int		len;
	PUnionDesKey	pkey = NULL;
	char		ipAddr[20][15+1];
	int		unsameNum;
	int		bufIndex;
	int		pageIndex = 1;
	int		lastIPAddrIndex = -1;
	int		pageIsEmpty = 1;
				
	if (pgContentWin == NULL)
		return(-1);
	
	if (pgunionDesKey == NULL)
		return(-1);
	
	for (keyNum = 0,unsameNum=0;;pageIndex++)
	{
		pageIsEmpty = 1;
		// 显示标题
		i = 1;
		sprintf(Buf[0],"第%04d页%32s %15s",
			pageIndex,
			"   密   钥   名   称  ",
			"备份Svr地址");
		wmove(pgContentWin,i,2);
		waddstr(pgContentWin,Buf[0]);
		// 初始化显示内容
		for (bufIndex = 0; bufIndex < conMaxNumOfInfoPerWin; bufIndex++)
		{
			memset(Buf[bufIndex],0,sizeof(Buf[bufIndex]));
			memset(Buf[bufIndex],' ',gColOfWin - 4);
		}
		bufIndex = 0;
		if ((pageIndex > 1) && (pkey != NULL))
		{
			// 显示上次未显示完的不同步的密钥
			for (bufIndex = 0;(lastIPAddrIndex >= 0) && (lastIPAddrIndex < unsameNum) 
				&& (lastIPAddrIndex < 20) && (bufIndex < conMaxNumOfInfoPerWin); 
				lastIPAddrIndex++,bufIndex++)
			{
				sprintf(Buf[bufIndex],"%40s %s",pkey->fullName,ipAddr[lastIPAddrIndex]);
			}
		}
		for (;keyNum < pgunionDesKeyDB->num; keyNum++)
		{
			pkey = pgunionDesKey+keyNum;
			if ((unsameNum = UnionCountAllUnsameDesKeyBrothers(pkey->fullName,pkey->value,ipAddr,20)) == 0)
			{
				//strcpy(ipAddr[0],"all same");
				//unsameNum = 1;
				continue;
			}
			else if (unsameNum < 0)
			{
				strcpy(ipAddr[0],"cannot check");
				unsameNum = 1;
			}
			for (lastIPAddrIndex = 0; (lastIPAddrIndex < unsameNum) && (lastIPAddrIndex < 20) ;
				bufIndex++,lastIPAddrIndex++)
			{
				if (bufIndex >= conMaxNumOfInfoPerWin)	// 显示已满
					goto refreshNow;
				sprintf(Buf[bufIndex],"%40s %s",pkey->fullName,ipAddr[lastIPAddrIndex]);
			}
		}
refreshNow:
		if (bufIndex > 0)
			pageIsEmpty = 0;
		for (i = 2,bufIndex = 0; bufIndex < conMaxNumOfInfoPerWin; i++,bufIndex++)
		{
			if ((len = strlen(Buf[bufIndex])) > 80 - 4)
				Buf[bufIndex][80-4] = 0;
			else
			{
				memset(Buf[bufIndex]+len,' ',80 - 4 - len);
				Buf[bufIndex][80-4] = 0;
			}
			wmove(pgContentWin,i,2);
			waddstr(pgContentWin,Buf[bufIndex]);
		}
		wrefresh(pgContentWin);
		if (!pageIsEmpty)
			sleep(10);
		if (keyNum < pgunionDesKeyDB->num)
		{
			continue;
		}
		break;
	}
	return(0);
}

int main(int argc,char *argv[])
{
	int	interval = 1;
	int	userSetInterval = 0;
	int	checkBrothers = 0;
	int	i;
	char	tmpBuf[256];
	
	UnionSetApplicationName(argv[0]);

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);

	// 分析参数
	for (i = 1; i < argc; i++)
	{
		strcpy(tmpBuf,argv[i]);
		UnionToUpperCase(tmpBuf);
		if (UnionIsDigitStr(tmpBuf))
		{
			interval = atoi(tmpBuf);
			userSetInterval = 1;
		}
		else if (strcmp(tmpBuf,"CHECKBROTHERS") == 0)
			checkBrothers = 1;
	}
	if (interval <= 0)
		interval = 1;
	if (checkBrothers && (!userSetInterval))
		interval = 30;

	if (!checkBrothers)
	{		
		if (UnionExistsAnotherTaskOfName(UnionGetApplicationName()))	// 2007/12/11修改
		{
			printf("Another task started!\n");
			return(UnionTaskActionBeforeExit());
		}
		if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,UnionGetApplicationName())) == NULL)
		{
			printf("UnionCreateTaskInstance Error!\n");
			return(UnionTaskActionBeforeExit());
		}
		return(UnionMonitorMain(interval,UnionGetApplicationName(),"密钥监控程序",UnionConnectDesKeyDB,RefreshDesKeyDBWin,UnionTaskActionBeforeExit));
	}
	else
	{
		sprintf(tmpBuf,"%s checkbrothers",UnionGetApplicationName());
		if (UnionExistsAnotherTaskOfName(tmpBuf))
		{
			printf("Another task started!\n");
			return(UnionTaskActionBeforeExit());
		}
		if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,tmpBuf)) == NULL)
		{
			printf("UnionCreateTaskInstance Error!\n");
			return(UnionTaskActionBeforeExit());
		}
		return(UnionMonitorMain(interval,UnionGetApplicationName(),"密钥备份服务器密钥同步检测程序",UnionConnectDesKeyDB,RefreshCheckBrothersWin,UnionTaskActionBeforeExit));
	}
}

int UnionHelp()
{
	printf("Usage:: %s [seconds] [checkbrothers]\n",UnionGetApplicationName());
	return(0);
}
