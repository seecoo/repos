//	Wolfgang Wang
//	2008/7/16

#include <stdio.h>
#include <string.h>
#include <time.h>

#define _UnionEnv_3_x_
#include "UnionEnv.h"

#include "unionLisence.h"

#include "UnionStr.h"
#include "unionREC.h"
#include "UnionDes.h"
#include "UnionMD5.h"
// #include "unionCommand.h"
#include "UnionAlgorithm.h"
#include "UnionLog.h"
#include "unionWorkingDir.h"

// #include "unionModule.h"

char	gunionLiscenceKey1[] = "27365A4AB7A3B8D49A0846EC0615B23F8AA40694BB7426F8";
char	gunionLiscenceKey2[] = "8E5FE09B1DFC466E3912697154FAF515A46B78D49A3BA08E";
//char	gunionLiscenceKey3[] = "83665A4AB7A3B8DE39127169515A46515A46B78D43F8AA40";
char	gunionLiscenceKey3[] = "CBE946F7FEA8028634FB946B38E3FBA402DFF2105B9E57F1";

#ifdef _WIN32
char    gunionCopyrightRegistryVar[128+1] = "";
#endif

#define conProductNameStr	"nameOfMyself"
#define conSerialNumberStr	"serialNumber"
#define conLiscenseCode		"liscenceCode"

// ��ð汾�Ǽ��ļ�������
int UnionGetCopyrightRegistryFileName(char *fileName)
{
#ifndef _WIN32
	UnionGetConfFileNameOfREC(fileName);
	return(0);
#else
	char	mainDir[512];
	memset(mainDir,0,sizeof(mainDir));
	UnionGetMainWorkingDir(mainDir);
	sprintf(fileName,"%s/unionCopyright.conf",mainDir);
	return(errCodeParameter);
#endif
}

char *UnionReadCopyrightRegistryVar(char *varName)
{
#ifndef _WIN32
        return(UnionReadStringTypeRECVar(varName));
#else
        int     ret;
        char    fileName[512+1];
        memset(fileName,0,sizeof(fileName));
        UnionGetCopyrightRegistryFileName(fileName);
        if ((ret = UnionReadEnviVarValueOfName(fileName,varName,2,gunionCopyrightRegistryVar)) < 0)
        {
                UnionUserErrLog("in :: UnionReadEnviVarValueOfName from [%s]\n",fileName);
                return(NULL);
        }
        return(gunionCopyrightRegistryVar);
#endif
}

/*
����
	�����������ݺ����к�����ǰ16λ��Ȩ��
�������
	inputData	�������ݣ����Ϊ��ָ�룬��ȡunionREC.CFG�е�nameOfMyself��ֵ
	serialNumber	���кţ����Ϊ��ָ�룬��ȡunionREC.CFG�е����кŵ�ֵ
	isVerify	�ǲ�����֤
�������
	key		���ɵ���Ȩ��
����ֵ
	>=0		�ɹ������ɵ���Ȩ�볤��
	<0		�������
*/
int UnionGenerateFrontLisenceKey(char *inputData,char *serialNumber,char *key,int isVerify)
{
	char		hexInputData[512+1];
	char		hexSerialNumber[512+1];
	char		buf[100+1];
	int		len;
	char		buf2[48+1];

	if ((inputData == NULL) || (serialNumber == NULL) || (key == NULL))
	{
		UnionUserErrLog("in UnionGenerateFrontLisenceKey:: null parameter!\n");
		return(errCodeParameter);
	}
	if ((len = strlen(inputData)) == 0)
	{
		UnionUserErrLog("in UnionGenerateFrontLisenceKey:: inputData len must > 0!\n");
		return(errCodeParameter);
	}
	// ��������������mac		
	// printf("*** inputData [%04d][%s]\n",len,ptr);
	bcdhex_to_aschex(inputData,len,hexInputData);
	UnionGenerateANSIX99MAC(gunionLiscenceKey1,hexInputData,len*2,buf2);
	buf2[16] = 0;
	if (!isVerify)
		UnionLog("in UnionGenerateFrontLisenceKey::*** middle result = [%s]\n",buf2);
	// �����ɵ�MAC���ݽ���3DES����
	Union3DesEncrypt64BitsText(gunionLiscenceKey1+16,buf2,buf2);
	if (!isVerify)
		UnionLog("in UnionGenerateFrontLisenceKey::*** middle result = [%s]\n",buf2);
	// �����к�����mac		
	if ((len = strlen(serialNumber)) == 0)
	{
		UnionUserErrLog("in UnionGenerateFrontLisenceKey:: conSerialNumberStr len must > 0!\n");
		return(errCodeParameter);
	}
	bcdhex_to_aschex(serialNumber,len,hexSerialNumber);
	UnionGenerateANSIX99MAC(gunionLiscenceKey2,hexSerialNumber,len*2,buf);
	buf[16] = 0;
	if (!isVerify)
		UnionLog("in UnionGenerateFrontLisenceKey::*** middle result = [%s]\n",buf);
	// �����ɵ�MAC���ݽ���3DES����
	Union3DesEncrypt64BitsText(gunionLiscenceKey2+16,buf,buf);
	if (!isVerify)
		UnionLog("in UnionGenerateFrontLisenceKey::*** middle result = [%s]\n",buf);
	
	// �����кź������������ɵĽ�����
	UnionXOR(buf2,buf,16,key);
	key[16] = 0;
	if (!isVerify)
		UnionLog("in UnionGenerateFrontLisenceKey::*** middle result = [%s]\n",key);
	return(16);
}

