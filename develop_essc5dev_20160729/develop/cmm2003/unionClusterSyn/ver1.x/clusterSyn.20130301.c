#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#ifndef _UnionEnv_3_x_
#define _UnionEnv_3_x_	
#endif

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include "UnionLog.h"
#include "UnionEnv.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionREC.h"

#include "clusterSyn.h"
#include "unionRealBaseDB.h"
#include "unionXMLPackage.h"
#include "unionVersion.h"

int UnionInitClusterDef(PUnionClusterDefTBL pdef)
{
	int		i;
	int		ret;
	char		fileName[512];
	char		*p;

	if (pdef == NULL)
		return(errCodeParameter);
	
	snprintf(fileName,sizeof(fileName),"%s/%s",getenv("UNIONETC"),conConfFileNameOfClusterDef);
	
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionInitClusterDef:: UnionInitEnvi [%s]!\n",fileName);
		return(ret);
	}

	if ((p = UnionGetEnviVarByName("clusterNo")) == NULL)
	{
		UnionUserErrLog("in UnionInitClusterDef:: UnionGetEnviVarByName for [%s]\n!","clusterNo");
		UnionClearEnvi();
		return(ret);
	}
	pdef->clusterNo = atoi(p);

	for (i = 0; (i < UnionGetEnviVarNum()) && (pdef->realNum < defMaxNumOfSynContent); i++)
	{
		// 读取表名
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,0)) == NULL)
		{
			UnionUserErrLog("in UnionInitClusterDef:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,0);
			continue;
		}
		strcpy(pdef->rec[i].tableName,p);
		
		// 读取命令
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,1)) == NULL)
		{
			UnionUserErrLog("in UnionInitClusterDef:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,1);
			continue;
		}
		snprintf(pdef->rec[i].cmd,sizeof(pdef->rec[i].cmd),"%s %s",p,"-reloadanyway");		
		++pdef->realNum;
	}
	UnionClearEnvi();
	return(0);
}

