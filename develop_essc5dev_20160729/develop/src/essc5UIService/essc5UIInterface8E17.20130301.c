#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "essc5UIService.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "essc5Interface.h"
#include "UnionStr.h"
#include "unionHsmCmd.h"
#include "remoteKeyPlatform.h"
#include "commWithHsmSvr.h"
#include "unionCertFun.h"
#include "unionVKForm.h"
#include "unionREC.h"
#include "unionHsmCmdVersion.h"

int doImportVK(int vkIndex,char *Vkey,char *localKEK,int vkIdx,char* vkeyByLmk);
int updateGkKey(char* certID,char* caCert,char* bankCert,char * VK,char* pfxFileName,char* caFileName,int vkIndex);

int UnionDealServiceCode8E17(PUnionHsmGroupRec phsmGroupRec)
{
        int     ret = 0;
        char    filePath[128];
        char    realBankCert[256];
        char    realCaCert[256];
        char    bankCert[64];
        char    caCert[64];
        char    certID[32];
        char    *ptr = NULL;
        char    passwd[32];
        char    VK[4096];
        char    kekByLmk[64];
        char    vkeyByLmk[4096];
        char    cerBuf[4096];
        char    caCerBuf[4096];
        char    vkIndex[32];

        memset(realBankCert,0,sizeof(realBankCert));
        memset(bankCert,0,sizeof(bankCert));
        memset(realCaCert,0,sizeof(realCaCert));
        memset(caCert,0,sizeof(caCert));
        memset(certID,0,sizeof(certID));
        memset(passwd,0,sizeof(passwd));
        memset(VK,0,sizeof(VK));
        memset(passwd,0,sizeof(passwd));
        memset(kekByLmk,0,sizeof(kekByLmk));
        memset(vkeyByLmk,0,sizeof(vkeyByLmk));
        memset(cerBuf,0,sizeof(cerBuf));
        memset(caCerBuf,0,sizeof(caCerBuf));
        memset(vkIndex,0,sizeof(vkIndex));

        if ((ret = UnionReadRequestXMLPackageValue("body/pfxFileName",bankCert,sizeof(bankCert))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E17:: UnionReadRequestXMLPackageValue[%s]!\n","body/pfxFileName");
                return(ret);
        }
        if ((ret = UnionReadRequestXMLPackageValue("body/caFileName",caCert,sizeof(caCert))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E17:: UnionReadRequestXMLPackageValue[%s]!\n","body/caFileName");
                return(ret);
        }
        if ((ret = UnionReadRequestXMLPackageValue("body/certID",certID,sizeof(certID))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E17:: UnionReadRequestXMLPackageValue[%s]!\n","body/certID");
                return(ret);
        }
        if ((ret = UnionReadRequestXMLPackageValue("body/password",passwd,sizeof(passwd))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E17:: UnionReadRequestXMLPackageValue[%s]!\n","body/bankVK");
                return(ret);
        }
        if ((ret = UnionReadRequestXMLPackageValue("body/vkIndex",vkIndex,sizeof(vkIndex))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E17:: UnionReadRequestXMLPackageValue[%s]!\n","body/bankVK");
                return(ret);
        }

        memset(filePath,0,sizeof(filePath));
        if ((ptr = UnionReadStringTypeRECVar("defaultDirOfFileReceiver")) == NULL)
        {
                UnionUserErrLog("in UnionDealServiceCode8E17:: UnionReadStringTypeRECVar[%s]!\n","defaultDirOfFileReceiver");
                return(errCodeRECMDL_VarNotExists);
        }

        UnionReadDirFromStr(ptr,-1,filePath);

        sprintf(realBankCert,"%s%s",filePath,bankCert);
        sprintf(realCaCert,"%s%s",filePath,caCert);
        if((ret=UnionGetVKAndCertFromPfx(realBankCert,passwd,VK,"xzhang_test.cer",realCaCert,cerBuf,caCerBuf))<0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E17:: unionGetVKAndCerFromPfX!\n");
                UnionSetResponseRemark("证书解析失败,请检查证书和密码");
                return(ret);
        }
		switch(phsmGroupRec->hsmCmdVersionID)
		{
			case conHsmCmdVerRacalStandardHsmCmd:
			case conHsmCmdVerSJL06StandardHsmCmd:
				if((ret = UnionHsmCmdTA(0, 32, "00A", NULL, kekByLmk))<0)
				{
						UnionUserErrLog("in UnionDealServiceCode8E17:: RacalCmdTA!\n");
						return(ret);
				}
				break;
			default:
				UnionUserErrLog("in UnionDealServiceCode8E17:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
				UnionSetResponseRemark("非法的加密机指令类型");
				return(errCodeParameter);
		}
        if (strlen(kekByLmk) > 0)
        {
                UnionAuditLog("in UnionImportPFXCer::kekbyLmk [%s] !\n", kekByLmk);
                if ((ret = doImportVK(atoi(vkIndex),VK, kekByLmk,18,vkeyByLmk)) < 0)
                {
                        UnionUserErrLog("in UnionImportPFXCer::UnionImportVK err! ret = [%d] !\n", ret);
                        return(ret);
                }
        }else
        {
                UnionUserErrLog("in UnionImportPFXCer::kekByLmk is null,ret=[%d]!\n", ret);
                return(ret);
        }
        ret=updateGkKey(certID,caCerBuf,cerBuf,vkeyByLmk,bankCert,caCert,atoi(vkIndex));
        return ret;
}
int doImportVK(int vkIndex,char *Vkey,char *localKEK,int vkIdx,char* vkeyByLmk)
{
        char    kekLen;
        int     ret = -1;

        char modulues[2048], publicExponent[48];
        char privateExponent[2048],primeP[1024];
        char primeQ[1024], dmp1[1024];
        char dmq1[1024],coef[1024];

        char enmodulues[2048], enpublicExponent[48];
        char enprivateExponent[2048],enprimeP[1024];
        char enprimeQ[1024], endmp1[1024];
        char endmq1[1024],encoef[1024];

        int vkeyByLmklen=0;
        int mlen,pclen,ptlen,pPlen,pQlen,dpl,dql,cfl;
        int n;
        char errCode[32];
        char IV[32] ="0000000000000000";
        char vkByLmkBCD[2048];

        memset(vkeyByLmk,0,sizeof(vkeyByLmk));


        //解析Der格式的Vkey属性值
        memset(modulues,0,sizeof(modulues));
        memset(publicExponent,0,sizeof(publicExponent));
        memset(privateExponent,0,sizeof(privateExponent));
        memset(primeP,0,sizeof(primeP));
        memset(primeQ,0,sizeof(primeQ));
        memset(dmp1,0,sizeof(dmp1));
        memset(dmq1,0,sizeof(dmq1));
        memset(coef,0,sizeof(coef));
        memset(enmodulues,0,sizeof(enmodulues));
        memset(enpublicExponent,0,sizeof(enpublicExponent));
        memset(enprivateExponent,0,sizeof(enprivateExponent));
        memset(enprimeP,0,sizeof(enprimeP));
        memset(enprimeQ,0,sizeof(enprimeQ));
        memset(endmp1,0,sizeof(endmp1));
        memset(endmq1,0,sizeof(endmq1));
        memset(encoef,0,sizeof(encoef));
        memset(vkByLmkBCD,0,sizeof(vkByLmkBCD));

        UnionLog("in doImportVK  UnionUnPackFromVK [%s]\n ",Vkey);

        ret = UnionUnPackFromVK(Vkey, modulues, publicExponent, privateExponent, primeP, primeQ, dmp1, dmq1, coef);

        if ( ret < 0 )
        {
                UnionUserErrLog("in doImportVK:: UnionUnPackFromVK is error!\n");
                return (ret);
        }

        n = (strlen(modulues)*8)/2 ;

        //加密所有RSA属性

        kekLen='1';

        memset(errCode, 0, sizeof(errCode));
        mlen = UnionHsmCmdTG('2',"01","00A",kekLen,localKEK,IV,strlen(modulues),modulues,&mlen,enmodulues);

        if ( mlen < 0 )
        {
                UnionUserErrLog("in doImportVK:: RacalCmdTG modulues error !\n");
                //return(ret);  
        }

        // add 公钥指数

        pclen = UnionHsmCmdTG('2',"01","00A",kekLen,localKEK,IV,strlen(publicExponent),publicExponent,&pclen,enpublicExponent);

        if ( pclen < 0 ) 
        {
                UnionUserErrLog("in doImportVK:: RacalCmdTG publicExponent error !\n");
                //return(ret);  
        }

        //add end 

        ptlen= UnionHsmCmdTG('2',"01","00A",kekLen,localKEK,IV,strlen(privateExponent),privateExponent,&ptlen,enprivateExponent);

        if ( ptlen < 0 )
        {
                UnionUserErrLog("in doImportVK:: RacalCmdTG privateExponent error!\n");
                //return(ret);  
        }

        pPlen= UnionHsmCmdTG('2',"01","00A",kekLen,localKEK,IV,strlen(primeP),primeP,&pPlen,enprimeP);

        if ( pPlen < 0 )
        {
                UnionUserErrLog("in doImportVK:: RacalCmdTG enprimeP error !\n");
                //return(ret);  
        }

        pQlen= UnionHsmCmdTG('2',"01","00A",kekLen,localKEK,IV,strlen(primeQ),primeQ,&pQlen,enprimeQ);

        if ( pQlen < 0 )
        {
                UnionUserErrLog("in doImportVK:: RacalCmdTG enprimeQ error !\n");
                //return(ret);  
        }

        dpl  = UnionHsmCmdTG('2',"01","00A",kekLen,localKEK,IV,strlen(dmp1),dmp1,&dpl,endmp1);

        if ( dpl < 0 )
        {
                UnionUserErrLog("in doImportVK:: RacalCmdTG endmp1 error !\n");
        } 

        dql  = UnionHsmCmdTG('2',"01","00A",kekLen,localKEK,IV,strlen(dmq1),dmq1,&dql,endmq1);

        if ( dql < 0 )
        {
                UnionUserErrLog("in doImportVK:: RacalCmdTG endmq1 error !\n");
                //return(ret);  
        }

        cfl  = UnionHsmCmdTG('2',"01","00A",kekLen,localKEK,IV,strlen(coef),coef,&cfl,encoef);

        if ( cfl < 0 )
        {
                UnionUserErrLog("in doImportVK:: RacalCmdTG encoef error !\n");
                //return(ret);  
        }

        ret = UnionHsmCmdUI("00", IV, strlen(localKEK), localKEK, "00A", n, mlen, enmodulues, pclen, \
                        enpublicExponent, ptlen, enprivateExponent, pPlen, enprimeP, pQlen, enprimeQ, dpl, endmp1, dql, \
                        endmq1, cfl, encoef, (unsigned char*)vkeyByLmk, &vkeyByLmklen);

        if ( ret < 0 )
        {
                UnionUserErrLog("in doImportVK:: RacalCmdUI is error!\n");
                return (ret);
        }

        aschex_to_bcdhex(vkeyByLmk,vkeyByLmklen,vkByLmkBCD);
        if((ret = UnionHsmCmdTY(vkIndex,vkeyByLmklen/2,(unsigned char*)vkByLmkBCD)) < 0)
        {
                UnionUserErrLog("in doImportVK:: RacalCmdTY  vkindex = [%d] \n",vkIndex);
                return(ret);
        }

        UnionProgramerLog("in doImportVK:: RacalCmdTY vkindex = [%d] \n",vkIndex); 

        return (0);
}
int updateGkKey(char* certID,char* caCert,char* bankCert,char * VK,char *pfxFileName,char* caFileName,int vkIndex)
{
        int ret=0;
        char sql[10000];
        memset(sql,0,sizeof(sql));
        sprintf(sql,"insert into certOfMof (certID,caCert,bankCert,bankVK,remark,pfxFileName,caFileName,vkIndex) values('%s','%s','%s','%s','%s','%s','%s',%d) ",certID,caCert,bankCert,VK,"",pfxFileName,caFileName,vkIndex);
        if ((ret = UnionExecRealDBSql(sql)) < 0)
        {
                UnionUserErrLog("in updateGkKey: UnionExecRealDBSql ret = [%d] sql = [%s]!\n",ret,sql);
                if ((ret == (errCodeOffsetOfOracle - 1403)) ||
                        (ret == (errCodeOffsetOfInformix - 100)))
                {
                        ret = errCodeKeyCacheMDL_KeyAlreadyExists;
                }
                UnionSetResponseRemark("证书索引已经存在");
                return(ret);
        }

        return 0;

}


