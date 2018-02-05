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
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "errCodeTranslater.h"
#include "highCached.h"
#include "unionVersion.h"
#include "baseUIService.h"

/*********************************
服务代码:	0207
服务名:		修改组织
功能描述:	修改组织
**********************************/

int UnionDealServiceCode0207(PUnionHsmGroupRec phsmGroupRec)
{
	int			i,j,ret;
	int			len = 0;
	char			organizationName[128];
	char			newOrganizationName[128];
	char			tmpBuf[1024];
	char			sql[8192];
	int			lenOfSql = 0;
	char			content[1024];
	char			fieldList[1024];
	char			fieldName[64][128];
	char			fieldValue[64][128];
	int			fieldNum = 0;
	
	PUnionTableDef		ptableDef = NULL;
	PUnionTableFieldDef	ptableFieldDef = NULL;


	// 读取原组织名 
	memset(organizationName,0,sizeof(organizationName));
	if ((ret = UnionReadRequestXMLPackageValue("body/organizationName",organizationName,sizeof(organizationName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0207:: UnionReadRequestXMLPackageValue[%s]!\n","body/organizationName");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0207:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/organizationName");
		return(errCodeDatabaseMDL_TableNotFound);
	}

	// 读取新组织名 
	memset(newOrganizationName,0,sizeof(newOrganizationName));
	if ((ret = UnionReadRequestXMLPackageValue("body/newOrganizationName",newOrganizationName,sizeof(newOrganizationName))) <= 0)
	{
		strcpy(newOrganizationName,"");
	}

	// 读取域定义串
	memset(fieldList,0,sizeof(fieldList));
	if ((ret = UnionReadRequestXMLPackageValue("body/fieldList",fieldList,sizeof(fieldList))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0207:: UnionReadRequestXMLPackageValue[%s]!\n","body/fieldList");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0207:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/fieldList");
		return(errCodeDatabaseMDL_TableNotFound);
	}

	// 拼分域定义串
	memset(fieldName,0,sizeof(fieldName));
	if ((fieldNum = UnionSeprateVarStrIntoVarGrp(fieldList,strlen(fieldList),',',fieldName,64)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0207:: UnionSeprateVarStrIntoVarGrp[%s]!\n",fieldList);
		return(fieldNum);
	}

	// 读取各个字段名对应的值
	memset(fieldValue,0,sizeof(fieldValue));
	for (i = 0; i < fieldNum; i++)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		memset(fieldValue[i],0,sizeof(fieldValue[i]));
		sprintf(tmpBuf,"body/field/%s",fieldName[i]);
		if ((ret = UnionReadRequestXMLPackageValue(tmpBuf,fieldValue[i],sizeof(fieldValue[i]))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0207:: UnionReadRequestXMLPackageValue[%s]!\n",tmpBuf);
			return(ret);
		}
		
		// 去掉前后空格
		UnionFilterHeadAndTailBlank(fieldValue[i]);
		
		// 将数据库的字符串中的单引号替换为2个单引号
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionTranslateSpecCharInDBStr(fieldValue[i],strlen(fieldValue[i]),tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0207:: UnionTranslateSpecCharInDBStr tmpBuf[%s][%d]!\n",tmpBuf,(int)sizeof(tmpBuf));
			return(ret);
		}
		memset(fieldValue[i],0,sizeof(fieldValue[i]));
		strcpy(fieldValue[i],tmpBuf);
	}


	// 检查是否在定义中
	if ((ptableDef = UnionFindTableDef("organization")) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode0207:: UnionFindTableDef[%s] not found!\n","organization");
		return(errCodeDatabaseMDL_TableNotFound);
	}

	// 查询读取的域名是否在定义中存在
	for (i = 0; i < fieldNum; i++)
	{
		for (j = 0; j < ptableDef->fieldNum; j++)
		{
			if (strcmp(fieldName[i],ptableDef->fieldDef[j].fieldName) == 0)
			{
				ptableFieldDef = &ptableDef->fieldDef[j];
				switch(ptableFieldDef->fieldType)
				{
					case 1 :	// 字符型
					case 4 :	// 二进制
						len += sprintf(content + len,"%s = '%s',",fieldName[i],fieldValue[i]);
						break;
					case 2 :	// 整型
					case 3 :	// 浮点数
					case 5 :	// 布尔型
						len += sprintf(content + len,"%s = %s,",fieldName[i],fieldValue[i]);
						break;
					default :
						len += sprintf(content + len,"%s = '%s',",fieldName[i],fieldValue[i]);
						break;
				}
				break;
			}
		}

		// 字段名不存在
		if (ptableDef->fieldNum == j)
		{
			UnionUserErrLog("in UnionDealServiceCode0207:: fieldName[%s] not found!\n",fieldName[i]);
			return(errCodeObjectMDL_FieldNotExist);
		}
	}

	// 去除结尾的","
	if (content[strlen(content)-1] == ',')
		content[strlen(content)-1] = '\0';

	// modify by leipp 20151202
	if ((strcmp(newOrganizationName,organizationName) != 0) && (strlen(newOrganizationName) > 0))
	{
		// 检查新组织名是否存在
		snprintf(sql,sizeof(sql),"select organizationName from organization where organizationName = '%s'",newOrganizationName);
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0207:: UnionSelectRealDBRecord ret[%d] sql[%s]!\n",ret,sql);
			return(ret);
		}
		else if (ret > 0)
		{
			UnionSetResponseRemark("组织名[%s]已经存在,无法修改",newOrganizationName);
			return(errCodeDatabaseMDL_RecordAlreadyExist);
		}

		// 新建组织名0
		lenOfSql = snprintf(sql,sizeof(sql),"insert into organization(organizationName) values('0');");
		
		// 修改用户组织为0
		lenOfSql += snprintf(sql+lenOfSql,sizeof(sql)-lenOfSql,"update sysUser set organization = '0' where organization = '%s';",organizationName);

		// 修改旧组织
		lenOfSql += snprintf(sql+lenOfSql,sizeof(sql)-lenOfSql,"update organization set organizationName = '%s',%s where organizationName = '%s';",newOrganizationName,content,organizationName);

		// 修改上级组织为新组织名
		lenOfSql += snprintf(sql+lenOfSql,sizeof(sql)-lenOfSql,"update organization set higherOrganization = '%s' where higherOrganization = '%s';",newOrganizationName,organizationName);

		// 修改组织0为新组织名
		lenOfSql += snprintf(sql+lenOfSql,sizeof(sql)-lenOfSql,"update sysUser set organization = '%s' where organization = '0';",newOrganizationName);

		// 删除组织名为0的组织
		lenOfSql += snprintf(sql+lenOfSql,sizeof(sql)-lenOfSql,"delete from  organization  where organizationName = '0';");
	}
	else
	{
		// 修改组织信息
		lenOfSql = snprintf(sql,sizeof(sql),"update organization set %s where organizationName = '%s'",content,organizationName);
	}
	sql[lenOfSql] = 0;
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0207:: UnionExecRealDBSql  ret[%d]  sql[%s]!\n",ret,sql);
		return(ret);
	}

	return(0);
}
