
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
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "baseUIService.h"

/*********************************
服务代码:	0502
服务名:		下载按钮
功能描述:	下载按钮
**********************************/

#define MAX_TOTALNUM 4096



int UnionDealServiceCode0502(PUnionHsmGroupRec phsmGroupRec)
{
	int	i;
	int	j;
	int	ret;
	int	len = 0;
	int	totalNum = 0;
	int	btnTotalNum = 0;
	int	useFlag = 0;
	int	paValueNum = 0;
	int	privilegeAccessNum = 0;
	char	userID[64];
	char	userRoleList[128];
	char	tmpBuf[128];
	char	sql[1024];
	char	userRoleListGrp[64][128];
	char	paValue[MAX_TOTALNUM][16];
	char	paType[MAX_TOTALNUM][16];
	int	privilegeOperation = 0;
	char	roleOrUser[32];
	char	viewName[128];
	PUnionSysButton      pFirstPos,pBtnList;
	char	btnArrStr[1024];

	// 读取用户ID
	memset(userID,0,sizeof(userID));
	if ((ret = UnionReadRequestXMLPackageValue("head/userID",userID,sizeof(userID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0502:: UnionReadRequestXMLPackageValue[head/userID]!\n");
		return(ret);
	}

	// 读取视图名
	memset(viewName,0,sizeof(viewName));
	if ((ret = UnionReadRequestXMLPackageValue("body/viewName",viewName,sizeof(viewName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0502:: UnionReadRequestXMLPackageValue[viewName]!\n");
		return(ret);
	}

	// 读取用途
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/useFlag",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0502:: UnionReadRequestXMLPackageValue[useFlag]!\n");
		return(ret);
	}
	useFlag = atoi(tmpBuf);
	
	
	//占住位置
	UnionSetResponseXMLPackageValue("body/totalNum", "0");
	
	//读取视图下的对应按钮
	len = sprintf(sql,"select * from sysButton where viewName = %s",viewName);

	// 1：显示
	// 2：管理
	// 3: 权限管理
	if ((useFlag == 1) || (useFlag == 3))
		len += sprintf(sql + len," and isVisible = 1");

	len += sprintf(sql + len," order by seqNo");
	
	sql[len] = 0;

	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0502:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}
	else if(ret > 0)
	{
		UnionLog("in UnionDealServiceCode0502:: UnionSelectRealDBRecord ret = [%d]!\n",ret);
		// 读取总数量
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("totalNum",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0502:: UnionReadXMLPackageValue[%s]!\n","totalNum");
			return(ret);
		}
		btnTotalNum = atoi(tmpBuf);
		
		pFirstPos = pBtnList = (PUnionSysButton)calloc(btnTotalNum,sizeof(TUnionSysButton));
	        if(!pFirstPos)
	        {
	
	                UnionUserErrLog("in UnionDealServiceCode0502:: calloc pBtnList error!\n");
	                return(errCodeParameter);
	        }
	}
	else
	{
		
		btnTotalNum = 0;
		return 0;
	}
	
	
	len = 0;
	
	for (i = 1; i <= btnTotalNum; i++)
	{
		if ((ret = UnionLocateXMLPackage("detail", i)) < 0)
		{
			UnionUserErrLog("in UnionDealService0502:: totalNum[%d]\n",totalNum);
			return(ret);
		}

		// 按钮名
		if ((ret = UnionReadXMLPackageValue("btnName",(*pBtnList).btnName, sizeof((*pBtnList).btnName))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0502:: UnionReadXMLPackageValue[btnName]!\n");
			free(pFirstPos);
			return(ret);
		}
		
		//拼接视图按钮串 privilegeAccessValue
		if(i == btnTotalNum)
		{
			len += sprintf(btnArrStr + len,"privilegeAccessValue=%s",(*pBtnList).btnName);
			btnArrStr[len] = 0;
			
		}
		else
		{
			len += sprintf(btnArrStr + len,"privilegeAccessValue=%s or ",(*pBtnList).btnName);
		}
		

		// 按钮显示名
		if ((ret = UnionReadXMLPackageValue("btnDisplayName",(*pBtnList).btnDisplayName, sizeof((*pBtnList).btnDisplayName))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0502:: UnionReadXMLPackageValue[btnDisplayName]!\n");
			free(pFirstPos);
			return(ret);
		}

		// 按钮命令
		if ((ret = UnionReadXMLPackageValue("btnOrder",(*pBtnList).btnOrder, sizeof((*pBtnList).btnOrder))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0502:: UnionReadXMLPackageValue[btnOrder]!\n");
			free(pFirstPos);
			return(ret);
		}

		// 操作视图名
		if ((ret = UnionReadXMLPackageValue("operViewName",(*pBtnList).operViewName, sizeof((*pBtnList).operViewName))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0502:: UnionReadXMLPackageValue[operViewName]!\n");
			free(pFirstPos);
			return(ret);
		}

		// 按钮图标
		if ((ret = UnionReadXMLPackageValue("btnIcon",(*pBtnList).btnIcon, sizeof((*pBtnList).btnIcon))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0502:: UnionReadXMLPackageValue[btnIcon]!\n");
			free(pFirstPos);
			return(ret);
		}

		// 按钮条件
		if ((ret = UnionReadXMLPackageValue("btnCondition",(*pBtnList).btnCondition, sizeof((*pBtnList).btnCondition))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0502:: UnionReadXMLPackageValue[btnCondition]!\n");
			free(pFirstPos);
			return(ret);
		}

		// 需要编辑
		if ((ret = UnionReadXMLPackageValue("isEdit",(*pBtnList).isEdit, sizeof((*pBtnList).isEdit))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0502:: UnionReadXMLPackageValue[isEdit]!\n");
			free(pFirstPos);
			return(ret);
		}

		// 选中行
		if ((ret = UnionReadXMLPackageValue("selectedRow",(*pBtnList).selectedRow, sizeof((*pBtnList).selectedRow))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0502:: UnionReadXMLPackageValue[selectedRow]!\n");
			free(pFirstPos);
			return(ret);
		}

		// 文件传输
		if ((ret = UnionReadXMLPackageValue("fileTransfer",(*pBtnList).fileTransfer, sizeof((*pBtnList).fileTransfer))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0502:: UnionReadXMLPackageValue[fileTransfer]!\n");
			free(pFirstPos);
			return(ret);
		}

		// 操作前提示
		if ((ret = UnionReadXMLPackageValue("promptBox",(*pBtnList).promptBox, sizeof((*pBtnList).promptBox))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0502:: UnionReadXMLPackageValue[promptBox]!\n");
			free(pFirstPos);
			return(ret);
		}

		// 操作后提示
		if ((ret = UnionReadXMLPackageValue("confirmBox",(*pBtnList).confirmBox, sizeof((*pBtnList).confirmBox))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0502:: UnionReadXMLPackageValue[confirmBox]!\n");
			free(pFirstPos);
			return(ret);
		}

		// 是否可见
		if ((ret = UnionReadXMLPackageValue("isVisible",(*pBtnList).isVisible, sizeof((*pBtnList).isVisible))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0502:: UnionReadXMLPackageValue[isVisible]!\n");
			free(pFirstPos);
			return(ret);
		}

		// 需要编辑
		if ((ret = UnionReadXMLPackageValue("isApproval",(*pBtnList).isApproval, sizeof((*pBtnList).isApproval))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0502:: UnionReadXMLPackageValue[isApproval]!\n");
			free(pFirstPos);
			return(ret);
		}

		// 顺序号
		if ((ret = UnionReadXMLPackageValue("seqNo",(*pBtnList).seqNo, sizeof((*pBtnList).seqNo))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0502:: UnionReadXMLPackageValue[seqNo]!\n");
			free(pFirstPos);
			return(ret);
		}

                
		// 备注
		if ((ret = UnionReadXMLPackageValue("remark",(*pBtnList).remark, sizeof((*pBtnList).remark))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0502:: UnionReadXMLPackageValue[remark]!\n");
			free(pFirstPos);
			return(ret);
		}
		pBtnList++;
	}
	
	

	// 读取角色
	memset(sql,0,sizeof(sql));
	sprintf(sql,"select userRoleList from sysUser where userID = '%s'",userID);

	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0502:: UnionSelectRealDBRecord[%s]!\n",sql);
		free(pFirstPos);
		return(ret);
	}
	UnionLog("in UnionDealServiceCode0502:: UnionSelectRealDBRecord ret = [%d]!\n",ret);

	UnionLocateXMLPackage("detail", 1);

	// 获取角色
	memset(userRoleList,0,sizeof(userRoleList));
	if ((ret = UnionReadXMLPackageValue("userRoleList",userRoleList,sizeof(userRoleList))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0502:: UnionReadXMLPackageValue[%s]!\n","userRoleList");
		free(pFirstPos);
		return(ret);
	}

	// 拼分域定义串
	memset(userRoleListGrp,0,sizeof(userRoleListGrp));
	totalNum = 0;
	if ((totalNum = UnionSeprateVarStrIntoVarGrp(userRoleList,strlen(userRoleList),',',userRoleListGrp,64)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0502:: UnionSeprateVarStrIntoVarGrp[%s]!\n",userRoleList);
		free(pFirstPos);
		return(totalNum);
	}

	memset(userRoleList,0,sizeof(userRoleList));
	len = 0;
	for (j = 0; j < totalNum; j++)
	{
		len += sprintf(userRoleList + len,"'%s',",userRoleListGrp[j]);
	}
	if (userRoleList[len - 1] == ',')
		userRoleList[len - 1] = '\0';	
	len = 0;
	totalNum = 0;

	// 查询用户名下的授权对象
	memset(sql,0,sizeof(sql));
	sprintf(sql,"select distinct privilegeAccessValue, privilegeMaster from privilege where ((privilegeMaster = 'USER' and privilegeMasterValue = '%s') or (privilegeMaster = 'ROLE' and privilegeMasterValue in (%s))) and privilegeOperation = 1 and privilegeAccess = 'BUTTON' and (%s)",userID,userRoleList,btnArrStr); 

	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0502:: UnionSelectRealDBRecord[%s]!\n",sql);
		free(pFirstPos);
		return(ret);
	}
	else if(ret > 0)
	{
		// 读取总数量   
		UnionLog("in UnionDealServiceCode0502:: UnionSelectRealDBRecord ret = [%d]!\n",ret);
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("totalNum",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0502:: UnionReadXMLPackageValue[%s]!\n","totalNum");
			UnionLogXMLPackage();
			free(pFirstPos);
			return(ret);
		}
	
		paValueNum = atoi(tmpBuf);
	}
	else
		paValueNum = 0;
	
	// 检查数组范围
	if (paValueNum > MAX_TOTALNUM)
	{
		UnionUserErrLog("in UnionDealServiceCode0502:: paValueNum[%d] > MAX_TOTALNUM[%d]!\n",paValueNum,MAX_TOTALNUM);
		free(pFirstPos);
		return(errCodeDatabaseMDL_MoreRecordFound);
	}
	
	// 读取授权对象
	for (i = 0; i < paValueNum; i++)
	{
		if ((ret = UnionLocateXMLPackage("detail", i+1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0502:: UnionLocateXMLPackage[%d]!\n",i);
			return(ret);
		}
		memset(paValue[i],0,sizeof(paValue[i]));
		if ((ret = UnionReadXMLPackageValue("privilegeAccessValue",paValue[i],sizeof(paValue[i]))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0502:: UnionReadXMLPackageValue[%s] [%d]!\n","privilegeAccessValue",i);
			return(ret);
		}
		memset(paType[i],0,sizeof(paType[i]));
		if ((ret = UnionReadXMLPackageValue("privilegeMaster",paType[i],sizeof(paType[i]))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0502:: UnionReadXMLPackageValue[%s] [%d]!\n","privilegeMaster",i);
			return(ret);
		}
	}
	
	//获取授权的按钮
	pBtnList = pFirstPos;
	for(i = 0; i < btnTotalNum; i++)
	{
		// 检测授权表
		privilegeOperation = 0;
		for (j = 0; j < paValueNum; j++)
		{
			if (strcmp((*pBtnList).btnName,paValue[j]) == 0)
			{
				privilegeOperation = 1;
				if (strcmp("ROLE",paType[j]) == 0)	// ROLE
					strcpy(roleOrUser, "1");
				else if (strcmp("USER",paType[j]) == 0)	// USER
					strcpy(roleOrUser, "2");
				else	// unknow
					strcpy(roleOrUser, "9");
				break;
			}
			
			
		}
		
		if(privilegeOperation == 0 && useFlag == 1)
		{
			//add by linxj 20160407	解决按钮下载失败的问题
			pBtnList++;
			continue;
		}
		
		
		//增加到返回报文中
		if ((ret = UnionLocateResponseNewXMLPackage("body/detail", privilegeAccessNum + 1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0502:: UnionLocateResponseNewXMLPackage!\n");
			free(pFirstPos);
			return(ret);
		}

		if ((ret = UnionSetResponseXMLPackageValue("btnName",(*pBtnList).btnName)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0502:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"btnName",(*pBtnList).btnName);
			free(pFirstPos);
			return(ret);
		}

		if ((ret = UnionSetResponseXMLPackageValue("btnDisplayName",(*pBtnList).btnDisplayName)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0502:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"btnDisplayName",(*pBtnList).btnDisplayName);
			free(pFirstPos);
			return(ret);
		}

		if(useFlag == 1 || useFlag == 2)
		{
			if ((ret = UnionSetResponseXMLPackageValue("btnOrder",(*pBtnList).btnOrder)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0502:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"btnOrder",(*pBtnList).btnOrder);
				free(pFirstPos);
				return(ret);
			}

			if ((ret = UnionSetResponseXMLPackageValue("operViewName",(*pBtnList).operViewName)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0502:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"operViewName",(*pBtnList).operViewName);
				free(pFirstPos);
				return(ret);
			}
	
			if ((ret = UnionSetResponseXMLPackageValue("btnIcon",(*pBtnList).btnIcon)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0502:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"btnIcon",(*pBtnList).btnIcon);
				free(pFirstPos);
				return(ret);
			}

			if ((ret = UnionSetResponseXMLPackageValue("btnCondition",(*pBtnList).btnCondition)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0502:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"btnCondition",(*pBtnList).btnCondition);
				free(pFirstPos);
				return(ret);
			}

			if ((ret = UnionSetResponseXMLPackageValue("isEdit",(*pBtnList).isEdit)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0502:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"isEdit",(*pBtnList).isEdit);
				free(pFirstPos);
				return(ret);
			}

			if ((ret = UnionSetResponseXMLPackageValue("selectedRow",(*pBtnList).selectedRow)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0502:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"selectedRow",(*pBtnList).selectedRow);
				free(pFirstPos);
				return(ret);
			}

			if ((ret = UnionSetResponseXMLPackageValue("fileTransfer",(*pBtnList).fileTransfer)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0502:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"fileTransfer",(*pBtnList).fileTransfer);
				free(pFirstPos);
				return(ret);
			}
			if ((ret = UnionSetResponseXMLPackageValue("promptBox",(*pBtnList).promptBox)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0502:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"promptBox",(*pBtnList).promptBox);
				free(pFirstPos);
				return(ret);
			}
	
			if ((ret = UnionSetResponseXMLPackageValue("confirmBox",(*pBtnList).confirmBox)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0502:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"confirmBox",(*pBtnList).confirmBox);
				free(pFirstPos);
				return(ret);
			}
			if ((ret = UnionSetResponseXMLPackageValue("isApproval",(*pBtnList).isApproval)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0502:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"isApproval",(*pBtnList).isApproval);
				free(pFirstPos);
				return(ret);
			}

			if ((ret = UnionSetResponseXMLPackageValue("seqNo",(*pBtnList).seqNo)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0502:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"seqNo",(*pBtnList).seqNo);
				free(pFirstPos);
				return(ret);
			}

			 
	
			if ((ret = UnionSetResponseXMLPackageValue("remark",(*pBtnList).remark)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0502:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"remark",(*pBtnList).remark);
				free(pFirstPos);
				return(ret);
			}
		}
		
		if(useFlag == 2)
		{
			if ((ret = UnionSetResponseXMLPackageValue("isVisible",(*pBtnList).isVisible)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0502:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"isVisible",(*pBtnList).isVisible);
				free(pFirstPos);
				return(ret);
			}
			if ((ret = UnionSetResponseXMLPackageValue("remark",(*pBtnList).remark)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0502:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"remark",(*pBtnList).remark);
				free(pFirstPos);
				return(ret);
			}
		}
		privilegeAccessNum++;
		pBtnList++;
	}
	
	free(pFirstPos);
	
	UnionLocateResponseXMLPackage("", 0);
	len = sprintf(tmpBuf,"%d",privilegeAccessNum);
	tmpBuf[len] = 0;
	UnionSetResponseXMLPackageValue("body/totalNum", tmpBuf);

	return 0;
}
