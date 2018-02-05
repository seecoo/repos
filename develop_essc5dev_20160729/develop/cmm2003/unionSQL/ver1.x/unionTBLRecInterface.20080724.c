//---------------------------------------------------------------------------


//#pragma hdrstop

#include "unionWorkingDir.h"
#include "unionErrCode.h"
#include "UnionLog.h"
#include "UnionStr.h"

#include "unionRecFile.h"
#include "unionTBLRecInterface.h"

//---------------------------------------------------------------------------
/*
功能	获得主表的配置文件的名称
输入参数
	tblAlais	表别名
输出参数
	fileName	配置文件名称
返回值
	>=0		正确
	<0		出错代码
*/
int UnionGetTBLRecInterfaceFileName(char *tblAlais,char *fileName)
{
	char	dir[512+1];
	int	ret;
	
	if (fileName == NULL)
		return(errCodeParameter);
	memset(dir,0,sizeof(dir));
	if ((ret = UnionGetTBLRecInterfaceDir(dir)) < 0)
	{
		UnionUserErrLog("in UnionGetTBLRecInterfaceFileName:: UnionGetTBLRecInterfaceDir!\n");
		return(ret);
	}
	sprintf(fileName,"%s/%s.conf",dir,tblAlais);
	return(0);
}


/*
功能	获得主表的配置
输入参数
	tblAlais	表别名
输出参数
	prec		配置记录
返回值
	>=0		正确
	<0		出错代码
*/
int UnionReadTBLRecInterface(char *tblAlais,PUnionTBLRecInterface prec)
{
	char	fileName[512+1];
	int	ret;
	int	lenOfRecStr;
	char	recStr[1024+1];
	
	memset(fileName,0,sizeof(fileName));
	if ((ret = UnionGetTBLRecInterfaceFileName(tblAlais,fileName)) < 0)
	{
		UnionUserErrLog("in UnionReadTBLRecInterface:: UnionGetTBLRecInterfaceFileName!\n");
		return(ret);
	}
	memset(recStr,0,sizeof(recStr));
	if ((lenOfRecStr = UnionReadRecStrFromFile(fileName,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionReadTBLRecInterface:: UnionGetTBLRecInterfaceFileName!\n");
		return(lenOfRecStr);
	}
	if (prec == NULL)
		return(errCodeParameter);
	memset(prec,0,sizeof(prec));
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"name",prec->name,sizeof(prec->name))) < 0)
	{
		UnionUserErrLog("in UnionReadTBLRecInterface:: name not defined for [%s]\n",tblAlais);
		return(ret);
	}
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"alais",prec->alais,sizeof(prec->alais))) <= 0)
		strcpy(prec->alais,tblAlais);
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"recIconFileName",prec->recIconFileName,sizeof(prec->recIconFileName))) <= 0)
		strcpy(prec->recIconFileName,"");
	if ((ret = UnionReadIntTypeRecFldFromRecStr(recStr,lenOfRecStr,"id",&(prec->id))) < 0)
	{
		UnionUserErrLog("in UnionReadTBLRecInterface:: id not defined for [%s]\n",tblAlais);
		return(ret);
	}
	memset(&(prec->fldAssignGrp),0,sizeof(prec->fldAssignGrp));
	if ((ret = UnionReadFldValueAssignMethodGrpDef(fileName,&(prec->fldAssignGrp))) < 0)
	{
		UnionUserErrLog("in UnionReadTBLRecInterface:: UnionReadFldValueAssignMethodGrpDef [%s]\n",tblAlais);
		return(ret);
	}
	if ((ret = UnionReadPrimaryKeyFldGrpDef(fileName,&(prec->primaryKeyFldGrp))) < 0)
	{
		UnionUserErrLog("in UnionReadTBLRecInterface:: primaryKeyFldGrp not defined for [%s]\n",tblAlais);
		return(ret);
	}
	return(0);
}

/*
功能	获得域的别名
输入参数
	prec            记录赋值方法
        fldName         域名
输出参数
	fldAlais        域别名
返回值
	>=0		正确
	<0		出错代码
*/
int UnionReadFldAlaisFromTBLRecInterface(PUnionTBLRecInterface prec,char *fldName,char *fldAlais)
{
        int     index;

        if ((prec == NULL) || (fldName == NULL))
                return(errCodeParameter);
        for (index = 0; index < prec->fldAssignGrp.fldNum; index++)
        {
                if (strcmp(fldName,prec->fldAssignGrp.fldName[index]) == 0)
                {
                        strcpy(fldAlais,prec->fldAssignGrp.fldAlais[index]);
                        return(strlen(fldAlais));
                }
        }
        UnionUserErrLog("in UnionReadFldAlaisFromTBLRecInterface:: fldName [%s] not defined in this conf [%s]\n",fldName,prec->alais);
        return(errCodeSQLInvalidFldName);
}

/*
功能	判断域是否是关键字域
输入参数
	prec            记录赋值方法
        fldName         域名
输出参数
	无
返回值
	1		是关键字域
	0		不是关键字域
*/
int UnionFldOfTBLRecInterfaceIsPrimaryKeyFld(PUnionTBLRecInterface prec,char *fldName)
{
        int     index;

        if ((prec == NULL) || (fldName == NULL))
                return(0);
        for (index = 0; index < prec->primaryKeyFldGrp.fldNum; index++)
        {
                if (strcmp(fldName,prec->primaryKeyFldGrp.fldName[index]) == 0)
                        return(1);
        }
        return(0);
}

//#pragma package(smart_init)

 
