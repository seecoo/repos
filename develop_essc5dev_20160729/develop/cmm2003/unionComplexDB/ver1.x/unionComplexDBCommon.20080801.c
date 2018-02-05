// �������ṩһЩͨ�õĺ���������ģ�����

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionComplexDBObjectFileName.h"
#include "unionComplexDBCommon.h"

// ���ܣ���һ��������ת��Ϊ������������
/*
���������
	type		������
���������
	fldTypeName	����������
����ֵ��
	�ɹ���>=0
	ʧ�ܣ�<0��������
*/
int UnionGetFldTypeName(TUnionObjectFldValueType type,char *fldTypeName)
{
	if (fldTypeName == NULL)
		return(errCodeParameter);
	switch (type)
	{
		case	conObjectFldType_String:
			strcpy(fldTypeName,"char");
			return(0);
		case	conObjectFldType_Int:
			strcpy(fldTypeName,"integer");
			return(0);
		case	conObjectFldType_Double:
			strcpy(fldTypeName,"double");
			return(0);
		case	conObjectFldType_Bit:
			strcpy(fldTypeName,"binary");
			return(0);
		case	conObjectFldType_Bool:
			strcpy(fldTypeName,"bool");
			return(0);
		case	conObjectFldType_Long:
			strcpy(fldTypeName,"long");
			return(0);
		default:
			UnionUserErrLog("in UnionGetFldTypeName:: fldType = [%d] invalid!\n",type);
			return(errCodeParameter);
	}
}

/*
���ܣ�ȥ���ַ������ұߵĶ����ַ�
���������
	Str���ַ���
	lenOfStr���ַ����ĳ���
	ch��Ҫȥ���Ķ����ַ�
���������
	Str��ȥ�������ַ�����ַ���
����ֵ��
	>=0���ɹ����������ַ����ĳ���
	<0��ʧ�ܣ�������
*/
int UnionFilterRightChar(char *Str,int lenOfStr,char ch)
{
	int	i;

	for (i = lenOfStr; i > 0; i--)
	{
		if ((Str[i-1] != ch) && (Str[i-1] != 0x09))
			return(i);
		else
			Str[i-1] = 0;
	}
	return(0);
}

/*
���ܣ�ȥ���ַ�������ߵĶ����ַ�
���������
	Str���ַ���
	lenOfStr���ַ����ĳ���
	ch��Ҫȥ���Ķ����ַ�
���������
	Str��ȥ�������ַ�����ַ���
����ֵ��
	>=0���ɹ����������ַ����ĳ���
	<0��ʧ�ܣ�������
*/
int UnionFilterLeftChar(char *Str,int lenOfStr,char ch)
{
	int	i;
	int	Len;
	int	j;

	for (i = 0,Len = lenOfStr; i < Len; i++)
	{
		if ((Str[i] != ch) && (Str[i] != 0x09))
			break;
	}
	if (i == 0)
		return(Len);

	for (j = 0; j < Len - i; j++)
	{
		Str[j] = Str[i + j];
	}

	Str[Len - i] = 0;

	return(Len - i);
}

int UnionIsDoubleStr(char *str)
{
	int	i,start=0;
	int	len;

	if (strlen(str) == 0)
		return(1);

	if (!UnionIsStringHasDotChar(str))
		return(0);
	
	// Mary add begin, 20081114
	if (str[0] == '-')
		start=1;
	// Mary add end, 20081114
	
	//for (i = 0,len = strlen(str); i < len; i++)		Mary delete, 20081114
	for (i = start,len = strlen(str); i < len; i++)		// Mary add, 20081114
		if (!isdigit(str[i]) && str[i]!='.')
			return(0);
	return(1);
}

// Mary add begin, 20081114
int UnionIsIntStr(char *str)
{
	int	i,start=0;
	int	len;

	if (str[0] == '-')
		start=1;
	
	for (i = start,len = strlen(str); i < len; i++)
		if (!isdigit(str[i]))
			return(0);
	return(1);
}
// Mary add end, 20081114