/*
����
	�����������ݺ����к����ɺ�16λ��Ȩ��
�������
	date1	ά����������
	date2	ֹͣʹ������
	isVerify	�ǲ�����֤
�������
	key		���ɵ���Ȩ��
����ֵ
	>=0		�ɹ������ɵ���Ȩ�볤��
	<0		�������
*/
int UnionGenerateAfterLisenceKey(char *date1,char *date2,char *key,int isVerify)
{
	int		len;
	char		date[16+1];
	char		tmpBuf[16+1];

	if ((date1 == NULL) || (date2 == NULL) || (key == NULL))
	{
		UnionUserErrLog("in UnionGenerateAfterLisenceKey:: null parameter!\n");
		return(errCodeParameter);
	}
	if ((len = strlen(date1)) == 0)
	{
		UnionUserErrLog("in UnionGenerateAfterLisenceKey:: date1 len must > 0!\n");
		return(errCodeParameter);
	}
	if ((len = strlen(date2)) == 0)
	{
		UnionUserErrLog("in UnionGenerateAfterLisenceKey:: date2 len must > 0!\n");
		return(errCodeParameter);
	}
	memset(date,0,sizeof(date));
	sprintf(date,"%s%s",date1,date2);
	// �����ݽ���3DES����
	memset(tmpBuf,0,sizeof(tmpBuf));
	Union3DesEncrypt64BitsText(gunionLiscenceKey3+16,date,tmpBuf);
	if (!isVerify)
		UnionLog("in UnionGenerateAfterLisenceKey::*** middle result = [%s]\n",tmpBuf);

	strcpy(key,tmpBuf);
	key[16] = 0;
	return(16);
}

/*
����
	�����������ݺ����к�����������Ȩ��
�������
	inputData	�������ݣ����Ϊ��ָ�룬��ȡunionREC.CFG�е�nameOfMyself��ֵ
	serialNumber	���кţ����Ϊ��ָ�룬��ȡunionREC.CFG�е����кŵ�ֵ
	date1		ά����������
	date2		ʹ�õ���ʱ��
	isVerify	�ǲ�����֤
�������
	key		���ɵ���Ȩ��
����ֵ
	>=0		�ɹ������ɵ���Ȩ�볤��
	<0		�������
*/
int UnionGenerateFinalLisenceKey(char *inputData,char *serialNumber,char *date1,char *date2,char *key,int isVerify)
{
	int	ret;
	char	frontKey[16+1];
	char	afterKey[16+1];
	
	if ((inputData == NULL) || (serialNumber == NULL) || (date1 == NULL) || (date2 == NULL) || (key == NULL))
	{
		UnionUserErrLog("in UnionGenerateFinalLisenceKey:: null parameter!\n");
		return(errCodeParameter);
	}
	
	memset(frontKey,0,sizeof(frontKey));
	if ((ret = UnionGenerateFrontLisenceKey(inputData,serialNumber,frontKey,isVerify)) < 0)
	{
		UnionUserErrLog("in UnionGenerateFinalLisenceKey:: UnionGenerateFrontLisenceKey!\n");
		return(ret);
	}
	memset(afterKey,0,sizeof(afterKey));
	if ((ret = UnionGenerateAfterLisenceKey(date1,date2,afterKey,isVerify)) < 0)
	{
		UnionUserErrLog("in UnionGenerateFinalLisenceKey:: UnionGenerateAfterLisenceKey!\n");
		return(ret);
	}
	UnionXOR(frontKey,afterKey,16,key);
	key[16] = 0;
	return(16);
}

