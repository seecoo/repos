//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>

//#include "UnionLog.h"
//#include "unionErrCode.h"

int UnionTaskActionBeforeExit()
{
	return 0;
}
int (*UnionFunDLVersion)(char *dlName);

int UnionGetFunDLVersion(char *dlName)
{
	int		ret = 0;
	char		funName[32];
	void		*handle = NULL;

	if ((handle = dlopen(dlName, RTLD_LAZY)) == NULL)
	{
		fprintf(stderr, "in UnionGetFunDLVersion:: dlopen[%s] error[%s]!\n",dlName,dlerror());
		return(-1);
	}

	memset(funName, 0, sizeof(funName));
	strcpy(funName, "UnionServiceVersion");

	if ((UnionFunDLVersion = dlsym(handle, funName)) == NULL)
	{
		//fprintf(stderr, "in UnionGetFunDLVersion:: dlsym error funName[%s]!\n", funName);
		fprintf(stdout, "���ļ�[%s]ȱ�ٰ汾��Ϣ����뷢��ʱ�����[2014-6-11]!\n", dlName);
		return(-2);
	}
	ret = (*UnionFunDLVersion)(dlName);
	return(ret);
}


int main(int argc, char **argv)
{
	char		*dlName = NULL;
	char		sDlName[256];

	if(argc < 2 || strcmp("help", (dlName = argv[1])) == 0 || strcmp("--help", dlName) == 0 || strcmp("-h", dlName) == 0)
	{
        	printf("   ������:        %s\n", argv[0]);
        	printf("     λ��:        %d Bits\n", (int)(sizeof(long) * 8));
        	printf(" ��Ȩ����:        Copyright ���Ͽ���\n");
        	printf(" ��������:        400-008-0466\n");
        	printf(" ��˾��ַ:        http://www.keyou.cn\n");
        	printf("\n");
		printf("     ��;: �鿴����̬��汾��Ϣ��\n");
		printf("     �÷�: %s libxxxx.so\n", argv[0]);
		printf("           libxxxx.soΪ��̬���ļ���\n\n");
		return(0);
	}
	
	if(strstr(dlName, "/"))
	{
		snprintf(sDlName, sizeof(sDlName), "%s", dlName);
	}
	else
	{
		snprintf(sDlName, sizeof(sDlName), "./%s", dlName);
	}

	/*
	if (strlen(dlName) < strlen("libxxxx.so") || strncmp(dlName + strlen(dlName) - strlen("libxxxx.so"), "lib", 3) != 0 ||  strncmp(dlName + strlen(dlName) - 3, ".so", 3) != 0)
	{
		fprintf(stderr, "in :: %s libName[%s] invalid!\n", argv[0], dlName);
		return(-3);
	}

	if(access(dlName, R_OK))
	{
		fprintf(stderr, "in :: %s libFile[%s] not found!\n", argv[0], dlName);
		return(-4);
	}
	*/

	return(UnionGetFunDLVersion(sDlName));
}

