//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2012-05-25

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#include "UnionLog.h"
#include "UnionStr.h"
#include "UnionTask.h"
#include "unionErrCode.h"
#include "unionREC.h"
#include "UnionSHA1.h"
#include "sm3.h"

#include "unionXMLPackage.h"
#include "unionFunSvrName.h"
#include "errCodeTranslater.h"
#include "unionRealBaseDB.h"
#include "commWithHsmSvr.h"
#include "registerRunningTime.h"
#include "packageConvert.h"
#include "accessCheck.h"
#include "unionMsgBufGroup.h"
#include "unionCommBetweenMDL.h"
#include "unionDataSync.h"

int UnionDealServiceTask(char *serviceCode,PUnionHsmGroupRec phsmGroupRec);

int UnionSendMonInfoToTransMonMDL(unsigned char *msg,int lenOfMsg)
{
	int	ret;
	TUnionMessageHeader	reqMsgHeader;

	//modify by liwj 20150412
	if (UnionExistsTaskOfName("transMonSvr") <= 0 && UnionExistsTaskOfName("resSpier -TRANSDETAIL") <= 0)
	{
		//UnionLog("in UnionSendYLPackageToTransMonSvrMDL:: task ylqzTransMon not started!\n");
		UnionLog("in UnionSendYLPackageToTransMonSvrMDL:: task transMonSvr or resSpierForTaskTbl not started!\n");
		return(0);
	}
	//modify end

	memset(&reqMsgHeader,0,sizeof(reqMsgHeader));
	if ((ret = UnionSendRequestToSpecifiedModuleWithNewMsgHeaderWithTime(conMDLTypeOffsetOfMonSvr + conMDLTypeUnionMonSvr,msg,lenOfMsg,&reqMsgHeader,1)) < 0)
	{
		UnionLog("in UnionSendMonInfoToTransMonMDL:: UnionSendRequestToSpecifiedModuleWithNewMsgHeader!\n");
		return(0);
	}

	return(0);
}

int UnionUserSpecFunForFunSvr()
{
	return(0);
}