/*
����
	�����������ݺ����к�����������Ȩ��
�������
	inputData	�������ݣ����Ϊ��ָ�룬��ȡunionREC.CFG�е�nameOfMyself��ֵ
	serialNumber	���кţ����Ϊ��ָ�룬��ȡunionREC.CFG�е����кŵ�ֵ
	date1		ά���������ڣ����Ϊ��ָ�룬��ȡ00000000��ֵ
	date2		ʹ�õ������ڣ����Ϊ��ָ�룬��ȡ00000000��ֵ
	isVerify	�ǲ�����֤
�������
	key		���ɵ���Ȩ��
����ֵ
	>=0		�ɹ������ɵ���Ȩ�볤��
	<0		�������
*/
int UnionGenerateFinalLisenceKeyUseSpecInputData(char *inputData,char *serialNumber,char *date1,char *date2,char *key,int isVerify)
{
	char 		ptrInputData[128+1];
	char 		ptrSerialNumber[128+1];
	char		ptrDate1[8+1];
	char		ptrDate2[8+1];
	int		ret;
	
	memset(ptrInputData,0,sizeof(ptrInputData));
	memset(ptrSerialNumber,0,sizeof(ptrSerialNumber));
	if ((inputData == NULL) || (serialNumber == NULL))
	{
		if ((ret = UnionReadFinalLiscenceInfo(ptrInputData,ptrSerialNumber,NULL)) < 0)
		{
			UnionUserErrLog("in UnionGenerateFinalLisenceKeyUseSpecInputData:: UnionReadFinalLiscenceInfo!\n");
			return(ret);
		}
	}
	// �����������
	if (inputData != NULL)
		strcpy(ptrInputData,inputData);
	// ������к�
	if (serialNumber != NULL)
		strcpy(ptrSerialNumber,serialNumber);
	// ���ά����������
	memset(ptrDate1,0,sizeof(ptrDate1));
	if (date1 == NULL)
		strcpy(ptrDate1,"00000000");
	else
		strcpy(ptrDate1,date1);
	// ���ʹ�õ�������
	memset(ptrDate2,0,sizeof(ptrDate2));
	if (date1 == NULL)
		strcpy(ptrDate2,"00000000");
	else
		strcpy(ptrDate2,date2);
	
	if ((ret = UnionGenerateFinalLisenceKey(ptrInputData,ptrSerialNumber,ptrDate1,ptrDate2,key,isVerify)) < 0)
	{
		UnionUserErrLog("in UnionGenerateFinalLisenceKeyUseSpecInputData:: UnionGenerateFinalLisenceKey!\n");
		return(ret);
	}
	return(ret);
}

/*
����
	�����������ݺ����к�����ǰ16��Ȩ��
�������
	inputData	�������ݣ����Ϊ��ָ�룬��ȡunionREC.CFG�е�nameOfMyself��ֵ
	serialNumber	���кţ����Ϊ��ָ�룬��ȡunionREC.CFG�е����кŵ�ֵ
	isVerify	�ǲ�����֤
�������
	key		���ɵ���Ȩ��
����ֵ
	>=0		�ɹ������ɵ���Ȩ�볤��
	<0		�������
*/
int UnionGenerateFrontLisenceKeyUseSpecInputData(char *inputData,char *serialNumber,char *key,int isVerify)
{
	char 		ptrInputData[128+1];
	char 		ptrSerialNumber[128+1];
	int		ret;
	
	memset(ptrInputData,0,sizeof(ptrInputData));
	memset(ptrSerialNumber,0,sizeof(ptrSerialNumber));
	if ((inputData == NULL) || (serialNumber == NULL))
	{
		if ((ret = UnionReadFinalLiscenceInfo(ptrInputData,ptrSerialNumber,NULL)) < 0)
		{
			UnionUserErrLog("in UnionGenerateFrontLisenceKeyUseSpecInputData:: UnionReadFinalLiscenceInfo!\n");
			return(ret);
		}
	}
	// �����������
	if (inputData != NULL)
		strcpy(ptrInputData,inputData);
	// ������к�
	if (serialNumber != NULL)
		strcpy(ptrSerialNumber,serialNumber);
	if ((ret = UnionGenerateFrontLisenceKey(ptrInputData,ptrSerialNumber,key,isVerify)) < 0)
	{
		UnionUserErrLog("in UnionGenerateFrontLisenceKeyUseSpecInputData:: UnionGenerateFrontLisenceKey!\n");
		return(ret);
	}
	return(ret);
}

