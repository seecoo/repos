#include <stdlib.h>

#include "UnionStr.h"
#include "unionRecFile.h"
#include "unionErrCode.h"
#include "UnionLog.h"			// ʹ��3.x�汾
#include "unionWorkingDir.h"
#include "unionResID.h"
#include "unionREC.h"

#include "unionComplexDBCommon.h"
#include "unionComplexDBObject.h"
#include "unionComplexDBRecord.h"
#include "unionComplexDBObjectDef.h"

#include "unionDatabaseCmd.h"

#include "unionTBLData.h"

// ��ȡ TBL �����ļ���
int UnionGetTBLDataFileName(char *fileName, char *resName)
{
	char		mainDir[512];
	char		*ptr;
	
	if ((NULL == fileName) || (NULL == resName))
	{
		UnionUserErrLog("in UnionGetTBLDataFileName, bad parameter.\n");
		return errCodeParameter;
	}
	
	memset(mainDir, 0, sizeof(mainDir));
#ifdef _WIN32
	UnionGetMainWorkingDir(mainDir);
	strcat(mainDir, "\\tmp");
#else
	if ((ptr = getenv("UNIONTEMP")) != NULL)
	{
		strcpy(mainDir, ptr);
	}
	else
	{
		UnionNullLog("in UnionGetTBLDataFileName, the env UNIONTEMP not defined.\n");
	}
#endif
	return sprintf(fileName, "%s/%s.txt", mainDir, resName);
}

/* 
���ܣ�
	��complexDB�е�������
����:
	[IN]:
		resName			: ������[����]
	[OUT]:
		NULL
����ֵ:
	[0, +d)				: �ɹ�, �������ݼ�¼������
	(-d, 0)				: ʧ��
*/
int UnionOutputTBLData(char *resName)
{
	int		ret;
	char		tblFileName[512];
	
	if (NULL == resName)
	{
		UnionUserErrLog("in UnionOutputTBLData, bad parameter.\n");
		return errCodeParameter;
	}
	
	memset(tblFileName, 0, sizeof(tblFileName));
	if ((ret = UnionGetTBLDataFileName(tblFileName, resName)) < 0)
	{
		UnionUserErrLog("in UnionOutputTBLData:: UnionGetTBLDataFileName error, resName: [%s], ret: [%d].\n", resName, ret);
		return ret;
	}
	
	if ((ret = UnionBatchSelectUnionObjectRecord(resName, "", tblFileName)) < 0)
	{
		UnionUserErrLog("in UnionOutputTBLData:: UnionBatchSelectUnionObjectRecord error, resName: [%s], reqStr: [], tblFileName: [%s], ret: [%d].\n", resName, tblFileName);
		return ret;
	}
	
	return ret;
}

