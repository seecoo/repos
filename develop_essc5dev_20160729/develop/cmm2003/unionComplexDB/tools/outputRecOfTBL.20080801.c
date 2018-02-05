//	Wolfgang Wang
//	2008/2/25

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "unionCommand.h"
#include "unionErrCode.h"
#include "unionComplexDBRecord.h"
#include "unionComplexDBDataTransfer.h"
#include "unionGenSQLFromTBLDefList.h"
#include "unionRecFile.h"
#include "UnionLog.h"

int main(int argc,char *argv[])
{
	int	ret;
	int	index = 0;
	char	*ptr;
	char	objectName[128+1];
	char	fileName[1024+1];
	int     len;
	char	data[1024+1];

	if ((ret = UnionConnectDatabase()) < 0)
	{
		printf("UnionConnectDatabase failure! ret = [%d]\n",ret);
		return(ret);
	}
	if (argc < 2)
		goto loop1;

	UnionSetIsOutPutInPutDataTools(1);

loop0:
	if (++index >= argc)
	{
		UnionCloseDatabase();
		return(0);
	}
	strcpy(objectName,argv[index]);
	sprintf(fileName,"%s/%s.txt",getenv("UNIONTEMP"),objectName);

	memset (data, 0, sizeof data);
        len = UnionPutRecFldIntoRecStr("remark","��ʼ������Կ��", 14, data, sizeof(data));
        // ���Ӽ�أ�2009-10-19 xusj
        UnionSendKeyDBBackuperInfoToTransSpier(len, data);
	if ((ret = UnionOutputAllRecFromSpecTBL(objectName,"",fileName)) < 0)
	{
		printf("UnionOutputAllRecFromSpecTBL [%s] failure! ret = [%d]\n",objectName,ret);
		len = UnionPutRecFldIntoRecStr("remark","������Կ��ʧ��", 14, data,sizeof(data));
	}
	else
	{
		printf("UnionOutputAllRecFromSpecTBL [%s] OK! totalRecNum = [%d]\n",objectName,ret);
		len = UnionPutRecFldIntoRecStr("remark","������Կ��ɹ�", 14, data,sizeof(data));
	}
	// ���Ӽ�أ�2009-10-19 xusj
        UnionSendKeyDBBackuperInfoToTransSpier(len, data);
	goto loop0;
	
loop1:
	ptr = UnionInput("\n\n��������������(exit�˳�)::");
	strcpy(objectName,ptr);
	if (UnionIsQuit(ptr))
	{
		UnionCloseDatabase();
		return(errCodeUserSelectExit);
	}
	sprintf(fileName,"%s/%s.txt",getenv("UNIONTEMP"),objectName);
	
	memset (data, 0, sizeof data);
        len = UnionPutRecFldIntoRecStr("remark","��ʼ������Կ��", 14, data, sizeof(data));
        // ���Ӽ�أ�2009-10-19 xusj
        UnionSendKeyDBBackuperInfoToTransSpier(len, data);
	if ((ret = UnionOutputAllRecFromSpecTBL(objectName,"",fileName)) < 0)
	{
		printf("UnionOutputAllRecFromSpecTBL [%s] failure! ret = [%d]\n",objectName,ret);
		len = UnionPutRecFldIntoRecStr("remark","������Կ��ʧ��", 14, data,sizeof(data));
	}
	else
	{
		printf("UnionOutputAllRecFromSpecTBL [%s] OK! totalRecNum = [%d]\n",objectName,ret);
		len = UnionPutRecFldIntoRecStr("remark","������Կ��ɹ�", 14, data,sizeof(data));
	}
	// ���Ӽ�أ�2009-10-19 xusj
        UnionSendKeyDBBackuperInfoToTransSpier(len, data);

	goto loop1;
}