/*
����
	�����������ݺ����к��Լ���Ȩ��У����Ȩ��
�������
	inputData	�������ݣ����Ϊ��ָ�룬��ȡunionREC.CFG�е�nameOfMyself��ֵ
	serialNumber	���кţ����Ϊ��ָ�룬��ȡunionREC.CFG�е����кŵ�ֵ
	key		Ҫ��У����Ȩ�룬���Ϊ��ָ�룬��ȡunionREC.CFG�е�liscenceCode
�������
	date1		ά����������
	date2		ʹ�õ�������
����ֵ
	=2		У��ɹ�
	=1		��ά����
	=0		У��ʧ��
	=-1		����ֹʹ������
	<-1		�������
*/
int UnionVerifyFinalLisenceKeyUseSpecInputData(char *inputData,char *serialNumber,char *date1,char *date2,char *key)
{
	char		frontKey[16+1];
	char		afterKey[16+1];
	int		ret;
	char		ptrKey[128+1];
	char		systemDate[8+1];
	char		tmpBuf[16+1];

	memset(frontKey,0,sizeof(frontKey));
	if ((ret = UnionGenerateFrontLisenceKeyUseSpecInputData(inputData,serialNumber,frontKey,1)) < 0)
	{
		UnionUserErrLog("in UnionVerifyFinalLisenceKeyUseSpecInputData:: UnionGenerateFrontLisenceKeyUseSpecInputData!\n");
		return(ret);
	}
	if (key != NULL)

		strcpy(ptrKey,key);
	else
	{
		memset(ptrKey,0,sizeof(ptrKey));
		if ((ret = UnionReadFinalLiscenceInfo(NULL,NULL,ptrKey)) < 0)
		{
			UnionUserErrLog("in UnionVerifyFinalLisenceKeyUseSpecInputData:: UnionReadFinalLiscenceInfo!\n");
			return(ret);
		}
	}
	// ��ǰ16λ��Ȩ���������Ȩ�����õ���16λ��Ȩ��
	memset(afterKey,0,sizeof(afterKey));
	UnionXOR(frontKey,ptrKey,16,afterKey);

	// �����ݽ���3DES���㣬�õ�����
	memset(tmpBuf,0,sizeof(tmpBuf));
	Union3DesDecrypt64BitsText(gunionLiscenceKey3+16,afterKey,tmpBuf);
	
	//UnionProgramerLog("in UnionVerifyFinalLisenceKeyUseSpecInputData:: date1+date2 = [%s]!\n", tmpBuf);

	memset(systemDate,0,sizeof(systemDate));
	UnionGetFullSystemDate(systemDate);
	
	memcpy(date2,tmpBuf+8,8);
	memcpy(date1,tmpBuf,8);

	// ����Ƿ�ʹ�ý�ֹ����
	if (memcmp(date2,"00000000",8) != 0)		// Ϊ8��0�����
	{
		if (!UnionVerifyDateStr(date2))		// ���ڸ�ʽ�Ƿ�
		{
			UnionUserErrLog("in UnionVerifyFinalLisenceKeyUseSpecInputData:: UnionVerifyDateStr useEndDate = [%s]!\n", date2);
			return(0);
		}
			
		if (strcmp(systemDate,date2) > 0)	// ��ǰ���ڴ��ڽ�ֹʹ������
			return(-1);
	}

	// ����Ƿ�ά������
	if (memcmp(date1,"00000000",8) != 0)		// Ϊ8��0�����
	{
		if (!UnionVerifyDateStr(date1))		// ���ڸ�ʽ�Ƿ�
		{
			UnionUserErrLog("in UnionVerifyFinalLisenceKeyUseSpecInputData:: UnionVerifyDateStr maintEnddate = [%s]!\n", date1);
			return(0);
		}
		if (strcmp(systemDate,date2) > 0)	// ��ǰ���ڴ���ά������
			return(1);
	}

	return(2);	
	/*
	//UnionLog("[%s] [%s]\n",localKey,ptrKey);
	if (strcmp(ptrKey,localKey) == 0)
		return(1);
	else
		return(0);
	*/
}