/*
���ܣ���"��1=��ֵ|��2=��ֵ|��3=��ֵ|��"���ָ�ʽ���У���ֳ�������
���������
	fieldNameStr����ʽ������ʽΪ"��1=��ֵ|��2=��ֵ|��3=��ֵ|��"
	lenOfStr����ʽ���ĳ���
���������
	fldGrp��������ṹ
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionFormFldGrpFromRecordStr(char *recordStr,int lenOfStr,PUnionObjectFldGrp fldGrp)
{
	int	iFldNum=0,i,j,iLen,iIsName=1;
	char	caBuf[1024];
	
	if (recordStr == NULL || lenOfStr <= 0 || fldGrp == NULL)
	{
		UnionUserErrLog("in UnionFormFldGrpFromRecordStr:: parameter is error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	memset(caBuf,0,sizeof(caBuf));
	memcpy(caBuf,recordStr,lenOfStr);
	
	iLen=UnionFilterRightChar(caBuf,lenOfStr,' ');		// ȥ���ұߵĶ���ո�
	iLen=UnionFilterRightChar(caBuf,iLen,'|');		// ȥ���ұߵĶ���'|'
	iLen=UnionFilterLeftChar(caBuf,iLen,' ');		// ȥ����ߵĶ���ո�
	iLen=UnionFilterLeftChar(caBuf,iLen,'|');		// ȥ����ߵĶ���'|'
	
	for (i=0,j=0;i<iLen;i++)
	{
		if (caBuf[i] != '=' && iIsName == 1)
		{
			fldGrp->fldNameGrp[iFldNum][j]=caBuf[i];
			j++;
		}
		if (caBuf[i] == '=')
		{
			iIsName=0;
			iFldNum++;
			j=0;
		}
		if (caBuf[i] == '|')
			iIsName=1;
	}
	fldGrp->fldNum=iFldNum;
	
	return(0);
}

/*
���ܣ���"����1.����2.����3��"���ָ�ʽ���У���ֳ�������
���������
	fieldNameStr����ʽ������ʽΪ"����1.����2.����3��"
	lenOfStr����ʽ���ĳ���
���������
	fldGrp��������ṹ
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionFormFldGrpFromFieldNameStr(char *fieldNameStr,int lenOfStr,PUnionObjectFldGrp fldGrp)
{
	int	iFldNum=0,i,j,iLen;
	char	caBuf[1024];
	
	if (fieldNameStr == NULL || lenOfStr <= 0 || fldGrp == NULL)
	{
		UnionUserErrLog("in UnionFormFldGrpFromFieldNameStr:: parameter is error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}

	memset(caBuf,0,sizeof(caBuf));
	memcpy(caBuf,fieldNameStr,lenOfStr);

	iLen=UnionFilterRightChar(caBuf,lenOfStr,' ');		// ȥ���ұߵĶ���ո�
	iLen=UnionFilterRightChar(caBuf,iLen,SeparableSignOfFieldNameGroup);		// ȥ���ұߵĶ���'.'
	iLen=UnionFilterLeftChar(caBuf,iLen,' ');		// ȥ����ߵĶ���ո�
	iLen=UnionFilterLeftChar(caBuf,iLen,SeparableSignOfFieldNameGroup);		// ȥ����ߵĶ���'.'
	
	for (i=0,j=0;i<iLen;i++)
	{
		if (caBuf[i] != SeparableSignOfFieldNameGroup)
		{
			fldGrp->fldNameGrp[iFldNum][j]=caBuf[i];
			j++;
		}
		if (caBuf[i] == SeparableSignOfFieldNameGroup || i == iLen-1)
		{
                        iFldNum++;
			j=0;
			continue;
		}
	}
	fldGrp->fldNum=iFldNum;
	
	return(0);
}

/*
���ܣ���������ṹƴ�ɸ�ʽ��"����1.����2.����3��"
���������
	fldGrp��������ṹ
���������
	fieldNameStr����ʽ������ʽΪ"����1.����2.����3��"
����ֵ��
	>=0���ɹ������ظ�ʽ���ĳ���
	<0��ʧ�ܣ�������
*/
int UnionFormFieldNameStrFromFldGrp(PUnionObjectFldGrp fldGrp,char *fieldNameStr)
{
	int	i,iLen=0;
	
	if (fldGrp == NULL || fieldNameStr == NULL)
	{
		UnionUserErrLog("in UnionFormFieldNameStrFromFldGrp:: parameter is error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}

	if (fldGrp->fldNum <= 0)
		return(0);
	
	for (i=0;i<fldGrp->fldNum;i++)
	{
		sprintf(&fieldNameStr[iLen],"%s%c",fldGrp->fldNameGrp[i],SeparableSignOfFieldNameGroup);
		iLen += (strlen(fldGrp->fldNameGrp[i])+1);
	}
	
	fieldNameStr[iLen-1] = '\0';

        // UnionLog("In UnionFormFieldNameStrFromFldGrp, fieldNameStr is: [%s], and the (iLen-1) is: [%d].\n", fieldNameStr, iLen-1);
	return(iLen-1);
}

/*
���ܣ���������ṹ�ͼ�¼ֵƴ�ɸ�ʽ��"��ֵ1.��ֵ2.��ֵ3��"
���������
	fldGrp��������ṹ
	record����¼������ʽΪ"��1=��ֵ|��2=��ֵ|��3=��ֵ|��"
	lenOfRecord����¼���ĳ���
���������
	fieldValueStr����ʽ������ʽΪ"��ֵ1.��ֵ2.��ֵ3��"
����ֵ��
	>=0���ɹ������ظ�ʽ��fieldValueStr�ĳ���
	<0��ʧ�ܣ�������
*/
int UnionFormFieldValueStrFromFldGrp(PUnionObjectFldGrp fldGrp,char *record,int lenOfRecord,char *fieldValueStr)
{
	int	i,iRet,iLen=0,iValueLen,iNewValueLen;
	char	caValue[4096],caNewValue[4096];
		
	if (fldGrp == NULL || record == NULL || lenOfRecord <= 0 || fieldValueStr == NULL)
	{
		UnionUserErrLog("in UnionFormFieldValueStrFromFldGrp:: parameter is error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	for (i=0;i<fldGrp->fldNum;i++)
	{
		memset(caValue,0,sizeof(caValue));
		iValueLen=UnionReadRecFldFromRecStr(record,lenOfRecord,fldGrp->fldNameGrp[i],caValue,sizeof(caValue));
		if (iValueLen <= 0)
		{
			UnionUserErrLog("in UnionFormFieldValueStrFromFldGrp:: UnionReadRecFldFromRecStr from [%s] fail,fldGrp->fldNameGrp[i]=[%s]! return=[%d]\n",record,fldGrp->fldNameGrp[i],iValueLen);
			return(iValueLen);
		}
		//sprintf(&fieldValueStr[iLen],"%s%c",caValue,SeparableSignOfFieldValueGroup);	Mary delete, 20081112
		//iLen += (iRet+1);	Mary delete, 20081112
		// Mary add begin, 20081112
		memset(caNewValue,0,sizeof(caNewValue));
		iNewValueLen=UnionTranslateStrIntoRecFldStr(caValue,iValueLen,caNewValue,sizeof(caNewValue));
		if (iNewValueLen <= 0)
		{
			UnionUserErrLog("in UnionFormFieldValueStrFromFldGrp:: UnionTranslateStrIntoRecFldStr for [%s] fail! return=[%d]\n",caValue,iNewValueLen);
			return(iNewValueLen);
		}
		if (iValueLen != iNewValueLen)
			UnionLog("in UnionFormFieldValueStrFromFldGrp:: caValue=[%s] iValueLen=[%d] caNewValue=[%s] iNewValueLen=[%d]\n",caValue,iValueLen,caNewValue,iNewValueLen);
		sprintf(&fieldValueStr[iLen],"%s%c",caNewValue,SeparableSignOfFieldValueGroup);
		iLen += (iNewValueLen+1);
		// Mary add end, 20081112
	}
	
	fieldValueStr[iLen-1] = '\0';
	
	return(iLen-1);
}

/*
���ܣ���������ṹ�ͼ�¼ֵƴ�ɸ�ʽ��"��1=��ֵ|��2=��ֵ|��3=��ֵ|��"
���������
	fldGrp��������ṹ
	record����¼������ʽΪ"��1=��ֵ|��2=��ֵ|��3=��ֵ|��"
	lenOfRecord����¼���ĳ���
	sizeOfRecStr��fieldRecStr�Ĵ洢�ռ��С
���������
	fieldRecStr����ʽ������ʽΪ"��1=��ֵ|��2=��ֵ|��3=��ֵ|��"
����ֵ��
	>=0���ɹ������ظ�ʽ��fieldRecStr�ĳ���
	<0��ʧ�ܣ�������
*/
int UnionFormFieldRecordStrFromFldGrp(PUnionObjectFldGrp fldGrp,char *record,int lenOfRecord,char *fieldRecStr,int sizeOfRecStr)
{
	int	i,iRet,iLen=0;
	char	caValue[512];
	
	if (fldGrp == NULL || record == NULL || lenOfRecord <= 0 || fieldRecStr == NULL || sizeOfRecStr <= 0)
	{
		UnionUserErrLog("in UnionFormFieldRecordStrFromFldGrp:: parameter is error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	for (i=0;i<fldGrp->fldNum;i++)
	{
		memset(caValue,0,sizeof(caValue));
		iRet=UnionReadRecFldFromRecStr(record,lenOfRecord,fldGrp->fldNameGrp[i],caValue,sizeof(caValue));
		if (iRet <= 0)
		{
			UnionUserErrLog("in UnionFormFieldRecordStrFromFldGrp:: UnionReadRecFldFromRecStr fail! return=[%d]\n",iRet);
			return(iRet);
		}
		iRet=UnionPutRecFldIntoRecStr(fldGrp->fldNameGrp[i],caValue,iRet,&fieldRecStr[iLen],sizeOfRecStr-iLen);
		if (iRet <= 0)
		{
			UnionUserErrLog("in UnionFormFieldRecordStrFromFldGrp:: UnionPutRecFldIntoRecStr fail! return=[%d]\n",iRet);
			return(iRet);
		}
		iLen += iRet;
	}
	
	return(iLen);
}

/*
���ܣ���"��1=��ֵ|��2=��ֵ|��"���ָ�ʽ��ƴ��"��ֵ1.��ֵ2.��ֵ3��"�ĸ�ʽ��
���������
	commonStr�����ʽ������ʽΪ"��1=��ֵ|��2=��ֵ|��"
	lenOfStr�����ʽ���ĳ���
���������
	fieldValueStr����ֵ��ʽ������ʽΪ"��ֵ1.��ֵ2.��ֵ3��"
����ֵ��
	>=0���ɹ���������ֵ��ʽ���ĳ���
	<0��ʧ�ܣ�������
*/
int UnionFormValueStrFromCommonStr(char *commonStr,int lenOfStr,char *fieldValueStr)
{
	int	i,j,iRet,iTmpLen,iLen=0,iIsValue=0;
	char	caValue[512],caBuf[2048];
	
	if (commonStr == NULL || lenOfStr <= 0 || fieldValueStr == NULL)
	{
		UnionUserErrLog("in UnionFormValueStrFromCommonStr:: parameter is error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	memset(caBuf,0,sizeof(caBuf));
	memcpy(caBuf,commonStr,lenOfStr);
	
	iTmpLen=UnionFilterRightChar(caBuf,lenOfStr,' ');		// ȥ���ұߵĶ���ո�
	iTmpLen=UnionFilterLeftChar(caBuf,iTmpLen,' ');			// ȥ����ߵĶ���ո�
	iTmpLen=UnionFilterLeftChar(caBuf,iTmpLen,'|');			// ȥ����ߵĶ���'|'
	
	memset(caValue,0,sizeof(caValue));
	for (i=0;i<iTmpLen;i++)
	{
		if (iIsValue && caBuf[i] != '|')
		{
			caValue[j]=caBuf[i];
			j++;
		}
		if (caBuf[i] == '|' || i == iTmpLen-1)
		{
			caValue[j]='\0';
			sprintf(&fieldValueStr[iLen],"%s%c",caValue,SeparableSignOfFieldValueGroup);
			iLen += (j+1);
			j=0;
			iIsValue=0;
			continue;
		}
		if (caBuf[i] == '=')
		{
			j=0;
			iIsValue=1;
			continue;
		}
	}
	
	fieldValueStr[iLen-1] = '\0';
	
	return(iLen-1);
}

/*
���ܣ��ж���ĸ�ֵ�Ƿ�Ϸ�
���������
	fieldDef������
	fieldValue����ֵ
	lenOfValue����ֵ�ĳ���
���������
	��
����ֵ��
	1���Ϸ�
	0�����Ϸ�
	<0��ʧ�ܣ����ش�����
*/
int UnionIsValidObjectFieldValue(TUnionObjectFldDef fieldDef,char *fieldValue,int lenOfValue)
{
	if (fieldDef.nullPermitted == 0 && lenOfValue <= 0)
	{
		UnionUserErrLog("in UnionIsValidObjectFieldValue:: field [%s] is null!\n",fieldDef.name);
		return(0);
	}
	/*
	if (lenOfValue > 0 && lenOfValue > fieldDef.size)
	{
		UnionUserErrLog("in UnionIsValidObjectFieldValue:: lenOfValue[%d] > fieldDef.size[%d]!\n",lenOfValue,fieldDef.size);
		return(0);
	}
	*/
	switch(fieldDef.type)
	{
	case conObjectFldType_Int:
		//if (!UnionIsDigitStr(fieldValue))	Mary delete, 20081114
		if (!UnionIsIntStr(fieldValue))		// Mary add, 20081114
		{
			UnionUserErrLog("in UnionIsValidObjectFieldValue:: integer value [%s] must be digit!\n",fieldValue);
			return(0);
		}
		break;
	case conObjectFldType_Double:
		if (!UnionIsDoubleStr(fieldValue))
		{
			UnionUserErrLog("in UnionIsValidObjectFieldValue:: double value [%s] must be digit and .!\n",fieldValue);
			return(0);
		}
		break;
	/*
	case conObjectFldType_Bool:
		if (lenOfValue != 1 || (fieldValue[0] != '0' && fieldValue[0] != '1'))
		{
			UnionUserErrLog("in UnionIsValidObjectFieldValue:: bool value [%s][%d] must has 0 or 1!\n",fieldValue,lenOfValue);
			return(0);
		}
		break;
	*/
	}
	
	return(1);
}

/*
���ܣ���ʵ���Ĵ洢λ�õõ�ʵ���Ĺؼ���
���������
	storePosition��ʵ���Ĵ洢λ��
	lenOfPosition��storePosition�ĳ���
���������
	primaryKey��ʵ���Ĺؼ���
����ֵ��
	>=0���ɹ������عؼ��ֵĳ���
	<0��ʧ�ܣ����ش�����
*/
int UnionGetRecordPrimaryKeyFromStorePosition(char *storePosition,int lenOfPosition,char *primaryKey)
{
	int	i,iLen=-1,iFlag=0;
	char	caBuf[512];
	
	if (storePosition == NULL || lenOfPosition <= 0 || primaryKey == NULL)
	{
		UnionUserErrLog("in UnionGetRecordPrimaryKeyFromStorePosition:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	memset(caBuf,0,sizeof(caBuf));
	memcpy(caBuf,storePosition,lenOfPosition);
	
	for (i=lenOfPosition;i>=0;i--)
	{
		if (caBuf[i] == '/' && iLen < 0)
		{
			iLen=0;
			iFlag=1;
			caBuf[i]='\0';
			continue;
		}
		if (caBuf[i] == '/' && iLen > 0)
			break;
		if (iFlag == 1)
			iLen++;
	}
	
	memcpy(primaryKey,&caBuf[i+1],iLen);
	return(iLen);
}
