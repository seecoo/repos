//	Author:		������
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
		fprintf(fp,"    ��̬����:        %s\n",dlName);
	fprintf(fp,"    ����ʱ��:        %.4s-%.2s-%.2s %.2s:%.2s:%.2s\n",pTime,pTime+4,pTime+6,pTime+8,pTime+10,pTime+12);
	//fprintf(fp,"      ���ݿ�:        %s\n", UnionGetDLDataBaseType());
	fprintf(fp,"        λ��:        %d Bits\n", (int)(sizeof(long) * 8));
	fprintf(fp,"\n");
	fprintf(fp,"    ��Ȩ����:        Copyright ���Ͽ���\n");
	fprintf(fp,"    ��������:        400-008-0466\n");
	fprintf(fp,"    ��˾��ַ:        http://www.keyou.cn\n");
	fprintf(fp,"\n");
	return(0);
}

int UnionServiceVersion(char *dlName)
{
	return(UnionPrintServiceVersion(dlName,NULL));
}