/*
����
	�����Ʒ���ݺ����к��Լ���Ȩ��
�������
	inputData	��������
	serialNumber	���к�
	key		Ҫ��У����Ȩ��
�������
	date1		ά����������
	date2		ʹ�õ�������
����ֵ
	>0		У��ɹ�
	=0		У��ʧ��
	<0		�������
*/
int UnionInputFinalLisenceKeyUseSpecInputData(char *inputData,char *serialNumber,char *date1,char *date2,char *key)
{
	int		ret;
	
	if ((inputData == NULL) || (serialNumber == NULL) || (key == NULL))
	{
		UnionUserErrLog("in UnionInputFinalLisenceKeyUseSpecInputData:: null pointer!\n");
		return(errCodeParameter);
	}
	if ((ret = UnionVerifyFinalLisenceKeyUseSpecInputData(inputData,serialNumber,date1,date2,key)) < 0)
	{
		UnionUserErrLog("in UnionInputFinalLisenceKeyUseSpecInputData:: UnionVerifyFinalLisenceKeyUseSpecInputData!\n");
		return(ret);
	}
	if (ret == 0)
		return(errCodeLiscenceCodeInvalid);

	if ((ret = UnionStoreFinalLisenceInfo(inputData,serialNumber,key)) < 0)
	{
		UnionUserErrLog("in UnionInputFinalLisenceKeyUseSpecInputData:: UnionStoreFinalLisenceInfo!\n");
		return(ret);
	}
	return(ret);
}

/*
����
	�洢��Ʒ���ݺ����к��Լ���Ȩ��
�������
	inputData	��������
	serialNumber	���к�
	key		Ҫ��У����Ȩ��
�������
	��
����ֵ
	>0		У��ɹ�
	=0		У��ʧ��
	<0		�������
*/
int UnionStoreFinalLisenceInfo(char *inputData,char *serialNumber,char *key)
{
	int		ret;
	char		fileName[512];

	if ((inputData == NULL) || (serialNumber == NULL) || (key == NULL))
	{
		UnionUserErrLog("in UnionStoreFinalLisenceInfo:: null pointer!\n");
		return(errCodeParameter);
	}

	memset(fileName,0,sizeof(fileName));
	UnionGetCopyrightRegistryFileName(fileName);
	if (UnionExistEnviVarInFile(fileName,conProductNameStr) > 0)
	{
		if ((ret = UnionUpdateEnviVar(fileName,conProductNameStr,"[string][%s]",inputData)) < 0)
		{
			UnionUserErrLog("in UnionStoreFinalLisenceInfo:: UnionUpdateEnviVar nameOfMyself!\n");
			return(ret);
		}
	}
	else
	{
		if ((ret = UnionInsertEnviVar(fileName,conProductNameStr,"[string][%s]",inputData)) < 0)
		{
			UnionUserErrLog("in UnionStoreFinalLisenceInfo:: UnionInsertRECVar nameOfMyself!\n");
			return(ret);
		}
	}
	if (UnionExistEnviVarInFile(fileName,conLiscenseCode) > 0)
	{
		if ((ret = UnionUpdateEnviVar(fileName,conLiscenseCode,"[string][%s]",key)) < 0)
		{
			UnionUserErrLog("in UnionStoreFinalLisenceInfo:: UnionUpdateEnviVar liscenceCode!\n");
			return(ret);
		}
	}
	else
	{
		if ((ret = UnionInsertEnviVar(fileName,conLiscenseCode,"[string][%s]",key)) < 0)
		{
			UnionUserErrLog("in UnionStoreFinalLisenceInfo:: UnionInsertRECVar liscenceCode!\n");
			return(ret);
		}
	}
	if (UnionExistEnviVarInFile(fileName,conSerialNumberStr) > 0)
	{
		if ((ret = UnionUpdateEnviVar(fileName,conSerialNumberStr,"[string][%s]",serialNumber)) < 0)
		{
			UnionUserErrLog("in UnionStoreFinalLisenceInfo:: UnionUpdateEnviVar conSerialNumberStr!\n");
			return(ret);
		}
	}
	else
	{
		if ((ret = UnionInsertEnviVar(fileName,conSerialNumberStr,"[string][%s]",serialNumber)) < 0)
		{
			UnionUserErrLog("in UnionStoreFinalLisenceInfo:: UnionInsertRECVar conSerialNumberStr!\n");
			return(ret);
		}
	}
	return(0);
}

