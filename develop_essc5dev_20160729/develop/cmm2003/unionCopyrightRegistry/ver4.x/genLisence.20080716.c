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
	char	productName[128+1];
	char	serialNumber[128+1];
	char	liscenceCode[128+1];
	char	*ptr;
	
loop:
	system("clear");
	printf("生成产品授权码\n");
	strcpy(productName,ptr = UnionInput("请输入产品名称::"));
	if (UnionIsQuit(ptr))
		return(0);
	strcpy(serialNumber,ptr = UnionInput("请输入产品序列号::"));
	if (UnionIsQuit(ptr))
		return(0);
	memset(liscenceCode,0,sizeof(liscenceCode));
	if ((ret = UnionGenerateLisenceKeyUseSpecInputData(productName,serialNumber,liscenceCode,0)) >= 0)
	{
		printf("产品名::%s\n",productName);
		printf("序列号::%s\n",serialNumber);
		printf("授权码::%s\n",liscenceCode);
	}
	else
		printf("UnionGenerateLisenceKeyUseSpecInputData出错,ret=[%d]\n",ret);
	if (UnionConfirm("继续生成新的序列号吗?"))
		goto loop;
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

