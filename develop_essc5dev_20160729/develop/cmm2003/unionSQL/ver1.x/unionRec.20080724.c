//---------------------------------------------------------------------------


//#pragma hdrstop

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "UnionStr.h"
#include "unionErrCode.h"
#include "UnionLog.h"

#include "unionRec0.h"
#include "unionCalcuOperator.h"

/*
����	��һ���ַ�����ɼ�¼��,�����ظ�����һ����
�������
	recStr		��¼��
	lenOfRecStr	��¼������
�������
	prec		�����ļ�¼
����ֵ
	>=0		��¼����������Ŀ
	<0		������
*/
int UnionReadRecFromRecStrRepeatPermitted(char *recStr,int lenOfRecStr,PUnionRec prec)
{
	int	lenOfFldValue,lenOfFldName;
	int	offset = 0;
	int	thisFldIsError = 0;
	
	if ((recStr == NULL) || (lenOfRecStr < 0) || (prec == NULL))
	{
		UnionUserErrLog("in UnionReadRecFromRecStrRepeatPermitted:: null pointer or lenOfRecStr = [%d]!\n",lenOfRecStr);
		return(errCodeParameter);
	}
	memset(prec,0,sizeof(*prec));
	prec->fldNum = 0;
	if (lenOfRecStr == 0)
	{
		return(prec->fldNum);
	}

	for (;;)
	{
		if (prec->fldNum == conMaxNumOfFldPerRec)	// ��¼������
			return(prec->fldNum);
		if (offset >= lenOfRecStr)			// ��������
			return(prec->fldNum);
		// ��������
		thisFldIsError = 0;
		for (lenOfFldName = 0;;)
		{
			if (offset >= lenOfRecStr)	// ���Ѷ���
				return(prec->fldNum);
			if (recStr[offset] == '=')	// 
			{
				offset++;
				break;	// �����ƶ�������
			}
			if (((recStr[offset] >= 'a') && (recStr[offset] <= 'z')) || 
				((recStr[offset] >= '0') && (recStr[offset] <= '9')) ||
				((recStr[offset] >= 'A') && (recStr[offset] <= 'Z')) ||
                                (recStr[offset] == '.') || (recStr[offset] == '[') ||
                                (recStr[offset] == ']'))
			{
				if (lenOfFldName >= sizeof(prec->fldName[prec->fldNum]) - 1)
				{
					UnionUserErrLog("in UnionReadRecFromRecStrRepeatPermitted:: the fldName of fldIndex [%d] is too long!\n",prec->fldNum);
					return(errCodeSQLRecFldNameTooLong);
				}
				prec->fldName[prec->fldNum][lenOfFldName] = recStr[offset];
				lenOfFldName++;
				offset++;
				continue;
			}
			else
			{
				if (recStr[offset] == '|')
				{
					thisFldIsError = 1;
					offset++;
					break;
				}

                                UnionUserErrLog("in UnionReadRecFromRecStrRepeatPermitted the invalid sign is: [%c] in [%s]\n", recStr[offset],recStr);
				UnionUserErrLog("in UnionReadRecFromRecStrRepeatPermitted:: invalid fldName of fldIndex [%d] lenOfFldName = [%d] lenOfRecStr = [%d]\n",prec->fldNum,lenOfFldName,lenOfRecStr);
				return(errCodeSQLInvalidFldName);
			}
		}
		if (lenOfFldName == 0)	// δ��������
		{
			UnionUserErrLog("in UnionReadRecFromRecStrRepeatPermitted:: invalid fldName of fldIndex [%d] lenOfFldName = [%d] lenOfRecStr = [%d]\n",prec->fldNum,lenOfFldName,lenOfRecStr);
			//return(errCodeSQLNullFldName);
			continue;
		}
		if (thisFldIsError)
			continue;
		// ����ֵ
		for (lenOfFldValue = 0;;)
		{
			if (offset >= lenOfRecStr)	// ���Ѷ���
				break;
			if (recStr[offset] == '|')	// 
			{
				offset++;
				break;	// ��ֵ�Ѷ���
			}
			if (lenOfFldValue >= sizeof(prec->fldValue[prec->fldNum]) - 1)
				UnionAuditLog("in UnionReadRecFromRecStrRepeatPermitted:: the fldValue of fldIndex [%d] is too long!\n",prec->fldNum);
			else
			{
				prec->fldValue[prec->fldNum][lenOfFldValue] = recStr[offset];
				lenOfFldValue++;
			}
			offset++;
			continue;
		}
		prec->fldNum++;	
	}
}
/*
����	��һ���ַ�����ɼ�¼��
�������
	recStr		��¼��
	lenOfRecStr	��¼������
�������
	prec		�����ļ�¼
����ֵ
	>=0		��¼����������Ŀ
	<0		������
*/
int UnionReadRecFromRecStr(char *recStr,int lenOfRecStr,PUnionRec prec)
{
	int	lenOfFldValue,lenOfFldName;
	int	offset = 0;
	int	thisFldIsError = 0;
	int	index;
	
	if ((recStr == NULL) || (lenOfRecStr < 0) || (prec == NULL))
	{
		UnionUserErrLog("in UnionReadRecFromRecStr:: null pointer or lenOfRecStr = [%d]!\n",lenOfRecStr);
		return(errCodeParameter);
	}
	memset(prec,0,sizeof(*prec));
	prec->fldNum = 0;
	if (lenOfRecStr == 0)
	{
		return(prec->fldNum);
	}

	for (;;)
	{
		if (prec->fldNum == conMaxNumOfFldPerRec)	// ��¼������
			return(prec->fldNum);
		if (offset >= lenOfRecStr)			// ��������
			return(prec->fldNum);
		// ��������
		thisFldIsError = 0;
		for (lenOfFldName = 0;;)
		{
			if (offset >= lenOfRecStr)	// ���Ѷ���
				return(prec->fldNum);
			if (recStr[offset] == '=')	// 
			{
				offset++;
				break;	// �����ƶ�������
			}
			if (((recStr[offset] >= 'a') && (recStr[offset] <= 'z')) || 
				((recStr[offset] >= '0') && (recStr[offset] <= '9')) ||
				((recStr[offset] >= 'A') && (recStr[offset] <= 'Z')) ||
                                (recStr[offset] == '.') || (recStr[offset] == '[') ||
                                (recStr[offset] == ']'))
			{
				if (lenOfFldName >= sizeof(prec->fldName[prec->fldNum]) - 1)
				{
					UnionUserErrLog("in UnionReadRecFromRecStr:: the fldName of fldIndex [%d] is too long!\n",prec->fldNum);
					return(errCodeSQLRecFldNameTooLong);
				}
				prec->fldName[prec->fldNum][lenOfFldName] = recStr[offset];
				lenOfFldName++;
				offset++;
				continue;
			}
			else
			{
				if (recStr[offset] == '|')
				{
					thisFldIsError = 1;
					offset++;
					break;
				}

                                UnionUserErrLog("in UnionReadRecFromRecStr the invalid sign is: [%c] in [%s]\n", recStr[offset],recStr);
				UnionUserErrLog("in UnionReadRecFromRecStr:: invalid fldName of fldIndex [%d] lenOfFldName = [%d] lenOfRecStr = [%d]\n",prec->fldNum,lenOfFldName,lenOfRecStr);
				return(errCodeSQLInvalidFldName);
			}
		}
		if (lenOfFldName == 0)	// δ��������
		{
			UnionUserErrLog("in UnionReadRecFromRecStr:: invalid fldName of fldIndex [%d] lenOfFldName = [%d] lenOfRecStr = [%d]\n",prec->fldNum,lenOfFldName,lenOfRecStr);
			//return(errCodeSQLNullFldName);
			continue;
		}
		if (thisFldIsError)
			continue;
		// ����ֵ
		for (lenOfFldValue = 0;;)
		{
			if (offset >= lenOfRecStr)	// ���Ѷ���
				break;
			if (recStr[offset] == '|')	// 
			{
				offset++;
				break;	// ��ֵ�Ѷ���
			}
			if (lenOfFldValue >= sizeof(prec->fldValue[prec->fldNum]) - 1)
				UnionAuditLog("in UnionReadRecFromRecStr:: the fldValue of fldIndex [%d] is too long!\n",prec->fldNum);
			else
			{
				prec->fldValue[prec->fldNum][lenOfFldValue] = recStr[offset];
				lenOfFldValue++;
			}
			offset++;
			continue;
		}
		// 2009/6/5������������
		for (index = 0; index < prec->fldNum; index++)
		{
			if (strcmp(prec->fldName[index],prec->fldName[prec->fldNum]) == 0)	// ������Ѷ���
				break;
		}
		if (index == prec->fldNum)	// δ����
			prec->fldNum++;
		// 2009/6/5�����������ӽ���
		//prec->fldNum++;	2009/6/5��������ɾ��
	}
}

