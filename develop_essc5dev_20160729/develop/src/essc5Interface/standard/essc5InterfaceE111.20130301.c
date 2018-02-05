//	Author:		’≈”¿∂®
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "essc5Interface.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "essc5Interface.h"
#include "UnionStr.h"
#include "symmetricKeyDB.h"
#include "unionHsmCmd.h"
#include "remoteKeyPlatform.h"
#include "unionHsmCmdVersion.h"

/***************************************
∑˛ŒÒ¥˙¬Î:	E111
∑˛ŒÒ√˚:		∏¸–¬∂‘≥∆√‹‘ø
π¶ƒ‹√Ë ˆ:	∏¸–¬∂‘≥∆√‹‘ø
***************************************/
int UnionDealServiceCodeE111(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	int				len;
	int				outputByZMK = 0;
	int				mode = 1;
	int				exportFlag = 0;
	char				protectKey[128];
	char				tmpBuf[128];
	char				zmk[64];
	char				tmk[64];
	char				keyByZMK[64];
	char				keyByRKM[64];
	char				keyByTMK[64];
	char				platformProtectKey[64];
	int				isRemoteKeyOperate = 0;
	int				isRemoteApplyKey = 0;
	int				isRemoteDistributeKey = 0;
	int				isDistributeFlag = 0;
	int				isApplyFlag = 0;
	char				remoteKeyValue[64];
	char				remoteCheckValue[32];
	char				sql[128];
	char				exportLmkKey[32];
	char				checkValue[32];
	char				keyName[160];
	int				flag = 0;
	char				algorithmID[8];
	
	char	        	        sysID[32];
        char    	                appID[32];
        char                    	creator[64];

	TUnionSymmetricKeyDB		symmetricKeyDB;
	PUnionSymmetricKeyValue		psymmetricKeyValue = NULL;
	TUnionSymmetricKeyDB		zmkKeyDB;
	PUnionSymmetricKeyValue		pzmkKeyValue = NULL;
	TUnionRemoteKeyPlatform		tkeyApplyPlatform;
	TUnionRemoteKeyPlatform		tkeyDistributePlatform;

	// ƒ£ Ω
        // 1£∫±æµÿ…˙≥…√‹‘ø£¨ƒ¨»œ÷µ
        // 2£∫±æµÿ…˙≥…√‹‘ø£¨≤¢¥Ú”°
        if ((ret = UnionReadRequestXMLPackageValue("body/mode",tmpBuf,sizeof(tmpBuf))) <= 0)
                mode = 1;
        else
        {
		tmpBuf[ret] = 0;
                mode = atoi(tmpBuf);
        }

	// ºÏ≤‚ «∑Ò‘∂≥Ã≤Ÿ◊˜
	isRemoteKeyOperate = UnionIsRemoteKeyOperate();
	if (isRemoteKeyOperate)
		flag = 1;

	
	// √‹‘ø√˚≥∆
	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE111:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}
	keyName[ret] = 0;
	UnionFilterHeadAndTailBlank(keyName);
	
	// ∂¡»°∂‘≥∆√‹‘ø
	UnionInitSymmetricKeyDB(&symmetricKeyDB);
	if ((ret =  UnionReadSymmetricKeyDBRec(keyName,0,&symmetricKeyDB)) < 0)
	{
		if (ret == errCodeKeyCacheMDL_WrongKeyName)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: keyName[%s] not find!\n",keyName);
			UnionSetResponseRemark("∂‘≥∆√‹‘ø[%s]≤ª¥Ê‘⁄",keyName);
			return(errCodeKeyCacheMDL_KeyNonExists);
		}
		UnionUserErrLog("in UnionDealServiceCodeE111:: UnionReadSymmetricKeyDBRec keyName[%s]!\n",keyName);
		return(ret);
	}

	// ºÏ≤È√‹‘ø «∑Òø…“‘ π”√
	if (0 == flag)
	{
		// Œ™ºÊ»›æ…œµÕ≥…Ë∂®µƒºÏ≤È±Í ∂
		if ((ret = UnionReadRequestXMLPackageValue("body/sysFlag",tmpBuf,sizeof(tmpBuf))) > 0)
			flag = 1;
	}
		
	if ((ret = UnionIsUseCheckSymmetricKeyDB(symmetricKeyDB.status,flag)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE111:: UnionIsUseCheckSymmetricKeyDB symmetricKeyDB.status[%d] flag[%d]!\n",symmetricKeyDB.status,flag);
		return(ret);
	}
	
	//  ‰≥ˆ±Í ∂
	// 0£∫≤ª ‰≥ˆ£¨ƒ¨»œ÷µ
	// 1£∫÷∏∂®√‹‘ø√˚≥∆±£ª§ ‰≥ˆ
	// 2£∫Õ‚¥¯ZMK√‹‘ø±£ª§ ‰≥ˆ
	if ((ret = UnionReadRequestXMLPackageValue("body/exportFlag",tmpBuf,sizeof(tmpBuf))) <= 0)
		exportFlag = 0;
	else
	{
		tmpBuf[ret] = 0;
		exportFlag = atoi(tmpBuf);
		if ((exportFlag != 0) && (exportFlag != 1) && (exportFlag != 2) && (exportFlag != 3) && (exportFlag != 9))
			exportFlag = 0;
	}

	// ≤ª‘ –Ì ‰≥ˆ
	if ((symmetricKeyDB.outputFlag == 0) && (exportFlag != 0))
	{
		UnionUserErrLog("in UnionDealServiceCodeE111:: symmetricKeyDB.outputFlag[%d]!\n",symmetricKeyDB.outputFlag);
		return(errCodeEsscMDL_KeyOutputNotPermitted);		
	}
	
	// ±£ª§√‹‘ø
	// µ±exportFlagŒ™0 ±£¨≤ª¥Ê‘⁄
	// µ±exportFlagŒ™1 ±£∫Œ™√‹‘ø√˚≥∆
	// µ±exportFlagŒ™2 ±£¨Œ™√‹‘ø÷µ
	if (exportFlag != 0 && exportFlag != 9)
	{
		if ((ret = UnionReadRequestXMLPackageValue("body/protectKey",protectKey,sizeof(protectKey))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionReadRequestXMLPackageValue[%s]!\n","body/protectKey");
			return(ret);	
		}
		protectKey[ret] = 0;
	}

	exportLmkKey[0] = 0;
	UnionReadRequestXMLPackageValue("body/exportLmkKey",exportLmkKey,sizeof(exportLmkKey));
	
	// ∂¡»°±£ª§√‹‘ø
	if (exportFlag == 0)
		outputByZMK = 0;
	else if (exportFlag == 1)
	{
		outputByZMK = 1;
		UnionInitSymmetricKeyDB(&zmkKeyDB);
		if ((ret =  UnionReadSymmetricKeyDBRec(protectKey,1,&zmkKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionReadSymmetricKeyDBRec protectKey[%s]!\n",protectKey);
			return(ret);
		}
		if ((zmkKeyDB.keyType != conZMK) && (zmkKeyDB.keyType != conTMK))
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: is not zmk or tmk [%s]!\n",protectKey);
			return(errCodeEsscMDL_WrongUsageOfKey);
		}

		if ((pzmkKeyValue = UnionGetSymmetricKeyValue(&zmkKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
			UnionSetResponseRemark("∂¡»°√‹‘ø÷µ ß∞‹");
			return(errCodeParameter);
		}

		if (zmkKeyDB.keyType == conTMK)
		{
			if (symmetricKeyDB.keyType != conTMK && 
				symmetricKeyDB.keyType != conTPK && 
				symmetricKeyDB.keyType != conPVK && 
			 	symmetricKeyDB.keyType != conTAK)
			{
				UnionUserErrLog("in UnionDealServiceCodeE111:: keyName[%s] keyType is not tmk,tpk,pvk or tak!\n",symmetricKeyDB.keyName);
				UnionSetResponseRemark("√‹‘ø[%s]≤ª÷ß≥÷[TMK]±£ª§∑Ω Ω",symmetricKeyDB.keyName);
				return(errCodeParameter);
			}
			strcpy(tmk,pzmkKeyValue->keyValue);
		}
		else
		strcpy(zmk,pzmkKeyValue->keyValue);
	}
	else if(exportFlag == 9)
	{
		outputByZMK = 1;
		//  π”√æ…√‹‘ø±£ª§–¬√‹‘ø£¨±£ª§√‹‘ø‘⁄œ¬√Ê∂¡√‹‘ø÷µ ±ªÒ»°
	}
	else
	{
		outputByZMK = 1;
		len = strlen(protectKey);
		if ((len != 16) && (len != 32) && (len != 48))
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: protectKey[%s] len != [16,32,48]\n",protectKey);
			return(errCodeEssc_KeyLength);
		}
		if (exportFlag == 3)
		{
			if (symmetricKeyDB.keyType != conTMK && 
				symmetricKeyDB.keyType != conTPK && 
				symmetricKeyDB.keyType != conPVK && 
			 	symmetricKeyDB.keyType != conTAK)
			{
				UnionUserErrLog("in UnionDealServiceCodeE111:: keyName[%s] keyType is not tmk,tpk,pvk or tak!\n",symmetricKeyDB.keyName);
				UnionSetResponseRemark("√‹‘ø[%s]≤ª÷ß≥÷[TMK]±£ª§∑Ω Ω",symmetricKeyDB.keyName);
				return(errCodeParameter);
			}

			strcpy(tmk,protectKey);
		}
		else
			strcpy(zmk,protectKey);
	}
	
	// …˙–ß ±º‰
	if ((ret = UnionReadRequestXMLPackageValue("body/activeDate",symmetricKeyDB.activeDate,sizeof(symmetricKeyDB.activeDate))) <= 0)
	{
		UnionGetFullSystemDate(symmetricKeyDB.activeDate);
	}
	UnionFilterHeadAndTailBlank(symmetricKeyDB.activeDate);
	if (!UnionIsValidFullDateStr(symmetricKeyDB.activeDate))
        {
                UnionUserErrLog("in UnionDealServiceCodeE111:: activeDate[%s] error!\n",symmetricKeyDB.activeDate);
                UnionSetResponseRemark("…˙–ß»’∆⁄∏Ò Ω[%s]∑«∑®,±ÿ–ÎŒ™YYYYMMDD∏Ò Ω",symmetricKeyDB.activeDate);
                return(errCodeParameter);
        }

	// ∂¡»°√‹‘ø÷µ
	if ((psymmetricKeyValue = UnionGetSymmetricKeyValue(&symmetricKeyDB,phsmGroupRec->lmkProtectMode)) != NULL)
	{
		// µ±«∞√‹‘ø÷√Œ™æ…√‹‘ø
		strcpy(psymmetricKeyValue->oldKeyValue,psymmetricKeyValue->keyValue);
		strcpy(symmetricKeyDB.oldCheckValue,symmetricKeyDB.checkValue);

		psymmetricKeyValue->keyValue[0] = 0;
		symmetricKeyDB.checkValue[0] = 0;;
	}
	else
		psymmetricKeyValue = &symmetricKeyDB.keyValue[0];

	strcpy(psymmetricKeyValue->lmkProtectMode,phsmGroupRec->lmkProtectMode);

	// add by leipp 20151022
	// À„∑®±Í ∂,»Áπ˚À„∑®±Í ∂”Îµ±«∞√‹‘øÀ„∑®±Í ∂≤ªÕ¨£¨‘Ú«Âø’√‹‘ø÷µ,–ﬁ∏ƒ√‹‘ø Ù–‘
	memset(algorithmID,0,sizeof(algorithmID));
	ret = UnionReadRequestXMLPackageValue("body/algorithmID",algorithmID,sizeof(algorithmID));
	if (ret > 0)
	{
		UnionFilterHeadAndTailBlank(algorithmID);
		if ((strcasecmp(algorithmID,"DES") == 0) && (symmetricKeyDB.algorithmID != conSymmetricAlgorithmIDOfDES))
		{
			symmetricKeyDB.algorithmID = conSymmetricAlgorithmIDOfDES;
			psymmetricKeyValue->oldKeyValue[0] = 0;
			symmetricKeyDB.oldCheckValue[0] = 0;
			strcpy(symmetricKeyDB.keyValue[1].lmkProtectMode,phsmGroupRec->lmkProtectMode);
		}	
		else if ((strcasecmp(algorithmID,"SM4") == 0) && (symmetricKeyDB.algorithmID != conSymmetricAlgorithmIDOfSM4))
		{
			symmetricKeyDB.algorithmID = conSymmetricAlgorithmIDOfSM4;
			psymmetricKeyValue->oldKeyValue[0] = 0;
			symmetricKeyDB.oldCheckValue[0] = 0;
			strcpy(symmetricKeyDB.keyValue[1].lmkProtectMode,phsmGroupRec->lmkProtectMode);
		}
	}
	// add end

	// »Áπ˚ « π”√æ…√‹‘øµº≥ˆ–¬√‹‘ø£¨…Ë÷√µº≥ˆ√‹‘øµƒ±£ª§√‹‘ø÷µ
	if(exportFlag == 9)
	{
		// ∑«÷˜√‹‘ø≤ª‘ –Ì π”√æ…√‹‘ø ‰≥ˆ
		if (symmetricKeyDB.outputFlag > 0 && symmetricKeyDB.keyType != conTMK && symmetricKeyDB.keyType != conZMK)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: work key not alow to exp by its old value!\n");
			return(errCodeEsscMDL_KeyOutputNotPermitted);
		}

		if(symmetricKeyDB.keyType == conTMK)
		{
			strcpy(tmk, psymmetricKeyValue->oldKeyValue);
		}
		else
		{
			strcpy(zmk, psymmetricKeyValue->oldKeyValue);
		}
	}
	
	// ±æµÿ≤Ÿ◊˜
	if (!isRemoteKeyOperate)
	{
		// modify by leipp 20151209
		// ∂¡»°≤¢ºÏ≤È…Í«Î∫Õ∑÷∑¢∆ΩÃ®
		if ((ret = UnionCheckRemoteKeyPlatform(symmetricKeyDB.keyName, symmetricKeyDB.keyApplyPlatform, symmetricKeyDB.keyDistributePlatform, &tkeyApplyPlatform, &tkeyDistributePlatform, &isRemoteApplyKey, &isRemoteDistributeKey)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionCheckRemoteKeyPlatform!\n");
			return(ret);
		}
		// modify end
	}
	else	// ‘∂≥Ã≤Ÿ◊˜
	{
		isRemoteApplyKey = 0;
		isRemoteDistributeKey = 0;
		// ªÒ»°‘∂≥Ã≤Ÿ◊˜±Í ∂
		if ((ret = UnionReadRequestXMLPackageValue("body/remoteOperateFlag",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionReadRequestXMLPackageValue[%s]!\n","body/remoteOperateFlag");
			return(ret);
		}
		tmpBuf[ret] = 0;

		if (tmpBuf[0] == '1')
			isApplyFlag = 1;
		else 
			isDistributeFlag = 1;
			
		// œµÕ≥ID
		if ((ret = UnionReadRequestXMLPackageValue("head/sysID",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionReadRequestXMLPackageValue[%s]!\n","head/sysID");
			return(ret);
		}
		tmpBuf[ret] = 0;

		//ªÒ»°±£ª§√‹‘ø
		snprintf(sql,sizeof(sql),"select protectKey from localKeyPlatform where sysID = '%s' and algorithmID = %d",tmpBuf,symmetricKeyDB.algorithmID);
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0) 
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionSelectRealDBRecord[%s]\n",sql);
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: remoteProtectKey not found!\n");
			UnionSetResponseRemark("‘∂≥Ã±£ª§√‹‘ø≤ª¥Ê‘⁄");
			return(errCodeParameter);
		}

		if ((ret = UnionLocateXMLPackage("detail", 1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionLocateXMLPackage[%s]!\n","detail");	
			return(ret);
		}

		// ∂¡»°∆ΩÃ®±£ª§√‹‘ø
		if ((ret = UnionReadXMLPackageValue("protectKey",platformProtectKey,sizeof(platformProtectKey))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionReadXMLPackageValue[%s]!\n","protectKey");
			return(ret);
		}
		platformProtectKey[ret] = 0;
	}

	// ‘∂≥Ã…Í«Î√‹‘ø
	if (isRemoteApplyKey)
	{
		// ≥ı ºªØ‘∂≥Ã√‹‘ø≤Ÿ◊˜«Î«Û±®Œƒ
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}

		// …Ë÷√…Í«Î±Í÷æ 1:…Í«Î
		if ((ret = UnionSetRequestRemoteXMLPackageValue("body/remoteOperateFlag","1")) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/remoteOperateFlag","1");
			return(ret);
		}

		// ◊™∑¢√‹‘ø≤Ÿ◊˜µΩ‘∂≥Ã∆ΩÃ®
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyApplyPlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}

	switch(mode)
	{
		case 1:
			if (isRemoteApplyKey)	//‘∂≥Ã…˙≥…£¨±æµÿ¥Ê¥¢
			{
				// ∂¡»°‘∂≥Ã√‹‘ø÷µ
				if((ret = UnionReadResponseRemoteXMLPackageValue("body/keyValue",remoteKeyValue,sizeof(remoteKeyValue))) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE111:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/keyValue");
					return(ret);
				}
				remoteKeyValue[ret] = 0;

				// ∂¡»°‘∂≥Ã√‹‘ø–£—È÷µ
				if((ret = UnionReadResponseRemoteXMLPackageValue("body/checkValue",remoteCheckValue,sizeof(remoteCheckValue))) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE111:: UnionReadResponseRemoteXMLPackageValue[%s]!\n","body/checkValue");
					return(ret);
				}
				remoteCheckValue[ret] = 0;

				switch(symmetricKeyDB.algorithmID)
				{
					case	conSymmetricAlgorithmIDOfDES:	// DESÀ„∑®
						// √‹‘ø◊™≥…±æµÿLMK∂‘º”√‹
						switch(phsmGroupRec->hsmCmdVersionID)
						{
							case conHsmCmdVerRacalStandardHsmCmd:
							case conHsmCmdVerSJL06StandardHsmCmd:
								if ((ret = UnionHsmCmdA6(NULL,symmetricKeyDB.keyType,tkeyApplyPlatform.protectKey,remoteKeyValue,psymmetricKeyValue->keyValue,symmetricKeyDB.checkValue)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE111:: UnionHsmCmdA6!\n");
									return(ret);
								}
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCodeE111:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
								UnionSetResponseRemark("∑«∑®µƒº”√‹ª˙÷∏¡Ó¿‡–Õ");
								return(errCodeParameter);
						}

						// ºÏ≤È–£—È÷µ
						if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(remoteCheckValue)) != 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE111:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
							return(errCodeEssc_CheckValue);
						}

						if (outputByZMK == 1)
						{
							// ∞—±æµÿLMK∂‘º”√‹◊™≥…ZMKº”√‹
							switch(phsmGroupRec->hsmCmdVersionID)
							{
								case	conHsmCmdVerRacalStandardHsmCmd:
								case	conHsmCmdVerSJL06StandardHsmCmd:

									if (((exportFlag == 1) && (zmkKeyDB.keyType == conTMK)) || (exportFlag == 3) || (exportFlag == 9 && symmetricKeyDB.keyType == conTMK))
									{
										// TMK±£ª§ ‰≥ˆ
										if (symmetricKeyDB.keyType == conTMK || symmetricKeyDB.keyType == conTPK || symmetricKeyDB.keyType == conPVK)
										{
											if ((ret = UnionHsmCmdAE(tmk,symmetricKeyDB.keyValue[0].keyValue,keyByTMK,checkValue)) < 0)
											{
												UnionUserErrLog("in UnionDealServiceCodeE111:: UnionHsmCmdAE!\n");
												return(ret);
											}
											strcpy(keyByZMK,keyByTMK);
										}
										else if (symmetricKeyDB.keyType == conTAK)
										{
											if ((ret = UnionHsmCmdAG(tmk,symmetricKeyDB.keyValue[0].keyValue,keyByTMK,checkValue)) < 0)
											{
												UnionUserErrLog("in UnionDealServiceCodeE111:: UnionHsmCmdAG!\n");
												return(ret);
											}
											strcpy(keyByZMK,keyByTMK);
										}
									}
									else
									{
										if ((ret =  UnionHsmCmdA8(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,zmk,keyByZMK,symmetricKeyDB.checkValue)) < 0)
										{
											UnionUserErrLog("in UnionDealServiceCodeE111:: UnionHsmCmdA8!\n");
											return(ret);
										}
									}

									break;
								default:
									UnionUserErrLog("in UnionDealServiceCodeE111:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
									UnionSetResponseRemark("∑«∑®µƒº”√‹ª˙÷∏¡Ó¿‡–Õ");
									return(errCodeParameter);
							}
							// ºÏ≤È–£—È÷µ
							if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(remoteCheckValue)) != 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE111:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
								return(errCodeEssc_CheckValue);
							}
						}
						break;
					case	conSymmetricAlgorithmIDOfSM4:	// SM4À„∑®
						// √‹‘ø◊™≥…±æµÿLMK∂‘º”√‹
						switch(phsmGroupRec->hsmCmdVersionID)
						{
							case conHsmCmdVerRacalStandardHsmCmd:
							case conHsmCmdVerSJL06StandardHsmCmd:
								if ((ret = UnionHsmCmdSV(symmetricKeyDB.keyType,tkeyApplyPlatform.protectKey,remoteKeyValue,psymmetricKeyValue->keyValue,symmetricKeyDB.checkValue)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE111:: UnionHsmCmdSV!\n");
									return(ret);
								}
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCodeE111:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
								UnionSetResponseRemark("∑«∑®µƒº”√‹ª˙÷∏¡Ó¿‡–Õ");
								return(errCodeParameter);
						}
						// ºÏ≤È–£—È÷µ
						if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(remoteCheckValue)) != 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE111:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
							return(errCodeEssc_CheckValue);
						}

						if (outputByZMK == 1)
						{
							// ∞—±æµÿLMK∂‘º”√‹◊™≥…ZMKº”√‹
							switch(phsmGroupRec->hsmCmdVersionID)
							{
								case conHsmCmdVerRacalStandardHsmCmd:
								case conHsmCmdVerSJL06StandardHsmCmd:
									if ((ret =  UnionHsmCmdST(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,zmk,keyByZMK,symmetricKeyDB.checkValue)) < 0)
									{
										UnionUserErrLog("in UnionDealServiceCodeE111:: UnionHsmCmdST!\n");
										return(ret);
									}
									break;
								default:
									UnionUserErrLog("in UnionDealServiceCodeE111:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
									UnionSetResponseRemark("∑«∑®µƒº”√‹ª˙÷∏¡Ó¿‡–Õ");
									return(errCodeParameter);
							}
							// ºÏ≤È–£—È÷µ
							if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(remoteCheckValue)) != 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE111:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
								return(errCodeEssc_CheckValue);
							}
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE111:: symmetricKeyDB.algorithmID[%d] is invalid!\n",symmetricKeyDB.algorithmID);
						return(errCodeEsscMDL_InvalidAlgorithmID);
				}			
			}
			else if (isDistributeFlag) //°°‘∂≥Ã¥Ê¥¢
			{
				// ∂¡»°‘∂≥Ã√‹‘ø÷µ
				if((ret = UnionReadRequestXMLPackageValue("body/keyValue",remoteKeyValue,sizeof(remoteKeyValue))) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE111:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyValue");
					return(ret);
				}
				remoteKeyValue[ret] = 0;

				// ∂¡»°‘∂≥Ã√‹‘ø–£—È÷µ
				if((ret = UnionReadRequestXMLPackageValue("body/checkValue",remoteCheckValue,sizeof(remoteCheckValue))) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE111:: UnionReadRequestXMLPackageValue[%s]!\n","body/checkValue");
					return(ret);
				}
				remoteCheckValue[ret] = 0;

				switch(symmetricKeyDB.algorithmID)
				{
					case	conSymmetricAlgorithmIDOfDES:	// DESÀ„∑®
						switch(phsmGroupRec->hsmCmdVersionID)
						{
							case	conHsmCmdVerRacalStandardHsmCmd:
							case	conHsmCmdVerSJL06StandardHsmCmd:
								if ((ret = UnionHsmCmdA6(NULL,symmetricKeyDB.keyType,platformProtectKey,remoteKeyValue,psymmetricKeyValue->keyValue,symmetricKeyDB.checkValue)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE111:: UnionHsmCmdA6!\n");
									return(ret);
								}
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCodeE111:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
								UnionSetResponseRemark("∑«∑®µƒº”√‹ª˙÷∏¡Ó¿‡–Õ");
								return(errCodeParameter);
						}

						// ºÏ≤È–£—È÷µ
						if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(remoteCheckValue)) != 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE111:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
							return(errCodeEssc_CheckValue);
						}

						break;
					case	conSymmetricAlgorithmIDOfSM4:	// SM4À„∑®
						// √‹‘ø◊™≥…±æµÿLMK∂‘º”√‹
						switch(phsmGroupRec->hsmCmdVersionID)
						{
							case conHsmCmdVerRacalStandardHsmCmd:
							case conHsmCmdVerSJL06StandardHsmCmd:
								if ((ret = UnionHsmCmdSV(symmetricKeyDB.keyType,platformProtectKey,remoteKeyValue,psymmetricKeyValue->keyValue,symmetricKeyDB.checkValue)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE111:: UnionHsmCmdSV!\n");
									return(ret);
								}
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCodeE111:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
								UnionSetResponseRemark("∑«∑®µƒº”√‹ª˙÷∏¡Ó¿‡–Õ");
								return(errCodeParameter);
						}
						// ºÏ≤È–£—È÷µ
						if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(remoteCheckValue)) != 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE111:: symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
							return(errCodeEssc_CheckValue);
						}

						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE111:: Distribute Key symmetricKeyDB.algorithmID[%d] is invalid!\n",symmetricKeyDB.algorithmID);
						return(errCodeEsscMDL_InvalidAlgorithmID);
				}			
			}
			else if(isRemoteApplyKey == 0)		// ±æµÿ…˙≥…
			{
				switch(symmetricKeyDB.algorithmID)
				{
					case	conSymmetricAlgorithmIDOfDES:	// DESÀ„∑®
						if (((exportFlag == 1) && (zmkKeyDB.keyType == conTMK)) || (exportFlag == 3) || (exportFlag == 9 && symmetricKeyDB.keyType == conTMK))
							strcpy(tmpBuf,"0");
						else
							sprintf(tmpBuf,"%d",outputByZMK);

						if ((ret =  UnionHsmCmdA0(atoi(tmpBuf),symmetricKeyDB.keyType,UnionConvertSymmetricKeyKeyLen(symmetricKeyDB.keyLen),zmk,psymmetricKeyValue->keyValue,keyByZMK,symmetricKeyDB.checkValue)) < 0)
						{
							UnionUserErrLog("in UnionDealServiceCodeE111:: UnionHsmCmdA0!\n");
							return(ret);
						}

						if (((exportFlag == 1) && (zmkKeyDB.keyType == conTMK)) || (exportFlag == 3) || (exportFlag == 9))
						{
							// TMK±£ª§ ‰≥ˆ
							if (symmetricKeyDB.keyType == conTMK || symmetricKeyDB.keyType == conTPK || symmetricKeyDB.keyType == conPVK)
							{
								if ((ret = UnionHsmCmdAE(tmk,symmetricKeyDB.keyValue[0].keyValue,keyByTMK,checkValue)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE111:: UnionHsmCmdAE!\n");
									return(ret);
								}
								strcpy(keyByZMK,keyByTMK);
							}
							else if (symmetricKeyDB.keyType == conTAK)
							{
								if ((ret = UnionHsmCmdAG(tmk,symmetricKeyDB.keyValue[0].keyValue,keyByTMK,checkValue)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE111:: UnionHsmCmdAG!\n");
									return(ret);
								}
								strcpy(keyByZMK,keyByTMK);
							}
						}
		
						if (isApplyFlag) // ‘∂≥Ã∑÷∑¢
						{
							// ∞—±æµÿLMK∂‘º”√‹◊™≥…‘∂≥ÃZMKº”√‹
							switch(phsmGroupRec->hsmCmdVersionID)
							{
								case conHsmCmdVerRacalStandardHsmCmd:
								case conHsmCmdVerSJL06StandardHsmCmd:
									if ((ret =  UnionHsmCmdA8(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,platformProtectKey,keyByZMK,remoteCheckValue)) < 0)
									{
										UnionUserErrLog("in UnionDealServiceCodeE111:: Distribute Key UnionHsmCmdA8!\n");
										return(ret);
									}
									break;
								default:
									UnionUserErrLog("in UnionDealServiceCodeE111:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
									UnionSetResponseRemark("∑«∑®µƒº”√‹ª˙÷∏¡Ó¿‡–Õ");
									return(errCodeParameter);
							}
							// ºÏ≤È–£—È÷µ
							if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(symmetricKeyDB.checkValue)) != 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE111:: Distribute Key symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
								return(errCodeEssc_CheckValue);
							}					
						}
						break;
					case	conSymmetricAlgorithmIDOfSM4:	// SM4À„∑®
						snprintf(tmpBuf,sizeof(tmpBuf),"%d",outputByZMK);
						switch(phsmGroupRec->hsmCmdVersionID)
						{
							case conHsmCmdVerRacalStandardHsmCmd:
							case conHsmCmdVerSJL06StandardHsmCmd:
								if ((ret = UnionHsmCmdWI(tmpBuf,symmetricKeyDB.keyType,zmk,psymmetricKeyValue->keyValue,keyByZMK,symmetricKeyDB.checkValue)) < 0)
								{
									UnionUserErrLog("in UnionDealServiceCodeE111:: UnionHsmCmdWI!\n");
									return(ret);
								}
								break;
							default:
								UnionUserErrLog("in UnionDealServiceCodeE111:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
								UnionSetResponseRemark("∑«∑®µƒº”√‹ª˙÷∏¡Ó¿‡–Õ");
								return(errCodeParameter);
						}
						if (isApplyFlag) // ‘∂≥Ã∑÷∑¢
						{
							
							// ∞—±æµÿLMK∂‘º”√‹◊™≥…‘∂≥ÃZMKº”√‹
							switch(phsmGroupRec->hsmCmdVersionID)
							{
								case	conHsmCmdVerRacalStandardHsmCmd:
								case	conHsmCmdVerSJL06StandardHsmCmd:
									if ((ret =  UnionHsmCmdST(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,platformProtectKey,keyByZMK,remoteCheckValue)) < 0)
									{
										UnionUserErrLog("in UnionDealServiceCodeE111:: Distribute Key UnionHsmCmdST!\n");
										return(ret);
									}
									break;
								default:
									UnionUserErrLog("in UnionDealServiceCodeE111:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
									UnionSetResponseRemark("∑«∑®µƒº”√‹ª˙÷∏¡Ó¿‡–Õ");
									return(errCodeParameter);
							}
							// ºÏ≤È–£—È÷µ
							if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(symmetricKeyDB.checkValue)) != 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE111:: Distribute Key symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
								return(errCodeEssc_CheckValue);
							}					
							break;
						}
						break;
					default:
						UnionUserErrLog("in UnionDealServiceCodeE111:: Distribute Key symmetricKeyDB.algorithmID[%d] is invalid!\n",symmetricKeyDB.algorithmID);
						return(errCodeEsscMDL_InvalidAlgorithmID);
				}
			}
			break;
		case 2:
		default:
			UnionUserErrLog("in UnionDealServiceCodeE111:: mode = [%d]!\n",mode);
			return(errCodeEsscMDLKeyOperationNotPermitted);
	}

	if (isRemoteDistributeKey)	// ‘∂≥Ã∑÷∑¢√‹‘ø
	{
		// ≥ı ºªØ‘∂≥Ã√‹‘ø≤Ÿ◊˜«Î«Û±®Œƒ
		if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
			return(ret);
		}

		// …Ë÷√…Í«Î±Í÷æ 2:∑÷∑¢
		if ((ret = UnionSetRequestRemoteXMLPackageValue("body/remoteOperateFlag","2")) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/remoteOperateFlag","2");
			return(ret);
		}
		if (mode == 1)	// …˙≥…√‹‘ø
		{
			// ‘ˆº”√‹‘ø∫Õ–£—È÷µ
			switch(symmetricKeyDB.algorithmID)
			{
				case	conSymmetricAlgorithmIDOfDES:	// DESÀ„∑®
					// ∞—±æµÿLMK∂‘º”√‹◊™≥…‘∂≥ÃZMKº”√‹
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret =  UnionHsmCmdA8(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,tkeyDistributePlatform.protectKey,keyByRKM,remoteCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE111:: Distribute Key UnionHsmCmdA8!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE111:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("∑«∑®µƒº”√‹ª˙÷∏¡Ó¿‡–Õ");
							return(errCodeParameter);
					}
					// ºÏ≤È–£—È÷µ
					if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(symmetricKeyDB.checkValue)) != 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE111:: Distribute Key symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
						return(errCodeEssc_CheckValue);
					}					
					break;
				case	conSymmetricAlgorithmIDOfSM4:	// SM4À„∑®
					// ∞—±æµÿLMK∂‘º”√‹◊™≥…‘∂≥ÃZMKº”√‹
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case conHsmCmdVerRacalStandardHsmCmd:
						case conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret =  UnionHsmCmdST(symmetricKeyDB.keyType,psymmetricKeyValue->keyValue,tkeyDistributePlatform.protectKey,keyByRKM,remoteCheckValue)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE111:: Distribute Key UnionHsmCmdST!\n");
								return(ret);
							}
							break;
						default:
							UnionUserErrLog("in UnionDealServiceCodeE111:: phsmGroupRec->hsmCmdVersionID[%d] is invalid\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("∑«∑®µƒº”√‹ª˙÷∏¡Ó¿‡–Õ");
							return(errCodeParameter);
					}
					// ºÏ≤È–£—È÷µ
					if (memcmp(symmetricKeyDB.checkValue,remoteCheckValue,strlen(symmetricKeyDB.checkValue)) != 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE111:: Distribute Key symmetricKeyDB.checkValue[%s] != remoteCheckValue[%s]\n",symmetricKeyDB.checkValue,remoteCheckValue);
						return(errCodeEssc_CheckValue);
					}					
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE111:: Distribute Key symmetricKeyDB.algorithmID[%d] is invalid!\n",symmetricKeyDB.algorithmID);
					return(errCodeEsscMDL_InvalidAlgorithmID);
			}			
			if ((ret = UnionSetRequestRemoteXMLPackageValue("body/keyValue",keyByRKM)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE111:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/keyValue",keyByRKM);
				return(ret);
			}
			if ((ret = UnionSetRequestRemoteXMLPackageValue("body/checkValue",remoteCheckValue)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE111:: UnionSetRequestRemoteXMLPackageValue[%s][%s]!\n","body/checkValue",remoteCheckValue);
				return(ret);
			}
		}
		// ◊™∑¢√‹‘ø≤Ÿ◊˜µΩ‘∂≥Ã∆ΩÃ®
		if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(&tkeyDistributePlatform)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
			return(ret);
		}
	}
	//add by zhouxw 	20150520
	if (isRemoteKeyOperate)         // ‘∂≥Ã√‹‘ø≤Ÿ◊˜
        {
                if ((ret = UnionReadRemoteSysIDAndAppID(sysID,sizeof(sysID),appID,sizeof(appID))) < 0)
                {       
                        UnionUserErrLog("in UnionDealServiceCodeE111:: UnionReadRemoteSysIDAndAppID!\n");
                        return(ret);
                }
        }
        else                            // ±æµÿ√‹‘ø≤Ÿ◊˜
        {       
                if ((ret = UnionReadRequestXMLPackageValue("head/sysID",sysID,sizeof(sysID))) < 0)
                {       
                        UnionUserErrLog("in UnionDealServiceCodeE119:: UnionReadRequestXMLPackageValue[%s]!\n","head/sysID");            
                        return(ret);
                }
                if ((ret = UnionReadRequestXMLPackageValue("head/appID",appID,sizeof(appID))) < 0)
                {       
                        UnionUserErrLog("in UnionDealServiceCodeE119:: UnionReadRequestXMLPackageValue[%s]!\n","head/appID");            
                        return(ret);
                }
        }
	
	
	// …Ë÷√¥¥Ω®’ﬂ
        UnionPieceSymmetricKeyCreator(sysID,appID,creator);
	
	//if(((symmetricKeyDB.creatorType == conSymmetricCreatorTypeOfApp) && ((strcmp(symmetricKeyDB.creator,creator) == 0) || (strcmp(symmetricKeyDB.creator,"APP") == 0))) || (symmetricKeyDB.creatorType == conSymmetricCreatorTypeOfUser) || (symmetricKeyDB.creatorType == conSymmetricCreatorTypeOfKMS) || (symmetricKeyDB.creatorType == conSymmetricCreatorTypeOfMove))

	if(((symmetricKeyDB.creatorType == conSymmetricCreatorTypeOfApp) && (strcmp(symmetricKeyDB.creator,creator) == 0)) || (symmetricKeyDB.creatorType == conSymmetricCreatorTypeOfUser) || (symmetricKeyDB.creatorType == conSymmetricCreatorTypeOfKMS) || (symmetricKeyDB.creatorType == conSymmetricCreatorTypeOfMove) || (strcmp(symmetricKeyDB.creator,"APP") == 0))
	{
	//add end 20150520
		if ((ret =  UnionUpdateSymmetricKeyDBKeyValue(&symmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionUpdateSymmetricKeyDBKeyValue,keyName[%s]!\n",symmetricKeyDB.keyName);
			return(ret);
		}
	//add by zhouxw 20150520
	}
	else
	{
		UnionSetResponseRemark("√‹‘ø[%s]∑«¥À¥¥Ω®’ﬂ[%s]¥¥Ω®,≤ª‘ –Ì∏¸–¬√‹‘ø!",keyName,creator);
                UnionUserErrLog("in UnionDealServiceCodeE111:: √‹‘ø[%s]∑«¥À¥¥Ω®’ﬂ[%s]¥¥Ω®,≤ª‘ –Ì∏¸–¬√‹‘ø§!",keyName,creator);
		return(errCodeEsscMDLKeyOperationNotPermitted);
	}
	//add end 20150520

	// modify by leipp 20160125	–ﬁ∏¥œÚ‘∂≥Ã∑÷∑¢√‹‘ø≤ª–Ë“™∑µªÿ√‹‘ø”Î–£—È÷µ,‘ˆº”[isRemoteKeyOperate == 0]
	if ((exportFlag != 0 && isRemoteKeyOperate == 0) || (isApplyFlag == 1))
	{
		if ((ret = UnionSetResponseXMLPackageValue("body/keyValue",keyByZMK)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionSetResponseXMLPackageValue[%s]!\n","body/keyValue");
			return(ret);
		}
		if ((ret = UnionSetResponseXMLPackageValue("body/checkValue",symmetricKeyDB.checkValue)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionSetResponseXMLPackageValue[%s]!\n","body/checkValue");
			return(ret);
		}
	}
	// modify end
	if (exportLmkKey[0] == '1' && !isRemoteKeyOperate)
	{
		if ((ret = UnionSetResponseXMLPackageValue("body/keyValue2",psymmetricKeyValue->keyValue)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionSetResponseXMLPackageValue[%s]!\n","body/keyValue2");
			return(ret);
		}

		if ((ret = UnionSetResponseXMLPackageValue("body/checkValue",symmetricKeyDB.checkValue)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE111:: UnionSetResponseXMLPackageValue[%s]!\n","body/checkValue");
			return(ret);
		}
	}
	
	return 0;
}

