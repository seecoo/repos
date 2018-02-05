//	Author:		周修伟
//	Copyright:	Union Tech. Guangzhou
//	Date:		2015-07-01

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "essc5UIService.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "symmetricKeyDB.h"
#include "unionHsmCmd.h"
#include "remoteKeyPlatform.h"
#include "unionHsmCmdVersion.h"
#define		maxGrpNum	1000
#define		keyTypeNum	3

/***************************************
服务代码:	8E0C
服务名:		批量插入并初始化密钥
功能描述:	批量插入并初始化密钥
***************************************/
int UnionDealServiceCode8E0C(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	int				i = 0;
	int				j = 0;
	
	char				keyNodeGrp[maxGrpNum][128];
	char				keyType[3][16]={"zak","zpk","zmk"};
	int				method = 0;
	char				tmpBuf[128];
	char				keyNodeList[4096];
	int				keyNodeNum = 0;
	char				appID[32];
	
	char				sql[512];
	int				len = 0;
	char    			isCheckAlonePrint[16];
        char    			keyValuePrintFormat[2048];
	char				tmpKeyValuePrintFormat[2048];
	int 				numOfComponent = 0;
	char				hsmIP[64];
	
	FILE				*fp;
	char				fileStoreDir[512];
	char				path[512];
	char				fullFileName[512];
	char				fileName[] = "initZMKkeyOfPOS";
	char				delimiter;
	char				*ptr = NULL;
	char				keyValue[64];
	char				checkValue[32];
	char				tmpCheckValue[32];
	
	int				keyTotalNum = 0;
	int				num = 0;
	int				updateKeyNode = 1;
	int				status = 0;
	int				algorithmID = 0;
	char				keyName[keyTypeNum][128];
	int				isDiffAlg = 0;
	int				keyExisted[3] = {0};
	int                             isRemoteApplyKey = 0;
        int                             isRemoteDistributeKey = 0;

	TUnionSymmetricKeyDB		symmetricKeyDB;
	//PUnionSymmetricKeyValue		psymmetricKeyValue = NULL;
	TUnionRemoteKeyPlatform		tkeyApplyPlatform;
	TUnionRemoteKeyPlatform		tkeyDistributePlatform;
	
	memset(&symmetricKeyDB, 0, sizeof(symmetricKeyDB));

	//密钥组
	//modify by zhouxw 20150907
	//if((ret = UnionReadRequestXMLPackageValue("body/keyGroup", symmetricKeyDB.keyGroup, sizeof(symmetricKeyDB.keyName))) <= 0)
	//	strcpy(symmetricKeyDB.keyGroup, "default");
	if((ret = UnionReadRequestXMLPackageValue("body/keyGroup", symmetricKeyDB.keyGroup, sizeof(symmetricKeyDB.keyName))) < 0)
	{
		//strcpy(symmetricKeyDB.keyGroup, "default");
		UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyGroup");
		return(ret);
	}
	else if(ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0C:: keyGroup should not be null!\n");
		UnionSetResponseRemark("密钥组不能为空");
		return(errCodeParameter);
	}
	UnionSetHsmGroupIDForHsmSvr(symmetricKeyDB.keyGroup);
	//modify end
	//add by zhouxw 20151013
        if ((ret = UnionGetHsmGroupRecByHsmGroupID(symmetricKeyDB.keyGroup,phsmGroupRec)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionGetHsmGroupRecByHsmGroupID 密码机组[%s]不存在!\n",symmetricKeyDB.keyGroup);
                return(ret);
        }
        //add end
	
	//应用
	if((ret = UnionReadRequestXMLPackageValue("body/appID", appID, sizeof(appID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionReadRequestXMLPackageValue[%s]\n", "body/appID");
		return(ret);
	}
	appID[ret] = 0;
	UnionFilterHeadAndTailBlank(appID);
        if (strlen(appID) == 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E0C:: appID can not be null!\n");
                UnionSetResponseRemark("appID不能为空");
                return(errCodeParameter);
        }

	//密钥节点
	if((ret = UnionReadRequestXMLPackageValue("body/keyNodeList1", keyNodeList,sizeof(keyNodeList))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionReadRequestXMLPackageValue[%s]\n", "body/keyNodeList1");
		return(ret);
	}
	keyNodeList[ret] = 0;
	UnionFilterHeadAndTailBlank(keyNodeList);
	if (strlen(keyNodeList) == 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E0C:: keyNodeList can not be null!\n");
                UnionSetResponseRemark("密钥节点清单不能为空");
                return(errCodeParameter);
        }
	
	//拼分域定义串
	if((keyNodeNum = UnionSeprateVarStrIntoVarGrp(keyNodeList, strlen(keyNodeList), ',', keyNodeGrp, maxGrpNum)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionSeprateVarStrIntoVarGrp[%s]\n", keyNodeList);
		return(keyNodeNum);
	}
	
	if (keyNodeNum > 1000)
        {
                UnionUserErrLog("in UnionDealServiceCode8E0C:: keyNodeNum[%d] > 1000!\n",keyNodeNum);
                UnionSetResponseRemark("密钥节点数不能超过1000个");
                return(errCodeTooManyNetNodes);
        }
	
	// 算法标识 
	if ((ret = UnionReadRequestXMLPackageValue("body/algorithmID",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionReadRequestXMLPackageValue[%s]!\n","body/algorithmID");
		return(ret);	
	}
	tmpBuf[ret] = 0;
	symmetricKeyDB.algorithmID = atoi(tmpBuf);

	// 密钥长度
	if ((ret = UnionReadRequestXMLPackageValue("body/keyLen",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyLen");
		return(ret);	
	}
	tmpBuf[ret] = 0;
	symmetricKeyDB.keyLen = atoi(tmpBuf);

	// 允许使用旧密钥
	if ((ret = UnionReadRequestXMLPackageValue("body/oldVersionKeyIsUsed",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionReadRequestXMLPackageValue[%s]!\n","body/oldVersionKeyIsUsed");
		return(ret);	
	}
	tmpBuf[ret] = 0;
	symmetricKeyDB.oldVersionKeyIsUsed = atoi(tmpBuf);

	if ((symmetricKeyDB.oldVersionKeyIsUsed != 0) && (symmetricKeyDB.oldVersionKeyIsUsed != 1))
	{
		UnionUserErrLog("in UnionDealServiceCode8E0C:: oldVersionKeyIsUsed[%s] error!\n",tmpBuf);
		return(errCodeParameter);
	}

	// 读取有效天数
	if ((ret = UnionReadRequestXMLPackageValue("body/effectiveDays",tmpBuf,sizeof(tmpBuf))) <= 0)
		symmetricKeyDB.effectiveDays = 0;
	else
	{
		tmpBuf[ret] = 0;
		symmetricKeyDB.effectiveDays = atoi(tmpBuf);
	}
	
	// 允许导入标识
	// 可选，默认值为0
	// 0：不允许
	// 1：允许
	if ((ret = UnionReadRequestXMLPackageValue("body/inputFlag",tmpBuf,sizeof(tmpBuf))) < 0)
		symmetricKeyDB.inputFlag = 0;
	else
	{
		tmpBuf[ret] = 0;
		symmetricKeyDB.inputFlag = atoi(tmpBuf);
		if ((symmetricKeyDB.inputFlag != 0) && (symmetricKeyDB.inputFlag != 1) && (symmetricKeyDB.inputFlag != 2))
		{
			UnionUserErrLog("in UnionDealServiceCode8E0C:: inputFlag[%s] error!\n",tmpBuf);
			return(errCodeParameter);
		}
	}

	// 允许导出标识
	// 可选，默认值为0
	// 0：不允许
	// 1：允许
	if ((ret = UnionReadRequestXMLPackageValue("body/outputFlag",tmpBuf,sizeof(tmpBuf))) <= 0)
		symmetricKeyDB.outputFlag = 0;
	else
	{
		tmpBuf[ret] = 0;
		symmetricKeyDB.outputFlag = atoi(tmpBuf);
		if ((symmetricKeyDB.outputFlag != 0) && (symmetricKeyDB.outputFlag != 1))
		{
			UnionUserErrLog("in UnionDealServiceCode8E0C:: outputFlag[%s] error!\n",tmpBuf);
			return(errCodeParameter);
		}
	}

	// 启用标识
	// 不启用
	symmetricKeyDB.status = 0;
	
	// 密钥申请平台
	if ((ret = UnionReadRequestXMLPackageValue("body/keyApplyPlatform",symmetricKeyDB.keyApplyPlatform,sizeof(symmetricKeyDB.keyApplyPlatform))) > 0)
	{
		// 查找remoteKeyPlatform
		memset(&tkeyApplyPlatform,0,sizeof(tkeyApplyPlatform));
		if ((ret = UnionReadRemoteKeyPlatformRec(symmetricKeyDB.keyApplyPlatform,conRemoteKeyPlatformKeyDirectionOfApply,&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionReadRemoteKeyPlatformRec keyApplyPlatform[%s]!\n",symmetricKeyDB.keyApplyPlatform);
			return(ret);
		}	
		isRemoteApplyKey = 1;
	}

	// 密钥分发平台
	if ((ret = UnionReadRequestXMLPackageValue("body/keyDistributePlatform",symmetricKeyDB.keyDistributePlatform,sizeof(symmetricKeyDB.keyDistributePlatform))) > 0)
	{
		// 查找remoteKeyPlatform
		memset(&tkeyDistributePlatform,0,sizeof(tkeyDistributePlatform));
		if ((ret = UnionReadRemoteKeyPlatformRec(symmetricKeyDB.keyDistributePlatform,conRemoteKeyPlatformKeyDirectionOfDistribute,&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionReadRemoteKeyPlatformRec keyDistributePlatform[%s]!\n",symmetricKeyDB.keyDistributePlatform);
			return(ret);
		}
		isRemoteDistributeKey = 1;
	}

	// 检测分发和申请是否同一平台
	if (isRemoteApplyKey && isRemoteDistributeKey)
	{
		if (strcmp(tkeyApplyPlatform.ipAddr,tkeyDistributePlatform.ipAddr) == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E0C:: tkeyApplyPlatform.ipAddr[%s] == tkeyDistributePlatform.ipAddr[%s] is error!\n",tkeyApplyPlatform.ipAddr,tkeyDistributePlatform.ipAddr);	
			UnionSetResponseRemark("申请和分发平台不能相同");
			return(errCodeParameter);
		}
	}
	
	// 生效日期
	if ((ret = UnionReadRequestXMLPackageValue("body/activeDate",symmetricKeyDB.activeDate,sizeof(symmetricKeyDB.activeDate))) <= 0)
		UnionGetFullSystemDate(symmetricKeyDB.activeDate);

	// 备注
	if ((ret = UnionReadRequestXMLPackageValue("body/remark",symmetricKeyDB.remark,sizeof(symmetricKeyDB.remark))) <= 0)
		strcpy(symmetricKeyDB.remark,"");

	// 创建者类型
	symmetricKeyDB.creatorType = conSymmetricCreatorTypeOfUser;

	// 创建者
	if ((ret = UnionReadRequestXMLPackageValue("head/userID",symmetricKeyDB.creator,sizeof(symmetricKeyDB.creator))) <= 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionReadRequestXMLPackageValue[%s]!\n","head/userID");
		return(ret);	
	}
	
	// 显示报文体
	if ((ret = UnionSetResponseXMLPackageValue("head/displayBody","1")) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionSetResponseXMLPackageValue[%s]!\n","head/displayBody");
		return(ret);
	}

	// 设置字段清单
	strcpy(tmpBuf,"keyNode,keyName,status");
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldList",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionSetResponseXMLPackageValue[%s]!\n","body/fieldList");
		return(ret);
	}

	//　设置字段清单中文名
	strcpy(tmpBuf,"密钥节点,密钥名称,状态");
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldListChnName",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionSetResponseXMLPackageValue[%s]!\n","body/fieldListChnName");
		return(ret);
	}

	// 密钥总数 
	keyTotalNum = keyNodeNum * keyTypeNum;
	
	//读取密钥初始方法
	if((ret = UnionReadRequestXMLPackageValue("body/method", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionReadRequestXMLPackageValue[%s]\n", "body/method");
		return(ret);
	}
	tmpBuf[ret] = 0;
	method = atoi(tmpBuf);
	if(method != 0 && method != 1)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0C:: [body/method] wrong value\n");
		return(errCodeParameter);
	}
	//0：通过密钥文件初始化密钥 1：随机生成并打印密钥
	if(0 == method)
	{
		//算法标识
        	if((ret = UnionReadRequestXMLPackageValue("body/algorithmID", tmpBuf, sizeof(tmpBuf))) <= 0)
                	algorithmID = 0;
        	else
       		{
        	        tmpBuf[ret] = 0;        
        	        algorithmID = atoi(tmpBuf);
        	}
        	if(algorithmID != 0 && algorithmID != 1)
        	{
        	        UnionUserErrLog("in UnionDealServiceCode8E0C:: wrong algorithmType[%d]\n", algorithmID);
        	        UnionSetResponseRemark("密钥类型错，必须为DES或SM4");
        	        return(errCodeParameter);
        	}
        	if(0 == algorithmID)            
        	        strcpy(fileName, "DESZMKkeyOfPOS");
        	else                            
                	strcpy(fileName, "SM4ZMKkeyOfPOS");

		len = snprintf(sql, sizeof(sql), "select * from fileType where fileTypeID='POSZ'");
		sql[len] = 0;
		if((ret = UnionSelectRealDBRecord(sql, 0, 0)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionSelectRealDBRecord[%s]\n", sql);
			return(ret);
		}
		if(ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E0C:: record not found\n");
			return(errCodeDatabaseMDL_RecordNotFound);
		}
		if((ret = UnionLocateXMLPackage("detail", 1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionLocateXMLPackage[detail]\n");
			return(ret);
		}
		if((ret = UnionReadXMLPackageValue("fileStoreDir", fileStoreDir, sizeof(fileStoreDir))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionReadXMLPackageValue[%s]\n", "fileStoreDir");
			return(ret);
		}
		fileStoreDir[ret] = 0;
		if((ret = UnionReadXMLPackageValue("delimiter", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionReadXMLPackageValue[%s]\n", "delimiter");
			return(ret);
		}
		tmpBuf[ret] = 0;
		delimiter = tmpBuf[0];
		
		UnionReadDirFromStr(fileStoreDir, -1, path);
		len = snprintf(fullFileName, sizeof(fullFileName), "%s/%s", path, fileName);
		fullFileName[len] =  0;
		if((fp = fopen(fullFileName, "r")) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCode8E0C:: open file[%s] failed\n", fullFileName);
			return(errCodeParameter);
		}
		if(fgets(tmpBuf, sizeof(tmpBuf), fp) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCode8E0C:: file[%s] is empty\n", fullFileName);
			return(errCodeParameter);
		}
		if((ptr = strchr(tmpBuf, '\r')) != NULL)
                	tmpBuf[ptr - tmpBuf] = 0;
        	if((ptr = strchr(tmpBuf, '\n')) != NULL)
                	tmpBuf[ptr - tmpBuf] = 0;
		if((ptr = strchr(tmpBuf, delimiter)) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCode8E0C:: file[%s] content[%s] error. example[keyValue%ccheckValue]\n", fullFileName, tmpBuf, delimiter);
			return(errCodeParameter);
		}
		*ptr = 0;
		strcpy(keyValue, tmpBuf);
		strcpy(checkValue, ptr+1);
		fclose(fp);
		if((strlen(keyValue) != 16 && strlen(keyValue) != 32 && strlen(keyValue) != 48) || strlen(checkValue) != 16)
        	{
                	UnionUserErrLog("in UnionDealServiceCode8E0C:: wrong length of key[%s] or checkValue[%s]\n", keyValue, checkValue);
        	        UnionSetResponseRemark("密钥文件内容错");
        	        return(errCodeParameter);
        	}
		if(strlen(keyValue) != symmetricKeyDB.keyLen / 4)
		{
			UnionUserErrLog("in UnionDealServiceCode8E0C:: different keyLen\n");
			UnionSetResponseRemark("界面选择的长度与密钥文件密钥长度不相符");
			return(errCodeParameter);
		}
		UnionSetIsUseNormalZmkType();
        	if((ret = UnionHsmCmdBU(algorithmID, conZMK, strlen(keyValue)/16-1, keyValue, tmpCheckValue)) < 0)
        	{
        	        UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionHsmCmdBU[%s]\n", keyValue);
        	        return(ret);
        	}
        	if(strcmp(checkValue, tmpCheckValue) != 0)
        	{
        	        UnionUserErrLog("in UnionDealServiceCode8E0C:: checkValue[%s] in file is diff with checkValue[%s] from BU command\n", checkValue, tmpCheckValue);
        	        UnionUserErrLog("密钥文件校验值校验失败");
        	        return(errCodeParameter);
        	}
	}
	else
	{
		//分量数量
		if((ret = UnionReadRequestXMLPackageValue("body/numOfComponent", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionReadRequestXMLPackageValue[%s]\n", "body/numOfComponent");
			return(ret);
		}
		tmpBuf[ret] = 0;
		numOfComponent = atoi(tmpBuf);
		//密码机IP
		if((ret = UnionReadRequestXMLPackageValue("body/ipAddr", hsmIP, sizeof(hsmIP))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionReadRequestXMLPackageValue[%s]\n", "body/hsmIP");
			return(ret);
		}
		hsmIP[ret] = 0;	
		//打印格式
		if((ret = UnionReadRequestXMLPackageValue("body/keyValuePrintFormat", keyValuePrintFormat, sizeof(keyValuePrintFormat))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionReadRequestXMLPackageValue[%s]\n", "body/keyValuePrintFormat");
			return(ret);
		}
		keyValuePrintFormat[ret] = 0;
		UnionFilterHeadAndTailBlank(keyValuePrintFormat);
        	if (strlen(keyValuePrintFormat) == 0)
        	{
        	        UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionReadRequestXMLPackageValue keyValuePrintFormat can not be null!\n");
        	        return(errCodeParameter);
        	}
		// 查询打印格式
        	len = snprintf(sql, sizeof(sql), "select * from keyPrintFormat where formatName = '%s'", keyValuePrintFormat);
        	sql[len] = 0;
        	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
        	{
        	        UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionSelectRealDBRecord sql[%s]!\n",sql);
        	        return(ret);
        	}
		else if(ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E0C:: record can't found[%s]!\n", sql);
			return(errCodeRecordNotExists);
		}
	
        	if ((ret = UnionLocateXMLPackage("detail",1)) < 0)
        	{
        		UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionLocateXMLPackage[%s]!\n","detail");
                	return(ret);
        	}	
        	// 获取是否单独打印校验值
       		if ((ret = UnionReadXMLPackageValue("isCheckAlonePrint",isCheckAlonePrint,sizeof(isCheckAlonePrint))) < 0)
        	{
        	        UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionReadXMLPackageValue[%s]!\n","isCheckAlonePrint");
       		        return(ret);
        	}
        	isCheckAlonePrint[ret] = 0;
        	UnionFilterHeadAndTailBlank(isCheckAlonePrint);
	
       		// 获取打印格式
        	if ((ret = UnionReadXMLPackageValue("format",keyValuePrintFormat,sizeof(keyValuePrintFormat))) < 0)
        	{
        	        UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionReadXMLPackageValue[%s]!\n","format");
			return(ret);
        	}
        	keyValuePrintFormat[ret] = 0;
	}
	memset(keyNodeList, 0, sizeof(keyNodeNum));
	for(i = 0; i < keyNodeNum; i++)
	{
		updateKeyNode = 1;
		for(j = 0; j < keyTypeNum; j++)
		{
			snprintf(keyName[j], sizeof(keyName[j]), "%s.%s.%s", appID, keyNodeGrp[i], keyType[j]);
			len = snprintf(sql, sizeof(sql), "select algorithmID from symmetricKeyDB where keyName='%s'", keyName[j]);
			sql[len] = 0;
			if((ret = UnionSelectRealDBRecord(sql, 0, 0)) > 0)
			{
				keyExisted[j] = 1;	
				if((ret = UnionLocateXMLPackage("detail", 1)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionLocateXMLPackage[detail]\n");
					return(ret);
				}
				if((ret = UnionReadXMLPackageValue("algorithmID", tmpBuf, sizeof(tmpBuf))) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionReadXMLPackageValue[algorithmID]\n");
					return(ret);
				}
				tmpBuf[ret] = 0;
				if(symmetricKeyDB.algorithmID != atoi(tmpBuf))
				{
					isDiffAlg = 1;
					keyExisted[j] = 0;
					break;
				}
			}
		}
		
		if(isDiffAlg)
		{
			//设置响应
			if((ret = UnionLocateResponseNewXMLPackage("body/detail", ++ num)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionLocateResponseNewXMLPackage[%s][%d]\n", "body/detail", num);
				return(ret);
			}
			if((ret = UnionSetResponseXMLPackageValue("keyNode", keyNodeGrp[i])) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionSetResponseXMLPackageValue[%s][%d]\n", "keyNode", num);
				return(ret);
			}
			if((ret = UnionSetResponseXMLPackageValue("keyName", "nokeycreated")) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionSetResponseXMLPackageValue[%s][%d]\n", "keyName", num);
				return(ret);
			}
			snprintf(tmpBuf, sizeof(tmpBuf), "%s算法的%s密钥已存在", symmetricKeyDB.algorithmID == 0?"SM4":"DES", keyType[j]);
			if((ret = UnionSetResponseXMLPackageValue("status", tmpBuf)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionSetResponseXMLPackageValue[%s] [%d]\n", "status", num);
				return(ret);
			}
			isDiffAlg = 0;
			updateKeyNode = 0;
			goto upKeyNode;
		}
		
		strcpy(tmpKeyValuePrintFormat, keyValuePrintFormat);
		for(j = 0; j < keyTypeNum; j++)
		{
			status = 0;
			symmetricKeyDB.keyType = UnionConvertSymmetricKeyKeyType(keyType[j]);
			strcpy(symmetricKeyDB.keyName, keyName[j]);
			if(!keyExisted[j])
			{
				symmetricKeyDB.status = 0;
				//创建密钥容器
				if((ret = UnionCreateSymmetricKeyDB(&symmetricKeyDB)) < 0)
                		{
                		        UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionCreateSymmetricKeyDB[%s] ret[%d]\n", symmetricKeyDB.keyName, ret);
					updateKeyNode = 0;
					status = 1;
					goto setResponse;
                		}
			}
			else
				status = 6;
			keyExisted[j] = 0;
			
			//zmk密钥要初始化
			if(symmetricKeyDB.keyType == conZMK)
			{
				if(method == 0)
				{
					strcpy(symmetricKeyDB.keyValue[0].lmkProtectMode, phsmGroupRec->lmkProtectMode);
					strcpy(symmetricKeyDB.keyValue[0].keyValue, keyValue);
                			strcpy(symmetricKeyDB.checkValue, checkValue);	
				}
				//打印密钥
				else
				{
        				if ((ret = UnionGenerateAndPrintSymmetricKey(phsmGroupRec,&symmetricKeyDB,tmpKeyValuePrintFormat,atoi(isCheckAlonePrint),numOfComponent,hsmIP,0,NULL)) < 0)
        				{
                				UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionGenerateAndPrintSymmetricKey!\n");
						updateKeyNode = 0;
						if(status == 6)
							status = 4;
						else
							status = 2;
						goto setResponse;
        				}
				}
				//更新密钥
        			if ((ret = UnionUpdateSymmetricKeyDBKeyValue(&symmetricKeyDB)) < 0)
        			{
                			UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionUpdateSymmetricKeyDBKeyValue keyName[%s]!\n",symmetricKeyDB.keyName);
					updateKeyNode = 0;
					if(status == 6)
                                        	status = 5;
					else
						status = 3;
					goto setResponse;
        			}
			}
setResponse:
			//设置响应
			if((ret = UnionLocateResponseNewXMLPackage("body/detail", ++ num)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionLocateResponseNewXMLPackage[%s][%d]\n", "body/detail", num);
				return(ret);
			}
			if((ret = UnionSetResponseXMLPackageValue("keyNode", keyNodeGrp[i])) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionSetResponseXMLPackageValue[%s][%d]\n", "keyNode", num);
				return(ret);
			}
			if((ret = UnionSetResponseXMLPackageValue("keyName", symmetricKeyDB.keyName)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionSetResponseXMLPackageValue[%s][%d]\n", "keyName", num);
				return(ret);
			}
			switch(status)
			{
				case 0:
					strcpy(tmpBuf, "成功");
					break;
				case 1:	
					strcpy(tmpBuf, "创建密钥失败");
					break;
				case 2:
					strcpy(tmpBuf, "打印密钥失败");
					break;
				case 3:
					strcpy(tmpBuf, "更新密钥值失败");
					break;
				case 4:
					strcpy(tmpBuf, "密钥已存在且打印失败");
					break;
				case 5:
					strcpy(tmpBuf, "密钥已存在且更新密钥值失败");
					break;
				case 6:
					strcpy(tmpBuf, "密钥已存在");
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCode8E0C:: wrong status[%d]\n", status);
					return(errCodeParameter);
			}
			if((ret = UnionSetResponseXMLPackageValue("status", tmpBuf)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionSetResponseXMLPackageValue[%s][%d]\n", "status", num);
				return(ret);
			}
		}
upKeyNode:
		if(updateKeyNode)
		{
			len = snprintf(sql, sizeof(sql), "update keyNode set status = 1 where appId='%s' and nodeID='%s'", appID, keyNodeGrp[i]);
			sql[len] = 0;
			if((ret = UnionExecRealDBSql(sql)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionExecRealDBSql[%s]\n", sql);
				return(ret);
			}
			if(ret != 1)
			{
				UnionUserErrLog("in UnionDealServiceCode8E0C:: affected no rows or too many rows, ret = [%d]\n", ret);
				return(errCodeParameter);
			}
		}
		else
		{
			strcat(keyNodeList, keyNodeGrp[i]);
			strcat(keyNodeList, ",");
		}
	}
	if((ret = UnionLocateResponseXMLPackage("body", 0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionLocateResponseXMLPackage[body]\n");
		return(ret);
	}
	snprintf(tmpBuf,sizeof(tmpBuf),"%d",num);
	if ((ret = UnionSetResponseXMLPackageValue("totalNum",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionSetResponseXMLPackageValue[%s]!\n","body/totalNum");
		return(ret);
	}
	//设置增加密钥失败的节点
	if(0 != strlen(keyNodeList))
	{
		if((ret = UnionSetResponseXMLPackageValue("failKeyNode", keyNodeList)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E0C:: UnionSetResponseXMLPackageValue[body/failKeyNode]\n");
			return(ret);
		}
	}

	return(0);
}
