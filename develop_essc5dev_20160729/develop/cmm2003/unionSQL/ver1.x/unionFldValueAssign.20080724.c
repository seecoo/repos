//---------------------------------------------------------------------------


//#pragma hdrstop

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionWorkingDir.h"
#include "unionErrCode.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "unionTBLRecInterface.h"
//#include "mngSvrCommProtocol.h"
//#include "esscRes.h"
//#include "mngSvrTeller.h"
//#include "unionSvrConf.h"
#include "unionRecFile.h"

#include "unionFldValueAssign.h"
#include "unionEnumValueDef.20080724.h"

//---------------------------------------------------------------------------
/*
功能	根据域赋值方法将域值转换为显示方式
输入参数
	method          赋值方法
        value           原值
        lenOfValue      原值长度
        sizeOfBuf       接收缓冲大小
输出参数
	displayValue    显示值
返回值
	>=0		显示值长度
	<0		出错代码
*/
int UnionConvertFldValueIntoDisplayFormat(char *method,char *value,int lenOfValue,char *displayValue,int sizeOfBuf)
{
        if ((method == NULL) || (value == NULL) || (displayValue == NULL))
                return(errCodeParameter);
        if (strncmp(conFldValueSetMethodEnumGrp,method,strlen(conFldValueSetMethodEnumGrp)) == 0)   // 枚举值
        {
                return(UnionReadEnumValueOtherAttr(method+strlen(conFldValueSetMethodEnumGrp),0,value,displayValue,sizeOfBuf));
        }
        strcpy(displayValue,value);
        return(lenOfValue);
}

//---------------------------------------------------------------------------
/*
功能	根据域赋值方法将域值由显示方式转换为实际值
输入参数
	method          赋值方法
	displayValue    显示值
        lenOfDisplayValue      原值长度
        sizeOfBuf       接收缓冲大小
输出参数
        value           原值
返回值
	>=0		显示值长度
	<0		出错代码
*/
int UnionConvertFldValueFromDisplayFormat(char *method,char *displayValue,int lenOfDisplayValue,char *value,int sizeOfBuf)
{
        if ((method == NULL) || (value == NULL) || (displayValue == NULL))
                return(errCodeParameter);
        if (strncmp(conFldValueSetMethodEnumGrp,method,strlen(conFldValueSetMethodEnumGrp)) == 0)   // 枚举值
        {
                return(UnionReadEnumValueOtherAttr(method+strlen(conFldValueSetMethodEnumGrp),1,displayValue,value,sizeOfBuf));
        }
        strcpy(value,displayValue);
        return(lenOfDisplayValue);
}

//---------------------------------------------------------------------------
/*
功能	从文件中读取域赋值方法定义
输入参数
	fileName	文件名
输出参数
	prec		域赋值方法定义
返回值
	>=0		域赋值方法的数目
	<0		出错代码
*/
int UnionReadFldValueAssignMethodGrpDef(char *fileName,PUnionFldValueAssignMethodGrp prec)
{
	int	ret,len;
	char	tmpBuf[2048+1];
	FILE	*fp;
        int     index;

	if ((fileName == NULL) || (prec == NULL))
		return(errCodeParameter);

	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadFldValueAssignMethodGrpDef:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	memset(prec,0,sizeof(prec));
	prec->fldNum = 0;
	while (!feof(fp))
	{
		if (prec->fldNum >= conMaxNumOfQueryFld)
			break;
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((len = UnionReadOneLineFromTxtStr(fp,tmpBuf,sizeof(tmpBuf))) <= 0)
			continue;
		if (UnionIsUnixShellRemarkLine(tmpBuf))
			continue;
		if ((ret = UnionReadRecFldFromRecStr(tmpBuf,len,"valueFldName",prec->fldName[prec->fldNum],sizeof(prec->fldName[prec->fldNum]))) <= 0)
			continue;
		if ((ret = UnionReadRecFldFromRecStr(tmpBuf,len,"fldAlais",prec->fldAlais[prec->fldNum],sizeof(prec->fldAlais[prec->fldNum]))) <= 0)
			strcpy(prec->fldAlais[prec->fldNum],prec->fldName[prec->fldNum]);
		if ((ret = UnionReadRecFldFromRecStr(tmpBuf,len,"value",prec->fldValue[prec->fldNum],sizeof(prec->fldValue[prec->fldNum]))) <= 0)
			strcpy(prec->fldValue[prec->fldNum],"");
		prec->fldNum += 1;
	}
	fclose(fp);
        UnionNullLog("fldValueSetMethod defined in [%s]\n",fileName);
        for (index = 0; index < prec->fldNum; index++)
        {
                UnionNullLog("[%s] [%s] [%s]\n",prec->fldName[index],prec->fldAlais[index],prec->fldValue[index]);
        }
        UnionNullLog("fldValueSetMethod num = [%d]\n",prec->fldNum);
	return(prec->fldNum);
}

