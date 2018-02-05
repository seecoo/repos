//	Author:		lusj
//	Copyright:	Union Tech. Guangzhou
//	Date:		2016-02-04

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <iconv.h>

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include "unionREC.h"
#include "UnionStr.h"
#include "UnionESBLog.h"
#include "UnionLog.h"
#include "unionErrCode.h"

#include "esscPackage.h"
#include "unionRealBaseDB.h"
#include "unionXMLPackage.h"
#include "symmetricKeyDB.h"
#include "unionTableData.h"
#include "accessCheck.h"
#include "commWithHsmSvr.h"

//add bu lusj 20160128
#include "esb_front_internal.h"
#include "esb_const.h"

#include "baseUIService.h"
#include "registerAuditAndApproval.h"

#define    	MAX_SOCK_BUF_SIZE     		64*1024
#define		defMaxESBSONum				1024
#define		defMaxESBFunHandleNum		2048


int				gunionIsInitInterChangeFunHandle = 0;
int				gunionIsInitESBSOHandle = 0;
int				gunionIsNotConvertPackage = 1;
int				gunionIsUnionPayPackage = 0;

unsigned long	FunHandle_NO,SOHandle_NO;



typedef int funSetRequestXMLPackage(struct stNode *);
typedef int funSetResponseXMLPackage(struct stNode *);

typedef struct
{
	unsigned long	id;
	char		serviceCode[64];
	funSetRequestXMLPackage		*pfunSetRequestXMLPackage;
	funSetResponseXMLPackage	*pfunSetResponseXMLPackage;
} TUnionInterChangeFunHandle;
TUnionInterChangeFunHandle	gunionInterChangeFunHandle[defMaxESBFunHandleNum];


typedef struct
{
	unsigned long	id;
	char		dlName[64];
	void		*handle;
}TUnionESBSOHandle;
TUnionESBSOHandle			gunionESBSOHandle[defMaxESBSONum];


char	gunionESB_AppID[32]={0};					//�û�ID
char	gunionESB_ServiceCode[32]={0};			//ESB������
char	gunionESB_ServiceScene[8]={0};			//�����볡��
char	gunionESB_ServiceVersion[8]={0};			//����汾
char	gunionESB_CONSUMER_SEQ_NO[64]={0};		//������ˮ��
char	gunionESB_DEST_BRANCH_NO[32]={0};		//���к�
char	gunionESB_SOURCE_BRANCH_NO[32]={0};		//���к�
char	gunionESB_BRANCH_ID[32]={0};				//����ID

char    head_buf[2048]={0};    					//���Խ׶�ͷд������
//add end by lusj 

int UnionIsCheckUnionPayPackage()
{
	return gunionIsUnionPayPackage;
}

