//	Author:		������
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
�������:	0301
������:		���Ӽ�¼
��������:	���Ӽ�¼
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

	// ����
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

	// ֻȡ��һ����
	if ((ptr = strchr(tableName,',')))
		*ptr = 0;

	// ��ȡ���嵥
	memset(fieldList,0,sizeof(fieldList));
	if ((ret = UnionReadRequestXMLPackageValue("body/fieldList",fieldList,sizeof(fieldList))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0301:: UnionReadRequestXMLPackageValue[%s]!\n","body/fieldList");
		return(ret);
	}

	// ƴ�����崮
	memset(fieldGrp,0,sizeof(fieldGrp));
	if ((fieldNum = UnionSeprateVarStrIntoVarGrp(fieldList,strlen(fieldList),',',fieldGrp,64)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0301:: UnionSeprateVarStrIntoVarGrp[%s]!\n",fieldList);
		return(fieldNum);
	}

	// ��ȡ��Ӧ��������ֵ
	for (i = 0; i < fieldNum; i++)
	{
		memset(fieldValue[i],0,sizeof(fieldValue[i]));
		memset(tmpBuf,0,sizeof(tmpBuf));
		
		// ȥ��ǰ��ո�
		UnionFilterHeadAndTailBlank(fieldGrp[i]);
		
		sprintf(tmpBuf,"body/field/%s",fieldGrp[i]);
		if ((ret = UnionReadRequestXMLPackageValue(tmpBuf,fieldValue[i],sizeof(fieldValue[i]))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0301:: UnionReadRequestXMLPackageValue[%s] [%d]!\n",tmpBuf,i);
			return(ret);
		}
		// ȥ��ǰ��ո�
		UnionFilterHeadAndTailBlank(fieldValue[i]);
		
		// �����ݿ���ַ����еĵ������滻Ϊ2��������
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionTranslateSpecCharInDBStr(fieldValue[i],strlen(fieldValue[i]),tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0301:: UnionTranslateSpecCharInDBStr tmpBuf[%s][%d]!\n",tmpBuf,(int)sizeof(tmpBuf));
			return(ret);
		}
		memset(fieldValue[i],0,sizeof(fieldValue[i]));
		strcpy(fieldValue[i],tmpBuf);
	}

	// ����Ƿ��ڶ�����
	if ((ptableDef = UnionFindTableDef(tableName)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode0301:: UnionFindTableDef[%s] not found!\n",tableName);
		return(errCodeDatabaseMDL_TableNotFound);
	}

	// ��ѯ��ȡ�������Ƿ��ڶ����д���
	memset(fieldList,0,sizeof(fieldList));
	memset(fieldValueList,0,sizeof(fieldValueList));
	for (i = 0; i < fieldNum; i++)
	{
		for (j = 0; j < ptableDef->fieldNum; j++)
		{
			// ����ֶ��Ƿ�ǰ���б�������
			if ((ptr = strchr(fieldGrp[i],'.')))
			{
				memset(tmpBuf,0,sizeof(tmpBuf));
				strcpy(tmpBuf,ptr +1);
				memset(fieldGrp[i],0,sizeof(fieldGrp[i]));
				strcpy(fieldGrp[i],tmpBuf);
			}
			
			if (strcmp(fieldGrp[i],ptableDef->fieldDef[j].fieldName) != 0)
				continue;

			// �����
			if (strcmp(tableName,"unionTask") == 0)
			{
				if (strcmp(fieldGrp[i],"taskName") == 0)
				{
					if (strlen(fieldValue[i]) < 5 || UnionIsDigitString(fieldValue[i]))
					{
						UnionUserErrLog("in UnionDealServiceCode0301:: %s[%s] length < 5 or is DigitString!\n",fieldGrp[i],fieldValue[i]);
						UnionSetResponseRemark("���������Ȳ���С��5���Ҳ���Ϊ������");
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
				case 1 :	// �ַ���
				case 4 :	// ������
				case 7 :	// ʱ��
					
					// �������Ĵ�С�Ƿ񳬹���󳤶�
					if (strlen(fieldValue[i]) > ptableFieldDef->fieldSize)
					{
						UnionUserErrLog("in UnionDealServiceCode0301:: field[%s] length[%s] > max length[%.0f]!\n",fieldGrp[i],fieldValue[i], ptableFieldDef->fieldSize);
						UnionSetResponseRemark("[%s:] ���볤��[%d] > ��󳤶�[%.0f]",ptableFieldDef->fieldChnName,strlen(fieldValue[i]),ptableFieldDef->fieldSize);
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
				case 2 :	// ����
				case 3 :	// ������
				case 5 :	// ������
					if (strlen(fieldValue[i]) > 0)
					{
						lenFieldName += sprintf(fieldList + lenFieldName,"%s,",fieldGrp[i]);
						lenValue += sprintf(fieldValueList + lenValue,"%s,",fieldValue[i]);
					}
					break;	
				case 6 :	// ������
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

					// �������Ĵ�С�Ƿ񳬹���󳤶�
					if (strlen(fieldValue[i]) > ptableFieldDef->fieldSize)
					{
						UnionUserErrLog("in UnionDealServiceCode0301:: field[%s] length[%s] > max length[%.0f]!\n",fieldGrp[i],fieldValue[i], ptableFieldDef->fieldSize);
						UnionSetResponseRemark("[%s:] ���볤��[%d] > ��󳤶�[%.0f]",ptableFieldDef->fieldChnName,strlen(fieldValue[i]),ptableFieldDef->fieldSize);
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

	// ȥ������","
	if (lenFieldName > 0)
		fieldList[lenFieldName - 1] = 0;
	if (lenValue > 0)
		fieldValueList[lenValue - 1] = 0;

	// ����sql�������
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
	
	// ���¹����ڴ�
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