/*
����	��һ����¼��д�뵽��¼�ַ���
�������
	prec		��¼
	sizeOfRecStr	��¼������Ĵ�С
�������
	recStr		��ɵļ�¼��
����ֵ
	>=0		��¼���ĳ���
	<0		������
*/
int UnionPutRecIntoRecStr(PUnionRec prec,char *recStr,int sizeOfRecStr)
{
	int	lenOfFldValue,lenOfFldName;
	int	offset = 0;
	int	index;
	int	thisFldLen;
	
	if ((recStr == NULL) || (sizeOfRecStr < 0) || (prec == NULL))
	{
		UnionUserErrLog("in UnionPutRecIntoRecStr:: null pointer or sizeOfRecStr = [%d]!\n",sizeOfRecStr);
		return(errCodeParameter);
	}
	for (index = 0; (index < prec->fldNum) && (index < conMaxNumOfFldPerRec); index++)
	{
		lenOfFldValue = strlen(prec->fldValue[index]);
		lenOfFldName = strlen(prec->fldName[index]);
		if ((thisFldLen = lenOfFldName + lenOfFldValue + 2) + offset >= sizeOfRecStr)
		{
			UnionUserErrLog("in UnionPutRecIntoRecStr:: sizeOfRecStr [%d] < expected [%d] when put fldIndex [%d]\n",sizeOfRecStr,thisFldLen + offset,index);
			return(errCodeSmallBuffer);
		}
		sprintf(recStr+offset,"%s=%s|",prec->fldName[index],prec->fldValue[index]);
		offset += thisFldLen;
	}
	if (prec->fldNum > conMaxNumOfFldPerRec)
	{
		UnionAuditLog("in UnionPutRecIntoRecStr:: fldNum [%d] too much, and [%d] fld not put into the recStr!\n",prec->fldNum,prec->fldNum-conMaxNumOfFldPerRec);
	}
	return(offset);
}

