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

int inputAllKeyFromDesKeyFile(const char *fileName,char *gunionKeyGroup);//modify by lusj 20151218 ������Կ��

int deleteDesKey(char *keyFile);//add by chenwd 20160118 Ǩ�ƻ���ɾ����Կ

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
	printf("	-appID	����AppIDǨ����Կ����ѡ\n");
	printf("	appID	Ӧ�����ƣ�-appID����ʱ��\n");
	printf("	-update	��ѡ,û�и�ѡ��ʱͨ��insert���������ݣ�����updateʱͨ��update��������Կ��¼\n");
	printf("        -zmk|-lmk|-lmkcheck     ��ԿǨ�ƹ���\n                  -zmk Ϊzmk��Կ����������\n                      -lmk��"
			"-lmkcheck Ϊlmk��Կ����������,���У�-lmk�������ݿⲻ����ԿУ��ֵ��飬-lmkcheck��Ҫ����ԿУ��ֵ���\n");
	printf("        protectID       lmk������ʽID\n");
	printf("        keyFile         ��Ҫ�������Կ�ļ�\n");
	printf("        zmkValue        ʹ��ģʽ-zmkʱ����Ҫ�����zmk��Կֵ\n");
	printf("        hsmGroup        ʹ��ģʽ-zmk����-lmkcheckʱ����Ҫ������������ID�����������Ĭ��Ϊ\"default\"�������ID.\n\n");
	printf("%s {-rollback} keyFile\n", UnionGetApplicationName());
	printf("        -rollback       ��ԿǨ��ʧ�ܣ�ɾ����Կ����\n");
	printf("        keyFile         Ǩ����Կ�ļ���\n");
	return(0);
}

char gunionProtectID[16];
char gunionZmkValue[52];
char gunionHsmGroup[128];
char inputSelectAppID[128];

int gunionIsUpdateKeyRec = 0; // Ǩ��ʱ��Կ���ڣ����ݿ��������update��ʽ

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

	if (argc > 3 && strcasecmp(argv[1], "-appID") == 0) //add by chenwd 20160122 ����AppID Ǩ��
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
		if (argc == 3 && strcasecmp(argvApp[1], "-rollback") == 0) //add by chenwd 20160115 Ǩ��ʧ�ܣ�����ɾ����Կ
		{
			if ((ret = deleteDesKey(argvApp[2])) < 0)
			{
				printf("input3XOr4XDesKeyTo5XDB -rollback Failure! \n");
				printf("��ʹ��Ǩ�Ƴɹ���¼�ļ�inputSucceedOfKeyName.xxxx.txt�ļ����л��ˣ�\n");
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

		// zmk ��Կ
		snprintf(gunionZmkValue, sizeof(gunionZmkValue), "%s", argvApp[4]);
		while (1)
		{
			if (strlen(gunionZmkValue) == 0)
			{
				if (UnionIsQuit(p=UnionInput("������ZMK������(exit�˳�)::")))
					return(UnionTaskActionBeforeExit());
				snprintf(gunionZmkValue, sizeof(gunionZmkValue), "%s", p);
			}
			if (!UnionIsValidDesKeyCryptogram(gunionZmkValue))
			{
				printf("�Ƿ���ZMK���ģ�������!\n");
				memset(gunionZmkValue, 0, sizeof(gunionZmkValue));
			}
			else
				break;
		}

		// ���ܻ�������
		if (argc > 5)
			snprintf(gunionHsmGroup, sizeof(gunionHsmGroup), "%s", argvApp[5]);
		else
			snprintf(gunionHsmGroup, sizeof(gunionHsmGroup), "default");
		UnionSetHsmGroupIDForHsmSvr(gunionHsmGroup);

	//	if ((ret = inputAllKeyFromDesKeyFile(argv[3])) < 0)
		if ((ret = inputAllKeyFromDesKeyFile(argvApp[3],gunionHsmGroup)) < 0)//modify by lusj 20151218 ������Կ��
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
	
		//add begin by lusj 20151218 ������Կ��
		if (argc > 4)
			snprintf(gunionHsmGroup, sizeof(gunionHsmGroup), "%s", argvApp[4]);
		else
			snprintf(gunionHsmGroup, sizeof(gunionHsmGroup), "default");
		//add end by lusj 20151218	
	
		if ((ret = insertDesKeyToDB(argvApp[2], argvApp[3], 0,gunionHsmGroup)) < 0) //modify by lusj 20151218 ������Կ��
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
		// ���ܻ�������
		if (argc > 4)
			snprintf(gunionHsmGroup, sizeof(gunionHsmGroup), "%s", argvApp[4]);
		else
			snprintf(gunionHsmGroup, sizeof(gunionHsmGroup), "default");
		UnionSetHsmGroupIDForHsmSvr(gunionHsmGroup);
		//insertDesKeyToDB(argv[2], argv[3], 1);
		
		if ((ret = insertDesKeyToDB(argvApp[2], argvApp[3], 1,gunionHsmGroup)) < 0)//modify by lusj 20151218 ������Կ��
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