int UnionReadClusterSyn(int clusterNo,TUnionClusterSyn clusterSyn[],int maxNum)
{
	int	i;
	int	ret;
	int	count;
	char	sql[512];
	char	tmpBuf[32];
	
	snprintf(sql,sizeof(sql),"select * from clusterSyn where clusterNo = %d order by sn",clusterNo);

	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionReadClusterSyn:: UnionSelectRealDBRecord\n");
		return(ret);
	}
	else if (ret == 0)
		return(0);	

	if ((count = ret) > maxNum)
		count = maxNum;

	if (count > 0)
		UnionLogXMLPackage();
	
	for (i = 0; i < count; i++)
	{
		if ((ret = UnionLocateXMLPackage("detail", i+1)) < 0)
		{
			UnionUserErrLog("in UnionReadClusterSyn:: UnionLocateXMLPackage!\n");
			continue;
		}
		
		if ((ret = UnionReadXMLPackageValue("sn", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionReadClusterSyn:: UnionReadXMLPackageValue[sn]!\n");
			return(ret);
		}
		clusterSyn[i].sn = atoi(tmpBuf);
		
		if ((ret = UnionReadXMLPackageValue("clusterNo", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionReadClusterSyn:: UnionReadXMLPackageValue[sn]!\n");
			return(ret);
		}
		clusterSyn[i].clusterNo = atoi(tmpBuf);
		
		if ((ret = UnionReadXMLPackageValue("cmd", clusterSyn[i].cmd, sizeof(clusterSyn[i].cmd))) < 0)
		{
			UnionUserErrLog("in UnionReadClusterSyn:: UnionReadXMLPackageValue[cmd]!\n");
			return(ret);
		}
		if ((ret = UnionReadXMLPackageValue("regTime",clusterSyn[i].regTime, sizeof(clusterSyn[i].regTime))) < 0)
		{
			UnionUserErrLog("in UnionReadClusterSyn:: UnionReadXMLPackageValue[regTime]!\n");
			return(ret);
		}
	}
	return(count);
}

int UnionDeleteClusterSynBySN(int sn)
{
	int	ret;
	char	sql[512];
	
	snprintf(sql,sizeof(sql),"delete from clusterSyn where sn = %d",sn);
	
	if ((ret = UnionExecRealDBSql2(0,sql)) < 0)
	{
		UnionUserErrLog("in UnionDeleteClusterSynBySN:: UnionSelectRealDBRecord2\n");
		return(ret);
	}
	return(ret);
}

int UnionDeleteClusterSynByClusterNo(int clusterNo)
{
	int	ret;
	char	sql[512];
	
	snprintf(sql,sizeof(sql),"delete from clusterSyn where clusterNo = %d",clusterNo);
	
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDeleteClusterSynByClusterNo:: UnionSelectRealDBRecord\n");
		return(ret);
	}
	return(ret);
}

int UnionInsertClusterSyn(PUnionClusterSyn pclusterSyn)
{
	int	ret;
	char	sql[1024];
	int	idValue = 0;
	
	if ((strcmp(UnionGetDataBaseType(),"MYSQL") != 0) && (strcmp(UnionGetDataBaseType(),"SQLITE") != 0))
	{
		 while(1)
		{
			if ((idValue = UnionGetAddSelfNumBySequence("clusterSyn_id_seq")) < 0)
			{
				UnionUserErrLog("in UnionInsertClusterSyn:: UnionGetAddSelfNumBySequence[%s]!\n","clusterSyn_id_seq");
				return(idValue);
			}

			// 检查序号是否已经存在
			snprintf(sql,sizeof(sql),"select sn from clusterSyn where sn = %d",idValue);
			if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
			{
				UnionUserErrLog("in UnionInsertClusterSyn:: UnionSelectRealDBRecord[%s]!\n",sql);
				return(ret);
			}
			else if (ret > 0)
			{
				continue;
			}
			break;
		}

		snprintf(sql,sizeof(sql),"insert into clusterSyn (sn,clusterNo,cmd,regTime) values(%d,%d,'%s','%s')",
			idValue,
			pclusterSyn->clusterNo,
			pclusterSyn->cmd,
			pclusterSyn->regTime);
	}
	else
	{
		snprintf(sql,sizeof(sql),"insert into clusterSyn (sn,clusterNo,cmd,regTime) values(%s,%d,'%s','%s')",
			"NULL",
			pclusterSyn->clusterNo,
			pclusterSyn->cmd,
			pclusterSyn->regTime);
	}
	
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionInsertClusterSyn:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}

	return(0);
}

// 读取本机的集群编号
int UnionGetClusterNoOfLocalHost()
{
	int	ret;
	TUnionClusterDefTBL	clusterDefTBL;
	
	if ((ret = UnionInitClusterDef(&clusterDefTBL)) < 0)
	{
		UnionUserErrLog("in UnionGetClusterNoOfLocalHost:: UnionInitClusterDef!\n");
		return(ret);
	}
	return(clusterDefTBL.clusterNo);
}
//add by zhouxw 20150911
// 读取本机下指定密码机ID的进程是否运行
int UnionGetStatusOfHsmID(char	*hsmID)
{
	int		ret;
	char		fileName[512];
	char		*p;
	
	snprintf(fileName,sizeof(fileName),"%s/%s",getenv("UNIONETC"),conConfFileNameOfClusterDef);
	
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		if(ret == errCodeUseOSErrCode)
			return(1);
		else
		{
			UnionUserErrLog("in UnionInitClusterDef:: UnionInitEnvi [%s]!\n",fileName);
			return(ret);
		}
	}

	if ((p = UnionGetEnviVarByName(hsmID)) == NULL)
	{
		//UnionLog("in UnionInitClusterDef:: UnionGetEnviVarByName for [%s]\n!",hsmID);
		UnionClearEnvi();
		return(1);
	}
	ret = atoi(p);
	UnionClearEnvi();
	return(ret);
}
//end

// 根据集群编号判断是否允许这个任务
int UnionIsRunThisTaskByClusterNo()
{
	if (UnionGetClusterNoOfLocalHost() == 1)
		return(1);
	else
		return(0);
}

// 更新数据库表的共享内存
int UnionUpdateShareMemoryForUpdateDB(char *tableName)
{
	int	i,j;
	int	ret;
	TUnionClusterDefTBL	clusterDefTBL;
	TUnionClusterSyn	clusterSyn;
	
	memset(&clusterDefTBL, 0, sizeof(TUnionClusterDefTBL));
	if ((ret = UnionInitClusterDef(&clusterDefTBL)) < 0)
	{
		UnionUserErrLog("in UnionUpdateShareMemoryForUpdateDB:: UnionInitClusterDef!\n");
		return(ret);
	}
	
	for (i = 0; i < clusterDefTBL.realNum; i++)
	{
		if (strcasecmp(tableName,clusterDefTBL.rec[i].tableName) == 0)
		{
			for (j = 0; j < UnionReadIntTypeRECVar("numOfCluster"); j++)
			{
				memset(&clusterSyn,0,sizeof(clusterSyn));
				clusterSyn.clusterNo = j + 1;
				strcpy(clusterSyn.cmd,clusterDefTBL.rec[i].cmd);
				UnionGetFullSystemDateTime(clusterSyn.regTime);
				if ((ret = UnionInsertClusterSyn(&clusterSyn)) < 0)
				{
					UnionUserErrLog("in UnionUpdateShareMemoryForUpdateDB:: UnionInsertClusterSyn!\n");
					return(ret);
				}
			}
		}
	}
	return(0);
}

