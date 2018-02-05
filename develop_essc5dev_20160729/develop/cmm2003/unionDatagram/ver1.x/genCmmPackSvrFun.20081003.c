//	Wolfgang Wang
//	2008/2/25

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "unionGenSvrFunUsingCmmPack.h"
#include "unionCommand.h"
#include "unionErrCode.h"

int main(int argc,char *argv[])
{
	int	ret;
	int	index;
	char	*ptr;
	char	appID[128+1];
	char	datagramID[128+1];
	char	version[128+1];
	
loop0:
	ptr = UnionInput("\n\n请输入程序版本日期(exit退出)::");
	strcpy(version,ptr);
	if (UnionIsQuit(ptr))
		return(errCodeUserSelectExit);
loop1:
	ptr = UnionInput("\n\n请输入报文用途标识(exit退出)::");
	strcpy(appID,ptr);
	if (UnionIsQuit(ptr))
		return(errCodeUserSelectExit);
loop2:
	ptr = UnionInput("\n\n请输入报文类型标识(exit退出)::");
	strcpy(datagramID,ptr);
	if (UnionIsQuit(ptr))
		goto loop1;
	if ((ret = UnionGenerateSvrFunUsingCmmPack(appID,datagramID,version)) < 0)
		printf("UnionGenerateSvrFunUsingCmmPack [%s] [%s] failure! ret = [%d]\n",appID,datagramID,ret);
	else
		printf("UnionGenerateSvrFunUsingCmmPack [%s] [%s] OK!\n",appID,datagramID);
	goto loop2;
}
