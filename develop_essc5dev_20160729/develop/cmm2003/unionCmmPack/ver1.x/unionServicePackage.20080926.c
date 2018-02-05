//	Wolfgang Wang, 2008/9/28

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "UnionStr.h"
#include "unionServicePackage.h"
#include "unionErrCode.h"
#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif
#include "UnionLog.h"
#include "UnionProc.h"
#ifndef _WIN32
#include "unionREC.h"
#else
#include "unionRECVar.h"
#endif

int gunionIsSpierClientPackage = 0;
int gunionLogPackageAnyway = 0;
unsigned long gunionCmmPackSsn = 0;

// �Ƚ��������ı�ʶ
/*
�������
	poriCmmPack	Դ��
	pdesCmmPack	Ŀ���
�������
	��
����ֵ
	>= 0		һ��,����һ�±�ʶ�������
	<0		��һ��
*/
int UnionVerifyCmmPackIdentifiedTag(PUnionCmmPackData poriCmmPack,PUnionCmmPackData pdesCmmPack)
{
	int			ret;
	unsigned long		ssn1 = 0,ssn2 = 0;
	char			sysTime1[100+1],sysTime2[100+1];
	int			pid1 = 0,threadID1 = 0,pid2 = 0,threadID2 = 0;
	int			okFld = 0;
	
	if ((poriCmmPack == NULL) || (pdesCmmPack == NULL))
		return(errCodeParameter);
		
	// �����̺�
	UnionReadIntTypeCmmPackFldFromFldDataList(poriCmmPack,conServiceFldProcID,&pid1);
	UnionReadIntTypeCmmPackFldFromFldDataList(pdesCmmPack,conServiceFldProcID,&pid2);
	if (pid1 != pid2)
	{
		UnionUserErrLog("in UnionVerifyCmmPackIdentifiedTag:: pid [%d] of first pack != that [%d] of second!\n",pid1,pid2);
		return(errCodeCmmPackMDL_PackIDNotIdentified);
	}
	else
		okFld++;
	// ���̺߳�
	UnionReadIntTypeCmmPackFldFromFldDataList(poriCmmPack,conServiceFldThreadID,&threadID1);
	UnionReadIntTypeCmmPackFldFromFldDataList(pdesCmmPack,conServiceFldThreadID,&threadID2);
	if (threadID1 != threadID2)
	{
		UnionUserErrLog("in UnionVerifyCmmPackIdentifiedTag:: threadID [%d] of first pack != that [%d] of second!\n",threadID1,threadID2);
		return(errCodeCmmPackMDL_PackIDNotIdentified);
	}
	else
		okFld++;
	// �����к�
	UnionReadUnsignedLongTypeCmmPackFldFromFldDataList(poriCmmPack,conServiceFldPackSSN,&ssn1);
	UnionReadUnsignedLongTypeCmmPackFldFromFldDataList(poriCmmPack,conServiceFldPackSSN,&ssn2);
	if (ssn2 != ssn1)
	{
		UnionUserErrLog("in UnionVerifyCmmPackIdentifiedTag:: ssn [%d] of first pack != that [%d] of second!\n",ssn1,ssn2);
		return(errCodeCmmPackMDL_PackIDNotIdentified);
	}
	else
		okFld++;
	// ��ʱ��
	memset(sysTime1,0,sizeof(sysTime1));
	memset(sysTime2,0,sizeof(sysTime2));
	UnionReadCmmPackFldFromFldDataList(poriCmmPack,conServiceFldPackTimeStamp,sizeof(sysTime1),sysTime1);
	UnionReadCmmPackFldFromFldDataList(poriCmmPack,conServiceFldPackTimeStamp,sizeof(sysTime2),sysTime2);
	if (strcmp(sysTime1,sysTime2) != 0)
	{
		UnionUserErrLog("in UnionVerifyCmmPackIdentifiedTag:: sysTime [%s] of first pack != that [%s] of second!\n",sysTime1,sysTime2);
		return(errCodeCmmPackMDL_PackIDNotIdentified);
	}
	else
		okFld++;
	return(okFld);
}

