#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionSmartCard.h"

int main(int argc,char *argv[])
{
	int	i;
	int	ret;
	int	cardReadsNum = 0;
	char	cardReaderName[10][128+1];
	char	atr[128+1];
	
	if ((cardReadsNum =  UnionGetCardListReaders(cardReaderName,10)) < 0)
	{
		printf("UnionGetCardListReaders error! ret = [%d]\n",cardReadsNum);
	}
	printf("UnionGetCardListReaders cardReadsNum = [%d]\n",cardReadsNum);
	for(i = 0; i < cardReadsNum; i++)
		printf("UnionGetCardListReaders cardReaderName[%d] = [%s]\n",cardReadsNum,cardReaderName[i]);
	/*
	if (UnionCardConnect("SCM Microsystems, Inc. SCR335 SmartCard Reader 0",atr) == false)
	{
		printf("UnionCardConnect error\n");
	}
	*/
	return(0);
}
