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

// 记录操作审计
int UnionRegisterOperatorAudit()
{
	int	ret;
	int	auditFlag = 0;
	char    serviceCode[32];
	char    userID[128];
	char	clientIPAddr[64];
	char	systemDateTime[32];
	char	tmpBuf[1024];
	char	sql[8192];
	char	event[4096];
	char	buf[204800];
	char	*ptr1 = NULL;
	char	*ptr2 = NULL;
	char	*ptr3 = NULL;
	char	primaryKeyGrp[conMaxFieldNumOfKeyGroup][64];
	char	primaryKeyNum = 0;
	char	tableName[64];
	char	record[2048];
	int	i = 0,len = 0;
	
	// 获取服务代码
	memset(serviceCode,0,sizeof(serviceCode));
	if ((ret = UnionReadRequestXMLPackageValue("head/serviceCode",serviceCode,sizeof(serviceCode))) < 0)
	{
		UnionUserErrLog("in UnionRegisterOperatorAudit:: UnionReadRequestXMLPackageValue[%s]!\n","head/serviceCode");
		return(ret);
	}

	// 获取用户ID
	memset(userID,0,sizeof(userID));
	if ((ret = UnionReadRequestXMLPackageValue("head/userID",userID,sizeof(userID))) < 0)
	{
		UnionUserErrLog("in UnionRegisterOperatorAudit:: UnionReadRequestXMLPackageValue[%s]!\n","head/userID");
		return(ret);
	}

	memset(clientIPAddr,0,sizeof(clientIPAddr));
	if ((ret = UnionReadRequestXMLPackageValue("head/clientIPAddr",clientIPAddr,sizeof(clientIPAddr))) < 0)
	{
		UnionUserErrLog("in UnionRegisterOperatorAudit:: UnionReadRequestXMLPackageValue[%s]!\n","head/clientIPAddr");
		return(ret);
	}

	memset(systemDateTime,0,sizeof(systemDateTime));
	UnionGetFullSystemDateTime(systemDateTime);

	// 读取审计标识
	memset(sql,0,sizeof(sql));
	sprintf(sql,"select auditFlag from serviceCode where serviceCode = '%s'",serviceCode);
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionRegisterOperatorAudit:: UnionSelectRealDBRecord!\n");
		return(ret);
	}
	
	UnionLocateXMLPackage("detail", 1);
	
	// 检查此类型的服务是否启用
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("auditFlag",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionRegisterOperatorAudit:: UnionReadXMLPackageValue[auditFlag]! ret = [%d]\n",ret);
		return(ret);
	}
	
	// 不登记审计信息
	if ((auditFlag = atoi(tmpBuf)) == 0)
		return(0);
	
	memset(buf,0,sizeof(buf));
	UnionRequestXMLPackageToBuf(buf,sizeof(buf));

	// 查找表和记录
	memset(tableName,0,sizeof(tableName));
	memset(record,0,sizeof(record));
	len = sprintf(record,"|");
	if ((ptr1 = strstr(buf,"<tableName>")) != NULL)
	{
		ptr2 = strstr(buf,"</tableName>");
		memcpy(tableName,ptr1 + 11,ptr2 - ptr1 - 11);
		
		if ((ptr3 = strstr(buf,"<field>")) != NULL)
		{
			// 查找表中的主键
			primaryKeyNum = UnionReadTablePrimaryKeyGrpFromImage(tableName,primaryKeyGrp);
			for (i = 0; i < primaryKeyNum; i++)
			{
				memset(tmpBuf,0,sizeof(tmpBuf));
				sprintf(tmpBuf,"<%s>",primaryKeyGrp[i]);
				if ((ptr1 = strstr(ptr3,tmpBuf)) != NULL)
				{
					memset(tmpBuf,0,sizeof(tmpBuf));
					sprintf(tmpBuf,"</%s>",primaryKeyGrp[i]);
					ptr2 = strstr(ptr3,tmpBuf);
					memset(tmpBuf,0,sizeof(tmpBuf));
					memcpy(tmpBuf,ptr1 + strlen(primaryKeyGrp[i]) + 2,ptr2 - ptr1 - strlen(primaryKeyGrp[i]) - 2);
					if (len + strlen(tmpBuf) + 1 > sizeof(record))
						break;
					len += sprintf(record + len,"%s|",tmpBuf);
				}
			}
		}
		/*if ((ptr1 = strstr(buf,"<condition>")) != NULL)
		{
			ptr2 = strstr(buf,"</condition>");	
			memset(tmpBuf,0,sizeof(tmpBuf));
			memcpy(tmpBuf,ptr1 + 11,ptr2 - ptr1 - 11);
			len += sprintf(record + len,"%s|",tmpBuf);
		}*/
		memset(event,0,sizeof(event));
		UnionTranslateSpecCharInDBStr(record,strlen(record),event,sizeof(event));
		strcpy(record,event);
	}
	
	memset(event,0,sizeof(event));
	if ((ptr1 = strstr(buf,"<body>")) != NULL)
	{	
		ptr2 = strstr(buf,"</body>");
		if ((ptr2 - ptr1 - 6) < sizeof(event) - 1)
		{
			//memcpy(event,ptr1 + 6,ptr2 - ptr1 - 6);
			// 将数据库的字符串中的单引号替换为2个单引号
			UnionTranslateSpecCharInDBStr(ptr1 + 6,ptr2 - ptr1 - 6,event,sizeof(event));
			// 去掉XML中的斜杠
			ptr1 = strstr(event,"</");
			while(ptr1)
			{
				memmove(ptr1+1,ptr1+2,strlen(ptr1)-1);
				ptr1 = strstr(event,"</");
			}
		}
	}
	
	// 建立sql插入语句
	memset(sql,0,sizeof(sql));
	sprintf(sql,"insert into operationAudit (userID,transTime,clientIPAddr,serviceCode,event,tableName,record) values('%s','%s','%s','%s','%s','%s','%s')",userID,systemDateTime,clientIPAddr,serviceCode,event,tableName,record);
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
                UnionUserErrLog("in UnionRegisterOperatorAudit:: UnionExecRealDBSql ret = [%d] sql[%s]!\n",ret,sql);
                return(ret);
        }
	
	return(0);
}
