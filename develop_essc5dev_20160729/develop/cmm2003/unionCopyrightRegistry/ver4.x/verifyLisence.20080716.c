//	Wolfgang Wang
//	2008/7/16

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <time.h>

#include "unionLisence.h"
#include "unionCommand.h"

int main(int argc,char **argv)
{
	int	ret;

	if ((ret = UnionVerifyLisenceKeyUseSpecInputData(NULL,NULL,NULL)) > 0)
		printf("产品授权验证成功!\n");
	else if (ret == 0)
		printf("产品授权验证失败!\n");
	else
		printf("产品授权验证出错,ret=[%d]\n",ret);
	return(0);
}	

int UnionIsDebug()
{
	return(1);
}

long UnionGetSizeOfLogFile()
{
	return(1000000);
}

int UnionGetNameOfLogFile(char *fileName)
{
	sprintf(fileName,"%s/unionlog.log",getenv("UNIONLOG"));
	return(0);
}
