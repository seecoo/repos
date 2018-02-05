//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#include <stdio.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionVarTypeDef.h"
#include "unionPointerDef.h"
#include "UnionStr.h"
#include "unionDefaultVarTypeDefDir.h"


/* 从缺省文件读指定名称的指针的定义
输入参数
	nameOfType	指针类型的名称
输出参数
	pdef	读出的指针定义
返回值：
	>=0 	成功
	<0	出错代码	
	
*/
int UnionReadPointerDefFromDefaultDefFile(char *nameOfType,PUnionPointerDef pdef)
{
	char			fileName[256+1];

	UnionGetDefaultFileNameOfVarTypeDef(nameOfType,fileName);
	return(UnionReadPointerDefFromSpecFile(fileName,pdef));
}

/* 从指定文件读指定名称的指针的定义
输入参数
	fileName	文件名称
输出参数
	pdef	读出的指针定义
返回值：
	>=0 	读出的指针的大小
	<0	出错代码	
	
*/
int UnionReadPointerDefFromSpecFile(char *fileName,PUnionPointerDef pdef)
{
	char	lineStr[1024+1];
	int	lineLen;
	int	ret;
	FILE	*fp;
	char	*ptr;
	
	if ((fileName == NULL) || (pdef == NULL))
	{
		UnionUserErrLog("in UnionReadPointerDefFromSpecFile:: parameter!\n");
		return(errCodeParameter);
	}
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadPointerDefFromSpecFile:: fopen [%s] error!\n",fileName);
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
			UnionSystemErrLog("in UnionReadPointerDefFromSpecFile:: UnionReadOneLineFromTxtStr error in [%s]\n",fileName);
			return(lineLen);
		}
		if (lineLen == 0)	// 空行
			continue;
		if (UnionIsUnixShellRemarkLine(lineStr))
			continue;
		if ((ret = UnionReadPointerDefFromStr(lineStr,lineLen,pdef)) < 0)
			continue;
		fclose(fp);
		return(0);
	}
	fclose(fp);
	return(errCodeCDPMDL_PointerNotDefined);
}

/* 将指针类型的定义写入到指定的文件中
输入参数
	pdef		指针类型定义
	fileName	文件名称
输出参数
	无
返回值：
	>=0 	读出的指针类型的大小
	<0	出错代码	
	
*/
int UnionWritePointerDefIntoSpecFile(PUnionPointerDef pdef,char *fileName)
{
	FILE	*fp;
	int	ret;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if ((fileName == NULL) || (strlen(fileName) == 0))
		return(UnionWritePointerDefIntoDefaultFile(pdef,""));
	UnionPrintPointerDefToFp(pdef,stdout);
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionWritePointerDefIntoSpecFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	if ((ret = UnionPrintPointerDefToFpInDefFormat(pdef,fp)) < 0)
	{
		UnionUserErrLog("in UnionWritePointerDefIntoSpecFile:: UnionPrintPointerDefToFpInDefFormat [%s]\n",fileName);
		return(ret);
	}
	fclose(fp);
	return(ret);
}

/* 将指针类型的定义写入到缺省的文件中
输入参数
	pdef		指针类型定义
	programFileName	指针类型所在的程序名称
输出参数
	无
返回值：
	>=0 	读出的指针类型的大小
	<0	出错代码	
	
*/
int UnionWritePointerDefIntoDefaultFile(PUnionPointerDef pdef,char *programFileName)
{
	char			fileName[256];
	TUnionVarTypeDef	varTypeDef;
	TUnionPointerDef	def;
	int			ret;
	
	if (pdef == NULL)
		return(errCodeParameter);


	// 是指针类型定义
	memset(&varTypeDef,0,sizeof(varTypeDef));
	strcpy(varTypeDef.nameOfType,pdef->name);
	if (!UnionExistsVarTypeDefInDefaultDefFile(varTypeDef.nameOfType))	// 未定义指针类型
	{
		varTypeDef.typeTag = conVarTypeTagPointer;
		UnionReadFileNameFromFullDir(programFileName,strlen(programFileName),varTypeDef.nameOfProgram);
		if ((ret = UnionWriteVarTypeDefIntoDefaultFile(&varTypeDef)) < 0)
		{
			UnionUserErrLog("in UnionReadSpecPointerDefFromCHeaderFile:: UnionWritePointerDefToDefaultFile!\n");
			return(ret);
		}
	}
	else	// 已定义指针类型
	{
		// 当前的类型不是指针类型类型
		if ((ret = UnionGetTypeTagOfSpecNameOfType(varTypeDef.nameOfType)) != conVarTypeTagPointer)
		{
			UnionUserErrLog("in UnionWritePointerDefIntoDefaultFile:: [%s] already exists!\n",varTypeDef.nameOfType);
			return(errCodeRECMDL_VarAlreadyExists);
		}
	}
	UnionGetDefaultFileNameOfVarTypeDef(varTypeDef.nameOfType,fileName);
	return(UnionWritePointerDefIntoSpecFile(pdef,fileName));
}

/* 将指针类型的定义写入到缺省的文件中
输入参数
	nameOfType	指针类型定义
	name		类型名称
	remark		说明
	programFileName	指针类型所在的程序名称
输出参数
	无
返回值：
	>=0 	读出的指针类型的大小
	<0	出错代码	
	
*/
int UnionWriteSpecPointerDefIntoDefaultFile(char *nameOfType,char *name,char *remark,char *programFileName)
{
	TUnionPointerDef	def;
	
	memset(&def,0,sizeof(def));
	if ((nameOfType == NULL) || (strlen(nameOfType) == 0) || (name == NULL) || (strlen(name) == 0))
		return(errCodeParameter);
	strcpy(def.nameOfType,nameOfType);
	strcpy(def.name,name);
	if (remark != NULL)
		strcpy(def.remark,remark);
	return(UnionWritePointerDefIntoDefaultFile(&def,programFileName));
}