// ����һ�����ı�ʶ
/*
�������
	poriCmmPack	Դ��
�������
	pdesCmmPack	Ŀ���
����ֵ
	>= 0		�ɹ�
	<0		�������
*/
int UnionCopyCmmPackIdentifiedTag(PUnionCmmPackData poriCmmPack,PUnionCmmPackData pdesCmmPack)
{
	int			ret;
	unsigned long		ssn;
	char			sysTime[100+1];
	int			pid = -1,threadID = -1;
	
	// �����̺�
	if ((ret = UnionReadIntTypeCmmPackFldFromFldDataList(poriCmmPack,conServiceFldProcID,&pid)) >= 0)
	{
		if ((ret = UnionPutIntTypeCmmPackFldIntoFldDataList(conServiceFldProcID,pid,pdesCmmPack)) < 0)
		{
			UnionUserErrLog("in UnionCopyCmmPackIdentifiedTag:: UnionPutIntTypeCmmPackFldIntoFldDataList [%d]!\n",conServiceFldProcID);
			return(ret);
		}
	}
	// ���̺߳�
	if ((ret = UnionReadIntTypeCmmPackFldFromFldDataList(poriCmmPack,conServiceFldThreadID,&threadID)) >= 0)
	{
		if ((ret = UnionPutIntTypeCmmPackFldIntoFldDataList(conServiceFldThreadID,threadID,pdesCmmPack)) < 0)
		{
			UnionUserErrLog("in UnionCopyCmmPackIdentifiedTag:: UnionPutIntTypeCmmPackFldIntoFldDataList [%d]!\n",conServiceFldThreadID);
			return(ret);
		}
	}
	// �����к�
	if ((ret = UnionReadUnsignedLongTypeCmmPackFldFromFldDataList(poriCmmPack,conServiceFldPackSSN,&ssn)) >= 0)
	{
		if ((ret = UnionPutUnsignedLongTypeCmmPackFldIntoFldDataList(conServiceFldPackSSN,ssn,pdesCmmPack)) < 0)
		{
			UnionUserErrLog("in UnionCopyCmmPackIdentifiedTag:: UnionPutIntTypeCmmPackFldIntoFldDataList [%d]!\n",conServiceFldPackSSN);
			return(ret);
		}
	}
	// ��ʱ��
	memset(sysTime,0,sizeof(sysTime));
	if ((ret = UnionReadCmmPackFldFromFldDataList(poriCmmPack,conServiceFldPackTimeStamp,sizeof(sysTime),sysTime)) >= 0)
	{
		if ((ret = UnionPutStringTypeCmmPackFldIntoFldDataList(conServiceFldPackTimeStamp,sysTime,pdesCmmPack)) < 0)
		{
			UnionUserErrLog("in UnionCopyCmmPackIdentifiedTag:: UnionPutStringTypeCmmPackFldIntoFldDataList [%d]!\n",conServiceFldPackTimeStamp);
			return(ret);
		}
	}
	return(0);
}


// Ϊһ������һ����ʶ
/*
�������
	��
�������
	ppack		���ṹ
����ֵ
	>= 0		�ɹ�
	<0		�������
*/
int UnionPutCmmPackIdentifiedTag(PUnionCmmPackData pcmmPack)
{
	int			ret;
	unsigned long		ssn;
	char			sysTime[14+1];
	
	// ������̺�
	if ((ret = UnionPutIntTypeCmmPackFldIntoFldDataList(conServiceFldProcID,UnionGetPID(),pcmmPack)) < 0)
	{
		UnionUserErrLog("in UnionPutCmmPackIdentifiedTag:: UnionPutIntTypeCmmPackFldIntoFldDataList [%d]!\n",conServiceFldProcID);
		return(ret);
	}
	// ����̺߳�
	if ((ret = UnionPutIntTypeCmmPackFldIntoFldDataList(conServiceFldThreadID,UnionGetThreadID(),pcmmPack)) < 0)
	{
		UnionUserErrLog("in UnionPutCmmPackIdentifiedTag:: UnionPutIntTypeCmmPackFldIntoFldDataList [%d]!\n",conServiceFldThreadID);
		return(ret);
	}
	// ������к�
	ssn = ++gunionCmmPackSsn;
	if ((ret = UnionPutIntTypeCmmPackFldIntoFldDataList(conServiceFldPackSSN,ssn,pcmmPack)) < 0)
	{
		UnionUserErrLog("in UnionPutCmmPackIdentifiedTag:: UnionPutIntTypeCmmPackFldIntoFldDataList [%d]!\n",conServiceFldPackSSN);
		return(ret);
	}
	// ���ʱ��
	memset(sysTime,0,sizeof(sysTime));
	UnionGetFullSystemDateTime(sysTime);
	if ((ret = UnionPutStringTypeCmmPackFldIntoFldDataList(conServiceFldPackTimeStamp,sysTime,pcmmPack)) < 0)
	{
		UnionUserErrLog("in UnionPutCmmPackIdentifiedTag:: UnionPutStringTypeCmmPackFldIntoFldDataList [%d]!\n",conServiceFldPackTimeStamp);
		return(ret);
	}
	return(0);
}

