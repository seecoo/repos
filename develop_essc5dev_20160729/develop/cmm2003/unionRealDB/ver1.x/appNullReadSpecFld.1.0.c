//	Author: Wolfgang Wang
//	Date: 2008-12-3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionREC.h"

#include "unionErrCode.h"

int UnionReadSpecFldDefinedBySpecApp(char *fldName, char *fldValue, int sizeOfFldValue)
{
	UnionUserErrLog("in UnionReadSpecFldDefinedBySpecApp:: fldName[%s] not found!\n",fldName);
	return(UnionSetUserDefinedErrorCode(errCodeEsscMDL_FldNotExists));
}
