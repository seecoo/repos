//	Wolfgang Wang
//	2008/2/25

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "unionGenClientFunUsingCmmPack.h"
#include "unionCommand.h"
#include "unionErrCode.h"

int main(int argc,char *argv[])
{
	int	ret;
	int	index;
	char	*ptr;
	char	funName[128+1];
	int	toNewFile = 1;
	
	if (UnionConfirm("\n\nΪÿ����������дһ��������?"))
		toNewFile = 1;
	else
		toNewFile = 0;
	
	for (index = 1; index < argc; index++)
	{
		strcpy(funName,argv[index]);
		if ((ret = UnionGenerateClientFunUsingCmmPack(funName,toNewFile)) < 0)
			printf("UnionGenerateClientFunUsingCmmPack [%s] failure! ret = [%d]\n",funName,ret);
		else
			printf("UnionGenerateClientFunUsingCmmPack [%s] OK!\n",funName);
	}
	if (argc >= 2)
		return(0);
loop1:
	ptr = UnionInput("\n\n������Ҫ���ɵĺ���������(exit�˳�)::");
	strcpy(funName,ptr);
	if (UnionIsQuit(ptr))
		return(errCodeUserSelectExit);
	if ((ret = UnionGenerateClientFunUsingCmmPack(funName,toNewFile)) < 0)
		printf("UnionGenerateClientFunUsingCmmPack [%s] failure! ret = [%d]\n",funName,ret);
	else
		printf("UnionGenerateClientFunUsingCmmPack [%s] OK!\n",funName);
	goto loop1;
}