void UnionSetLogPackageAnyway()
{
	gunionLogPackageAnyway = 1;
}

void UnionCloseLogPackageAnyway()
{
	gunionLogPackageAnyway = 0;
}


// ���ý���ĳ����Ǽ�ؿͻ���
int UnionSetPackageAsSpierClientPackage()
{
	gunionIsSpierClientPackage = 1;
        return(0);
}

// ��ʼ������
void UnionInitServicePackage(PUnionServicePackage ppack)
{
	memset(ppack->idOfApp,0,sizeof(ppack->idOfApp));
	memset(ppack->serviceID,0,sizeof(ppack->serviceID));
	ppack->direction = conIsRequestServicePackage;
	ppack->resCode = 0;
	UnionInitCmmPackFldDataList(&(ppack->datagram));
	return;
}

// ���ð�ͷ
int UnionSetServicePackageHeader(char *idOfApp,int serviceCode,int direction,int resCode,PUnionServicePackage ppack)
{
	if ((idOfApp == NULL) || (ppack == NULL))
	{
		UnionUserErrLog("in UnionSetServicePackageHeader:: null pointer!\n");
		return(errCodeParameter);
	}
	if (strlen(idOfApp) >= 2)
		memcpy(ppack->idOfApp,idOfApp,sizeof(ppack->idOfApp)-1);
	else
		memset(ppack->idOfApp,' ',2);
	sprintf(ppack->serviceID,"%03d",serviceCode%1000);
	ppack->direction = direction;
	if (direction == conIsResponseServicePackage)
		ppack->resCode = resCode;
	else
		ppack->resCode = 0;
	return(0);
}

// ���ð�ͷ
int UnionSetServicePackageData(PUnionCmmPackData pdata,PUnionServicePackage ppack)
{
	if ((pdata == NULL) || (ppack == NULL))
	{
		UnionUserErrLog("in UnionSetServicePackageData:: null pointer!\n");
		return(errCodeParameter);
	}
	//memcpy(&(ppack->datagram),pdata,sizeof(*pdata));
	//return(0);
	return(UnionCopyPackFldDataList(pdata,&(ppack->datagram)));
}

// ��һ����
/*
�������
	ppack		���ṹ
	sizeOfBuf	buf�Ĵ�С
�������
	buf		��õİ�
����ֵ
	>= 0		��õİ��ĳ���
	<0		�������
*/
int UnionPackPackage(PUnionServicePackage ppack,char *buf,int sizeOfBuf)
{
	int	ret;
	int	offset;
	
	if ((ppack == NULL) || (buf == NULL))
	{
		UnionUserErrLog("in UnionPackPackage:: null pointer!\n");
		return(errCodeParameter);
	}
	if (sizeOfBuf < 2 + 3 + 1)
	{
		UnionUserErrLog("in UnionPackPackage:: sizeOfBuf [%d] too small!\n",sizeOfBuf);
		return(errCodeSmallBuffer);
	}
	memcpy(buf,ppack->idOfApp,2);
	offset = 2;
	memcpy(buf+offset,ppack->serviceID,3);
	offset += 3;
	sprintf(buf+offset,"%d",ppack->direction % 10);
	offset++;
	if (ppack->direction == conIsResponseServicePackage)
	{
		sprintf(buf+offset,"%06d",ppack->resCode);
		offset += 6;
	}
	if ((ret = UnionPackFldDataListIntoCmmPack(&(ppack->datagram),sizeOfBuf-offset,buf+offset)) < 0)
	{
		UnionUserErrLog("in UnionPackPackage:: UnionPackFldDataListIntoCmmPack!\n");
		return(ret);
	}
	return(offset+ret);
}	

