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
	char       record[2560+1];
        char       varDefList[2048+1];
        char       sizeList[1024+1];
        char       condition[1024+1];
        char       returnValueName[64+1];
	char       varGrp[conMaxNumOfVarPerFun][128+1];
	char       varGrp1[conMaxNumOfArrayDimision][128+1];
        int        ret;
        int        i,j;

        memset(record, 0, sizeof record);
        memset(varDefList, 0, sizeof varDefList);
        memset(sizeList, 0, sizeof sizeList);
        memset(condition, 0, sizeof condition);
        memset(varGrp, 0, sizeof varGrp);
        memset(varGrp1, 0, sizeof varGrp1);
        memset(returnValueName, 0, sizeof returnValueName);

        strcpy(condition, "funName=");
        strcat(condition, funName);
        strcat(condition, "|");
        /* 从表里取出一条record */
        if ((ret = UnionSelectUniqueObjectRecordByPrimaryKey("funDef", condition, record, sizeof(record)))<0)

        {
                UnionUserErrLog("in UnionReadFunDefFromDefaultDefFile:: UnionReadFunDefFromDefaultDefFile!\n");
                return(ret);
        }
        
        /* 给函数名称赋值 */
        strcpy(pdef->funName, funName);

        /* 给返回值名称赋值 */
        UnionReadRecFldFromRecStr(record, strlen(record), "returnValueName", returnValueName, sizeof (returnValueName));

        /* 给函数说明赋值 */
        UnionReadRecFldFromRecStr(record, strlen(record), "remark", pdef->remark, sizeof (pdef->remark));

        /* 给参数数目赋值 */
        UnionReadRecFldFromRecStr(record, strlen(record), "varDefList", varDefList, sizeof (varDefList));
        ret = UnionSeprateVarStrIntoVarGrp(varDefList, strlen(varDefList), ',', varGrp, conMaxNumOfVarPerFun);
        if (ret < 0)
        {
                UnionUserErrLog("in UnionReadFunDefFromDefaultDefFile:: UnionReadFunDefFromDefaultDefFile!\n");
                return(ret);
        }
	pdef->varNum = ret;

        /* 取参数列表 */
	for (i=0; i<pdef->varNum; i++)
	{
		memset(record, 0 ,sizeof record);
		memset(condition, 0 ,sizeof condition);

		strcpy(condition, "funName=");
		strcat(condition, funName);
		strcat(condition, "|");
		strcat(condition, "name=");
		strcat(condition, varGrp[i]);
		strcat(condition, "|");
		/* 从表里取出一条record */
		if ((ret = UnionSelectUniqueObjectRecordByPrimaryKey("funVarDef", condition, record, sizeof(record)))<0)

		{
			UnionUserErrLog("in UnionReadFunDefFromDefaultDefFile:: UnionReadFunDefFromDefaultDefFile!\n");
			return(ret);
		}

                /* 给参数的类型赋值 */
		UnionReadRecFldFromRecStr(record, strlen(record), "nameOfType", pdef->varGrp[i].varDef.nameOfType, sizeof (pdef->varGrp[i].varDef.nameOfType));

                /* 给参数的属性(是否是指针)赋值 */
		UnionReadIntTypeRecFldFromRecStr( record, strlen(record), "isPointer", &(pdef->varGrp[i].varDef.isPointer) );

                /* 给参数的属性(判断是输入还是输出)赋值 */
		UnionReadIntTypeRecFldFromRecStr( record, strlen(record), "typeTag", &(pdef->varGrp[i].isOutput) );

                /* 给参数的名称赋值 */
		strcpy(pdef->varGrp[i].varDef.name, varGrp[i]);

                /* 判断参数是否为数组 */
		UnionReadRecFldFromRecStr(record, strlen(record), "sizeList", sizeList, sizeof (sizeList));
		ret = UnionSeprateVarStrIntoVarGrp(sizeList, strlen(sizeList), ',', varGrp1, conMaxNumOfArrayDimision);
		if (ret < 0)
		{
			UnionUserErrLog("in UnionReadFunDefFromDefaultDefFile:: UnionReadFunDefFromDefaultDefFile!\n");
			return(ret);
		}
		if (ret == 0)      /* 不是数组 */
		{
			pdef->varGrp[i].varDef.dimisionNum = 0;
		}
		else if (ret > 0)  /* 是数组 */
		{
			pdef->varGrp[i].varDef.dimisionNum = ret;
			/* 给每一维大小赋值 */
			for(j=0; j<pdef->varGrp[i].varDef.dimisionNum; j++)
			{
				strncpy(pdef->varGrp[i].varDef.sizeOfDimision[j], varGrp1[j], 64);
			}
		}

                /* 给参数说明赋值 */
		UnionReadRecFldFromRecStr(record, strlen(record), "remark", pdef->varGrp[i].varDef.remark, sizeof (pdef->varGrp[i].varDef.remark));
	}

        /* 取函数的返回类型 */
	strcpy(condition, "funName=");
	strcat(condition, funName);
	strcat(condition, "|");
	strcat(condition, "name=");
	strcat(condition, returnValueName);
	strcat(condition, "|");
	/* 从表里取出一条record */
	if ((ret = UnionSelectUniqueObjectRecordByPrimaryKey("funVarDef", condition, record, sizeof(record)))<0)

	{
		UnionUserErrLog("in UnionReadFunDefFromDefaultDefFile:: UnionReadFunDefFromDefaultDefFile!\n");
		return(ret);
	}
	UnionReadRecFldFromRecStr(record, strlen(record), "nameOfType", pdef->returnType.nameOfType, sizeof (pdef->returnType.nameOfType));

	UnionReadIntTypeRecFldFromRecStr( record, strlen(record), "isPointer", &(pdef->returnType.isPointer) );
        
        strcpy(pdef->returnType.name, returnValueName);

        return(0);
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

