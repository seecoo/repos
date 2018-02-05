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
#include "UnionStr.h"
#include "asymmetricKeyDB.h"
#include "unionHsmCmd.h"
#include "remoteKeyPlatform.h"
#include "commWithHsmSvr.h"
#include "unionHsmCmdVersion.h"

/***************************************
服务代码:       8E15
服务名:         申请非对称密钥
功能描述:       申请非对称密钥
***************************************/
int UnionDealServiceCode8E15(PUnionHsmGroupRec phsmGroupRec)
{
        int                             ret;
        char                            keyName[128];
        char                            tmpBuf[32];
        int				addLen = 0;
	int                             lenOfVK = 0;
        char                            vkValue[2560];
        char                            remoteVK[2560];
        unsigned char                   pkexp[32];
	int				lenOfPKExp = 0;
	char				flag[32];

        TUnionAsymmetricKeyDB           asymmetricKeyDB;
        TUnionRemoteKeyPlatform         tkeyApplyPlatform;

        memset(&asymmetricKeyDB,0,sizeof(asymmetricKeyDB));

        // 读取密钥名称
        memset(keyName,0,sizeof(keyName));
        if ((ret = UnionReadRequestXMLPackageValue("body/keyName",asymmetricKeyDB.keyName,sizeof(asymmetricKeyDB.keyName))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E15:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
                return(ret);
        }
        UnionFilterHeadAndTailBlank(asymmetricKeyDB.keyName);
        if (strlen(asymmetricKeyDB.keyName) == 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E15:: keyName can not be null!\n");
                UnionSetResponseRemark("密钥名称不能为空!");
                return(errCodeParameter);
        }

        // 读取非对称密钥
        if ((ret = UnionReadAsymmetricKeyDBRec(asymmetricKeyDB.keyName,0,&asymmetricKeyDB)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E15:: UnionReadAsymmetricKeyDBRec[%s]!\n",asymmetricKeyDB.keyName);
                return(ret);
        }

	//add by zhouxw 20150908
	UnionSetHsmGroupIDForHsmSvr(asymmetricKeyDB.keyGroup);
	//add end
	//add by zhouxw 20151013
        if ((ret = UnionGetHsmGroupRecByHsmGroupID(asymmetricKeyDB.keyGroup,phsmGroupRec)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E15:: UnionGetHsmGroupRecByHsmGroupID 密码机组[%s]不存在!\n",asymmetricKeyDB.keyGroup);
                return(ret);
        }
        //add end
        // 公钥指数
        if (asymmetricKeyDB.algorithmID == conAsymmetricAlgorithmIDOfRSA)
        {
                memset(tmpBuf,0,sizeof(tmpBuf));
                if (asymmetricKeyDB.pkExponent < 256)
                        sprintf(tmpBuf,"%02X",asymmetricKeyDB.pkExponent);
                else if (asymmetricKeyDB.pkExponent < 256 * 256)
                        sprintf(tmpBuf,"%04X",asymmetricKeyDB.pkExponent);
                else
                        sprintf(tmpBuf,"%06X",asymmetricKeyDB.pkExponent);

                memset(pkexp,0,sizeof(pkexp));
                lenOfPKExp = aschex_to_bcdhex(tmpBuf,strlen(tmpBuf),(char *)pkexp);
        }

        // 模式
        // 1：申请当前密钥，默认值
        // 2：申请新密钥
        memset(flag,0,sizeof(flag));
        if ((ret = UnionReadRequestXMLPackageValue("body/flag",flag,sizeof(flag))) <= 0)
                flag[0] = '0';
        else
        {
                if ((flag[0] != '1') && (flag[0] != '2'))
                {
                        UnionUserErrLog("in UnionDealServiceCode8E15:: flag[%s] error!\n",flag);
                        return(errCodeParameter);
                }
        }

	memset(asymmetricKeyDB.oldPKValue,0,sizeof(asymmetricKeyDB.oldPKValue));
        memset(asymmetricKeyDB.oldVKValue,0,sizeof(asymmetricKeyDB.oldVKValue));

        strcpy(asymmetricKeyDB.oldPKValue,asymmetricKeyDB.pkValue);
        strcpy(asymmetricKeyDB.oldVKValue,asymmetricKeyDB.vkValue);

        memset(asymmetricKeyDB.pkValue,0,sizeof(asymmetricKeyDB.pkValue));
        memset(asymmetricKeyDB.vkValue,0,sizeof(asymmetricKeyDB.vkValue));

	// 密钥申请平台
	if (strlen(asymmetricKeyDB.keyApplyPlatform) > 0)
	{
		memset(&tkeyApplyPlatform,0,sizeof(tkeyApplyPlatform));
                if ((ret =  UnionReadRemoteKeyPlatformRec(asymmetricKeyDB.keyApplyPlatform,conRemoteKeyPlatformKeyDirectionOfApply,&tkeyApplyPlatform)) < 0)
                {
			UnionUserErrLog("in UnionDealServiceCode8E15:: UnionReadRemoteKeyPlatformRec keyApplyPlatform[%s]!\n",asymmetricKeyDB.keyApplyPlatform);
                        return(ret);
                }
        }
        else
        {
                UnionUserErrLog("in UnionDealServiceCode8E15:: keyName[%s] keyApplyPlatform can not be null!\n",asymmetricKeyDB.keyName);
                UnionSetResponseRemark("申请平台不能为空");
                return(errCodeParameter);
        }

        // 初始化远程密钥操作请求报文
        if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E15:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
                return(ret);
        }

	// 服务转换
	if ((ret = UnionSetRequestRemoteXMLPackageValue("head/serviceCode","E124")) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E15:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","head/serviceCode","E124");
		return(ret);
	}

	if ((ret = UnionSetRequestRemoteXMLPackageValue("body/mode",flag)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E15:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/mode",flag);
		return(ret);
	}

        // 转发密钥操作到远程平台
        if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E15:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
                return(ret);
        }

	memset(remoteVK,0,sizeof(remoteVK));
        if((ret = UnionReadResponseRemoteXMLPackageValue("body/vkValue",remoteVK,sizeof(remoteVK))) < 0)
        {
		UnionUserErrLog("in UnionDealServiceCode8E15:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/vkValue");
                return(ret);
        }

        if((ret = UnionReadResponseRemoteXMLPackageValue("body/pkValue",asymmetricKeyDB.pkValue,sizeof(asymmetricKeyDB.pkValue))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E15:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/pkValue");
                return(ret);
        }

	switch(asymmetricKeyDB.algorithmID)
        {
                case    conAsymmetricAlgorithmIDOfRSA:  // RSA算法
			switch(phsmGroupRec->hsmCmdVersionID)
                        {
                                case    conHsmCmdVerRacalStandardHsmCmd:
                                case    conHsmCmdVerSJL06StandardHsmCmd:
					memset(vkValue,0,sizeof(vkValue));
                                        lenOfVK = aschex_to_bcdhex(remoteVK,strlen(remoteVK),vkValue);
                                        if ((lenOfVK = UnionHsmCmdUI2("00",NULL,strlen(tkeyApplyPlatform.protectKey),tkeyApplyPlatform.protectKey,NULL,vkValue,lenOfVK,(unsigned char*)asymmetricKeyDB.vkValue)) < 0)
                                        {
						UnionUserErrLog("in UnionDealServiceCode8E15:: UnionHsmCmdUI2!\n");
                                                return(lenOfVK);
                                        }

                                        break;
                                default:
                                        UnionUserErrLog("in UnionDealServiceCode8E15:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
                                        UnionSetResponseRemark("非法的加密机指令类型");
                                        return(errCodeParameter);
                        }
                        break;
                case    conAsymmetricAlgorithmIDOfSM2:
			switch(phsmGroupRec->hsmCmdVersionID)
                        {       
                                case    conHsmCmdVerRacalStandardHsmCmd:
                                case    conHsmCmdVerSJL06StandardHsmCmd:
                                        memset(vkValue,0,sizeof(vkValue));
                                        lenOfVK = aschex_to_bcdhex(remoteVK,strlen(remoteVK),vkValue);
                                        // 填充数据
                                        if (lenOfVK % 16 != 0)
                                        {
                                                addLen = 16 - lenOfVK % 16;
                                        	sprintf(vkValue + lenOfVK,"%0*d",addLen,0);
						lenOfVK += addLen; 
					}
			
                                        memset(tmpBuf,0,sizeof(tmpBuf));
                                        if ((lenOfVK = UnionHsmCmdWA(1,"01","000",1,tkeyApplyPlatform.protectKey,tkeyApplyPlatform.checkValue,NULL,lenOfVK,(unsigned char*)vkValue,tmpBuf)) < 0)
                                        {
                                                UnionUserErrLog("in UnionDealServiceCode8E15:: UnionHsmCmdWA!\n");
                                                return(lenOfVK);
                                        }
					memset(asymmetricKeyDB.vkValue,0,sizeof(asymmetricKeyDB.vkValue));
					bcdhex_to_aschex(tmpBuf,lenOfVK,asymmetricKeyDB.vkValue);
                                        break;
                                default:
                                        UnionUserErrLog("in UnionDealServiceCode8E15:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
                                        UnionSetResponseRemark("非法的加密机指令类型");
                                        return(errCodeParameter);
                        }
                        break;
                default:
                        UnionUserErrLog("in UnionDealServiceCode8E15:: asymmetricKeyDB.algorithmID[%d] is invalid!\n",asymmetricKeyDB.algorithmID);
			return(errCodeEsscMDL_InvalidAlgorithmID);
        }

        // 更新非对称密钥
        if ((ret =  UnionUpdateAsymmetricKeyDBKeyValue(&asymmetricKeyDB)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E15:: UnionUpdateAsymmetricKeyDBKeyValue,keyName[%s]!\n",asymmetricKeyDB.keyName);
				return(ret);
        }
        
        return(0);
}


