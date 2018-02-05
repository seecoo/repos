/*
	create by hzh in 2010.5.27
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "unionZipAPI.h"

#define ZIP_MAX_PARA_NUM 500
char *gpara[ZIP_MAX_PARA_NUM];
char gbuffer[4096];

//将0x01字符转换为空格
int UnionTrans01ToSpace(unsigned char *buff)
{
	int len = 0;
	int i = 0;
	len = strlen(buff);
	for(i=0;i<len;i++)
	{
		if(buff[i] == 0x01)
		{
			buff[i] = 0x20;
		}
	}
}

//将参数中的0x01字符,全部转回为空格
int UnionTransAll01ToSpace()
{
	int i = 0;
	unsigned char *p = NULL;
	for(i=0;i<ZIP_MAX_PARA_NUM&&gpara[i]!=NULL;i++)
	{
		p = (unsigned char*)(gpara[i]);
		UnionTrans01ToSpace(p);
	}
}

int UnionCmdBuf2Argv(char *cmdBuf)
{
	char tbuf[1024];
	char *p = NULL;
	char *p1 = NULL;
	int len = 0;
	int offset = 0;
	int i = 0;
	int num = 1;
	
	memset(gbuffer,0,sizeof(gbuffer));
	for(i=0;i<ZIP_MAX_PARA_NUM;i++)
		gpara[i] = NULL;
		
	strcpy(gbuffer,cmdBuf);
	len = strlen(gbuffer);
	if (len == 0)
		return 0;
	p1 = (char *)gbuffer;
	i = 0;
	gpara[i] = "unionzip";
	i++;
	while(offset < len && i < ZIP_MAX_PARA_NUM)
	{
		if (gbuffer[offset] == '\x20' || gbuffer[offset] == '\x09')
		{
			offset++;
			continue;
		}
		if ((p = strstr(gbuffer+offset,"\x20")) || (p = strstr(gbuffer+offset,"\x09")))
		{
			*p = '\0';
			gpara[i] = (char *)(gbuffer+offset);
			offset = p - p1 + 1;
			i++;
			num++;
		}
		else {
			break;
		}
	}
	
	if (offset < len && i < ZIP_MAX_PARA_NUM)
	{
		gpara[i] = (char *)(gbuffer+offset);
		num++;
	}
	return num;
}

/*
功能：将文件(可以是多个文件)压缩为指定的ZIP压缩文件
输入：
destZipFile: 压缩生成的文件名
srcFileNameStrs: 
	待压缩的文件。如果要压缩多个文件，要将每个文件用空格隔开，如有要压缩目录(下的所有文件),需在目录前加"-r"参数
	例子：UnionZip("aaa.zip","aaa bbb -r ccc"); 是将文件aaa,bbb及目录ccc(含ccc下的所有文件)压缩生成aaa.zip
*/
int UnionZip(char *destZipFile,char *srcFileNameStrs)
{
	int argNum = 0;
	int i = 0;
	int ret = 0;
	char tbuf[4096];
	sprintf(tbuf,"%s -q %s",destZipFile,srcFileNameStrs);

	argNum = UnionCmdBuf2Argv(tbuf);
	UnionTransAll01ToSpace(); // add by  hzh in 2010.8.19
	ret = zipmain(argNum,gpara);
	
	return ret;
}


/*
功能: 将文件解压到指定路径
输入:
	ZipFileName: 待解压的文件名
	destDir： 解压后存放的路径
*/
int UnionUnZip(char *ZipFileName,char *destDir)
{
	char destDirBuf[1024];
	char tbuf[4096];
	int argNum = 0;
	if (destDir == NULL || strlen(destDir) == 0)
		strcpy(destDirBuf,"./");
	else 
		strcpy(destDirBuf,destDir);
		
	sprintf(tbuf,"-q -u %s -d %s",ZipFileName,destDirBuf);
	argNum = UnionCmdBuf2Argv(tbuf);
	return unzipMain(argNum,gpara);
}

