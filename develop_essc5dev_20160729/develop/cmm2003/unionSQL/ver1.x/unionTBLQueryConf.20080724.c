//---------------------------------------------------------------------------


//#pragma hdrstop

#include "unionWorkingDir.h"
#include "unionErrCode.h"
#include "UnionLog.h"
#include "UnionStr.h"

#include "unionRecFile.h"
#include "unionTBLQueryConf.h"

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
int UnionGetTBLQueryConfFileName(char *tblAlais,char *fileName)
{
	char	dir[512+1];
	int	ret;
	
	if (fileName == NULL)
		return(errCodeParameter);
	memset(dir,0,sizeof(dir));
	if ((ret = UnionGetTBLQueryConfDir(dir)) < 0)
	{
		UnionUserErrLog("in UnionGetTBLQueryConfFileName:: UnionGetTBLQueryConfDir!\n");
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
int UnionReadTBLQueryConf(char *tblAlais,PUnionTBLQueryConf prec)
{
	char	fileName[512+1];
	int	ret;
	int	lenOfRecStr;
	char	recStr[1024+1];
	
	memset(fileName,0,sizeof(fileName));
	if ((ret = UnionGetTBLQueryConfFileName(tblAlais,fileName)) < 0)
	{
		UnionUserErrLog("in UnionReadTBLQueryConf:: UnionGetTBLQueryConfFileName!\n");
		return(ret);
	}
	memset(recStr,0,sizeof(recStr));
	if ((lenOfRecStr = UnionReadRecStrFromFile(fileName,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionReadTBLQueryConf:: UnionGetTBLQueryConfFileName!\n");
		return(lenOfRecStr);
	}
	if (prec == NULL)
		return(errCodeParameter);
	memset(prec,0,sizeof(prec));
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"name",prec->name,sizeof(prec->name))) < 0)
	{
		UnionUserErrLog("in UnionReadTBLQueryConf:: name not defined for [%s]\n",tblAlais);
		return(ret);
	}
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"alais",prec->alais,sizeof(prec->alais))) <= 0)
		strcpy(prec->alais,tblAlais);
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"formIconFileName",prec->formIconFileName,sizeof(prec->formIconFileName))) <= 0)
		strcpy(prec->formIconFileName,"");
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"formPopMenuDefFileName",prec->formPopMenuDefFileName,sizeof(prec->formPopMenuDefFileName))) <= 0)
		strcpy(prec->formPopMenuDefFileName,"");
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"recIconFileName",prec->recIconFileName,sizeof(prec->recIconFileName))) <= 0)
		strcpy(prec->recIconFileName,"");
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"recPopMenuDefFileName",prec->recPopMenuDefFileName,sizeof(prec->recPopMenuDefFileName))) <= 0)
		strcpy(prec->recPopMenuDefFileName,"");
	if ((ret = UnionReadIntTypeRecFldFromRecStr(recStr,lenOfRecStr,"id",&(prec->id))) < 0)
	{
		UnionUserErrLog("in UnionReadTBLQueryConf:: id not defined for [%s]\n",tblAlais);
		return(ret);
	}
	if ((ret = UnionReadIntTypeRecFldFromRecStr(recStr,lenOfRecStr,"maxNumOfRecPerPage",&(prec->maxNumOfRecPerPage))) <= 0)
		prec->maxNumOfRecPerPage = 1024;
	memset(&(prec->queryFldGrp),0,sizeof(prec->queryFldGrp));
	if ((ret = UnionReadQueryFldGrpDef(fileName,&(prec->queryFldGrp))) < 0)
	{
		UnionUserErrLog("in UnionReadTBLQueryConf:: queryFldGrp not defined for [%s]\n",tblAlais);
		return(ret);
	}
	memset(&(prec->primaryKeyFldGrp),0,sizeof(prec->primaryKeyFldGrp));
	if ((ret = UnionReadPrimaryKeyFldGrpDef(fileName,&(prec->primaryKeyFldGrp))) < 0)
	{
		UnionUserErrLog("in UnionReadTBLQueryConf:: primaryKeyFldGrp not defined for [%s]\n",tblAlais);
		return(ret);
	}
	return(0);
}

