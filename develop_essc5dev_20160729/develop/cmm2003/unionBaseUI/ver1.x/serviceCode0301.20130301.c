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
服务代码:	0301
服务名:		增加记录
功能描述:	增加记录
**********************************/
int UnionDealServiceCode0301(PUnionHsmGroupRec phsmGroupRec)
{
	int	i = 0;
	int	j = 0;
	int	ret = 0;
	int	lenFieldName = 0;
	int	lenValue = 0;
	int	fieldNum = 0;
	char	tableName[128];
	char	tmpBuf[4096];
	char	sql[89120];
	char	fieldList[2048];
	char	fieldValueList[89120];
	char	fieldGrp[64][128];
	char	fieldValue[64][4096];
	char	*ptr = NULL;
	int	loopNum = 1;
	char	userID[80];
	char	organizationID[16];
	
	PUnionTableDef		ptableDef = NULL;
	PUnionTableFieldDef	ptableFieldDef = NULL;
	TUnionClusterDefTBL	clusterDefTBL;
	TUnionClusterSyn	clusterSyn;

	// 表名
	memset(tableName,0,sizeof(tableName));
	if ((ret = UnionReadRequestXMLPackageValue("body/tableName",tableName,sizeof(tableName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0301:: UnionReadRequestXMLPackageValue[%s]!\n","body/tableName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(tableName);
	if (strlen(tableName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0301:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/tableName");
		return(errCodeDatabaseMDL_TableNotFound);
	}

	// 只取第一个表
	if ((ptr = strchr(tableName,',')))
		*ptr = 0;

	// 读取域清单
	memset(fieldList,0,sizeof(fieldList));
	if ((ret = UnionReadRequestXMLPackageValue("body/fieldList",fieldList,sizeof(fieldList))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0301:: UnionReadRequestXMLPackageValue[%s]!\n","body/fieldList");
		return(ret);
	}

	// 拼分域定义串
	memset(fieldGrp,0,sizeof(fieldGrp));
	if ((fieldNum = UnionSeprateVarStrIntoVarGrp(fieldList,strlen(fieldList),',',fieldGrp,64)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0301:: UnionSeprateVarStrIntoVarGrp[%s]!\n",fieldList);
		return(fieldNum);
	}

	// 读取对应域名的域值
	for (i = 0; i < fieldNum; i++)
	{
		memset(fieldValue[i],0,sizeof(fieldValue[i]));
		memset(tmpBuf,0,sizeof(tmpBuf));
		
		// 去掉前后空格
		UnionFilterHeadAndTailBlank(fieldGrp[i]);
		
		sprintf(tmpBuf,"body/field/%s",fieldGrp[i]);
		if ((ret = UnionReadRequestXMLPackageValue(tmpBuf,fieldValue[i],sizeof(fieldValue[i]))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0301:: UnionReadRequestXMLPackageValue[%s] [%d]!\n",tmpBuf,i);
			return(ret);
		}
		// 去掉前后空格
		UnionFilterHeadAndTailBlank(fieldValue[i]);
		
		// 将数据库的字符串中的单引号替换为2个单引号
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionTranslateSpecCharInDBStr(fieldValue[i],strlen(fieldValue[i]),tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0301:: UnionTranslateSpecCharInDBStr tmpBuf[%s][%d]!\n",tmpBuf,(int)sizeof(tmpBuf));
			return(ret);
		}
		memset(fieldValue[i],0,sizeof(fieldValue[i]));
		strcpy(fieldValue[i],tmpBuf);
	}

	// 检查是否在定义中
	if ((ptableDef = UnionFindTableDef(tableName)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode0301:: UnionFindTableDef[%s] not found!\n",tableName);
		return(errCodeDatabaseMDL_TableNotFound);
	}

	// 查询读取的域名是否在定义中存在
	memset(fieldList,0,sizeof(fieldList));
	memset(fieldValueList,0,sizeof(fieldValueList));
	for (i = 0; i < fieldNum; i++)
	{
		for (j = 0; j < ptableDef->fieldNum; j++)
		{
			// 检查字段是否前面有表名存在
			if ((ptr = strchr(fieldGrp[i],'.')))
			{
				memset(tmpBuf,0,sizeof(tmpBuf));
				strcpy(tmpBuf,ptr +1);
				memset(fieldGrp[i],0,sizeof(fieldGrp[i]));
				strcpy(fieldGrp[i],tmpBuf);
			}
			
			if (strcmp(fieldGrp[i],ptableDef->fieldDef[j].fieldName) != 0)
				continue;

			// 任务表
			if (strcmp(tableName,"unionTask") == 0)
			{
				if (strcmp(fieldGrp[i],"taskName") == 0)
				{
					if (strlen(fieldValue[i]) < 5 || UnionIsDigitString(fieldValue[i]))
					{
						UnionUserErrLog("in UnionDealServiceCode0301:: %s[%s] length < 5 or is DigitString!\n",fieldGrp[i],fieldValue[i]);
						UnionSetResponseRemark("任务名长度不能小于5，且不能为纯数字");
						return(errCodeParameter);
					}
				}
			}
			else if (strcmp(tableName,"externalSystem") == 0)
			{
				if ((strcasecmp(UnionGetIDOfCustomization(),"aliyun") == 0) && (strcmp(fieldGrp[i],"sysID") == 0))
				{
					if ((ret = UnionReadRequestXMLPackageValue("head/userID",userID,sizeof(userID))) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCode0301:: UnionReadRequestXMLPackageValue[%s]!\n","head/userID");
						return(ret);
					}
					userID[ret] = 0;
					snprintf(sql,sizeof(sql),"select organizationID from organization left join sysUser on organization.organizationName = sysUser.organization where userID ='%s'",userID);
					if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCode0301:: UnionSelectRealDBRecord ret[%d] sql[%s]!\n",ret,sql);
						return(ret);
					}
					UnionLocateXMLPackage("detail", 1);

					if ((ret = UnionReadXMLPackageValue("organizationID", organizationID, sizeof(organizationID))) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCode0301:: UnionReadXMLPackageValue[%s]!\n","organizationID");
						return(ret);
					}
					organizationID[ret] = 0;
					snprintf(fieldValue[i]+strlen(fieldValue[i]),sizeof(fieldValue[i]),":%s",organizationID);	
				}
			}

			ptableFieldDef = &ptableDef->fieldDef[j];

			switch(ptableFieldDef->fieldType)
			{
				case 1 :	// 字符串
				case 4 :	// 二进制
				case 7 :	// 时间
					
					// 检查输入的大小是否超过最大长度
					if (strlen(fieldValue[i]) > ptableFieldDef->fieldSize)
					{
						UnionUserErrLog("in UnionDealServiceCode0301:: field[%s] length[%s] > max length[%.0f]!\n",fieldGrp[i],fieldValue[i], ptableFieldDef->fieldSize);
						UnionSetResponseRemark("[%s:] 输入长度[%d] > 最大长度[%.0f]",ptableFieldDef->fieldChnName,strlen(fieldValue[i]),ptableFieldDef->fieldSize);
						return(errCodeParameter);
					}

					if (strlen(fieldValue[i]) > 0)
					{
						lenFieldName += sprintf(fieldList + lenFieldName,"%s,",fieldGrp[i]);
						lenValue += sprintf(fieldValueList + lenValue,"'%s',",fieldValue[i]);
					}
					else
					{
						lenFieldName += sprintf(fieldList + lenFieldName,"%s,",fieldGrp[i]);
						lenValue += sprintf(fieldValueList + lenValue,"null,");
					}
					break;
				case 2 :	// 整型
				case 3 :	// 浮点数
				case 5 :	// 布尔型
					if (strlen(fieldValue[i]) > 0)
					{
						lenFieldName += sprintf(fieldList + lenFieldName,"%s,",fieldGrp[i]);
						lenValue += sprintf(fieldValueList + lenValue,"%s,",fieldValue[i]);
					}
					break;	
				case 6 :	// 自增长
					if (atoi(fieldValue[i]) > 0)
					{
						lenFieldName += sprintf(fieldList + lenFieldName,"%s,",fieldGrp[i]);
						lenValue += sprintf(fieldValueList + lenValue,"%s,",fieldValue[i]);
						break;	
					}

					if ((strcmp(UnionGetDataBaseType(),"ORACLE") == 0) ||
						(strcmp(UnionGetDataBaseType(),"DB2") == 0) ||
						(strcmp(UnionGetDataBaseType(),"INFORMIX") == 0))
					{
						memset(tmpBuf,0,sizeof(tmpBuf));
						if (strcmp(UnionGetDataBaseType(),"DB2") == 0)
							sprintf(tmpBuf,"next value for %s_id_seq",tableName);
						else
							sprintf(tmpBuf,"%s_id_seq.NEXTVAL",tableName);

						lenFieldName += sprintf(fieldList + lenFieldName,"%s,",fieldGrp[i]);
						lenValue += sprintf(fieldValueList + lenValue,"%s,", tmpBuf);

						memset(sql,0,sizeof(sql));
						sprintf(sql,"select %s from %s order by %s desc",fieldGrp[i],tableName,fieldGrp[i]);
						if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCode0301:: UnionSelectRealDBRecord[%s]!\n",sql);
							return(ret);
						}
						else if (ret == 0)
							break;

						UnionLocateXMLPackage("detail",1);
						memset(tmpBuf,0,sizeof(tmpBuf));
						if ((ret = UnionReadXMLPackageValue(fieldGrp[i],tmpBuf,sizeof(tmpBuf))) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCode0301:: UnionReadXMLPackageValue[%s]!\n",fieldGrp[i]);
							return(ret);
						}
						else
							loopNum = atoi(tmpBuf);
					}
					else	// MySQL
					{
						lenFieldName += sprintf(fieldList + lenFieldName,"%s,",fieldGrp[i]);
						lenValue += sprintf(fieldValueList + lenValue,"NULL,");
					}	
					break;
				default :

					// 检查输入的大小是否超过最大长度
					if (strlen(fieldValue[i]) > ptableFieldDef->fieldSize)
					{
						UnionUserErrLog("in UnionDealServiceCode0301:: field[%s] length[%s] > max length[%.0f]!\n",fieldGrp[i],fieldValue[i], ptableFieldDef->fieldSize);
						UnionSetResponseRemark("[%s:] 输入长度[%d] > 最大长度[%.0f]",ptableFieldDef->fieldChnName,strlen(fieldValue[i]),ptableFieldDef->fieldSize);
						return(errCodeParameter);
					}

					if (strlen(fieldValue[i]) > 0)
					{
						lenFieldName += sprintf(fieldList + lenFieldName,"%s,",fieldGrp[i]);
						lenValue += sprintf(fieldValueList + lenValue,"'%s',",fieldValue[i]);
					}
					else
					{
						lenFieldName += sprintf(fieldList + lenFieldName,"%s,",fieldGrp[i]);
						lenValue += sprintf(fieldValueList + lenValue,"null,");
					}
					break;
			}
		}
	}

	// 去除最后的","
	if (lenFieldName > 0)
		fieldList[lenFieldName - 1] = 0;
	if (lenValue > 0)
		fieldValueList[lenValue - 1] = 0;

	// 建立sql插入语句
	for (i = 0; i < loopNum; i++)
	{
		memset(sql,0,sizeof(sql));
		sprintf(sql,"insert into %s (%s) values(%s)",tableName,fieldList,fieldValueList);
		if ((ret = UnionExecRealDBSql(sql)) < 0)
		{
			if (i == loopNum -1)
			{
				UnionUserErrLog("in UnionDealServiceCode0301:: UnionExecRealDBSql[%s]!\n",sql);
				return (ret);
			}
			continue;
		}
		else if (ret > 0)
			break;
	}
	
	// 更新共享内存
	memset(&clusterDefTBL,0,sizeof(clusterDefTBL));
	if ((ret = UnionInitClusterDef(&clusterDefTBL)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0301:: UnionInitClusterDef!\n");
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
					UnionUserErrLog("in UnionDealServiceCode0301:: UnionInsertClusterSyn!\n");
					return(ret);
				}
			}
		}
	}
	return(0);
}
