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
#include "unionREC.h"
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
	int	successNum = 0;
	int	failNum = 0;
	char	*ptr;
	char	tableName[128+1];

	if (argc < 2)
		goto loop1;

	if ((ret = UnionReloadTableDefTBL()) < 0)
	{
		printf("UnionReloadTableDefTBL failure! ret = [%d]\n",ret);
		return(0);
	}

	UnionConnectREC(); // 2015-06-26 以防第一次读rec表冲掉要导入的表数据
		
loop0:
	if (++index >= argc)
	{
		UnionCloseDatabase();
		return(0);
	}
	memset(tableName,0,sizeof(tableName));
	strcpy(tableName,argv[index]);
	if ((ret = UnionInputOneTableDataToDB(tableName,&successNum,&failNum)) < 0)
	{
		printf("导入表数据 [表名：%20s] 失败! ret = [%d]\n",tableName,ret);
	}
	else
		printf("导入表数据 [表名：%20s] 总记录数[%5d] 成功记录数[%5d] 失败记录数[%5d]\n",tableName,ret,successNum,failNum);
	goto loop0;
	
loop1:
	ptr = UnionInput("\n\n请输入对象的名称(exit退出)::");
	memset(tableName,0,sizeof(tableName));
	strcpy(tableName,ptr);
	if (UnionIsQuit(ptr))
	{
		UnionCloseDatabase();
		return(errCodeUserSelectExit);
	}
	if ((ret = UnionInputOneTableDataToDB(tableName,&successNum,&failNum)) < 0)
	{
		printf("导入表数据 [表名：%20s] 失败! ret = [%d]\n",tableName,ret);
	}
	else
		printf("导入表数据 [表名：%20s] 总记录数[%5d] 成功记录数[%5d] 失败记录数[%5d]\n",tableName,ret,successNum,failNum);
	goto loop1;
}