// ��һ����
/*
�������
	data		����
	lenOfData	���ݵĳ���
�������
	ppack		����İ�
����ֵ
	>= 0		����İ�����Ч����
	<0		�������
*/
int UnionUnpackPackage(char *data,int lenOfData,PUnionServicePackage ppack)
{
	int	ret;
	int	offset;
	
	if ((ppack == NULL) || (data == NULL))
	{
		UnionUserErrLog("in UnionUnpackPackage:: null pointer!\n");
		return(errCodeParameter);
	}
	if (lenOfData < 2 + 3 + 1)
	{
		UnionUserErrLog("in UnionUnpackPackage:: lenOfData [%d] too short!\n",lenOfData);
		return(errCodeAPIPackageTooShort);
	}
	memcpy(ppack->idOfApp,data,2);
	offset = 2;
	memcpy(ppack->serviceID,data+offset,3);
	offset += 3;
	ppack->direction = data[offset] - '0';
	offset++;
	if (ppack->direction == conIsResponseServicePackage)
	{
		if (offset + 6 > lenOfData)
		{
			UnionUserErrLog("in UnionUnpackPackage:: lenOfData [%d] too short!\n",lenOfData);
			return(errCodeAPIPackageTooShort);
		}
		ppack->resCode = UnionConvertIntStringToInt(data+offset,6);
		offset += 6;
	}
	if ((ret = UnionUnpackCmmPackIntoFldDataList(data+offset,lenOfData-offset,&(ppack->datagram))) < 0)
	{
		UnionUserErrLog("in UnionUnpackPackage:: UnionUnpackCmmPackIntoFldDataList! len=[%04d][%s]\n",lenOfData,data);
		return(ret);
	}
	return(offset+ret);
}	

// ��ʼ��������
int UnionInitRequestPackage(PUnionServiceRequestPackage ppack)
{
	UnionInitServicePackage(ppack);
	ppack->direction = conIsRequestServicePackage;
	return(0);
}

// ��ʼ����Ӧ����
int UnionInitResponsePackage(PUnionServiceResponsePackage ppack)
{
	UnionInitServicePackage(ppack);
	ppack->direction = conIsResponseServicePackage;
	return(0);
}

// �������д����־
void UnionLogRequestPackage(PUnionServiceRequestPackage ppack)
{
	char	title[100];
	
	if (ppack == NULL)
		return;
	sprintf(title,"requestPackage:: idOfApp=[%s] serivceID=[%s] direction=[%d]",ppack->idOfApp,ppack->serviceID,ppack->direction);
	UnionLogCmmPackData(title,&(ppack->datagram));
}	

// ����Ӧ��д����־
void UnionLogResponsePackage(PUnionServiceResponsePackage ppack)
{
	char	title[100];
	
	if (ppack == NULL)
		return;
	sprintf(title,"responsePackage:: idOfApp=[%s] serivceID=[%s] direction=[%d] resCode=[%d]",ppack->idOfApp,ppack->serviceID,ppack->direction,ppack->resCode);
	UnionLogCmmPackData(title,&(ppack->datagram));
}	

// ��һ�������
int UnionPackRequestPackage(char *idOfApp,int serviceCode,PUnionCmmPackData pdatagram,char *buf,int sizeOfBuf)
{
	int				ret;
	TUnionServiceRequestPackage	ppack;
	
	UnionInitServicePackage(&ppack);
	if ((ret = UnionSetServicePackageHeader(idOfApp,serviceCode,conIsRequestServicePackage,0,&ppack)) < 0)
	{
		UnionUserErrLog("in UnionPackRequestPackage:: UnionSetServicePackageHeader!\n");
		return(ret);
	}
	if ((ret = UnionSetServicePackageData(pdatagram,&ppack)) < 0)
	{
		UnionUserErrLog("in UnionPackRequestPackage:: UnionSetServicePackageData!\n");
		return(ret);
	}
	UnionLogRequestPackage(&ppack);
	if ((ret = UnionPackPackage(&ppack,buf,sizeOfBuf)) < 0)
	{
		UnionUserErrLog("in UnionPackRequestPackage:: UnionPackPackage!\n");
		return(ret);
	}
	return(ret);
}

