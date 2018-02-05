//	Author: ������
//	Date: 2008-11-29

// ������ѧ�����

#include <stdio.h>
#include <string.h>

#include "unionCalcuOperator.h"
#include "unionErrCode.h"
#include "UnionLog.h"
#include "UnionStr.h"

/*��
����
	��һ���������ת��Ϊ�ڲ���ʶ
�������
	specOperatorTag	�����
�������
	��
����ֵ
	>=0		�ڲ���ʾ�������
	<0		�������
*/
int UnionConvertCalcuOperatorStrTagIntoIntTag(char *specOperatorTag)
{
	char	operatorTag[128+1];
	
	strcpy(operatorTag,specOperatorTag);
	UnionToUpperCase(operatorTag);
	if (strcmp(conCalcuOperatorTagNot,operatorTag) == 0)
		return(conCalcuOperatorNot);
	else if (strcmp(conCalcuOperatorTagNotOfCProgram,operatorTag) == 0)
		return(conCalcuOperatorNot);
	if (strcmp(conCalcuOperatorTagAnd,operatorTag) == 0)
		return(conCalcuOperatorAnd);
	else if (strcmp(conCalcuOperatorTagAndOfCProgram,operatorTag) == 0)
		return(conCalcuOperatorAnd);
	if (strcmp(conCalcuOperatorTagOr,operatorTag) == 0)
		return(conCalcuOperatorOr);
	else if (strcmp(conCalcuOperatorTagOrOfCProgram,operatorTag) == 0)
		return(conCalcuOperatorOr);
	else
	{
		UnionUserErrLog("in UnionConvertCalcuOperatorStrTagIntoIntTag:: invalid operator [%s]\n",specOperatorTag);
		return(errCodeCDPMDL_InvalidCalcuOperator);
	}
}

/*
����	��һ����ϵ����ת��Ϊ��ϵ����
�������
	compType	��ϵ����
�������
	compTag		��ϵ����
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionConvertSpecRelationCompTypeIntoDBRelationCompTag(int compType,char *compTag)
{
	switch (compType)
	{
		case	conCalcuOperatorAnd:
			strcpy(compTag," and ");
			break;
		case	conCalcuOperatorOr:
			strcpy(compTag," or ");
			break;
		case	conCalcuOperatorNot:
			strcpy(compTag," and ");
			break;
		default:
			strcpy(compTag," and ");
			break;
	}		
	return(strlen(compTag));	
}

/*
����	��һ����ϵ����ת��Ϊ��ֵ����
�������
	compType	��ϵ����
�������
	compTag		��ϵ����
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionConvertSpecRelationCompTypeIntoDBValueAssignTag(int compType,char *compTag)
{
	switch (compType)
	{
		case	conCalcuOperatorAnd:
			strcpy(compTag,"=");
			break;
		case	conCalcuOperatorOr:
			strcpy(compTag,"=");
			break;
		case	conCalcuOperatorNot:
			strcpy(compTag,"<>");
			break;
		default:
			strcpy(compTag,"=");
			break;
	}
	return(strlen(compTag));	
}