/*
����	�Ƚ�һ����¼���Ƿ���������
�������
	recStr		��¼��
	lenOfRecStr	��¼������
	conStr		������
	lenOfConStr	����������
�������
	��
����ֵ
	>0		����
	=0		������
	<0		��������
*/
int UnionIsRecStrFitSpecConditon(char *recStr,int lenOfRecStr,char *conStr,int lenOfConStr)
{
	int		ret;
	TUnionRec	conRec;
	int		index = 0;
	int		compType = conCalcuOperatorAnd;
	
	if ((lenOfConStr == 0) || (conStr == NULL))	// δ�����ѯ����
		return(1);
			
	// ��ȡ��¼������
	memset(&conRec,0,sizeof(conRec));
	if ((ret = UnionReadRecFromRecStr(conStr,lenOfConStr,&conRec)) < 0)
	{
		UnionUserErrLog("in UnionIsRecStrFitSpecConditon:: UnionReadRecFromRecStr [%d] [%s]\n",lenOfConStr,conStr);
		return(ret);
	}
	
	// ��ȡ������
	if (strcmp(conRec.fldName[0],"operator") == 0)
	{
		if ((compType = UnionConvertCalcuOperatorStrTagIntoIntTag(conRec.fldValue[0])) < 0)
		{
			UnionUserErrLog("in UnionIsRecStrFitSpecConditon:: UnionConvertCalcuOperatorStrTagIntoIntTag [%s] ret = [%d]\n",conRec.fldValue[0],compType);
			return(compType);
		}
		index = 1;
	}
	
	switch (compType)
	{
		case	conCalcuOperatorAnd:
			return(UnionIsRecStrFitAndCondition(&conRec,index,recStr,lenOfRecStr));
		case	conCalcuOperatorOr:
			return(UnionIsRecStrFitOrCondition(&conRec,index,recStr,lenOfRecStr));
		case	conCalcuOperatorNot:
			return(UnionIsRecStrFitNotCondition(&conRec,index,recStr,lenOfRecStr));
		default:
			UnionUserErrLog("in UnionIsRecStrFitSpecConditon:: [%s] not valid relation operator!\n",conRec.fldValue[0]);
			return(errCodeCDPMDL_InvalidRelationCalcuOperator);
	}		
}

