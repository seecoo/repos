// wolfang wang
// 2008/10/3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionProgramDefFile.h"
#include "UnionLog.h"

/* 从指定文件读指定名称的类型标识的定义
输入参数
	fileName	文件名称
	nameOfProgram	指定的类型
输出参数
	pdef	读出的类型标识定义
返回值：
	>=0 	读出的类型标识的大小
	<0	出错代码	
	
*/
int UnionReadProgramDefFromSpecFile(char *fileName,char *nameOfProgram,PUnionProgramDef pdef)
{
	char	lineStr[1024+1];
	int	lineLen;
	int	ret;
	FILE	*fp;
	
	if ((fileName == NULL) || (pdef == NULL))
	{
		UnionUserErrLog("in UnionReadProgramDefFromSpecFile:: parameter!\n");
		return(errCodeParameter);
	}
	if ((fileName == NULL) || (strlen(fileName) == 0) || (strcmp(fileName,"null") == 0))
		return(UnionReadProgramDefFromDefaultFile(nameOfProgram,pdef));
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadProgramDefFromSpecFile:: fopen [%s] error!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	memset(pdef,0,sizeof(*pdef));
	while (!feof(fp))
	{
		// 从文件中读取一行
		memset(lineStr,0,sizeof(lineStr));
		if ((lineLen = UnionReadOneDataLineFromTxtFile(fp,lineStr,sizeof(lineStr))) < 0)
		{
			if ((lineLen == errCodeEnviMDL_NullLine) || (lineLen == errCodeFileEnd))	// 空行
				continue;
			UnionSystemErrLog("in UnionReadProgramDefFromSpecFile:: UnionReadOneDataLineFromTxtFile error in [%s]\n",fileName);
			fclose(fp);
			return(lineLen);
		}
		if (lineLen == 0)	// 空行
			continue;
		if ((ret = UnionReadProgramDefFromStr(lineStr,lineLen,pdef)) < 0)
			continue;
		if (strcmp(nameOfProgram,pdef->nameOfProgram) == 0)
		{
			fclose(fp);
			return(0);
		}
	}
	fclose(fp);
	UnionUserErrLog("in UnionReadProgramDefFromSpecFile:: [%s] not defined in [%s]\n",nameOfProgram,fileName);
	return(errCodeCDPMDL_ProgramNotDefined);
}

/* 从缺省文件读指定名称的类型标识的定义
输入参数
	nameOfProgram	指定的类型
输出参数
	pdef	读出的类型标识定义
返回值：
	>=0 	读出的类型标识的大小
	<0	出错代码	
	
*/
int UnionReadProgramDefFromDefaultFile(char *nameOfProgram,PUnionProgramDef pdef)
{
	char	fileName[256];
	
	UnionGetDefaultFileNameOfProgramDef(nameOfProgram,fileName);
	return(UnionReadProgramDefFromSpecFile(fileName,nameOfProgram,pdef));
}

