//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *UnionGetProductGeneratedTime();
//char *UnionGetDLDataBaseType();

int UnionPrintServiceVersion(char *dlName,FILE *fp)
{
	if (fp == NULL)
	{
		fp = stdout;
	}
	char    *pTime = NULL;

	pTime = UnionGetProductGeneratedTime();

	fprintf(fp,"\n");
	if (dlName != NULL)
		fprintf(fp,"    动态库名:        %s\n",dlName);
	fprintf(fp,"    生成时间:        %.4s-%.2s-%.2s %.2s:%.2s:%.2s\n",pTime,pTime+4,pTime+6,pTime+8,pTime+10,pTime+12);
	//fprintf(fp,"      数据库:        %s\n", UnionGetDLDataBaseType());
	fprintf(fp,"        位数:        %d Bits\n", (int)(sizeof(long) * 8));
	fprintf(fp,"\n");
	fprintf(fp,"    版权所有:        Copyright 江南科友\n");
	fprintf(fp,"    服务热线:        400-008-0466\n");
	fprintf(fp,"    公司网址:        http://www.keyou.cn\n");
	fprintf(fp,"\n");
	return(0);
}

int UnionServiceVersion(char *dlName)
{
	return(UnionPrintServiceVersion(dlName,NULL));
}

