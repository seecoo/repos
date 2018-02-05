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
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "unionCommand.h"
#include "unionErrCode.h"
#include "UnionStr.h"
#include "unionRealDBCommon.h"
#include "unionRealBaseDB.h"
#include "symmetricKeyDB.h"
#include "unionVersion.h"
#include "UnionTask.h"
#include "unionHsmCmd.h"
#include "unionXMLPackage.h"

int UnionReadHsmLmkProtectModeByHsmGroupID(char *hsmGroupID,char *lmkProtectMode)
{
	int	ret;
	char	sql[128+1];
	
	memset(sql,0,sizeof(sql));
	sprintf(sql,"select lmkProtectMode from hsmGroup where hsmGroupID='%s'",hsmGroupID);
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		printf("in UnionReadHsmGroupRecFromHsmGroupID:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}
	else if (ret == 0)
	{
		printf("in UnionReadHsmLmkProtectModeByHsmGroupID:: count[0] sql[%s]!\n",sql);
		return(errCodeParameter);
	}

	UnionLocateXMLPackage("detail", 1);

	// 读取保护方式
	if ((ret = UnionReadXMLPackageValue("lmkProtectMode", lmkProtectMode, sizeof(lmkProtectMode))) < 0)
	{
		printf("in UnionReadHsmLmkProtectModeByHsmGroupID:: UnionReadXMLPackageValue[%s]!\n","lmkProtectMode");
		return(ret);
	}
	lmkProtectMode[ret] = 0;

	return 0;
}

// add end by chenwd 20151028
/* 
功能：	读取一个对称密钥表
参数：	keyName[in]		密钥名称
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionReadKeyInfo(char *keyName)
{
	int		ret;
	int		num;
	char		sql[2048];
	char		tmpBuf[1024];

	// 从对称密钥库表中读取
	snprintf(sql,sizeof(sql),"select symmetricKeyDB.keyName,keyGroup,algorithmID,keyType,keyLen,inputFlag,outputFlag,effectiveDays,status,oldVersionKeyIsUsed,activeDate,checkValue,oldCheckValue,keyUpdateTime,keyApplyPlatform,keyDistributePlatform,creatorType,creator,createTime,usingUnit,symmetricKeyDB.remark,lmkProtectMode,keyValue,oldKeyValue from symmetricKeyDB left join symmetricKeyValue on symmetricKeyDB.keyName = symmetricKeyValue.keyName where symmetricKeyDB.keyName = '%s'",keyName);

	if ((num = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionReadKeyInfo:: UnionExecRealDBSql[%s]!\n",sql);
		return(num);
	}
	else if (num == 0)
	{
		UnionLog("in UnionReadKeyInfo keyName[%s] not find!\n",keyName);
		printf("[keyName][%s] not find!\n",keyName);
		return(num);
	}

	UnionLocateXMLPackage("detail", 1);

	if ((ret = UnionReadXMLPackageValue("keyName", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadKeyInfo:: UnionReadXMLPackageValue[%s]!\n","keyName");
		return(ret);
	}
	tmpBuf[ret] = 0;
	printf("[keyName]			[%s]\n",tmpBuf);
	
	if ((ret = UnionReadXMLPackageValue("keyValue", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadKeyInfo:: UnionReadXMLPackageValue[%s]!\n","keyGroup");
		return(ret);
	}
	tmpBuf[ret] = 0;
	printf("[keyValue]			[%s]\n",tmpBuf);
	
	if ((ret = UnionReadXMLPackageValue("checkValue", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadKeyInfo:: UnionReadXMLPackageValue[%s]!\n","checkValue");
		return(ret);
	}
	tmpBuf[ret] = 0;
	printf("[checkValue]			[%s]\n",tmpBuf);
	
	if ((ret = UnionReadXMLPackageValue("oldKeyValue", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadKeyInfo:: UnionReadXMLPackageValue[%s]!\n","keyGroup");
		return(ret);
	}
	tmpBuf[ret] = 0;
	printf("[oldKeyValue]			[%s]\n",tmpBuf);
	
	if ((ret = UnionReadXMLPackageValue("oldVersionKeyIsUsed", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadKeyInfo:: UnionReadXMLPackageValue[%s]!\n","oldVersionKeyIsUsed");
		return(ret);
	}
	tmpBuf[ret] = 0;
	printf("[oldVersionKeyIsUsed]		[%s]\n",tmpBuf);
	
	if ((ret = UnionReadXMLPackageValue("keyGroup", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadKeyInfo:: UnionReadXMLPackageValue[%s]!\n","keyGroup");
		return(ret);
	}
	tmpBuf[ret] = 0;
	printf("[keyGroup]			[%s]\n",tmpBuf);

	if ((ret = UnionReadXMLPackageValue("algorithmID", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadKeyInfo:: UnionReadXMLPackageValue[%s]!\n","algorithmID");
		return(ret);
	}
	tmpBuf[ret] = 0;
	printf("[algorithmID]			[%s]\n",tmpBuf);

	if ((ret = UnionReadXMLPackageValue("keyType", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadKeyInfo:: UnionReadXMLPackageValue[%s]!\n","keyType");
		return(ret);
	}
	tmpBuf[ret] = 0;
	
	printf("[keyType]			[%s]\n",UnionConvertSymmetricKeyKeyTypeToExternal(atoi(tmpBuf)));

	if ((ret = UnionReadXMLPackageValue("keyLen", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadKeyInfo:: UnionReadXMLPackageValue[%s]!\n","keyLen");
		return(ret);
	}
	tmpBuf[ret] = 0;
	printf("[keyLen]			[%s]\n",tmpBuf);

	if ((ret = UnionReadXMLPackageValue("inputFlag", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadKeyInfo:: UnionReadXMLPackageValue[%s]!\n","inputFlag");
		return(ret);
	}
	tmpBuf[ret] = 0;
	printf("[inputFlag]			[%s]\n",tmpBuf);

	if ((ret = UnionReadXMLPackageValue("outputFlag", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadKeyInfo:: UnionReadXMLPackageValue[%s]!\n","outputFlag");
		return(ret);
	}
	tmpBuf[ret] = 0;
	printf("[outputFlag]			[%s]\n",tmpBuf);

	if ((ret = UnionReadXMLPackageValue("effectiveDays", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadKeyInfo:: UnionReadXMLPackageValue[%s]!\n","effectiveDays");
		return(ret);
	}
	tmpBuf[ret] = 0;
	printf("[effectiveDays]			[%s]\n",tmpBuf);

	if ((ret = UnionReadXMLPackageValue("status", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadKeyInfo:: UnionReadXMLPackageValue[%s]!\n","status");
		return(ret);
	}
	tmpBuf[ret] = 0;
	printf("[status]			[%s]\n",tmpBuf);

	if ((ret = UnionReadXMLPackageValue("oldCheckValue", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadKeyInfo:: UnionReadXMLPackageValue[%s]!\n","oldCheckValue");
		return(ret);
	}
	tmpBuf[ret] = 0;
	printf("[oldCheckValue]			[%s]\n",tmpBuf);
	
	if ((ret = UnionReadXMLPackageValue("keyUpdateTime", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadKeyInfo:: UnionReadXMLPackageValue[%s]!\n","keyUpdateTime");
		return(ret);
	}
	tmpBuf[ret] = 0;
	printf("[keyUpdateTime]			[%s]\n",tmpBuf);
	

	if ((ret = UnionReadXMLPackageValue("activeDate", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadKeyInfo:: UnionReadXMLPackageValue[%s]!\n","activeDate");
		return(ret);
	}
	tmpBuf[ret] = 0;
	printf("[activeDate]			[%s]\n",tmpBuf);

	if ((ret = UnionReadXMLPackageValue("keyApplyPlatform", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadKeyInfo:: UnionReadXMLPackageValue[%s]!\n","keyApplyPlatform");
		return(ret);
	}
	tmpBuf[ret] = 0;
	printf("[keyApplyPlatform]		[%s]\n",tmpBuf);

	if ((ret = UnionReadXMLPackageValue("keyDistributePlatform", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadKeyInfo:: UnionReadXMLPackageValue[%s]!\n","keyDistributePlatform");
		return(ret);
	}
	tmpBuf[ret] = 0;
	printf("[keyDistributePlatform]		[%s]\n",tmpBuf);

	if ((ret = UnionReadXMLPackageValue("creatorType", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadKeyInfo:: UnionReadXMLPackageValue[%s]!\n","creatorType");
		return(ret);
	}
	tmpBuf[ret] = 0;
	printf("[creatorType]			[%s]\n",tmpBuf);

	if ((ret = UnionReadXMLPackageValue("creator", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadKeyInfo:: UnionReadXMLPackageValue[%s]!\n","creator");
		return(ret);
	}
	tmpBuf[ret] = 0;
	printf("[creator]			[%s]\n",tmpBuf);

	if ((ret = UnionReadXMLPackageValue("createTime", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadKeyInfo:: UnionReadXMLPackageValue[%s]!\n","createTime");
		return(ret);
	}
	tmpBuf[ret] = 0;
	printf("[createTime]			[%s]\n",tmpBuf);
	
	if ((ret = UnionReadXMLPackageValue("usingUnit", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadKeyInfo:: UnionReadXMLPackageValue[%s]!\n","usingUnit");
		return(ret);
	}
	tmpBuf[ret] = 0;
	printf("[usingUnit]			[%s]\n",tmpBuf);
	
	if ((ret = UnionReadXMLPackageValue("remark", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadKeyInfo:: UnionReadXMLPackageValue[%s]!\n","remark");
		return(ret);
	}
	tmpBuf[ret] = 0;
	printf("[remark]			[%s]\n",tmpBuf);

	return(0);
}

// add end by chenwd 20151028

// 读取字符串中的域值
/* 
功能：从一个字符串得到指定第几个域的值
输入参数:
         buf             输入字符串
         separator       分隔符
         index           第几个值
输出参数：
         value             第index的值
返回值：
    返回值的长度
*/
int UnionReadFiledFromStr(char *buf,char separator,int index,char *value)
{
	int	i,len;
	char	tmpBuf[10240+1];
	char	*p = NULL;
	char	*ptr = NULL;

	if (index < 1)
		return(errCodeParameter);

	if ((len = strlen(buf)) > (int)sizeof(tmpBuf) -2)
		return(errCodeParameter);

	strcpy(tmpBuf,buf);
	tmpBuf[len] = 0;
	if (tmpBuf[len-1] != separator)
		tmpBuf[len] = separator;
	len = 0;
	for(i = 0; i < index; i++)
	{
		p = tmpBuf;
		if ((ptr = strchr(tmpBuf,separator)) != NULL)
		{
			if (i != index - 1)
			{
				len = strlen(ptr + 1);
				memmove(tmpBuf,ptr + 1, len);
				tmpBuf[len] = 0;
			}
			else
				break;
		}
		else
			return(errCodeParameter);
	}
	memcpy(value,tmpBuf,ptr-p);
	return(ptr-p);
}

// 设置密钥默认值
void UnionSetSymmetrickDefaultValue(PUnionSymmetricKeyDB psymmetricKeyDB)
{
	// 设置默认组		
	strcpy(psymmetricKeyDB->keyGroup,"default");
	//psymmetricKeyDB->algorithmID = conSymmetricAlgorithmIDOfDES;	
	psymmetricKeyDB->inputFlag = 1;
	psymmetricKeyDB->outputFlag = 1;
	psymmetricKeyDB->status = conSymmetricKeyStatusOfEnabled;
	psymmetricKeyDB->oldVersionKeyIsUsed = 1;
	psymmetricKeyDB->creatorType = conSymmetricCreatorTypeOfUser;
	strcpy(psymmetricKeyDB->creator,"mngDesKey");
}

// 将外部长度转换为内部长度
int UnionConvertKeyKeyLenToInside(int keyLen)
{
	switch (keyLen)
	{
		case	16:
		case	64:
			return(con64BitsSymmetricKey);
		case	32:
		case	128:
			return(con128BitsSymmetricKey);
		case	48:
		case	192:
			return(con192BitsSymmetricKey);
		default:
			printf("in UnionConvertKeyKeyLenToInside:: keyLen[%d] is error!\n",keyLen);
			return(errCodeEssc_KeyLength);
	}
}

// 产生密钥
int UnionGenerateKeyValue(PUnionSymmetricKeyDB psymmetricKeyDB)
{
	int 	ret = 0;

	switch(psymmetricKeyDB->algorithmID)
	{
		case conSymmetricAlgorithmIDOfDES:
			if ((ret = UnionHsmCmdA0(0,psymmetricKeyDB->keyType,UnionConvertSymmetricKeyKeyLen(psymmetricKeyDB->keyLen),NULL,psymmetricKeyDB->keyValue[0].keyValue,NULL,psymmetricKeyDB->checkValue)) < 0)
			{
				//printf("in UnionGenerateKeyValue:: UnionHsmCmdA0 err!\n");
				return(ret);
			}
			break;
		case conSymmetricAlgorithmIDOfSM4:
			if ((ret = UnionHsmCmdWI("0",psymmetricKeyDB->keyType,NULL,psymmetricKeyDB->keyValue[0].keyValue,NULL,psymmetricKeyDB->checkValue)) < 0)
			{
				//printf("in UnionGenerateKeyValue:: UnionHsmCmdA0 err!\n");
				return(ret);
			}
			break;
		default:
			printf("in UnionGenerateKeyValue:: key[%s] algorithmID = %d not supported!!\n", psymmetricKeyDB->keyName, psymmetricKeyDB->algorithmID);
			return(-1);
	}

	return 0;
}

// 根据节点创建密钥
int UnionCreateKeyByKeyNode(char *keyNodeFileName,int isGenerateKey,PUnionSymmetricKeyDB psymmetricKeyDB)
{
	int	ret = 0;
	FILE	*fpKeyNode = NULL;
	char	oriNode[32+1];
	char	desNode[32+1];
	char	*ptr = NULL;
	char	prefix[16+1];
	int	lenOfPrefix = 0;
	char	suffix[16+1];
	int	successNum = 0;
	int	failedNum = 0;
	int	len = 0;
	char	failedNodeList[8192+1];	
	int	lenOfNode = 0;

	if ((fpKeyNode = fopen(keyNodeFileName,"r")) == NULL)
	{
		printf("in UnionCreateKeyByKeyNode:: fopen[%s] is failed!\n",keyNodeFileName);
		return(errCodeParameter);
	}
	
	// 获取前缀和后缀
	if ((ptr = strstr(psymmetricKeyDB->keyName,".null.")) != NULL || (ptr = strstr(psymmetricKeyDB->keyName,".NULL.")) != NULL)
	{
		memset(prefix,0,sizeof(prefix));	
		lenOfPrefix = ptr - psymmetricKeyDB->keyName;
		memcpy(prefix,psymmetricKeyDB->keyName,lenOfPrefix);
		
		memset(suffix,0,sizeof(suffix));
		strcpy(suffix,ptr+6);
	}
	else
	{
		printf("in UnionCreateKeyByKeyNode:: keyName[%s] is invalid!\n",psymmetricKeyDB->keyName);
		fclose(fpKeyNode);
		return(errCodeParameter);
	}
	
	memset(failedNodeList,0,sizeof(failedNodeList));
	while(!feof(fpKeyNode))
	{
		if ((ret = UnionReadOneLineFromTxtStr(fpKeyNode,oriNode,sizeof(oriNode))) <= 0)	
			continue;

		oriNode[ret] = 0;
		UnionFilterAllBlankChars(oriNode,strlen(oriNode),desNode,sizeof(desNode));
		
		// 拼装密钥名称
		len = sprintf(psymmetricKeyDB->keyName,"%s.%s.%s",prefix,desNode,suffix);
		psymmetricKeyDB->keyName[len] = 0;
		UnionGetFullSystemDate(psymmetricKeyDB->activeDate);

		if (isGenerateKey)
		{
			if ((ret = UnionGenerateKeyValue(psymmetricKeyDB)) < 0)
			{
				//printf("in UnionCreateKeyByKeyNode:: UnionGenerateKeyValue is failed!  keyName[%s]\n",psymmetricKeyDB->keyName);
				if (failedNum == 0)
					lenOfNode = sprintf(failedNodeList,"失败密钥节点:");
				if (lenOfNode < 8192)
					lenOfNode += sprintf(failedNodeList+lenOfNode,"%s,",desNode);
				else
					sprintf(failedNodeList,"失败数量太多了,无法显示");
				failedNum ++;
				continue;
			}
		}

		//增加密钥
		if ((ret = UnionGenerateSymmetricKeyDBRec(psymmetricKeyDB)) < 0)
		{
			//printf("in UnionCreateKeyByKeyNode:: UnionGenerateSymmetricKeyDBRec[%s]!\n",psymmetricKeyDB->keyName);	
			if (failedNum == 0)
				lenOfNode = sprintf(failedNodeList,"失败密钥节点:");
			if (lenOfNode < 8192)
				lenOfNode += sprintf(failedNodeList+lenOfNode,"%s,",desNode);
			else
				sprintf(failedNodeList,"失败数量太多了,无法显示");
			failedNum ++;
			continue;
		}

		successNum++;
	}
	if (fpKeyNode != NULL)
		fclose(fpKeyNode);
	fpKeyNode = NULL;
	printf("密钥名称[%s.null.%s], 成功数量[%d], 失败数量[%d] %s!\n",prefix,suffix,successNum,failedNum,failedNodeList);
	return (successNum);
}

// 根据节点创建密钥
int UnionCreateKeyByStartNodeAndEndNode(char *appID,char *keyType,int startNode,int endNode,PUnionSymmetricKeyDB psymmetricKeyDB)
{
	int	ret = 0;
	int	successNum = 0;
	int	failedNum = 0;
	int	len = 0;
	char	failedNodeList[8192+1];	
	int	lenOfNode = 0;
	int	i = 0;

	memset(failedNodeList,0,sizeof(failedNodeList));
	for (i = startNode; i <= endNode; i++)
	{
		// 拼装密钥名称
		len = sprintf(psymmetricKeyDB->keyName,"%s.%06d.%s",appID,i,keyType);
		psymmetricKeyDB->keyName[len] = 0;
		UnionGetFullSystemDate(psymmetricKeyDB->activeDate);

		if ((ret = UnionGenerateKeyValue(psymmetricKeyDB)) < 0)
		{
			if (failedNum == 0)
				lenOfNode = sprintf(failedNodeList,"失败密钥节点:");
			if (lenOfNode < 8192)
				lenOfNode += sprintf(failedNodeList+lenOfNode,"%d,",i);
			else
				sprintf(failedNodeList,"失败数量太多了,无法显示");
			failedNum ++;
			continue;
		}

		//增加密钥
		if ((ret = UnionGenerateSymmetricKeyDBRec(psymmetricKeyDB)) < 0)
		{
			if (failedNum == 0)
				lenOfNode = sprintf(failedNodeList,"失败密钥节点:");
			if (lenOfNode < 8192)
				lenOfNode += sprintf(failedNodeList+lenOfNode,"%d,",i);
			else
				sprintf(failedNodeList,"失败数量太多了,无法显示");
			failedNum ++;
			continue;
		}

		successNum++;
	}
	printf("密钥名称[%s.null.%s], 成功数量[%d], 失败数量[%d] %s!\n",appID,keyType,successNum,failedNum,failedNodeList);
	return (successNum);
}
