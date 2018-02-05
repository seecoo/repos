//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "unionHsmCmd.h"
#include "unionHsmCmdVersion.h"
#include "unionREC.h"
#include "baseUIService.h"

/***************************************
服务代码:	0H10
服务名:		恢复私钥
功能描述:	恢复私钥
***************************************/

int UnionImportAndCheckVK(PUnionHsmGroupRec phsmGroupRec,int isRecoverByGroup,int algorithmID,char *hsmGroupID,char *vkIndex,int lenOfVK,char *vkValue,char *oriSign,char *ipAddr);
int UnionDealServiceCode0H10(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret = 0;
	char				filePath[512];
	char				oriSign[4096];
	//char				sql[512];
	char				*ptr = NULL;
	FILE				*fp = NULL;
	char				fileFullName[512];
	char				fileName[128];
	char				tmpNum[2560];
	char				vkInfo[2560];
	//char				algorithmID[21][32];
	char				algorithmID[42][32];
	char				vkIndexList[128];
	//char				vkIndexGrp[21][128];
	char				vkIndexGrp[42][128];
	char				hsmGroupID[32];
	char				vkValue[2560];
	int				j = 0,i = 0;
	int				len = 0;
	int				vkIndexNum = 0;
	int				lenOfVK = 0;
	int				isFailed = 0;
	int				failNum = 0;
	int				totalNum = 0;
	char				fieldListChnName[128];
	char				fieldList[128];
	char				ipAddr[64];
	int				vkIdx = 0;
	int				isRecoverByGroup;
	
	// 读取文件路径
	memset(filePath,0,sizeof(filePath));
	if ((ptr = UnionReadStringTypeRECVar("defaultDirOfFileReceiver")) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode0H10:: UnionReadStringTypeRECVar[%s]!\n","defaultDirOfFileReceiver");
		return(errCodeRECMDL_VarNotExists);
	}

	UnionReadDirFromStr(ptr,-1,filePath);

	// 读取文件名
	if ((ret = UnionReadRequestXMLPackageValue("body/fileName",fileName,sizeof(fileName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H10:: UnionReadRequestXMLPackageValue[%s]!\n","body/fileName");
		return(ret);
	}
	fileName[ret] = 0;
	UnionFilterHeadAndTailBlank(fileName);
	if (strlen(fileName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H10:: fileName can not be null!\n");
		UnionSetResponseRemark("文件名不能为空");
		return(errCodeParameter);
	}
	

	// 密码机组
	memset(hsmGroupID,0,sizeof(hsmGroupID));
	if ((ret = UnionReadRequestXMLPackageValue("body/hsmGroupID",hsmGroupID,sizeof(hsmGroupID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H10:: UnionReadRequestXMLPackageValue[%s]!\n","body/hsmGroupID");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(hsmGroupID);
	if (strlen(hsmGroupID) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H10:: hsmGroupID can not be null!\n");
		UnionSetResponseRemark("密码机组不能为空");
		return(errCodeParameter);
	}

	// add by chenqy 20151021
        // 密码机ip (可以为空)
        memset(ipAddr,0,sizeof(ipAddr));
        UnionReadRequestXMLPackageValue("body/ipAddr",ipAddr,sizeof(ipAddr));
        UnionFilterHeadAndTailBlank(ipAddr);
	
	if(strlen(ipAddr) == 0 || ipAddr == NULL)
	{
		isRecoverByGroup = 1;
	}
	else
	{
		isRecoverByGroup = 0;
	}
	//add end
	
	// 索引号
	if ((ret = UnionReadRequestXMLPackageValue("body/vkIndexList",vkIndexList,sizeof(vkIndexList))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H10:: UnionReadRequestXMLPackageValue[%s]!\n","body/vkIndexList");
		return(ret);
	}
	vkIndexList[ret] = 0;
	UnionFilterHeadAndTailBlank(vkIndexList);
	if (strlen(vkIndexList) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H10:: vkIndexList can not be null!\n");
		UnionSetResponseRemark("索引不能为空");
		return(errCodeParameter);
	}

	// 拼分域定义串
	memset(vkIndexGrp,0,sizeof(vkIndexGrp));
	// modify by lisq 20141216 增加国密索引21-42
	//if ((vkIndexNum = UnionSeprateVarStrIntoVarGrp(vkIndexList,strlen(vkIndexList),',',vkIndexGrp,21)) < 0)
	if ((vkIndexNum = UnionSeprateVarStrIntoVarGrp(vkIndexList,strlen(vkIndexList),',',vkIndexGrp,42)) < 0)
{
		UnionUserErrLog("in UnionDealServiceCode0H10:: UnionSeprateVarStrIntoVarGrp[%s]!\n",vkIndexList);
		return(vkIndexNum);
	}
/*
	for (i = 0; i < vkIndexNum; i++)
	{
		// 查询密钥信息
		// modify by lisq 20141216 增加国密索引21-42
		//len = sprintf(sql,"select * from asymmetricKeyDB where hsmGroupID = '%s' and vkIndex = '%s'",hsmGroupID,vkIndexGrp[i]);
		vkIdx = atoi(vkIndexGrp[i]);
		if (vkIdx > 20)
			vkIdx = (vkIdx - 21);

		len = sprintf(sql,"select * from asymmetricKeyDB where hsmGroupID = '%s' and vkIndex = '%02d'",hsmGroupID,vkIdx);
		// modify by lisq 20141216 end 增加国密索引21-42
		sql[len] = 0;
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0H10:: UnionSelectRealDBRecord sql[%s]!\n",sql);
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0H10:: keyName not found, sql[%s]!\n",sql);
			UnionSetResponseRemark("密钥不存在");
			return(errCodeDatabaseMDL_RecordNotFound);
		}

		if ((ret = UnionLocateXMLPackage("detail",1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0H10:: UnionLocateXMLPackage [%s]!\n","detail");
			return(ret);
		}
		
		// 读取密钥类型
		memset(algorithmID[i],0,sizeof(algorithmID[i]));
		if ((ret = UnionReadXMLPackageValue("algorithmID",algorithmID[i],sizeof(algorithmID[i]))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0H10:: UnionReadXMLPackageValue [%s]!\n","algorithmID");
			return(ret);
		}
	}
*/
	// 打开文件
	len = sprintf(fileFullName,"%s/%s",filePath,fileName);
	fileFullName[len] = 0;

	if ((fp = fopen(fileFullName,"r+")) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode0H10:: fopen[%s]!\n",fileFullName);
		return(errCodeUseOSErrCode);
	}

	// 设置显示报文体
	if ((ret = UnionSetResponseXMLPackageValue("head/displayBody","1")) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H10:: UnionSetResponseXMLPackageValue[%s]\n","body/displayBody");
		return(ret);
	}

	// 字段清单
	len = sprintf(fieldList,"ipAddr,vkIndex,status");
	fieldList[len] = 0;
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldList",fieldList)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H10:: UnionSetResponseXMLPackageValue[%s]\n","body/fieldList");
		return(ret);
	}

	// 字段清单中文名
	len = sprintf(fieldListChnName,"加密机IP,索引,恢复状态");
	fieldListChnName[len] = 0;
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldListChnName",fieldListChnName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H10:: UnionSetResponseXMLPackageValue[%s]\n","body/fieldListChnName");
		return(ret);
	}

	// 设置总数
	len = sprintf(tmpNum,"%d",vkIndexNum);
	tmpNum[len] = 0;
	if ((ret = UnionSetResponseXMLPackageValue("body/totalNum",tmpNum)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H10:: UnionSetResponseXMLPackageValue[%s]\n","body/totalNum");
		return(ret);
	}

	i = 0;
	while (!feof(fp))
	{
		memset(vkInfo,0,sizeof(vkInfo));
		if (fgets(vkInfo,sizeof(vkInfo),fp) != NULL)
		{
			if (vkInfo[0] != '[')
				continue;
			i++;
			memset(tmpNum,0,sizeof(tmpNum));						
			switch (i)
			{
				case	1:	// 索引
					if ((ptr = strchr(vkInfo,']')) != NULL)
					{
						memcpy(tmpNum,vkInfo+1,ptr-vkInfo-1);
						for (j = 0; j < vkIndexNum; j++)
						{
							if (strcmp(vkIndexGrp[j],tmpNum) == 0)
							{
								break;	
							}
						}
						if (j == vkIndexNum)
							i = 0;
					}
					else
						i = 0;
					break;
				case	2:	// 算法标识
					if ((ptr = strchr(vkInfo,']')) != NULL)
					{
						/*
						memcpy(tmpNum,vkInfo+1,ptr-vkInfo-1);
						if (atoi(algorithmID[j]) == 0)
						{
							if (strcasecmp("RSA",tmpNum) != 0)
								i = 0;
						}
						else if (atoi(algorithmID[j]) == 1)
							if (strcasecmp("SM2",tmpNum) != 0)
								i = 0;
						*/
						memcpy(tmpNum,vkInfo+1,ptr-vkInfo-1);
						if (strcasecmp("RSA",tmpNum) == 0)
						{
							sprintf(algorithmID[j],"0");
						}
						else if (strcasecmp("SM2",tmpNum) == 0)
						{
							sprintf(algorithmID[j],"1");
						}
						else
						{
							i = 0;
						}

					}
					else
						i = 0;
					break;
				case	3:	// 密钥长度
					if ((ptr = strchr(vkInfo,']')) != NULL)
					{
						memcpy(tmpNum,vkInfo+1,ptr-vkInfo-1);
						lenOfVK = atoi(tmpNum);
					}
					else
						i = 0;
					break;
				case	4:	// 密钥值
					if ((ptr = strchr(vkInfo,']')) != NULL)
					{
						memcpy(vkValue,vkInfo+1,lenOfVK);
						vkValue[lenOfVK] = 0;
					}
					else
						i = 0;
					break;

				case	5:	// 校验值
					if ((ptr = strchr(vkInfo,']')) != NULL)
					{
						len = ptr-vkInfo-1;
						memcpy(oriSign,vkInfo+1,len);
						oriSign[len] = 0;
						// modify by lisq 20141216 增加国密索引21-42
						//ret = UnionImportAndCheckVK(phsmGroupRec,atoi(algorithmID[j]),hsmGroupID,vkIndexGrp[j],lenOfVK,vkValue,oriSign,ipAddr);
						memset(tmpNum, 0, sizeof(tmpNum));
						vkIdx = atoi(vkIndexGrp[j]);
						if (vkIdx > 20)
							vkIdx = (vkIdx - 21);
						sprintf(tmpNum, "%02d", vkIdx);
						ret = UnionImportAndCheckVK(phsmGroupRec,isRecoverByGroup,atoi(algorithmID[j]),hsmGroupID,tmpNum,lenOfVK,vkValue,oriSign,ipAddr);
						// modify by lisq 20141216 end 增加国密索引21-42
						if (ret < 0)
						{
							isFailed = 1;
							failNum++;
						}
					
						totalNum++;
						if ((ret = UnionLocateResponseNewXMLPackage("body/detail",totalNum)) < 0)	
						{
							UnionUserErrLog("in UnionDealServiceCode0H10:: UnionSetResponseXMLPackageValue[%s][%d]\n","body/detail",totalNum);
							fclose(fp);
							return(ret);
						}

						// 加密机IP地址
						if ((ret = UnionSetResponseXMLPackageValue("ipAddr",ipAddr)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCode0H10:: UnionSetResponseXMLPackageValue[%s][%d]\n","ipAddr",totalNum);
							fclose(fp);
							return(ret);
						}

						// 私钥索引
						if ((ret = UnionSetResponseXMLPackageValue("vkIndex",vkIndexGrp[j])) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCode0H10:: UnionSetResponseXMLPackageValue[%s][%d]\n","vkIndex",totalNum);
							fclose(fp);
							return(ret);
						}

						// 私钥索引
						len = sprintf(tmpNum,"%s",isFailed == 0 ? "成功":"失败");
						tmpNum[len] = 0;
						if ((ret = UnionSetResponseXMLPackageValue("status",tmpNum)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCode0H10:: UnionSetResponseXMLPackageValue[%s][%d]\n","status",totalNum);
							fclose(fp);
							return(ret);
						}
						UnionLocateResponseXMLPackage("",0);
						if (isFailed == 1 && failNum > 1)
						{
							for (i = totalNum; i > failNum; i--)
							{
								if ((ret = UnionExchangeIDResponseXMLPackage("body/detail",i,i-1)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCode0H10:: UnionExchangeIDResponseXMLPackage[%s][%d][%d]!\n","body/detail",i,i-1);
								}
							}
						}
						isFailed = 0;

					}
					i = 0;
					break;
			}
		}
	}

	fclose(fp);

	return (0);
}

int UnionImportAndCheckVK(PUnionHsmGroupRec phsmGroupRec,int isRecoverByGroup,int algorithmID,char *hsmGroupID, char *vkIndex,int lenOfVK,char *vkValue,char *oriSign,char *ipAddr)
{
	int		len = 0;
	int		ret = 0;
	char		sql[256+1];
	char		tmpNum[10+1];
	int		totalNum = 0;
	int		i = 0;
	char		bcdVK[2560+1];
	char		sign[16+1];
	char		userID[16+1];
	char		hsmIP[16+1];

	// modify by chenqy 20151021
	if (isRecoverByGroup)
	{
		// 查询密码机IP
		len = sprintf(sql,"select * from hsm where hsmGroupID = '%s' and enabled = 1 and status = 1 ",hsmGroupID);
		sql[len] = 0;
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0H10:: UnionSelectRealDBRecord sql[%s]!\n",sql);
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0H10:: hsm not found, sql[%s]!\n",sql);
			UnionSetResponseRemark("没有找到加密机");
			return(errCodeDatabaseMDL_RecordNotFound);
		}
		memset(tmpNum,0,sizeof(tmpNum));
		if ((ret = UnionReadXMLPackageValue("totalNum",tmpNum,sizeof(tmpNum))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0H10:: UnionReadXMLPackageValue[%s]!\n","totalNum");
			return(ret);
		}
		totalNum = atoi(tmpNum);
	}
	else
	{
		totalNum = 1;	
	}
	// modify end

	
	for (i = 1; i <= totalNum; i++)
	{
		// modify by chenqy 20151021
		if (isRecoverByGroup)
		{
			if ((ret = UnionLocateXMLPackage("detail",i)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0H10:: UnionLocateXMLPackage[%s][%d]!\n","detail",i);
				return(ret);
			}
			memset(hsmIP,0,sizeof(hsmIP));
			if ((ret = UnionReadXMLPackageValue("ipAddr",hsmIP,sizeof(hsmIP))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0H10:: UnionLocateXMLPackage[%s][%d]!\n","ipAddr",i);
				return(ret);
			}

			strcpy(ipAddr,hsmIP);
		}
		// modify end
	
		aschex_to_bcdhex(vkValue,lenOfVK,bcdVK);	
		switch (algorithmID)
		{
			case	0:	// RSA算法
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:

						UnionSetUseSpecHsmIPAddrForOneCmd(ipAddr);

						if ((ret = UnionHsmCmd35(vkIndex,bcdVK,lenOfVK/2)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCode0H10:: UnionHsmCmd35 error!\n");
							return(ret);
						}

						// 签名
						UnionSetUseSpecHsmIPAddrForOneCmd(ipAddr);
						if ((ret = UnionHsmCmd37('1',vkIndex,0,NULL,16,"0000000000000000",bcdVK,sizeof(bcdVK))) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCode0H10:: UnionHsmCmd37!\n");
							return(ret);
						}

						bcdhex_to_aschex(bcdVK,8,sign);
						sign[16] = 0;
						if (memcmp(oriSign,sign,16) != 0)
						{
							UnionUserErrLog("in UnionDealServiceCode0H10:: oriSign[%s] != curSign[%s]\n",oriSign,sign);
							UnionSetResponseRemark("导入私钥失败");
							return(errCodeParameter);
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCode0H10:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}
				break;
			case	1:
				switch(phsmGroupRec->hsmCmdVersionID)
				{
					case	conHsmCmdVerRacalStandardHsmCmd:
					case	conHsmCmdVerSJL06StandardHsmCmd:

						UnionSetUseSpecHsmIPAddrForOneCmd(ipAddr);
						if ((ret = UnionHsmCmdK2(atoi(vkIndex),lenOfVK/2,(unsigned char*)bcdVK)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCode0H10:: UnionHsmCmdK2 error!\n");
							return(ret);
						}

						// 验签签名
						UnionSetUseSpecHsmIPAddrForOneCmd(ipAddr);
						memset(userID,0,sizeof(userID));	
						aschex_to_bcdhex("12345678",8,userID);
						if ((ret = UnionHsmCmdK4(atoi(vkIndex),"02",4,userID,0,NULL,strlen(oriSign),oriSign,16,"0000000000000000")) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCode0H10:: UnionHsmCmdK4!\n");
							return(ret);
						}

						break;
					default:
						UnionUserErrLog("in UnionDealServiceCode0H10:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
						UnionSetResponseRemark("非法的加密机指令类型");
						return(errCodeParameter);
				}
				break;
			default:
				UnionUserErrLog("in UnionDealServiceCode0H10:: algorithmID[%d]\n",algorithmID);
				UnionSetResponseRemark("非法的密钥类型");
				return(errCodeParameter);
		}
	}
	return 0;

}


