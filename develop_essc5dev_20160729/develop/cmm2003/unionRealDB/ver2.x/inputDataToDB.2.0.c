//	������
//	2012-12-27

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include "unionCommand.h"
#include "unionErrCode.h"
#include "UnionStr.h"
#include "unionREC.h"
#include "unionRealDBCommon.h"
#include "unionRealBaseDB.h"
#include "unionTableDef.h"
#include "inputAndOutputDataForDB.h"

int UnionTaskActionBeforeExit()
{
        //UnionCloseDatabase();	//modify by leipp 20150923
        exit(0);
}

int main(int argc,char *argv[])
{
	int	ret;
	int	index = 0;
	int	successNum = 0;
	int	failNum = 0;
	char	*ptr;
	char	tableName[128+1];

	if (argc < 2)
		goto loop1;

	if ((ret = UnionReloadTableDefTBL()) < 0)
	{
		printf("UnionReloadTableDefTBL failure! ret = [%d]\n",ret);
		return(0);
	}

	UnionConnectREC(); // 2015-06-26 �Է���һ�ζ�rec����Ҫ����ı�����
		
loop0:
	if (++index >= argc)
	{
		UnionCloseDatabase();
		return(0);
	}
	memset(tableName,0,sizeof(tableName));
	strcpy(tableName,argv[index]);
	if ((ret = UnionInputOneTableDataToDB(tableName,&successNum,&failNum)) < 0)
	{
		printf("��������� [������%20s] ʧ��! ret = [%d]\n",tableName,ret);
	}
	else
		printf("��������� [������%20s] �ܼ�¼��[%5d] �ɹ���¼��[%5d] ʧ�ܼ�¼��[%5d]\n",tableName,ret,successNum,failNum);
	goto loop0;
	
loop1:
	ptr = UnionInput("\n\n��������������(exit�˳�)::");
	memset(tableName,0,sizeof(tableName));
	strcpy(tableName,ptr);
	if (UnionIsQuit(ptr))
	{
		UnionCloseDatabase();
		return(errCodeUserSelectExit);
	}
	if ((ret = UnionInputOneTableDataToDB(tableName,&successNum,&failNum)) < 0)
	{
		printf("��������� [������%20s] ʧ��! ret = [%d]\n",tableName,ret);
	}
	else
		printf("��������� [������%20s] �ܼ�¼��[%5d] �ɹ���¼��[%5d] ʧ�ܼ�¼��[%5d]\n",tableName,ret,successNum,failNum);
	goto loop1;
}

