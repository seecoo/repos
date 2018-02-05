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
服务代码:	0H09
服务名:		备份私钥
功能描述:	备份私钥
***************************************/

int UnionDealServiceCode0H09(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret=0;
	char				filePath[512];
	char				sign[4096];
	char				sql[512];
	char				*ptr = NULL;
	char				bcdVK[1280];
	char				ascVK[2560];
	FILE				*fp;
	char				fileFullName[512];
	int				lenOfVK = 0;
	int				lenOfData = 0;
	int				i = 0;
	char				tmpNum[32];
	char				ipAddr[64];
	int				len = 0;
	int				keyAlgorithmID = 0;
	char				algorithmID[32];
	char				vkIndexList[128];
	//char				vkIndexGrp[21][128];
	char				vkIndexGrp[42][128];
	char				hsmGroupID[32];
	int				vkIndexNum = 0;
	int				j = 0;
	char				userID[32];
	char				fieldListChnName[128];
	char				fieldList[128];
	char				isFailed = 0;
	int				failNum = 0;
	char				pk[1024];
	char				fileName[128];
	int				vkIdx = 0;

	// 读取文件路径
	memset(filePath,0,sizeof(filePath));
	if ((ptr = UnionReadStringTypeRECVar("defaultDirOfFileReceiver")) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode0H09:: UnionReadStringTypeRECVar[%s]!\n","defaultDirOfFileReceiver");
		return(errCodeRECMDL_VarNotExists);
	}

	UnionReadDirFromStr(ptr,-1,filePath);

	// 密码机组
	memset(hsmGroupID,0,sizeof(hsmGroupID));
	if ((ret = UnionReadRequestXMLPackageValue("body/hsmGroupID",hsmGroupID,sizeof(hsmGroupID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H09:: UnionReadRequestXMLPackageValue[%s]!\n","body/hsmGroupID");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(hsmGroupID);
	if (strlen(hsmGroupID) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H09:: hsmGroupID can not be null!\n");
		UnionSetResponseRemark("密码机组不能为空");
		return(errCodeParameter);
	}
		
	// 读取密钥类型
	memset(algorithmID,0,sizeof(algorithmID));
	if ((ret = UnionReadRequestXMLPackageValue("body/algorithmID",algorithmID,sizeof(algorithmID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H09:: UnionReadRequestXMLPackageValue [%s]!\n","body/algorithmID");
		return(ret);
	}
	
	// 索引号
	if ((ret = UnionReadRequestXMLPackageValue("body/vkIndexList",vkIndexList,sizeof(vkIndexList))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H09:: UnionReadRequestXMLPackageValue[%s]!\n","body/vkIndexList");
		return(ret);
	}
	vkIndexList[ret] = 0;
	UnionFilterHeadAndTailBlank(vkIndexList);
	if (strlen(vkIndexList) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H09:: vkIndex can not be null!\n");
		UnionSetResponseRemark("索引不能为空");
		return(errCodeParameter);
	}

	// 拼分域定义串
	memset(vkIndexGrp,0,sizeof(vkIndexGrp));
	// modify by lisq 20141216 增加国密索引21-42
	//if ((vkIndexNum = UnionSeprateVarStrIntoVarGrp(vkIndexList,strlen(vkIndexList),',',vkIndexGrp,21)) < 0)
	if ((vkIndexNum = UnionSeprateVarStrIntoVarGrp(vkIndexList,strlen(vkIndexList),',',vkIndexGrp,42)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H09:: UnionSeprateVarStrIntoVarGrp[%s]!\n",vkIndexList);
		return(vkIndexNum);
	}

	// 打开文件
	len = 0;
	for (i = 0; i < vkIndexNum; i++)
		len += sprintf(vkIndexList + len,"%s_",vkIndexGrp[i]);
	
	vkIndexList[len-1] = 0;
	len = sprintf(fileFullName,"%s/%s_%s_%s.vk",filePath,algorithmID,hsmGroupID,vkIndexList);
	
	fileFullName[len] = 0;

	len = sprintf(fileName,"%s_%s_%s.vk",algorithmID,hsmGroupID,vkIndexList);
	fileName[len] = 0;

	if ((fp = fopen(fileFullName,"w+")) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode0H09:: fopen[%s]!\n",fileFullName);
		return(errCodeUseOSErrCode);
	}

	fprintf(fp,"# 密码机组: %s\n",hsmGroupID);

	// 查询密码机IP
	len = sprintf(sql,"select * from hsm where hsmGroupID = '%s' and status = 1 and enabled = 1",hsmGroupID);
	sql[len] = 0;
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H09:: UnionSelectRealDBRecord sql[%s]!\n",sql);
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H09:: hsm not found, sql[%s]!\n",sql);
		UnionSetResponseRemark("没有找到加密机");
		return(errCodeDatabaseMDL_RecordNotFound);
	}

	memset(tmpNum,0,sizeof(tmpNum));
	if ((ret = UnionReadXMLPackageValue("totalNum",tmpNum,sizeof(tmpNum))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H09:: UnionReadXMLPackageValue[%s]!\n","totalNum");
		return(ret);
	}

	// 设置显示报文体
	if ((ret = UnionSetResponseXMLPackageValue("head/displayBody","1")) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H09:: UnionSetResponseXMLPackageValue[%s]\n","body/displayBody");
		return(ret);
	}

	// 字段清单
	len = sprintf(fieldList,"ipAddr,vkIndex,status");
	fieldList[len] = 0;
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldList",fieldList)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H09:: UnionSetResponseXMLPackageValue[%s]\n","body/fieldList");
		return(ret);
	}

	// 字段清单中文名
	len = sprintf(fieldListChnName,"加密机IP,索引,备份状况");
	fieldListChnName[len] = 0;
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldListChnName",fieldListChnName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H09:: UnionSetResponseXMLPackageValue[%s]\n","body/fieldListChnName");
		return(ret);
	}

	// 设置总数
	len = sprintf(tmpNum,"%d",vkIndexNum);
	tmpNum[len] = 0;
	if ((ret = UnionSetResponseXMLPackageValue("body/totalNum",tmpNum)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H09:: UnionSetResponseXMLPackageValue[%s]\n","body/totalNum");
		return(ret);
	}

	for (i = 1; i <= 1; i++)
	{
		if ((ret = UnionLocateXMLPackage("detail",i)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0H09:: UnionLocateXMLPackage[%s][%d]!\n","detail",i);
			isFailed = 1;
			goto jmp;
		}

		memset(ipAddr,0,sizeof(ipAddr));	
		if ((ret = UnionReadXMLPackageValue("ipAddr",ipAddr,sizeof(ipAddr))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0H09:: UnionLocateXMLPackage[%s][%d]!\n","ipAddr",i);
			isFailed = 1;
			goto jmp;
		}

		for (j = 0; j < vkIndexNum; j++)
		{
			if (atoi(algorithmID) == 0)
			{
				keyAlgorithmID = 0;
				strcpy(algorithmID,"RSA");
			}
			else if (atoi(algorithmID) == 1)
			{
				keyAlgorithmID = 1;
				strcpy(algorithmID,"SM2");
			}

			// modify by lisq 20141216 增加国密索引21-42
			vkIdx = atoi(vkIndexGrp[j]);
			if (vkIdx > 20)
				vkIdx = (vkIdx - 21);

			memset(tmpNum, 0, sizeof(tmpNum));
			sprintf(tmpNum, "%02d", vkIdx);
			// modify by lisq 20141216 end 增加国密索引21-42

			switch (keyAlgorithmID)
			{
				case	0:	// RSA算法
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case	conHsmCmdVerRacalStandardHsmCmd:
						case	conHsmCmdVerSJL06StandardHsmCmd:
							// modify by lisq 20141216 增加国密索引21-42
							//if ((lenOfVK = UnionHsmCmd36(vkIndexGrp[j],bcdVK,sizeof(bcdVK))) < 0)
							// add by liwj 20141226 设置访问指定组的加密机
							UnionSetUseSpecHsmGroupForOneCmd(hsmGroupID);
							// add end
							if ((lenOfVK = UnionHsmCmd36(tmpNum,bcdVK,sizeof(bcdVK))) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCode0H09:: UnionHsmCmd36 error vkIndex[%s] ipAddr[%s]!\n",vkIndexGrp[j],ipAddr);
								isFailed = 1;
								goto jmp;
							}

							lenOfVK = bcdhex_to_aschex(bcdVK,lenOfVK,ascVK);
							ascVK[lenOfVK] = 0;

							// 签名
							// modify by lisq 20141216 增加国密索引21-42
							//if ((lenOfData = UnionHsmCmd37('1',vkIndexGrp[j],0,NULL,16,"0000000000000000",sign,sizeof(sign))) < 0)
							// add by liwj 20141226 设置访问指定组的加密机
							UnionSetUseSpecHsmGroupForOneCmd(hsmGroupID);
							// add end
							if ((lenOfData = UnionHsmCmd37('1',tmpNum,0,NULL,16,"0000000000000000",sign,sizeof(sign))) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCode0H09:: UnionHsmCmd37 vkIndex[%s] ipAddr[%s]!\n",vkIndexGrp[j],ipAddr);
								isFailed = 1;
								goto jmp;
							}

							lenOfData = bcdhex_to_aschex(sign,8,bcdVK);
							memcpy(sign,bcdVK,16);
							sign[lenOfData] = 0;
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCode0H09:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
							isFailed = 1;
							goto jmp;
					}
					break;
				case	1:
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case	conHsmCmdVerRacalStandardHsmCmd:
						case	conHsmCmdVerSJL06StandardHsmCmd:
							// modify by lisq 20141216 增加国密索引21-42
							//if ((ret = UnionHsmCmdK8(atoi(vkIndexGrp[j]),0,&lenOfVK,bcdVK,pk)) < 0)
							// add by liwj 20141226 设置访问指定组的加密机
							UnionSetUseSpecHsmGroupForOneCmd(hsmGroupID);
							// add end
							if ((ret = UnionHsmCmdK8(vkIdx,"0",&lenOfVK,(unsigned char*)bcdVK,(unsigned char*)pk)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCode0H09:: UnionHsmCmdK8 error!\n");
								isFailed = 1;
								goto jmp;
							}

							lenOfVK = bcdhex_to_aschex(bcdVK,lenOfVK,ascVK);
							ascVK[lenOfVK] = 0;

							// 签名
							memset(userID,0,sizeof(userID));	
							aschex_to_bcdhex("12345678",8,userID);
							// odify by lisq 20141216 增加国密索引21-42
							//if ((lenOfData = UnionHsmCmdK3("02",4,userID,16,"0000000000000000",atoi(vkIndexGrp[j]),0,NULL,sign,sizeof(sign))) < 0)
							// add by liwj 20141226 设置访问指定组的加密机
							UnionSetUseSpecHsmGroupForOneCmd(hsmGroupID);
							// add end
							if ((lenOfData = UnionHsmCmdK3("02",4,userID,16,"0000000000000000",vkIdx,0,NULL,sign,sizeof(sign))) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCode0H09:: UnionHsmCmdK3!\n");
								isFailed = 1;
								goto jmp;
							}
							sign[lenOfData] = 0;
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCode0H09:: hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							isFailed = 1;
							goto jmp;
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCode0H09:: algorithmID[%d]\n",keyAlgorithmID);
					isFailed = 1;
					goto jmp;
			}
			fprintf(fp,"\n[%s]\n",vkIndexGrp[j]);
			fprintf(fp,"[%s]\n",algorithmID);
			fprintf(fp,"[%04d]\n",(int)strlen(ascVK));	
			fprintf(fp,"[%s]\n",ascVK);	
			fprintf(fp,"[%s]\n",sign);	

jmp:
			if ((ret = UnionLocateResponseNewXMLPackage("body/detail",j+1)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0H09:: UnionSetResponseXMLPackageValue[%s][%d]\n","body/detail",j+1);
				return(ret);
			}

			// 加密机IP地址
			if ((ret = UnionSetResponseXMLPackageValue("ipAddr",ipAddr)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0H09:: UnionSetResponseXMLPackageValue[%s][%d]\n","ipAddr",j+1);
				return(ret);
			}

			// 私钥索引
			if ((ret = UnionSetResponseXMLPackageValue("vkIndex",vkIndexGrp[j])) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0H09:: UnionSetResponseXMLPackageValue[%s][%d]\n","vkIndex",j+1);
				return(ret);
			}

			// 私钥索引
			if (isFailed == 1)
				failNum ++;
			len = sprintf(tmpNum,"%s",isFailed == 0 ? "成功":"失败");
			tmpNum[len] = 0;
			if ((ret = UnionSetResponseXMLPackageValue("status",tmpNum)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0H09:: UnionSetResponseXMLPackageValue[%s][%d]\n","status",j+1);
				return(ret);
			}
			UnionLocateResponseXMLPackage("",0);
			if (isFailed == 1 && failNum > 1)
			{
				for (i = j; i > failNum; i--)
				{
					if ((ret = UnionExchangeIDResponseXMLPackage("body/detail",i,i-1)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCode0H09:: UnionExchangeIDResponseXMLPackage[%s][%d][%d]!\n","body/detail",i,i-1);
					}
				}
			}
			isFailed = 0;
		}
	}
	fclose(fp);

	if ((ret = UnionSetResponseXMLPackageValue("file/filename",fileName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H09:: UnionSetResponseXMLPackageValue[%s]\n","file/filename");
		return(ret);
	}
	
	return (0);
}


