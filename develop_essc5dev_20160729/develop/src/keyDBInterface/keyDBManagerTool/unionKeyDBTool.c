#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#ifndef _UnionEnv_3_x_
#define _UnionEnv_3_x_	
#endif

#define _realBaseDB_2_x_

#include "UnionProc.h"
#include "UnionLog.h"
#include "unionModule.h"
#include "UnionEnv.h"
#include "UnionStr.h"
#include "unionREC.h"
#include "unionErrCode.h"

#include "unionMemcached.h"
#include "unionRealBaseDB.h"
#include "unionXMLPackage.h"
#include "unionCommand.h"
#include "symmetricKeyDB.h"
#include "unionHighCachedAPI.h"

#include "asymmetricKeyDB.h"

// º”‘ÿ∂‘≥∆√‹‘ø
int UnionReloadSymmetricKeyDB()
{
	int		ret,len,num,recordNum,iCnt,i = 0,j=0;
	int		numOfValue;
	char		sql[128];
	char		tmpBuf[1024];
	char            highCachedKey[256];
	char            keyDBStr[4096];
	//char		keyName[10][64];
	char		tmpKeyName[256];
	int		realNum,numOfPerPage;
	char		dateTime[32];
	TUnionSymmetricKeyDB tsymmetricKeyDB;

	snprintf(sql,sizeof(sql),"select X.*,Y.lmkProtectMode,Y.keyValue,Y.oldKeyValue from %s X left join %s Y on X.keyName=Y.keyName order by X.keyName",defTableNameOfSymmetricKeyDB,defTableNameOfSymmetricKeyValue);
	if ((iCnt = UnionSelectRealDBRecordCounts(defTableNameOfSymmetricKeyDB,NULL,NULL)) <0)
	{
		UnionUserErrLog("in UnionReloadSymmetricKeyDB:: UnionSelectRealDBRecordCounts iCnt=[%d]\n",iCnt);
		return(iCnt);
	}
	numOfPerPage = 500;
	i = iCnt % numOfPerPage;
	if (i > 0)
	{
		num = iCnt / numOfPerPage +1;
	}
	else
	{
		num = iCnt / numOfPerPage;
	}
	realNum = 0;
	UnionGetFullSystemDateTime(dateTime);
	dateTime[14] = 0;
	printf("SymmetricKeyDB:: begin time is [%s]\n",dateTime);
	for(i = 1; i <= num;i++)
	{
		if ((recordNum = UnionSelectRealDBRecord(sql,i,numOfPerPage)) < 0)
		{
			UnionUserErrLog("in UnionReloadSymmetricKeyDB:: UnionSelectRealDBRecord\n");
			return(recordNum);
		}
		tmpKeyName[0] = 0;
		numOfValue = 0;
		for(j = 0; j < recordNum; j++)
		{
			memset(&tsymmetricKeyDB,0,sizeof(tsymmetricKeyDB));
			if ((ret = UnionLocateXMLPackage("detail", j+1)) < 0)
			{
				UnionUserErrLog("in UnionReloadSymmetricKeyDB:: UnionLocateXMLPackage\n");
				continue;
			}	
			if ((ret = UnionReadXMLPackageValue("keyName", tsymmetricKeyDB.keyName, 128)) < 0)
			{
				UnionUserErrLog("in UnionReloadSymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","keyName");
				return(ret);
			}
			
			if ((strlen(tmpKeyName) > 0) && (strcmp(tmpKeyName,tsymmetricKeyDB.keyName) == 0))
			{
				numOfValue ++;
				goto readSymmetricKeyValue;
			}
			else
			{
				strcpy(tmpKeyName,tsymmetricKeyDB.keyName);
				numOfValue = 0;
			}
				
			if ((ret = UnionReadXMLPackageValue("keyGroup", tsymmetricKeyDB.keyGroup, sizeof(tsymmetricKeyDB.keyGroup))) < 0)
        	        {
                	        UnionUserErrLog("in UnionReloadSymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","keyGroup");
				return(ret);
			}
			if ((ret = UnionReadXMLPackageValue("algorithmID", tmpBuf, sizeof(tmpBuf))) < 0)
                	{
                        	UnionUserErrLog("in UnionReloadSymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","algorithmID");
                        	return(ret);
                	}
                	tmpBuf[ret] = 0;
                	tsymmetricKeyDB.algorithmID = atoi(tmpBuf);

                	if ((ret = UnionReadXMLPackageValue("keyType", tmpBuf, sizeof(tmpBuf))) < 0)
                	{
				UnionUserErrLog("in UnionReloadSymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","keyType");
				return(ret);
               		}
			tsymmetricKeyDB.keyType = atoi(tmpBuf);

			if ((ret = UnionReadXMLPackageValue("keyLen", tmpBuf, sizeof(tmpBuf))) < 0)
			{
				UnionUserErrLog("in UnionReloadSymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","keyLen");
				return(ret);
			}
			tsymmetricKeyDB.keyLen = atoi(tmpBuf);

			if ((ret = UnionReadXMLPackageValue("inputFlag", tmpBuf, sizeof(tmpBuf))) < 0)
			{
				UnionUserErrLog("in UnionReloadSymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","inputFlag");
				return(ret);
			}
			tsymmetricKeyDB.inputFlag = atoi(tmpBuf);

			if ((ret = UnionReadXMLPackageValue("outputFlag", tmpBuf, sizeof(tmpBuf))) < 0)
			{
				UnionUserErrLog("in UnionReloadSymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","outputFlag");
 				return(ret);
	                }
                	tsymmetricKeyDB.outputFlag = atoi(tmpBuf);

                	if ((ret = UnionReadXMLPackageValue("effectiveDays", tmpBuf, sizeof(tmpBuf))) < 0)
                	{
                	        UnionUserErrLog("in UnionReloadSymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","effectiveDays");
                	        return(ret);
                	}
			tsymmetricKeyDB.effectiveDays = atoi(tmpBuf);

			if ((ret = UnionReadXMLPackageValue("status", tmpBuf, sizeof(tmpBuf))) < 0)
			{
				UnionUserErrLog("in UnionReloadSymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","status");
				return(ret);
			}
			tsymmetricKeyDB.status = atoi(tmpBuf);

    			if ((ret = UnionReadXMLPackageValue("oldVersionKeyIsUsed", tmpBuf, sizeof(tmpBuf))) < 0)
                	{
                       		UnionUserErrLog("in UnionReloadSymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","oldVersionKeyIsUsed");
                       		return(ret);
			}
                	tsymmetricKeyDB.oldVersionKeyIsUsed = atoi(tmpBuf);

                	if ((ret = UnionReadXMLPackageValue("checkValue", tsymmetricKeyDB.checkValue, sizeof(tsymmetricKeyDB.checkValue))) < 0)
                	{
				UnionUserErrLog("in UnionReloadSymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","checkValue");
				return(ret);
			}

			if ((ret = UnionReadXMLPackageValue("oldCheckValue", tsymmetricKeyDB.oldCheckValue, sizeof(tsymmetricKeyDB.oldCheckValue))) < 0)
			{
				UnionUserErrLog("in UnionReloadSymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","oldCheckValue");
				return(ret);
			}
			if ((ret = UnionReadXMLPackageValue("keyUpdateTime", tsymmetricKeyDB.keyUpdateTime, sizeof(tsymmetricKeyDB.keyUpdateTime))) < 0)
			{
				UnionUserErrLog("in UnionReloadSymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","keyUpdateTime");
				return(ret);
			}

			if ((ret = UnionReadXMLPackageValue("activeDate", tsymmetricKeyDB.activeDate, sizeof(tsymmetricKeyDB.activeDate))) < 0)
			{
				UnionUserErrLog("in UnionReloadSymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","activeDate");
				return(ret);
			}

			if ((ret = UnionReadXMLPackageValue("keyApplyPlatform", tsymmetricKeyDB.keyApplyPlatform, sizeof(tsymmetricKeyDB.keyApplyPlatform))) < 0)
			{
               	        	UnionUserErrLog("in UnionReloadSymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","keyApplyPlatform");
               		        return(ret);
               		}

			if ((ret = UnionReadXMLPackageValue("keyDistributePlatform", tsymmetricKeyDB.keyDistributePlatform, sizeof(tsymmetricKeyDB.keyDistributePlatform))) < 0)
              		{
				UnionUserErrLog("in UnionReloadSymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","keyDistributePlatform");
				return(ret);
			}

			if ((ret = UnionReadXMLPackageValue("creatorType", tmpBuf, sizeof(tmpBuf))) < 0)
			{
				UnionUserErrLog("in UnionReloadSymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","creatorType");
				return(ret);
			}
			tsymmetricKeyDB.creatorType = atoi(tmpBuf);

			if ((ret = UnionReadXMLPackageValue("creator", tsymmetricKeyDB.creator, sizeof(tsymmetricKeyDB.creator))) < 0)
			{
				UnionUserErrLog("in UnionReloadSymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","creator");
				return(ret);
			}

			if ((ret = UnionReadXMLPackageValue("createTime", tsymmetricKeyDB.createTime, sizeof(tsymmetricKeyDB.createTime))) < 0)
			{
				UnionUserErrLog("in UnionReloadSymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","createTime");
				return(ret);
			}

			if ((ret = UnionReadXMLPackageValue("createTime", tsymmetricKeyDB.createTime, sizeof(tsymmetricKeyDB.createTime))) < 0)
			{
				UnionUserErrLog("in UnionReloadSymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","createTime");
				return(ret);
			}

			if ((ret = UnionReadXMLPackageValue("createTime", tsymmetricKeyDB.createTime, sizeof(tsymmetricKeyDB.createTime))) < 0)
			{
				UnionUserErrLog("in UnionReloadSymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","createTime");
				return(ret);
			}

			if ((ret = UnionReadXMLPackageValue("usingUnit", tsymmetricKeyDB.usingUnit, sizeof(tsymmetricKeyDB.usingUnit))) < 0)
			{
				UnionUserErrLog("in UnionReloadSymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","usingUnit");
				return(ret);
			}

			if ((ret = UnionReadXMLPackageValue("remark", tsymmetricKeyDB.remark, sizeof(tsymmetricKeyDB.remark))) < 0)
			{
				UnionUserErrLog("in UnionReloadSymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","remark");
				return(ret);
			}
			//read from SymmetricKeyValue

readSymmetricKeyValue:
                        if ((ret = UnionReadXMLPackageValue("lmkProtectMode", tsymmetricKeyDB.keyValue[numOfValue].lmkProtectMode, sizeof(tsymmetricKeyDB.keyValue[numOfValue].lmkProtectMode))) < 0)
                        {
                                UnionUserErrLog("in UnionReloadSymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","lmkProtectMode");
                                return(ret);
                        }
                        if ((ret = UnionReadXMLPackageValue("keyValue", tsymmetricKeyDB.keyValue[numOfValue].keyValue, sizeof(tsymmetricKeyDB.keyValue[numOfValue].keyValue))) < 0)
                        {
                                UnionUserErrLog("in UnionReloadSymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","keyValue");
                                return(ret);
                        }
                        if ((ret = UnionReadXMLPackageValue("oldKeyValue", tsymmetricKeyDB.keyValue[numOfValue].oldKeyValue, sizeof(tsymmetricKeyDB.keyValue[numOfValue].oldKeyValue))) < 0)
                        {
                                UnionUserErrLog("in UnionReloadSymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","oldKeyValue");
                                return(ret);
                        }

			len = UnionGetKeyOfHighCachedForSymmetricKeyDB(tsymmetricKeyDB.keyName,highCachedKey);
			highCachedKey[len] = 0;
			if ((ret = UnionGetHighCachedValue(highCachedKey,keyDBStr,sizeof(keyDBStr))) <= 0)
			{
				UnionSetIsNotUseHighCached();
				if ((len = UnionSymmetricKeyDBStructToString(&tsymmetricKeyDB,keyDBStr)) < 0)
				{
					UnionUserErrLog("in UnionReloadSymmetricKeyDB:: UnionSymmetricKeyDBStructToString!\n");
					return(len);
				}
				keyDBStr[len] = 0;
				
				if ((ret = UnionSetHighCachedValue(highCachedKey,keyDBStr,len,0)) < 0)
				{
					UnionUserErrLog("in UnionReloadSymmetricKeyDB:: UnionSetHighCachedValue error!\n");
				}
				realNum ++;
			}
			else
			{
				if ((ret = UnionSymmetricKeyDBStringToStruct(keyDBStr,&tsymmetricKeyDB)) < 0)
				{
					UnionUserErrLog("in UnionReloadSymmetricKeyDB:: UnionSymmetricKeyDBStringToStruct keyDBStr[%s]!\n",keyDBStr);
					return(ret);
				}
				UnionProgramerLog("in UnionReloadSymmetricKeyDB:: keyName[%s] highCached!\n",tsymmetricKeyDB.keyName);
				realNum ++;
			}
			if((realNum % 500)== 0)
			{
				printf("SymmetricKeyDB:: now reload %d num of key success!\n",realNum);
			}
		}
	}
	printf("SymmetricKeyDB:: now reload %d num of key success!\n",realNum);
	UnionGetFullSystemDateTime(dateTime);
	dateTime[14] = 0;
        printf("SymmetricKeyDB:: the end time is [%s]\n",dateTime);
	return(realNum);
}

