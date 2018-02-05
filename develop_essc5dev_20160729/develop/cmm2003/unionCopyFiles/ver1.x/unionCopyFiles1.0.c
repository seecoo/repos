// Wolfgang Wang, 2003/12/25

#include <stdio.h>
#include <string.h>

#define _UnionEnv_3_x_

#include "UnionEnv.h"
#include "UnionStr.h"

char	gOriginDir[1024];
char	gDestinitionDir[1024];
char	gOriginFile[256];
char	gDestinitionFile[256];

int UnionCopyFiles(char *fileName)
{
	FILE			*fp;
	PUnionEnviVariable	pVar;
	char			line[2048];
	char			cmdBuf[2048];
	char			*p0,*p1;
	int			lineNum = 0;
	int			fileNum = 0;
	int			ret;
	
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		printf("Cannot open %s\n",fileName);
		return(-1);
	}
	
	memset(gOriginDir,0,sizeof(gOriginDir));
	memset(gDestinitionDir,0,sizeof(gDestinitionDir));
	while (!feof(fp))
	{
		memset(line,0,sizeof(line));
		if ((ret = UnionReadOneFileLine(fp,line)) < 0)
		{
			if (!feof(fp))
			{
				printf("UnionReadOneFileLine Error!\n");
				ret = -1;
			}
			break;
		}
		lineNum++;
		if (UnionIsUnixShellRemarkLine(line))
			continue;
		if ((pVar = ReadUnionEnviVarFromStr(line)) == NULL)
		{
			printf("%s\n",line);
			continue;
		}
		if ((p0 = UnionReadValueOfTheIndexOfEnviVar(pVar,0)) == NULL)
		{
			printf("Line:: %d error!\n",lineNum);
			ret = -1;
			break;
		}
		strcpy(gOriginFile,p0);
		if ((p1 = UnionReadValueOfTheIndexOfEnviVar(pVar,1)) == NULL)
		{
			if (UnionStrContainCharStar(gOriginFile))
				memset(gDestinitionFile,0,sizeof(gDestinitionFile));
			else
				UnionDeleteSuffixVersionFromStr(gOriginFile,gDestinitionFile);
		}
		else
			strcpy(gDestinitionFile,p1);
		UnionToUpperCase(p0);
		if (strcmp(p0,"ORIGINDIR") == 0)
		{	
			if (p1 == NULL)
			{
				printf("Line:: %d error!\n",lineNum);
				ret = -1;
				break;
			}			
			memset(gOriginDir,0,sizeof(gOriginDir));
			if ((ret = UnionConvertUnixShellStr(p1,strlen(p1),gOriginDir,sizeof(gOriginDir))) < 0)
			{
				printf("Line:: %d error!\n",lineNum);
				break;
			}
			printf("Set original directory::    [%s]\n",gOriginDir);
			continue;
		}
		if (strcmp(p0,"DESTINITIONDIR") == 0)
		{	
			if (p1 == NULL)
			{
				printf("Line:: %d error!\n",lineNum);
				ret = -1;
				break;
			}				
			memset(gDestinitionDir,0,sizeof(gDestinitionDir));
			if ((ret = UnionConvertUnixShellStr(p1,strlen(p1),gDestinitionDir,sizeof(gDestinitionDir))) < 0)
			{
				printf("Line:: %d error!\n",lineNum);
				break;
			}
			printf("Set destinition directory:: [%s]\n",gDestinitionDir);
			continue;
		}
		if (strcmp(p0,"WAIT") == 0)
		{
			printf("Press enter to continue ...\n");
			//getchar();
			getchar();
			system("clear");
			continue;
		}
		if (strcmp(p0,"NULL") == 0)
		{
			printf("\n");
			continue;
		}
		memset(cmdBuf,0,sizeof(cmdBuf));
		sprintf(cmdBuf,"cp -r ");
		if (strlen(gOriginDir) != 0)
			sprintf(cmdBuf+strlen(cmdBuf),"%s/",gOriginDir);
		sprintf(cmdBuf+strlen(cmdBuf),"%s ",gOriginFile);
		if (strlen(gDestinitionDir) != 0)
			sprintf(cmdBuf+strlen(cmdBuf),"%s/",gDestinitionDir);
		sprintf(cmdBuf+strlen(cmdBuf),"%s",gDestinitionFile);
		//sprintf(cmdBuf,"cp %s/%s %s/%s",gOriginDir,gOriginFile,gDestinitionDir,gDestinitionFile);
		if ((ret = system(cmdBuf)) != 0)
		{
			printf("Failure::    %25s %25s\n",gOriginFile,gDestinitionFile);
			ret = -1;
			break;
		}
		else
			printf("Success::    %25s %25s\n",gOriginFile,gDestinitionFile);
		fileNum++;
	}
	if (feof(fp))
	{
		printf("\nTotal [%d] files copied!\n",fileNum); 
		ret = 0;
	}
	fclose(fp);
	
	return(ret);
}

int main(int argc,char *argv[])
{
	int	ret;
	int	i;
	
	if (argc <= 1)
	{
		printf("Usage:: %s copyDefinitionFile\n",argv[0]);
		return(-1);
	}
	
	if ((ret = UnionCopyFiles(argv[1])) < 0)
		printf("Copy files according to file [%s] failure!\n",argv[1]);
	else
		printf("Copy files according to file [%s] ok!\n",argv[1]);
		
	return(ret);
}

long UnionGetSizeOfLogFile()
{
	return(10000000);
}

int UnionGetNameOfLogFile(char *logFileName)
{
	sprintf(logFileName,"%s/log/%s",getenv("HOME"),"unionCopyFile.log");
	return(0);
}

int UnionIsDebug()
{
	return(1);
}

int UnionReadIntTypeRECVar(char *str)
{
        return 0;
}