/*
功能	将主表的配置写入到文件中
输入参数
	prec		配置记录
	fp		文件句柄
输出参数
	
返回值
	>=0		正确
	<0		出错代码
*/
int UnionPrintTBLQueryConfToFp(PUnionTBLQueryConf prec,FILE *fp)
{
	int	ret;
	int	lenOfRecStr = 0;
	char	recStr[1024+1];
	FILE	*outFp = stdout;
	
	if (fp != NULL)
		outFp = fp;
		
	if (prec == NULL)
	{
		UnionUserErrLog("in UnionPrintTBLQueryConfToFp:: prec is NULL!\n");
		return(errCodeNullPointer);
	}
	memset(recStr,0,sizeof(recStr));
	if ((ret = UnionPutRecFldIntoRecStr("name",prec->name,strlen(prec->name),recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr)) < 0)
	{
		UnionUserErrLog("in UnionPrintTBLQueryConfToFp:: UnionPutRecFldIntoRecStr name of [%s]\n",prec->name);
		return(ret);
	}
	lenOfRecStr += ret;
	if ((ret = UnionPutRecFldIntoRecStr("alais",prec->alais,strlen(prec->alais),recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr)) <= 0)
	{
		UnionUserErrLog("in UnionPrintTBLQueryConfToFp:: UnionPutRecFldIntoRecStr alais of [%s]\n",prec->name);
		return(ret);
	}
	lenOfRecStr += ret;
	if ((ret = UnionPutIntTypeRecFldIntoRecStr("id",prec->id,recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr)) < 0)
	{
		UnionUserErrLog("in UnionPrintTBLQueryConfToFp:: UnionPutRecFldIntoRecStr id of [%s]\n",prec->name);
		return(ret);
	}
	lenOfRecStr += ret;
	if ((ret = UnionPutRecFldIntoRecStr("formIconFileName",prec->formIconFileName,strlen(prec->formIconFileName),recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr)) <= 0)
	{
		UnionUserErrLog("in UnionPrintTBLQueryConfToFp:: UnionPutRecFldIntoRecStr formIconFileName of [%s]\n",prec->name);
		return(ret);
	}
	lenOfRecStr += ret;
	if ((ret = UnionPutRecFldIntoRecStr("formPopMenuDefFileName",prec->formPopMenuDefFileName,strlen(prec->formPopMenuDefFileName),recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr)) <= 0)
	{
		UnionUserErrLog("in UnionPrintTBLQueryConfToFp:: UnionPutRecFldIntoRecStr formPopMenuDefFileName of [%s]\n",prec->name);
		return(ret);
	}
	lenOfRecStr += ret;
	if ((ret = UnionPutRecFldIntoRecStr("recIconFileName",prec->recIconFileName,strlen(prec->recIconFileName),recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr)) <= 0)
	{
		UnionUserErrLog("in UnionPrintTBLQueryConfToFp:: UnionPutRecFldIntoRecStr recIconFileName of [%s]\n",prec->name);
		return(ret);
	}
	lenOfRecStr += ret;
	if ((ret = UnionPutRecFldIntoRecStr("recPopMenuDefFileName",prec->recPopMenuDefFileName,strlen(prec->recPopMenuDefFileName),recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr)) <= 0)
	{
		UnionUserErrLog("in UnionPrintTBLQueryConfToFp:: UnionPutRecFldIntoRecStr recPopMenuDefFileName of [%s]\n",prec->name);
		return(ret);
	}
	lenOfRecStr += ret;
	if ((ret = UnionPutRecFldIntoRecStr("recDoubleClickMenuFileName",prec->recDoubleClickMenuFileName,strlen(prec->recDoubleClickMenuFileName),recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr)) <= 0)
	{
		UnionUserErrLog("in UnionPrintTBLQueryConfToFp:: UnionPutRecFldIntoRecStr recDoubleClickMenuFileName of [%s]\n",prec->name);
		return(ret);
	}
	lenOfRecStr += ret;
	if ((ret = UnionPutIntTypeRecFldIntoRecStr("maxNumOfRecPerPage",prec->maxNumOfRecPerPage,recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr)) <= 0)
	{
		UnionUserErrLog("in UnionPrintTBLQueryConfToFp:: UnionPutRecFldIntoRecStr maxNumOfRecPerPage of [%s]\n",prec->name);
		return(ret);
	}
	lenOfRecStr += ret;
	if ((ret = UnionPutIntTypeRecFldIntoRecStr("useSeparateTBL",prec->useSeparateTBL,recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr)) <= 0)
	{
		UnionUserErrLog("in UnionPrintTBLQueryConfToFp:: UnionPutRecFldIntoRecStr useSeparateTBL of [%s]\n",prec->name);
		return(ret);
	}
	lenOfRecStr += ret;
	fprintf(outFp,"%s\n",recStr);
	
	if ((ret = UnionPrintPrimaryKeyFldGrpDefToFp(&(prec->primaryKeyFldGrp),outFp)) < 0)
	{
		UnionUserErrLog("in UnionPrintTBLQueryConfToFp:: UnionPrintPrimaryKeyFldGrpDefToFp primaryKeyFldGrp of [%s]\n",prec->name);
		return(ret);
	}
	if ((ret = UnionPrintQueryFldGrpDefToFp(&(prec->queryFldGrp),outFp)) < 0)
	{
		UnionUserErrLog("in UnionPrintTBLQueryConfToFp:: UnionPrintQueryFldGrpDefToFp queryFldGrp of [%s]\n",prec->name);
		return(ret);
	}
	return(0);
}

//#pragma package(smart_init)

