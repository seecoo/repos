#ifdef _WIN32
#include <vcl.h>
#include <windows.h>
#include "dir.h"
#endif

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "unionResID.h"
#include "UnionLog.h"

int	gunionCurrentResID		= -1;
int	gunionCurrentServiceID		= -1;
int	gunionMngSvrIsBackuper = 0;	// �жϵ�ǰmngSvr�Ƿ��Ǳ���Svr,2010-01-22,Wolfgang Wang

// add by xusj 20100607 begin
char    gunionCurrentMngSvrClientReqStr[8192+1] = "";   // �洢��ǰ������
int     gunionLenOfCurrentMngSvrClientReqStr = 0;       // ��ǰ���󴮵ĳ���
char	gunionCurrentMngSvrClientResStr[8192+1] = "";	// �洢��ǰ����Ӧ��
int	gunionLenOfCurrentMngSvrClientResStr = 0;		// ��ǰ��Ӧ���ĳ���
char	gunionTellerNoOfCurrentOperation[40+1] = "";		// 2009/8/1,Wolfgang Wang added
int	gunionCurrentOperationType = -1;			// 2009/8/1,Wolfgang Wang added
char	gunionCurrentResName[40+1] = "";			// ��ǰ��Դ����,2009/11/9,����������
int	gunionMngSvrIsBackSvr = 0;	// �жϵ�ǰmngSvr�Ƿ��Ǻ�̨Svr,2010-01-22,Wolfgang Wang

// add by xusj begin 20100315,���ӵ�½��ʾ
char	gunionLogonRemark[256+1] = "";


// add by xusj begin 20100607,���������������
int	gunionIsDBBeginAWork=0;

int	gunionIsNoneMngSvr = 0;	// 2010/6/9,����������

// 2010/6/9,����������
int UnionSetAsNonMngSvr()
{
	gunionIsNoneMngSvr = 1;
	return(0);
}

// 2010/6/9,����������
int UnionIsNonMngSvr()
{
	return(gunionIsNoneMngSvr);
}

// 2010/6/9,����������
int UnionSetAsMngSvr()
{
	gunionIsNoneMngSvr = 0;
	return(0);
}


// ��ʼһ������
int UnionDBBeginAWork()
{
	if (UnionReadIntTypeRECVar("isKmcUseDBWork") > 0)
		gunionIsDBBeginAWork = 1;
	else
		gunionIsDBBeginAWork = 0;
	return 0;
}

/***
// ����һ������
int UnionDBEndAWork()
{
	if (gunionIsDBBeginAWork)
		UnionRollBackWork();
	return 0;
}
***/

int UnionDBIsBeginAWork()
{
	return (gunionIsDBBeginAWork);
}
// add by xusj end 20100607,���������������
int UnionSetLogonRemark(char *remark)
{
	strcpy(gunionLogonRemark, remark);
	return 0;
}

int UnionInitLogonRemark()
{
	memset(gunionLogonRemark, 0, sizeof gunionLogonRemark);
	return 0;
}

char *UnionReadLogonRemark()
{
	return(gunionLogonRemark);
}

int UnionGetLogonRemark(char *remark)
{
	strcpy(remark, gunionLogonRemark);
	return 0;
}
// add by xusj end 20100315,���ӵ�½��ʾ
int UnionSetCurrentTellerNo(char *tellerNo)
{
	strcpy(gunionTellerNoOfCurrentOperation,tellerNo);	// 2009/8/1,Wolfgang Wang added
	return(0);
}
       
int UnionSetCurrentOperationType(int operationType)
{
	return(gunionCurrentOperationType = operationType);
}

int UnionSetCurrentResName(char *resName)	// 2009/11/9,����������
{
	strcpy(gunionCurrentResName,resName);	
	return(0);
}

// add by wolfang wang, 2010-6-2
int UnionSetCurrentMngSvrClientResStr(char *resStr,int lenOfRecStr)
{
	if (lenOfRecStr >= sizeof(gunionCurrentMngSvrClientResStr))
		gunionLenOfCurrentMngSvrClientResStr = sizeof(gunionCurrentMngSvrClientResStr) - 2;
	else
		gunionLenOfCurrentMngSvrClientResStr = lenOfRecStr;
	memcpy(gunionCurrentMngSvrClientResStr,resStr,gunionLenOfCurrentMngSvrClientResStr);
	gunionCurrentMngSvrClientResStr[gunionLenOfCurrentMngSvrClientResStr+1] = 0;
	return;
}

