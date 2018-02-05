//	Author:		ÕÅÓÀ¶¨
//	Copyright:	Union Tech. Guangzhou
//	Date:		2012-05-25

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionREC.h"
#include "unionXMLPackage.h"
#include "errCodeTranslater.h"

char 	gunionResponseRemark[128] = "";

void UnionSetResponseRemarkForError(char *fmt,...)
{
	va_list args;
	
	va_start(args,fmt);
	vsnprintf(gunionResponseRemark,sizeof(gunionResponseRemark),fmt,args);
	va_end(args);
	
	return;
}

int UnionSetXMLResponsePackageForError(int resCode)
{
	int		ret;
	unsigned long	useTime;
	char		errRemark[512];
	char		tmpBuf[128];
	
	if ((ret = UnionInitHeadOfResponseXMLPackage()) < 0)
	{
		UnionUserErrLog("in UnionSetXMLResponsePackageForError:: UnionInitHeadOfResponseXMLPackage!\n");
		return(0);
	}
	
	useTime = UnionGetTotalRunningTimeInMacroSeconds();	
	snprintf(tmpBuf,sizeof(tmpBuf),"%ld",useTime);
	UnionSetXMLPackageValue("head/useTime",tmpBuf);

	snprintf(tmpBuf,sizeof(tmpBuf),"%06d",abs(resCode));
	UnionSetResponseXMLPackageValue("head/responseCode",tmpBuf);
		
	if (strlen(gunionResponseRemark) == 0)
	{
		if ((ret = UnionTranslateErrCodeIntoRemark(resCode,errRemark,sizeof(errRemark))) > 0)       // ×ª»»´íÎóÂë
		{
			UnionSetResponseXMLPackageValue("head/responseRemark",errRemark);
		}
	}
	else
	{
		UnionSetResponseXMLPackageValue("head/responseRemark",gunionResponseRemark);
		gunionResponseRemark[0] = 0;
	}

	return(0);
}
