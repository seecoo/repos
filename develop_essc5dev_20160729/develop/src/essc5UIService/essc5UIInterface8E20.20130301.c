//      Author:         张永定
//      Copyright:      Union Tech. Guangzhou
//      Date:           2013-01-10

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "essc5UIService.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "symmetricKeyDB.h"
#include "unionHsmCmd.h"
#include "remoteKeyPlatform.h"

#define maxGrpNum 20
#define maxCommitNum 30

/***************************************
服务代码:       8E20
服务名:		导入密钥节点
功能描述:	导入密钥节点
***************************************/
static int batchInsertKeyNodeToDB(char *sql,char nodeIDGrp[][72],int count,int *failedNum,int *succeedNum);
int UnionDealServiceCode8E20(PUnionHsmGroupRec phsmGroupRec)
{
	int			len = 0;	
	char            	sql[8192];
	int                     ret = 0;
	char            	fileStoreDir[512];
        char            	fileDir[512];
	int 			columns = 0;
	char                    delimiter[32];
	char			example[256];
	FILE                    *fp;
	char			parameterName[maxGrpNum][128];
	char			parameterValue[maxGrpNum][128];
	char                    tmpBuf[512];
	int			nodeIDNo = 0;
	int                     totalNum = 0;
	int			succeedNum = 0;
	int			failedNum = 0;
	int			statusNo = 0;
	int			i = 0;
        char                    fieldList[32];
        char                    fieldListChnName[32];
	char			fieldNameList[64];
	char			fieldValueList[256];
	char                    fileFullName[512];
	char                    fileName[128];
	int			count = 0;
	int			lenOfSql = 0;
	char			nodeIDGrp[maxCommitNum][72];
	
        snprintf(sql,sizeof(sql),"select * from fileType where fileTypeID = 'NODE'");
        if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E20:: UnionSelectRealDBRecord[%s]!\n",sql);
                return(ret);
        }
        else if (ret == 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E20:: fileTypeID[NODE]!\n");
                return(errCodeDatabaseMDL_RecordNotFound);
        }

        if ((ret = UnionLocateXMLPackage("detail",1)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E20:: UnionLocateXMLPackage[%s]!\n","detail");
                return(ret);
        }
		
	// 读取文件存储目录
        if ((ret = UnionReadXMLPackageValue("fileStoreDir",fileStoreDir,sizeof(fileStoreDir))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E20:: UnionReadXMLPackageValue[%s]!\n","fileStoreDir");
                return(ret);
        }
	fileStoreDir[ret] = 0;

        memset(fileDir,0,sizeof(fileDir));
        UnionReadDirFromStr(fileStoreDir,-1,fileDir);
	
	// 读取分隔符
        if ((ret = UnionReadXMLPackageValue("delimiter",delimiter,sizeof(delimiter))) < 0)
        {
		UnionUserErrLog("in UnionDealServiceCode8E20:: UnionReadXMLPackageValue[%s]!\n","delimiter");
		return(ret);
	}
	delimiter[ret] = 0;

	// 读取示例
        if ((ret = UnionReadXMLPackageValue("example",example,sizeof(example))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E20:: UnionReadXMLPackageValue[%s]!\n","example");
                return(ret);
        }
	UnionFilterHeadAndTailBlank(example);

	//modify by leipp 20151120	之前只能支持100条以内的文件，现重新修整
	// 拼分域定义串
        if ((columns = UnionSeprateVarStrIntoArray(example,strlen(example),delimiter[0],(char *)&parameterName,maxGrpNum,128)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E20:: UnionSeprateVarStrIntoArray[%s]!\n",example);
                return(columns);
        }

	snprintf(fieldNameList,sizeof(fieldNameList),"%s,%s,%s,%s,%s",parameterName[0],parameterName[1],parameterName[2],parameterName[3],parameterName[4]);

	for (i = 0; i < columns;i++)
	{
		if (strcmp(parameterName[i],"nodeID") == 0)
		{
			nodeIDNo = i;
		}
		else if (strcmp(parameterName[i],"status") == 0)
		{
			statusNo = i;
		}
	}

	// 文件名
        if ((ret = UnionReadRequestXMLPackageValue("body/fileName",fileName,sizeof(fileName))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E18:: UnionReadRequestXMLPackageValue[%s]!\n","body/fileName");
                return(ret);
        }
	fileName[ret] = 0;	
	
	snprintf(fileFullName,sizeof(fileFullName),"%s/%s",fileDir,fileName);
	if ((fp = fopen(fileFullName,"r")) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode8E18:: fopen [%s] is failed!\n",fileFullName);
		return(errCodeParameter);
	}
			
	// 设置显示报文体
        if ((ret = UnionSetResponseXMLPackageValue("head/displayBody","1")) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E20:: UnionSetResponseXMLPackageValue[%s]\n","head/displayBody");
                return(ret);
        }

        // 字段清单
        snprintf(fieldList,sizeof(fieldList),"%s","nodeID,status");
        if ((ret = UnionSetResponseXMLPackageValue("body/fieldList",fieldList)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E20:: UnionSetResponseXMLPackageValue[%s]\n","body/fieldList");
                return(ret);
        }

        // 字段清单中文名
	snprintf(fieldListChnName,sizeof(fieldListChnName),"%s","节点ID,状态");
        if ((ret = UnionSetResponseXMLPackageValue("body/fieldListChnName",fieldListChnName)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E20:: UnionSetResponseXMLPackageValue[%s]\n","body/fieldListChnName");
                return(ret);
        }

        // 设置总数
        if ((ret = UnionSetResponseXMLPackageValue("body/totalNum","")) < 0)
        {
		UnionUserErrLog("in UnionDealServiceCode8E20:: UnionSetResponseXMLPackageValue[%s]\n","body/totalNum");
		return(ret);
        }

	while(!feof(fp))
        {
		if ((ret = UnionReadOneDataLineFromTxtFile(fp,tmpBuf,sizeof(tmpBuf))) < 0)
                {
                        if (errCodeFileEnd == ret)
			{
				break;
			}

                        UnionUserErrLog("in UnionDealServiceCode8E20:: UnionReadOneLineFromTxtFile [%s] ret = [%d]!\n",fileName,ret);
                        fclose(fp);
                        return(ret);
		}

		// 拼分域定义串
		if ((ret = UnionSeprateVarStrIntoArray(tmpBuf,strlen(tmpBuf),delimiter[0],(char *)&parameterValue,maxGrpNum,128)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E20:: UnionSeprateVarStrIntoArray[%s]!\n",tmpBuf);
			continue;
		}
		else if (ret < columns)
		{
			UnionUserErrLog("in UnionDealServiceCode8E20:: real column[%d] < expect column[%d]!\n",ret,columns);
			continue;
		}
		
		// 因为status是整型,不能带单引号,所以需要找到,单独设置
		len = 0;
		for (i = 0; i < columns; i++)
		{
			UnionFilterHeadAndTailBlank(parameterValue[i]);
			if (i == nodeIDNo)
				snprintf(nodeIDGrp[count],sizeof(nodeIDGrp[count]),"%s",parameterValue[i]);
			if (i == statusNo)
				len += snprintf(fieldValueList+len,sizeof(fieldValueList)-len,"%s,",parameterValue[i]);
			else
				len += snprintf(fieldValueList+len,sizeof(fieldValueList)-len,"'%s',",parameterValue[i]);
		}
		fieldValueList[len-1] = 0;

		lenOfSql += snprintf(sql+lenOfSql,sizeof(sql)-lenOfSql,"insert into keyNode(%s) values(%s);",fieldNameList,fieldValueList);
		count++;
		if (count < maxCommitNum)
			continue;

		if ((ret = batchInsertKeyNodeToDB(sql,nodeIDGrp,count,&failedNum,&succeedNum)) < 0)
		{
                        UnionUserErrLog("in UnionDealServiceCode8E20:: batchInsertKeyNodeToDB sql[%s]!\n",sql);
			return(ret);
		}

		count = 0;
		lenOfSql = 0;
	}
	if (count > 0)
	{
		if ((ret = batchInsertKeyNodeToDB(sql,nodeIDGrp,count,&failedNum,&succeedNum)) < 0)
		{
                        UnionUserErrLog("in UnionDealServiceCode8E20:: batchInsertKeyNodeToDB sql[%s]!\n",sql);
			return(ret);
		}
	}

	totalNum = succeedNum + failedNum;
	UnionLocateResponseXMLPackage("",0);

	//赋值总记录数
	snprintf(tmpBuf,sizeof(tmpBuf),"%d",failedNum+3);
	if ((ret = UnionSetResponseXMLPackageValue("body/totalNum",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E20:: UnionSetResponseXMLPackageValue[body/totalNum]!\n");
		return(ret);
	}	

	UnionLocateResponseNewXMLPackage("body/detail",1);
	snprintf(tmpBuf,sizeof(tmpBuf),"%d",failedNum);
	UnionSetResponseXMLPackageValue("nodeID","失败数");
	UnionSetResponseXMLPackageValue("status",tmpBuf);

	UnionLocateResponseNewXMLPackage("body/detail",2);
	snprintf(tmpBuf,sizeof(tmpBuf),"%d",succeedNum);
	UnionSetResponseXMLPackageValue("nodeID","成功数");
	UnionSetResponseXMLPackageValue("status",tmpBuf);

	UnionLocateResponseNewXMLPackage("body/detail",3);
	UnionSetResponseXMLPackageValue("nodeID","总数");
	snprintf(tmpBuf,sizeof(tmpBuf),"%d",totalNum);
	UnionSetResponseXMLPackageValue("status",tmpBuf);
        return(0);
	// modify end by leipp 20151120
}

static int batchInsertKeyNodeToDB(char *sql,char nodeIDGrp[][72],int count,int *failedNum,int *succeedNum)
{
	int	ret = 0;
	int	status = 0;
	int	j=0;

	if ((ret = UnionExecRealDBSql(sql)) <= 0)
	{
		UnionUserErrLog("in batchInsertKeyNodeToDB:: UnionExecRealDBSql sql[%s]!\n",sql);
		status = 0;
	}
	else
		status = 1;

	if (status == 0)
	{
		for (j = 0; j < count; j++)
		{
			(*failedNum)++;
			if ((ret = UnionLocateResponseNewXMLPackage("body/detail",*failedNum+3)) < 0)
			{
				UnionUserErrLog("in batchInsertKeyNodeToDB:: UnionSetResponseXMLPackageValue[body/detail]\n");
				return(ret);
			}

			// 节点ID
			if ((ret = UnionSetResponseXMLPackageValue("nodeID",nodeIDGrp[j])) < 0)
			{
				UnionUserErrLog("in batchInsertKeyNodeToDB:: UnionSetResponseXMLPackageValue[nodeID]\n");
				return(ret);
			}

			if ((ret = UnionSetResponseXMLPackageValue("status","失败")) < 0)
			{
				UnionUserErrLog("in batchInsertKeyNodeToDB:: UnionSetResponseXMLPackageValue[status]\n");
				return(ret);
			}
		}
	}
	else
	{
		*succeedNum += count;
	}

	return 0;
}
