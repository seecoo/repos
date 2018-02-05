//	Wolfgang Wang
//	2009/4/29

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "unionGenIncFromTBL.h"
#include "unionCommand.h"
#include "unionErrCode.h"
#include "unionOperationList.h"
#include "UnionLog.h"

int UnionHelp()
{
	printf("Usage:: genOperationID [fileName]\n");
	return(0);
}

int UnionInputResFldTagIntoTBL(char *fileName)
{
	char	tmpBuf[2048+1];
	FILE	*fp;
	int	len;
	int	ret;
	int	recNum = 0;
	TUnionOperationList	rec;
	int	offset;
	char	*ptr;
	
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionInputResFldTagIntoTBL:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	while (!feof(fp))
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((len = UnionReadOneLineFromTxtFile(fp,tmpBuf,sizeof(tmpBuf))) < 0)
			continue;
		if (len == 0)
			continue;
		if (strncmp(tmpBuf,"#define conResID",strlen("#define conResID")) == 0)
			continue;
		if (strncmp(tmpBuf,"#define ",strlen("#define ")) != 0)
			continue;
		offset = strlen("#define ");
		memset(&rec,0,sizeof(rec));
		sscanf(tmpBuf+offset,"%s%s",rec.macroName,rec.macroValue);
		if ((ptr = strstr(tmpBuf+offset,"//")) != NULL)
		{
			strcpy(rec.remark,ptr+2);
			UnionFilterHeadAndTailBlank(rec.remark);
			memcpy(rec.ID,rec.remark,sizeof(rec.ID)-1);
		}
		strcpy(rec.inputTeller,getenv("LOGNAME"));
		strcpy(rec.inputDate,UnionGetCurrentFullSystemDate());
		strcpy(rec.inputTime,UnionGetCurrentSystemTime());
		if ((ret = UnionInsertOperationListRec(&rec)) < 0)
		{
			UnionUserErrLog("in UnionInputResFldTagIntoTBL:: UnionInsertOperationListRec! [%s] [%s] [%s]\n",rec.ID,rec.macroName,rec.macroValue);
			continue;
		}
		recNum++;
	}
	fclose(fp);
	return(recNum);
}
				
int main(int argc,char *argv[])
{
	int	ret;
	int	argvIndex = 1;
	int	index;
	char	*ptr;
	char	tmpBuf[2048+1];
	FILE	*fp;
	
	if (argc >= 2)
	{
		strcpy(tmpBuf,argv[argvIndex]);
		UnionToUpperCase(tmpBuf);
		if (strcmp(tmpBuf,"HELP") == 0)
			return(UnionHelp());
	}
	for (index = argvIndex; index < argc; index++)
	{
		strcpy(tmpBuf,argv[index]);
		if ((ret = UnionInputResFldTagIntoTBL(tmpBuf)) < 0)
		{
			printf("UnionInputResFldTagIntoTBL [%s] failure! ret = [%d]\n",tmpBuf,ret);
			continue;
		}
		else
			printf("UnionInputResFldTagIntoTBL [%d] from [%s] ok!\n",ret,tmpBuf);
	}
	if (argc > argvIndex + 1)
		return(0);

loop:
	ptr = UnionInput("\n\n请输入文件名(exit退出)::");
	strcpy(tmpBuf,ptr);
	if (UnionIsQuit(ptr))
		return(errCodeUserSelectExit);
	if ((ret = UnionInputResFldTagIntoTBL(tmpBuf)) < 0)
	{
		printf("UnionInputResFldTagIntoTBL [%s] failure! ret = [%d]\n",tmpBuf,ret);
		goto loop;
	}
	else
		printf("UnionInputResFldTagIntoTBL [%d] from [%s] ok!\n",ret,tmpBuf);
	goto loop;
}
