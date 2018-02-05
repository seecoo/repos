//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#include <stdio.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionStructDefFile.h"
#include "unionSimpleTypeDefFile.h"
#include "UnionStr.h"
#include "unionVarTypeDef.h"

/* 从指定文件读指定名称的结构的定义
输入参数
	fileName	文件名称
输出参数
	pdef	读出的结构定义
返回值：
	>=0 	读出的结构的大小
	<0	出错代码	
	
*/
int UnionReadStructDefFromSpecFile(char *fileName,PUnionStructDef pdef)
{
	char	lineStr[1024+1];
	int	lineLen;
	int	ret;
	FILE	*fp;
	char	*ptr;
	
	if ((fileName == NULL) || (pdef == NULL))
	{
		UnionUserErrLog("in UnionReadStructDefFromSpecFile:: parameter!\n");
		return(errCodeParameter);
	}
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadStructDefFromSpecFile:: fopen [%s] error!\n",fileName);
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
			UnionSystemErrLog("in UnionReadStructDefFromSpecFile:: UnionReadOneLineFromTxtStr error in [%s]\n",fileName);
			return(lineLen);
		}
		if (lineLen == 0)	// 空行
			continue;
		if (UnionIsUnixShellRemarkLine(lineStr))
			continue;
		if (strncmp(lineStr,conStructDeclareLineTag,strlen(conStructDeclareLineTag)) == 0)
		{
			if ((ret = UnionReadStructDeclareDefFromStr(lineStr,lineLen,&(pdef->declareDef))) < 0)
			{
				UnionUserErrLog("in UnionReadStructDefFromSpecFile:: declare error in [%s]\n",fileName);
				fclose(fp);
				return(errCodeParameter);
			}
			continue;
		}
		if (pdef->fldNum >= conMaxNumOfVarPerStruct)	// 域定义太多
		{
			UnionUserErrLog("in UnionReadStructDefFromSpecFile:: too many fldNum [%d] defined in [%s]\n",pdef->fldNum,fileName);
			fclose(fp);
			return(errCodeParameter);
		}
		if ((ret = UnionReadVarDefFromStr(lineStr,lineLen,&(pdef->fldGrp[pdef->fldNum]))) < 0)
		{
			UnionAuditLog("in UnionReadStructDefFromSpecFile:: UnionReadVarDefFromStr from [%s]\n",lineStr);
			//fclose(fp);
			//return(ret);
			continue;
		}
		pdef->fldNum += 1;
	}
	fclose(fp);
	return(0);
}

/* 从缺省定义文件读指定名称的结构的定义
输入参数
	nameOfType	结构类型
输出参数
	pdef	读出的结构定义
返回值：
	>=0 	读出的结构的大小
	<0	出错代码	
	
*/
int UnionReadStructDefFromDefaultDefFile(char *nameOfType,PUnionStructDef pdef)
{
	char	fileName[512+1];
	int	ret;
	
	memset(fileName,0,sizeof(fileName));
	UnionGetDefaultFileNameOfVarTypeDef(nameOfType,fileName);
	if ((ret = UnionReadStructDefFromSpecFile(fileName,pdef)) < 0)
	{
		UnionUserErrLog("in UnionReadStructDefFromDefaultDefFile:: UnionReadStructDefFromSpecFile [%s] from [%s]\n",nameOfType,fileName);
		return(ret);
	}
	return(ret);
}

/* 将结构的定义写入到指定的文件中
输入参数
	pdef		结构定义
	fileName	文件名称
输出参数
	无
返回值：
	>=0 	读出的结构的大小
	<0	出错代码	
	
*/
int UnionWriteStructDefIntoSpecFile(PUnionStructDef pdef,char *fileName)
{
	FILE	*fp;
	int	ret;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if ((fileName == NULL) || (strlen(fileName) == 0))
		return(UnionWriteStructDefIntoDefaultFile(pdef,""));
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionWriteStructDefIntoSpecFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	if ((ret = UnionPrintStructDefToFpInDefFormat(pdef,fp)) < 0)
	{
		UnionUserErrLog("in UnionWriteStructDefIntoSpecFile:: UnionPrintStructDefToFpInDefFormat [%s]\n",fileName);
		return(ret);
	}
	fclose(fp);
	return(ret);
}