/*
����
	��ȡ��Ʒ���ݺ����к��Լ���Ȩ��
�������
	��
�������
	inputData	��������
	serialNumber	���к�
	key		Ҫ��У����Ȩ��
����ֵ
	>0		У��ɹ�
	=0		У��ʧ��
	<0		�������
*/
int UnionReadFinalLiscenceInfo(char *inputData,char *serialNumber,char *key)
{
	char		*ptr;

	if (inputData != NULL)
	{
		if ((ptr = UnionReadCopyrightRegistryVar(conProductNameStr)) == NULL)
		{
			UnionUserErrLog("in UnionReadFinalLiscenceInfo:: UnionReadCopyrightRegistryVar nameOfMyself!\n");
			return(errCodeParameter);
		}
		strcpy(inputData,ptr);
	}
	// ������к�
	if (serialNumber != NULL)
	{
		if ((ptr = UnionReadCopyrightRegistryVar(conSerialNumberStr)) == NULL)
		{
			UnionUserErrLog("in UnionReadFinalLiscenceInfo:: UnionReadCopyrightRegistryVar conSerialNumberStr!\n");
			return(errCodeParameter);
		}
		strcpy(serialNumber,ptr);
	}
	// �����Ȩ��
	if (key != NULL)
	{
		if ((ptr = UnionReadCopyrightRegistryVar(conLiscenseCode)) == NULL)
		{
			UnionUserErrLog("in UnionReadFinalLiscenceInfo:: UnionReadCopyrightRegistryVar liscenceCode!\n");
			return(errCodeParameter);
		}
		strcpy(key,ptr);
	}
	return(0);
}

/*
����
	У���Ʒ��Ȩ��Ϣ
�������
	��
�������
	��
����ֵ
	=2		У��ɹ�
	=1		��ά����
	=0		У��ʧ��
	=-1		����ֹʹ������
	<-1		�������
*/
int UnionVerifyFinalLisenceInfo()
{
	char 		inputData[128+1];
	char 		serialNumber[128+1];
	char 		key[128+1];
	char		frontKey[16+1];
	char		afterKey[16+1];
	char		tmpBuf[16+1];
	char		date1[8+1];
	char		date2[8+1];
	char		systemDate[8+1];
	int		ret;
	
	memset(inputData,0,sizeof(inputData));
	memset(serialNumber,0,sizeof(serialNumber));
	memset(key,0,sizeof(key));
	if ((ret = UnionReadFinalLiscenceInfo(inputData,serialNumber,key)) < 0)
	{
		UnionUserErrLog("in UnionVerifyFinalLisenceInfo:: UnionReadFinalLiscenceInfo!\n");
		return(ret);
	}

	memset(frontKey,0,sizeof(frontKey));
	if ((ret = UnionGenerateFrontLisenceKey(inputData,serialNumber,frontKey,1)) < 0)
	{
		UnionUserErrLog("in UnionVerifyFinalLisenceInfo:: UnionGenerateFrontLisenceKey!\n");
		return(ret);
	}

	// ��ǰ16λ��Ȩ���������Ȩ�����õ���16λ��Ȩ��
	memset(afterKey,0,sizeof(afterKey));
	UnionXOR(frontKey,key,16,afterKey);

	// �����ݽ���3DES���㣬�õ�����
	memset(tmpBuf,0,sizeof(tmpBuf));
	Union3DesDecrypt64BitsText(gunionLiscenceKey3+16,afterKey,tmpBuf);
	
	memset(systemDate,0,sizeof(systemDate));
	UnionGetFullSystemDate(systemDate);

	// ����Ƿ�ʹ�ý�ֹ����
	memset(date2,0,sizeof(date2));
	memcpy(date2,tmpBuf+8,8);
	if (memcmp(date2,"00000000",8) != 0)		// Ϊ8��0�����
	{
		if (!UnionVerifyDateStr(date2))		// ���ڸ�ʽ�Ƿ�
			return(0);
			
		if (strcmp(systemDate,date2) > 0)	// ��ǰ���ڴ��ڽ�ֹʹ������
			return(-1);
	}

	// ����Ƿ�ά������
	memset(date1,0,sizeof(date1));
	memcpy(date1,tmpBuf,8);
	if (memcmp(date1,"00000000",8) != 0)		// Ϊ8��0�����
	{
		if (!UnionVerifyDateStr(date1))		// ���ڸ�ʽ�Ƿ�
			return(0);
		if (strcmp(systemDate,date2) > 0)	// ��ǰ���ڴ���ά������
			return(1);
	}

	return(2);	
}

