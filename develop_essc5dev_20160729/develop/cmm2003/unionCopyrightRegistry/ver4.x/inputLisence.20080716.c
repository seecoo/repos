//	Wolfgang Wang
//	2008/7/16

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <time.h>

#include "unionLisence.h"
#include "unionCommand.h"

int main(int argc,char **argv)
{
	int	ret;
	char	productName[128+1];
	char	serialNumber[128+1];
	char	liscenceCode[128+1];
	char	*ptr;
	
loop:
	system("clear");
	printf("��֤��Ʒ��Ȩ��\n");
	strcpy(productName,ptr = UnionInput("�������Ʒ����::"));
	if (UnionIsQuit(ptr))
		return(0);
	strcpy(serialNumber,ptr = UnionInput("�������Ʒ���к�::"));
	if (UnionIsQuit(ptr))
		return(0);
	strcpy(liscenceCode,ptr = UnionInput("�������Ʒ��Ȩ��::"));
	if (UnionIsQuit(ptr))
		return(0);
	if ((ret = UnionInputLisenceKeyUseSpecInputData(productName,serialNumber,liscenceCode)) >= 0)
		printf("��Ʒ��Ȩ�ɹ�!\n");
	else
		printf("UnionInputLisenceKeyUseSpecInputData ����,ret=[%d]\n",ret);
	if (UnionConfirm("������֤��?"))
		goto loop;
	return(0);
}	

int UnionIsDebug()
{
	return(1);
}

long UnionGetSizeOfLogFile()
{
	return(1000000);
}

int UnionGetNameOfLogFile(char *fileName)
{
	sprintf(fileName,"%s/unionlog.log",getenv("UNIONLOG"));
	return(0);
}
