//	Author: Wolfgang Wang
//	Date: 2008-10-23

#include <stdio.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>
#include <vcl.h>
#include <Filectrl.hpp>
#endif

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionRecFile.h"
#include "unionRec0.h"
#include "unionErrCode.h"
#include "unionComplexDBDataTransfer.h"
#include "unionComplexDBRecord.h"

/*
���ܣ�
	�������е����з��������ļ�¼
���������
	resName		��������
	condition	����
	outputFileName	�ļ����ƣ���¼д�뵽���ļ���
���������
	��
����ֵ��
	0��		�����ļ�¼��
	<0��		ʧ�ܣ�������
*/
int UnionOutputAllRecFromSpecTBL(char *resName,char *condition,char *outputFileName)
{
	int			ret;
	int			len;
	char			data[1024+1];

	memset (data, 0, sizeof data);
        len = UnionPutRecFldIntoRecStr("remark","��ʼ������Կ��", 14, data, sizeof(data));
        // ���Ӽ�أ�2009-10-19 xusj
        //UnionSendKeyDBBackuperInfoToTransSpier(len, data);

	// �����м�¼
	if ((ret = UnionBatchSelectObjectRecord(resName,condition,outputFileName)) < 0)
	{
		UnionUserErrLog("in UnionOutputAllRecFromSpecTBL:: UnionBatchSelectObjectRecord [%s]!\n",resName);
		return(ret);
	}

	memset (data, 0, sizeof data);
        if (ret >= 0)
                len = UnionPutRecFldIntoRecStr("remark","������Կ��ʧ��", 14, data,sizeof(data));
        else
                len = UnionPutRecFldIntoRecStr("remark","������Կ��ɹ�", 14, data,sizeof(data));
        // ���Ӽ�أ�2009-10-19 xusj
        //UnionSendKeyDBBackuperInfoToTransSpier(len, data);

	return(ret);
}

/*
���ܣ�
	����¼�������
���������
	resName		��������
	inputFileName	�ļ����ƣ���¼�����ڸ��ļ���
���������
	��
����ֵ��
	0��		����ļ�¼��
	<0��		ʧ�ܣ�������
*/
int UnionInputAllRecIntoSpecTBL(char *resName,char *inputFileName)
{
	int			ret;
	int			recNum = 0;
	FILE			*recFileFp = NULL;
	char			recStr[8192*2+1];
	int			lenOfRecStr;
	int			lineNum = 0;
	int			retryTimes = 0;
	
	// ���ļ�
	if ((recFileFp = fopen(inputFileName,"r")) == NULL)
	{
		UnionUserErrLog("in UnionInputAllRecIntoSpecTBL:: fopen [%s]\n",inputFileName);
		//goto errExit;
		return(0-abs(errno));
	}
	while (!feof(recFileFp))
	{
		memset(recStr,0,sizeof(recStr));
		lineNum++;
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(recFileFp,recStr,sizeof(recStr))) <= 0)
			continue;
		UnionLog("in UnionInputAllRecIntoSpecTBL:: begin input %04dth record [%04d][%s]\n",lineNum,lenOfRecStr,recStr);
		retryTimes = 0;
retryOnceMore:
		retryTimes++;
		if ((ret = UnionInsertObjectRecord(resName,recStr,lenOfRecStr)) < 0)
		{
			if ((ret == errCodeObjectMDL_PrimaryKeyIsRepeat) || (ret == -803) || (ret == -1))
			{
				UnionAuditLog("in UnionInputAllRecIntoSpecTBL:: UnionInsertObjectRecord [%04d][%s] into [%s]\n",lenOfRecStr,recStr,resName);
				continue;
			}
			if (ret == -12899)	// ĳ���ֵ̫��
			{
				//UnionLog("resName = [%s] retryTimes = [%d]\n",resName,retryTimes);
				if ((strcmp(resName,"tableList") == 0) && (retryTimes <= 1))
				{
					UnionAuditLog("in UnionInputAllRecIntoSpecTBL:: value too long,retry!\n");
					UnionDeleteSpecFldOnUnionRec(recStr, lenOfRecStr, "initValueList");
					lenOfRecStr = strlen(recStr);
					UnionDeleteSpecFldOnUnionRec(recStr, lenOfRecStr, "recMenuItemList");
					lenOfRecStr = strlen(recStr);
					goto retryOnceMore;
				}
			}		
			UnionUserErrLog("in UnionInputAllRecIntoSpecTBL:: UnionInsertObjectRecord [%04d][%s] into [%s] ret = [%d]\n",lenOfRecStr,recStr,resName,ret);
			//goto errExit;
		}
		else
			recNum++;
	}
	ret = recNum;
errExit:
	fclose(recFileFp);
	return(ret);
}

