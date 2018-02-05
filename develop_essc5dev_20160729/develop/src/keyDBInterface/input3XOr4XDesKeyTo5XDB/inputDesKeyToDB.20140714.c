//      Author: linxj
//      Date:   20140714

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include "UnionLog.h"
#include "UnionTask.h"
#include "unionVersion.h"
#include "unionCommand.h"

#include "unionErrCode.h"
#include "unionRealDBCommon.h"
#include "unionRealBaseDB.h"
#include "symmetricKeyDB.h"
#include "unionHsmCmd.h"

//PUnionTaskInstance      ptaskInstance = NULL;


int insertDesKeyToDB(char *protectID, char *keyFile, int checkKey,char *gunionKeyGroup); //modify by lusj 20151218

int inputAllKeyFromDesKeyFile(const char *fileName,char *gunionKeyGroup);//modify by lusj 20151218 增加密钥组

int deleteDesKey(char *keyFile);//add by chenwd 20160118 迁移回退删除密钥

int UnionTaskActionBeforeExit()
{

        UnionDisconnectTaskTBL();
        UnionDisconnectLogFileTBL();
        exit(0);
}

int UnionHelp()
{
	printf("Usage::\n");
	printf("%s {-appID} appID [-update] {-zmk|-lmk|-lmkcheck} protectID keyFile zmkValue hsmGroup\n",UnionGetApplicationName());
	printf("	-appID	依据AppID迁移密钥，可选\n");
	printf("	appID	应用名称，-appID存在时有\n");
	printf("	-update	可选,没有该选项时通过insert语句更新数据，存在update时通过update语句更新密钥记录\n");
	printf("        -zmk|-lmk|-lmkcheck     密钥迁移功能\n                  -zmk 为zmk密钥保护导出的\n                      -lmk和"
			"-lmkcheck 为lmk密钥保护导出的,其中，-lmk导入数据库不做密钥校验值检查，-lmkcheck需要做密钥校验值检查\n");
	printf("        protectID       lmk保护方式ID\n");
	printf("        keyFile         需要导入的密钥文件\n");
	printf("        zmkValue        使用模式-zmk时，需要传入的zmk密钥值\n");
	printf("        hsmGroup        使用模式-zmk或者-lmkcheck时，需要传入的密码机组ID，如果不传，默认为\"default\"密码机组ID.\n\n");
	printf("%s {-rollback} keyFile\n", UnionGetApplicationName());
	printf("        -rollback       密钥迁移失败，删除密钥功能\n");
	printf("        keyFile         迁移密钥文件名\n");
	return(0);
}

char gunionProtectID[16];
char gunionZmkValue[52];
char gunionHsmGroup[128];
char inputSelectAppID[128];

int gunionIsUpdateKeyRec = 0; // 迁移时密钥存在，数据库操作采用update方式

int UnionIsUpdateKeyRec()
{
	return(gunionIsUpdateKeyRec);
}

