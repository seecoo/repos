//	张永定
//	2012-12-27

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include "unionCommand.h"
#include "unionErrCode.h"
#include "UnionStr.h"
#include "unionRealDBCommon.h"
#include "unionRealBaseDB.h"
#include "unionTableDef.h"
#include "inputAndOutputDataForDB.h"

int UnionTaskActionBeforeExit()
{
        //UnionCloseDatabase();	//modify by leipp 20150923
        exit(0);
}


int main(int argc,char *argv[])
{
	int	ret;
	int	index = 0;
	char	*ptr;
	char	tableName[128+1];
	
	if (argc < 2)
		goto loop1;

	if ((ret = UnionReloadTableDefTBL()) < 0)
	{
		printf("UnionReloadTableDefTBL failure! ret = [%d]\n",ret);
		return(0);
	}

loop0:
	if (++index >= argc)
	{
		UnionCloseDatabase();
		return(0);
	}
	memset(tableName,0,sizeof(tableName));
	strcpy(tableName,argv[index]);
	if ((ret = UnionOutputOneTableDataFromDB(tableName)) < 0)
		printf("导出表数据 [表名：%20s] 失败! ret = [%d]\n",tableName,ret);
	else
		printf("导出表数据 [表名：%20s] 成功 记录数[%8d]!\n",tableName,ret);
	goto loop0;
	
loop1:
	ptr = UnionInput("\n\n请输入数据库表的名称(exit退出)::");
	if (UnionIsQuit(ptr))
	{
		UnionCloseDatabase();
		return(errCodeUserSelectExit);
	}
	strcpy(tableName,ptr);
	if ((ret = UnionOutputOneTableDataFromDB(tableName)) < 0)
		printf("导入表数据 [表名：%20s] 失败! ret = [%d]\n",tableName,ret);
	else
		printf("导出表数据 [表名：%20s] 成功 记录数[%8d]!\n",tableName,ret);
	goto loop1;
}

