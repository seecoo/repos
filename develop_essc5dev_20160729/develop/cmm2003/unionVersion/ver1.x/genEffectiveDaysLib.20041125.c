//	Author:		Wolfgang Wang
//	Date:		2003/09/20
//	Version:	1.0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "unionVersion.h"

int UnionGenerateFile(long effectiveDays)
{
	char	fileName[512];
	FILE	*fp;
	
	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"unionEffectiveDays.c");
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		printf("fopen [%s] error!\n",fileName);
		return(-1);
	}
	
	fprintf(fp,"#include \"unionVersion.h\"\n");
	fprintf(fp,"\n\n");
	fprintf(fp,"long UnionReadPredefinedProductEffectiveDays()\n");
	fprintf(fp,"{\n");
	fprintf(fp,"	return(%ld);\n",effectiveDays);
	fprintf(fp,"}\n");
	
	fclose(fp);
	
	return(0);
}

int main()
{
	long	effectiveDays;
	int	i,ret;
	char	tmpBuf[512];
	
	for (;;)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		printf("Input effective days::");
		scanf("%s",tmpBuf);
		for (i = 0; i < 4; i++)
			tmpBuf[i] = toupper(tmpBuf[i]);
		if ((strcmp(tmpBuf,"QUIT") == 0) || (strcmp(tmpBuf,"EXIT") == 0))				
			return(0);
		effectiveDays = atol(tmpBuf);
		if ((ret = UnionGenerateFile(effectiveDays)) < 0)
		{
			printf("UnionGenerateFile Error! ret = [%d]\n",ret);
			return(ret);
		}
		system("make -f $CMM2003TOOLSDIR/genEffectiveDaysLib.mk");
		printf("make OK!\n");
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"mv unionEffectiveDays.a $CMM2003VERLIB/unionEffectiveDays.%ld.a",effectiveDays);
		system(tmpBuf);
		printf("%s\n",tmpBuf);
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"rm unionEffectiveDays.c");
		system(tmpBuf);
		printf("%s\n",tmpBuf);
	}
}