/*
����	�Ƚ�һ����¼���Ƿ���������������
�������
	pconRec		������
	index		�������ĸ���ʼ�Ƚ�
	recStr		��¼��
	lenOfRecStr	��¼������
�������
	��
����ֵ
	>0		����
	=0		������
	<0		��������
*/
int UnionIsRecStrFitAndCondition(PUnionRec pconRec,int startIndex,char *recStr,int lenOfRecStr)
{
	int		index;
	char		fldValue[1024+1];
	int		lenOfFldValue;
	
	if ((pconRec == NULL) || (recStr == NULL))
		return(errCodeParameter);
		
	for (index = startIndex; index < pconRec->fldNum; index++)
	{
		memset(fldValue,0,sizeof(fldValue));
		if ((lenOfFldValue = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,pconRec->fldName[index],fldValue,sizeof(fldValue))) < 0)
		{
			UnionUserErrLog("in UnionIsRecStrFitAndCondition:: UnionReadRecFldFromRecStr [%s] from [%d] [%s]\n",pconRec->fldName[index],lenOfRecStr,recStr);
			return(lenOfFldValue);
		}
		if (strcmp(fldValue,pconRec->fldValue[index]) != 0)	// �ȽϽ��
			return(0);
	}
	return(1);
}

/*
����	�Ƚ�һ����¼���Ƿ��������л�����
�������
	pconRec		����
	index		�������ĸ���ʼ�Ƚ�
	recStr		��¼��
	lenOfRecStr	��¼������
�������
	��
����ֵ
	>0		����
	=0		������
	<0		��������
*/
int UnionIsRecStrFitOrCondition(PUnionRec pconRec,int startIndex,char *recStr,int lenOfRecStr)
{
	int		index;
	char		fldValue[1024+1];
	int		lenOfFldValue;
	
	if ((pconRec == NULL) || (recStr == NULL))
		return(errCodeParameter);
		
	for (index = startIndex; index < pconRec->fldNum; index++)
	{
		memset(fldValue,0,sizeof(fldValue));
		if ((lenOfFldValue = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,pconRec->fldName[index],fldValue,sizeof(fldValue))) < 0)
		{
			UnionUserErrLog("in UnionIsRecStrFitOrCondition:: UnionReadRecFldFromRecStr [%s] from [%d] [%s]\n",pconRec->fldName[index],lenOfRecStr,recStr);
			return(lenOfFldValue);
		}
		if (strcmp(fldValue,pconRec->fldValue[index]) == 0)	// �ȽϽ��
			return(1);
	}
	return(0);
}

/*
����	�Ƚ�һ����¼���Ƿ��������з�����
�������
	pconRec		����
	index		�������ĸ���ʼ�Ƚ�
	recStr		��¼��
	lenOfRecStr	��¼������
�������
	��
����ֵ
	>0		����
	=0		������
	<0		��������
*/
int UnionIsRecStrFitNotCondition(PUnionRec pconRec,int startIndex,char *recStr,int lenOfRecStr)
{
	int		index;
	char		fldValue[1024+1];
	int		lenOfFldValue;
	
	if ((pconRec == NULL) || (recStr == NULL))
		return(errCodeParameter);
		
	for (index = startIndex; index < pconRec->fldNum; index++)
	{
		memset(fldValue,0,sizeof(fldValue));
		if ((lenOfFldValue = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,pconRec->fldName[index],fldValue,sizeof(fldValue))) < 0)
		{
			UnionUserErrLog("in UnionIsRecStrFitNotCondition:: UnionReadRecFldFromRecStr [%s] from [%d] [%s]\n",pconRec->fldName[index],lenOfRecStr,recStr);
			return(lenOfFldValue);
		}
		if (strcmp(fldValue,pconRec->fldValue[index]) == 0)	// �ȽϽ��
			return(0);
	}
	return(1);
}

//---------------------------------------------------------------------------

//#pragma package(smart_init)
