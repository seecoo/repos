//	张永定
//	2012-12-27

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

int main(int argc,char *argv[])
{
	int	ret;
	int	successNum = 0;
	int	failNum = 0;
	char	*ptr;
	char	tableName[128+1];
	char	prefix[16+1];

	if ((ret = UnionConnectDatabase()) < 0)
	{
		printf("UnionConnectDatabase failure! ret = [%d]\n",ret);
		return(ret);
	}

	if (argc < 3)
		goto loop1;

	if ((ret = UnionReloadTableDefTBL()) < 0)
	{
		printf("UnionReloadTableDefTBL failure! ret = [%d]\n",ret);
		return(0);
	}
		
	/*if (++index >= argc)
	{
		UnionCloseDatabase();
		return(0);
	}*/
	memset(tableName,0,sizeof(tableName));
	strcpy(tableName,argv[1]);
	memset(prefix,0,sizeof(prefix));
	strcpy(prefix,argv[2]);
	if ((ret = UnionInputOneTableDataToDB_oldVersion(tableName,prefix,&successNum,&failNum)) < 0)
	{
		printf("导入表数据 [表名：%s] 失败! ret = [%d]\n",tableName,ret);
	}
	else
		printf("导入表数据[%20s] 总记录数[%5d] 成功记录数[%5d] 失败记录数[%5d]\n",tableName,ret,successNum,failNum);
	
	UnionCloseDatabase();
	return(0);
loop1:
	ptr = UnionInput("\n\n请输入对象的名称(exit退出)::");
	memset(tableName,0,sizeof(tableName));
	strcpy(tableName,ptr);
	if (UnionIsQuit(ptr))
	{
		UnionCloseDatabase();
		return(errCodeUserSelectExit);
	}
	ptr = UnionInput("\n\n请输入表字段的前缀(exit退出)::");
	memset(prefix,0,sizeof(prefix));
	strcpy(prefix,ptr);
	if (UnionIsQuit(ptr))
	{
		UnionCloseDatabase();
		return(errCodeUserSelectExit);
	}
	if ((ret = UnionInputOneTableDataToDB_oldVersion(tableName,prefix,&successNum,&failNum)) < 0)
	{
		printf("导入表数据 [表名：%s] 失败! ret = [%d]\n",tableName,ret);
	}
	else
		printf("导入表数据[%20s] 总记录数[%5d] 成功记录数[%5d] 失败记录数[%5d]\n",tableName,ret,successNum,failNum);
	goto loop1;
}