int getSYS_HEADFromESBnode(struct stNode *inNode)
{
	int		ret;


	//������
	if((ret = getValueFromTreeLen(inNode,"SYS_HEAD.SERVICE_CODE",gunionESB_ServiceCode,32)) < 0)
	{
		UnionUserErrLog("in getSYS_HEADFromESBnode:: getValueFromTreeLen::SYS_HEAD.SERVICE_CODE!\n");
		//	return(ret);
	}


	//������񳡾�
	if((ret = getValueFromTreeLen(inNode,"SYS_HEAD.SERVICE_SCENE",gunionESB_ServiceScene,8)) < 0)
	{
		UnionUserErrLog("in getSYS_HEADFromESBnode:: getValueFromTreeLen::SYS_HEAD.SERVICE_SCENE!\n");
		//	return(ret);
	}

	//������汾
	if((ret = getValueFromTreeLen(inNode,"SYS_HEAD.SERVICE_VERSION",gunionESB_ServiceVersion,8)) < 0)
	{
		UnionUserErrLog("in getSYS_HEADFromESBnode:: getValueFromTreeLen::SYS_HEAD.SERVICE_VERSION!\n");
		//	return(ret);
	}

	//����ϵͳ���
	if((ret = getValueFromTreeLen(inNode,"SYS_HEAD.CONSUMER_ID",gunionESB_AppID,32)) < 0)	//���أ�1 - �ɹ�  -1 - ʧ��
	{
		UnionUserErrLog("in getSYS_HEADFromESBnode:: getValueFromTreeLen::SYS_HEAD.CONSUMER_ID!\n");
		//	return(ret);
	}
	
	//���ͷ�������(���к�)
	if((ret = getValueFromTreeLen(inNode,"SYS_HEAD.BRANCH_ID",gunionESB_BRANCH_ID,32)) < 0)
	{
		UnionLog("in getSYS_HEADFromESBnode:: getValueFromTreeLen::SYS_HEAD.BRANCH_ID!\n");
		//	return(ret);
	}

	//Դ�ڵ���(������)
	if((ret = getValueFromTreeLen(inNode,"SYS_HEAD.SOURCE_BRANCH_NO",gunionESB_SOURCE_BRANCH_NO,32)) < 0)
	{
		UnionLog("in getSYS_HEADFromESBnode:: getValueFromTreeLen::SYS_HEAD.SOURCE_BRANCH_NO!\n");
		//	return(ret);
	}

	//������ˮ��
	if((ret = getValueFromTreeLen(inNode,"SYS_HEAD.CONSUMER_SEQ_NO",gunionESB_CONSUMER_SEQ_NO,64)) < 0)
	{
		UnionLog("in getSYS_HEADFromESBnode:: getValueFromTreeLen::SYS_HEAD.CONSUMER_SEQ_NO!\n");
		//	return(ret);
	}



	return(1);

}
int setSYS_HEADToESBnode(struct stNode *outNode,char *responseCode,char *responseRemark,int lenOfresponseCod,int lenOfresponseRemark)
{
	struct stNode * childNode;
	struct stNode *temp, *sysNode;
	char	Time[16];
	char	TRAN_DATE[8+1];
	char	TRAN_TIMESTAMP[9+1];


	memcpy(Time,UnionGetCurrentFullSystemDateTime(),14);
	Time[14]=0;

	memcpy(TRAN_DATE,Time,8);//��������
	TRAN_DATE[8]=0;
	memcpy(TRAN_TIMESTAMP,Time+8,6);//����ʱ��
	TRAN_TIMESTAMP[6]=0;	


	//���׷�����
	if((childNode = addFieldToNode(outNode,"SYS_HEAD","SERVICE_CODE","string",strlen(gunionESB_ServiceCode),0,gunionESB_ServiceCode)) == NULL)//�����ڵ����addFieldToNode�������ڵ����Ȼ�ȡ�����ڵ㣬���������ڵ����fieldֵ������ֱ�ӵ��øú���
	{
		UnionUserErrLog("in setSYS_HEADToESBnode:: addFieldToNode::[SYS_HEAD.RET_STATUS] err!\n");
		//	return(errCodePackageDefMDL_FldNotDefined);
	}

	//������񳡾�
	if((childNode = addFieldToNode(outNode,"SYS_HEAD","SERVICE_SCENE","string",strlen(gunionESB_ServiceScene),0,gunionESB_ServiceScene)) == NULL)//�����ڵ����addFieldToNode�������ڵ����Ȼ�ȡ�����ڵ㣬���������ڵ����fieldֵ������ֱ�ӵ��øú���
	{
		UnionUserErrLog("in setSYS_HEADToESBnode:: addFieldToNode::[SYS_HEAD.RET_STATUS] err!\n");
		//	return(errCodePackageDefMDL_FldNotDefined);
	}

	//������汾
	if((childNode = addFieldToNode(outNode,"SYS_HEAD","SERVICE_VERSION","string",strlen(gunionESB_ServiceVersion),0,gunionESB_ServiceVersion)) == NULL)//�����ڵ����addFieldToNode�������ڵ����Ȼ�ȡ�����ڵ㣬���������ڵ����fieldֵ������ֱ�ӵ��øú���
	{
		UnionUserErrLog("in setSYS_HEADToESBnode:: addFieldToNode::[SYS_HEAD.RET_STATUS] err!\n");
		//	return(errCodePackageDefMDL_FldNotDefined);
	}

	sysNode = getNodeFromTree(outNode,"SYS_HEAD");
	temp = addChildToNode(sysNode,"RET",2,NULL);

	temp = addChildToNode(temp,"RET",1,NULL);


	
	//����״̬
	if((childNode = addFieldToNode(outNode,"SYS_HEAD","RET_STATUS","string",1,0,"S")) == NULL)//�����ڵ����addFieldToNode�������ڵ����Ȼ�ȡ�����ڵ㣬���������ڵ����fieldֵ������ֱ�ӵ��øú���
	{
		UnionUserErrLog("in setSYS_HEADToESBnode:: addFieldToNode::[SYS_HEAD.RET_STATUS] err!\n");
		//	return(errCodePackageDefMDL_FldNotDefined);
	}

	//���׷��ش���
	if((childNode = addFieldToNode(temp,NULL,"RET_CODE","string",strlen(responseCode),0,responseCode)) == NULL)
	{
		UnionUserErrLog("in setSYS_HEADToESBnode:: addFieldToNode::[SYS_HEAD.RET_CODE] err!\n");
		//	return(errCodePackageDefMDL_FldNotDefined);
	}

	//���׷�����Ϣ
	if((childNode = addFieldToNode(temp,NULL,"RET_MSG","string",strlen(responseRemark),0,responseRemark)) == NULL)
	{
		UnionUserErrLog("in setSYS_HEADToESBnode:: addFieldToNode::[SYS_HEAD.RET_MSG] err!\n");
		//	return(errCodePackageDefMDL_FldNotDefined);
	}
	

	//��������
	if((childNode = addFieldToNode(outNode,"SYS_HEAD","TRAN_DATE","string",8,0,TRAN_DATE)) == NULL)
	{
		UnionUserErrLog("in setSYS_HEADToESBnode:: addFieldToNode::[SYS_HEAD.TRAN_DATE] err!\n");
		//	return(errCodePackageDefMDL_FldNotDefined);
	}

	//����ʱ��
	if((childNode = addFieldToNode(outNode,"SYS_HEAD","TRAN_TIMESTAMP","string",6,0,TRAN_TIMESTAMP)) == NULL)
	{
		UnionUserErrLog("in setSYS_HEADToESBnode:: addFieldToNode::[SYS_HEAD.TRAN_TIMESTAMP] err!\n");
		//	return(errCodePackageDefMDL_FldNotDefined);
	}

	//������ˮ��
	/*
	if((childNode = addFieldToNode(outNode,"SYS_HEAD","CONSUMER_SEQ_NO","string",strlen(gunionESB_CONSUMER_SEQ_NO),0,gunionESB_CONSUMER_SEQ_NO)) == NULL)
	{
		UnionUserErrLog("in setSYS_HEADToESBnode:: addFieldToNode::[SYS_HEAD.DEST_BRANCH_NO] err!\n");
		//	return(errCodePackageDefMDL_FldNotDefined);
	}
	*/

	return(0);	
}

