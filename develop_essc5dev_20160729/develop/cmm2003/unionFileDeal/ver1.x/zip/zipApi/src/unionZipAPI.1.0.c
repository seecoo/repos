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

//��0x01�ַ�ת��Ϊ�ո�
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

//�������е�0x01�ַ�,ȫ��ת��Ϊ�ո�
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
���ܣ����ļ�(�����Ƕ���ļ�)ѹ��Ϊָ����ZIPѹ���ļ�
���룺
destZipFile: ѹ�����ɵ��ļ���
srcFileNameStrs: 
	��ѹ�����ļ������Ҫѹ������ļ���Ҫ��ÿ���ļ��ÿո����������Ҫѹ��Ŀ¼(�µ������ļ�),����Ŀ¼ǰ��"-r"����
	���ӣ�UnionZip("aaa.zip","aaa bbb -r ccc"); �ǽ��ļ�aaa,bbb��Ŀ¼ccc(��ccc�µ������ļ�)ѹ������aaa.zip
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
����: ���ļ���ѹ��ָ��·��
����:
	ZipFileName: ����ѹ���ļ���
	destDir�� ��ѹ���ŵ�·��
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

