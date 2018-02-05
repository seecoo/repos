#include <stdio.h>

#include "unionErrCode.h"

#include "unionRecFile.h"
#include "UnionLog.h"

FILE  *g_fp;
int    g_allRecordNum=0;
int    g_curRecordNum=0;
char	g_caRecFileName[256];	// Mary add for lock, 20081211

/*
  打开游标
  输入: recFileName - 文件名
        recordNum   - 记录数
  输出: 无
  返回: >=0 - 正确
	      < 0 - 错误
*/
int
UnionOpenCursor(char *recFileName, int recordNum)
{
	//g_fp = fopen(recFileName, "r");	Mary modify, 20081211
	g_fp = (FILE *)UnionOpenRecFileHDL(recFileName);
	if (g_fp == NULL)
	{
		UnionUserErrLog("in UnionOpenCursor:: UnionOpenRecFileHDL fail!\n");
		return(-1);
	}
	
	g_allRecordNum = recordNum;
	g_curRecordNum = 0;
	
	memset(g_caRecFileName,0,sizeof(g_caRecFileName));	// Mary add, 20081211
	strcpy(g_caRecFileName,recFileName);			// Mary add, 20081211
	
	return 0;
}

/*
  从游标里取一条记录
  输入: 无
  输出: record - 记录
  返回: >=0 - 正确
	      < 0 - 错误
*/
int
UnionFetchOneRecord(char *record)
{
	int len=0;
	
	if((g_curRecordNum >= g_allRecordNum) || feof(g_fp))
	{
		return(errCodeDatabaseMDL_NoData);
	}
	
	while((*record = fgetc(g_fp)) != '\n')
	{
		record++;
		len++;
	}
	*record = '\0';
	
	g_curRecordNum++;
	return len;
}

/*
  关闭游标
  输入: 无
  输出: 无
  返回: >=0 - 正确
	      < 0 - 错误
*/
int
UnionCloseCursor()
{
	//fclose(g_fp);		Mary modify, 20081211
	UnionCloseRecFileHDLOfFileName(g_fp,g_caRecFileName);
	g_allRecordNum = 0;
	g_curRecordNum = 0;
	return 0;
}
