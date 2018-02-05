#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x
#endif

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"essc5Interface.h"
#include"unionErrCode.h"
#include"unionXMLPackage.h"
#include"unionRealBaseDB.h"
#include"UnionLog.h"
#include"UnionStr.h"
#include"unionHsmCmd.h"
#include"defaultIBMPinOffsetDef.h"
#include"symmetricKeyDB.h"
#include"unionHsmCmdVersion.h"

int UnionDealUnionPayResponsePackage274(int serviceID,char *appNo,char *bankNo)
{
	int		ret = 0;
	char		zpkName[136];
	char		cpvkName[136];
	char		cvpvkName[136];

	if (serviceID == 274)
	{
		snprintf(zpkName,sizeof(zpkName),"%s.%s.ZPK",bankNo,appNo);
		snprintf(cpvkName,sizeof(cpvkName),"%s.56.CPVK",bankNo);
		snprintf(cvpvkName,sizeof(cvpvkName),"%s.56.CVPVK",bankNo);
	}
	else if (serviceID == 275)
	{
		snprintf(zpkName,sizeof(zpkName),"%s.%s.ZPK",bankNo,appNo);
		snprintf(cpvkName,sizeof(cpvkName),"%s.57.QPVK",bankNo);
		snprintf(cvpvkName,sizeof(cvpvkName),"%s.57.QVPVK",bankNo);
	}

	if ((ret = UnionSetRequestXMLPackageValue("body/zpkName",zpkName)) < 0)
	{
		UnionUserErrLog("in UnionDealUnionPayResponsePackage274:: UnionSetRequestXMLPackageValue [%d][body/zpkName][%s]!\n",serviceID,zpkName);
		return(ret);
	}
	if ((ret = UnionSetRequestXMLPackageValue("body/cpvkName",cpvkName)) < 0)
	{
		UnionUserErrLog("in UnionDealUnionPayResponsePackage274:: UnionSetRequestXMLPackageValue [%d][body/cpvkName][%s]!\n",serviceID,cpvkName);
		return(ret);
	}
	if ((ret = UnionSetRequestXMLPackageValue("body/cvpvkName",cvpvkName)) < 0)
	{
		UnionUserErrLog("in UnionDealUnionPayResponsePackage274:: UnionSetRequestXMLPackageValue [%d][body/cvpvkName][%s]!\n",serviceID,cvpvkName);
		return(ret);
	}

	return 0;
}

int UnionDealConvertPackageKeyNameWithZPK(int serviceID,char *appNo,char *bankNo1,char *bankNo2)
{
	int		ret = 0;
	char		zpkName[136];
	char		pvkName[136];

	switch(serviceID)
	{
		case	281:
				snprintf(pvkName,sizeof(pvkName),"%s.56.CPVK",bankNo1);
				break;
		case	283:
				snprintf(pvkName,sizeof(pvkName),"%s.56.CVPVK",bankNo1);
				break;
		case	284:
				snprintf(pvkName,sizeof(pvkName),"%s.56.CPVK",bankNo2);
				break;
		case	285:
				snprintf(pvkName,sizeof(pvkName),"%s.56.CVPVK",bankNo2);
				break;
		case	291:
				snprintf(pvkName,sizeof(pvkName),"%s.57.QPVK",bankNo1);
				break;
		case	293:
				snprintf(pvkName,sizeof(pvkName),"%s.57.QVPVK",bankNo1);
				break;
		case	294:
				snprintf(pvkName,sizeof(pvkName),"%s.57.QPVK",bankNo2);
				break;
		case	295:
				snprintf(pvkName,sizeof(pvkName),"%s.57.QVPVK",bankNo2);
				break;
		default:
			UnionUserErrLog("in UnionDealConvertPackageKeyNameWithZPK:: serviceID[%d] is invalid!\n",serviceID);
			return(errCodeInvalidService);
		
	}

	if ((ret = UnionSetRequestXMLPackageValue("body/keyName",pvkName)) < 0)
	{
		UnionUserErrLog("in UnionDealConvertPackageKeyNameWithZPK:: UnionSetRequestXMLPackageValue [%d][body/keyName2][%s]!\n",serviceID,pvkName);
		return(ret);
	}

	switch (atoi(appNo))
	{
		case	51:
			snprintf(zpkName,sizeof(zpkName),"9999.51.ZPK");
			break;
		case	50:
			snprintf(zpkName,sizeof(zpkName),"9998.50.ZPK");
			break;
		case	60:
			snprintf(zpkName,sizeof(zpkName),"9900.60.ZPK");
			break;
		default:
			snprintf(zpkName,sizeof(zpkName),"%s.%s.ZPK",bankNo1,appNo);
			break;
	}

	if ((ret = UnionSetRequestXMLPackageValue("body/keyNameOfZPK",zpkName)) < 0)
	{
		UnionUserErrLog("in UnionDealConvertPackageKeyNameWithZPK:: UnionSetRequestXMLPackageValue [%d][body/keyNameOfZPK][%s]!\n",serviceID,zpkName);
		return(ret);
	}

	return(0);
}

