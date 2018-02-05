// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2006/08/08
// Version:	1.0

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "unionErrCode.h"
#include "unionResID.h"
//#include "unionTableList.h"
//#include "unionOperationList.h"
#include "unionGenIncFromTBL.h"
#include "unionGenTBLDefFromTBL.h"
#include "mngSvrCommProtocol.h"
#include "unionMngSvrOperationType.h"
#include "unionGenSQLFromTBLDefList.h"
#include "unionComplexDBDataTransfer.h"
#include "unionMenuItemType.h"
#include "UnionLog.h"

int UnionExcuteUnionDepartmentManagementSoftwareService(char *tellerTypeID,int handle,int resID,int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	UnionAuditLog("in UnionExcuteDBTableDesignService:: invalid resID [%d]\n",resID);
	return(errCodeEsscMDL_InvalidService);
}

