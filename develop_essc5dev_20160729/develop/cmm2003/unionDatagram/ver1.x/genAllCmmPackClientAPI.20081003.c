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
	char	programName[128+1];
	
	for (index = 1; index < argc; index++)
	{
		strcpy(programName,argv[index]);
		if ((ret = UnionGenerateAllCmmPackClientAPIInSpecProgram(programName)) < 0)
			printf("UnionGenerateAllCmmPackClientAPIInSpecProgram [%s] failure! ret = [%d]\n",programName,ret);
		else
			printf("UnionGenerateAllCmmPackClientAPIInSpecProgram [%s] OK!\n",programName);
	}
	if (argc >= 2)
		return(0);
loop1:
	ptr = UnionInput("\n\n请输入要生成的程序名称(exit退出)::");
	strcpy(programName,ptr);
	if (UnionIsQuit(ptr))
		return(errCodeUserSelectExit);
	if ((ret = UnionGenerateAllCmmPackClientAPIInSpecProgram(programName)) < 0)
		printf("UnionGenerateAllCmmPackClientAPIInSpecProgram [%s] failure! ret = [%d]\n",programName,ret);
	else
		printf("UnionGenerateAllCmmPackClientAPIInSpecProgram [%s] OK!\n",programName);
	goto loop1;
}
