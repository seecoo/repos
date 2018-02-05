//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#include <stdio.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionSimpleTypeDef.h"
#include "UnionStr.h"
#include "unionDefaultVarTypeDefDir.h"
#include "unionVarTypeDefFile.h"

/* 从指定文件读指定名称的简单类型的定义
输入参数
	fileName	文件名称
输出参数
	pdef	读出的简单类型定义
返回值：
	>=0 	读出的简单类型的大小
	<0	出错代码	
	
*/
int UnionReadSimpleTypeDefFromSpecFile(char *fileName,PUnionSimpleTypeDef pdef)
{
	char	lineStr[1024+1];
	int	lineLen;
	int	ret;
	FILE	*fp;
	char	*ptr;
	
	if ((fileName == NULL) || (pdef == NULL))
	{
		UnionUserErrLog("in UnionReadSimpleTypeDefFromSpecFile:: parameter!\n");
		return(errCodeParameter);
	}
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadSimpleTypeDefFromSpecFile:: fopen [%s] error!\n",fileName);
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
			UnionSystemErrLog("in UnionReadSimpleTypeDefFromSpecFile:: UnionReadOneLineFromTxtStr error in [%s]\n",fileName);
			return(lineLen);
		}
		if (lineLen == 0)	// 空行
			continue;
		if (UnionIsUnixShellRemarkLine(lineStr))
			continue;
		if ((ret = UnionReadSimpleTypeDefFromStr(lineStr,lineLen,pdef)) < 0)
			continue;
		fclose(fp);
		return(0);
	}
	fclose(fp);
	return(errCodeCDPMDL_SimpleTypeNotDefined);
}

/* 从缺省定义文件读指定名称的简单类型的定义
输入参数
	nameOfType	结构类型
输出参数
	pdef	读出的结构定义
返回值：
	>=0 	读出的结构的大小
	<0	出错代码	
	
*/
int UnionReadSimpleTypeDefFromDefaultDefFile(char *nameOfType,PUnionSimpleTypeDef pdef)
{
	char	fileName[512+1];
	int	ret;
	
	memset(fileName,0,sizeof(fileName));
	UnionGetDefaultFileNameOfVarTypeDef(nameOfType,fileName);
	if ((ret = UnionReadSimpleTypeDefFromSpecFile(fileName,pdef)) < 0)
	{
		UnionUserErrLog("in UnionReadSimpleTypeDefFromDefaultDefFile:: UnionReadSimpleTypeDefFromSpecFile [%s] from [%s]\n",nameOfType,fileName);
		return(ret);
	}
	return(ret);
}


/* 将简单类型的定义写入到指定的文件中
输入参数
	pdef		简单类型定义
	fileName	文件名称
输出参数
	无
返回值：
	>=0 	读出的简单类型的大小
	<0	出错代码	
	
*/
int UnionWriteSimpleTypeDefIntoSpecFile(PUnionSimpleTypeDef pdef,char *fileName)
{
	FILE	*fp;
	int	ret;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if ((fileName == NULL) || (strlen(fileName) == 0))
		return(UnionWriteSimpleTypeDefIntoDefaultFile(pdef,""));
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionWriteSimpleTypeDefIntoSpecFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	if ((ret = UnionPrintSimpleTypeDefToFpInDefFormat(pdef,fp)) < 0)
	{
		UnionUserErrLog("in UnionWriteSimpleTypeDefIntoSpecFile:: UnionPrintSimpleTypeDefToFpInDefFormat [%s]\n",fileName);
		return(ret);
	}
	fclose(fp);
	return(ret);
}

/* 将简单类型的定义写入到缺省的文件中
输入参数
	pdef		简单类型定义
	programFileName	简单类型所在的程序名称
输出参数
	无
返回值：
	>=0 	读出的简单类型的大小
	<0	出错代码	
	
*/
int UnionWriteSimpleTypeDefIntoDefaultFile(PUnionSimpleTypeDef pdef,char *programFileName)
{
	char			fileName[256];
	TUnionVarTypeDef	varTypeDef;
	TUnionSimpleTypeDef	def;
	int			ret;
	
	if (pdef == NULL)
		return(errCodeParameter);

	// 是简单类型定义
	memset(&varTypeDef,0,sizeof(varTypeDef));
	strcpy(varTypeDef.nameOfType,pdef->name);
	if (!UnionExistsVarTypeDefInDefaultDefFile(varTypeDef.nameOfType))	// 未定义简单类型
	{
		varTypeDef.typeTag = conVarTypeTagSimpleType;
		UnionReadFileNameFromFullDir(programFileName,strlen(programFileName),varTypeDef.nameOfProgram);
		if ((ret = UnionWriteVarTypeDefIntoDefaultFile(&varTypeDef)) < 0)
		{
			UnionUserErrLog("in UnionReadSpecSimpleTypeDefFromCHeaderFile:: UnionWriteSimpleTypeDefToDefaultFile!\n");
			return(ret);
		}
	}
	else	// 已定义简单类型
	{
		// 当前的类型不是简单类型类型
		if ((ret = UnionGetTypeTagOfSpecNameOfType(varTypeDef.nameOfType)) != conVarTypeTagSimpleType)
		{
			UnionUserErrLog("in UnionWriteSimpleTypeDefIntoDefaultFile:: [%s] already exists!\n",varTypeDef.nameOfType);
			return(errCodeRECMDL_VarAlreadyExists);
		}
	}
	UnionGetDefaultFileNameOfVarTypeDef(varTypeDef.nameOfType,fileName);
	return(UnionWriteSimpleTypeDefIntoSpecFile(pdef,fileName));
}

/* 将简单类型的定义写入到缺省的文件中
输入参数
	nameOfType	简单类型定义
	name		类型名称
	remark		说明
	programFileName	简单类型所在的程序名称
输出参数
	无
返回值：
	>=0 	读出的简单类型的大小
	<0	出错代码	
	
*/
int UnionWriteSpecSimpleTypeDefIntoDefaultFile(char *nameOfType,char *name,char *remark,char *programFileName)
{
	TUnionSimpleTypeDef	def;
	
	memset(&def,0,sizeof(def));
	if ((nameOfType == NULL) || (strlen(nameOfType) == 0) || (name == NULL) || (strlen(name) == 0))
		return(errCodeParameter);
	strcpy(def.nameOfType,nameOfType);
	strcpy(def.name,name);
	if (remark != NULL)
		strcpy(def.remark,remark);
	return(UnionWriteSimpleTypeDefIntoDefaultFile(&def,programFileName));
}