// 2010-3-20 wolfgang added
int UnionReadFldFromCurrentMngSvrClientResStr(char *fldName,char *buf,int sizeOfBuf)
{
	return(UnionReadRecFldFromRecStr(gunionCurrentMngSvrClientResStr,gunionLenOfCurrentMngSvrClientResStr,fldName,buf,sizeOfBuf));
}

// 2010-3-20 wolfgang added
int UnionReadIntTypeFldFromCurrentMngSvrClientResStr(char *fldName,int *intValue)
{
	return(UnionReadIntTypeRecFldFromRecStr(gunionCurrentMngSvrClientResStr,gunionLenOfCurrentMngSvrClientResStr,fldName,intValue));
}

// 2013-1-14 tanhj add
// �޸�����
int UnionUpdateCurrentMngSvrClientResStr(char *fldName,char *buf,int sizeOfBuf)
{
	//��ɾ�������ԭ�����е������
	gunionLenOfCurrentMngSvrClientReqStr = UnionDeleteRecFldFromRecStr(gunionCurrentMngSvrClientReqStr,fldName);
	
	//���޸ĵ���ӵ�������
	return(UnionPutFldToCurrentMngSvrClientReqStr(fldName,buf,sizeOfBuf));
}

// added 2012-06-26
char *UnionGetCurrentMngSvrClientReqStr()
{
        return(gunionCurrentMngSvrClientReqStr);
}
// end of addication 2012-06-26
int UnionSetCurrentMngSvrClientReqStr(char *data,int lenOfData)
{
        if (lenOfData >= sizeof(gunionCurrentMngSvrClientReqStr))
                gunionLenOfCurrentMngSvrClientReqStr = sizeof(gunionCurrentMngSvrClientReqStr) - 1;
        else
        {
                gunionLenOfCurrentMngSvrClientReqStr = lenOfData;
        }
        memcpy(gunionCurrentMngSvrClientReqStr,data,gunionLenOfCurrentMngSvrClientReqStr);
        gunionCurrentMngSvrClientReqStr[gunionLenOfCurrentMngSvrClientReqStr] = 0;
	return(gunionLenOfCurrentMngSvrClientReqStr);
}

int UnionPutFldToCurrentMngSvrClientReqStr(char *fldName,char *value,int lenOfValue)
{
	gunionLenOfCurrentMngSvrClientReqStr = sprintf(gunionCurrentMngSvrClientReqStr,"%s%s=%s|",gunionCurrentMngSvrClientReqStr,fldName,value);
	return(gunionLenOfCurrentMngSvrClientReqStr);
}

int UnionReadFldFromCurrentMngSvrClientReqStr(char *fldName,char *buf,int sizeOfBuf)
{
        return(UnionReadRecFldFromRecStr(gunionCurrentMngSvrClientReqStr,gunionLenOfCurrentMngSvrClientReqStr,fldName,buf,sizeOfBuf));
}

int UnionReadIntTypeFldFromCurrentMngSvrClientReqStr(char *fldName,int *intValue)
{
        return(UnionReadIntTypeRecFldFromRecStr(gunionCurrentMngSvrClientReqStr,gunionLenOfCurrentMngSvrClientReqStr,fldName,intValue));
}

int UnionReadLongTypeFldFromCurrentMngSvrClientReqStr(char *fldName,long *intValue)
{
        return(UnionReadLongTypeRecFldFromRecStr(gunionCurrentMngSvrClientReqStr,gunionLenOfCurrentMngSvrClientReqStr,fldName,intValue));
}
// add by xusj 20100607 end

// ��ȡ��ǰ��Դ��
int UnionGetCurrentResID()
{
	return gunionCurrentResID;
}

// ���õ�ǰ��Դ��
int UnionSetCurrentResID(int resID)
{
	gunionCurrentResID = resID;
}

// ��ȡ��ǰ�����
int UnionGetCurrentServiceID()
{
	return gunionCurrentServiceID;
}

// ���õ�ǰ�����
int UnionSetCurrentServiceID(int serviceID)
{
	gunionCurrentServiceID = serviceID;
}


//2009/11/9,����������
char *UnionGetCurrentResName()
{
	return(gunionCurrentResName);
}

// 2009/8/1,Wolfgang Wang added
char *UnionGetCurrentOperationTellerNo()
{
	return(gunionTellerNoOfCurrentOperation);
}

