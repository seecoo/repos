#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _UnionEnv_3_x_
#define _UnionEnv_3_x_
#endif

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

#include "UnionEnv.h"
#include "unionDatabaseCmd.h"

#include "unionTBLData.h"

// ��env����ת��ΪcomplexDB����
int unionTranslateEnvDataToComplexDBData(char *desStr, int sizeOfDesStr, char *oriStr, int lenOfOriStr, char *idOfObject)
{
	TUnionObject		sObject;
	PUnionEnviVariable 	pvar;
	char			caValue[512];
	int			ret, iRet;
	int			i, offset;
	
	if ((NULL == desStr) || (NULL == oriStr) || (0 >= sizeOfDesStr) || (0 >= lenOfOriStr))
	{
		UnionUserErrLog("in unionTranslateEnvDataToComplexDBData, bad parameter.\n");
		return errCodeParameter;
	}
	
	// ��ȡ������
	memset(&sObject,0,sizeof(TUnionObject));
	iRet=UnionReadObjectDef(idOfObject,&sObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in unionTranslateEnvDataToComplexDBData:: UnionReadObjectDef fail! return=[%d]\n",iRet);
		return(iRet);
	}
	
	if ((pvar = ReadUnionEnviVarFromStr(oriStr)) == NULL)
	{
		UnionUserErrLog("in unionTranslateEnvDataToComplexDBData:: ReadUnionEnviVarFromStr, read env var from: [%s] error.\n", oriStr);
		return errCodeEnviMDL_NullLine;
	}
	
	for (i = 0, offset = 0; i < sObject.fldNum; i++)
	{
		if (NULL == pvar->pVariableValue[i])
		{
			UnionUserErrLog("in unionTranslateEnvDataToComplexDBData, the fld: [%s]'s value is null, please check the data is you need.\n", sObject.fldDefGrp[i].name);
			break;
		}
		
		memset(caValue, 0, sizeof(caValue));
		strcpy(caValue, (char *)pvar->pVariableValue[i]);
		if ((ret = UnionPutRecFldIntoRecStr(sObject.fldDefGrp[i].name, caValue, strlen(caValue), desStr + offset, sizeOfDesStr - offset)) < 0)
		{
			UnionUserErrLog("in unionTranslateEnvDataToComplexDBData:: UnionPutRecFldIntoRecStr error, ret: [%d].\n", ret);
			UnionFreeEnviVar(pvar);
			return ret;
		}
		offset	+= ret;
	}
	UnionFreeEnviVar(pvar);
	
	return offset;
}

/* 
���ܣ�
	�� tblFileName �ļ���,�����ݵ��� resName ��
����:
	[IN]:
		tblFileName		: ��Ҫ���������ݵ��ļ���,Ҫ��Ϊȫ·���ļ���
		resName			: ������[����]
		isEnvData		: �Ƿ�Ϊ env ����
	[OUT]:
		NULL
����ֵ:
	[0, +d)				: �ɹ�, �������ݼ�¼������
	(-d, 0)				: ʧ��
*/
int UnionInputTBLData(char *tblFileName, char *resName, int isEnvData)
{
	char			recStr[2048 + 1];
	char			desStr[2048 + 1];
	int			lenOfRecStr;
	int 			ret, nums, len;
	TUnionRecFileHDL        hdl;
	
	if ((NULL == tblFileName) || (NULL == resName))
	{
		UnionUserErrLog("in UnionInputTBLData, bad parameter.\n");
		return errCodeParameter;
	}
	
	// �ж������ļ��Ƿ����
	if (UnionExistsFile(tblFileName) == 0)
	{
		UnionUserErrLog("in UnionInputTBLData, not find the file: [%s].\n", tblFileName);
		return errCodeParameter;
	}
	
	if ((hdl = UnionOpenRecFileHDL(tblFileName)) == NULL)
	{
		UnionUserErrLog("in UnionInputTBLData, open file: [%s] failed.\n", tblFileName);
		return errCodeUseOSErrCode;
	}
	
	nums			= 0;
	while (1)
	{
		memset(recStr, 0, sizeof(recStr));
                if ((lenOfRecStr = UnionReadNextRecFromFile(hdl,recStr,sizeof(recStr))) == 0)	// ���޼�¼
			break;
		
		if (lenOfRecStr < 0)	// ����
                {
                        UnionCloseRecFileHDL(hdl);
                        return(lenOfRecStr);
                }
                
                memset(desStr, 0, sizeof(desStr));
                if (1 == isEnvData)
                {
                	if ((ret = unionTranslateEnvDataToComplexDBData(desStr, sizeof(desStr), recStr, lenOfRecStr, resName)) < 0)
                	{
                		UnionUserErrLog("in UnionInputTBLData:: unionTranslateEnvDataToComplexDBData error, recStr: [%s], resName: [%s], ret: [%d].\n", recStr, resName, ret);
                		continue;
                	}
                	len	= ret;
                }
                else
                {
                	memcpy(desStr, recStr, lenOfRecStr);
                	len	= lenOfRecStr;
                }
                
                if ((ret = UnionInsertObjectRecord(resName, desStr, len)) < 0)
                {
                	UnionUserErrLog("in UnionInputTBLData:: UnionInsertObjectRecord error, resName: [%s], desStr: [%s], ret: [%d].\n", resName, desStr, ret);
                	continue;
                }
                nums++;
	}
	UnionCloseRecFileHDL(hdl);
	
	return nums;
}

