// wolfang wang
// 2008/10/3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionVarDef.h"
#include "unionFunDefFile.h"
#include "UnionLog.h"

/*
功能	
	从缺省定义文件读取一个函数定义
输入参数
	funName		函数名称
输出参数
	pdef		读出的函数定义
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadFunDefFromDefaultDefFile(char *funName,PUnionFunDef pdef)
{
	char	fileName[512+1];
	
	memset(fileName,0,sizeof(fileName));
	UnionGetDefaultFileNameOfFunDef(funName,fileName);
	return(UnionReadFunDefFromSpecFile(fileName,pdef));
}

/*
功能	
	从文件读取一个函数定义
输入参数
	fileName	文件名称
输出参数
	pdef		读出的函数定义
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadFunDefFromSpecFile(char *fileName,PUnionFunDef pdef)
{
	FILE			*fp;
	int			lineLen;
	char			lineStr[1024+1];
	int			lineNum = 0;
	int			ret;
	
	if ((pdef == NULL) || (fileName == NULL))
	{
		UnionUserErrLog("in UnionReadFunDefFromSpecFile:: null parameter!\n");
		return(errCodeParameter);
	}
	
	// 打开定义文件
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadFunDefFromSpecFile:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	// 从文件中读取定义
	memset(pdef,0,sizeof(*pdef));
	while (!feof(fp))
	{
		memset(lineStr,0,sizeof(lineStr));
		if (((lineLen = UnionReadOneLineFromTxtFile(fp,lineStr,sizeof(lineStr))) < 0) && (lineLen != errCodeEnviMDL_NullLine))
		{
			UnionUserErrLog("in UnionReadFunDefFromSpecFile:: UnionReadOneLineFromTxtFile [%s] !lineNum = [%04d] ret = [%d]\n",fileName,lineNum,lineLen);
			ret = lineLen;
			goto errExit;
		}
		lineNum++;
		if (lineLen == 0)	// 空行
			continue;
		if (UnionIsUnixShellRemarkLine(lineStr))	// 注释行
			continue;
		// 函数名称
		if (strncmp(lineStr,conFunDefTagFunName,strlen(conFunDefTagFunName)) == 0)
		{
			if ((ret = UnionReadRecFldFromRecStr(lineStr,lineLen,conFunDefTagFunName,pdef->funName,sizeof(pdef->funName))) < 0)
			{
				UnionUserErrLog("in UnionReadFunDefFromSpecFile:: UnionReadIntTypeRecFldFromRecStr [%s]! lineNum = [%04d]\n",conFunDefTagFunName,lineNum);
				goto errExit;
			}
			UnionReadRecFldFromRecStr(lineStr,lineLen,conFunDefTagFunRemark,pdef->remark,sizeof(pdef->remark));
			continue;
		}
		// 返回值类型
		if (strncmp(lineStr,conFunDefTagReturnType,strlen(conFunDefTagReturnType)) == 0)
		{
			if ((ret = UnionReadVarDeclareTypeDefFromStr(lineStr+strlen(conFunDefTagReturnType),lineLen-strlen(conFunDefTagReturnType),&(pdef->returnType))) < 0)
			{
				UnionUserErrLog("in UnionReadFunDefFromSpecFile:: UnionReadVarDeclareTypeDefFromStr [%s]! lineNum = [%04d]\n",conFunDefTagReturnType,lineNum);
				goto errExit;
			}
			continue;
		}
		// 参数定义
		if (strncmp(lineStr,conFunDefTagVarDef,strlen(conFunDefTagVarDef)) != 0)
			continue;
		if (pdef->varNum >= conMaxNumOfVarPerFun)
		{
			UnionUserErrLog("in UnionReadFunDefFromSpecFile:: too many var [%d] defined for fun [%s]! lineNum = [%04d]\n",pdef->varNum,pdef->funName,lineNum);
			ret = errCodeCDPMDL_TooManyFunVarDefined;
			goto errExit;
		}
		// 变量定义
		if ((ret = UnionReadVarDefFromStr(lineStr+strlen(conFunDefTagVarDef),lineLen-strlen(conFunDefTagVarDef),&(pdef->varGrp[pdef->varNum].varDef))) < 0)
		{
			UnionUserErrLog("in UnionReadFunDefFromSpecFile:: UnionReadVarDefFromStr [%s]! lineNum = [%04d]\n",conFunDefTagReturnType,lineNum);
			goto errExit;
		}
		// 输出参数
		if ((ret = UnionReadIntTypeRecFldFromRecStr(lineStr+strlen(conFunDefTagVarDef),lineLen-strlen(conFunDefTagVarDef),conFunDefTagIsOutputVar,&(pdef->varGrp[pdef->varNum].isOutput))) < 0)
			pdef->varGrp[pdef->varNum].isOutput = 0;
		pdef->varNum += 1;
	}
	ret = 0;
errExit:
	fclose(fp);
	return(ret);
}

