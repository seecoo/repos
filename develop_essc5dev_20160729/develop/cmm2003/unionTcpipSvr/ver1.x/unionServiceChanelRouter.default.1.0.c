// Author:	Wolfgang Wang
// Date:	2011/1/23

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "unionErrCode.h"
#include "UnionStr.h"
#include "unionREC.h"
#include "UnionLog.h"

#include "unionServiceChanelRouter.h"

/*
����
	��ȡ����ͨ����ʶ
�������
	reqStr		��������
	lenOfReqStr	���󴮳���
�������
	��
����ֵ
	>=0		����ͨ���ı�ʶ
	<0		�������
*/
long UnionGetServiceChanelID(char *reqStr,int lenOfReqStr)
{
	char	varName[40+1];
	int	appIDLen = 2;
	int	serviceIDLen = 3;

#ifdef _appLenIs3Chars_
	appIDLen = 3;
#endif
	if (lenOfReqStr < appIDLen+serviceIDLen)
	{
		UnionUserErrLog("in UnionGetServiceChanelID:: lenOfReqStr = [%d] error!\n",lenOfReqStr);
		return(UnionSetUserDefinedErrorCode(errCodeAPIClientReqLen));
	}
	
	memset(varName,0,sizeof(varName));
	strcpy(varName,"serviceChanelIDOf");
	memcpy(varName+17,reqStr,appIDLen+serviceIDLen);
	return(UnionReadLongTypeRECVar(varName));
}

