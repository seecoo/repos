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
�������:	0207
������:		�޸���֯
��������:	�޸���֯
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


	// ��ȡԭ��֯�� 
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

	// ��ȡ����֯�� 
	memset(newOrganizationName,0,sizeof(newOrganizationName));
	if ((ret = UnionReadRequestXMLPackageValue("body/newOrganizationName",newOrganizationName,sizeof(newOrganizationName))) <= 0)
	{
		strcpy(newOrganizationName,"");
	}

	// ��ȡ���崮
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

	// ƴ�����崮
	memset(fieldName,0,sizeof(fieldName));
	if ((fieldNum = UnionSeprateVarStrIntoVarGrp(fieldList,strlen(fieldList),',',fieldName,64)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0207:: UnionSeprateVarStrIntoVarGrp[%s]!\n",fieldList);
		return(fieldNum);
	}

	// ��ȡ�����ֶ�����Ӧ��ֵ
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
		
		// ȥ��ǰ��ո�
		UnionFilterHeadAndTailBlank(fieldValue[i]);
		
		// �����ݿ���ַ����еĵ������滻Ϊ2��������
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionTranslateSpecCharInDBStr(fieldValue[i],strlen(fieldValue[i]),tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0207:: UnionTranslateSpecCharInDBStr tmpBuf[%s][%d]!\n",tmpBuf,(int)sizeof(tmpBuf));
			return(ret);
		}
		memset(fieldValue[i],0,sizeof(fieldValue[i]));
		strcpy(fieldValue[i],tmpBuf);
	}


	// ����Ƿ��ڶ�����
	if ((ptableDef = UnionFindTableDef("organization")) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode0207:: UnionFindTableDef[%s] not found!\n","organization");
		return(errCodeDatabaseMDL_TableNotFound);
	}

	// ��ѯ��ȡ�������Ƿ��ڶ����д���
	for (i = 0; i < fieldNum; i++)
	{
		for (j = 0; j < ptableDef->fieldNum; j++)
		{
			if (strcmp(fieldName[i],ptableDef->fieldDef[j].fieldName) == 0)
			{
				ptableFieldDef = &ptableDef->fieldDef[j];
				switch(ptableFieldDef->fieldType)
				{
					case 1 :	// �ַ���
					case 4 :	// ������
						len += sprintf(content + len,"%s = '%s',",fieldName[i],fieldValue[i]);
						break;
					case 2 :	// ����
					case 3 :	// ������
					case 5 :	// ������
						len += sprintf(content + len,"%s = %s,",fieldName[i],fieldValue[i]);
						break;
					default :
						len += sprintf(content + len,"%s = '%s',",fieldName[i],fieldValue[i]);
						break;
				}
				break;
			}
		}

		// �ֶ���������
		if (ptableDef->fieldNum == j)
		{
			UnionUserErrLog("in UnionDealServiceCode0207:: fieldName[%s] not found!\n",fieldName[i]);
			return(errCodeObjectMDL_FieldNotExist);
		}
	}

	// ȥ����β��","
	if (content[strlen(content)-1] == ',')
		content[strlen(content)-1] = '\0';

	// modify by leipp 20151202
	if ((strcmp(newOrganizationName,organizationName) != 0) && (strlen(newOrganizationName) > 0))
	{
		// �������֯���Ƿ����
		snprintf(sql,sizeof(sql),"select organizationName from organization where organizationName = '%s'",newOrganizationName);
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0207:: UnionSelectRealDBRecord ret[%d] sql[%s]!\n",ret,sql);
			return(ret);
		}
		else if (ret > 0)
		{
			UnionSetResponseRemark("��֯��[%s]�Ѿ�����,�޷��޸�",newOrganizationName);
			return(errCodeDatabaseMDL_RecordAlreadyExist);
		}

		// �½���֯��0
		lenOfSql = snprintf(sql,sizeof(sql),"insert into organization(organizationName) values('0');");
		
		// �޸��û���֯Ϊ0
		lenOfSql += snprintf(sql+lenOfSql,sizeof(sql)-lenOfSql,"update sysUser set organization = '0' where organization = '%s';",organizationName);

		// �޸ľ���֯
		lenOfSql += snprintf(sql+lenOfSql,sizeof(sql)-lenOfSql,"update organization set organizationName = '%s',%s where organizationName = '%s';",newOrganizationName,content,organizationName);

		// �޸��ϼ���֯Ϊ����֯��
		lenOfSql += snprintf(sql+lenOfSql,sizeof(sql)-lenOfSql,"update organization set higherOrganization = '%s' where higherOrganization = '%s';",newOrganizationName,organizationName);

		// �޸���֯0Ϊ����֯��
		lenOfSql += snprintf(sql+lenOfSql,sizeof(sql)-lenOfSql,"update sysUser set organization = '%s' where organization = '0';",newOrganizationName);

		// ɾ����֯��Ϊ0����֯
		lenOfSql += snprintf(sql+lenOfSql,sizeof(sql)-lenOfSql,"delete from  organization  where organizationName = '0';");
	}
	else
	{
		// �޸���֯��Ϣ
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