// 2009/8/1,Wolfgang Wang added
int UnionIsBackuperMngSvr()
{
	return(gunionMngSvrIsBackuper);
}

// 2009/8/1,Wolfgang Wang added
// �ж��Ƿ�Ҫ���ֵܷ�����ͬ���ò���
int UnionIsSynchronizeComplexDBOperationToBrothers()
{
	if (gunionMngSvrIsBackuper)	// 2010-01-22,Wolfgang Wang
		return(0);
	if (strcmp(gunionTellerNoOfCurrentOperation,"backuper") == 0)
		return(0);
	if (UnionReadIntTypeRECVar("isKmcSynToBrothers")<=0) // 2010-06-07,xusj
		return(0);
	else
		return(1);
}

// 2010-01-22,Wolfgang Wang
void UnionSetAsBackupMngSvr()
{
	gunionMngSvrIsBackuper = 1;
}

// 2010-01-22,Wolfgang Wang
void UnionSetAsBackMngSvr()
{
	gunionMngSvrIsBackSvr = 1;
}
 
// 2010-01-22,Wolfgang Wang
int UnionIsBackMngSvr()
{
	return(gunionMngSvrIsBackSvr);
}

int UnionIsNonAuthorizationMngSvrService(int resID,int serviceID)
{
	//UnionLog("entering ...\n");
	if (UnionIsNonMngSvr())
		return(1);
			
	if (serviceID == conResCmdReadAllMainMenuDef)	// ��ȡ�������˵�
		return(1);

	if (resID == conResIDOperatorTBL)
	{
		switch (serviceID)
		{
			case	conResCmdReadMainMenuItem:	// ��ȡ���˵��Ĳ˵���
			case	conResCmdReadSecondaryMenuItem:	// ��ȡ�����˵��Ĳ˵���
			case	conResCmdReadInterfacePopupMenuItem:	// ��ȡ�����Ҽ��˵���
			case	conResCmdReadRecordPopupMenuItem:	// ��ȡ��¼�Ҽ��˵���
			case	conResCmdReadRecordDoubleClickMenuItem:	// ��ȡ��¼˫���˵���
			case	conResCmdReadMenuItemCmd:	// ��ȡ�˵����ִ������
			case	conResCmdReadPopupMenuItem:	// ��ȡ�Ҽ��˵����ִ������
			// 2011-11-11 ����������
			case	conResCmdUpdatePassword:	// �޸�����
			// 2011-7-5 ����������
			case	conResCmdGetOneRandomNumber:	// ��ȡ�����
				return(1);
		}
	}
	else if ((resID == conResIDMenuDef) || (resID == conResIDMainMenu))
	{
		switch (serviceID)
		{
			case	conResCmdGenerateMainMenu:	// ��������
			case	conResCmdGenerateSpec2LevelMenu:	// ����ָ�������˵�
				return(1);
		}
	}
	else if (resID == conResIDViewList)
	{
		switch (serviceID)
		{
			case	conResCmdGenerateTBLQueryInterface:	// ���ɽ�����ͼ
			case	conResCmdCreateDefaultViewOfTBL:	// ���ɽ�����ͼ
				return(1);
		}
	}
	else if (resID == conResIDEnumValueDef)
	{
		switch (serviceID)
		{
			case	conResCmdDownloadEnumDef:		// ��ȡö�ٶ����ļ�
				return(1);
		}
	}
	// UnionLog("in UnionIsNonAuthorizationMngSvrService:: [%03d-%03d] authorizied need!\n",resID,serviceID);
	return(0);
}

int UnionFormTaskNameOfMngSvr(int port,char *taskName)
{
	char	varName[128+1];
	
	// �ж��Ƿ��Ǻ�̨�˿�
	sprintf(varName,"isBackupMngSvrPort%d",port);
	if (UnionReadIntTypeRECVar(varName) > 0)
	{
		UnionSetAsBackupMngSvr();		
		sprintf(taskName,"%s %d backup",UnionGetApplicationName(),port);
	}
	else
	{
		sprintf(varName,"isBackMngSvrPort%d",port);
		if (UnionReadIntTypeRECVar(varName) > 0)
		{
			UnionSetAsBackMngSvr();		
			sprintf(taskName,"%s %d back",UnionGetApplicationName(),port);
		}
		else
			sprintf(taskName,"%s %d",UnionGetApplicationName(),port);
	}
	return(0);
}

