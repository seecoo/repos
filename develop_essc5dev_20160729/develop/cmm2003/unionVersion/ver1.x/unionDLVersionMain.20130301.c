//	Author:		张永定
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
		fprintf(stdout, "库文件[%s]缺少版本信息或编译发布时间旧于[2014-6-11]!\n", dlName);
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
        	printf("   程序名:        %s\n", argv[0]);
        	printf("     位数:        %d Bits\n", (int)(sizeof(long) * 8));
        	printf(" 版权所有:        Copyright 江南科友\n");
        	printf(" 服务热线:        400-008-0466\n");
        	printf(" 公司网址:        http://www.keyou.cn\n");
        	printf("\n");
		printf("     用途: 查看服务动态库版本信息。\n");
		printf("     用法: %s libxxxx.so\n", argv[0]);
		printf("           libxxxx.so为动态库文件名\n\n");
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

