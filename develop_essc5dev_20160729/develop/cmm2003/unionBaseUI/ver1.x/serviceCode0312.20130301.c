//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-02-22

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <math.h>

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionREC.h"
#include "UnionTask.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "errCodeTranslater.h"
#include "highCached.h"
#include "unionVersion.h"
#include "clusterSyn.h"
#include "baseUIService.h"

/*********************************
服务代码:	0312
服务名:		导入表数据
功能描述:	导入表数据
**********************************/
int UnionPutTableDataToDB(char *tableName,char *fileName,int *successNum, int *failNum);
int UnionDealServiceCode0312(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret = 0;
	char	tmpBuf[128+1];
	int	successNum = 0;
	int	failNum = 0;
	char	fileName[64+1];
	//int	i  = 0, j = 0,len = 0;
	int	len = 0;
	char	tableName[40+1];
	
	//TUnionClusterDefTBL	clusterDefTBL;
	//TUnionClusterSyn	clusterSyn;

	// 文件名
	memset(fileName,0,sizeof(fileName));
	if ((ret = UnionReadRequestXMLPackageValue("body/fileName",fileName,sizeof(fileName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0312:: UnionReadRequestXMLPackageValue[%s]!\n","body/fileName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(fileName);
	if (strlen(fileName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0312:: fileName is null!\n");
		return(errCodeParameter);
	}

	// 读取表名
	memset(tableName,0,sizeof(tableName));
	if ((ret = UnionReadFldFromBuf(fileName,'-',1,tableName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0312:: UnionReadFldFromBuf fileName[%s]!\n",fileName);
		return(errCodeParameter);
	}

	if ((ret = UnionPutTableDataToDB(tableName,fileName,&successNum,&failNum)) < 0)	
	{
		UnionUserErrLog("in UnionDealServiceCode0312:: UnionPutTableDataToDB failed tableName[%s]!\n",tableName);
		UnionSetResponseRemark("导入数据失败");
		return(errCodeParameter);
	}

	// 更新共享内存
	/*memset(&clusterDefTBL,0,sizeof(clusterDefTBL));
	if ((ret = UnionInitClusterDef(&clusterDefTBL)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0312:: UnionInitClusterDef!\n");
		return(ret);
	}

	for (i = 0; i < clusterDefTBL.realNum; i++)
	{
		if (strcasecmp(fileName,clusterDefTBL.rec[i].tableName) == 0)
		{
			for (j = 0; j < UnionReadIntTypeRECVar("numOfCluster"); j++)
			{
				memset(&clusterSyn,0,sizeof(clusterSyn));
				clusterSyn.clusterNo = j + 1;
				strcpy(clusterSyn.cmd,clusterDefTBL.rec[i].cmd);
				UnionGetFullSystemDateTime(clusterSyn.regTime);
				if ((ret = UnionInsertClusterSyn(&clusterSyn)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCode0312:: UnionInsertClusterSyn!\n");
					return(ret);
				}
			}
		}
	}
	*/
	// 显示报文体
	if ((ret = UnionSetResponseXMLPackageValue("head/displayBody","1")) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0312:: UnionSetResponseXMLPackageValue[%s]!\n","head/displayBody");	
		return(ret);
	}

	// 设置字段清单
	len = sprintf(tmpBuf,"tableName,successNum,failNum");
	tmpBuf[len] = 0;
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldList",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0312:: UnionSetResponseXMLPackageValue[%s]!\n","body/fieldList");	
		return(ret);
	}

	//　设置字段清单中文名
	len = sprintf(tmpBuf,"表名,成功数,失败数");
	tmpBuf[len] = 0;
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldListChnName",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0312:: UnionSetResponseXMLPackageValue[%s]!\n","body/fieldListChnName");
		return(ret);
	}

	// 设置总数
	if ((ret = UnionSetResponseXMLPackageValue("body/totalNum","1")) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0312:: UnionSetResponseXMLPackageValue[%s]!\n","body/totalNum");
		return(ret);
	}

	if ((ret = UnionLocateResponseNewXMLPackage("body/detail",1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0312:: UnionLocateResponseNewXMLPackage[%s]!\n","body/detail");
		return(ret);
	}

	// 设置表名
	if ((ret = UnionSetResponseXMLPackageValue("tableName",fileName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0312:: UnionSetResponseXMLPackageValue[%s]!\n","tableName");
		return(ret);
	}

	// 设置成功数
	ret = sprintf(tmpBuf,"%d",successNum);
	tmpBuf[ret] = 0;
	if ((ret = UnionSetResponseXMLPackageValue("successNum",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0312:: UnionSetResponseXMLPackageValue[%s]!\n","successNum");
		return(ret);
	}

	// 设置失败数
	ret = sprintf(tmpBuf,"%d",failNum);
	tmpBuf[ret] = 0;
	if ((ret = UnionSetResponseXMLPackageValue("failNum",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0312:: UnionSetResponseXMLPackageValue[%s]!\n","failNum");
		return(ret);
	}

	return(0);
}

int UnionPutTableDataToDB(char *tableName,char *fileName,int *successNum, int *failNum)
{
	int	i,j;
	int	ret;
	int	iCnt;
	int	lenOfFiled = 0;
	int	lenOfValue = 0;
	int	sNum = 0;
	int	fNum = 0;
	int	fieldNum = 0;
	char	fieldList[2048+1];
	char	valueList[409600+1];
	char	value[8192+1];
	char	tmpFileDir[256+1];
	char	sql[819200+1];
	char	fieldGrp[128][128];
	char	fileFullName[512+1];
	char	*ptr = NULL;
	char	fileDir[256+1];
	char	tmpBuf[8192+1];
	
	PUnionTableDef	ptableDef = NULL;

	if ((ptr = UnionReadStringTypeRECVar("defaultDirOfFileReceiver")) == NULL)
	{
		UnionUserErrLog("in UnionPutTableDataToDB:: UnionReadStringTypeRECVar[%s] found!\n","defaultDirOfFileReceiver");
		return(errCodeRECMDL_VarNotExists);
	}
	memset(fileDir,0,sizeof(fileDir));
	memset(tmpFileDir,0,sizeof(tmpFileDir));
	sprintf(tmpFileDir,"%s",ptr);
	if ((ret = UnionReadDirFromStr(tmpFileDir,-1,fileDir)) < 0)
	{
		UnionUserErrLog("in UnionPutTableDataToDB:: UnionReadDirFromStr[%s] error!\n",tmpFileDir);
		UnionSetResponseRemark("获取文件路径失败");
		return(errCodeParameter);
	}
	ret = sprintf(fileFullName,"%s/%s",fileDir,fileName);
	fileFullName[ret] = 0;

	if ((ptableDef = UnionFindTableDef(tableName)) == NULL)
	{
		UnionUserErrLog("in UnionPutTableDataToDB:: UnionFindTableDef[%s] failure!\n",tableName);
		UnionSetResponseRemark("查找表信息失败");
		return(errCodeParameter);
	}
	
	memset(fieldList,0,sizeof(fieldList));
	if ((ret = UnionReadTableFieldListOfTableName(tableName,0,fieldList)) < 0)
	{
		UnionUserErrLog("in UnionPutTableDataToDB:: UnionReadTableFieldListOfTableName[%s] failure!\n",tableName);
		return(ret);
	}

	if ((fieldNum = UnionSeprateVarStrIntoVarGrp(fieldList, strlen(fieldList),',',fieldGrp,128)) < 0)
	{
		UnionUserErrLog("in UnionPutTableDataToDB:: UnionSeprateVarStrIntoVarGrp[%s] failure!\n",fieldList);
		return(fieldNum);
	}
	
	if (access(fileFullName,0) != 0)
	{
		UnionUserErrLog("in UnionPutTableDataToDB:: fileFullName[%s] not found!\n",fileFullName);
		UnionSetResponseRemark("文件[%s.xml]不存在",tableName);
		return(errCodeParameter);
	}

	// 初始化XML文件
	if ((ret = UnionInitXMLPackage(fileFullName,NULL,0)) < 0)
	{
		UnionUserErrLog("in UnionPutTableDataToDB:: UnionInitXMLPackage[%s] failure!\n",fileFullName);
		return(ret);
	}
	
	// 读取总数量
	if ((ret = UnionReadXMLPackageValue("body/totalNum", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionPutTableDataToDB:: UnionReadXMLPackageValue[%s]!\n","body/totalNum");
		return(ret);
	}
	tmpBuf[ret] = 0;
	iCnt = atoi(tmpBuf);
	
	sNum = 0;
	fNum = 0;
	for(i = 0; i < iCnt; i++)
	{
		if ((ret = UnionLocateXMLPackage("body/detail", i+1)) < 0)
		{
			UnionUserErrLog("in UnionPutTableDataToDB:: UnionLocateXMLPackage [%s]!\n","body/detail");
			failNum ++;
			continue;
		}
		
		lenOfFiled = 0;
		lenOfValue = 0;
		for (j = 0; j < fieldNum; j++)
		{
			if ((ret = UnionReadXMLPackageValue(fieldGrp[j], tmpBuf, sizeof(tmpBuf))) < 0)
			{
				continue;
			}
			tmpBuf[ret] = 0;
			
			memset(value,0,sizeof(value));
			UnionTranslateSpecCharInDBStr(tmpBuf,strlen(tmpBuf),value,sizeof(value));

			switch(ptableDef->fieldDef[j].fieldType)
			{
				case 1 :	// 字符串
				case 4 :	// 二进制
				case 7 :	// 时间
					lenOfFiled += sprintf(fieldList + lenOfFiled,"%s,",fieldGrp[j]);
					lenOfValue += sprintf(valueList + lenOfValue,"'%s',",value);
					break;
				default:
					if (strlen(value) > 0)
					{
						lenOfFiled += sprintf(fieldList + lenOfFiled,"%s,",fieldGrp[j]);
						lenOfValue += sprintf(valueList + lenOfValue,"%s,",value);
					}
					else
					{
						lenOfFiled += sprintf(fieldList + lenOfFiled,"%s,",fieldGrp[j]);
						lenOfValue += sprintf(valueList + lenOfValue,"null,");
					}
					break;
			}
		}
		
		fieldList[lenOfFiled - 1] = 0;
		valueList[lenOfValue - 1] = 0;
		
		ret = sprintf(sql,"insert into %s(%s) values(%s)",tableName,fieldList,valueList);
		sql[ret] = 0;
		
		//UnionUserErrLog("in UnionPutTableDataToDB:: UnionExecRealDBSql[%s]!\n",sql);
		if ((ret = UnionExecRealDBSql(sql)) < 0)
		{
			//UnionUserErrLog("in UnionPutTableDataToDB:: UnionExecRealDBSql[%s]!\n",sql);
			fNum ++;
			continue;
		}
		sNum ++;
	}
	*successNum = sNum;
	*failNum = fNum;

	if (unlink(fileFullName) < 0)
	{
		UnionUserErrLog("in UnionPutTableDataToDB:: unlink[%s] is failed!\n",fileFullName);
	}
	return(iCnt);
}
