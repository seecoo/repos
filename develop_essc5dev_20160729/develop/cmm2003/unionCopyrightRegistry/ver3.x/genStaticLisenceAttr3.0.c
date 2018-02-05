//	Author:		Wolfgang Wang
//	Date:		2004/6/17
//	Version:	1.0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define gFileName	"unionFixedLisence"

#include "unionLisence.h"
#include "UnionLog.h"


int UnionGenerateFile()
{
	FILE	*fp;
	char	fileName[512];
	char	buf[256];
	long	effectiveDays;
	time_t	lisenceTime;
		
	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s.c",gFileName);
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		printf("fopen [%s] error!\n",fileName);
		return(-1);
	}

	fprintf(fp,"#include <stdio.h>\n");
	fprintf(fp,"#include <string.h>\n");
	fprintf(fp,"#include <time.h>\n");
	fprintf(fp,"\n");
	fprintf(fp,"#include \"%s/include/unionLisence.h\"\n\n",getenv("UNIONLIBDIR"));
	fprintf(fp,"\n");
	fprintf(fp,"int UnionGetFixedLisence(PUnionLisence plisence)\n");
	fprintf(fp,"{\n");
	fprintf(fp,"	if (plisence == NULL)\n");
	fprintf(fp,"	{\n");
	fprintf(fp,"		UnionUserErrLog(\"in UnionGetFixedLisence:: null pointer!\\n\");\n");
	fprintf(fp,"		return(-1);\n");
	fprintf(fp,"	}\n");
loopInputSerialNumber:
	memset(buf,0,sizeof(buf));
	printf("请输入序列号(最长16个字节):");
	scanf("%s",buf);
	if ((strlen(buf) > 16) || (strlen(buf) <= 0))
	{
		printf("错误的序列号，请重新输入!\n");
		goto loopInputSerialNumber;
	}
	fprintf(fp,"	strcpy(plisence->serialNumber,\"%s\");\n",buf);
loopInputLisenceCode:
	memset(buf,0,sizeof(buf));
	printf("请输入校验数据(最长16个字节):");
	scanf("%s",buf);
	if ((strlen(buf) > 16) || (strlen(buf) <= 0))
	{
		printf("错误的校验数据，请重新输入!\n");
		goto loopInputLisenceCode;
	}
	fprintf(fp,"	strcpy(plisence->lisenceData,\"%s\");\n",buf);
loopInputUserName:
	memset(buf,0,sizeof(buf));
	printf("请输入用户名称(最长40个字节):");
	scanf("%s",buf);
	if ((strlen(buf) > 40) || (strlen(buf) <= 0))
	{
		printf("错误的用户名称，请重新输入!\n");
		goto loopInputUserName;
	}
	fprintf(fp,"	strcpy(plisence->userName,\"%s\");\n",buf);

	memset(buf,0,sizeof(buf));
	printf("请输入软件有效日期:");
	scanf("%ld",&effectiveDays);
	fprintf(fp,"	plisence->effectiveDays = %ld;\n",effectiveDays);
	
	time(&lisenceTime);
	fprintf(fp,"	plisence->lisenceTime = %ld;\n",lisenceTime);
	fprintf(fp,"	\n");
	fprintf(fp,"	return(0);\n");
	fprintf(fp,"}\n");
	
	fclose(fp);
	
	return(0);
}

int UnionComplyFile()
{
	char	cmd[512];
	char    makeCmd[16+1];
	
	memset(makeCmd,0,sizeof(makeCmd));
	if (getenv("CC") == NULL)
		memcpy(makeCmd,"cc",2);
	else
		sprintf(makeCmd,"%s",getenv("CC"));

	if (getenv("OSSPEC") == NULL) 
		sprintf(cmd,"%s -c %s.c",makeCmd,gFileName);
	else
		sprintf(cmd,"%s -c %s %s.c",makeCmd,getenv("OSSPEC"),gFileName);

	system(cmd);

	memset(cmd,0,sizeof(cmd));
	sprintf(cmd,"ar %s rv %s.a %s.o",getenv("OSSPECPACK"),gFileName,gFileName);
	system(cmd);

	memset(cmd,0,sizeof(cmd));
	sprintf(cmd,"rm %s.o",gFileName);
	system(cmd);

	return(0);
}
	
	
int main()
{
	UnionGenerateFile();
	UnionComplyFile();
	return(0);
}
