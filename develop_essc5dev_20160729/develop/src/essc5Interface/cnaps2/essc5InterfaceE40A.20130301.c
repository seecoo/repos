#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif  
       
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <libgen.h>


#include "essc5Interface.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "essc5Interface.h"
#include "UnionStr.h"
#include "unionHsmCmd.h"
#include "asymmetricKeyDB.h"
#include "UnionMD5.h"
#include "UnionSHA1.h"
#include "unionHsmCmdVersion.h"
#include "unionREC.h"
#include "unionCertErrCode.h"
#include "unionCertFun.h"
#include "sjl06.h"
#include "unionCertFunSM2.h"

/***************************************
  �������:	E40A
  ������:  	E40A ��ȡ֤���㷨����
  ���ܣ�	��ȡ֤���㷨���ͼ���Կ
 ***************************************/
 
int UnionDealServiceCodeE40A(PUnionHsmGroupRec phsmGroupRec)
{
	int 			ret;
	char 			bankID[48];
	char			pix[16];
	char			cerVersion[16];
		
	int			intAlgFlag = 0;
	char			algFlag[16];
	char			sql[512];
	char			hashID[16];
	
	TUnionAsymmetricKeyDB	asymmetricKeyDB;

	memset(&asymmetricKeyDB, 0, sizeof(asymmetricKeyDB));

	// ��ȡ������
	if ((ret = UnionReadRequestXMLPackageValue("body/bankID", bankID, sizeof(bankID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE40A:: UnionReadRequestXMLPackageValue[%s]!\n", "body/bankID");
		return(ret);
	}
	else if(ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE40A:: UnionReadRequestXMLPackageValue[%s]!\n", "body/bankID");
		UnionSetResponseRemark("�����Ų���Ϊ��");
		return(errCodeParameter);
	}
	bankID[ret] = 0;
	
	// ��ȡӦ��ID
	if ((ret = UnionReadRequestXMLPackageValue("body/pix", pix, sizeof(pix))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE40A:: UnionReadRequestXMLPackageValue[%s]!\n", "body/pix");
		return(ret);
	}
	else
	{
		pix[ret] = 0;
		UnionFilterHeadAndTailBlank(pix);
		if (strlen(pix) == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE40A:: pix[%s] cna not be null!\n", pix);
			UnionSetResponseRemark("Ӧ��ID����Ϊ��");
			return(errCodeParameter);
		}
		if(!UnionIsBCDStr(pix))
		{
			UnionUserErrLog("in UnionDealServiceCodeE40A:: pix[%s] is error!\n", pix);
			UnionSetResponseRemark("Ӧ��ID[%s]�Ƿ�������Ϊʮ��������", pix);
			return(errCodeParameter);
		}
	}
	
	//��ȡ�汾��
	if ((ret = UnionReadRequestXMLPackageValue("body/certVersion", cerVersion, sizeof(cerVersion))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE40A:: UnionReadRequestXMLPackageValue[%s]!\n", "body/certVersion");
		return(ret);
	}
	else if(ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE40A:: UnionReadRequestXMLPackageValue[%s]!\n", "body/certVersion");
		UnionSetResponseRemark("�汾�Ų���Ϊ��");
		return(errCodeParameter);
	}
	cerVersion[ret] = 0;
	
	// ��ȡ�㷨��ʶ
	if ((ret = UnionReadRequestXMLPackageValue("body/algFlag", algFlag, sizeof(algFlag))) <= 0)
	{
		snprintf(asymmetricKeyDB.keyName, sizeof(asymmetricKeyDB.keyName), "CNAPS2.%s-%s-%s.cer", pix, bankID, cerVersion);

		if ((ret = UnionReadAsymmetricKeyDBRec(asymmetricKeyDB.keyName, 1, &asymmetricKeyDB)) < 0)
		{
			snprintf(asymmetricKeyDB.keyName, sizeof(asymmetricKeyDB.keyName), "CNAPS2.%s-%s-%s.cer_sm2", pix, bankID, cerVersion);
			if ((ret = UnionReadAsymmetricKeyDBRec(asymmetricKeyDB.keyName, 1, &asymmetricKeyDB)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE40A:: UnionReadAsymmetricKeyDBRec[%s]!\n", asymmetricKeyDB.keyName);
				return(ret);
			}
		}
		intAlgFlag = asymmetricKeyDB.algorithmID;
	}
	else
	{
		algFlag[ret] = 0;
		intAlgFlag = atoi(algFlag);
		switch(intAlgFlag)
		{
			case	0:
				snprintf(asymmetricKeyDB.keyName, sizeof(asymmetricKeyDB.keyName), "CNAPS2.%s-%s-%s.cer", pix, bankID, cerVersion);
				break;
			case	1:
				snprintf(asymmetricKeyDB.keyName, sizeof(asymmetricKeyDB.keyName), "CNAPS2.%s-%s-%s.cer_sm2", pix, bankID, cerVersion);
				break;
			default:
				UnionUserErrLog("in UnionDealServiceCodeE40A:: algFlag[%d] error!\n", intAlgFlag);
				return(errCodeParameter);
		}
		if ((ret = UnionReadAsymmetricKeyDBRec(asymmetricKeyDB.keyName, 1, &asymmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE40A:: UnionReadAsymmetricKeyDBRec[%s]!\n", asymmetricKeyDB.keyName);
			return(ret);
		}
	}
	
	snprintf(sql, sizeof(sql), "select * from certOfCnaps2 where keyName = '%s'", asymmetricKeyDB.keyName);
	if ((ret = UnionSelectRealDBRecord(sql, 0, 0l)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE40A:: UnionSelectRealDBRecord[%s]!\n", sql);
		return(ret);
	}
	else if(ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE40A:: cert record not find[%s]!\n", asymmetricKeyDB.keyName);
		UnionSetResponseRemark("֤���¼û�ҵ�");
		return(errCodeParameter);
	}
	UnionLocateXMLPackage("detail", 1);
	if((ret = UnionReadXMLPackageValue("hashID", hashID, sizeof(hashID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE40A:: UnionReadRequestXMLPackageValue[%s]!\n", "body/hashID");
		return(ret);
	}
	
	if ((ret = UnionSetResponseXMLPackageValue("body/hashID", hashID)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE40A:: UnionSetResponseXMLPackageValue[%s]!\n", "body/hashID");
		return(ret);
	}
	
	if(intAlgFlag == 1)
	{
		if ((ret = UnionSetResponseXMLPackageValue("body/pkValue", asymmetricKeyDB.pkValue)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE40A:: UnionSetResponseXMLPackageValue[%s[!\n", "body/pkValue");
			return(ret);
		}
	}
	return 0;
}
