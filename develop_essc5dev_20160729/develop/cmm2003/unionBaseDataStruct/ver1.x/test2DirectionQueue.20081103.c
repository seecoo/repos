// Wolfgang Wang
// 2008/11/03

#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <time.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "union2DirectionQueue.h"
#include "unionCommand.h"
#include "unionLockMachinism.h"

int UnionTest2DirectionQueue()
{
	PUnion2DirectionQueueRec	prec = NULL;
	char				*ptr;
	char				tmpBuf[512+1];
	int				isInsert = 1;
	int				ret;
	PUnion2DirectionQueueRec	ptmpRec;
	long				index;
	long				loopTimes;
	time_t				start,finish;
	
loopCommand:
	ptr = UnionInput("\n\n输入命令::(exit/insert/delete/loop)");
	strcpy(tmpBuf,ptr);
	if (UnionIsQuit(ptr))
	{
		printf("UnionDeleteAll2DirectionQueueRec = [%d]\n",UnionDeleteAll2DirectionQueueRec(prec));
		return(0);
	}
	if (strcmp(ptr,"INSERT") == 0)
		isInsert = 1;
	else if (strcmp(ptr,"DELETE") == 0)
		isInsert = 0;
	else if (strcmp(ptr,"LOOP") == 0)
	{
loopContinue:
		if (UnionIsQuit(ptr=UnionInput("输入循环次数::")))
			goto loopCommand;
		if (prec == NULL)
		{
			printf("queue is null!\n");
			if ((prec = UnionNew2DirectionQueueRec(ptr,NULL,0)) == NULL)
			{
				printf("UnionNew2DirectionQueueRec failure!\n");
				goto loopCommand;
			}
		}
		loopTimes = atol(ptr);
		time(&start);
		for (index = 0; index < loopTimes; index++)
		{
			sprintf(tmpBuf,"%ld",index);
			if ((ret = UnionInsert2DirectionQueueRec(prec,tmpBuf,NULL,0)) < 0)
			{
				printf("UnionInsert2DirectionQueueRec failure! ret = [%d]\n",ret);
				goto loopCommand;
			}			
		}
		time(&finish);
		//UnionPrintAll2DirectionQueueRecToFile(prec,NULL);
		printf("timeUsed = [%ld] for [%ld] index = [%ld]\n",finish-start,loopTimes,index);
		printf("UnionDeleteAll2DirectionQueueRec = [%d]\n",UnionDeleteAll2DirectionQueueRec(prec));
		prec = NULL;
		goto loopContinue;
	}	
	/*
	else
	{
		printf("非法命令字!");
		goto loopCommand;
	}
	*/
		
inputKey:
	UnionPrintAll2DirectionQueueRecToFile(prec,NULL);
	ptr = UnionInput("输入关键字::");
	strcpy(tmpBuf,ptr);
	if (UnionIsQuit(ptr))
		goto loopCommand;
	if (!isInsert)
		goto deleteNow;
	if (prec == NULL)
	{
		printf("queue is null!\n");
		if ((prec = UnionNew2DirectionQueueRec(tmpBuf,NULL,0)) == NULL)
		{
			printf("UnionNew2DirectionQueueRec failure!\n");
			goto inputKey;
		}
	}
	else
	{
		if ((ret = UnionInsert2DirectionQueueRec(prec,tmpBuf,NULL,0)) < 0)
		{
			printf("UnionInsert2DirectionQueueRec failure! ret = [%d]\n",ret);
			goto inputKey;
		}
	}
	goto inputKey;

deleteNow:
	if ((ret = UnionDelete2DirectionQueueRec(prec,tmpBuf)) < 0)
	{
		if (ret != errCodeKeyWordIsMyself)
		{
			printf("UnionDelete2DirectionQueueRec failure! ret = [%d]\n",ret);
			goto inputKey;
		}
		//else
		//	printf("is myself!\n");
		ptmpRec = prec->next;
		UnionFree2DirectionQueueRec(prec);
		prec = ptmpRec;	
		//printf("delete myself ok!!\n");
	}
	goto inputKey;
}

int UnionTestLockKeyWord()
{
	char			*ptr;
	char			tmpBuf[512+1];
	int			isLock = 1;
	int			ret;
	
loopCommand:
	ptr = UnionInput("\n\n输入命令::(exit/lock/unlock)");
	strcpy(tmpBuf,ptr);
	if (UnionIsQuit(ptr))
	{
		printf("UnionCloseKeyWordLockMachinism = [%d]\n",UnionCloseKeyWordLockMachinism());
		return(0);
	}
	if (strcmp(ptr,"LOCK") == 0)
		isLock = 1;
	else if (strcmp(ptr,"UNLOCK") == 0)
		isLock = 0;
	else
		goto lockNow;
	/*
	else
	{
		printf("非法命令字!");
		goto loopCommand;
	}
	*/
		
inputKey:
	UnionPrintAllLockedKeyWordToFile(NULL);
	ptr = UnionInput("输入关键字::");
	strcpy(tmpBuf,ptr);
	if (UnionIsQuit(ptr))
		goto loopCommand;
	if (!isLock)
		goto unlockNow;
lockNow:
	if ((ret = UnionLockKeyWord(tmpBuf)) < 0)
	{
		printf("UnionLockKeyWord failure! ret = [%d]\n",ret);
		goto inputKey;
	}
	printf("UnionLockKeyWord [%s] ok!\n",tmpBuf);
	goto inputKey;

unlockNow:
	if ((ret = UnionUnlockKeyWord(tmpBuf)) < 0)
	{
		printf("UnionUnlockKeyWord failure! ret = [%d]\n",ret);
		goto inputKey;
	}
	printf("UnionUnlockKeyWord [%s] ok!\n",tmpBuf);
	goto inputKey;
}

/*
void main()
{
	UnionTest2DirectionQueue();
}
*/
