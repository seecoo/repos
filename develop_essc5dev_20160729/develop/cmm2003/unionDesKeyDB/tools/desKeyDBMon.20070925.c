//	Author:		Wolfgang Wang
//	Date:		2003/09/22


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

extern WINDOW		*pgContentWin;

PUnionTaskInstance	ptaskInstance = NULL;

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

int main(int argc,char *argv[])
{
	int	interval;
	
	
	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		interval = 1;
	else
	{
		if ((interval = abs(atoi(argv[1]))) <= 0)
			interval = 1;
	}

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

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());


	return(UnionMonitorMain(interval,UnionGetApplicationName(),"密钥监控程序",UnionConnectDesKeyDB,RefreshDesKeyDBWin,UnionTaskActionBeforeExit));
}

int UnionHelp()
{
	printf("Usage:: %s [seconds]\n",UnionGetApplicationName());
	return(0);
}