/*
功能	读指定表的指定域的赋值方法
输入参数
	tblName         表别名
        isFldAlaisUsed  根据域名还是域别名来读  1根据域别名，0根据域名
        fldTag          域名或域别名
输出参数
	method		读出的赋值方法
返回值
	>=0		域赋值方法的长度
	<0		出错代码
*/
int UnionReadFldValueAssignMethodOfSpecFldOfSpecTBL(char *tblName,int isFldAlaisUsed,char *fldTag,char *method,int sizeOfBuf)
{
        int     ret;
        char    fileName[512+1];
        FILE    *fp;
        char    tmpBuf[512+1];
        char    fldName[100+1];
        int     len;

        // 从表中读取赋值方法
        memset(fileName,0,sizeof(fileName));
        if ((ret = UnionGetTBLRecInterfaceFileName(tblName,fileName)) < 0)
        {
                UnionUserErrLog("in UnionReadFldValueAssignMethodOfSpecFldOfSpecTBL:: UnionGetTBLRecInterfaceFileName!\n");
                return(ret);
        }
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadFldValueAssignMethodOfSpecFldOfSpecTBL:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	while (!feof(fp))
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((len = UnionReadOneLineFromTxtStr(fp,tmpBuf,sizeof(tmpBuf))) <= 0)
			continue;
		if (UnionIsUnixShellRemarkLine(tmpBuf))
			continue;
                memset(fldName,0,sizeof(fldName));
                if ((ret = UnionReadRecFldFromRecStr(tmpBuf,len,"valueFldName",fldName,sizeof(fldName))) < 0)
                        continue;
                if (isFldAlaisUsed)
                {
		        if ((ret = UnionReadRecFldFromRecStr(tmpBuf,len,"fldAlais",fldName,sizeof(fldName))) < 0)
			        continue;
                }
                if (strcmp(fldName,fldTag) != 0)
                        continue;
                // 读到了域赋值定义
                fclose(fp);
		return(UnionReadRecFldFromRecStr(tmpBuf,len,"value",method,sizeOfBuf));
        }
        fclose(fp);
        UnionUserErrLog("in UnionReadFldValueAssignMethodOfSpecFldOfSpecTBL:: no fld assign method defined for [%s] [%s]\n",fldTag,tblName);
        return(errCodeSQLFldAssignMethodNotSet);
}

/*
功能	获得某表的某域的取值表
输入参数
	tblName		表名
        fldAlais        域别名
        maxValueNum     最多可读出的值数目
输出参数
	varGrp		读出的值
返回值
	>=0		读出的值数目
	<0		出错代码
*/
int UnionGetAvailableValueGrp(char *tblName,char *fldAlais,char varGrp[][128+1],int maxValueNum)
{
        int     ret;
        char    method[512+1];
        int     methodLen;

        // 读域的赋值方法
        memset(method,0,sizeof(method));
        if ((methodLen = UnionReadFldValueAssignMethodOfSpecFldOfSpecTBL(tblName,1,fldAlais,method,sizeof(method))) < 0)
        {
                UnionUserErrLog("in UnionGetAvailableValueGrp:: UnionReadFldValueAssignMethodOfSpecFldOfSpecTBL of fld [%s] of tbl [%s]\n",fldAlais,tblName);
                return(methodLen);
        }
        if (ret == 0)
                return(0);
        if (strcmp(conFldValueSetMethodTagUserInput,method) == 0)     // 采用用户交互输入方法
                return(0);
        else if (strcmp(conFldValueSetMethodTagAutoSetBySoftware,method) == 0) // 系统自动赋值
                return(0);
        else if (strncmp(conFldValueSetMethodDefaultSet,method,strlen(conFldValueSetMethodDefaultSet)) == 0) // 缺省赋值
        {
                if (strlen(method) - strlen(conFldValueSetMethodDefaultSet) >= sizeof(varGrp[0]))
                {
                        memcpy(varGrp[0],method+strlen(conFldValueSetMethodDefaultSet),sizeof(varGrp[0])-2);
                        varGrp[0][sizeof(varGrp[0])-1] = 0;
                }
                else
                        strcpy(varGrp[0],method+strlen(conFldValueSetMethodDefaultSet));
                return(1);
        }
        else if (strncmp(conFldValueSetMethodRefrenceOtherTBLFld,method,strlen(conFldValueSetMethodRefrenceOtherTBLFld)) == 0)   // 引用其它表域
        {
                return(UnionGetAvailableValueGrpByQueryCondition(method+strlen(conFldValueSetMethodRefrenceOtherTBLFld),
                        methodLen-strlen(conFldValueSetMethodRefrenceOtherTBLFld),
                        varGrp,maxValueNum));
        }
        else if (strncmp(conFldValueSetMethodEnumGrp,method,strlen(conFldValueSetMethodEnumGrp)) == 0)   // 枚举值
        {
                return(UnionReadAllRemarkOfEnumType(method+strlen(conFldValueSetMethodEnumGrp),varGrp,maxValueNum));
        }
        else
        {
                UnionUserErrLog("in UnionGetAvailableValueGrp:: unknown method [%s] for fld [%s] of tbl [%s]\n",method,fldAlais,tblName);
                return(errCodeSQLFldAssignMethodNotSupport);
        }
}

//#pragma package(smart_init)