int UnionInitAPISO(char *serviceCode)
{
	unsigned long	k;


	char	dlName[128];
	char	fullDLName[128];
	char	funName[128];

	if (gunionIsInitInterChangeFunHandle == 0)
	{
		for (FunHandle_NO = 0; FunHandle_NO < defMaxESBFunHandleNum; FunHandle_NO++)
		{
			gunionInterChangeFunHandle[FunHandle_NO].id = -1;
			memset(gunionInterChangeFunHandle[FunHandle_NO].serviceCode,0,sizeof(gunionInterChangeFunHandle[FunHandle_NO].serviceCode));
			gunionInterChangeFunHandle[FunHandle_NO].pfunSetRequestXMLPackage = NULL;
			gunionInterChangeFunHandle[FunHandle_NO].pfunSetResponseXMLPackage = NULL;
		}
		gunionIsInitInterChangeFunHandle = 1;
	}

	if (gunionIsInitESBSOHandle == 0)
	{
		for (SOHandle_NO = 0; SOHandle_NO < defMaxESBSONum; SOHandle_NO++)
		{
			gunionESBSOHandle[SOHandle_NO].id = -1;
			memset(gunionESBSOHandle[SOHandle_NO].dlName,0,sizeof(gunionESBSOHandle[SOHandle_NO].dlName));
			gunionESBSOHandle[SOHandle_NO].handle = NULL;
		}
		gunionIsInitESBSOHandle = 1;
	}

	FunHandle_NO = UnionHashTableHashString(serviceCode,0);
	FunHandle_NO %= defMaxESBFunHandleNum;

	for (k = 0;; k++)
	{
		if (k == defMaxESBFunHandleNum)
		{
			UnionUserErrLog("in UnionInitAPISO:: fun number[%d] is too big!\n",defMaxESBFunHandleNum);
			return(errCodeFileTransSvrInvalidServiceCode);
		}

		if (strcmp(gunionInterChangeFunHandle[FunHandle_NO].serviceCode,serviceCode) == 0)
		{
			return(1);
		}
		else if (gunionInterChangeFunHandle[FunHandle_NO].id == -1)
		{
			gunionInterChangeFunHandle[FunHandle_NO].id = FunHandle_NO;
			break;
		}
		else
			FunHandle_NO = (FunHandle_NO + 1) % defMaxESBFunHandleNum;
	}

	snprintf(dlName,sizeof(dlName),"libDGNS.%s.so",serviceCode);

	SOHandle_NO = UnionHashTableHashString(dlName,0);
	SOHandle_NO %= defMaxESBFunHandleNum;

	for (k = 0;; k++)
	{
		if (k == defMaxESBSONum)
		{
			UnionUserErrLog("in UnionInitAPISO:: handle number[%d] is too big!\n",defMaxESBSONum);
			return(errCodeFileTransSvrInvalidServiceCode);
		}

		if (strcmp(gunionESBSOHandle[SOHandle_NO].dlName,dlName) == 0)
		{
			if (gunionESBSOHandle[SOHandle_NO].handle == NULL)
			{
				UnionUserErrLog("in UnionInitAPISO:: dlName[%s] handle[%ld] is null!\n",dlName,SOHandle_NO);
				return(errCodeFileTransSvrInvalidServiceCode);
			}

			snprintf(funName,sizeof(funName),"UnionSetRequestXMLPackageFromESBnode");
			if ((gunionInterChangeFunHandle[FunHandle_NO].pfunSetRequestXMLPackage = (funSetRequestXMLPackage *)dlsym(gunionESBSOHandle[SOHandle_NO].handle,funName)) == NULL)
			{
				gunionInterChangeFunHandle[FunHandle_NO].id = -1;
				UnionUserErrLog("in UnionInitAPISO:: dlsym error funName[%s]!\n",funName);
				return(errCodeFileTransSvrInvalidServiceCode);
			}

			snprintf(funName,sizeof(funName),"UnionSetResponseESBnodeFromXMLPackage");
			if ((gunionInterChangeFunHandle[FunHandle_NO].pfunSetResponseXMLPackage = (funSetResponseXMLPackage *)dlsym(gunionESBSOHandle[SOHandle_NO].handle,funName)) == NULL)
			{
				gunionInterChangeFunHandle[FunHandle_NO].id = -1;
				UnionUserErrLog("in UnionInitAPISO:: dlsym error funName[%s]!\n",funName);
				return(errCodeFileTransSvrInvalidServiceCode);
			}
			strcpy(gunionInterChangeFunHandle[FunHandle_NO].serviceCode,serviceCode);
			return(1);
		}
		else if (gunionESBSOHandle[SOHandle_NO].id == -1)
		{
			gunionESBSOHandle[SOHandle_NO].id = SOHandle_NO;
			break;
		}
		else
			SOHandle_NO = (SOHandle_NO + 1) % defMaxESBSONum;
	}	

	if (getenv("UNIONDLDIR") != NULL)
		snprintf(fullDLName,sizeof(fullDLName),"%s/%s",getenv("UNIONDLDIR"),dlName);
	else
		snprintf(fullDLName,sizeof(fullDLName),"%s/bin/%s",getenv("UNIONREC"),dlName);

	if ((gunionESBSOHandle[SOHandle_NO].handle = dlopen(fullDLName, RTLD_NOW)) == NULL)
	{
		UnionUserErrLog("in UnionInitAPISO:: dlopen[%s] error[%s]!\n",fullDLName,dlerror());
		gunionInterChangeFunHandle[FunHandle_NO].id = -1;
		gunionESBSOHandle[SOHandle_NO].id = -1;
		return(errCodeFileTransSvrInvalidServiceCode);
	}
	strcpy(gunionESBSOHandle[SOHandle_NO].dlName,dlName);

	snprintf(funName,sizeof(funName),"UnionSetRequestXMLPackageFromESBnode");
	if ((gunionInterChangeFunHandle[FunHandle_NO].pfunSetRequestXMLPackage = (funSetRequestXMLPackage *)dlsym(gunionESBSOHandle[SOHandle_NO].handle,funName)) == NULL)
	{
		gunionInterChangeFunHandle[FunHandle_NO].id = -1;
		UnionUserErrLog("in UnionInitAPISO:: dlsym error funName[%s]!\n",funName);
		return(errCodeFileTransSvrInvalidServiceCode);
	}
	snprintf(funName,sizeof(funName),"UnionSetResponseESBnodeFromXMLPackage");
	if ((gunionInterChangeFunHandle[FunHandle_NO].pfunSetResponseXMLPackage = (funSetResponseXMLPackage *)dlsym(gunionESBSOHandle[SOHandle_NO].handle,funName)) == NULL)
	{
		gunionInterChangeFunHandle[FunHandle_NO].id = -1;
		UnionUserErrLog("in UnionInitAPISO:: dlsym error funName[%s]!\n",funName);
		return(errCodeFileTransSvrInvalidServiceCode);
	}
	strcpy(gunionInterChangeFunHandle[FunHandle_NO].serviceCode,serviceCode);

	return 1;
}

