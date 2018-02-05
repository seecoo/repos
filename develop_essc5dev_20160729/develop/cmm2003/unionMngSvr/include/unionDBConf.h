#ifndef UnionDBConf_H
#define UnionDBConf_H

typedef struct TUnionDBConf
{
        char    DBName[128];
        char    userName[256];
        char    password[256];
        char    DBType[128];
        char    IPAddr[16];
        int     port;
} TUnionDBConf;
typedef TUnionDBConf    *PUnionDBConf;

//---------------------------------------------------------------------------
int UnionIsUseRealDB();

// 判断是否从 unionEnv 读取 DB 数据
int UnionIsReadComplexDBOnUnionEnv();

int UnionConvertStrIntoDBFldStr(char *oriStr,int lenOfOriStr,char *desStr,int sizeOfDesStr);

// 功能：读取数据库信息配置信息
int UnionReadConnDBInfoFromConf(char *DBName, char *userName, char *password, char *DBType);

int UnionReadConnDBInfoRecFromConf(PUnionDBConf prec);
//---------------------------------------------------------------------------

#endif

