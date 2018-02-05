//	Wolfgang Wang
//	2003/11/25

#include <stdio.h>
#include <string.h>

int main(int argc,char *argv[])
{
	int	ret;
	char	cmdBuf[1024];
	char	dateTime[14+1];
	char	confirm;
	
	if (argc < 6)
	{
		printf("Usage:: %s tmpTarFileName user project product version\n",argv[0]);
		return(-1);
	}
	
	// 获取生成日期时间
	memset(dateTime,0,sizeof(dateTime));
	UnionGetFullSystemDate(dateTime);
	
	
	printf("user			[%s]\n",argv[2]);
	printf("project			[%s]\n",argv[3]);
	printf("product			[%s]\n",argv[4]);
	printf("version			[%s]\n",argv[5]);
	printf("Generate such a package?(Y/N)");
	scanf("%c",&confirm);
	if ((confirm != 'Y') && (confirm != 'y'))	
	{
		printf("Don't generate such a package!\n");
		return(-1);
	}

	// 
	memset(cmdBuf,0,sizeof(cmdBuf));
	sprintf(cmdBuf,"mv %s %s/setupLibs/%s.%s.%s.%s.%s.tar",
		argv[1],
		getenv("PRODUCTHOUSEWARE"),
		argv[2],
		argv[3],
		argv[4],
		argv[5],
		dateTime);	
	
	printf("%s\n",cmdBuf);
	if (system(cmdBuf) == 0)
	{
		printf("Generate package OK!\n");
		return(0);
	}
	else
	{
		printf("Generate package Failure!\n");
		return(-1);
	}
}
