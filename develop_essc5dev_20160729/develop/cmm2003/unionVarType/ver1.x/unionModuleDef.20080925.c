// wolfang wang
// 2008/10/3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionModuleDef.h"
#include "UnionLog.h"

/*
功能	
	获得一个模块的开发目录和头文件配置
输入参数
	nameOfModulde	模块名称
输出参数
	devDir		开发目录
	incFileConf	头文件配置
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGetDevDirAndIncFileConfOfModule(char *nameOfModule,char *devDir,char *incFileConf)
{
	int	ret;
	TUnionModuleDef	def;
	char	fullDir[256+1];
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadModuleDefFromDefaultFile(nameOfModule,&def)) < 0)
	{
		UnionUserErrLog("in UnionGetDevDirAndIncFileConfOfModule:: UnionReadModuleDefFromDefaultFile!\n");
		return(ret);
	}
	if (devDir != NULL)
	{
		memset(fullDir,0,sizeof(fullDir));
		if ((ret = UnionReadDirFromStr(def.devDir,-1,fullDir)) < 0)
		{
			UnionUserErrLog("in UnionGetDevDirAndIncFileConfOfModule:: UnionReadDirFromStr [%s]!\n",def.devDir);
			return(ret);
		}
		//UnionLog("devDir = [%s] fullDir = [%s] ret = [%d]\n",def.devDir,fullDir,ret);
		strcpy(devDir,fullDir);
	}
	if (incFileConf != NULL)
		strcpy(incFileConf,def.incFileConf);
	return(0);
}

/*
功能	
	从一个定义口串中读取一个模块定义
输入参数
	str		定义串
	lenOfStr	定义串的长度
输出参数
	pdef		变量定义
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadModuleDefFromStr(char *str,int lenOfStr,PUnionModuleDef pdef)
{
	int	ret;
	char	typeTag[128+1];
	
	memset(pdef,0,sizeof(*pdef));

	// 模块
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conModuleDefTagNameOfModule,pdef->nameOfModule,sizeof(pdef->nameOfModule))) < 0)
	{
		UnionUserErrLog("in UnionReadModuleDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]!\n",conModuleDefTagNameOfModule,str);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadModuleDefFromStr:: nameOfModule must be defined!\n");
		return(errCodeCDPMDL_ModuleNotDefined);
	}
	// 开发目录
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conModuleDefTagDevDir,pdef->devDir,sizeof(pdef->devDir))) < 0)
	{
		UnionUserErrLog("in UnionReadModuleDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]!\n",conModuleDefTagDevDir,str);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadModuleDefFromStr:: nameOfModule must be defined!\n");
		return(errCodeCDPMDL_ModuleDevDirNotDefined);
	}
	UnionReadRecFldFromRecStr(str,lenOfStr,conModuleDefTagIncFileConf,pdef->incFileConf,sizeof(pdef->incFileConf));
	UnionReadRecFldFromRecStr(str,lenOfStr,conModuleDefTagRemark,pdef->remark,sizeof(pdef->remark));
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
int UnionPrintModuleDefToFp(PUnionModuleDef pdef,FILE *fp)
{
	FILE	*outFp = stdout;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp != NULL)
		outFp = fp;
	
	fprintf(outFp,"nameOfModule=%s|devDir=%s|incFileConf=%s|remark=%s|\n",pdef->nameOfModule,pdef->devDir,pdef->incFileConf,pdef->remark);
	return(0);
}

/* 将指定文件中定义的类型标识打印到文件中
输入参数
	fileName	文件名称
	nameOfModule	类型名称
输出参数
	fp		文件句柄
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionPrintModuleDefInFileToFp(char *fileName,char *nameOfModule,FILE *fp)
{
	TUnionModuleDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadModuleDefFromSpecFile(fileName,nameOfModule,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintModuleDefInFileToFp:: UnionReadModuleDefFromSpecFile!\n");
		return(ret);
	}
	return(UnionPrintModuleDefToFp(&def,fp));
}
	
/* 将指定文件中定义的类型标识打印到屏幕上
输入参数
	pdef	类型标识定义
	nameOfModule	类型名称
输出参数
	无
返回值：
	>=0 	打印的域数量
	<0	出错代码	
	
*/
int UnionOutputModuleDefInFile(char *nameOfModule,char *fileName)
{
	return(UnionPrintModuleDefInFileToFp(fileName,nameOfModule,stdout));

}
