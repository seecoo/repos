#define _UnionLogMDL_3_x_	
#define _UnionEnv_3_x_	
#define _realBaseDB_2_x_

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "UnionLog.h"
#include "unionErrCode.h"
#include "UnionStr.h"

#include "unionRealBaseDB.h"
#include "unionXMLPackage.h"
#include "unionRealDBCommon.h"

int UnionTaskActionBeforeExit()
{
	exit(1);
}

int callback(void *arg ,int nCount,char** pValue,char** pName)
{
	int	i;
	int	*iCnt = (int *)arg;
	
	for(i = 0;i < nCount; i++)
	{
		if (*iCnt == 1)
			printf("%s:%s\n",pName[i],pValue[i]);
	}
	return 0;
}

int main(int argc,char *argv[])
{
	int		i;
	int		ret = 0;
	int 		iCnt = 1;
	int		processes = 1;
	pid_t		pid;
	char		option[10+1];
	char		tmpBuf[8192*10+1];
	char		sql[1024+1];;
	int nodeID = 1000001;

	if (argc < 5)
	{
		printf("usage:: testdb [processes] [select|select2|exec|exec2] [sql] [count]\n");
		return(0);
	}
	
	processes = atoi(argv[1]);
	strcpy(option,argv[2]);
	strcpy(sql,argv[3]);
	iCnt = atoi(argv[4]);
	
	for (i = 0; i < processes; i ++)
	{
		pid = fork() ;
		if( pid < 0 )
		{
			printf( "fork failed\n" );
			return -1;
		}
		else if( pid == 0 )
			goto cliPro;
		else
			continue;
	}
	
	for (i = 0; i < processes; i ++)
	{
		wait(NULL);
	}
	return(0);

cliPro:	
	for (i = 0; i < iCnt; i ++)
	{
		if (strcasecmp(option,"select") == 0)
		{
			ret = UnionSelectRealDBRecord(sql,0,0);
			if (iCnt == 1)
			{
				UnionXMLPackageToBuf(tmpBuf,sizeof(tmpBuf));
				printf("%s\n",tmpBuf);
				UnionLogXMLPackage();
			}
		}
		else if (strcasecmp(option,"select2") == 0)
		{
			ret = UnionSelectRealDBRecord2(sql,0,0,callback,(void *)&iCnt);
		}
		else if(strcasecmp(option,"exec") == 0)
		{
			ret = UnionExecRealDBSql(sql);
		}
			
		if (ret < 0)
		{
			printf("err ret = [%d] i =[%d]!\n",ret,i);
			break;
		}
		nodeID++;
	}
	if (ret > 0)
		printf("ok iCnt[%d] ret = [%d]\n",iCnt,ret);
	
	UnionCloseDatabase();
	UnionFreeXMLPackage();
	UnionDestroyILOG();
	
	return(0);
}