// ��һ�������
int UnionUnpackRequestPackage(char *data,int lenOfData,char *idOfApp,int *serviceID,PUnionCmmPackData pdatagram)
{
	int				ret;
	TUnionServiceRequestPackage	ppack;
	
	UnionInitServicePackage(&ppack);
	if ((ret = UnionUnpackPackage(data,lenOfData,&ppack)) < 0)
	{
		UnionUserErrLog("in UnionUnpackRequestPackage:: UnionUnpackPackage!\n");
		return(ret);
	}
	UnionLogRequestPackage(&ppack);
	if (ppack.direction != conIsRequestServicePackage)
	{
		UnionUserErrLog("in UnionUnpackRequestPackage:: this is not a request package!\n");
		return(errCodeAPIPackageNotRequest);
	}
	if (idOfApp != NULL)
		strcpy(idOfApp,ppack.idOfApp);
	if (serviceID != NULL)
		*serviceID = atoi(ppack.serviceID);
	if (pdatagram != NULL)
		//memcpy(pdatagram,&(ppack.datagram),sizeof(*pdatagram));
		return(UnionCopyPackFldDataList(&(ppack.datagram),pdatagram));
	return(0);
}

// ��һ����Ӧ��
int UnionPackResponsePackage(char *idOfApp,int serviceCode,int resCode,PUnionCmmPackData pdatagram,char *buf,int sizeOfBuf)
{
	int				ret;
	TUnionServiceResponsePackage	ppack;
	
	UnionInitServicePackage(&ppack);
	if ((ret = UnionSetServicePackageHeader(idOfApp,serviceCode,conIsResponseServicePackage,resCode,&ppack)) < 0)
	{
		UnionUserErrLog("in UnionPackResponsePackage:: UnionSetServicePackageHeader!\n");
		return(ret);
	}
	if ((ret = UnionSetServicePackageData(pdatagram,&ppack)) < 0)
	{
		UnionUserErrLog("in UnionPackResponsePackage:: UnionSetServicePackageData!\n");
		return(ret);
	}
	UnionLogResponsePackage(&ppack);
	if ((ret = UnionPackPackage(&ppack,buf,sizeOfBuf)) < 0)
	{
		UnionUserErrLog("in UnionPackResponsePackage:: UnionPackPackage!\n");
		return(ret);
	}
	return(ret);
}

// ��һ����Ӧ��
int UnionUnpackResponsePackage(char *data,int lenOfData,char *idOfApp,int *serviceID,int *resCode,PUnionCmmPackData pdatagram)
{
	int				ret;
	TUnionServiceResponsePackage	ppack;
	
	UnionInitServicePackage(&ppack);
	if ((ret = UnionUnpackPackage(data,lenOfData,&ppack)) < 0)
	{
		UnionUserErrLog("in UnionUnpackResponsePackage:: UnionUnpackPackage!\n");
		return(ret);
	}
	UnionLogResponsePackage(&ppack);
	if (ppack.direction != conIsResponseServicePackage)
	{
		UnionUserErrLog("in UnionUnpackResponsePackage:: this is not a response package!\n");
		return(errCodeAPIPackageNotResponse);
	}
	if (idOfApp != NULL)
		strcpy(idOfApp,ppack.idOfApp);
	if (serviceID != NULL)
		*serviceID = atoi(ppack.serviceID);
	if (resCode != NULL)
		*resCode = ppack.resCode;
	if (pdatagram != NULL)
		//memcpy(pdatagram,&(ppack.datagram),sizeof(*pdatagram));
		return(UnionCopyPackFldDataList(&(ppack.datagram),pdatagram));
	return(0);
}
