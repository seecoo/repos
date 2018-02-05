//	Wolfgang Wang
//	2008/2/25

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "unionDesignKeyWord.h"
#include "unionCommand.h"
#include "unionErrCode.h"

int main(int argc,char *argv[])
{
	int	ret;
	int	index;
	char	*ptr;
	char	appID[128+1];
	char	datagramID[128+1];
	
loop1:
	ptr = UnionInput("\n\n�����뱨����;��ʶ(exit�˳�)::");
	strcpy(appID,ptr);
	if (UnionIsQuit(ptr))
		return(errCodeUserSelectExit);
loop2:
	ptr = UnionInput("\n\n�����뱨�����ͱ�ʶ(exit�˳�)::");
	strcpy(datagramID,ptr);
	if (UnionIsQuit(ptr))
		goto loop1;
	if ((ret = UnionPrintSpecDatagramDefToSpecFile(appID,datagramID,NULL)) < 0)
	{
		printf("UnionPrintSpecDatagramDefToSpecFile [%s] [%s] failure! ret = [%d]\n",appID,datagramID,ret);
	}
	goto loop2;
}
