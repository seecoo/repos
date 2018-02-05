#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>

#include "UnionLog.h"
#include "unionErrCode.h"

/*
 * 使用tar命令打包一个文件
 * 输入：packName - 包名称，fileName - 文件名称(带路径)
 * 输出：无
 * 返回：>=0,成功；　<0,失败
 */
int  UnionPackOneFileWithTarCmd(char *packName, char *fileName)
{
	int		ret;
	char		cmdStr[1024+1];
	char		baseName[128+1];
	char		dirName[128+1];

	if (packName == NULL || fileName == NULL)
	{
		UnionUserErrLog("in UnionPackOneFileWithTarCmd:: invalid parameter!\n");
		return(errCodeParameter);
	}
	
	if (access(fileName, F_OK) != 0)
	{
		UnionUserErrLog("in UnionPackOneFileWithTarCmd:: file [%s] is not exist!\n", fileName);
		return(errCodeParameter);
	}

	strcpy(baseName, basename(fileName));
	strcpy(dirName, dirname(fileName));
	if (chdir(dirName) != 0)
	{
		UnionUserErrLog("in UnionPackOneFileWithTarCmd:: chdir [%s] err!\n", dirName);
		return(errCodeParameter);
	}

	memset(cmdStr, 0, sizeof cmdStr);
	sprintf(cmdStr, "tar rvf %s %s", packName, baseName);
#ifdef _AIX_
	if (system(cmdStr) != 0)
	{
		UnionUserErrLog("in UnionPackOneFileWithTarCmd:: system [%s] err!\n", cmdStr);
		return(errCodeParameter);
	}
#else
	system(cmdStr);
#endif
	return 0;
}

/*
 * 使用tar命令打包一个目录
 * 输入：packName - 包名称，dir - 目录名称
 * 输出：无
 * 返回：>=0,成功；　<0,失败
 */
int  UnionPackOneDirectoryWithTarCmd(char *packName, char *dir)
{
	int		ret;
	char		cmdStr[1024+1];

	if (packName == NULL || dir == NULL)
	{
		UnionUserErrLog("in UnionPackOneFileWithTarCmd:: invalid parameter!\n");
		return(errCodeParameter);
	}
	
	if (access(dir, F_OK) != 0)
	{
		UnionUserErrLog("in UnionPackOneFileWithTarCmd:: dir [%s] is not exist!\n", dir);
		return(errCodeParameter);
	}

	if (chdir(dir) != 0)
	{
		UnionUserErrLog("in UnionPackOneFileWithTarCmd:: chdir [%s] err!\n", dir);
		return(errCodeParameter);
	}

	memset(cmdStr, 0, sizeof cmdStr);
	sprintf(cmdStr, "tar cvf %s *", packName);
#ifdef _AIX_
	if (system(cmdStr) != 0)
	{
		UnionUserErrLog("in UnionPackOneFileWithTarCmd:: system [%s] err!\n", cmdStr);
		return(errCodeParameter);
	}
#else
	system(cmdStr);
#endif
	return 0;
}

/*
 * 使用tar命令解包
 * 输入：packName - 包名称; dirName - 解包目录; maxNum - fileList数组的最大个数
 * 输出：fileList - 文件清单
 * 返回：>=0,成功；　<0,失败
 */
int  UnionUnPackWithTarCmd(char *packName, char *dirName, int maxNum, char fileList[][128+1])
{
	int		ret;
	int		i;
	char		line[128+1];
	char		cmdStr[1024+1];
	FILE		*fp;

	if (access(packName, F_OK) != 0 || access(dirName, F_OK) != 0)
	{
		UnionUserErrLog("in UnionUnPackWithTarCmd:: pack [%s] or dir [%s] is not exist!\n", packName, dirName);
		return(errCodeParameter);
	}

	memset(cmdStr, 0, sizeof cmdStr);
#ifdef _AIX_
	sprintf(cmdStr, "tar tvf %s|awk '{print $9}'", packName);
#else
	sprintf(cmdStr, "tar tvf %s|awk '{print $6}'", packName);
#endif
	fp = popen(cmdStr, "r");
        if (fp == NULL)
        {
                UnionUserErrLog("打开文件错!cmd=[%s]\n", cmdStr);
                return (0-abs(errno));
        }

	i = 0;
	memset(line, 0, sizeof line);
	while(fgets(line, sizeof line, fp)!=NULL)
        {
		if (i >= maxNum)
		{
			UnionUserErrLog("fileList is too small [%d]\n", maxNum);
			break;
		}
		line[strlen(line)-1] = '\0';

		if ( line[strlen(line)-1] == '/')
			continue;

		strcpy(fileList[i], basename(line));

		i++;
		memset(line, 0, sizeof line);
	}
	pclose(fp);

	if (chdir(dirName) != 0)
	{
		UnionUserErrLog("in UnionPackOneFileWithTarCmd:: chdir [%s] err!\n", dirName);
		return(errCodeParameter);
	}

	memset(cmdStr, 0, sizeof cmdStr);
	sprintf(cmdStr, "tar xvf %s", packName);
#ifdef _AIX_
	if (system(cmdStr) != 0)
	{
		UnionUserErrLog("in UnionPackOneFileWithTarCmd:: system [%s] err!\n", cmdStr);
		return(errCodeParameter);
	}
#else
	system(cmdStr);
#endif

	return i;
}
