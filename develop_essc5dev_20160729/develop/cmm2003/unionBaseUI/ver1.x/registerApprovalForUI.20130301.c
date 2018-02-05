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
#include "UnionLog.h"
#include "UnionStr.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "unionTableDef.h"
#include "unionVersion.h"

// 记录审批
int UnionRegisterApproval()
{
	int			ret;
	char			sql[40960];
	char			buf[20480];
	int			i = 0,len = 0;
	char			viewName[32];
	char			btnName[32];
	char   			applicant[64];
	char			content[4096];
	char			approver[64];
	char			remark[128];
	char			applyTime[32];
	int			maxNum = 5;
	int			maxFieldValue = 4000;
	char			recordDetail[maxNum][4096];
	int			recordNum = 0;
	int			lenOfBuf = 0;
	int			lenOfTail = 0;
	int			status = 0;
	char			tmpBuf[128];
	int			loopNum = 1;
	
	if ((ret = UnionLocateRequestXMLPackage("approval",0)) < 0)
		return(0);
	
	UnionLocateRequestXMLPackage("",0);

	// 读取申请人
	memset(applicant,0,sizeof(applicant));
	if ((ret = UnionReadRequestXMLPackageValue("head/userID",applicant,sizeof(applicant))) < 0)
	{
		UnionUserErrLog("in UnionRegisterApproval:: UnionReadRequestXMLPackageValue[%s]!\n","head/userID");
		return(ret);
	}

	// 读取申请时间
	memset(applyTime,0,sizeof(applyTime));
	UnionGetFullSystemDateTime(applyTime);
	/*if ((ret = UnionReadRequestXMLPackageValue("head/transTime",applyTime,sizeof(applyTime))) < 0)
	{
		UnionUserErrLog("in UnionRegisterApproval:: UnionReadRequestXMLPackageValue[%s]!\n","head/transTime");
		return(ret);
	}*/

	// 读取视图名
	memset(viewName,0,sizeof(viewName));
	if ((ret = UnionReadRequestXMLPackageValue("approval/viewName",viewName,sizeof(viewName))) < 0)
	{
		UnionUserErrLog("in UnionRegisterApproval:: UnionReadRequestXMLPackageValue[%s]!\n","approval/viewName");
		return(ret);
	}

	// 读取操作名
	memset(btnName,0,sizeof(btnName));
	if ((ret = UnionReadRequestXMLPackageValue("approval/btnName",btnName,sizeof(btnName))) < 0)
	{
		UnionUserErrLog("in UnionRegisterApproval:: UnionReadRequestXMLPackageValue[%s]!\n","approval/btnName");
		return(ret);
	}

	// 读取内容
	memset(content,0,sizeof(content));
	if ((ret = UnionReadRequestXMLPackageValue("approval/content",content,sizeof(content))) < 0)
	{
		UnionUserErrLog("in UnionRegisterApproval:: UnionReadRequestXMLPackageValue[%s]!\n","approval/content");
		return(ret);
	}

	// 读取审批人
	memset(approver,0,sizeof(approver));
	if ((ret = UnionReadRequestXMLPackageValue("approval/approver",approver,sizeof(approver))) < 0)
	{
		UnionUserErrLog("in UnionRegisterApproval:: UnionReadRequestXMLPackageValue[%s]!\n","approval/approver");
		return(ret);
	}

	// 读取备注
	memset(remark,0,sizeof(remark));
	if ((ret = UnionReadRequestXMLPackageValue("approval/remark",remark,sizeof(remark))) < 0)
	{
		UnionUserErrLog("in UnionRegisterApproval:: UnionReadRequestXMLPackageValue[%s]!\n","approval/remark");
		return(ret);
	}

	if (strcmp(applicant,approver) == 0)
	{
		UnionUserErrLog("in UnionRegisterApproval:: UnionReadRequestXMLPackageValue[%s]!\n","approval/remark");
		UnionSetResponseRemark("申请人和审核人不能相同");	
		return(errCodeParameter);
	}

	status = 0;

	UnionDeleteRequestXMLPackageNode("approval",0);
	lenOfBuf = UnionRequestXMLPackageToBuf(buf,sizeof(buf));
	buf[lenOfBuf] = 0;

	// 将数据库的字符串中的单引号替换为2个单引号
	if ((ret = UnionTranslateSpecCharInDBStr(buf,strlen(buf),sql,sizeof(sql))) < 0)
	{
		UnionUserErrLog("in UnionRegisterApproval:: UnionTranslateSpecCharInDBStr buf[%s][%d]!\n",buf,(int)sizeof(buf));
		return(ret);
	}
	lenOfBuf = sprintf(buf,"%s",sql);
	buf[lenOfBuf] = 0;

	if (lenOfBuf > 20000)
	{
		UnionUserErrLog("in UnionRegisterApproval:: request xml buf[%d] is too long!\n",lenOfBuf);
		UnionSetResponseRemark("请求报文数据[%d]>20000个字节",lenOfBuf);
		return(errCodeParameter);
	}

	if ((recordNum = lenOfBuf / maxFieldValue) == 0)
	{
		memcpy(recordDetail[0],buf,lenOfBuf);
		recordDetail[0][lenOfBuf] = 0;
		recordNum ++;
	}
	else
	{
		for (i = 0; i < recordNum; i++)
		{
			memcpy(recordDetail[i],buf + i * maxFieldValue,maxFieldValue);
			recordDetail[i][maxFieldValue] = 0;
		}

		if ((lenOfTail = lenOfBuf % maxFieldValue) != 0)
		{
			memcpy(recordDetail[i], buf + i * maxFieldValue,lenOfTail);
			recordDetail[i][lenOfTail] = 0;
			recordNum ++;
		}
	}
	for (i = recordNum; i < 5; i++)
		recordDetail[i][0] = 0;
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((strcmp(UnionGetDataBaseType(),"ORACLE") == 0)  ||
		(strcmp(UnionGetDataBaseType(),"DB2") == 0) ||
		(strcmp(UnionGetDataBaseType(),"INFORMIX") == 0))
	{
		len = sprintf(sql,"select recordID from approval order by recordID desc");
		sql[len] = 0;
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0301:: UnionSelectRealDBRecord[%s]!\n",sql);
			return(ret);
		}
		else if (ret > 0)
		{
			UnionLocateXMLPackage("detail",1);
			memset(tmpBuf,0,sizeof(tmpBuf));
			if ((ret = UnionReadXMLPackageValue("recordID",tmpBuf,sizeof(tmpBuf))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0301:: UnionReadXMLPackageValue[%s]!\n","recordID");
				return(ret);
			}
			else
				loopNum = atoi(tmpBuf);
		}

		if (strcmp(UnionGetDataBaseType(),"DB2") == 0)
			strcpy(tmpBuf,"next value for approval_id_seq");
		else
			strcpy(tmpBuf,"approval_id_seq.NEXTVAL");
	}
	else
	{
		strcpy(tmpBuf,"NULL");
	}
	
	for (i = 0; i < loopNum; i++)
	{
		// 建立sql插入语句
		len = sprintf(sql,"insert into approval (recordID,status,viewName,btnName,content,applicant,applyTime,"
				"approver,remark,recordDetail1,recordDetail2,recordDetail3,recordDetail4,recordDetail5)"
				" values(%s,%d,'%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s')",
				tmpBuf,status,viewName,btnName,content,applicant,applyTime,approver,remark,
				recordDetail[0],recordDetail[1],recordDetail[2],recordDetail[3],recordDetail[4]);
		sql[len] = 0;
		
		if ((ret = UnionExecRealDBSql(sql)) < 0)
		{
			UnionUserErrLog("in UnionRegisterApproval:: UnionExecRealDBSql ret = [%d] sql[%s]!\n",ret,sql);
			return(ret);
		}
		else if (ret > 0)	
			break;
	}

	return(1);
	/*else if ((strcmp(UnionGetDataBaseType(),"DB2") == 0) || (strcmp(UnionGetDataBaseType(),"INFORMIX") == 0))
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"approval_id_seq");
		while(1)
		{
			if ((recordID = UnionGetAddSelfNumBySequence(tmpBuf)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0301:: UnionGetAddSelfNumBySequence[%s]!\n", tmpBuf);
				return(recordID);
			}
			
			// 检查序号是否已经存在
			len = sprintf(sql,"select recordID from approval where recordID = %d",recordID);
			sql[len] = 0;
			
			if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0301:: UnionSelectRealDBRecord[%s]!\n",sql);
				return(ret);
			}
			else if (ret > 0)
			{
				continue;
			}
			break;
		}
	}
	// 建立sql插入语句
	len = sprintf(sql,"insert into approval (recordID,status,viewName,btnName,content,applicant,applyTime,"
			"approver,remark,recordDetail1,recordDetail2,recordDetail3,recordDetail4,recordDetail5)"
			" values(%d,%d,'%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s')",
			recordID,status,viewName,btnName,content,applicant,applyTime,approver,remark,
			recordDetail[0],recordDetail[1],recordDetail[2],recordDetail[3],recordDetail[4]);
	sql[len] = 0;
	
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionRegisterApproval:: UnionExecRealDBSql ret = [%d] sql[%s]!\n",ret,sql);
		return(ret);
	}

	return(1);*/
}
