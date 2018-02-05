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

// �ж��Ƿ�� unionEnv ��ȡ DB ����
int UnionIsReadComplexDBOnUnionEnv();

int UnionConvertStrIntoDBFldStr(char *oriStr,int lenOfOriStr,char *desStr,int sizeOfDesStr);

// ���ܣ���ȡ���ݿ���Ϣ������Ϣ
int UnionReadConnDBInfoFromConf(char *DBName, char *userName, char *password, char *DBType);

int UnionReadConnDBInfoRecFromConf(PUnionDBConf prec);
//---------------------------------------------------------------------------

#endif

