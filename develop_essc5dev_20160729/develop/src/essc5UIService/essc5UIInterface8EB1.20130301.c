//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "baseUIService.h"

/***************************************
服务代码:	8EB1
服务名:		启用订单
功能描述:	启用订单
***************************************/

int UnionGetDateAfterSpecifiedDate1(char *startDate,int months,char *date)
{
	char	tmpBuf[32];
	int	year,month,day;
	
	if (months <= 0)
		return(errCodeParameter);
	
	memset(tmpBuf,0,sizeof(tmpBuf));	
	strcpy(tmpBuf,startDate);
	day = atoi(tmpBuf+6);
	tmpBuf[6] = 0;
	month = atoi(tmpBuf+4);
	tmpBuf[4] = 0;
	year = atoi(tmpBuf);
	
	month += months;
	year = year + (month-1) / 12;
	month = month % 12;
	if (month == 0)
		month = 12;
	if (day == 31)
	{
		switch (month)
		{
			case	2:
				if ((year % 4 == 0) && (year % 100 != 0))
					day = 29;
				else
					day = 28;
				break;
			case	4:
			case	6:
			case	9:
			case	11:
				day = 30;
				break;
			default:
				break;
		}
	}
	else if (day == 30 || day == 29)
	{
		switch (month)
		{
			case	2:
				if ((year % 4 == 0) && (year % 100 != 0))
					day = 29;
				else
					day = 28;
				break;
			default:
				break;
		}
	}
	sprintf(tmpBuf,"%04d%02d%02d",year,month,day);
	memcpy(date,tmpBuf,8);
	return(8);
}



int UnionDealServiceCode8EB1(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	int	len = 0;
	char	startDate[32];
	char	endDate[32];
	char	sql[128];
	char	orderTime[32];
	char	usingUnit[32];
	int	buyTimes = 0;
	char	tmpNum[32];
	
	// 获取当前日期
	memset(startDate,0,sizeof(startDate));
	UnionGetFullSystemDate(startDate);
	
	// 读取订单时间
	memset(orderTime,0,sizeof(orderTime));
	if ((ret = UnionReadRequestXMLPackageValue("body/orderTime",orderTime,sizeof(orderTime))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EB1:: UnionReadRequestXMLPackageValue[%s]!\n","body/orderTime");
		return(ret);
	}

	// 读取单位编号
	memset(usingUnit,0,sizeof(usingUnit));
	if ((ret = UnionReadRequestXMLPackageValue("body/usingUnit",usingUnit,sizeof(usingUnit))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EB1:: UnionReadRequestXMLPackageValue[%s]!\n","body/usingUnit");
		return(ret);
	}

	// 查询购买时长
	len = sprintf(sql,"select * from unitOrder where usingUnit = '%s' and orderTime = '%s'",usingUnit,orderTime);
	sql[len] = 0;
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EB1:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EB1:: usingUnit[%s] orderTime[%s]  unitOrder not found!\n",usingUnit,orderTime);
		return(errCodeDatabaseMDL_RecordNotFound);
	}

	if ((ret = UnionLocateXMLPackage("detail",1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EB1:: UnionLocateXMLPackage[%s]!\n","detail");
		return(ret);
	}

	// 获取订单状态
	memset(tmpNum,0,sizeof(tmpNum));
	if ((ret = UnionReadXMLPackageValue("status",tmpNum,sizeof(tmpNum))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EB1:: UnionReadXMLPackageValue[%s]!\n","status");
		return(ret);
	}

	if (atoi(tmpNum) != 3)
	{
		UnionUserErrLog("in UnionDealServiceCode8EB1:: status[%d] != 3!\n",atoi(tmpNum));
		UnionSetResponseRemark("只有未启用订单才可使用");
		return(errCodeParameter);
	}

	// 获取购买时长
	memset(tmpNum,0,sizeof(tmpNum));
	if ((ret = UnionReadXMLPackageValue("buyTimes",tmpNum,sizeof(tmpNum))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EB1:: UnionReadXMLPackageValue[%s]!\n","buyTimes");
		return(ret);
	}
	else
		buyTimes = atoi(tmpNum);


	// 检查订单最后时间
	len = sprintf(sql,"select * from unitOrder where usingUnit = '%s' and endDate is not NULL order by endDate desc",usingUnit);	
	sql[len] = 0;
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EB1:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}
	else
	{
		if ((ret = UnionLocateXMLPackage("detail",1)) < 0)	
		{
			UnionUserErrLog("in UnionDealServiceCode8EB1:: UnionLocateXMLPackage[%s]!\n","detail");
			return(ret);
		}

		memset(tmpNum,0,sizeof(tmpNum));
		if ((ret = UnionReadXMLPackageValue("endDate",tmpNum,sizeof(tmpNum))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8EB1:: UnionReadXMLPackageValue[%s]!\n","endDate");
			return(ret);
		}
		
		// 对比当前时间和订单最后时间
		if (strcmp(tmpNum,startDate) > 0)
		{
			// 计算起始日期
			if ((ret = UnionDecideDateAfterSpecDate(tmpNum,1,startDate)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8EB1:: UnionDecideDateAfterSpecDate specDate[%s] snapDays[1]!\n",tmpNum);
				return(ret);
			}
		}
	}

	// 计算结束日期
	memset(endDate,0,sizeof(endDate));
	if ((ret = UnionGetDateAfterSpecifiedDate1(startDate,buyTimes,endDate)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EB1:: UnionGetDateAfterSpecifiedDate1  startDate[%s] buyTimes[%d]!\n",startDate,buyTimes);
		return(ret);
	}

	// 更新订单
	len = sprintf(sql,"update unitOrder set status = 1,startDate = '%s',endDate = '%s' where orderTime = '%s' and usingUnit = '%s'",startDate,endDate,orderTime,usingUnit);
        sql[len] = 0;
        if ((ret = UnionExecRealDBSql(sql)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8EB1:: UnionExecRealDBSql[%s]!\n",sql);
                return(ret);
        }

	return(0);
}



