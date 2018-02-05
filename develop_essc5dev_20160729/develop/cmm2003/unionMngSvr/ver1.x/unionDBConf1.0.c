
#include "stdlib.h"
#include "unionErrCode.h"
#include "unionRecFile.h"
#include "UnionStr.h"
#include "UnionLog.h"
// #include <vcl.h>
#include "unionWorkingDir.h"

#include "unionDBConf.h"

//---------------------------------------------------------------------------
// 数据库配置文件名
const char              pgunionDBConfInfoFileName[]  = "unionDBConfInfo.conf";
int                     gunionIsCommWithRealDB = -1;
int			gunionIsReadDBOnUnionEnv = 0;

//---------------------------------------------------------------------------

int UnionConvertStrIntoDBFldStr(char *oriStr,int lenOfOriStr,char *desStr,int sizeOfDesStr)
{
        int     index;
        if (!UnionIsUseRealDB())
        {
                strcpy(desStr,oriStr);
                return(lenOfOriStr);
        }
        else
        {
                sprintf(desStr,"'%s'",oriStr);
                return(lenOfOriStr+2);
        }
}

int UnionIsUseRealDB()
{
        char	DBType[128+1];
        int	ret;

        if (gunionIsCommWithRealDB >= 0)
        	return(gunionIsCommWithRealDB);

        memset(DBType,0,sizeof(DBType));
        if ((ret = UnionReadConnDBInfoFromConf(NULL,NULL,NULL,DBType)) < 0)
        {
        	UnionUserErrLog("in UnionIsUseRealDB:: UnionReadConnDBInfoFromConf!\n");
        	return(0);
        }
        if ((strcmp(DBType,"complexDB") == 0) || (strcmp(DBType,"simpleDB") == 0) || (strlen(DBType) == 0))
        	gunionIsCommWithRealDB = 0;
        else if (strcmp(DBType, "unionEnv") == 0)
                gunionIsCommWithRealDB = 0;
        else
        {
        	gunionIsCommWithRealDB = 1;
                if (strstr(DBType,"MSDAORA") != NULL)       //
                        UnionSetFldNameUseUpperCase();
        }
	UnionProgramerLog("in UnionIsUseRealDB:: gunionIsCommWithRealDB = [%d]\n",gunionIsCommWithRealDB);
        return(gunionIsCommWithRealDB);
}

// 判断是否从 unionEnv 读取 DB 数据
int UnionIsReadComplexDBOnUnionEnv()
{
	TUnionDBConf	rec;
        int	ret;

	memset(&rec,0,sizeof(rec));
	if ((ret = UnionReadConnDBInfoRecFromConf(&rec)) < 0)
	{
		//UnionUserErrLog("in UnionIsReadComplexDBOnUnionEnv:: UnionReadConnDBInfoRecFromConf!\n");
		UnionLog("in UnionIsReadComplexDBOnUnionEnv:: UnionReadConnDBInfoRecFromConf!\n");
		return(0);
	}
	if (strcmp(rec.DBType,"unionEnv") == 0)
		gunionIsReadDBOnUnionEnv = 1;
	else
		gunionIsReadDBOnUnionEnv = 0; 
	UnionProgramerLog("in UnionIsReadComplexDBOnUnionEnv:: gunionIsReadDBOnUnionEnv = [%d]\n",gunionIsReadDBOnUnionEnv);
        return(gunionIsReadDBOnUnionEnv);
}

// 功能：读取数据库信息配置信息
int UnionReadConnDBInfoFromConf(char *DBName, char *userName, char *password, char *DBType)
{
	TUnionDBConf	rec;
        int	ret;

	memset(&rec,0,sizeof(rec));
	if ((ret = UnionReadConnDBInfoRecFromConf(&rec)) < 0)
	{
		UnionUserErrLog("in UnionReadConnDBInfoFromConf:: UnionReadConnDBInfoRecFromConf!\n");
		return(ret);
	}

	if (DBName != NULL)
		strcpy(DBName,rec.DBName);
	if (userName != NULL)
		strcpy(userName,rec.userName);
	if (password != NULL)
		strcpy(password,rec.password);
	if (DBType != NULL)
		strcpy(DBType,rec.DBType);
	return(ret);
}

