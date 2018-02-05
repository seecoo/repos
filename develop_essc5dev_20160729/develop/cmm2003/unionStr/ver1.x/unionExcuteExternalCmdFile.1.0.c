// Wolfgang Wang
// 2010-8-19

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionExcuteExternalCmdFile.h"

/*
����
	ִ��һ�������ļ�
�������
	fileName	�����ļ�����
	sizeOfResStr	��Ӧ���Ĵ�С
�������
	resStr		ִ�������ļ�֮��ƴ�ɵ���Ӧ��
����ֵ
	>=0		��Ӧ���ĳ���
	<0		�������
*/
int UnionExecuteExternalCmdDefinedInSpecFile(char *fileName,char *resStr,int sizeOfResStr)
{
	FILE					*fp;
	char					cmdStr[2048+1];
	char					resDefStr[512+1];
	int					lenOfCmdReqStr;
	int					lenOfCmdResStr;
	int					lenOfResDefStr;
	int					ret;
	int					resOffset = 0;
	PUnionExternalCmdFun			pexternCmdFun;
	int					externalCmdDllConnected = 0;
#ifdef _WIN32
	HINSTANCE				externalCmdDll;
#endif
	
	// �������ļ�
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionUserErrLog("in UnionExecuteExternalCmdDefinedInSpecFile:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	
	while (!feof(fp))
	{
		// ���ļ��ж�ȡһ��ָ������
		memset(cmdStr,0,sizeof(cmdStr));
		lenOfCmdReqStr = UnionReadOneLineFromTxtFile(fp,cmdStr,sizeof(cmdStr));
		if (strncmp(cmdStr,"dllName=",8) == 0) // �Ƕ�̬���ʶ
		{
			UnionLog("dllName=%s",cmdStr+8);
#ifdef _WIN32
			// ���ض�̬��
			if((externalCmdDll = LoadLibrary(cmdStr+8)) == NULL)
			{
				UnionUserErrLog("in UnionExecuteExternalCmdDefinedInSpecFile:: LoadLibrary [%s]!\n",cmdStr+8);
				UnionNotice("LoadLibrary,����̬[%s]װ��ʧ��",cmdStr+8);
				ret = -1;
				goto errExit;
			}
			externalCmdDllConnected = 1;
			// ��ȡ�ⲿ�������IP��ַ
			if ((pexternCmdFun = (PUnionExternalCmdFun)GetProcAddress(externalCmdDll,conMngSvrClientExternalCmdFunName)) == NULL)
			{
				UnionUserErrLog("in UnionExecuteExternalCmdDefinedInSpecFile:: GetProcAddress [%s] ����\n",conMngSvrClientExternalCmdFunName);
				FreeLibrary(externalCmdDll);
				UnionNotice("GetProcAddress��ȡ����[%s]��ַ����\n",conMngSvrClientExternalCmdFunName);
				ret = -1;
				goto errExit;
			}
#endif
			continue;
		}
		if (UnionIsUnixShellRemarkLine(cmdStr))	// ��ע����
			continue;
		if ((lenOfCmdReqStr == 0) || (lenOfCmdReqStr == errCodeEnviMDL_NullLine))	// ����
			continue;
		if (!externalCmdDllConnected)	// û�����Ӷ�̬��
			pexternCmdFun = UnionExcuteExternalCmd;
		// ***** ִ��һ���ⲿ����
		//UnionLog("cmdIndex=[%02d]externalReqStr=[%04d][%s]\n",++cmdIndex,lenOfCmdReqStr,cmdStr);
		if ((lenOfCmdResStr = pexternCmdFun(cmdStr,lenOfCmdReqStr,cmdStr,sizeof(cmdStr))) < 0)
		{
			UnionUserErrLog("in UnionExecuteExternalCmdDefinedInSpecFile:: UnionExcuteExternalCmd [%s]!\n",cmdStr);
			ret = lenOfCmdResStr;
			goto errExit;
		}
		cmdStr[lenOfCmdResStr] = 0;
		//UnionLog("cmdIndex=[%02d]externalResStr=[%04d][%s]\n",cmdIndex,lenOfCmdResStr,cmdStr);
		// ���ļ��ж�ȡ��Ӧƴװ����
		memset(resDefStr,0,sizeof(resDefStr));
		lenOfResDefStr = UnionReadOneLineFromTxtFile(fp,resDefStr,sizeof(resDefStr));
		if (UnionIsUnixShellRemarkLine(resDefStr))	// ��ע����,����Ҫ��Ӧ
			continue;
		//UnionLog("cmdIndex=[%02d]resDefStr=[%04d][%s]\n",cmdIndex,lenOfResDefStr,resDefStr);
		if ((lenOfResDefStr == 0) || (lenOfResDefStr == errCodeEnviMDL_NullLine))	// ����,����Ҫ��Ӧ
			continue;
		// ***** ������Ӧȡֵ���壬����Ӧ���뵽��Ӧ����
		if ((ret = UnionFormRecStrUnderSpecDef(resDefStr,lenOfResDefStr,cmdStr,lenOfCmdResStr,
			resStr+resOffset,sizeOfResStr-resOffset)) < 0)
		{
			UnionUserErrLog("in UnionExecuteExternalCmdDefinedInSpecFile:: UnionFormRecStrUnderSpecDef rules = [%s] resStr = [%s]!\n",resDefStr,cmdStr);
			goto errExit;
		}
		//UnionLog("ret = [%d] resOffset = [%d] res=[%04d][%s][%s]\n",ret,resOffset,strlen(resStr),resStr,resStr+resOffset);
		resOffset += ret;
		//UnionLog("cmdIndex=[%02d]resStr=[%04d][%s]\n",cmdIndex,resOffset,resStr);
	}
	ret = 0;
errExit:
#ifdef _WIN32
	// �ͷŶ�̬������
	if (externalCmdDllConnected)
		FreeLibrary(externalCmdDll);	
#endif
	fclose(fp);
	if (ret == 0)
		return(resOffset);
	else
		return(ret);
}