int main(int argc, char **argv)
{
	int	ret;
	char	*p;
	
	char	**argvApp;
	char	inputSucceedOfKeyName[128];
	char	inputFailOfKeyName[128];

	argvApp = argv;
	memset(inputSelectAppID, 0 ,sizeof(inputSelectAppID));
	
	UnionSetApplicationName(argv[0]);

	if (argc > 3 && strcasecmp(argv[1], "-appID") == 0) //add by chenwd 20160122 依据AppID 迁移
	{
		snprintf(inputSelectAppID, sizeof(inputSelectAppID), "%s.", argv[2]);
		argvApp++;
		argvApp++;
		argc = argc - 2;
	}
	if (argc > 3 && strcasecmp(argvApp[1], "-update") == 0)
	{
		gunionIsUpdateKeyRec = 1;
		argvApp++;
		argc = argc - 1;
	}

	if (argc < 4)
	{
		if (argc == 3 && strcasecmp(argvApp[1], "-rollback") == 0) //add by chenwd 20160115 迁移失败，回退删除密钥
		{
			if ((ret = deleteDesKey(argvApp[2])) < 0)
			{
				printf("input3XOr4XDesKeyTo5XDB -rollback Failure! \n");
				printf("请使用迁移成功记录文件inputSucceedOfKeyName.xxxx.txt文件进行回退！\n");
			}
			else
			{
				printf("input3XOr4XDesKeyTo5XDB -rollback OK! \n");
			}

			return(UnionTaskActionBeforeExit());
		}

		return(UnionHelp());
	}
	/*if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}*/

	snprintf(gunionProtectID, sizeof(gunionProtectID), "%s", argvApp[2]);
	
	signal(SIGCHLD, SIG_IGN);

	if (strcasecmp(argvApp[1], "-zmk") == 0)
	{
		if (argc < 5)
			return(UnionHelp());

		// zmk 密钥
		snprintf(gunionZmkValue, sizeof(gunionZmkValue), "%s", argvApp[4]);
		while (1)
		{
			if (strlen(gunionZmkValue) == 0)
			{
				if (UnionIsQuit(p=UnionInput("请输入ZMK的密文(exit退出)::")))
					return(UnionTaskActionBeforeExit());
				snprintf(gunionZmkValue, sizeof(gunionZmkValue), "%s", p);
			}
			if (!UnionIsValidDesKeyCryptogram(gunionZmkValue))
			{
				printf("非法的ZMK密文，请重输!\n");
				memset(gunionZmkValue, 0, sizeof(gunionZmkValue));
			}
			else
				break;
		}

		// 加密机组设置
		if (argc > 5)
			snprintf(gunionHsmGroup, sizeof(gunionHsmGroup), "%s", argvApp[5]);
		else
			snprintf(gunionHsmGroup, sizeof(gunionHsmGroup), "default");
		UnionSetHsmGroupIDForHsmSvr(gunionHsmGroup);

	//	if ((ret = inputAllKeyFromDesKeyFile(argv[3])) < 0)
		if ((ret = inputAllKeyFromDesKeyFile(argvApp[3],gunionHsmGroup)) < 0)//modify by lusj 20151218 增加密钥组
		{
			printf("input3XOr4XDesKeyTo5XDB -zmk Failure!\n");
		}
		else
		{
			printf("input3XOr4XDesKeyTo5XDB -zmk OK!\n");
			memset(inputSucceedOfKeyName, 0, sizeof(inputSucceedOfKeyName));
			memset(inputFailOfKeyName, 0, sizeof(inputFailOfKeyName));
			snprintf(inputSucceedOfKeyName,sizeof(inputSucceedOfKeyName),"inputSucceedOfKeyName.%s.txt",UnionGetCurrentFullSystemDateTime());
			snprintf(inputFailOfKeyName,sizeof(inputFailOfKeyName),"inputDesKeyCheckFail.%s.txt",UnionGetCurrentFullSystemDateTime());
			if ((ret = rename("inputSucceedOfKeyName.txt",inputSucceedOfKeyName)) == 0)
			{
				printf("InputSucceedOfKeyNameRecordFile::%s\n",inputSucceedOfKeyName);
			}
			
			if ((ret = rename("inputDesKeyCheckFail.txt",inputFailOfKeyName)) == 0)
			{
				printf("InputFailOfKeyNameRecordFile::%s\n",inputFailOfKeyName);
			}
		}
	}
	else if (strcasecmp(argvApp[1], "-lmk") == 0)
	{
		//insertDesKeyToDB(argv[2], argv[3], 0);
	
		//add begin by lusj 20151218 增加密钥组
		if (argc > 4)
			snprintf(gunionHsmGroup, sizeof(gunionHsmGroup), "%s", argvApp[4]);
		else
			snprintf(gunionHsmGroup, sizeof(gunionHsmGroup), "default");
		//add end by lusj 20151218	
	
		if ((ret = insertDesKeyToDB(argvApp[2], argvApp[3], 0,gunionHsmGroup)) < 0) //modify by lusj 20151218 增加密钥组
		{
			printf("input3XOr4XDesKeyTo5XDB -lmk Failure!\n");
		}
		else
		{
			printf("input3XOr4XDesKeyTo5XDB -lmk OK!\n");
			memset(inputSucceedOfKeyName, 0, sizeof(inputSucceedOfKeyName));
			memset(inputFailOfKeyName, 0, sizeof(inputFailOfKeyName));
			snprintf(inputSucceedOfKeyName,sizeof(inputSucceedOfKeyName),"inputSucceedOfKeyName.%s.txt",UnionGetCurrentFullSystemDateTime());
			snprintf(inputFailOfKeyName,sizeof(inputFailOfKeyName),"inputDesKeyCheckFail.%s.txt",UnionGetCurrentFullSystemDateTime());
			if ((ret = rename("inputSucceedOfKeyName.txt",inputSucceedOfKeyName)) == 0)
			{
				printf("InputSucceedOfKeyNameRecordFile::%s\n",inputSucceedOfKeyName);
			}
			
			if ((ret = rename("inputDesKeyCheckFail.txt",inputFailOfKeyName)) == 0)
			{
				printf("InputFailOfKeyNameRecordFile::%s\n",inputFailOfKeyName);
			}
		}
	}
	else if (strcasecmp(argvApp[1], "-lmkcheck") == 0)
	{
		// 加密机组设置
		if (argc > 4)
			snprintf(gunionHsmGroup, sizeof(gunionHsmGroup), "%s", argvApp[4]);
		else
			snprintf(gunionHsmGroup, sizeof(gunionHsmGroup), "default");
		UnionSetHsmGroupIDForHsmSvr(gunionHsmGroup);
		//insertDesKeyToDB(argv[2], argv[3], 1);
		
		if ((ret = insertDesKeyToDB(argvApp[2], argvApp[3], 1,gunionHsmGroup)) < 0)//modify by lusj 20151218 增加密钥组
		{
			printf("input3XOr4XDesKeyTo5XDB -lmkcheck Failure!\n");
		}
		else
		{
			printf("input3XOr4XDesKeyTo5XDB -lmkcheck OK!\n");
			memset(inputSucceedOfKeyName, 0, sizeof(inputSucceedOfKeyName));
			memset(inputFailOfKeyName, 0, sizeof(inputFailOfKeyName));
			snprintf(inputSucceedOfKeyName,sizeof(inputSucceedOfKeyName),"inputSucceedOfKeyName.%s.txt",UnionGetCurrentFullSystemDateTime());
			snprintf(inputFailOfKeyName,sizeof(inputFailOfKeyName),"inputDesKeyCheckFail.%s.txt",UnionGetCurrentFullSystemDateTime());
			if ((ret = rename("inputSucceedOfKeyName.txt",inputSucceedOfKeyName)) == 0)
			{
				printf("InputSucceedOfKeyNameRecordFile::%s\n",inputSucceedOfKeyName);
			}
			
			if ((ret = rename("inputDesKeyCheckFail.txt",inputFailOfKeyName)) == 0)
			{
				printf("InputFailOfKeyNameRecordFile::%s\n",inputFailOfKeyName);
			}
		}
	}
	else
		return(UnionHelp());

	return(UnionTaskActionBeforeExit());
}

