//	Author: Wolfgang Wang
//	Date: 2008-12-3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionREC.h"

#include "unionErrCode.h"

#define conEsscFldMessageFlag			900

int UnionReadSpecFldDefinedBySpecApp(char *fldName, char *fldValue, int sizeOfFldValue)
{
	int ret;
	char *ptr;
	
	if (NULL == fldName || NULL == fldValue || 0 >= sizeOfFldValue)
	{
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	if (strcmp(fldName, "branch") == 0)
	{
		if ((ret = UnionReadEsscRequestPackageFld(conEsscFldMessageFlag,fldValue,sizeOfFldValue))< 0)
		{
			if ((ptr = (char*)UnionReadStringTypeRECVar("defaultBranchNo")) == NULL)
			{
				UnionUserErrLog("in UnionReadSpecFldDefinedBySpecApp, UnionReadRecFldFromRecStr branch error, and no var defaultBranchNo defined in unionREC.\n");

				return(ret);
			}
			else
			{
				strcpy(fldValue, ptr);
				UnionLog("in UnionReadSpecFldDefinedBySpecApp, fldName[%s], fldValue[%s].\n", fldName, fldValue);
				return strlen(fldValue);
			}
		}
		else
		{
			return(ret);
		}
	}
	else
		return(UnionSetUserDefinedErrorCode(errCodeEsscMDL_FldNotExists));
}
