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
	printf("验证产品授权码\n");
	strcpy(productName,ptr = UnionInput("请输入产品名称::"));
	if (UnionIsQuit(ptr))
		return(0);
	strcpy(serialNumber,ptr = UnionInput("请输入产品序列号::"));
	if (UnionIsQuit(ptr))
		return(0);
	strcpy(liscenceCode,ptr = UnionInput("请输入产品授权码::"));
	if (UnionIsQuit(ptr))
		return(0);
	if ((ret = UnionInputLisenceKeyUseSpecInputData(productName,serialNumber,liscenceCode)) >= 0)
		printf("产品授权成功!\n");
	else
		printf("UnionInputLisenceKeyUseSpecInputData 出错,ret=[%d]\n",ret);
	if (UnionConfirm("继续验证吗?"))
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
