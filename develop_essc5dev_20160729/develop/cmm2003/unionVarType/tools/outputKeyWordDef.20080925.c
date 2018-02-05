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
	char	tmpBuf[128+1];
	
	for (index = 1; index < argc; index++)
	{
		strcpy(tmpBuf,argv[index]);
		if ((ret = UnionDisplayKeyWordDefToFp(tmpBuf,stdout)) < 0)
		{
			printf("UnionDisplayKeyWordDefToFp [%s] failure! ret = [%d]\n",tmpBuf,ret);
			continue;
		}
	}
	if (argc > 1)
		return(0);

loop:
	ptr = UnionInput("\n\nÇëÊäÈë¹Ø¼ü×Ö(exitÍË³ö)::");
	strcpy(tmpBuf,ptr);
	if (UnionIsQuit(ptr))
		return(errCodeUserSelectExit);
	if ((ret = UnionDisplayKeyWordDefToFp(tmpBuf,stdout)) < 0)
	{
		printf("UnionDisplayKeyWordDefToFp [%s] failure! ret = [%d]\n",tmpBuf,ret);
	}
	goto loop;
}