/*
����
	�����������ݺ����к�������Ȩ��
�������
	inputData	�������ݣ����Ϊ��ָ�룬��ȡunionREC.CFG�е�nameOfMyself��ֵ
	serialNumber	���кţ����Ϊ��ָ�룬��ȡunionREC.CFG�е����кŵ�ֵ
	isVerify	�ǲ�����֤
�������
	key		���ɵ���Ȩ��
����ֵ
	>=0		�ɹ������ɵ���Ȩ�볤��
	<0		�������
*/
int UnionGenerateLisenceKey(char *inputData,char *serialNumber,char *key,int isVerify)
{
	int	ret;
	
	if ((ret = UnionGenerateFrontLisenceKey(inputData,serialNumber,key,isVerify)) < 0)
	{
		UnionUserErrLog("in UnionGenerateLisenceKey:: UnionGenerateFrontLisenceKey!\n");
		return(ret);
	}
	key[12] = 0;
	return(12);
}

/*
����
	�����������ݺ����к�������Ȩ��
�������
	inputData	�������ݣ����Ϊ��ָ�룬��ȡunionREC.CFG�е�nameOfMyself��ֵ
	serialNumber	���кţ����Ϊ��ָ�룬��ȡunionREC.CFG�е����кŵ�ֵ
	isVerify	�ǲ�����֤
�������
	key		���ɵ���Ȩ��
����ֵ
	>=0		�ɹ������ɵ���Ȩ�볤��
	<0		�������
*/
int UnionGenerateLisenceKeyUseSpecInputData(char *inputData,char *serialNumber,char *key,int isVerify)
{
	int		ret;
	
	if ((ret = UnionGenerateFrontLisenceKeyUseSpecInputData(inputData,serialNumber,key,isVerify)) < 0)
	{
		UnionUserErrLog("in UnionGenerateLisenceKeyUseSpecInputData:: UnionGenerateFrontLisenceKeyUseSpecInputData!\n");
		return(ret);
	}
	key[12] = 0;
	return(12);
}

/*
����
	�����������ݺ����к��Լ���Ȩ��У����Ȩ��
�������
	inputData	�������ݣ����Ϊ��ָ�룬��ȡunionREC.CFG�е�nameOfMyself��ֵ
	serialNumber	���кţ����Ϊ��ָ�룬��ȡunionREC.CFG�е����кŵ�ֵ
	key		Ҫ��У����Ȩ�룬���Ϊ��ָ�룬��ȡunionREC.CFG�е�liscenceCode
�������
	��
����ֵ
	>0		У��ɹ�
	=0		У��ʧ��
	<0		�������
*/
int UnionVerifyLisenceKeyUseSpecInputData(char *inputData,char *serialNumber,char *key)
{
	char		localKey[16+1];
	int		ret;
	char		ptrKey[128+1];

	memset(localKey,0,sizeof(localKey));
	if ((ret = UnionGenerateLisenceKeyUseSpecInputData(inputData,serialNumber,localKey,1)) < 0)
	{
		UnionUserErrLog("in UnionVerifyLisenceKeyUseSpecInputData:: UnionGenerateLisenceKeyUseSpecInputData!\n");
		return(ret);
	}
	if (key != NULL)
		strcpy(ptrKey,key);
	else
	{
		memset(ptrKey,0,sizeof(ptrKey));
		if ((ret = UnionReadLiscenceInfo(NULL,NULL,ptrKey)) < 0)
		{
			UnionUserErrLog("in UnionVerifyLisenceKeyUseSpecInputData:: UnionReadLiscenceInfo!\n");
			return(ret);
		}
	}
	//UnionLog("[%s] [%s]\n",localKey,ptrKey);
	if (strcmp(ptrKey,localKey) == 0)
		return(1);
	else
		return(0);
}

