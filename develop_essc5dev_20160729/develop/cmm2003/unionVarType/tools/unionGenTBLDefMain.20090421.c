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
	printf("Usage:: genTBLDef [-i|-o] [tableName|all|tableList]\n");
	printf(" -i	user confirmation needed when file already exists.\n");
	printf(" -o	overwrite the file if it already exists.\n");
	printf(" -j	keep the already existing file when it already exists.\n");
	return(0);
}

int main(int argc,char *argv[])
{
	int	ret;
	int	argvIndex = 1;
	int	index;
	char	*ptr;
	char	tmpBuf[128+1];
	int	interactive = conOverwriteModeWhenFileExistsUserConfirm;
	
	if (argc >= 2)
	{
		strcpy(tmpBuf,argv[argvIndex]);
		UnionToUpperCase(tmpBuf);
		if (strcmp(tmpBuf,"HELP") == 0)
			return(UnionHelp());
		if (strcmp(tmpBuf,"-I") == 0)
		{
			interactive = conOverwriteModeWhenFileExistsUserConfirm;
			argvIndex++;
		}
		else if (strcmp(tmpBuf,"-O") == 0)
		{
			interactive = conOverwriteModeWhenFileExistsOverwrite;
			argvIndex++;
		}
		else if (strcmp(tmpBuf,"-J") == 0)
		{
			interactive = conOverwriteModeWhenFileExistsReturn;
			argvIndex++;
		}
	}
	if (argc == argvIndex + 1)
	{
		strcpy(tmpBuf,argv[argvIndex]);
		UnionToUpperCase(tmpBuf);
		if (strcmp(tmpBuf,"ALL") == 0)
			return(UnionCreateAllObjectDefFromTBLDefInTableList(interactive));
	}
	for (index = argvIndex; index < argc; index++)
	{
		strcpy(tmpBuf,argv[index]);
		if ((ret = UnionCreateObjectDefFromTBLDefInTableList(tmpBuf,interactive)) < 0)
		{
			printf("UnionCreateObjectDefFromTBLDefInTableList [%s] failure! ret = [%d]\n",tmpBuf,ret);
			continue;
		}
		else
			printf("UnionCreateObjectDefFromTBLDefInTableList [%s] ok!\n",tmpBuf);
	}
	if (argc > argvIndex + 1)
		return(0);

loop:
	ptr = UnionInput("\n\n请输入表名(exit退出)::");
	strcpy(tmpBuf,ptr);
	if (UnionIsQuit(ptr))
		return(errCodeUserSelectExit);
	if ((ret = UnionCreateObjectDefFromTBLDefInTableList(tmpBuf,interactive)) < 0)
	{
		printf("UnionCreateObjectDefFromTBLDefInTableList [%s] failure! ret = [%d]\n",tmpBuf,ret);
		goto loop;
	}
	else
		printf("UnionCreateObjectDefFromTBLDefInTableList [%s] ok!\n",tmpBuf);
	goto loop;
}