int UnionDealConvertPackageOneKeyName(int serviceID,char *appNo,char *bankNo)
{
	int	ret = 0;
	char	keyName[136];

	switch(serviceID)
	{
		case	204:
				snprintf(keyName,sizeof(keyName),"%s.%s.ZPK",bankNo,appNo);
				break;
		case	205:
		case	250:
		case	251:
				snprintf(keyName,sizeof(keyName),"%s.%s.ZAK",bankNo,appNo);
				break;
		case	240:
				snprintf(keyName,sizeof(keyName),"%s.%s.VisaCVK",bankNo,appNo);
				break;
		case	241:
				snprintf(keyName,sizeof(keyName),"%s.%s.MasterCVK",bankNo,appNo);
				break;
		case	260:
		case	261:
				snprintf(keyName,sizeof(keyName),"%s.%s.ZNK",bankNo,appNo);
				break;
		case	541:
				snprintf(keyName,sizeof(keyName),"%s.%s.ZMK",bankNo,appNo);
				break;
		case	671:
		case	674:
		case	675:
				snprintf(keyName,sizeof(keyName),"%s.%s.CSCK",bankNo,appNo);
				break;
		default:
			UnionUserErrLog("in UnionDealConvertPackageOneKeyName:: serviceID[%d] is invalid!\n",serviceID);
			return(errCodeInvalidService);
	}

	if ((ret = UnionSetRequestXMLPackageValue("body/keyName",keyName)) < 0)
	{
		UnionUserErrLog("in UnionDealConvertPackageOneKeyName:: UnionSetRequestXMLPackageValue [%d][body/keyName][%s]!\n",serviceID,keyName);
		return(ret);
	}
	return(0);
}

int UnionDealConvertPackageKeyNameWithProtectKey(int serviceID,char *appNo,char *bankNo)
{
	int	ret = 0;
	char	keyName[136];
	char	zmkName[136];

	switch(serviceID)
	{
		case	200:
		case	202:
		case	252:
		case	253:
		case	256:
			snprintf(keyName,sizeof(keyName),"%s.%s.ZPK",bankNo,appNo);
			break;
		case	201:
		case	203:
		case	254:
		case	255:
		case	540:
			snprintf(keyName,sizeof(keyName),"%s.%s.ZAK",bankNo,appNo);
			break;
		case	672:
		case	673:
			snprintf(keyName,sizeof(keyName),"%s.%s.CSCK",bankNo,appNo);
			break;
		default:
			UnionUserErrLog("in UnionDealConvertPackageKeyNameWithProtectKey:: serviceID[%d] is invalid!\n",serviceID);
			return(errCodeInvalidService);
	}
	snprintf(zmkName,sizeof(zmkName),"%s.%s.ZMK",bankNo,appNo);

	if ((ret = UnionSetRequestXMLPackageValue("body/keyName",keyName)) < 0)
	{
		UnionUserErrLog("in UnionDealConvertPackageKeyNameWithProtectKey:: UnionSetRequestXMLPackageValue [%d][body/keyName][%s]!\n",serviceID,keyName);
		return(ret);
	}

	if ((ret = UnionSetRequestXMLPackageValue("body/protectKey",zmkName)) < 0)
	{
		UnionUserErrLog("in UnionDealConvertPackageKeyNameWithProtectKey:: UnionSetRequestXMLPackageValue [%d][body/protectKey][%s]!\n",serviceID,zmkName);
		return(ret);
	}
	return(0);
}