int	UnionSetRequestPackage(struct stNode *inNode)
{
	int ret;
	ret = (*gunionInterChangeFunHandle[FunHandle_NO].pfunSetRequestXMLPackage)(inNode);
	return(ret);
}


int UnionSetResponsePackage(struct stNode *outNode)
{
	int ret;
	ret = (*gunionInterChangeFunHandle[FunHandle_NO].pfunSetResponseXMLPackage)(outNode);
	return(ret);
}



// ESB����ת�ɵ�ǰESSC����
int UnionConvertOtherPackageToXMLPackage(unsigned char *buf,int lenOfBuf,char *cliIPAddr)
{

	int	len;
	int	ret;
	char	currServiceCode[128];
	char	*ptr = NULL;
	char	hsmGroupIDList[32];//���ܻ����
	char	currServiceCode_log[128];


	//add by lusj 20160127 
	int 	lenOfmsg;	//��Ϣͷ��+XML���ĳ�
	int 	lenOfhead=0;//��Ϣͷ��
	int		lenOfXML=0;//XML���ĳ�
	int		isCheckEsbPackage=-1;
	char 	msg[40960];//��Ϣͷ+XML����
	char	lenOfhead_buf[4+1];
	char 	head [2048];	//��Ϣͷ������У����Ϣ
	char	xml[30720];	//XML����

	struct stNode *inNode;
	//add end by lusj 20160127


	if ((ptr = UnionReadStringTypeRECVar("abOfMyself")) == NULL)
	{
		UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionReadStringTypeRECVar[%s] error!\n","abOfMyself");
		return(errCodeRECMDL_VarNotExists);
	}
	if (strcasecmp(UnionGetIDOfCustomization(),"UnionPay") == 0)
		gunionIsUnionPayPackage = 1;
	else
		gunionIsUnionPayPackage = 0;

	// ��XML����
	if (memcmp(buf,PACKAGE_VERSION_001,4) == 0)
	{
		UnionSetPackageType(PACKAGE_TYPE_V001);
		gunionIsNotConvertPackage = 1;
		return(0);
	}
	else if (memcmp(buf,"<?xml",5) == 0)
	{
		UnionSetPackageType(PACKAGE_TYPE_XML);
		gunionIsNotConvertPackage = 1;
		return(0);
	}
	else
	{
		UnionSetPackageType(PACKAGE_TYPE_V001);
		gunionIsNotConvertPackage = 0;
	}


	//add by lusj 20160127 ��ݸũ�����ⱨ�Ĵ����ӱ����л�ȡ�ض���ʽ��XML����
	UnionRecESBTranscationLog(buf,lenOfBuf);
	//��ȡ��Ϣ���ݳ�
	lenOfmsg =lenOfBuf; 

	//��ȡ��Ϣ����
	memcpy(msg,buf,lenOfmsg);
	msg[lenOfmsg]=0;

	//��ȡ��Ϣͷ��
	memset(lenOfhead_buf, 0, sizeof(lenOfhead_buf));
	memcpy(lenOfhead_buf, msg, 4);//��Ϣ��ǰ4���ֽ�Ϊ��Ϣͷ�ĳ���
	lenOfhead = atoi(lenOfhead_buf);

	//��ȡ��Ϣͷ
	memcpy(head,msg+4,lenOfhead);
	head[lenOfhead]=0;

	/*���Խ׶ν��ܱ��ĵ�ͷ���䣬���ظ���Ӧ����*/
	memcpy(head_buf,msg,lenOfhead+4);
	head_buf[lenOfhead+4]=0;

	//��ȡXML����
	lenOfXML=lenOfmsg-4-lenOfhead;//XML���ĳ�
	if(lenOfXML > 0)
	{
		memcpy(xml,msg+4+lenOfhead,lenOfXML);
		xml[lenOfXML]=0;
	}

	if (UnionReadIntTypeRECVar("isCheckEsbPackage") <= 0)
		isCheckEsbPackage=0;
	else
		isCheckEsbPackage=1;
	if (isCheckEsbPackage == 1) 
	{
		if ((ret= synWorkKey())<1)
		{
		 	UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: checkResponseSeries!\n");
			return (ret);
		}
		//�����ض���������У�� ���ڽ���У����Ҫ����ESBϵͳ�� ���Խ׶Σ�������
		if((ret=checkResponseSeries(head,xml)) < 0)
		{
			UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: checkResponseSeries!\n");
			return(ret);
		}
	}	
	//���ܣ���XML����ת����stNode�����ͽṹ
	inNode = xmlToNode(xml,lenOfXML+1);


	//�ӽڵ��л�ȡSYS_HEAD�����Ϣ
	if((ret = getSYS_HEADFromESBnode(inNode)) < 0)
	{
		UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: getSYS_HEADFromESBnode failed!\n");
		return(errCodePackageDefMDL_FldNotDefined);
	}
	//add end by lusj 20160127

	// ����������ͷ
	if ((ret = UnionInitRequestXMLPackage(NULL,NULL,0)) < 0)
	{
		UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionInitRequestXMLPackage!\n");
		return(ret);
	}

	if (gunionIsUnionPayPackage)
	{
		// ����ϵͳID
		if ((ret = UnionSetRequestXMLPackageValue("head/sysID","unionPay")) < 0)
		{
			UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetRequestXMLPackageValue [head/sysID][unionPay]!\n");
			return(ret);
		}
		// ����Ӧ��ID
		if ((ret = UnionSetRequestXMLPackageValue("head/appID","unionPay")) < 0)
		{
			UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetRequestXMLPackageValue [head/appID][unionPay]!\n");
			return(ret);
		}

		// ��ȡ���ܻ����	
		if ((ret = UnionReadHsmGroupIDListByAppID("unionPay",hsmGroupIDList,sizeof(hsmGroupIDList))) < 0)
		{
			UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionReadHsmGroupIDListByAppID[unionPay]!\n");
			return(ret);
		}
		hsmGroupIDList[ret] = 0;
	}
	else
	{
		// ����ϵͳID
		if ((ret = UnionSetRequestXMLPackageValue("head/sysID",gunionESB_AppID)) < 0)
		{
			UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetRequestXMLPackageValue [head/sysID][%s]!\n",gunionESB_AppID);
			return(ret);
		}
		// ����Ӧ��ID
		if ((ret = UnionSetRequestXMLPackageValue("head/appID",gunionESB_AppID)) < 0)
		{
			UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetRequestXMLPackageValue [head/appID][%s]!\n",gunionESB_AppID);
			return(ret);
		}

		// ��ȡ���ܻ����	
		if ((ret = UnionReadHsmGroupIDListByAppID(gunionESB_AppID,hsmGroupIDList,sizeof(hsmGroupIDList))) < 0)
		{
			UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionReadHsmGroupIDListByAppID[%s]!\n",gunionESB_AppID);
			return(ret);
		}
		hsmGroupIDList[ret] = 0;
	}

	// ����IP��ַ
	if ((ret = UnionSetRequestXMLPackageValue("head/clientIPAddr",cliIPAddr)) < 0)
	{
		UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetRequestXMLPackageValue [head/cliIPAddr][%s]!\n",cliIPAddr);
		return(ret);
	}
	// ���ý���ʱ��
	if ((ret = UnionSetRequestXMLPackageValue("head/transTime",UnionGetCurrentFullSystemDateTime())) < 0)
	{
		UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetRequestXMLPackageValue [head/transTime][%s]!\n",UnionGetCurrentFullSystemDateTime());
		return(ret);
	}
	// ���ý��ױ�ʶ
	if ((ret = UnionSetRequestXMLPackageValue("head/transFlag","1")) < 0)
	{
		UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetRequestXMLPackageValue [head/transFlag][%s]!\n","1");
		return(ret);
	}

	len=snprintf(currServiceCode,sizeof(currServiceCode),"%s%s%s",gunionESB_ServiceCode,gunionESB_ServiceScene,gunionESB_ServiceVersion);
	currServiceCode[len]=0;
	len=snprintf(currServiceCode_log,sizeof(currServiceCode),"%s_%s_%s",gunionESB_ServiceCode,gunionESB_ServiceScene,gunionESB_ServiceVersion);
	currServiceCode_log[len]=0;

	//���أӣϽӿ�
	if((ret = UnionInitAPISO(currServiceCode))<0)
	{
		UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionInitAPISO err! \n");
		return (errCodeFileTransSvrInvalidServiceCode);
	}


	if((ret=UnionSetRequestPackage(inNode)) < 0) 
	{
		UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetRequestPackage err ! \n");
		return(ret);
	}

	freeNode(inNode);//�ͷ����ڵ�
	UnionESBStartLog(gunionESB_AppID,currServiceCode_log,gunionESB_CONSUMER_SEQ_NO);
	return(1);
}