/*
����
	�����Ʒ���ݺ����к��Լ���Ȩ��
�������
	inputData	��������
	serialNumber	���к�
	key		Ҫ��У����Ȩ��
�������
	��
����ֵ
	>0		У��ɹ�
	=0		У��ʧ��
	<0		�������
*/
int UnionInputLisenceKeyUseSpecInputData(char *inputData,char *serialNumber,char *key)
{
	int		ret;
	
	if ((inputData == NULL) || (serialNumber == NULL) || (key == NULL))
	{
		UnionUserErrLog("in UnionInputLisenceKeyUseSpecInputData:: null pointer!\n");
		return(errCodeParameter);
	}
	if ((ret = UnionVerifyLisenceKeyUseSpecInputData(inputData,serialNumber,key)) < 0)
	{
		UnionUserErrLog("in UnionInputLisenceKeyUseSpecInputData:: UnionVerifyLisenceKeyUseSpecInputData!\n");
		return(ret);
	}
	if (ret == 0)
		return(errCodeLiscenceCodeInvalid);

	if ((ret = UnionStoreLisenceInfo(inputData,serialNumber,key)) < 0)
	{
		UnionUserErrLog("in UnionInputLisenceKeyUseSpecInputData:: UnionStoreLisenceInfo!\n");
		return(ret);
	}
	return(ret);
}

/*
����
	�洢��Ʒ���ݺ����к��Լ���Ȩ��
�������
	inputData	��������
	serialNumber	���к�
	key		Ҫ��У����Ȩ��
�������
	��
����ֵ
	>0		У��ɹ�
	=0		У��ʧ��
	<0		�������
*/
int UnionStoreLisenceInfo(char *inputData,char *serialNumber,char *key)
{
	return(UnionStoreFinalLisenceInfo(inputData,serialNumber,key));
}

/*
����
	��ȡ��Ʒ���ݺ����к��Լ���Ȩ��
�������
	��
�������
	inputData	��������
	serialNumber	���к�
	key		Ҫ��У����Ȩ��
����ֵ
	>0		У��ɹ�
	=0		У��ʧ��
	<0		�������
*/
int UnionReadLiscenceInfo(char *inputData,char *serialNumber,char *key)
{
	return(UnionReadFinalLiscenceInfo(inputData,serialNumber,key));
}

/*
����
	У���Ʒ��Ȩ��Ϣ
�������
	��
�������
	��
����ֵ
	>0		У��ɹ�
	=0		У��ʧ��
	<0		�������
*/
int UnionVerifyLisenceInfo()
{
	char		localKey[16+1];
	char 		inputData[128+1];
	char 		serialNumber[128+1];
	char 		key[128+1];
	int		ret;
	
	memset(inputData,0,sizeof(inputData));
	memset(serialNumber,0,sizeof(serialNumber));
	memset(key,0,sizeof(key));

	if ((ret = UnionReadLiscenceInfo(inputData,serialNumber,key)) < 0)
	{
		UnionUserErrLog("in UnionVerifyLisenceInfo:: UnionReadLiscenceInfo!\n");
		return(ret);
	}
	if ((ret = UnionGenerateLisenceKey(inputData,serialNumber,localKey,1)) < 0)
	{
		UnionUserErrLog("in UnionVerifyLisenceInfo:: UnionGenerateLisenceKey!\n");
		return(ret);
	}
	if (strcmp(key,localKey) == 0)
		return(1);
	else
		return(0);
}
