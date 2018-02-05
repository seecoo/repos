//	Author:		Wolfgang Wang
//	Date:		2001/08/29
//	Version:	2.0

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif
#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "UnionLog.h"
#include "unionCommand.h"
#include "unionVersion.h"
#include "UnionTask.h"

#include "unionTableData.h"

PUnionTaskInstance	ptaskInstance = NULL;

int UnionHelp()
{
	printf("Usage:: %s tableName command\n",UnionGetApplicationName());
	printf(" Where command as:\n");
	printf("  -reload 		reload the tableDataTBL\n");
	printf("  -all			print all tableDataTBL definition\n");
	return(0);
}

int UnionTaskActionBeforeExit()
{
	//UnionDisconnectTableDataTBL();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

int main(int argc,char *argv[])
{
	int	ret;
	char	tableName[32+1];
	char	tmpBuf[128+1];
	char	remark[128+1];
		
	UnionSetApplicationName(argv[0]);
		
	if (argc < 2)
		return(UnionHelp());

	memset(tableName,0,sizeof(tableName));
	strcpy(tableName,argv[1]);	
	
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);

	if (argc < 3)
		return(UnionHelp());
	
	memset(remark,0,sizeof(remark));
	sprintf(remark,"加载共享内存[%s]",tableName);
	
	if (strcasecmp(argv[2],"-RELOAD") == 0)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"确定加载数据库表[%s]吗?",tableName);
		
		if (!UnionConfirm(tmpBuf))
			return(-1);
		if ((ret = UnionReloadTableDataTBL(tableName)) < 0)
			printf("***** %-30s Error!\n",remark);
		else
			printf("***** %-30s OK!\n",remark);
	}
	else if (strcasecmp(argv[2],"-RELOADANYWAY") == 0)
	{
		if ((ret = UnionReloadTableDataTBL(tableName)) < 0)
			printf("***** %-30s Error!\n",remark);
		else
			printf("***** %-30s OK!\n",remark);
	}
	else if (strcasecmp(argv[2],"-ALL") == 0)
		UnionPrintTableDataTBLToFile(tableName,stdout);
	else 
		return(UnionHelp());		
	
	return(0);
}