int UnionSetConvertPackageKeyName(char *serviceID,char *appNo)
{
	int	ret = 0;
	char	bankNo1[8];
	char	bankNo2[8];

	if (strcmp(serviceID,"100") == 0)
		return 0;

	if ((ret = UnionReadRequestXMLPackageValue("body/bankID",bankNo1,sizeof(bankNo1))) < 0)
	{
		UnionUserErrLog("in UnionSetConvertPackageKeyName:: UnionReadRequestXMLPackageValue serviceID[%s][body/bankID]!\n",serviceID);
		return(ret);
	}else if (ret == 0)
	{
		UnionUserErrLog("in UnionSetConvertPackageKeyName:: UnionReadRequestXMLPackageValue serviceID[%s] bankID is null!\n",serviceID);
		return(errCodeParameter);
	}
	bankNo1[ret] = 0;
	
	if ((ret = UnionReadRequestXMLPackageValue("body/bankID2",bankNo2,sizeof(bankNo2))) > 0)
		bankNo2[ret] = 0;
	else
		bankNo2[0] = 0;

	if ((strcmp(serviceID,"275") == 0) || (strcmp(serviceID,"274") == 0))
	{
		if ((ret = UnionDealUnionPayResponsePackage274(atoi(serviceID),appNo,bankNo1)) < 0)
		{
			UnionUserErrLog("in UnionSetConvertPackageKeyName:: UnionDealUnionPayResponsePackage274 [%s]!\n",serviceID);
			return(ret);
		}
	}
	else if ((strcmp(serviceID,"281") == 0)	||	\
			(strcmp(serviceID,"283") == 0)	||	\
			(strcmp(serviceID,"284") == 0)	||	\
			(strcmp(serviceID,"285") == 0)	||	\
			(strcmp(serviceID,"291") == 0)  ||	\
			(strcmp(serviceID,"293") == 0)	||	\
			(strcmp(serviceID,"294") == 0)	||	\
			(strcmp(serviceID,"295") == 0))
	{
		if ((ret = UnionDealConvertPackageKeyNameWithZPK(atoi(serviceID), appNo, bankNo1, bankNo2)) < 0)
		{
			UnionUserErrLog("in UnionSetConvertPackageKeyName:: UnionDealConvertPackageKeyNameWithZPK [%s]!\n",serviceID);
			return(ret);
		}
	}
	else if ((strcmp(serviceID,"204") == 0)	||	\
			(strcmp(serviceID,"205") == 0)	||	\
			(strcmp(serviceID,"240") == 0)	||	\
			(strcmp(serviceID,"241") == 0)	||	\
			(strcmp(serviceID,"250") == 0)	||	\
			(strcmp(serviceID,"251") == 0)	||	\
			(strcmp(serviceID,"260") == 0)	||	\
			(strcmp(serviceID,"261") == 0)	||	\
			(strcmp(serviceID,"541") == 0)	||	\
			(strcmp(serviceID,"671") == 0)	||	\
			(strcmp(serviceID,"674") == 0)	||	\
			(strcmp(serviceID,"675") == 0))
	{
		if ((ret = UnionDealConvertPackageOneKeyName(atoi(serviceID), appNo, bankNo1)) < 0)
		{
			UnionUserErrLog("in UnionSetConvertPackageKeyName:: UnionDealConvertPackageOneKeyName [%s]!\n",serviceID);
			return(ret);
		}
	}
	else if ((strcmp(serviceID,"200") == 0) || 	\
			(strcmp(serviceID,"201") == 0)	||	\
			(strcmp(serviceID,"202") == 0)	||	\
			(strcmp(serviceID,"203") == 0)	||	\
			(strcmp(serviceID,"252") == 0)	||	\
			(strcmp(serviceID,"253") == 0)	||	\
			(strcmp(serviceID,"254") == 0)	||	\
			(strcmp(serviceID,"255") == 0)	||	\
			(strcmp(serviceID,"256") == 0)	||	\
			(strcmp(serviceID,"540") == 0)	||	\
			(strcmp(serviceID,"672") == 0)	||	\
			(strcmp(serviceID,"673") == 0))
	{
		if ((ret = UnionDealConvertPackageKeyNameWithProtectKey(atoi(serviceID), appNo, bankNo1)) < 0)
		{
			UnionUserErrLog("in UnionSetConvertPackageKeyName:: UnionDealConvertPackageKeyNameWithProtectKey [%s]!\n",serviceID);
			return(ret);
		}
	}
	/*else
	{
		UnionUserErrLog("in UnionSetConvertPackageKeyName:: UnionDealConvertPackageKeyNameWithProtectKey [%s]!\n",serviceID);
		return(errCodeInvalidService);
	}*/

	return 0;
}
