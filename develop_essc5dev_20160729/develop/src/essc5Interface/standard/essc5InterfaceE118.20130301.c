//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "essc5Interface.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "essc5Interface.h"
#include "UnionStr.h"
#include "symmetricKeyDB.h"
#include "unionHsmCmd.h"
#include "remoteKeyPlatform.h"

/***************************************
服务代码:	E118
服务名:		打印密钥
功能描述:	打印密钥
***************************************/
int UnionDealServiceCodeE118(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	int				len = 0;
	char				tmpNum[32];
	char				hsmIP[64];
	char				keyPrintFormat[2048];
	char				formatName[64];
	int				numOfComponent = 0;
	char				sql[128];
	int				keyLen = 0;
	char				algorithmID[32];
	int				appendPrintNum = 0;
	int				mode = 0;
	int				maxParamNum = 20;
	char				appendPrintParam[maxParamNum][80];
	int				i = 0;
	char				tmpBuf[128];
	char				isCheckAlonePrint[32];
	char				keyType[32];
	char				keyName[160];
	

	TUnionSymmetricKeyDB		symmetricKeyDB;
	PUnionSymmetricKeyValue		psymmetricKeyValue = NULL;
	
	UnionInitSymmetricKeyDB(&symmetricKeyDB);
	// 密钥名称
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE118:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	keyName[ret] = 0;
	UnionFilterHeadAndTailBlank(keyName);
	snprintf(symmetricKeyDB.keyName,sizeof(symmetricKeyDB.keyName),"%s",keyName);

	// 打印模式
	// 1：密钥值存在，打印并存储
	// 2：密钥名称存在,密钥值不存在，打印并存储
	// 3：密钥名称不存在，打印不存储
	if ((ret = UnionReadRequestXMLPackageValue("body/mode",tmpNum,sizeof(tmpNum))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE118:: UnionReadRequestXMLPackageValue[%s]!\n","body/mode");
		return(ret);
	}
	else
		mode = atoi(tmpNum);
	
	switch (mode)
	{
		case 	1:
		case	2:
			// 读取对称密钥
			if ((ret =  UnionReadSymmetricKeyDBRec(keyName,0,&symmetricKeyDB)) < 0)
			{
				if (ret == errCodeKeyCacheMDL_WrongKeyName)
				{
					UnionUserErrLog("in UnionDealServiceCodeE118:: 对称密钥[%s]不存在!\n",keyName);
					UnionSetResponseRemark("对称密钥[%s]不存在",keyName);
					return(errCodeKeyCacheMDL_KeyNonExists);
				}
				UnionUserErrLog("in UnionDealServiceCodeE118:: UnionReadSymmetricKeyDBRec symmetricKeyDB.keyName[%s] ret[%d]!\n",keyName,ret);
				return(ret);
			}

			if (mode == 2)
			{
				// 检查密钥值是否存在
				if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) != NULL)
				{
					UnionUserErrLog("in UnionDealServiceCodeE118:: mode[%d] keyName[%s] is find keyValue!\n",mode,keyName);
					UnionSetResponseRemark("对称密钥[%s],密钥值存在",keyName);
					return(errCodeParameter);
				}
			}
			break;

		case	0:
		case	3:
			// 算法标识
			if ((ret = UnionReadRequestXMLPackageValue("body/algorithmID",algorithmID,sizeof(algorithmID))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE118:: UnionReadRequestXMLPackageValue[%s]!\n","body/algorithmID");
				return(ret);
			}

			// 密钥长度
			if ((ret = UnionReadRequestXMLPackageValue("body/keyLen",tmpNum,sizeof(tmpNum))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE118:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyLen");
				return(ret);
			}
			else
				keyLen = atoi(tmpNum);
			
			// 检查密钥长度及算法标识
			if (strcmp(algorithmID,"DES") == 0)
			{
				symmetricKeyDB.algorithmID = conSymmetricAlgorithmIDOfDES;
				switch (keyLen)
				{
					case	64:
					case	16:
						symmetricKeyDB.keyLen = con64BitsSymmetricKey;
						break;
					case	128:
					case	32:
						symmetricKeyDB.keyLen = con128BitsSymmetricKey;
						break;
					case	192:
					case	48:
						symmetricKeyDB.keyLen = con192BitsSymmetricKey;
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE110:: keyLen[%d] is invalid!\n",keyLen);
						return(errCodeEssc_KeyLength);
				}
			}
			else if (strcmp(algorithmID,"SM4") == 0)
			{
				symmetricKeyDB.algorithmID = conSymmetricAlgorithmIDOfSM4;

				if (keyLen != 128)
				{
					UnionUserErrLog("in UnionDealServiceCodeE118:: keyLen[%d] != 128!\n",keyLen);
					UnionSetResponseRemark("密钥长度[%d],必须为128",keyLen);
					return(errCodeEssc_KeyLength);
				}
				symmetricKeyDB.keyLen = con128BitsSymmetricKey;
			}
			else
			{
				UnionUserErrLog("in UnionDealServiceCodeE118:: algorithmID[%s] != DES or SM4 !\n",algorithmID);
				UnionSetResponseRemark("算法标识[%s],必须为[DES或SM4]",algorithmID);
				return(errCodeEsscMDL_InvalidAlgorithmID);
			}

			//if (mode == 0)
			{
				// 密钥类型
				if ((ret = UnionReadRequestXMLPackageValue("body/keyType",keyType,sizeof(keyType))) <= 0)
				{
					//UnionUserErrLog("in UnionDealServiceCodeE118:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyType");
					//return(ret);    
					strcpy(keyType,"ZMK");
					if ((ret = UnionSetRequestXMLPackageValue("body/keyType",keyType)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE118:: UnionSetRequestXMLPackageValue[%s]!\n","body/keyType");
						return(ret);

					}
					if ((ret = UnionSetRequestXMLPackageValue("body/enabled","1")) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE118:: UnionSetRequestXMLPackageValue[%s]!\n","body/enabled");
						return(ret);
					}
				}
				if ((symmetricKeyDB.keyType = UnionConvertSymmetricKeyKeyType(keyType)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE118:: keyType[%s] error!\n",keyType);
					return(symmetricKeyDB.keyType); 
				}

				// add by liwj 20150706
				// mode = 3 时，E110 mode是没有3的，需要将mode 调整为0
				if (mode == 3)
				{
					snprintf(tmpBuf, sizeof(tmpBuf), "%d", 0);
					if ((ret = UnionSetRequestXMLPackageValue("body/mode",tmpBuf)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE118:: UnionSetRequestXMLPackageValue[%s]!\n","body/mode");
						return(ret);
					}
				}
				// end

				if ((ret = UnionDealServiceCodeE110(phsmGroupRec)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE118:: UnionDealServiceCodeE110!\n");
					UnionSetResponseRemark("存储密钥信息失败");
					return(ret);
				}
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE118:: mode[%d] != 1 or 2 or 3!\n",mode);
			UnionSetResponseRemark("非法打印密钥模式[%d]",mode);
			return(errCodeParameter);
	}

	// 打印格式
	if ((ret = UnionReadRequestXMLPackageValue("body/formatName",formatName,sizeof(formatName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE118:: UnionReadRequestXMLPackageValue[%s]!\n","body/formatName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(formatName);

	// 根据打印名称查找打印格式
	len = sprintf(sql,"select * from keyPrintFormat where formatName = '%s'",formatName);
	sql[len] = 0;
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE118:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE118:: formatName[%s] not found, sql[%s]!\n",formatName,sql);
		UnionSetResponseRemark("打印名称[%s]没有找到",formatName);
		return(errCodeDatabaseMDL_RecordNotFound);
	}

	if ((ret = UnionLocateXMLPackage("detail",1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE118:: UnionLocateXMLPackage[%s]!\n","detail");
		return(ret);
	}

	// 获取校验值打印格式
	if ((ret = UnionReadXMLPackageValue("isCheckAlonePrint",isCheckAlonePrint,sizeof(isCheckAlonePrint))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE118:: UnionReadXMLPackageValue[%s]!\n","isCheckAlonePrint");
		return(ret);
	}

	UnionFilterHeadAndTailBlank(isCheckAlonePrint);
	if (atoi(isCheckAlonePrint) != 0 && atoi(isCheckAlonePrint) != 1)
	{
		UnionUserErrLog("in UnionDealServiceCodeE118:: isCheckAlonePrint[%s] != 0 or != 1!\n",isCheckAlonePrint);
		return(errCodeParameter);
	}

	// 获取打印格式
	if ((ret = UnionReadXMLPackageValue("format",keyPrintFormat,sizeof(keyPrintFormat))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE118:: UnionReadXMLPackageValue[%s]!\n","format");
		return(ret);
	}
	keyPrintFormat[ret] = 0;

	// 分量数量
	if ((ret = UnionReadRequestXMLPackageValue("body/numOfComponent",tmpNum,sizeof(tmpNum))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE118:: UnionReadRequestXMLPackageValue[%s]!\n","body/numOfComponent");
		return(ret);
	}
	else
		numOfComponent = atoi(tmpNum);

	// 检查分量数量
	if (numOfComponent < 2 || numOfComponent > 9)
	{
		UnionUserErrLog("in UnionDealServiceCodeE118:: numOfComponent[%d] < 2 or numOfComponent[%d] > 9!\n",numOfComponent,numOfComponent);
		return(errCodeParameter);
	}

	// 密码机IP
	if ((ret = UnionReadRequestXMLPackageValue("body/hsmIP",hsmIP,sizeof(hsmIP))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE118:: UnionReadRequestXMLPackageValue[%s]!\n","body/hsmIP");
		return(ret);
	}
	
	// 检查IP地址
	if (!UnionIsValidIPAddrStr(hsmIP))
	{
		UnionUserErrLog("in UnionDealServiceCodeE118:: hsmIP[%s] is invalid!\n",hsmIP);
		UnionSetResponseRemark("密码机IP[%s]非法",hsmIP);
		return(errCodeInvalidIPAddr);
	}

	// 附加打印数量
	if ((ret = UnionReadRequestXMLPackageValue("body/appendPrintNum",tmpNum,sizeof(tmpNum))) < 0)
		appendPrintNum = 0;
	else
		appendPrintNum = atoi(tmpNum);

	if (appendPrintNum > maxParamNum)
	{
		UnionUserErrLog("in UnionDealServiceCodeE118:: appendPrintNum[%d] > maxParamNum[%d]!\n",appendPrintNum,maxParamNum);
		UnionSetResponseRemark("附件打印参数[%d]大于最大值[%d]",appendPrintNum,maxParamNum);
		return(errCodeParameter);
	}

	for (i = 0; i < appendPrintNum; i++)
	{
		// 附加打印数量
		sprintf(tmpBuf,"body/appendPrintParam%d",i+1);
		
		// 打印参数
		if ((ret = UnionReadRequestXMLPackageValue(tmpBuf,appendPrintParam[i],sizeof(appendPrintParam[i]))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE118:: UnionReadRequestXMLPackageValue[%s]!\n",tmpBuf);
			return(ret);
		}
	}

	// 生成并打印密钥
	if ((ret = UnionGenerateAndPrintSymmetricKey(phsmGroupRec,&symmetricKeyDB,keyPrintFormat,atoi(isCheckAlonePrint),numOfComponent,hsmIP,appendPrintNum,appendPrintParam)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE118:: UnionGenerateAndPrintSymmetricKey!\n");
		UnionSetResponseRemark("打印密钥失败",symmetricKeyDB.keyName);
		return(ret);
	}

	if (mode != 3)
	{
		// 更新对称密钥
		if ((ret = UnionUpdateSymmetricKeyDBKeyValue(&symmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE118:: UnionUpdateSymmetricKeyDBKeyValue keyName[%s]!\n",symmetricKeyDB.keyName);
			return(ret);
		}
	}

	return(0);
}


