#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>

#include "UnionLog.h"
#include "unionErrCode.h"

/*
 * ʹ��tar������һ���ļ�
 * ���룺packName - �����ƣ�fileName - �ļ�����(��·��)
 * �������
 * ���أ�>=0,�ɹ�����<0,ʧ��
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
 * ʹ��tar������һ��Ŀ¼
 * ���룺packName - �����ƣ�dir - Ŀ¼����
 * �������
 * ���أ�>=0,�ɹ�����<0,ʧ��
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
 * ʹ��tar������
 * ���룺packName - ������; dirName - ���Ŀ¼; maxNum - fileList�����������
 * �����fileList - �ļ��嵥
 * ���أ�>=0,�ɹ�����<0,ʧ��
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
                UnionUserErrLog("���ļ���!cmd=[%s]\n", cmdStr);
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
