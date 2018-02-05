//      Author:         张永定
//      Copyright:      Union Tech. Guangzhou
//      Date:           2013-01-10

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
#include "essc5UIService.h"
#include "UnionStr.h"
#include "symmetricKeyDB.h"
#include "unionHsmCmd.h"
#include "remoteKeyPlatform.h"
#define maxGrpNum 100

/***************************************
服务代码:       8EC1
服务名:		导入加密机密钥信息
功能描述:	导入加密机密钥信息
***************************************/
int UnionDealServiceCode8EC1(PUnionHsmGroupRec phsmGroupRec)
{
	
	char            	sql[1024];
	int                     ret;
        char            	fileDir[512];
	FILE                    *fp;
	char			ParameterValue[maxGrpNum][128];
	char                    tmpBuf[20480];
	char                    keyIndex[32];
	char			desaddress[1024];
	char			ssfaddress[1024];
	char			keyType[32];
	char			flag[32];
	char			keyLen[32];
	char			groupNum[32];
	char			keyName[1024];
	char                    fileFullName[512];
	char                    fileName[128];
	char			*ptr = NULL;
	char                    fileStoreDir[512];
	
		
	// 读取文件存储目录
        memset(fileStoreDir,0,sizeof(fileStoreDir));
	memcpy(fileStoreDir,"$UNIONREC/fileDir/default",strlen("$UNIONREC/fileDir/default"));	
	memset(fileDir,0,sizeof(fileDir));
        UnionReadDirFromStr(fileStoreDir,-1,fileDir);
	// 文件名
	memset(fileName,0,sizeof(fileName));
        if ((ret = UnionReadRequestXMLPackageValue("body/fileName",fileName,sizeof(fileName))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8EC1:: UnionReadRequestXMLPackageValue[%s]!\n","body/fileName");
                return(ret);
        }

	memset(fileFullName,0,sizeof(fileFullName));
	sprintf(fileFullName,"%s/%s",fileDir,fileName);
	UnionLog("fileFullName = %s\n",fileFullName);
	
	if ((fp = fopen(fileFullName,"r")) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode8EC1:: fopen [%s] is failed!\n",fileFullName);
		return(errCodeParameter);
	}
	while(memset(tmpBuf,0,sizeof(tmpBuf)) && fgets(tmpBuf,20480 ,fp))
        {
				
		UnionFilterHeadAndTailBlank(tmpBuf);
		
		if ((ptr = strstr(tmpBuf,"\r")) != NULL)
			tmpBuf[ptr - tmpBuf] = 0;	
		
		// 拼分域定义串
		memset(ParameterValue,0,sizeof(ParameterValue));
		if ((ret = UnionSeprateVarStrIntoVarGrp(tmpBuf,strlen(tmpBuf),'|',ParameterValue,maxGrpNum)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8EC1:: UnionSeprateVarStrIntoVarGrp[%s]!\n",tmpBuf);
			return(ret);
		}
                
		memset(keyIndex,0,sizeof(keyIndex));
		memcpy(keyIndex,ParameterValue[0],strlen(ParameterValue[0]));
                UnionFilterHeadAndTailBlank(keyIndex);
		if(strlen(keyIndex)==0)
		{
			UnionUserErrLog("in UnionDealServiceCode8EC1:: keyIndex is not null[%s]!\n",keyIndex);
                        return(-1);
		}
		memset(desaddress,0,sizeof(desaddress));
		memcpy(desaddress,ParameterValue[1],strlen(ParameterValue[1]));
                UnionFilterHeadAndTailBlank(desaddress);
		memset(ssfaddress,0,sizeof(ssfaddress));
		memcpy(ssfaddress,ParameterValue[2],strlen(ParameterValue[2]));
                UnionFilterHeadAndTailBlank(ssfaddress);
		memset(keyType,0,sizeof(keyType));
		memcpy(keyType,ParameterValue[3],strlen(ParameterValue[3]));
                UnionFilterHeadAndTailBlank(keyType);
		memset(flag,0,sizeof(flag));
		memcpy(flag,ParameterValue[4],strlen(ParameterValue[4]));
                UnionFilterHeadAndTailBlank(flag);
		memset(keyLen,0,sizeof(keyLen));
		memcpy(keyLen,ParameterValue[5],strlen(ParameterValue[5]));
                UnionFilterHeadAndTailBlank(keyLen);
		if(strlen(keyLen)==0)
		{	
			UnionUserErrLog("in UnionDealServiceCode8EC1:: keyLen is not null[%s]!\n",keyLen);
                        return(-1);
		}
		memset(groupNum,0,sizeof(groupNum));
		memcpy(groupNum,ParameterValue[6],strlen(ParameterValue[6]));
                UnionFilterHeadAndTailBlank(groupNum);
		memset(keyName,0,sizeof(keyName));
		memcpy(keyName,ParameterValue[7],strlen(ParameterValue[7]));
                UnionFilterHeadAndTailBlank(keyName);
                memset(sql,0,sizeof(sql));
                sprintf(sql,"insert into keyIndexInfo(keyIndex,desaddress,ssfaddress,keyType,flag,keyLen,groupNum,keyName) values('%s','%s','%s','%s','%s','%s','%s','%s')",keyIndex,desaddress,ssfaddress,keyType,flag,keyLen,groupNum,keyName);

                if ((ret = UnionExecRealDBSql(sql)) <= 0)
                {
                	UnionUserErrLog("in UnionDealServiceCode8EC1:: UnionExecRealDBSql sql[%s]!\n",sql);
			return (ret);		
                }

	}
	return 0;
}