int TranceRemarkToUTF8(char *responseRemark,char *UTF8ResponseRemark)
{
	iconv_t cd;
	size_t insize=strlen(responseRemark);
	 char *in = responseRemark;
	 char *out = UTF8ResponseRemark;
	 size_t outlen = insize *4;

	 
	int lenOfUTF8ResponseRemark;

	cd = iconv_open("UTF-8","GBK");
	
	if (cd==0) 
		return -1;
	
	
	if (iconv(cd,&in,&insize,&out,&outlen) == -1) 
		return -1;

	lenOfUTF8ResponseRemark=outlen=strlen(UTF8ResponseRemark);

	UTF8ResponseRemark[lenOfUTF8ResponseRemark]=0;
	iconv_close(cd);

	return (lenOfUTF8ResponseRemark);	
}


// ��ǰESSC����ת��ESB����
int UnionConvertXMLPackageToOtherPackage(unsigned char *buf,int sizeOfBuf)
{
	int	len;
	int	ret;
	int		result_flag=-1;
	int	errCode = 0;
	char	responseCode[32];
	char		ESB_responseCode[32];
	char	responseRemark[128];
	char	currServiceCode[128];
	char	UTF8ResponseRemark[256];
	int		lenOfUTF8ResponseRemark;
	char	currServiceCode_log[128];


	//add begin by lusj 20160128
	int	oLen=0;
	char output[MAX_SOCK_BUF_SIZE];
	struct stNode *outNode;
	
	memset(output,0x00,sizeof(output));
	outNode = createXmlNode();
	//add end by lusj 20160128

	// ��XML����
	if (gunionIsNotConvertPackage)
		return(0);


	UnionLocateResponseXMLPackage("",0);

	// ��ȡ��Ӧ��
	if ((ret = UnionReadResponseXMLPackageValue("head/responseCode",responseCode,sizeof(responseCode))) < 0)
	{
		UnionUserErrLog("in UnionConvertXMLPackageToOtherPackage:: UnionReadResponseXMLPackageValue[head/responseCode]!\n");
		UnionLogResponseXMLPackage();
		return(ret);
	}
	responseCode[ret] = 0;
	errCode = 0 - atoi(responseCode);

	//����ESB���ش�����
	if (errCode < 0)
	{
		len=snprintf(ESB_responseCode,sizeof(ESB_responseCode),"%s%s%s","800804","0000",responseCode);
		ESB_responseCode[len]=0;	
	}
	else if(errCode == 0)
	{
		len=snprintf(ESB_responseCode,sizeof(ESB_responseCode),"%s",responseCode);
		ESB_responseCode[len]=0;
	}

	// ��ȡ��Ӧ����
	if ((ret = UnionReadResponseXMLPackageValue("head/responseRemark",responseRemark,sizeof(responseRemark))) < 0)
	{
		UnionUserErrLog("in UnionConvertXMLPackageToOtherPackage:: UnionReadResponseXMLPackageValue[head/responseRemark]!\n");
		return(ret);
	}
	responseRemark[ret] = 0;
	len=snprintf(currServiceCode_log,sizeof(currServiceCode),"%s_%s_%s",gunionESB_ServiceCode,gunionESB_ServiceScene,gunionESB_ServiceVersion);
	currServiceCode_log[len]=0;

	if(errCode < 0)
	{
		UnionESBErrLog(responseCode,responseRemark,gunionESB_AppID,currServiceCode_log,gunionESB_CONSUMER_SEQ_NO);
		result_flag=1;
	}
	else
		result_flag=0;

	//GBK������ϢתUTF-8
	memset(UTF8ResponseRemark,0,sizeof(UTF8ResponseRemark));	
	if ((ret=TranceRemarkToUTF8(responseRemark,UTF8ResponseRemark))<0)
	{
		UnionDebugLog("in UnionConvertXMLPackageToOtherPackage:: TranceRemarkToUTF8:: trance responseRemark to UTF-8 faile ret=[%d] !\n",ret);
	}
	lenOfUTF8ResponseRemark=ret;
	
	UnionInitEsscResponsePackage();

	//add by lusj 20160129 ��ƽ̨5���ر���ͷ��Ϣ�ӵ����ڵ�ͷ��
	if((ret = setSYS_HEADToESBnode(outNode,ESB_responseCode,UTF8ResponseRemark,strlen(ESB_responseCode),lenOfUTF8ResponseRemark))<0)
	{
		UnionUserErrLog("in UnionConvertXMLPackageToOtherPackage:: setSYS_HEADToESBnode err!\n");
		return(ret);
	}
	//add end by lusj 20160129

	errCode = 0 - atoi(responseCode);


	// ������Ϊ00ʱ���趨���ر��ĵ�body��
	if (errCode == 0)
	{
		len=snprintf(currServiceCode,sizeof(currServiceCode),"%s%s%s",gunionESB_ServiceCode,gunionESB_ServiceScene,gunionESB_ServiceVersion);
		currServiceCode[len]=0;
		if((ret=UnionSetResponsePackage(outNode)) < 0) 
		{
			UnionUserErrLog("in UnionConvertXMLPackageToOtherPackage:: UnionSetResponsePackage[%d] err! \n",ret);
			return(ret);
		}
	}

	//���ṹ��Ϣ����ת��ΪXML������
	putTree2XML(outNode, output, &oLen, 0);
	freeNode(outNode);

	sprintf((char *)buf,"%s",output);	
	buf[oLen]=0;
	UnionSendESBTranscationLog(buf,oLen);
	UnionESBEndLog(result_flag,gunionESB_AppID,currServiceCode_log,gunionESB_CONSUMER_SEQ_NO);
	return(oLen);

}
