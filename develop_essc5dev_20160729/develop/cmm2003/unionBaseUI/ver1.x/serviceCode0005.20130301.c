//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "baseUIService.h"
#include "otpInterface.h"



/***************************************
服务代码:       0005
服务名:         获取与验证激活码
功能描述:       获取与验证激活码
***************************************/
int UnionDealServiceCode0005(PUnionHsmGroupRec phsmGroupRec)
{

        char    userID[40+1];
        char    IMEI[40+1];
        char    cvk[8+1];
        char    activeCode[12+1];
        char    tokenType[16];
        char    sql[512];
        int 	ret = -1;

        memset(tokenType, 0, sizeof(tokenType));
        if((ret = UnionReadRequestXMLPackageValue("body/tokenType",tokenType,sizeof(tokenType))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode0005:: UnionReadRequestXMLPackageValue[%s]!\n","body/tokenType");
                return ret;
        }

        if(tokenType[0] != '0' && tokenType[0] != '1')
        {
                UnionUserErrLog("in UnionDealServiceCode0005::tokenType = %s ERR!\n", tokenType);
                return errCodeParameter;
        }
        if(tokenType[0] == '0')
        {
                // 读取待激活用户
                memset(userID,0,sizeof(userID));
                if ((ret = UnionReadRequestXMLPackageValue("body/userID",userID,sizeof(userID))) < 0)
                {
                                UnionUserErrLog("in UnionDealServiceCode0005:: UnionReadRequestXMLPackageValue[%s]!\n","body/userID");
                                return(ret);
                }

                // 读取IMEI
                memset(IMEI,0,sizeof(IMEI));
                if ((ret = UnionReadRequestXMLPackageValue("body/IMEI",IMEI,sizeof(IMEI))) < 0)
                {
                                UnionUserErrLog("in UnionDealServiceCode0005:: UnionReadRequestXMLPackageValue[%s]!\n","body/IMEI");
                                return(ret);
                }

                // 读取cvk
                memset(cvk,0,sizeof(cvk));
                if ((ret = UnionReadRequestXMLPackageValue("body/cvk",cvk,sizeof(cvk))) < 0)
                {
                                UnionUserErrLog("in UnionDealServiceCode0005:: UnionReadRequestXMLPackageValue[%s]!\n","body/cvk");
                                return(ret);
                }

                if((ret = UnionActiveTokenEx(userID,IMEI,cvk,activeCode))<0)
                {
                        UnionUserErrLog("in UnionDealServiceCode0005:: UnionActiveTokenEx err userID=[%s]!,IMEI=[%s],cvk=[%s]\n",userID,IMEI,cvk);
                        return(ret);

                }

                // 设置响应数据
                if ((ret = UnionSetResponseXMLPackageValue("body/activeCode",activeCode)) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode0005:: UnionSetResponseXMLPackageValue[%s]!\n","body/activeCode");
                        return(ret);
                }
        }
        else if(tokenType[0] == '1')
        {
                // 读取待激活用户
                memset(userID,0,sizeof(userID));
                if ((ret = UnionReadRequestXMLPackageValue("body/userID",userID,sizeof(userID))) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode0005:: UnionReadRequestXMLPackageValue[%s]!\n","body/userID");
                        return(ret);
                }

                // 读取IMEI
                memset(IMEI,0,sizeof(IMEI));
                if ((ret = UnionReadRequestXMLPackageValue("body/IMEI",IMEI,sizeof(IMEI))) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode0005:: UnionReadRequestXMLPackageValue[%s]!\n","body/IMEI");
                        return(ret);
                }

                memset(activeCode, 0, sizeof(activeCode));
                if((ret = UnionReadRequestXMLPackageValue("body/activeCode", activeCode, sizeof(activeCode))) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode0005:: UnionReadRequestXMLPackageValue[%s]!\n","body/activeCode");
                        return ret;
                }

                if((ret = UnionVerifyTokenActiveCode(userID,IMEI,activeCode)) != 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode0005::UnionVerifyTokenActiveCode Err!\n");
                        return ret;
                }

        }

        //将激活的信息插入数据库sysUser表中
        sprintf(sql,"update sysUser set userPk='%s|%s' where userID = '%s'",IMEI,activeCode,userID);
        if ((ret = UnionExecRealDBSql(sql)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode0005:: UnionExecRealDBSql[%s]!\n",sql);
                return(ret);
        }
        return(0);
}
