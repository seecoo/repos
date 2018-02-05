//	Author:		Wolfgang Wang
//	Date:		2003/09/20
//	Version:	1.0

#include <stdio.h>
#include <string.h>

#include "unionVersion.h"
#include "UnionLog.h"
#include "unionCommand.h"

int UnionGenerateFile(char *version)
{
	char	fileName[512];
	FILE	*fp;
	
	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"unionPredefinedVersion.c");
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		printf("fopen [%s] error!\n",fileName);
		return(-1);
	}
	
	fprintf(fp,"#include \"unionVersion.h\"\n");
	fprintf(fp,"\n\n");
	fprintf(fp,"char *UnionReadPredfinedProductVersionNumber()\n");
	fprintf(fp,"{\n");
	fprintf(fp,"	return(\"%s\");\n",version);
	fprintf(fp,"}\n");
	
	fclose(fp);
	
	return(0);
}

int main()
{
	int	ret;
	char	version[32];
	char	cmd[512];
	
	for (;;)
	{
		memset(version, 0, sizeof(version));
		printf("Input versionNumber::");
		scanf("%31s", version);
		if(strlen(version) == 0)
		{
			printf("Input NULL!!");
			continue;
		}
	
		if(strcmp("quit", version) == 0 || strcmp("exit", version) == 0)
		{
			break;
		}
		printf("Input value = [%s]\n", version);
		if(version[0] < '0' || version[0] > '9')
		{
			continue;
		}

		if ((ret = UnionGenerateFile(version)) < 0)
		{
			printf("UnionGenerateFile Error! ret = [%d]\n",ret);
			return(ret);
		}
		system("make -f $CMM2003TOOLSDIR/verNumber.mk");
		printf("make OK!\n");
		memset(cmd,0,sizeof(cmd));
		sprintf(cmd,"mv unionPredefinedVersion.a $CMM2003VERLIB/unionPredefinedVersion%s.a", version);
		system(cmd);
		printf("%s\n",cmd);
		system("rm unionPredefinedVersion.c");
	}
	return(0);
}

int UnionReadIntTypeRECVar(char *str)
{
	return 0;
}
