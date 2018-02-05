//	Wolfgang Wang
//	2008/2/25

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "unionVarValue.h"
#include "unionFunDef.h"
#include "unionGenFunTestProgram.h"
#include "unionAutoGenFileFunGrp.h"
#include "unionErrCode.h"
#include "unionDatagramDef.h"
#include "unionDatagramClassDef.h"
#include "UnionLog.h"

/*
功能	
	产生一个对报文域进行赋值的函数
输入参数
	funName		函数名称
	pdef		报文指针
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateSvrCmmPackFldSetCodes(char *funName,PUnionDatagramDef pdef,FILE *fp)
{
	int				ret;
	FILE				*outFp=stdout;
	int				index;

	if (pdef == NULL)
		return(errCodeParameter);
		
	if (fp != NULL)
		outFp = fp;
	fprintf(outFp,"\n        // 以下对响应报文进行赋值\n");
	fprintf(outFp,"        // 初始化响应报文\n");
	fprintf(outFp,"        UnionInitCmmPackFldDataList(ppackRes);\n");
	for (index = 0; index < pdef->fldNum; index++)
	{
		if (pdef->fldGrp[index].isRequest)
			continue;
		fprintf(outFp,"        if ((ret = UnionPutCmmPackFldIntoFldDataList(%s,strlen(%s),%s,ppackRes)) < 0)\n",pdef->fldGrp[index].fldID,pdef->fldGrp[index].alais,pdef->fldGrp[index].alais);
		fprintf(outFp,"        {\n");
		fprintf(outFp,"                UnionUserErrLog(\"in %s:: UnionPutCmmPackFldIntoFldDataList %s! ret = [%%d]\\n\",ret);\n",funName,pdef->fldGrp[index].fldID);
		fprintf(outFp,"                return(ret);\n");
		fprintf(outFp,"        }\n");
	}
	return(0);
}

/*
功能	
	产生一个从报文域读值代码的函数
输入参数
	funName		函数名称
	pdef		报文指针
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateSvrCmmPackFldReadCodes(char *funName,PUnionDatagramDef pdef,FILE *fp)
{
	int				ret;
	FILE				*outFp=stdout;
	int				index;
	int				isOptional;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp != NULL)
		outFp = fp;
	fprintf(outFp,"\n        // 以下从报文域读值\n");
	for (index = 0; index < pdef->fldNum; index++)
	{
		if (!pdef->fldGrp[index].isRequest)	// 不是请求报文
			continue;
		fprintf(outFp,"        if ((ret = UnionReadCmmPackFldFromFldDataList(ppackReq,%s,sizeof(%s),%s)) < 0)\n",pdef->fldGrp[index].fldID,pdef->fldGrp[index].alais,pdef->fldGrp[index].alais);
continueWrite:
		fprintf(outFp,"        {\n");
		fprintf(outFp,"                if (!(isOptional = %d))\n",pdef->fldGrp[index].optional);
		fprintf(outFp,"                {       // 必选域报错\n"); 
		fprintf(outFp,"                        UnionUserErrLog(\"in %s:: UnionReadCmmPackFldFromFldDataList %s! ret = [%%d]\\n\",ret);\n",funName,pdef->fldGrp[index].fldID);
		fprintf(outFp,"                        return(ret);\n");
		fprintf(outFp,"                }\n");
		fprintf(outFp,"        }\n");
	}
	return(0);
}

/*
功能	
	产生变量声明
输入参数
	pdef		报文指针
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateSvrCmmPackVarDeclarionCodes(PUnionDatagramDef pdef,FILE *fp)
{
	FILE	*outFp = stdout;
	int	index;
		
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp != NULL)
		outFp = fp;
	fprintf(outFp,"{\n");
	for (index = 0; index < pdef->fldNum; index++)
	{
		fprintf(outFp,"        char        %s[%d+1];\n",pdef->fldGrp[index].alais,pdef->fldGrp[index].len);
	}
	fprintf(outFp,"        int         ret;\n");
	fprintf(outFp,"        int         isOptional;\n");
	fprintf(outFp,"\n");
	fprintf(outFp,"        // 初始化变量\n");
	for (index = 0; index < pdef->fldNum; index++)
	{
		fprintf(outFp,"        memset(%s,0,sizeof(%s));\n",pdef->fldGrp[index].alais,pdef->fldGrp[index].alais);
	}
	return(0);
}

/*
功能	
	产生与服务器通讯的代码
输入参数
	fp		文件句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateNullCmmPackFunCodes(FILE *fp)
{
	FILE	*outFp = stdout;
	int	ret;
	
	if (fp != NULL)
		outFp = fp;

	fprintf(outFp,"\n        // 在此处增加功能代码\n");
	fprintf(outFp,"\n");
	fprintf(outFp,"        // 功能代码增加结束\n\n");
	return(0);
}	

/*
功能	
	拼装函数名称
输入参数
	appID		应用用途
	datagramID	报文标识
输入出数
	funName		函数名称
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateSvrCmmPackFunNameOfService(char *appID,char *datagramID,char *funName)
{
	TUnionDatagramDef	def;
	int			ret;
		
	// 读函数定义
	memset(&def,0,sizeof(&def));
	if ((ret = UnionReadSpecDatagramDefFromFile(appID,datagramID,&def)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSvrCmmPackFunNameOfService:: UnionReadSpecDatagramDefFromFile [%s][%s]!\n",appID,datagramID);
		return(ret);
	}
	sprintf(funName,"UnionService%s",def.serviceID);
	return(0);
}

/*
功能	
	拼装程序名称
输入参数
	appID		应用用途
	datagramID	报文标识
	version		程序版本
输入出数
	programName	程序全名名称
	incFileConf	配置头文件的文件名称
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateSvrCmmPackProgramNameOfService(char *appID,char *datagramID,char *version,char *incFileConf,char *programName)
{
	char	devDir[256+1];
	int	ret;
	char	nameOfModuleOfSvr[64+1];
	char	funName[128+1];
	
	memset(nameOfModuleOfSvr,0,sizeof(nameOfModuleOfSvr));
	if ((ret = UnionGetNameOfModuleSvrOfSpecDatagramClass(appID,nameOfModuleOfSvr)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSvrCmmPackProgramNameOfService:: UnionGetNameOfModuleSvrOfSpecDatagramClass!\n");
		return(ret);
	}
	memset(devDir,0,sizeof(devDir));
	if ((ret = UnionGetDevDirAndIncFileConfOfModule(nameOfModuleOfSvr,devDir,incFileConf)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSvrCmmPackProgramNameOfService:: UnionGetDevDirAndIncFileConfOfModule [%s]!\n",nameOfModuleOfSvr);
		return(ret);
	}
	memset(funName,0,sizeof(funName));
	if ((ret = UnionGenerateSvrCmmPackFunNameOfService(appID,datagramID,funName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSvrCmmPackProgramNameOfService:: UnionGenerateSvrCmmPackFunNameOfService!\n");
		return(ret);
	}
	sprintf(programName,"%s/%s.%s.c",devDir,funName,version);
	return(0);
}

/*
功能	
	生成一个服务端函数
输入参数
	appID		应用用途
	datagramID	报文标识
	incConfFileName	生成的函数要使用的头文件
	fp		文件名柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateSvrFunUsingCmmPackToFp(char *appID,char *datagramID,char *incConfFileName,FILE *fp)
{
	int			ret;
	FILE			*outFp = stdout;
	TUnionDatagramDef	def;
	int			index;
	char			funName[128+1];
	
	// 读函数定义
	memset(&def,0,sizeof(&def));
	if ((ret = UnionReadSpecDatagramDefFromFile(appID,datagramID,&def)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSvrFunUsingCmmPackToFp:: UnionReadSpecDatagramDefFromFile [%s][%s]!\n",appID,datagramID);
		return(ret);
	}
	if (fp != NULL)
		outFp = fp;

	// 写包含的头文件信息
	if ((ret = UnionAddIncludeFileToFile(incConfFileName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSvrFunUsingCmmPackToFp:: UnionAddIncludeFileToFile!\n");
		goto errExit;
	}
	// 包含打包文件
	fprintf(outFp,"#ifndef _unionCmmPackData_\n");
	fprintf(outFp,"#include \"unionCmmPackData.h\"\n");
	fprintf(outFp,"#endif\n");
	fprintf(outFp,"\n");
	
	// 拼装函数名称
	memset(funName,0,sizeof(funName));
	if ((ret = UnionGenerateSvrCmmPackFunNameOfService(appID,datagramID,funName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSvrFunUsingCmmPackToFp:: UnionGenerateSvrCmmPackFunNameOfService [%s]!\n",funName);
		goto errExit;
	}
	// 将函数定义写入到程序中
	fprintf(outFp,"int %s(PUnionCmmPackData ppackReq,PUnionCmmPackData ppackRes)\n",funName);

	// 产生变量声明代码
	if ((ret = UnionGenerateSvrCmmPackVarDeclarionCodes(&def,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSvrFunUsingCmmPackToFp:: UnionGenerateSvrCmmPackVarDeclarionCodes [%s]!\n",funName);
		goto errExit;
	}

	// 产生从报文读值的代码
	if ((ret = UnionGenerateSvrCmmPackFldReadCodes(funName,&def,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSvrFunUsingCmmPackToFp:: UnionGenerateSvrCmmPackFldReadCodes [%s]!\n",funName);
		goto errExit;
	}
	
	// 产生功能代码
	if ((ret = UnionGenerateNullCmmPackFunCodes(fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSvrFunUsingCmmPackToFp:: UnionGenerateNullCmmPackFunCodes [%s]!\n",funName);
		goto errExit;
	}

	// 产生报文赋值代码
	if ((ret = UnionGenerateSvrCmmPackFldSetCodes(funName,&def,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSvrFunUsingCmmPackToFp:: UnionGenerateSvrCmmPackFldSetCodes [%s]!\n",funName);
		goto errExit;
	}


	// 产生结尾代码
	fprintf(outFp,"\n");
	fprintf(outFp,"        return(ret);\n");
	fprintf(outFp,"}\n\n");
errExit:
	if (fp != stdout)
		fclose(fp);
	return(ret);
}

/*
功能	
	生成一个服务端函数
输入参数
	appID		应用用途
	datagramID	报文标识
	version		程序版本
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateSvrFunUsingCmmPack(char *appID,char *datagramID,char *version)
{
	char	fileName[256+1];
	char	incFileConf[256+1];
	int	ret;
	FILE	*fp;
	
	memset(fileName,0,sizeof(fileName));
	memset(incFileConf,0,sizeof(incFileConf));
	if ((ret = UnionGenerateSvrCmmPackProgramNameOfService(appID,datagramID,version,incFileConf,fileName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSvrFunUsingCmmPack:: UnionGenerateSvrCmmPackProgramNameOfService!\n");
		return(ret);
	}
	if ((fp = fopen(fileName,"w")) == 0)
	{
		UnionSystemErrLog("in UnionGenerateSvrFunUsingCmmPack:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	if ((ret = UnionGenerateSvrFunUsingCmmPackToFp(appID,datagramID,incFileConf,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSvrFunUsingCmmPack:: UnionGenerateSvrFunUsingCmmPackToFp [%s][%s]!\n",appID,datagramID);
		fclose(fp);
		return(ret);
	}
	fclose(fp);
	return(0);
}

