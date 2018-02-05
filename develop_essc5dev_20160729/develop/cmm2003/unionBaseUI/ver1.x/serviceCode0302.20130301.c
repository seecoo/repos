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
服务代码:	0302
服务名:		修改记录
功能描述:	修改记录
**********************************/

int UnionDealServiceCode0302(PUnionHsmGroupRec phsmGroupRec)
{
	int	i;
	int	j;
	int	ret;
	int	len;
	int	lenSet = 0;
	char	tableName[128];
	char	tmpBuf[4096];
	char	sql[20480];
	char	contentSet[20480];
	char	condition[1024];
	char	fieldList[1024];
	char	fieldName[64][128];
	char	fieldValue[64][4069];
	int	fieldNum;
	char	taskName[128];
	int	port = -1;
	char	*ptr = NULL;
	
	PUnionTableDef		ptableDef = NULL;
	PUnionTableFieldDef	ptableFieldDef = NULL;
	TUnionClusterDefTBL	clusterDefTBL;
	TUnionClusterSyn	clusterSyn;

	// 表名
	memset(tableName,0,sizeof(tableName));
	if ((ret = UnionReadRequestXMLPackageValue("body/tableName",tableName,sizeof(tableName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0302:: UnionReadRequestXMLPackageValue[%s]!\n","body/tableName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(tableName);
	if (strlen(tableName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0302:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/tableName");
		return(errCodeDatabaseMDL_TableNotFound);
	}

	// 只取第一个表
	if ((ptr = strchr(tableName,',')))
		*ptr = 0;

	// 读取域清单
	memset(fieldList,0,sizeof(fieldList));
	if ((ret = UnionReadRequestXMLPackageValue("body/fieldList",fieldList,sizeof(fieldList))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0302:: UnionReadRequestXMLPackageValue[%s]!\n","body/fieldList");
		return(ret);
	}

	// 拼分域定义串
	memset(fieldName,0,sizeof(fieldName));
	if ((fieldNum = UnionSeprateVarStrIntoVarGrp(fieldList,strlen(fieldList),',',fieldName,64)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0302:: UnionSeprateVarStrIntoVarGrp[%s]!\n",fieldList);
		return(fieldNum);
	}

	// 读取各个字段名对应的值
	for (i = 0; i < fieldNum; i++)
	{
		len = sprintf(tmpBuf,"body/field/%s",fieldName[i]);
		tmpBuf[len] = 0;
		if ((ret = UnionReadRequestXMLPackageValue(tmpBuf,fieldValue[i],sizeof(fieldValue[i]))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0302:: UnionReadRequestXMLPackageValue[%s]!\n",tmpBuf);
			return(ret);
		}
		fieldValue[i][ret] = 0;
		// 去掉前后空格
		UnionFilterHeadAndTailBlank(fieldValue[i]);

		// 将数据库的字符串中的单引号替换为2个单引号
		if ((ret = UnionTranslateSpecCharInDBStr(fieldValue[i],strlen(fieldValue[i]),tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0302:: UnionTranslateSpecCharInDBStr tmpBuf[%s][%d]!\n",tmpBuf,(int)sizeof(tmpBuf));
			return(ret);
		}
		tmpBuf[ret] = 0;
		len = sprintf(fieldValue[i],"%s",tmpBuf);
		fieldValue[i][len] = 0;
	}

	// 读取条件
	if ((ret = UnionReadRequestXMLPackageValue("body/condition",condition,sizeof(condition))) < 0)
	{
		//UnionUserErrLog("in UnionDealServiceCode0302:: UnionReadRequestXMLPackageValue[%s]!\n","body/condition");
		//return(ret);
	}
	condition[ret] = 0;

	if ((ptr = strstr(condition,"order by")) != NULL)
		ptr = 0;
	UnionFilterHeadAndTailBlank(condition);
	
	// 检查是否在定义中
	if ((ptableDef = UnionFindTableDef(tableName)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode0302:: UnionFindTableDef[%s] not found!\n",tableName);
		return(errCodeDatabaseMDL_TableNotFound);
	}

	// 查询读取的域名是否在定义中存在
	memset(contentSet,0,sizeof(contentSet));
	for (i = 0; i < fieldNum; i++)
	{
		// 去掉字段前面的表名
		if ((ptr = strchr(fieldName[i],'.')) != NULL)
		{
			len = strlen(ptr + 1);
			memmove(fieldName[i],ptr + 1,len);
			fieldName[i][len] = 0;
		}
		
		for (j = 0; j < ptableDef->fieldNum; j++)
		{
			if (strcmp(fieldName[i],ptableDef->fieldDef[j].fieldName) == 0)
			{
				ptableFieldDef = &ptableDef->fieldDef[j];
				switch(ptableFieldDef->fieldType)
				{
					case 1 :	// 字符型
					case 4 :	// 二进制
					case 7 :	// 时间
						// 检查输入的大小是否超过最大长度
						if (strlen(fieldValue[i]) > ptableFieldDef->fieldSize)
						{
							UnionUserErrLog("in UnionDealServiceCode0302:: field[%s] length[%s] > max length[%.0f]!\n",fieldName[i],fieldValue[i], ptableFieldDef->fieldSize);
							UnionSetResponseRemark("[%s:] 输入长度[%d] > 最大长度[%.0f]",ptableFieldDef->fieldChnName,strlen(fieldValue[i]),ptableFieldDef->fieldSize);
							return(errCodeParameter);
						}
						if (strlen(fieldValue[i]) > 0)
							lenSet += sprintf(contentSet + lenSet,"%s = '%s',",fieldName[i],fieldValue[i]);
						else
							lenSet += sprintf(contentSet + lenSet,"%s = null,",fieldName[i]);
						break;
					case 2 :	// 整型
					case 3 :	// 浮点数
					case 5 :	// 布尔型
					case 6 :	// 自增长
						if (strlen(fieldValue[i]) > 0)
						{
							lenSet += sprintf(contentSet + lenSet,"%s = %s,",fieldName[i],fieldValue[i]);
						}
						break;
					default :
						// 检查输入的大小是否超过最大长度
						if (strlen(fieldValue[i]) > ptableFieldDef->fieldSize)
						{
							UnionUserErrLog("in UnionDealServiceCode0302:: field[%s] length[%s] > max length[%.0f]!\n",fieldName[i],fieldValue[i], ptableFieldDef->fieldSize);
							UnionSetResponseRemark("[%s:] 输入长度[%d] > 最大长度[%.0f]",ptableFieldDef->fieldChnName,strlen(fieldValue[i]),ptableFieldDef->fieldSize);
							return(errCodeParameter);
						}
						if (strlen(fieldValue[i]) > 0)
							lenSet += sprintf(contentSet + lenSet,"%s = '%s',",fieldName[i],fieldValue[i]);
						else
							lenSet += sprintf(contentSet + lenSet,"%s = null,",fieldName[i]);
						break;
				}
				break;
			}
		}

		// 字段名不存在
		if (ptableDef->fieldNum == j)
		{
			UnionUserErrLog("in UnionDealServiceCode0302:: fieldName[%s] not found!\n",fieldName[i]);
			return(errCodeObjectMDL_FieldNotExist);
		}
	}

	// 建立sql修改语句
	if (contentSet[lenSet - 1] == ',')
		contentSet[lenSet - 1] = 0;
	
	memset(sql,0,sizeof(sql));
	if (strlen(condition) == 0)
		sprintf(sql,"update %s set %s",tableName,contentSet);
	else
		sprintf(sql,"update %s set %s where %s ",tableName,contentSet,condition);
	
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0302:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}
	/*if ((strcmp(UnionGetDataBaseType(),"MYSQL") != 0) && (ret == 0))
	{
		UnionUserErrLog("in UnionDealServiceCode0302:: UnionExecRealDBSql[%s]!\n",sql);
		return(errCodeDatabaseMDL_RecordUpdateFailure);
	}*/

	// 更新共享内存
	memset(&clusterDefTBL,0,sizeof(clusterDefTBL));
	if ((ret = UnionInitClusterDef(&clusterDefTBL)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0302:: UnionInitClusterDef!\n");
		return(ret);
	}

	for (i = 0; i < clusterDefTBL.realNum; i++)
	{
		if (strcasecmp(tableName,clusterDefTBL.rec[i].tableName) == 0)
		{
			for (j = 0; j < UnionReadIntTypeRECVar("numOfCluster"); j++)
			{
				memset(&clusterSyn,0,sizeof(clusterSyn));
				len = 0;
				if (strcasecmp(tableName,"unionTask") == 0)
				{
					memset(taskName,0,sizeof(taskName));
					if (memcmp(condition,"taskName='",10) == 0)
					{
						if ((ptr = strstr(condition + 10,"'")) != NULL)
						{
							memcpy(taskName,condition + 10, ptr - condition - 10);
							if (memcmp(taskName,"appTask",7) == 0)
							{
								if ((ptr = strstr(taskName," ")) != NULL)
								{
									*ptr = 0;
									port = atoi(ptr+1);
								}
								len = sprintf(clusterSyn.cmd,"mngTask -clnanyway \" %d\";",port);
							}
							else if (strcmp(taskName,"mngClusterSyn") != 0)
							{
								len = sprintf(clusterSyn.cmd,"mngTask -clnanyway \"%s\";",taskName);
							}
							if (memcmp(taskName,"dbSvr",5) == 0)
								len += sprintf(clusterSyn.cmd + len,"dbSvr;");
						}
					}
				}
				clusterSyn.clusterNo = j + 1;
				sprintf(clusterSyn.cmd+len,"%s",clusterDefTBL.rec[i].cmd);
				UnionGetFullSystemDateTime(clusterSyn.regTime);
				if ((ret = UnionInsertClusterSyn(&clusterSyn)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCode0302:: UnionInsertClusterSyn!\n");
					return(ret);
				}
			}
		}
	}

	return(0);
}
