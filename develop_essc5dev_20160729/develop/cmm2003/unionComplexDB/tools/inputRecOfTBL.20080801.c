//	Wolfgang Wang
//	2008/2/25

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "unionCommand.h"
#include "unionErrCode.h"
#include "unionComplexDBRecord.h"
#include "unionComplexDBDataTransfer.h"
#include "unionGenSQLFromTBLDefList.h"
#include "unionRecFile.h"
#include "UnionLog.h"

int main(int argc,char *argv[])
{
	int	ret;
	int	index = 0;
	char	*ptr;
	char	objectName[128+1];
	char	fileName[1024+1];
	char	systemCmd[1024+1];

	if ((ret = UnionConnectDatabase()) < 0)
	{
		printf("UnionConnectDatabase failure! ret = [%d]\n",ret);
		return(ret);
	}

	if (argc < 2)
		goto loop1;

	UnionSetIsOutPutInPutDataTools(1);
		
loop0:
	if (++index >= argc)
	{
		UnionCloseDatabase();
		return(0);
	}
	strcpy(objectName,argv[index]);
	sprintf(fileName,"%s/%s.txt",getenv("UNIONTEMP"),objectName);
	if ((ret = UnionInputAllRecIntoSpecTBL(objectName,fileName)) < 0)
	{
		printf("导入表数据 [表名：%s] !    记录总数 = [%d]\n",objectName,UnionGetRowNumOfFile(fileName));
		printf("导入表数据 [表名：%s] 失败! ret = [%d]\n",objectName,ret);
	}
	else if (ret == UnionGetRowNumOfFile(fileName))
		printf("导入表数据 [表名：%s]  总记录数 = [%d]   成功记录数 = [%d]   全部导入成功！\n",objectName,UnionGetRowNumOfFile(fileName),ret);
	else if (ret == 0)
		printf("导入表数据 [表名：%s]  总记录数 = [%d]   成功记录数 = [%d]   导入不成功！\n",objectName,UnionGetRowNumOfFile(fileName),ret);
	else
		printf("导入表数据 [表名：%s]  总记录数 = [%d]   成功记录数 = [%d]   部分导入成功！\n",objectName,UnionGetRowNumOfFile(fileName),ret);
	goto loop0;
	
loop1:
	ptr = UnionInput("\n\n请输入对象的名称(exit退出)::");
	strcpy(objectName,ptr);
	if (UnionIsQuit(ptr))
	{
		UnionCloseDatabase();
		return(errCodeUserSelectExit);
	}
	sprintf(fileName,"%s/%s.txt",getenv("UNIONTEMP"),objectName);
	if ((ret = UnionInputAllRecIntoSpecTBL(objectName,fileName)) < 0)
	{
		printf("导入表数据 [表名：%s] !    记录总数 = [%d]\n",objectName,UnionGetRowNumOfFile(fileName));
		printf("导入表数据 [表名：%s] 失败! ret = [%d]\n",objectName,ret);
	}
	else if (ret == UnionGetRowNumOfFile(fileName))
		printf("导入表数据 [表名：%s]  总记录数 = [%d]   成功记录数 = [%d]   全部导入成功！\n",objectName,UnionGetRowNumOfFile(fileName),ret);
	else if (ret == 0)
		printf("导入表数据 [表名：%s]  总记录数 = [%d]   成功记录数 = [%d]   导入不成功！\n",objectName,UnionGetRowNumOfFile(fileName),ret);
	else
		printf("导入表数据 [表名：%s]  总记录数 = [%d]   成功记录数 = [%d]   部分导入成功！\n",objectName,UnionGetRowNumOfFile(fileName),ret);
	goto loop1;
}

int UnionGetRowNumOfFile(char *fileName)
{
	int		lenOfRecStr = 0;
	int		lineNum = 0;
	char		recStr[4096+1];
	FILE		*recFileFp = NULL;
	

	// 打开文件
	if ((recFileFp = fopen(fileName,"r")) == NULL)
	{
		UnionUserErrLog("in UnionGetRowNumOfFile:: fopen [%s]\n",fileName);
		//goto errExit;
		return(0-abs(errno));
	}

	while (!feof(recFileFp))
	{
		memset(recStr,0,sizeof(recStr));
		lineNum++;
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(recFileFp,recStr,sizeof(recStr))) <= 0)
			continue;
	}
	lineNum --;
	fclose(recFileFp);
	return(lineNum);
}
