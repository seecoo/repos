
#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/dir.h>

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionREC.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "baseUIService.h"

/*********************************
�������:	0501
������:		���ز˵�
��������:	���ز˵�
**********************************/

#define MAX_TOTALNUM 4096


int UnionDealServiceCode0501(PUnionHsmGroupRec phsmGroupRec)
{
	int	i;
	int	j;
	int	ret;
	int	isManager = 0;
	int	len = 0;
	int	totalNum = 0;
	int	useFlag = 0;
	int	privilegeAccessNum = 0;
	int	paValueNum = 0;
	char	userID[64];
	char	userRoleList[128];
	char	menuName[128];
	char	menuDisplayName[128];
	int	menuType;
	int	menuLevel;
	char	menuParentName[128];
	char	menuOrder[64];
	char	menuIcon[128];
	int	isVisible = 0;
	int	isLeaf;
	int	seqNo;
	char	varNameOfEnabled[32];
	char	varValueOfEnabled[128];
	char	remark[128];
	char	tmpBuf[128];
	char	sql[1024];
	char	userRoleGrp[64][128];
	char	paValue[MAX_TOTALNUM][32];
	char	privilegeAccessValue[MAX_TOTALNUM][32];
	char	paType[MAX_TOTALNUM][32];
	int	privilegeOperation = 0;
	char	roleOrUser[32];

	// ��ȡ�û�ID
	memset(userID,0,sizeof(userID));
	if ((ret = UnionReadRequestXMLPackageValue("head/userID",userID,sizeof(userID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0501:: UnionReadRequestXMLPackageValue[head/userID]!\n");
		return(ret);
	}
	
	// ��ȡ��;
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/useFlag",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0501:: UnionReadRequestXMLPackageValue[useFlag]!\n");
		return(ret);
	}
	useFlag = atoi(tmpBuf);

	// ��ȡ�˵�����
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/menuType",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0501:: UnionReadRequestXMLPackageValue[menuType]!\n");
		return(ret);
	}
	menuType = atoi(tmpBuf);

	// ��ѯ��ɫ
	memset(sql,0,sizeof(sql));
	sprintf(sql,"select userRoleList from sysUser where userID = '%s'",userID);

	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0501:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}

	UnionLocateXMLPackage("detail", 1);

	// ��ȡ��ɫ
	memset(userRoleList,0,sizeof(userRoleList));
	if ((ret = UnionReadXMLPackageValue("userRoleList",userRoleList,sizeof(userRoleList))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0501:: UnionReadXMLPackageValue[%s]!\n","userRoleList");
		return(ret);
	}

	// ϵͳ����Ա����ʾ��߲˵�
	//if ((strcmp(userRoleList,"01") == 0 || strcmp(userRoleList,"00") == 0) && (menuType == 1) && (useFlag == 1))
	// ���ݿͻ�����,��Ҫ�鿴����Ƿ�ɹ�,�����ǰ̨������߲˵�����ʾ.
	if ((strcmp(userRoleList,"01") == 0) && (menuType == 1) && (useFlag == 1))
		goto systemManager;

	totalNum = 0;
	// ƴ�����崮
	memset(userRoleGrp,0,sizeof(userRoleGrp));
	if ((totalNum = UnionSeprateVarStrIntoVarGrp(userRoleList,strlen(userRoleList),',',userRoleGrp,64)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0501:: UnionSeprateVarStrIntoVarGrp[%s]!\n",userRoleList);
		return(totalNum);
	}

	memset(userRoleList,0,sizeof(userRoleList));
	len = 0;
	for (j = 0; j < totalNum; j++)
	{
		if (memcmp(userRoleGrp[j],"01",2) == 0)		// ϵͳ����Ա
			isManager = 1;
		len += sprintf(userRoleList + len,"'%s',",userRoleGrp[j]);
	}
	if (userRoleList[len - 1] == ',')
		userRoleList[len - 1] = '\0';	
	len = 0;
	totalNum = 0;

	// ��ѯ�û����µ���Ȩ����
	memset(sql,0,sizeof(sql));
	//sprintf(sql,"select distinct(privilegeAccessValue) from privilege where ((privilegeMaster = 'USER' and privilegeMasterValue = '%s') or (privilegeMaster = 'ROLE' and privilegeMasterValue in (%s))) and privilegeOperation = 1 and privilegeAccess = 'MENU'",userID,userRoleList); 
	sprintf(sql,"select distinct privilegeAccessValue, privilegeMaster from privilege where ((privilegeMaster = 'USER' and privilegeMasterValue = '%s') or (privilegeMaster = 'ROLE' and privilegeMasterValue in (%s))) and privilegeOperation = 1 and privilegeAccess = 'MENU'",userID,userRoleList); 

	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0501:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}
	else if(ret > 0)
	{
		// ��ȡ������   
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("totalNum",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0501:: UnionReadXMLPackageValue[%s]!\n","totalNum");
			return(ret);
		}
	
		paValueNum = atoi(tmpBuf);
	}
	else
		paValueNum = 0;

	// ������鷶Χ
	if (paValueNum > MAX_TOTALNUM)
	{
		UnionUserErrLog("in UnionDealServiceCode0501:: paValueNum[%d] > MAX_TOTALNUM[%d]!\n",paValueNum,MAX_TOTALNUM);
		return(errCodeDatabaseMDL_MoreRecordFound);
	}
	// ��ȡ��Ȩ����
	for (i = 0; i < paValueNum; i++)
	{
		if ((ret = UnionLocateXMLPackage("detail", i+1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0501:: UnionLocateXMLPackage[%d]!\n",i);
			return(ret);	
		}
		memset(paValue[i],0,sizeof(paValue[i]));
		if ((ret = UnionReadXMLPackageValue("privilegeAccessValue",paValue[i],sizeof(paValue[i]))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0501:: UnionReadXMLPackageValue[%s] [%d]!\n","privilegeAccessValue",i);
			return(ret);
		}
		memset(paType[i],0,sizeof(paType[i]));
		if ((ret = UnionReadXMLPackageValue("privilegeMaster",paType[i],sizeof(paType[i]))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0501:: UnionReadXMLPackageValue[%s] [%d]!\n","privilegeMaster",i);
			return(ret);
		}
	}

	len = 0;
	memset(sql,0,sizeof(sql));
	len = sprintf(sql,"%s","select * from sysMenu where ");

	// 0������
	// 1����߲˵�
	// 2����ݲ˵�
	if ((menuType == 1) || (menuType == 2))
		len += sprintf(sql + len,"menuType = %d ",menuType);
	else
		len += sprintf(sql + len,"1 = 1 ");
		
	// 1����ʾ
	// 2������
	// 3: Ȩ�޹���
	if ((useFlag == 1) || (useFlag == 3))
		len += sprintf(sql + len,"and isVisible = 1 ");

	len += sprintf(sql + len,"order by menuLevel,seqNo");

	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0501:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}

	// ��ȡ������	
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("totalNum",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0501:: UnionReadXMLPackageValue[%s]!\n","totalNum");
		return(ret);
	}
	totalNum = atoi(tmpBuf);
	
	UnionSetResponseXMLPackageValue("body/totalNum", "0");
	
	for (i = 1; i <= totalNum; i++)
	{
		if ((ret = UnionLocateXMLPackage("detail", i)) < 0)
		{
		        UnionUserErrLog("in UnionDealService0501:: totalNum[%d]\n",totalNum);	
			return(ret);	
		}
	
		// �˵��� 
		memset(menuName,0,sizeof(menuName));
		if ((ret = UnionReadXMLPackageValue("menuName",menuName,sizeof(menuName))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0501:: UnionReadXMLPackageValue[menuName]!\n");
			return(ret);
		}

		// �˵���ʾ��
		memset(menuDisplayName,0,sizeof(menuDisplayName));
		if ((ret = UnionReadXMLPackageValue("menuDisplayName",menuDisplayName,sizeof(menuDisplayName))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0501:: UnionReadXMLPackageValue[menuDisplayName]!\n");
			return(ret);
		}

		// �˵�����
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("menuType",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0501:: UnionReadXMLPackageValue[menuType]!\n");
			return(ret);
		}
		menuType = atoi(tmpBuf);
		
		// �˵�����
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("menuLevel",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0501:: UnionReadXMLPackageValue[menuLevel]!\n");
			return(ret);
		}
		menuLevel = atoi(tmpBuf);
	
		// ���˵�
		memset(menuParentName,0,sizeof(menuParentName));
		if ((ret = UnionReadXMLPackageValue("menuParentName",menuParentName,sizeof(menuParentName))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0501:: UnionReadXMLPackageValue[menuParentName]!\n");
			return(ret);
		}

		// ��Ҷ��
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("isLeaf",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0501:: UnionReadXMLPackageValue[isLeaf]!\n");
			return(ret);
		}
		isLeaf = atoi(tmpBuf);

		// ˳���
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("seqNo",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0501:: UnionReadXMLPackageValue[seqNo]!\n");
			return(ret);
		}
		seqNo = atoi(tmpBuf);

		// ���ñ�����
		memset(varNameOfEnabled,0,sizeof(varNameOfEnabled));
		if ((ret = UnionReadXMLPackageValue("varNameOfEnabled",varNameOfEnabled,sizeof(varNameOfEnabled))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0501:: UnionReadXMLPackageValue[varNameOfEnabled]!\n");
			return(ret);
		}

		// ���ñ���ֵ
		memset(varValueOfEnabled,0,sizeof(varValueOfEnabled));
		if ((ret = UnionReadXMLPackageValue("varValueOfEnabled",varValueOfEnabled,sizeof(varValueOfEnabled))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0501:: UnionReadXMLPackageValue[varValueOfEnabled]!\n");
			return(ret);
		}

		// �˵�ͼ��
		memset(menuIcon,0,sizeof(menuIcon));
		if ((ret = UnionReadXMLPackageValue("menuIcon",menuIcon,sizeof(menuIcon))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0501:: UnionReadXMLPackageValue[menuIcon]!\n");
			return(ret);
		}

		if(useFlag == 1 || useFlag == 2)
		{
			// �˵�����
			memset(menuOrder,0,sizeof(menuOrder));
			if ((ret = UnionReadXMLPackageValue("menuOrder",menuOrder,sizeof(menuOrder))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0501:: UnionReadXMLPackageValue[menuOrder]!\n");
				return(ret);
			}
		}

		if ((useFlag == 1) || (useFlag == 3))
		{
			// �������˵��Ƿ���Ч
			if ((strlen(varNameOfEnabled) > 0) && (strlen(varValueOfEnabled) > 0))
			{
				//UnionLog("in UnionDealServiceCode0501:: varNameOfEnabled[%s] varValueOfEnabled[%s]\n",varNameOfEnabled,varValueOfEnabled);
				if (UnionReadStringTypeRECVar(varNameOfEnabled) != NULL)
				{
					if (strcmp(UnionReadStringTypeRECVar(varNameOfEnabled),varValueOfEnabled) != 0)
						continue;
				}
			}
		}
		
		if (useFlag == 2)
		{
			// �Ƿ�ɼ� 
			memset(tmpBuf,0,sizeof(tmpBuf));
			if ((ret = UnionReadXMLPackageValue("isVisible",tmpBuf,sizeof(tmpBuf))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0501:: UnionReadXMLPackageValue[isVisible]!\n");
				return(ret);
			}
			isVisible = atoi(tmpBuf);
	
			// ��ע
			memset(remark,0,sizeof(remark));
			if ((ret = UnionReadXMLPackageValue("remark",remark,sizeof(remark))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0501:: UnionReadXMLPackageValue[remark]!\n");
				return(ret);
			}
		}

		// �����Ȩ��
		privilegeOperation = 0;
		memset(roleOrUser,0,sizeof(roleOrUser));
		memset(privilegeAccessValue[privilegeAccessNum],0,sizeof(privilegeAccessValue[privilegeAccessNum]));
		for (j = 0; j < paValueNum; j++)
		{
			if (strcmp(menuName,paValue[j]) == 0)
			{
				strcpy(privilegeAccessValue[privilegeAccessNum],menuName);
				privilegeOperation = 1;
				if (strcmp("ROLE",paType[j]) == 0)	// ROLE
					strcpy(roleOrUser, "1");
				else if (strcmp("USER",paType[j]) == 0)	// USER
					strcpy(roleOrUser, "2");
				else
					strcpy(roleOrUser, "9");
				break;
			}			
		}

		// ��Ȩ�˵�,ֻ��Ȩ��ϵͳ����Ա
		if (strcmp(menuName,"3") == 0)
		{
			memset(privilegeAccessValue[privilegeAccessNum],0,sizeof(privilegeAccessValue[privilegeAccessNum]));
			if (isManager == 1)
			{
				strcpy(privilegeAccessValue[privilegeAccessNum],menuName);
				privilegeOperation = 1;
				strcpy(roleOrUser, "1");
			}
			else
			{
				privilegeOperation = 0;
			}
		}

		if ((useFlag == 1) || (useFlag == 3))
		{
			// �˵��������1ʱ����鸸�ڵ��Ƿ��Ѿ���Ȩ
			if ((menuLevel > 1) && (privilegeOperation == 1))
			{
				privilegeOperation = 0;
				for (j = 0; j < paValueNum; j++)
				{
					if (strcmp(menuParentName,privilegeAccessValue[j]) == 0)
					{
						privilegeOperation = 1;
						break;
					}
				}
			}

			if ((useFlag == 1) && (privilegeOperation == 0))
			{
				continue;
			}
		}

		// ϵͳ����Ա��ʾ�Ŀ�ݲ˵�
		if (strcmp(userRoleList,"01") == 0 && (menuType == 2) && (useFlag == 1))
		{
			if (atoi(menuName) > 3 || atoi(menuParentName) > 3)
				continue;
		}


		if ((ret = UnionLocateResponseNewXMLPackage("body/detail", privilegeAccessNum + 1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0501:: UnionLocateResponseNewXMLPackage!\n");
			return(ret);
		}

		if ((ret = UnionSetResponseXMLPackageValue("menuName",menuName)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0501:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"menuName",menuName);
			return(ret);
		}

		if ((ret = UnionSetResponseXMLPackageValue("menuDisplayName",menuDisplayName)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0501:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"menuDisplayName",menuDisplayName);
			return(ret);
		}

		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%d",menuType);
		if ((ret = UnionSetResponseXMLPackageValue("menuType",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0501:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"menuType",tmpBuf);
			return(ret);
		}

		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%d",menuLevel);
		if ((ret = UnionSetResponseXMLPackageValue("menuLevel",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0501:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"menuLevel",tmpBuf);
			return(ret);
		}

		if ((ret = UnionSetResponseXMLPackageValue("menuParentName",menuParentName)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0501:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"menuParentName",menuParentName);
			return(ret);
		}

		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%d",isLeaf);
		if ((ret = UnionSetResponseXMLPackageValue("isLeaf",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0501:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"isLeaf",tmpBuf);
			return(ret);
		}

		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%d",seqNo);
		if ((ret = UnionSetResponseXMLPackageValue("seqNo",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0501:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"seqNo",tmpBuf);
			return(ret);
		}

		if ((ret = UnionSetResponseXMLPackageValue("menuIcon",menuIcon)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0501:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",totalNum,"menuIcon",menuIcon);
			return(ret);
		}

		if(useFlag == 1 || useFlag == 2)
		{
			if ((ret = UnionSetResponseXMLPackageValue("menuOrder",menuOrder)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0501:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"menuOrder",menuOrder);
				return(ret);
			}
		}

		if(useFlag == 2)
		{
			memset(tmpBuf,0,sizeof(tmpBuf));
			sprintf(tmpBuf,"%d",isVisible);
			if ((ret = UnionSetResponseXMLPackageValue("isVisible",tmpBuf)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0501:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"isVisible",tmpBuf);
				return(ret);
			}
		
			if ((ret = UnionSetResponseXMLPackageValue("varNameOfEnabled",varNameOfEnabled)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0501:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"varNameOfEnabled",varNameOfEnabled);
				return(ret);
			}

			if ((ret = UnionSetResponseXMLPackageValue("varValueOfEnabled",varValueOfEnabled)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0501:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"varValueOfEnabled",varValueOfEnabled);
				return(ret);
			}

			if ((ret = UnionSetResponseXMLPackageValue("remark",remark)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0501:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"remark",remark);
				return(ret);
			}
		}

		privilegeAccessNum++;
	}
systemManager:
	UnionLocateResponseXMLPackage("", 0);
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d",privilegeAccessNum);
	UnionSetResponseXMLPackageValue("body/totalNum", tmpBuf);

	return 0;
}
