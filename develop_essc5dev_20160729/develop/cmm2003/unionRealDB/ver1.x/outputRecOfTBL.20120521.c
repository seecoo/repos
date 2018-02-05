//	Wolfgang Wang
//	2008/2/25

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "unionCommand.h"
#include "unionErrCode.h"
#include "unionGenSQLFromTBLDefList.h"
#include "unionRecFile.h"
#include "unionTableDef.h"

int main(int argc,char *argv[])
{
	int	ret;
	int	index = 0;
	char	*ptr;
	char	objectName[128+1];
	char	fileName[1024+1];
	int     len;

/*
	if ((ret = UnionConnectDatabase()) < 0)
	{
		printf("UnionConnectDatabase failure! ret = [%d]\n",ret);
		return(ret);
	}
*/
	//链接TableDefTBL共享内存
	if ((ret = UnionReloadTableDefTBL()) < 0)
	{
		printf("outputRecOfTBL :: UnionConnectTableDefTBL failure! ret = [%d]\n",ret);
		return(0);
	}	
	if (argc < 2)
		goto loop1;

	UnionSetIsOutPutInPutDataTools(1);

loop0:
	if (++index >= argc)
	{
		UnionCloseDatabase();
		return(0);
	}
	strcpy(objectName,argv[index]);
	sprintf(fileName,"%s/%s.txt",getenv("UNIONTEMP"),objectName);

	if ((ret = UnionOutputAllRecFromSpecTBL(objectName,"",fileName)) < 0)
	{
		printf("UnionOutputAllRecFromSpecTBL [%s] failure! ret = [%d]\n",objectName,ret);
	}
	else
	{
		printf("UnionOutputAllRecFromSpecTBL [%s] OK! totalRecNum = [%d]\n",objectName,ret);
	}
	goto loop0;
	
loop1:
	ptr = UnionInput("\n\n请输入对象的名称(exit退出)::");
	strcpy(objectName,ptr);
	if (UnionIsQuit(ptr))
	{
		UnionCloseDatabase();
		return(errCodeUserSelectExit);
	}
	sprintf(fileName,"%s/%s.txt",getenv("UNIONTEMP"),objectName);
	
	if ((ret = UnionOutputAllRecFromSpecTBL(objectName,"",fileName)) < 0)
	{
		printf("UnionOutputAllRecFromSpecTBL [%s] failure! ret = [%d]\n",objectName,ret);
	}
	else
	{
		printf("UnionOutputAllRecFromSpecTBL [%s] OK! totalRecNum = [%d]\n",objectName,ret);
	}

	goto loop1;
}

