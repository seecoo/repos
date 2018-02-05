//	Wolfgang Wang
//	2009/4/29

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "unionGenIncFromTBL.h"
#include "unionCommand.h"
#include "unionErrCode.h"

char gunionMsgSvrTmpFileName[512] = "";

char *UnionGenerateMngSvrTempFile()
{
	sprintf(gunionMsgSvrTmpFileName,"%s/%s.tmp",getenv("UNIONTEMP"),getpid());
	return(gunionMsgSvrTmpFileName);
}

int UnionHelp()
{
	printf("Usage:: genTBLQuery [tableName|all|tableList]\n");
	return(0);
}

int main(int argc,char *argv[])
{
	int	ret;
	int	argvIndex = 1;
	int	index;
	char	*ptr;
	char	tmpBuf[128+1];
	
	if (argc >= 2)
	{
		strcpy(tmpBuf,argv[argvIndex]);
		UnionToUpperCase(tmpBuf);
		if (strcmp(tmpBuf,"HELP") == 0)
			return(UnionHelp());
	}
	if (argc == argvIndex + 1)
	{
		strcpy(tmpBuf,argv[argvIndex]);
		UnionToUpperCase(tmpBuf);
		//if (strcmp(tmpBuf,"ALL") == 0)
		//	return(UnionGenerateAllTBLQueryConfFromTBLDefInTableListToFp(stdout));
	}
	for (index = argvIndex; index < argc; index++)
	{
		strcpy(tmpBuf,argv[index]);
		if ((ret = UnionGenerateTBLQueryConfFromTBLDefInTableListToFp(tmpBuf,stdout)) < 0)
		{
			printf("UnionGenerateTBLQueryConfFromTBLDefInTableListToFp [%s] failure! ret = [%d]\n",tmpBuf,ret);
			continue;
		}
		else
			printf("UnionGenerateTBLQueryConfFromTBLDefInTableListToFp [%s] ok!\n",tmpBuf);
	}
	if (argc > argvIndex + 1)
		return(0);

loop:
	ptr = UnionInput("\n\n请输入表名(exit退出)::");
	strcpy(tmpBuf,ptr);
	if (UnionIsQuit(ptr))
		return(errCodeUserSelectExit);
	if ((ret = UnionGenerateTBLQueryConfFromTBLDefInTableListToFp(tmpBuf,stdout)) < 0)
	{
		printf("UnionGenerateTBLQueryConfFromTBLDefInTableListToFp [%s] failure! ret = [%d]\n",tmpBuf,ret);
		goto loop;
	}
	else
		printf("UnionGenerateTBLQueryConfFromTBLDefInTableListToFp [%s] ok!\n",tmpBuf);
	goto loop;
}