int UnionFunSvrInterprotor(char *cliIPAddr,int port,unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfResStr)
{
	int	isUI;
	int	timeout;
	int     ret = 0;
	int	len;
	// char	date[32];
	char    errRemark[128];
	char    tmpBuf[512];
	// char	sql[512];
	unsigned char	hash[80];
	char	tmpData[64];
	int	isSM3 = 0;
	// char	tmpNum[32];
	// char	unitID[32];
	long	useTime = 0;
	char	strUseTime[32];
	char	hsmGroupID[32];
	char	sql[128];
	unsigned char	transMonBuf[1024];
	TUnionHsmGroupRec	hsmGroupRec;
	PUnionXMLPackageHead	pxmlPackageHead = NULL;
	
	if ((reqStr == NULL) || (resStr == NULL))
	{
		UnionUserErrLog("in UnionFunSvrInterprotor:: Null Pointer or lenOfReqStr = [%d] sizeOfBuf = [%d]!\n",lenOfReqStr,sizeOfResStr);
		return(errCodeParameter);
	}
	
	// ��������ת��ΪXML����
	if ((ret = UnionConvertOtherPackageToXMLPackage(reqStr,lenOfReqStr,cliIPAddr)) < 0)
	{
		if (ret == errCodeSJL05MDL_HsmCmdFormatError)
		{
			UnionUserErrLog("in UnionFunSvrInterprotor:: SJL05 hsm cmd format error!\n");
			resStr[0] = 'E';
			resStr[1] = 0xFF;
			return(2);
		}
		UnionUserErrLog("in UnionFunSvrInterprotor:: UnionConvertOtherPackageToXMLPackage!\n");
		//return(errCodeEsscMDLReqStr);
		// modify by leipp 20151028
		if (ret == errCodeEsscMDL_NotEsscRequestPackage)
			return(errCodeEsscMDLReqStr);
		else
			goto packNow;
		// modify by leipp 20151028 end
	}
	else if (ret == 0)
	{
		// ��ʼ�������
		if ((ret = UnionInitRequestXMLPackage(NULL,(char *)reqStr,lenOfReqStr)) < 0)
		{
			UnionUserErrLog("in UnionFunSvrInterprotor:: UnionInitRequestXMLPackage!\n");
			return(errCodeEsscMDLReqStr);
		}
	}

	// ��ʼ��XML��Ӧ��
	if ((ret = UnionInitHeadOfResponseXMLPackage()) < 0)
	{
		UnionUserErrLog("in UnionFunSvrInterprotor:: UnionInitHeadOfResponseXMLPackage!\n");
		return(errCodeEsscMDLReqStr);
	}
	
	pxmlPackageHead = UnionGetXMLPackageHead();	
	if (pxmlPackageHead->isUI)
		UnionSetDataSyncIsWaitResult();
	else
		UnionSetDataSyncIsNotWaitResult();

	if (pxmlPackageHead->isUI)
		UnionLog("\n\n************ New of Transcation[%s %s] ************\n\n",pxmlPackageHead->userID,pxmlPackageHead->serviceCode);
	else
		UnionLog("\n\n************ New of Transcation[%s:%s %s] ************\n\n",pxmlPackageHead->sysID,pxmlPackageHead->appID,pxmlPackageHead->serviceCode);

	// ���н�����
	if ((ret = UnionAccessCheck(cliIPAddr,port,&timeout,&isUI)) < 0)
	{
		UnionUserErrLog("in UnionFunSvrInterprotor:: UnionAccessCheck!\n");
		goto packNow;
	}
	
	// ��¼��־
	UnionLogRequestXMLPackage();

	if (pxmlPackageHead->isUI || ((UnionReadIntTypeRECVar("isCheckHashOfPackage") > 0) && (strcmp(pxmlPackageHead->appID,"RKM") != 0)))
	{
		if ((len = UnionSelectRequestXMLPackageValue("body",(char *)resStr,sizeOfResStr)) > 0)
		{
			// modify by leipp 20150706
			// ����HASH
			/*memset(tmpBuf,0,sizeof(tmpBuf));
			UnionSHA1(resStr,len,(unsigned char *)tmpBuf);
			
			ret = bcdhex_to_aschex(tmpBuf,20,(char *)hash);
			hash[ret] = 0;*/
	
			if ((ret = UnionReadRequestXMLPackageValue("head/hash",tmpBuf,sizeof(tmpBuf))) < 0)
			{
				UnionUserErrLog("in UnionFunSvrInterprotor:: UnionReadRequestXMLPackageValue[%s]!\n","head/hash");
				goto packNow;
			}
			tmpBuf[ret] = 0;
			
			if (ret == 64)
			{
				sm3(resStr,len,(unsigned char *)tmpData);
				ret = bcdhex_to_aschex(tmpData,32,(char *)hash);
				hash[ret] = 0;
				isSM3 = 1;
			}
			else
			{
				UnionSHA1(resStr,len,(unsigned char *)tmpData);
				ret = bcdhex_to_aschex(tmpData,20,(char *)hash);
				hash[ret] = 0;

			}
			// modify end

			if (strcmp((char *)hash,tmpBuf) != 0)
			{
				UnionUserErrLog("in UnionFunSvrInterprotor:: local hash[%s] != remote hash[%s],len[%d] hashData[%s]\n",hash,tmpBuf,len,resStr);
				ret = errCodeHashValueErr;
				goto packNow;
			}
		}
	}

	if (pxmlPackageHead->isUI)
	{
		UnionSetIsNotUseHighCached();
		
		// ����û���¼״̬
		if ((ret = UnionCheckLoginFlagOfUser(pxmlPackageHead->sysID,pxmlPackageHead->userID,pxmlPackageHead->serviceCode)) < 0)
		{
			UnionUserErrLog("in UnionFunSvrInterprotor:: UnionCheckLoginFlagOfUser[%d]!\n",ret);
			goto packNow;
		}
			
		// ����Ĭ���������
		if ((ret = UnionGetHsmGroupRecByHsmGroupID("default",&hsmGroupRec)) < 0)
		{
			if (ret != errCodeEsscMDL_EsscHsmGrpIDNotDefined)
			{
				UnionUserErrLog("in UnionFunSvrInterprotor:: UnionGetHsmGroupRecByHsmGroupID hsmGroupID[%s]!\n","default");
				goto packNow;
			}
			else
			{
				// add by leipp 20151015
				len = sprintf(sql,"select hsmGroupID from hsmGroup where enabled = 1");
				sql[len] = 0;
				if ((ret = UnionSelectRealDBRecord(sql,1,1)) < 0)
				{
					UnionUserErrLog("in UnionFunSvrInterprotor:: UnionSelectRealDBRecord sql[%s]!\n",sql);
					goto packNow;
				}
				else if (ret > 0)
				{
					if ((ret = UnionLocateXMLPackage("detail",1)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCode0H03:: UnionLocateXMLPackage [%s]!\n","detail");
						goto packNow;
					}

					memset(hsmGroupID,0,sizeof(hsmGroupID));
					if ((ret = UnionReadXMLPackageValue("hsmGroupID",hsmGroupID,sizeof(hsmGroupID))) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCode0H03:: UnionReadXMLPackageValue[%s]!\n","hsmGroupID");
						goto packNow;
					}
					
					if ((ret = UnionGetHsmGroupRecByHsmGroupID(hsmGroupID,&hsmGroupRec)) < 0)
					{
						if (ret != errCodeEsscMDL_EsscHsmGrpIDNotDefined)
						{
							UnionUserErrLog("in UnionFunSvrInterprotor:: UnionGetHsmGroupRecByHsmGroupID hsmGroupID[%s]!\n",hsmGroupID);
							goto packNow;
						}
					}
				}
				// add end 20151015
			}
		}
	}
	else
	{
		UnionSetIsUseHighCached();
		
		// ����Ӧ�ñ�	
		if ((ret = UnionReadHsmGroupIDByAppID(pxmlPackageHead->appID,&hsmGroupRec)) < 0)
		{
			UnionUserErrLog("in UnionFunSvrInterprotor:: UnionReadHsmGroupIDByAppID[%s]!\n",pxmlPackageHead->appID);
			goto packNow;
		}
	}
//privateHsm:
	UnionSetHsmGroupIDForHsmSvr(hsmGroupRec.hsmGroupID);
		
	// ������
	if ((ret = UnionDealServiceTask(pxmlPackageHead->serviceCode,&hsmGroupRec)) < 0)
	{
		UnionUserErrLog("in UnionFunSvrInterprotor:: UnionDealServiceTask[%d]!\n",ret);
		goto packNow;
	}

packNow:
	// ����XML��λ
	UnionLocateResponseXMLPackage("",0);

	useTime = UnionGetTotalRunningTimeInMacroSeconds();
	snprintf(strUseTime,sizeof(strUseTime),"%ld",useTime);
	UnionSetResponseXMLPackageValue("head/useTime",strUseTime);

	if (ret >= 0)
	{
		if (pxmlPackageHead->isUI || (UnionReadIntTypeRECVar("isCheckHashOfPackage") > 0))
		{
			if ((len = UnionSelectResponseXMLPackageValue("body",(char *)resStr,sizeOfResStr)) > 0)
			{
				// ����HASH
				if (!isSM3)
				{
					memset(tmpBuf,0,sizeof(tmpBuf));
					UnionSHA1(resStr,len,(unsigned char *)tmpBuf);
					ret = bcdhex_to_aschex(tmpBuf,20,(char *)hash);
					hash[ret] = 0;
				}
				else
				{
					sm3(resStr,len,(unsigned char *)tmpData);
					ret = bcdhex_to_aschex(tmpData,32,(char *)hash);
					hash[ret] = 0;
				}
				
				UnionSetResponseXMLPackageValue("head/hash",(char *)hash);
			}
			else if (len <= 0) // ���body������,������hashΪ��
				UnionSetResponseXMLPackageValue("head/hash","");
		}
	}
	
	// ������Ӧ����
	if (ret < 0)    // ִ�д���
	{

		// ������Ӧ��
		snprintf(tmpBuf,sizeof(tmpBuf),"%06d",abs(ret));
		UnionSetResponseXMLPackageValue("head/responseCode",tmpBuf);

		if ((len = UnionGetResponseRemark(errRemark,sizeof(errRemark))) <= 0)
		{
			if ((len = UnionTranslateErrCodeIntoRemark(ret,errRemark,sizeof(errRemark))) > 0)       // ת��������
			{
				errRemark[len] = 0;
				UnionSetResponseXMLPackageValue("head/responseRemark",errRemark);
			}
			else
				UnionUserErrLog("in UnionFunSvrInterprotor:: UnionTranslateErrCodeIntoRemark ret = [%d]!\n",len);
		}
		else
		{
			errRemark[len] = 0;
			UnionSetResponseXMLPackageValue("head/responseRemark",errRemark);
		}
	}
	else
	{
		UnionSetResponseRemark("");
		UnionSetResponseXMLPackageValue("head/responseCode","000000");
		UnionSetResponseXMLPackageValue("head/responseRemark","�ɹ�");	
	}	

	// ��¼��־
	UnionLogResponseXMLPackage();

	// XML����ת��Ϊ��������
	if ((ret = UnionConvertXMLPackageToOtherPackage(resStr,sizeOfResStr)) < 0)
	{
		UnionUserErrLog("in UnionFunSvrInterprotor:: UnionConvertXMLPackageToOtherPackage! ret = [%d]\n",ret);
		return(ret);
	}
	else if (ret > 0)
		len = ret;
	else
		len = UnionResponseXMLPackageToBuf((char *)resStr, sizeOfResStr);		

	resStr[len] = 0;

	// ���ͼ����Ϣ,2015/1/12����
	///*
	
	if (pxmlPackageHead != NULL) // modify by leipp 20151028 ,���Ӷ�pxmlPackageHead�жϿ�
	{
		if (!pxmlPackageHead->isUI) 
		{
			if (pxmlPackageHead->serviceCode[0] != 'K') // modify by lizh 20160620 KMS����Ҫ���ͼ����Ϣ
			{
				// ɾ��body�ڵ�
				UnionDeleteResponseXMLPackageNode("body",0);
				if ((ret = UnionResponseXMLPackageToBuf((char *)transMonBuf,sizeof(transMonBuf))) > 0)
				{
					transMonBuf[ret] = 0;
					
					// ������Ϣ����
					UnionSetGroupIDOfMsgBuf(defMsgBufGroupOfMon);
					// ���ͼ����Ϣ
					UnionSendMonInfoToTransMonMDL(transMonBuf,ret);
					// �ָ���Ϣ����
					UnionSetGroupIDOfMsgBuf(defMsgBufGroupOfAPP);
				}
			}
		}
	//	*/

		if (pxmlPackageHead->isUI)
			UnionLog("\n\n************ End of Transcation[%s %s] ************\n\n",pxmlPackageHead->userID,pxmlPackageHead->serviceCode);
		else
			UnionLog("\n\n************ End of Transcation[%s:%s %s] ************\n\n",pxmlPackageHead->sysID,pxmlPackageHead->appID,pxmlPackageHead->serviceCode);		
	}
	
	return(len);
}
