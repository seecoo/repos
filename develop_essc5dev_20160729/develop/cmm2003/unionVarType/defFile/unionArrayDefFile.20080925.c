//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#include <stdio.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionArrayDefFile.h"
#include "UnionStr.h"

/* 从缺省文件读指定名称的数组的类型定义
输入参数
	nameOfType	数组类型的名称
输出参数
	typeNameOfArray	数组类型的类型
返回值：
	>=0 	成功
	<0	出错代码	
	
*/
int UnionReadTypeOfArrayDefFromDefaultDefFile(char *nameOfType,char *typeNameOfArray)
{
	TUnionArrayDef	def;
	int			ret;
	
	if ((ret = UnionReadArrayDefFromDefaultDefFile(nameOfType,&def)) < 0)
	{
		UnionUserErrLog("in UnionReadTypeOfArrayDefFromDefaultDefFile:: UnionReadArrayDefFromDefaultDefFile!\n");
		return(ret);
	}
	strcpy(typeNameOfArray,def.nameOfType);
	return(0);
}

/* 从缺省定义文件读指定名称的数组的定义
输入参数
	nameOfType	变量类型
输出参数
	pdef	读出的数组定义
返回值：
	>=0 	读出的数组的大小
	<0	出错代码	
	
*/
int UnionReadArrayDefFromDefaultDefFile(char *nameOfType,PUnionArrayDef pdef)
{
	char			fileName[256+1];

	UnionGetDefaultFileNameOfVarTypeDef(nameOfType,fileName);
	return(UnionReadArrayDefFromSpecFile(fileName,pdef));
}

/* 从指定文件读指定名称的数组的定义
输入参数
	fileName	文件名称
输出参数
	pdef	读出的数组定义
返回值：
	>=0 	读出的数组的大小
	<0	出错代码	
	
*/
int UnionReadArrayDefFromSpecFile(char *fileName,PUnionArrayDef pdef)
{
	char	lineStr[1024+1];
	int	lineLen;
	int	ret;
	FILE	*fp;
	char	*ptr;
	
	if ((fileName == NULL) || (pdef == NULL))
	{
		UnionUserErrLog("in UnionReadArrayDefFromSpecFile:: parameter!\n");
		return(errCodeParameter);
	}
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadArrayDefFromSpecFile:: fopen [%s] error!\n",fileName);
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
			UnionSystemErrLog("in UnionReadArrayDefFromSpecFile:: UnionReadOneLineFromTxtStr error in [%s]\n",fileName);
			return(lineLen);
		}
		if (lineLen == 0)	// 空行
			continue;
		if (UnionIsUnixShellRemarkLine(lineStr))
			continue;
		if ((ret = UnionReadArrayDefFromStr(lineStr,lineLen,pdef)) < 0)
			continue;
		fclose(fp);
		return(0);
	}
	fclose(fp);
	return(errCodeCDPMDL_ArrayNotDefined);
}

