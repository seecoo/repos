
#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "UnionLog.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "unionREC.h"
#include "baseUIService.h"

/***************************************
�������:	0104
������:		��ȡ�û���¼��ʽ
��������:	��ȡ�û���¼��ʽ
***************************************/
int UnionDealServiceCode0104(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	char	userID[40+1];
	char	loginMode[1+1];
	char	sql[128+1];
	char	cardPKXY[512+1];	
	char    ukeyAlgorithmsIdentity[1+1];
        char    ukeyVerifyMode[1+1];
	char	*ptr = NULL;

	// ��ȡ�û�ID
	memset(userID,0,sizeof(userID));
	if ((ret = UnionReadRequestXMLPackageValue("head/userID",userID,sizeof(userID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0104:: UnionReadRequestXMLPackageValue[%s]!\n","head/userID");
		return(ret);
	}

	// �����û���
	memset(sql,0,sizeof(sql));
	sprintf(sql,"select * from sysUser where userID = '%s'",userID);
	
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0104:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0104:: �û�ID[%s]������!\n",sql);
		return(errCodeOperatorMDL_OperatorNotExists);		
	}
	
	if ((ret = UnionLocateXMLPackage("detail", 1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0104:: UnionLocateXMLPackage!\n");
		return(ret);
	}

	// ��¼��ʽ
        memset(loginMode,0,sizeof(loginMode));

	if ((ret = UnionReadXMLPackageValue("loginMode", loginMode, sizeof(loginMode))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0104:: UnionReadXMLPackageValue[%s]!\n","loginMode");
		return(ret);
	}

        if (atoi(loginMode) == 2)
        {
		memset(ukeyAlgorithmsIdentity,0,sizeof(ukeyAlgorithmsIdentity));
		if ((ptr = UnionReadStringTypeRECVar("ukeyLoginAlgorithmsIdentity")) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCode0104:: UnionReadStringTypeRECVar[%s]!\n","ukeyLoginAlgorithmsIdentity");
			return(errCodeRECMDL_VarNotExists);
		}
		strcpy(ukeyAlgorithmsIdentity,ptr);

                // ������Ӧ����

                if ((ret = UnionSetResponseXMLPackageValue("body/ukeyLoginAlgorithmsIdentity",ukeyAlgorithmsIdentity)) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode0104:: UnionSetResponseXMLPackageValue[%s]!\n","body/ukeyAlgorithmsIdentity");
                        return(ret);
                }
		ptr = NULL;
                memset(ukeyVerifyMode,0,sizeof(ukeyVerifyMode));
                if ((ptr = UnionReadStringTypeRECVar("ukeyVerifyMode")) == NULL)
                {
                        UnionUserErrLog("in UnionDealServiceCode0104:: UnionReadStringTypeRECVar[%s]!\n","ukeyVerifyMode");
                        return(errCodeRECMDL_VarNotExists);
                }
                strcpy(ukeyVerifyMode,ptr);

                // ������Ӧ����
                if ((ret = UnionSetResponseXMLPackageValue("body/ukeyVerifyMode",ukeyVerifyMode)) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode0104:: UnionSetResponseXMLPackageValue[%s]!\n","body/ukeyVerifyMode");
                        return(ret);
                }
        }
	else if (atoi(loginMode) == 3)
	{
		if ((ret = UnionReadXMLPackageValue("cardPKXY", cardPKXY, sizeof(cardPKXY))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0104:: UnionReadXMLPackageValue[%s]!\n","cardPKXY");
			return(ret);
		}
		cardPKXY[ret] = 0;

		// ������Ӧ����
		if ((ret = UnionSetResponseXMLPackageValue("body/cardPKXY",cardPKXY)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0104:: UnionSetResponseXMLPackageValue[%s]!\n","body/cardPKXY");
			return(ret);
		}
                ptr = NULL;
                memset(ukeyVerifyMode,0,sizeof(ukeyVerifyMode));
                if ((ptr = UnionReadStringTypeRECVar("ukeyVerifyMode")) == NULL)
                {
                        UnionUserErrLog("in UnionDealServiceCode0104:: UnionReadStringTypeRECVar[%s]!\n","ukeyVerifyMode");
                        return(errCodeRECMDL_VarNotExists);
                }
                strcpy(ukeyVerifyMode,ptr);

                // ������Ӧ����
                if ((ret = UnionSetResponseXMLPackageValue("body/ukeyVerifyMode",ukeyVerifyMode)) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCode0104:: UnionSetResponseXMLPackageValue[%s]!\n","body/ukeyVerifyMode");
                        return(ret);
                }
	}

	// ������Ӧ����
	if ((ret = UnionSetResponseXMLPackageValue("body/loginMode",loginMode)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0104:: UnionSetResponseXMLPackageValue[%s]!\n","body/loginMode");
		return(ret);
	}

	return(0);
}

