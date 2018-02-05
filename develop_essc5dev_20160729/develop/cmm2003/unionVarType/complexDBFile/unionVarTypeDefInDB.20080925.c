// wolfang wang
// 2008/10/3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionVarTypeDefFile.h"
#include "UnionLog.h"

/*
功能	
	获得一个关键字所在的程序名称
输入参数
	keyWord		函数名称
输出参数
	programName	程序名称
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGetProgramFileNameOfKeyWord(char *keyWord,char *nameOfProgram)
{
	int	ret;
	TUnionVarTypeDef	def;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadVarTypeDefFromDefaultFile(keyWord,&def)) < 0)
	{
		UnionUserErrLog("in UnionGetProgramFileNameOfKeyWord:: UnionReadVarTypeDefFromDefaultFile!\n");
		return(ret);
	}
	if (nameOfProgram != NULL)
		strcpy(nameOfProgram,def.nameOfProgram);
	return(0);
}

/* 判断是否存在一个类型的定义
输入参数
	fileName	文件名称
	nameOfType	指定的类型
输出参数
	无
返回值：
	>=1 	存在
	0	不存在	
	<0	出错
	
*/
int UnionExistsVarTypeDefInSpecFile(char *fileName,char *nameOfType)
{
	char			lineStr[1024+1];
	int			lineLen;
	int			ret;
	FILE			*fp;
	TUnionVarTypeDef	def;
	
	if (fileName == NULL)
	{
		UnionUserErrLog("in UnionExistsVarTypeDefInSpecFile:: parameter!\n");
		return(errCodeParameter);
	}
	if ((fileName == NULL) || (strlen(fileName) == 0) || (strcmp(fileName,"null") == 0))
		return(UnionExistsVarTypeDefInDefaultDefFile(nameOfType));
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionExistsVarTypeDefInSpecFile:: fopen [%s] error!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	memset(&def,0,sizeof(def));
	while (!feof(fp))
	{
		// 从文件中读取一行
		memset(lineStr,0,sizeof(lineStr));
		if ((lineLen = UnionReadOneDataLineFromTxtFile(fp,lineStr,sizeof(lineStr))) < 0)
		{
			if ((lineLen == errCodeEnviMDL_NullLine) || (lineLen == errCodeFileEnd))	// 空行
				continue;
			UnionSystemErrLog("in UnionExistsVarTypeDefInSpecFile:: UnionReadOneDataLineFromTxtFile error in [%s]\n",fileName);
			fclose(fp);
			return(lineLen);
		}
		if (lineLen == 0)	// 空行
			continue;
		if ((ret = UnionReadVarTypeDefFromStr(lineStr,lineLen,&def)) < 0)
			continue;
		if (strcmp(nameOfType,def.nameOfType) == 0)
		{
			fclose(fp);
			return(1);
		}
	}
	fclose(fp);
	return(0);
}

/* 判断缺省定义文件中是否存在一个类型的定义
输入参数
	nameOfType	指定的类型
输出参数
	无
返回值：
	>=1 	存在
	0	不存在	
	<0	出错
	
*/
int UnionExistsVarTypeDefInDefaultDefFile(char *nameOfType)
{
	char       record[1024+1];
        char       condition[1024+1];
        int        ret;

        memset(record, 0, sizeof record);
        memset(condition, 0, sizeof condition);

        strcpy(condition, "nameOfType=");
        strcat(condition, nameOfType);
        strcat(condition, "|");

        ret = UnionSelectUniqueObjectRecordByPrimaryKey("keyWord", condition, record, sizeof(record));
        if (ret == errCodeObjectMDL_ObjectDefinitionNotExists)
        {
        	return 0;
        }
        return ret;

}	

/* 从指定文件读指定名称的类型标识的定义
输入参数
	fileName	文件名称
	nameOfType	指定的类型
输出参数
	pdef	读出的类型标识定义
返回值：
	>=0 	读出的类型标识的大小
	<0	出错代码	
	
*/
int UnionReadVarTypeDefFromSpecFile(char *fileName,char *nameOfType,PUnionVarTypeDef pdef)
{
	char	lineStr[1024+1];
	int	lineLen;
	int	ret;
	FILE	*fp;
	
	if ((fileName == NULL) || (pdef == NULL))
	{
		UnionUserErrLog("in UnionReadVarTypeDefFromSpecFile:: parameter!\n");
		return(errCodeParameter);
	}
	if ((fileName == NULL) || (strlen(fileName) == 0) || (strcmp(fileName,"null") == 0))
		return(UnionReadVarTypeDefFromDefaultFile(nameOfType,pdef));
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadVarTypeDefFromSpecFile:: fopen [%s] error!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	memset(pdef,0,sizeof(*pdef));
	while (!feof(fp))
	{
		// 从文件中读取一行
		memset(lineStr,0,sizeof(lineStr));
		if ((lineLen = UnionReadOneLineFromTxtStr(fp,lineStr,sizeof(lineStr))) < 0)
		{
			if ((lineLen == errCodeEnviMDL_NullLine) || (lineLen == errCodeFileEnd))	// 空行
				continue;
			UnionSystemErrLog("in UnionReadVarTypeDefFromSpecFile:: UnionReadOneLineFromTxtStr error in [%s]\n",fileName);
			fclose(fp);
			return(lineLen);
		}
		if (lineLen == 0)	// 空行
			continue;
		if (UnionIsUnixShellRemarkLine(lineStr))
			continue;
		if ((ret = UnionReadVarTypeDefFromStr(lineStr,lineLen,pdef)) < 0)
			continue;
		if (strcmp(nameOfType,pdef->nameOfType) == 0)
		{
			fclose(fp);
			return(0);
		}
	}
	fclose(fp);
	UnionUserErrLog("in UnionReadVarTypeDefFromSpecFile:: [%s] not defined in [%s]\n",nameOfType,fileName);
	return(errCodeCDPMDL_VarTypeNotDefined);
}