/* 将结构的定义写入到缺省的文件中
输入参数
	pdef		结构定义
	programFileName	结构所在的程序名称
输出参数
	无
返回值：
	>=0 	读出的结构的大小
	<0	出错代码	
	
*/
int UnionWriteStructDefIntoDefaultFile(PUnionStructDef pdef,char *programFileName)
{
	char			fileName[256];
	TUnionVarTypeDef	varTypeDef;
	TUnionSimpleTypeDef	simpleTypeDef;
	TUnionStructDef		def;
	int			ret;
	
	if (pdef == NULL)
		return(errCodeParameter);

	memset(&varTypeDef,0,sizeof(varTypeDef));
	if (strlen(pdef->declareDef.typeDefName) != 0)
		strcpy(varTypeDef.nameOfType,pdef->declareDef.typeDefName);
	else
		strcpy(varTypeDef.nameOfType,pdef->declareDef.structName);
	varTypeDef.typeTag = conVarTypeTagStruct;
	UnionReadFileNameFromFullDir(programFileName,strlen(programFileName),varTypeDef.nameOfProgram);

	if (pdef->fldNum == 0)	// 是裸的typedef定义
	{
		if (strcmp(pdef->declareDef.typeDefName,pdef->declareDef.structName) != 0)	// 两个名称不相同，是使用typedef对一个struct进行了名称替换
		{
			memset(&simpleTypeDef,0,sizeof(simpleTypeDef));
			strcpy(simpleTypeDef.nameOfType,pdef->declareDef.structName);
			strcpy(simpleTypeDef.name,pdef->declareDef.typeDefName);
			strcpy(simpleTypeDef.remark,pdef->declareDef.remark);
			return(UnionWriteSimpleTypeDefIntoDefaultFile(&simpleTypeDef,programFileName));
		}
		// 是使用typedef将结构名称置为了可以直接使用
		if (!UnionExistsVarTypeDefInDefaultDefFile(pdef->declareDef.structName))	// 未定义结构
		{
			// 仅增加类型定义
			if ((ret = UnionWriteVarTypeDefIntoDefaultFile(&varTypeDef)) < 0)
			{
				UnionUserErrLog("in UnionReadSpecStructDefFromCHeaderFile:: UnionWriteStructDefToDefaultFile!\n");
				return(ret);
			}
			return(ret);
		}
		// 当前的类型不是结构类型
		if ((ret = UnionGetTypeTagOfSpecNameOfType(varTypeDef.nameOfType)) != conVarTypeTagStruct)
		{
			UnionUserErrLog("in UnionWriteStructDefIntoDefaultFile:: [%s] already exists!\n",varTypeDef.nameOfType);
			return(errCodeRECMDL_VarAlreadyExists);
		}
		// 读现在的定义
		memset(&def,0,sizeof(def));
		if ((ret = UnionReadStructDefFromDefaultDefFile(varTypeDef.nameOfType,&def)) < 0)
		{
			UnionUserErrLog("in UnionWriteStructDefIntoDefaultFile:: UnionReadStructDefFromDefaultDefFile [%s]!\n",varTypeDef.nameOfType);
			return(ret);
		}
		strcpy(def.declareDef.typeDefName,pdef->declareDef.typeDefName);
		UnionGetDefaultFileNameOfVarTypeDef(def.declareDef.structName,fileName);
		return(UnionWriteStructDefIntoSpecFile(&def,fileName));
	}
	
	// 是结构定义
	if (!UnionExistsVarTypeDefInDefaultDefFile(varTypeDef.nameOfType))	// 未定义结构
	{
		if ((ret = UnionWriteVarTypeDefIntoDefaultFile(&varTypeDef)) < 0)
		{
			UnionUserErrLog("in UnionReadSpecStructDefFromCHeaderFile:: UnionWriteStructDefToDefaultFile!\n");
			return(ret);
		}
	}
	else	// 已定义结构
	{
		// 当前的类型不是结构类型
		if ((ret = UnionGetTypeTagOfSpecNameOfType(varTypeDef.nameOfType)) != conVarTypeTagStruct)
		{
			UnionUserErrLog("in UnionWriteStructDefIntoDefaultFile:: [%s] already exists!\n",varTypeDef.nameOfType);
			return(errCodeRECMDL_VarAlreadyExists);
		}
		if (strlen(pdef->declareDef.typeDefName) == 0)
			strcpy(pdef->declareDef.typeDefName,pdef->declareDef.structName);
	}
	UnionGetDefaultFileNameOfVarTypeDef(varTypeDef.nameOfType,fileName);
	return(UnionWriteStructDefIntoSpecFile(pdef,fileName));
}
