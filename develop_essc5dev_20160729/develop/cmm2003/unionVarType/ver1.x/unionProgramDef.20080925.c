// wolfang wang
// 2008/10/3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionProgramDef.h"
#include "UnionLog.h"

/*
功能	
	获得一个程序所属的模块名称
输入参数
	nameOfProgram	程序名称
输出参数
	nameOfModule	模块名称
	version		版本
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGetModuleNameAndVersionOfProgram(char *nameOfProgram,char *nameOfModule,char *version)
{
	int	ret;
	TUnionProgramDef	def;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadProgramDefFromDefaultFile(nameOfProgram,&def)) < 0)
	{
		UnionUserErrLog("in UnionGetModuleNameAndVersionOfProgram:: UnionReadProgramDefFromDefaultFile!\n");
		return(ret);
	}
	if (nameOfProgram != NULL)
		strcpy(nameOfModule,def.nameOfModule);
	if (version != NULL)
		strcpy(version,def.version);
	return(0);
}


/*
功能	
	从一个定义口串中读取一个程序定义
输入参数
	str		定义串
	lenOfStr	定义串的长度
输入出数
	pdef		变量定义
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadProgramDefFromStr(char *str,int lenOfStr,PUnionProgramDef pdef)
{
	int	ret;
	char	typeTag[128+1];
	
	memset(pdef,0,sizeof(*pdef));

	// 程序
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conProgramDefTagNameOfProgram,pdef->nameOfProgram,sizeof(pdef->nameOfProgram))) < 0)
	{
		UnionUserErrLog("in UnionReadProgramDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]!\n",conProgramDefTagNameOfProgram,str);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadProgramDefFromStr:: nameOfProgram must be defined!\n");
		return(errCodeCDPMDL_ProgramNotDefined);
	}
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conProgramDefTagNameOfModule,pdef->nameOfModule,sizeof(pdef->nameOfModule))) < 0)
	{
		UnionUserErrLog("in UnionReadProgramDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]!\n",conProgramDefTagNameOfModule,str);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadProgramDefFromStr:: nameOfProgram must be defined!\n");
		return(errCodeCDPMDL_ModuleNotDefined);
	}
	// 版本
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conProgramDefTagVersion,pdef->version,sizeof(pdef->version))) < 0)
	{
		UnionUserErrLog("in UnionReadProgramDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]!\n",conProgramDefTagVersion,str);
		return(ret);
	}
	UnionReadRecFldFromRecStr(str,lenOfStr,conProgramDefTagRemark,pdef->remark,sizeof(pdef->remark));
	return(0);
}


/* 将指定类型标识的定义打印到文件中
输入参数
	pdef	类型标识定义
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintProgramDefToFp(PUnionProgramDef pdef,FILE *fp)
{
	FILE	*outFp = stdout;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp != NULL)
		outFp = fp;
	
	fprintf(outFp,"nameOfProgram=%s|version=%s|nameOfModule,remark=%s|\n",pdef->nameOfProgram,pdef->version,pdef->nameOfModule,pdef->remark);
	return(0);
}

/* 将指定文件中定义的类型标识打印到文件中
输入参数
	fileName	文件名称
	nameOfProgram	类型名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintProgramDefInFileToFp(char *fileName,char *nameOfProgram,FILE *fp)
{
	TUnionProgramDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadProgramDefFromSpecFile(fileName,nameOfProgram,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintProgramDefInFileToFp:: UnionReadProgramDefFromSpecFile!\n");
		return(ret);
	}
	return(UnionPrintProgramDefToFp(&def,fp));
}
	
/* 将指定文件中定义的类型标识打印到屏幕上
输入参数
	pdef	类型标识定义
	nameOfProgram	类型名称
输出参数
	无
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionOutputProgramDefInFile(char *nameOfProgram,char *fileName)
{
	return(UnionPrintProgramDefInFileToFp(fileName,nameOfProgram,stdout));

}