/* 从缺省文件读指定名称的类型标识的定义
输入参数
	nameOfType	指定的类型
输出参数
	pdef	读出的类型标识定义
返回值：
	>=0 	读出的类型标识的大小
	<0	出错代码	
	
*/
int UnionReadVarTypeDefFromDefaultFile(char *nameOfType,PUnionVarTypeDef pdef)
{
	char       record[1024+1];
        char       condition[1024+1];
        int        ret;

        memset(record, 0, sizeof record);
        memset(condition, 0, sizeof condition);

        strcpy(condition, "nameOfType=");
        strcat(condition, nameOfType);
        strcat(condition, "|");
        /* 从表里取出一条record */
        if ((ret = UnionSelectUniqueObjectRecordByPrimaryKey("keyWord", condition, record, sizeof(record)))<0)

        {
                UnionUserErrLog("in UnionReadVarTypeDefFromDefaultFile:: UnionReadVarTypeDefFromDefaultFile!\n");
                return(ret);
        }
        
        /* 给关键字名称赋值 */
        strcpy(pdef->nameOfType, nameOfType);

        /* 给关键字类型赋值 */
        UnionReadIntTypeRecFldFromRecStr( record, strlen(record), "typeTag", &(pdef->typeTag) );

        /* 给关键字所属模块赋值 */
        UnionReadRecFldFromRecStr(record, strlen(record), "nameOfProgram", pdef->nameOfProgram, sizeof (pdef->nameOfProgram));

        return(0);
}

/* 将类型标识的定义写入到指定的文件中
输入参数
	pdef		类型标识定义
	fileName	文件名称
输出参数
	无
返回值：
	>=0 	读出的类型标识的大小
	<0	出错代码	
	
*/
int UnionWriteVarTypeDefIntoSpecFile(PUnionVarTypeDef pdef,char *fileName)
{
	FILE	*fp;
	int	ret;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if ((ret = UnionExistsVarTypeDefInSpecFile(fileName,pdef->nameOfType)) < 0)
	{
		UnionUserErrLog("in UnionWriteVarTypeDefIntoSpecFile:: UnionExistsVarTypeDefInSpecFile [%s]\n",pdef->nameOfType);
		return(ret);
	}
	if (ret > 0)
	{
		UnionUserErrLog("in UnionWriteVarTypeDefIntoSpecFile:: [%s] already exists!\n",pdef->nameOfType);		
		return(errCodeRECMDL_VarAlreadyExists);
	}
	if ((fileName == NULL) || (strlen(fileName) == 0))
		return(UnionWriteVarTypeDefIntoDefaultFile(pdef));
	if ((fp = fopen(fileName,"a+")) == NULL)
	{
		UnionSystemErrLog("in UnionWriteVarTypeDefIntoSpecFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	if ((ret = UnionPrintVarTypeDefToFpInDefFormat(pdef,fp)) < 0)
	{
		UnionUserErrLog("in UnionWriteVarTypeDefIntoSpecFile:: UnionPrintVarTypeDefToFpInDefFormat [%s]\n",fileName);
		return(ret);
	}
	fclose(fp);
	return(ret);
}

/* 将类型标识的定义写入到缺省的文件中
输入参数
	pdef		类型标识定义
	fileName	文件名称
输出参数
	无
返回值：
	>=0 	读出的类型标识的大小
	<0	出错代码	
	
*/
int UnionWriteVarTypeDefIntoDefaultFile(PUnionVarTypeDef pdef)
{
	char	record[1024+1];

	memset(record, 0, sizeof record);

	if (NULL == pdef)
		return (errCodeParameter);

	sprintf(record, "%s=%s|%s=%d|%s=%s|", "nameOfType",pdef->nameOfType,"typeTag",pdef->typeTag,"nameOfProgram",pdef->nameOfProgram);

	return (UnionInsertObjectRecord("keyWord", record, strlen(record)));
}
