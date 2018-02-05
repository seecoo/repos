//	Author:		zhouxw
//	Copyright:	Union Tech. Guangzhou
//	Date:		20150713

#ifndef	_realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<time.h>

#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "essc5UIService.h"
#include "UnionStr.h"
#include "symmetricKeyDB.h"
#include "unionHsmCmdVersion.h"
#include "unionREC.h"

#define maxGrpNum 16

/***************************************
服务代码:       8E1D
服务名:         批量插入并打印密钥
功能描述:       批量插入并打印密钥
***************************************/

int UnionDealServiceCode8E1D(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret = 0;
	char    fileStoreDir[512];
        char    fileDir[512];
	char	fileFullName[512];
        int     columns = 0;
        char    delimiter[32];
        char    example[1024];
	char	fileName[128];
        FILE    *fp;
        char    ParameterName[maxGrpNum][128];
        char    ParameterValue[maxGrpNum][128];
	
	int	algorithmID = 0;
	char    appID[32];
	
	int	len;
	char    sql[512];
        char    isCheckAlonePrint[16];
        char    keyValuePrintFormat[2048];
	char    keyValuePrintFormat1[2048];
	int	i = 0;
	int	lineno = 0;
	char	tmpBuf[512];
	int	numOfComponent;
	char	hsmIP[16];
	char	userID[64];
	char	keyName[3][128];
	char	keyType[3][16]={"zak","zpk","zmk"};
	char	appendPara[3][80];
	char	*ptr;
	
	int	status = 0;
	
	char                            fieldList[1024];
        char                            fieldListChnName[4096];
	
	TUnionSymmetricKeyDB		symmetricKeyDB;

	len = snprintf(sql, sizeof(sql), "select * from fileType where fileTypeID = 'TERM'");
	if((ret = UnionSelectRealDBRecord(sql, 0, 0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1D:: UnionSelectRealDBRecord[%s]\n", sql);
		return(ret);
	}
	else if(ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1D:: fileTypeID[TERM]\n");
		return(errCodeDatabaseMDL_RecordNotFound);
	}
	
	if((ret = UnionLocateXMLPackage("detail", 1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1D:: UnionLocateXMLPackage[%s]\n","detail");
		return(ret);
	}
	
	// 读取文件存储目录
	if((ret = UnionReadXMLPackageValue("fileStoreDir", fileStoreDir, sizeof(fileStoreDir))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1D:: UnionReadXMLPackageValue[%s]\n","fileStoreDir");
		return(ret);
	}
	fileStoreDir[ret] = 0;
	UnionReadDirFromStr(fileStoreDir, -1, fileDir);
	
	//读取分隔符
	if((ret = UnionReadXMLPackageValue("delimiter", delimiter, sizeof(delimiter))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1D:: UnionReadXMLPackageValue[%s]\n","delimiter");
		return(ret);
	}
	delimiter[ret] = 0;
	
	//读取示例
	if((ret = UnionReadXMLPackageValue("example", example, sizeof(example))) < 0)
	{	
		UnionUserErrLog("in UnionDealServiceCode8E1D:: UnionReadXMLPackageValue[%s]\n","example");
		return(ret);
	}
	example[ret] = 0;
	UnionFilterHeadAndTailBlank(example);

	//读取列数
	if((ret = UnionReadXMLPackageValue("columns", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1D:: UnionReadXMLPackageValue[%s]\n","columns");
		return(ret);
	}
	tmpBuf[ret] = 0;
	columns = atoi(tmpBuf);

	//拼分域定义串
	if((ret = UnionSeprateVarStrIntoVarGrp(example, strlen(example), delimiter[0], ParameterName, maxGrpNum)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1D:: UnionSeprateVarStrIntoVarGrp[%s]\n", example);
		return(ret);
	}
	if(ret != columns)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1D:: ret[%d] is not eq to columns[%d]\n", ret, columns);
		columns = ret;
	}
	
	//算法标识
	if((ret = UnionReadRequestXMLPackageValue("body/algorithmID", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1D:: UnionReadRequestXMLPackageValue[%s]\n", "body/algorithmID");
		return(ret);
	}
	tmpBuf[ret] = 0;
	algorithmID = atoi(tmpBuf);
	

	//分量数量
	if((ret = UnionReadRequestXMLPackageValue("body/numOfComponent", tmpBuf, sizeof(tmpBuf))) <= 0)
		numOfComponent = 2;
	else
	{
		tmpBuf[ret] = 0;
		numOfComponent =atoi(tmpBuf);
	}
	if(numOfComponent <= 1 || numOfComponent > 9)
        {
                UnionUserErrLog("in UnionDealServiceCode8E1D:: numOfComponent[%d] is invalid\n", atoi(tmpBuf));
                return(errCodeParameter);
        }
	
	// 密码机IP
        if ((ret = UnionReadRequestXMLPackageValue("body/ipAddr",hsmIP,sizeof(hsmIP))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E1D:: UnionReadRequestXMLPackageValue[%s]!\n","body/hsmIP");
                return(ret);
        }
	hsmIP[ret] = 0;
        UnionFilterHeadAndTailBlank(hsmIP);
        if (!UnionIsValidIPAddrStr(hsmIP))
        {
                UnionUserErrLog("in UnionDealServiceCode8E1D:: ipAddr[%s] is valid!\n",hsmIP);
                return(errCodeInvalidIPAddr);
        }	

	if((ret = UnionReadRequestXMLPackageValue("head/userID", userID, sizeof(userID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1D:: UnionReadRequestXMLPackageValue[%s]\n", "head/userID");
		return(ret);
	}
	if(ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1D:: UnionReadRequestXMLPackageValue[%s]\n", "head/userID");
		UnionSetResponseRemark("用户ID为空");
		return(errCodeParameter);
	}
	userID[ret] = 0;
	UnionFilterHeadAndTailBlank(userID);
	
	// 查询打印格式
        len = snprintf(sql, sizeof(sql), "select * from keyPrintFormat where formatName = 'batchKeyPrintFormat'");
        sql[len] = 0;
        if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E1D:: UnionSelectRealDBRecord sql[%s]!\n",sql);
                return(ret);
        }
	else if(ret == 0)
        {       
                UnionUserErrLog("in UnionDealServiceCode8E1C:: record can't found[%s]!\n", sql);
                return(errCodeRecordNotExists);
        }

        if ((ret = UnionLocateXMLPackage("detail",1)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E1D:: UnionLocateXMLPackage[%s]!\n","detail");
                return(ret);
        }
        // 获取是否单独打印校验值
        if ((ret = UnionReadXMLPackageValue("isCheckAlonePrint",isCheckAlonePrint,sizeof(isCheckAlonePrint))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E1D:: UnionReadXMLPackageValue[%s]!\n","isCheckAlonePrint");
                return(ret);
        }
        isCheckAlonePrint[ret] = 0;
        UnionFilterHeadAndTailBlank(isCheckAlonePrint);

        // 获取打印格式
        if ((ret = UnionReadXMLPackageValue("format",keyValuePrintFormat,sizeof(keyValuePrintFormat))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E1D:: UnionReadXMLPackageValue[%s]!\n","format");
                return(ret);
        }
        keyValuePrintFormat[ret] = 0;

	// 设置显示报文体
        if ((ret = UnionSetResponseXMLPackageValue("head/displayBody","1")) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E1D:: UnionSetResponseXMLPackageValue[%s]\n","body/displayBody");
                return(ret);
        }
        
        // 字段清单
        snprintf(fieldList,sizeof(fieldList),"owner,status");
        if ((ret = UnionSetResponseXMLPackageValue("body/fieldList",fieldList)) < 0)
        {       
                UnionUserErrLog("in UnionDealServiceCode8E1D:: UnionSetResponseXMLPackageValue[%s]\n","body/fieldList");              
                return(ret);
        }
        
        // 字段清单中文名
        snprintf(fieldListChnName,sizeof(fieldListChnName),"终端号,状态");
        if ((ret = UnionSetResponseXMLPackageValue("body/fieldListChnName",fieldListChnName)) < 0)
        {       
                UnionUserErrLog("in UnionDealServiceCode8E1D:: UnionSetResponseXMLPackageValue[%s]\n","body/fieldListChnName");       
                return(ret);
        }
        
	//读取文件名
        if((ret = UnionReadRequestXMLPackageValue("body/fileName", fileName, sizeof(fieldList))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E1D:: UnionReadRequestXMLPackageValue[%s]\n", "body/fileName");
                return(ret);
        }
        fileName[ret] = 0;
                
        snprintf(fileFullName, sizeof(fileFullName), "%s/%s", fileDir, fileName);
        UnionLog("fileFullName = %s\n", fileFullName);

        if((fp = fopen(fileFullName, "r")) == NULL)
        {
                UnionUserErrLog("in UnionDealServiceCode8E1D:: fopen [%s] failed\n", fileFullName);
                return(errCodeParameter);
        }
	
	while(memset(tmpBuf, 0, sizeof(tmpBuf)) && fgets(tmpBuf, 512, fp))
	{
		
		if ((ptr = strstr(tmpBuf,"\r")) != NULL)
                        tmpBuf[ptr - tmpBuf] = 0;
		
		//拼分域定义串
		if((ret = UnionSeprateVarStrIntoVarGrp(tmpBuf, strlen(tmpBuf), delimiter[0], ParameterValue, maxGrpNum)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E1D:: UnionSeprateVarStrIntoVarGrp[%s]\n", tmpBuf);
			return(ret);
		}
	
		for(i = 0; i < columns; i++)
		{
			if(strcasecmp(ParameterName[i], "owner") == 0)
			{
				strcpy(appendPara[0], ParameterValue[i]);
				UnionFilterHeadAndTailBlank(appendPara[0]);
			}
			
			if(strcasecmp(ParameterName[i], "organizationNo") == 0)
			{
				strcpy(appendPara[1], ParameterValue[i]);
				UnionFilterHeadAndTailBlank(appendPara[1]);
			}
			
			if(strcasecmp(ParameterName[i], "address") == 0)
			{
				strcpy(appendPara[2], ParameterValue[i]);
				UnionFilterHeadAndTailBlank(appendPara[2]);
			}
			
			if(strcasecmp(ParameterName[i], "app") == 0)
			{
				if((ParameterValue[i] == NULL) || (strlen(ParameterValue[i]) == 0))
					strcpy(appID, "atmp");
				else
				{
					strcpy(appID, ParameterValue[i]);
					UnionFilterHeadAndTailBlank(appID);
				}
			}
		}
		
		//密钥名
        	snprintf(keyName[0], sizeof(keyName[0]), "%s.%s.%s", appID, appendPara[0], "zak");
		snprintf(keyName[1], sizeof(keyName[1]), "%s.%s.%s", appID, appendPara[0], "zpk");
        	snprintf(keyName[2], sizeof(keyName[2]), "%s.%s.%s", appID, appendPara[0], "zmk");
		
		for(i = 0; i < 3; i++)
        	{
                	memset(&symmetricKeyDB, 0, sizeof(symmetricKeyDB));
			memcpy(symmetricKeyDB.keyName, keyName[i], strlen(keyName[i]) + 1);
                	if((ret = UnionReadSymmetricKeyDBRec(symmetricKeyDB.keyName,0,&symmetricKeyDB)) >= 0)
			{
				if(symmetricKeyDB.algorithmID == algorithmID)
					continue;
				else
				{
					UnionUserErrLog("in UnionDealServiceCode8E1D:: [%s][%s] already exist\n", symmetricKeyDB.keyName, symmetricKeyDB.algorithmID == 0 ? "DES算法":"SM4算法");
					status = 4;
					break;
				}
			}
		}
		
		if(i != 3)
                        goto setResponse;
		
		for(i = 0; i < 3; i++)
                {
                        memset(&symmetricKeyDB, 0, sizeof(symmetricKeyDB));
                        memcpy(symmetricKeyDB.keyName, keyName[i], strlen(keyName[i]) + 1);
                        if((ret = UnionReadSymmetricKeyDBRec(symmetricKeyDB.keyName,0,&symmetricKeyDB)) >= 0)
				continue;	
			
                	symmetricKeyDB.keyType = UnionConvertSymmetricKeyKeyType(keyType[i]);
                	symmetricKeyDB.algorithmID = algorithmID;
                	symmetricKeyDB.keyLen = con128BitsSymmetricKey;
	
                	//密钥默认属性
                	strcpy(symmetricKeyDB.keyGroup,"default");
                	symmetricKeyDB.inputFlag = 1;
                	symmetricKeyDB.outputFlag = 1;
                	symmetricKeyDB.status = conSymmetricKeyStatusOfInitial;
                	symmetricKeyDB.oldVersionKeyIsUsed = 1;
                	symmetricKeyDB.creatorType = conSymmetricCreatorTypeOfUser;
                	strcpy(symmetricKeyDB.creator,userID);
	
                	if((ret = UnionCreateSymmetricKeyDB(&symmetricKeyDB)) < 0)
                	{
                	        UnionUserErrLog("in UnionDealServiceCode8E1D:: UnionCreateSymmetricKeyDB[%s] ret = [%d]\n", symmetricKeyDB.keyName,ret);
				status = 1;
				break;
                	}
        	}

		if(i != 3)
			goto setResponse;
		
		//密钥插入正常后插入终端打印记录
		len = snprintf(sql, sizeof(sql), "insert into keyPrintMsg (terminalID,branchID,address,algorithmID) values('%s','%s','%s',%d)", appendPara[0], appendPara[1], appendPara[2], algorithmID);
                sql[len] = 0;
                UnionExecRealDBSql(sql);
		
                // 打印密钥
                memcpy(keyValuePrintFormat1, keyValuePrintFormat, strlen(keyValuePrintFormat)+1);
                if ((ret = UnionGenerateAndPrintSymmetricKey(phsmGroupRec,&symmetricKeyDB,keyValuePrintFormat1,atoi(isCheckAlonePrint),numOfComponent,hsmIP,3,appendPara)) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode8E1D:: UnionGenerateAndPrintSymmetricKey ret = [%d]\n", ret);
                        status = 2;
			goto setResponse;
                }

                //更新密钥
                if ((ret = UnionUpdateSymmetricKeyDBKeyValue(&symmetricKeyDB)) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode8E1D:: UnionUpdateSymmetricKeyDBKeyValue keyName[%s]!\n",symmetricKeyDB.keyName);
                        status = 3;
                	goto setResponse;
		}
                status = 0;
		
setResponse:
		//设置响应	
                if((ret = UnionLocateResponseNewXMLPackage("body/detail", lineno+1)) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode8E1D:: UnionLocateResponseXMLPackage[%s][%d]\n","body/detail", i+1);
                        return(ret);
                }

                //终端号
                if((ret = UnionSetResponseXMLPackageValue("owner", appendPara[0])) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode8E1D:: UnionSetResponseXMLPackageValue[%s][%d]\n", "owner", lineno+1);
                        return(ret);
                }

                //状态
                switch(status)
                {
                        case 0:
                                snprintf(tmpBuf, sizeof(tmpBuf), "成功");
                                break;	
			case 1:
                                snprintf(tmpBuf, sizeof(tmpBuf), "创建密钥失败");
                                break;
                        case 2:
                                snprintf(tmpBuf, sizeof(tmpBuf), "打印密钥失败");
                                break;
                        case 3:
                                snprintf(tmpBuf, sizeof(tmpBuf), "更新密钥失败");
                                break;
			case 4:
				snprintf(tmpBuf, sizeof(tmpBuf), "%s算法的密钥已存在", algorithmID == 0 ? "SM4":"DES");
				break;
                        default:
                                UnionUserErrLog("in UnionDealServiceCode8E1D:: invalid status[%d]\n", status);
                                return(errCodeParameter);
                }
                if((ret = UnionSetResponseXMLPackageValue("status", tmpBuf)) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode8E1D:: UnionSetResponseXMLPackageValue[%s][%d]\n","status", lineno+1);
                        return(ret);
                }
		lineno++;
	}
	fclose(fp);
	// 设置总数
	if((ret = UnionLocateResponseXMLPackage("body", 0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1D:: UnionLocateResponseXMLPackage[body]\n");	
		return(ret);
	}
        snprintf(tmpBuf,sizeof(tmpBuf),"%d",lineno);
        if ((ret = UnionSetResponseXMLPackageValue("totalNum",tmpBuf)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E1D:: UnionSetResponseXMLPackageValue[%s]\n","body/totalNum");
                return(ret);
        }

	return(0);
}

