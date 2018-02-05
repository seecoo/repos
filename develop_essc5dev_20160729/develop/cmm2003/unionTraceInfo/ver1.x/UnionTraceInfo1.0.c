#include <stdio.h>

#include "unionREC.h"
#include "unionErrCode.h"
#include "unionTraceInfo.h"
#include "UnionLog.h"

/************************************
 * discrible: write trace log
 * input:  appName - application name
 *         str - write to the trace log
 * output: null
 * return: 0 - success
 *        <0 - failure
 * author: xusj
 * date:   2008/12/11
*************************************/
int UnionTraceInfo(char *appName, char *str)
{
        int     ret;
        int     maxFileSize;
        char    fullAppName[100+1];
        char    traceLog[100+1];
        char    fullFileName[256+1];
        FILE    *fp;

        memset(traceLog, 0, sizeof traceLog);
        memset(fullAppName, 0, sizeof fullAppName);
        memset(fullFileName, 0, sizeof fullFileName);

        sprintf(fullAppName, "%s%s", "traceFileOf", appName);
        if ( UnionReadStringTypeRECVar(fullAppName) == NULL )
                return 0;

        strcpy(traceLog, UnionReadStringTypeRECVar(fullAppName));
        sprintf(fullFileName, "%s/%s.trc", getenv("UNIONLOG"), traceLog);

        if ( (maxFileSize = UnionReadIntTypeRECVar("traceFileMaxSize")) < 0 )
                maxFileSize = 1024*1024;
        else
                maxFileSize *= 1024*1024;

        if (UnionExistsFile(fullFileName))
        {
                if ( maxFileSize > UnionFileSize(fullFileName) )
                {
                        fp = fopen(fullFileName, "a+");
                        if (fp == NULL)
                        {
                                UnionUserErrLog("in UnionTraceInfo:: open file err!\n");
                                return (errCodeUseOSErrCode);
                        }
                }
                else
                {
                        fp = fopen(fullFileName, "w");
                        if (fp == NULL)
                        {
                                UnionUserErrLog("in UnionTraceInfo:: open file err!\n");
                                return (errCodeUseOSErrCode);
                        }
                }
        }
        else
        {
                fp = fopen(fullFileName, "w");
                if (fp == NULL)
                {
                        UnionUserErrLog("in UnionTraceInfo:: open file err!\n");
                        return (errCodeUseOSErrCode);
                }
        }

        fprintf(fp, "%s\n", str);
        fclose(fp);

        return 0;
}
