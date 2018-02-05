//	Author:		Wolfgang Wang
//	Date:		2003/09/20
//	Version:	1.0

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define gFileName	"unionProductGenerateTime"


int UnionGetFullSystemDateTime(char *datetime)
{
        time_t tmptime;
        struct tm *tmptm;
        int  tmpyear;

        time(&tmptime);
        tmptm = (struct tm *)localtime(&tmptime);
        if(tmptm->tm_year > 90)
                tmpyear = 1900 + tmptm->tm_year%1900;
        else
                tmpyear = 2000 + tmptm->tm_year;

        sprintf(datetime,"%04d%02d%02d%02d%02d%02d",tmpyear,tmptm->tm_mon+1,tmptm->tm_mday,tmptm->tm_hour,tmptm->tm_min,tmptm->tm_sec);

        return(0);

}

int UnionGenerateFile()
{
	FILE	*fp;
	char	fileName[512];
	char	nowTime[100];
	
	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s.c",gFileName);
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		printf("fopen [%s] error!\n",fileName);
		return(-1);
	}
	
	//fprintf(fp,"#include \"UnionLog.h\"\n");
	fprintf(fp,"#include <stdio.h>\n");
	fprintf(fp,"#include <stdlib.h>\n");
	fprintf(fp,"#include <string.h>\n");
	fprintf(fp,"char pgProductGeneratedTime[20];\n");
	fprintf(fp,"char *UnionGetProductGeneratedTime()\n");
	fprintf(fp,"{\n");
	fprintf(fp,"	memset(pgProductGeneratedTime,0,sizeof(pgProductGeneratedTime));\n");
	memset(nowTime,0,sizeof(nowTime));
	UnionGetFullSystemDateTime(nowTime);
	fprintf(fp,"	strcpy(pgProductGeneratedTime,\"%s\");\n",nowTime);
	fprintf(fp,"	return(pgProductGeneratedTime);\n");
	fprintf(fp,"}\n");
	
	fclose(fp);
	
	return(0);
}

int UnionComplyFile(char *fileName)
{
	char	cmd[512];
	char	makeCmd[32];

	memset(cmd,0,sizeof(cmd));
	memset(makeCmd,0,sizeof(makeCmd));
	if (getenv("CC") == NULL)
		memcpy(makeCmd,"cc",2);
	else
		sprintf(makeCmd,"%s",getenv("CC"));

	if (getenv("OSSPEC") == NULL)	
		sprintf(cmd,"%s -c %s.c",makeCmd,gFileName);
	else
		sprintf(cmd,"%s -c %s %s.c",makeCmd,getenv("OSSPEC"),gFileName);
	printf("%s\n",cmd);
	system(cmd);
	sprintf(cmd,"ar %s rv %s.a %s.o",getenv("OSSPECPACK"),fileName,gFileName);
	printf("%s\n",cmd);
	system(cmd);
	sprintf(cmd,"rm %s.c %s.o",gFileName,gFileName);
	printf("%s\n",cmd);
	system(cmd);
	return(0);
}
	
	
int main(int argc, char **argv)
{
	UnionGenerateFile();
	if(argc > 1)
	{
		UnionComplyFile(argv[1]);
	}
	else
	{
		UnionComplyFile(gFileName);
	}
	return(0);
}
