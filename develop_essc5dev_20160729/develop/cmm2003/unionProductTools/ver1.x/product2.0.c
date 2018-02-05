//	Wolfgang Wang
//	2003/09/09

#define _UnionTask_3_x_
#define _UnionDesKeyDB_2_x_
#define _UnionLogMDL_3_x_
#define _UnionEnv_3_x_

#include <stdio.h>
#include <string.h>

#include "UnionLog.h"
#include "unionCommand.h"
#include "unionVersion.h"
#include "UnionTask.h"
#include "unionModule.h"
#include "UnionEnv.h"

char	workingDir[256];

PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	return(exit(0));
}

int UnionAnalysisFile(char *fileName)
{
	char			fullFileName[512];
	FILE			*fp;
	PUnionEnviVariable	pvar;
	char			tmpBuf[1024];
	int			len;
	
	memset(fullFileName,0,sizeof(fullFileName));
	sprintf(fullFileName,"%s/Product/%s",getenv("HOME"),fileName);
	if ((fp = fopen(fullFileName)) == NULL)
	{
		printf("fopen [%s] Error!\n",fullFileName);
		return(-1);
	}
	while (!feof(fp))
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((len = UnionReadOneFileLine(fp,tmpBuf)) < 0)
			break;
		if ((pvar = ReadUnionEnviVarFromStr(tmpBuf)) == NULL)
			continue;
		if (
	}
	
	return(0);
}
	
int UnionGenerateBackup()
{
	char	*p;

	memset(cmd,0,sizeof(cmd));
	sprintf(cmd,"%s/Product/gen%s%s%s.x",workingDir,project,product,version);
	return(system(cmd));
}

int UnionGetNameOfPlatForm(char *platForm)
{
	FILE	*fp;
	char	fileName[512];
	
	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/tmp/platForm.txt",workingDir);
	
	memset(cmd,0,sizeof(cmd));
	sprintf(cmd,"uname > %s",fileName);
	system(cmd);
	
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		sprintf(platForm,"unknownUnix");
		return(0);
	}
	
	fscanf(fp,"%s",platForm);
	
	fclose(fp);
	
	if (strlen(platForm) == 0)
		sprintf(platForm,"unknownUnix");

	return(0);
}

int UnionGenerateProductDir()
{	
	// 创建产品目录
	memset(platForm,0,sizeof(platForm));
	UnionGetNameOfPlatForm(platForm);
	// 创建一个产品目录
	memset(cmd,0,sizeof(cmd));
	sprintf(cmd,"mkdir $HOME/ProductHouseWare");
	system(cmd);
	memset(cmd,0,sizeof(cmd));
	sprintf(cmd,"mkdir $HOME/ProductHouseWare/%s",platForm);
	printf("%s\n",cmd);
	system(cmd);	
}

int UnionGenerateFullProductName()
{
	char	dateTime[14+1];
	char	*p;

	memset(dateTime,0,sizeof(dateTime));
	UnionGetFullSystemDateTime(dateTime);

	memset(fullProductName,0,sizeof(fullProductName));
	sprintf(fullProductName,"%s.%s.%s.%s.%s.tar",user,project,product,version,dateTime);
	
	return(0);
}

int UnionGenerateProduct()
{
	int	ret;
	
	if ((ret = UnionGenerateBackup() < 0))
		return(ret);

	if ((ret = UnionGenerateFullProductName()) < 0)
		return(ret);
		
	
	// 将产品移到产品目录
	memset(cmd,0,sizeof(cmd));
	sprintf(cmd,"mv %s/tmp/tmp.tar $HOME/ProductHouseWare/%s/%s",workingDir,platForm,fullProductName);
	printf("%s\n",cmd);
	
	return(system(cmd));
}
	
int main(int argc,char *argv[])
{
	int	ret;
	char	*p;
	char	tmpBuf[100];
	
	/*
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"product")) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}
	*/
	
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
	
	UnionGenerateProductDir();
	
	memset(user,0,sizeof(user));
	memset(project,0,sizeof(project));
	memset(product,0,sizeof(product));
	memset(version,0,sizeof(version));
	if (argc >= 2)
		strcpy(user,argv[1]);
	else
		strcpy(user,(char *)getenv("USERNAME"));
	if (argc >= 3)
		strcpy(project,argv[2]);
	else
		strcpy(project,(char *)getenv("MAINPROJECTNAME"));
	if (argc >= 4)
		strcpy(product,argv[3]);
	else
		strcpy(product,(char *)getenv("MAINPRODUCTNAME"));
	if (argc >= 5)
		strcpy(version,argv[4]);
	else
		strcpy(version,(char *)getenv("CURRENTVERSION"));
	memset(workingDir,0,sizeof(workingDir));
	sprintf(workingDir,"%s",getenv("HOME"));
	
loopGen:
	printf("\nCurrentWorkingDir	[%s]\n",workingDir);
	printf("Project			[%s]\n",project);
	printf("User			[%s]\n",user);
	printf("Product			[%s]\n",product);
	printf("Version			[%s]\n",version);	
	if (UnionIsQuit(p = UnionInput("Command(user/project/product/version/generate/workingdir/exit/quit)>")))
		return(UnionTaskActionBeforeExit());
	UnionToUpperCase(p);
	
	if (strcmp(p,"USER") == 0)
	{
		memset(user,0,sizeof(user));
		strcpy(user,UnionInput(">Input UserName::"));
		goto loopGen;
	}
	
	if (strcmp(p,"VERSION") == 0)
	{
		memset(version,0,sizeof(version));
		strcpy(version,UnionInput(">Input Version::"));
		goto loopGen;
	}
	
	if (strcmp(p,"PROJECT") == 0)
	{
		memset(project,0,sizeof(project));
		p = UnionInput(">Input ProjectID::");
		//UnionToUpperCase(p);
		strcpy(project,p);
		goto loopGen;
	}
	
	if (strcmp(p,"WORKINGDIR") == 0)
	{
		p = UnionInput(">Input WorkingDir::");
		UnionToUpperCase(p);
		memset(workingDir,0,sizeof(workingDir));
		sprintf(workingDir,"%s",getenv(p));
		goto loopGen;
	}
	
	if (strcmp(p,"PRODUCT") == 0)
	{
loopInputType:
		p = UnionInput(">Input Product Name (quit/exit to exit)::");
		memset(tmpBuf,0,sizeof(tmpBuf));
		strcpy(tmpBuf,p);
		if (UnionIsQuit(p))
			goto loopGen;
		strcpy(product,tmpBuf);
		goto loopGen;
	}
	
	if (strcmp(p,"GENERATE") == 0)
	{
		if (!UnionConfirm("Generate product for [%s] [%s] [%s] [%s]",user,project,product,version))
			goto loopGen;
		UnionGenerateProduct();
		goto loopGen;
	}
	
	printf("Warning:: Invalid Command!\n");
	goto loopGen;
}

int UnionHelp()
{
	printf("Usage:: product [user [project [product [version]]]]\n");
	return(0);
}