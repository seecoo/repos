// wolfang wang
// 2008/10/3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionVariableDefFile.h"
#include "unionVarTypeDef.h"
#include "UnionLog.h"

/* 从指定文件读指定名称的类型标识的定义
输入参数
	fileName	文件名称
输出参数
	pdef	读出的类型标识定义
返回值：
	>=0 	读出的类型标识的大小
	<0	出错代码	
	
*/
int UnionReadVariableDefFromSpecFile(char *fileName,PUnionVariableDef pdef)
{
	char	lineStr[1024+1];
	int	lineLen;
	int	ret;
	FILE	*fp;
	char	*ptr;
	
	if ((fileName == NULL) || (pdef == NULL))
	{
		UnionUserErrLog("in UnionReadVariableDefFromSpecFile:: parameter!\n");
		return(errCodeParameter);
	}
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadVariableDefFromSpecFile:: fopen [%s] error!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	memset(pdef,0,sizeof(*pdef));
	while (!feof(fp))
	{
		// 从文件中读取一行
		memset(lineStr,0,sizeof(lineStr));
		if ((lineLen = UnionReadOneLineFromTxtStr(fp,lineStr,sizeof(lineStr))) < 0)
		{
			if (lineLen == errCodeEnviMDL_NullLine)	// 空行
				continue;
			UnionSystemErrLog("in UnionReadVariableDefFromSpecFile:: UnionReadOneLineFromTxtStr error in [%s]\n",fileName);
			return(lineLen);
		}
		if (lineLen == 0)	// 空行
			continue;
		if (UnionIsUnixShellRemarkLine(lineStr))
			continue;
		if ((ret = UnionReadVariableDefFromStr(lineStr,lineLen,pdef)) < 0)
			continue;
		fclose(fp);
		return(0);
	}
	fclose(fp);
	return(errCodeCDPMDL_GlobalVariableNotDefined);
}

/* 从缺省定义文件读指定名称的全局变量的定义
输入参数
	nameOfVariable	全局变量类型
输出参数
	pdef	读出的全局变量定义
返回值：
	>=0 	读出的全局变量的大小
	<0	出错代码	
	
*/
int UnionReadVariableDefFromDefaultDefFile(char *nameOfVariable,PUnionVariableDef pdef)
{
	char	fileName[512+1];
	int	ret;
	
	memset(fileName,0,sizeof(fileName));
	UnionGetDefaultFileNameOfVariableDef(nameOfVariable,fileName);
	if ((ret = UnionReadVariableDefFromSpecFile(fileName,pdef)) < 0)
	{
		UnionUserErrLog("in UnionReadVariableDefFromDefaultDefFile:: UnionReadVariableDefFromSpecFile [%s] from [%s]\n",nameOfVariable,fileName);
		return(ret);
	}
	return(ret);
}

