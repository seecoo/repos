// Author:	Wolfgang Wang
// Date:	2011/1/23

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "unionErrCode.h"
#include "UnionStr.h"
#include "unionREC.h"
#include "UnionLog.h"

#include "externalPack-InternalPack-Trans.h"

/*
����
	���ⲿ������ת��Ϊ�ڲ�������
�������
	exteranlReqPack		�ⲿ������
	lenOfExternalReqPack	�ⲿ�����ĳ���
	sizeOfInteranlReqPack	�ڲ������Ļ����С
�������
	interanlReqPack		�ڲ�������
����ֵ
	>=0			�ڲ������ĳ���
	<0			�������
*/
int UnionTransExternalReqPackIntoInternalReqPack(unsigned char *exteranlReqPack,int lenOfExternalReqPack,
		unsigned char *interanlReqPack,int sizeOfInteranlReqPack)
{
	if (sizeOfInteranlReqPack < lenOfExternalReqPack)
	{
		UnionUserErrLog("in UnionTransExternalReqPackIntoInternalReqPack:: sizeOfInteranlReqPack [%d] < expected [%d]!\n",
			sizeOfInteranlReqPack,lenOfExternalReqPack);
		return(UnionSetUserDefinedErrorCode(errCodeSmallBuffer));
	}
	if ((exteranlReqPack == NULL) || (interanlReqPack == NULL) || (lenOfExternalReqPack < 0))
	{
		UnionUserErrLog("in UnionTransExternalReqPackIntoInternalReqPack:: error parameter exteranlReqPack interanlReqPack = [%x] = [%x] lenOfExternalReqPack = [%d]!\n",
			 exteranlReqPack,interanlReqPack,lenOfExternalReqPack);
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	memcpy(interanlReqPack,exteranlReqPack,lenOfExternalReqPack);
	return(lenOfExternalReqPack);
}

/*
����
	���ڲ���Ӧ����ת��Ϊ�ⲿ��Ӧ����
�������
	interanlResPack		�ڲ���Ӧ����
	lenOfInternalResPack	�ڲ���Ӧ���ĳ���
	sizeOfExteranlResPack	�ⲿ��Ӧ���Ļ����С
�������
	exteranlResPack		�ⲿ��Ӧ����
����ֵ
	>=0			�ⲿ��Ӧ���ĳ���
	<0			�������
*/
int UnionTransInternalResPackIntoExternalResPack(unsigned char *interanlResPack,int lenOfInternalResPack,unsigned char *exteranlResPack,
		int sizeOfExteranlResPack)
{
	if (sizeOfExteranlResPack < lenOfInternalResPack)
	{
		UnionUserErrLog("in UnionTransInternalResPackIntoExternalResPack:: sizeOfExteranlResPack [%d] < expected [%d]!\n",
			sizeOfExteranlResPack,lenOfInternalResPack);
		return(UnionSetUserDefinedErrorCode(errCodeSmallBuffer));
	}
	if ((exteranlResPack == NULL) || (interanlResPack == NULL) || (lenOfInternalResPack < 0))
	{
		UnionUserErrLog("in UnionTransInternalResPackIntoExternalResPack:: error parameter exteranlResPack interanlResPack = [%x] = [%x] lenOfInternalResPack = [%d]!\n",
			 exteranlResPack,interanlResPack,lenOfInternalResPack);
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	memcpy(exteranlResPack,interanlResPack,lenOfInternalResPack);
	return(lenOfInternalResPack);
}