// º”‘ÿ∑«∂‘≥∆√‹‘ø
int UnionReloadAsymmetricKeyDB()
{
	int		ret,len,num,recordNum,iCnt,i = 0,j=0;
	int		numOfValue;
	char		sql[128];
	char		tmpBuf[1024];
	char            highCachedKey[256];
	char            keyDBStr[4096];
	char		tmpKeyName[256];
	int		realNum,numOfPerPage;
	char		dateTime[32];
	TUnionAsymmetricKeyDB asymmetricKeyDB;

	snprintf(sql,sizeof(sql),"select * from %s",defTableNameOfAsymmetricKeyDB);
	if ((iCnt = UnionSelectRealDBRecordCounts(defTableNameOfAsymmetricKeyDB,NULL,NULL)) <0)
	{
		UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionSelectRealDBRecordCounts iCnt=[%d]\n",iCnt);
		return(iCnt);
	}
	numOfPerPage = 500;
	i = iCnt % numOfPerPage;
	if (i > 0)
	{
		num = iCnt / numOfPerPage +1;
	}
	else
	{
		num = iCnt / numOfPerPage;
	}
	realNum = 0;
	UnionGetFullSystemDateTime(dateTime);
	dateTime[14] = 0;
	printf("AsymmetricKeyDB:: begin time is [%s]\n",dateTime);
	for(i = 1; i <= num;i++)
	{
		if ((recordNum = UnionSelectRealDBRecord(sql,i,numOfPerPage)) < 0)
		{
			UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionSelectRealDBRecord\n");
			return(recordNum);
		}
		tmpKeyName[0] = 0;
		numOfValue = 0;
		for(j = 0; j < recordNum; j++)
		{
			memset(&asymmetricKeyDB,0,sizeof(asymmetricKeyDB));
			if ((ret = UnionLocateXMLPackage("detail", j+1)) < 0)
			{
				UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionLocateXMLPackage\n");
				continue;
			}	
			if ((ret = UnionReadXMLPackageValue("keyName", asymmetricKeyDB.keyName, 128)) < 0)
			{
				UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","keyName");
				return(ret);
			}
			
			if ((strlen(tmpKeyName) > 0) && (strcmp(tmpKeyName,asymmetricKeyDB.keyName) == 0))
			{
				numOfValue ++;
				goto readAsymmetricKeyValue;
			}
			else
			{
				strcpy(tmpKeyName,asymmetricKeyDB.keyName);
				numOfValue = 0;
			}
				
			if ((ret = UnionReadXMLPackageValue("keyGroup", asymmetricKeyDB.keyGroup, sizeof(asymmetricKeyDB.keyGroup))) < 0)
        	        {
                	        UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","keyGroup");
				return(ret);
			}
			if ((ret = UnionReadXMLPackageValue("algorithmID", tmpBuf, sizeof(tmpBuf))) < 0)
                	{
                        	UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","algorithmID");
                        	return(ret);
                	}
                	tmpBuf[ret] = 0;
                	asymmetricKeyDB.algorithmID = atoi(tmpBuf);
                	if (asymmetricKeyDB.algorithmID == conAsymmetricAlgorithmIDOfRSA)
                	{
                		if ((ret = UnionReadXMLPackageValue("pkExponent", tmpBuf, sizeof(tmpBuf))) < 0)
                		{
                        	UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","pkExponent");
                        	return(ret);
                		}
                	}
									asymmetricKeyDB.pkExponent=atoi(tmpBuf);
									
                	if ((ret = UnionReadXMLPackageValue("keyType", tmpBuf, sizeof(tmpBuf))) < 0)
                	{
				UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","keyType");
				return(ret);
               		}
			asymmetricKeyDB.keyType = atoi(tmpBuf);

			if ((ret = UnionReadXMLPackageValue("keyLen", tmpBuf, sizeof(tmpBuf))) < 0)
			{
				UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","keyLen");
				return(ret);
			}
			asymmetricKeyDB.keyLen = atoi(tmpBuf);

			if ((ret = UnionReadXMLPackageValue("inputFlag", tmpBuf, sizeof(tmpBuf))) < 0)
			{
				UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","inputFlag");
				return(ret);
			}
			asymmetricKeyDB.inputFlag = atoi(tmpBuf);

			if ((ret = UnionReadXMLPackageValue("outputFlag", tmpBuf, sizeof(tmpBuf))) < 0)
			{
				UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","outputFlag");
 				return(ret);
	                }
                	asymmetricKeyDB.outputFlag = atoi(tmpBuf);

                	if ((ret = UnionReadXMLPackageValue("effectiveDays", tmpBuf, sizeof(tmpBuf))) < 0)
                	{
                	        UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","effectiveDays");
                	        return(ret);
                	}
			asymmetricKeyDB.effectiveDays = atoi(tmpBuf);

			if ((ret = UnionReadXMLPackageValue("status", tmpBuf, sizeof(tmpBuf))) < 0)
			{
				UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","status");
				return(ret);
			}
			asymmetricKeyDB.status = atoi(tmpBuf);
			if ((ret = UnionReadXMLPackageValue("vkStoreLocation", tmpBuf, sizeof(tmpBuf))) < 0)
			{
				UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","vkStoreLocation");
				return(ret);
			}
			asymmetricKeyDB.vkStoreLocation = atoi(tmpBuf);
    			if ((ret = UnionReadXMLPackageValue("oldVersionKeyIsUsed", tmpBuf, sizeof(tmpBuf))) < 0)
                	{
                       		UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","oldVersionKeyIsUsed");
                       		return(ret);
			}
                	asymmetricKeyDB.oldVersionKeyIsUsed = atoi(tmpBuf);
			
			if ((ret = UnionReadXMLPackageValue("pkCheckValue", asymmetricKeyDB.pkCheckValue, sizeof(asymmetricKeyDB.pkCheckValue))) < 0)
			{
				UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","pkCheckValue");
				return(ret);
			}
			if ((ret = UnionReadXMLPackageValue("oldPKCheckValue", asymmetricKeyDB.oldPKCheckValue, sizeof(asymmetricKeyDB.oldPKCheckValue))) < 0)
			{
				UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","oldPKCheckValue");
				return(ret);
			}
			if((ret = UnionReadXMLPackageValue("vkCheckValue", asymmetricKeyDB.vkCheckValue, sizeof(asymmetricKeyDB.vkCheckValue))) < 0)
			{
				UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","vkCheckValue");
				return(ret);
			}
			if((ret = UnionReadXMLPackageValue("oldVKCheckValue", asymmetricKeyDB.oldVKCheckValue, sizeof(asymmetricKeyDB.oldVKCheckValue))) < 0)
			{
				UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","oldVKCheckValue");
				return(ret);
			}
			if ((ret = UnionReadXMLPackageValue("keyUpdateTime", asymmetricKeyDB.keyUpdateTime, sizeof(asymmetricKeyDB.keyUpdateTime))) < 0)
			{
				UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","keyUpdateTime");
				return(ret);
			}

			if ((ret = UnionReadXMLPackageValue("activeDate", asymmetricKeyDB.activeDate, sizeof(asymmetricKeyDB.activeDate))) < 0)
			{
				UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","activeDate");
				return(ret);
			}

			if ((ret = UnionReadXMLPackageValue("keyApplyPlatform", asymmetricKeyDB.keyApplyPlatform, sizeof(asymmetricKeyDB.keyApplyPlatform))) < 0)
			{
               	        	UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","keyApplyPlatform");
               		        return(ret);
               		}

			if ((ret = UnionReadXMLPackageValue("keyDistributePlatform", asymmetricKeyDB.keyDistributePlatform, sizeof(asymmetricKeyDB.keyDistributePlatform))) < 0)
              		{
				UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","keyDistributePlatform");
				return(ret);
			}

			if ((ret = UnionReadXMLPackageValue("creatorType", tmpBuf, sizeof(tmpBuf))) < 0)
			{
				UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","creatorType");
				return(ret);
			}
			asymmetricKeyDB.creatorType = atoi(tmpBuf);

			if ((ret = UnionReadXMLPackageValue("creator", asymmetricKeyDB.creator, sizeof(asymmetricKeyDB.creator))) < 0)
			{
				UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","creator");
				return(ret);
			}

			if ((ret = UnionReadXMLPackageValue("createTime", asymmetricKeyDB.createTime, sizeof(asymmetricKeyDB.createTime))) < 0)
			{
				UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","createTime");
				return(ret);
			}

			if ((ret = UnionReadXMLPackageValue("createTime", asymmetricKeyDB.createTime, sizeof(asymmetricKeyDB.createTime))) < 0)
			{
				UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","createTime");
				return(ret);
			}

			if ((ret = UnionReadXMLPackageValue("createTime", asymmetricKeyDB.createTime, sizeof(asymmetricKeyDB.createTime))) < 0)
			{
				UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","createTime");
				return(ret);
			}

			if ((ret = UnionReadXMLPackageValue("usingUnit", asymmetricKeyDB.usingUnit, sizeof(asymmetricKeyDB.usingUnit))) < 0)
			{
				UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","usingUnit");
				return(ret);
			}

			if ((ret = UnionReadXMLPackageValue("remark", asymmetricKeyDB.remark, sizeof(asymmetricKeyDB.remark))) < 0)
			{
				UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","remark");
				return(ret);
			}
			//read from AsymmetricKeyValue

readAsymmetricKeyValue:
                        if ((ret = UnionReadXMLPackageValue("pkValue", asymmetricKeyDB.pkValue, sizeof(asymmetricKeyDB.pkValue))) < 0)
                        {
                                UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","pkValue");
                                return(ret);
                        }
                        if ((ret = UnionReadXMLPackageValue("oldPKValue", asymmetricKeyDB.oldPKValue, sizeof(asymmetricKeyDB.oldPKValue))) < 0)
                        {
                                UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","oldPKValue");
                                return(ret);
                        }
                        if ((ret = UnionReadXMLPackageValue("vkValue", asymmetricKeyDB.vkValue, sizeof(asymmetricKeyDB.vkValue))) < 0)
                        {
                                UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","vkValue");
                                return(ret);
                        }
                        if ((ret = UnionReadXMLPackageValue("oldVKValue", asymmetricKeyDB.oldVKValue, sizeof(asymmetricKeyDB.oldVKValue))) < 0)
                        {
                                UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionReadXMLPackageValue[%s]!\n","oldVKValue");
                                return(ret);
                        }

			len = UnionGetKeyOfHighCachedForAsymmetricKeyDB(asymmetricKeyDB.keyName,highCachedKey);
			highCachedKey[len] = 0;
			if ((ret = UnionGetHighCachedValue(highCachedKey,keyDBStr,sizeof(keyDBStr))) <= 0)
			{
				UnionSetIsNotUseHighCached();
				if ((len = UnionAsymmetricKeyDBStringToStruct(keyDBStr,&asymmetricKeyDB)) < 0)
				{
					UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionAsymmetricKeyDBStructToString!\n");
					return(len);
				}
				keyDBStr[len] = 0;
				
				if ((ret = UnionSetHighCachedValue(highCachedKey,keyDBStr,len,0)) < 0)
				{
					UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionSetHighCachedValue error!\n");
				}
				realNum ++;
			}
			else
			{
				if ((ret = UnionAsymmetricKeyDBStringToStruct(keyDBStr,&asymmetricKeyDB)) < 0)
				{
					UnionUserErrLog("in UnionReloadAsymmetricKeyDB:: UnionAsymmetricKeyDBStringToStruct keyDBStr[%s]!\n",keyDBStr);
					return(ret);
				}
				UnionProgramerLog("in UnionReloadAsymmetricKeyDB:: keyName[%s] highCached!\n",asymmetricKeyDB.keyName);
				realNum ++;
			}
			if((realNum % 500)== 0)
			{
				printf("AsymmetricKeyDB:: now reload %d num of key success!\n",realNum);
			}
		}
	}
	printf("AsymmetricKeyDB:: now reload %d num of key success!\n",realNum);
	UnionGetFullSystemDateTime(dateTime);
	dateTime[14] = 0;
        printf("SymmetricKeyDB:: the end time is [%s]\n",dateTime);
	return(realNum);
	return(0);
}

int UnionReloadKeyDB()
{
	int	ret;
	
	// º”‘ÿ∂‘≥∆√‹‘ø
	if ((ret = UnionReloadSymmetricKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionReloadKeyDB:: UnionReloadSymmetricKeyDB!\n");
		return(ret);
	}
	// º”‘ÿ∑«∂‘≥∆√‹‘ø
	if ((ret = UnionReloadAsymmetricKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionReloadKeyDB:: UnionReloadAsymmetricKeyDB!\n");
		return(ret);
	}
	return(0);
}