int UnionReadConnDBInfoRecFromConf(PUnionDBConf prec)
{
        char                    mainWorkDir[512 + 1];
        char                    realFileName[512 + 1];
        char	                recStr[3072+1];
        int	                lenOfRecStr;
        TUnionRecFileHDL        hdl = NULL;
	int			thisPort;
	int			port;
	TUnionDBConf		defaultRec,rec;
	int			existsDefaultRec = 0;
	int			ret;
		
        UnionLog("In UnionReadConnDBInfoFromConf, to set DB info.\n");
#ifdef _WIN32
        memset(mainWorkDir, 0, sizeof(mainWorkDir));
        UnionGetMainWorkingDir(mainWorkDir);
        memset(realFileName, 0, sizeof(realFileName));
        sprintf(realFileName, "%s\\%s", mainWorkDir, pgunionDBConfInfoFileName);
#else
        sprintf(realFileName, "%s/%s",getenv("UNIONETC"), pgunionDBConfInfoFileName);
#endif

        memset(prec,0,sizeof(TUnionDBConf));

	
        // 数据库配置文件不存在
        if (!UnionExistsFile(realFileName))
       	{
       		//UnionUserErrLog("in UnionReadConnDBInfoRecFromConf:: no DBInfo defined!\n");
       		UnionLog("in UnionReadConnDBInfoRecFromConf:: no DBInfo defined!\n");
       		return(errCodeEsscMDL_DBSvrNotDefined);
        }

	if ((hdl = UnionOpenRecFileHDL(realFileName)) == NULL)
	{
		UnionUserErrLog("In UnionReadConnDBInfoFromConf, can't read DB info from: [%s].\n", realFileName);
  		return errCodeUseOSErrCode;
	}
	// 读当前端口
	thisPort = UnionReadResMngClientPort();
	//UnionProgramerLog("in UnionReadConnDBInfoFromConf:: my port = [%d]\n",thisPort);
	while (1)
	{
		memset(recStr, 0, sizeof(recStr));
		if ((lenOfRecStr = UnionReadNextRecFromFile(hdl,recStr,sizeof(recStr))) == 0)	// 已无记录
			break;

		if (lenOfRecStr < 0)	// 出错
		{
			UnionCloseRecFileHDL(hdl);
			return(lenOfRecStr);
		}
		memset(&rec,0,sizeof(rec));
		if ((ret = UnionReadRecFldFromRecStr(recStr, lenOfRecStr, "DBName", rec.DBName, sizeof(rec.DBName))) <= 0)	// 未定义库名称
			continue;
		UnionReadRecFldFromRecStr(recStr, lenOfRecStr, "DBName", rec.DBName, sizeof(rec.DBName));
		UnionReadRecFldFromRecStr(recStr, lenOfRecStr, "userName", rec.userName, sizeof(rec.userName));
		UnionReadRecFldFromRecStr(recStr, lenOfRecStr, "password", rec.password, sizeof(rec.password));
		UnionReadRecFldFromRecStr(recStr, lenOfRecStr, "DBType", rec.DBType, sizeof(rec.DBType));
		UnionReadRecFldFromRecStr(recStr, lenOfRecStr, "IP", rec.IPAddr, sizeof(rec.IPAddr));
		UnionReadIntTypeRecFldFromRecStr(recStr, lenOfRecStr, "port", &rec.port);
                if (thisPort > 0)
                {
             // modify by wangk 2009-7-30
			//if ((ret = UnionReadIntTypeRecFldFromRecStr(recStr, lenOfRecStr, "DBPort",&port)) <= 0)	// 本记录中未定义端口
			if ((ret = UnionReadIntTypeRecFldFromRecStr(recStr, lenOfRecStr, "DBPort",&port)) < 0)	// 本记录中未定义端口
			{
				if (!existsDefaultRec)	// 未定义缺省端口
				{
					// 复制成缺省端口
					existsDefaultRec = 1;
					memcpy(&defaultRec,&rec,sizeof(rec));
				}
                	}
                	else
                	{
                		if (port == thisPort)
                		{
                			memcpy(prec,&rec,sizeof(rec));
                			UnionCloseRecFileHDLOfFileName(hdl, realFileName);
                			return(0);
                		}
                	}
                }	
                else	// 不需要比较端口
                {
                	memcpy(prec,&rec,sizeof(rec));
                	UnionCloseRecFileHDLOfFileName(hdl, realFileName);
                	return(0);
                }
        }
        UnionCloseRecFileHDLOfFileName(hdl, realFileName);
        if (existsDefaultRec)
        {
        	memcpy(prec,&defaultRec,sizeof(*prec));
        	return(0);
        }
        else
       	{
       		//UnionUserErrLog("in UnionReadConnDBInfoRecFromConf:: no DBInfo defined!\n");
       		UnionLog("in UnionReadConnDBInfoRecFromConf:: no DBInfo defined!\n");
       		return(errCodeEsscMDL_DBSvrNotDefined);
       	}
}

