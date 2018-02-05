#include <stdio.h>
#include <string.h>

#include "UnionAlgorithm.h"

int main()
{
	char	Buf1[512],Buf2[512];
	
	/*
	memset(Buf1,0,sizeof(Buf1));
	Union1DesEncrypt64BitsText("1111111111111111","ABCDEF0123456789",Buf1);
	printf("Buf1 = [%s]\n",Buf1);
	
	memset(Buf2,0,sizeof(Buf2));
	Union1DesDecrypt64BitsText("1111111111111111",Buf1,Buf2);
	printf("Buf2 = [%s]\n",Buf2);
	
	memset(Buf1,0,sizeof(Buf1));
	UnionGenerateANSIX99MAC("1111111111111111","ABCDEF0123456789",16,Buf1);
	printf("Buf1 = [%s]\n",Buf1);
	
	memset(Buf1,0,sizeof(Buf1));
	UnionGenerateANSIX99MAC("1111111111111111","ABCDEF0123456789AAAAAAAAAA",26,Buf1);
	printf("Buf1 = [%s]\n",Buf1);
	
	
	memset(Buf1,0,sizeof(Buf1));
	UnionXOR("1111111111111111","0000000000000000",16,Buf1);
	printf("Buf1 = [%s]\n",Buf1);

	memset(Buf1,0,sizeof(Buf1));
	UnionXOR("1111111111111111","1111111111111111",16,Buf1);
	printf("Buf1 = [%s]\n",Buf1);

	memset(Buf1,0,sizeof(Buf1));
	UnionMD5((unsigned char *)"1111111111111111",16,(unsigned char *)Buf1);
	printf("Buf1 = [%s]\n",Buf1);

	memset(Buf1,0,sizeof(Buf1));
	UnionMD5((unsigned char *)"11111111111111110000000000000000ABCDEF0123456789",48,(unsigned char *)Buf1);
	printf("Buf1 = [%s]\n",Buf1);
	*/
	memset(Buf1,0,sizeof(Buf1));
	UnionGeneratePBOCDPK("0F1345069DBA1650873B7594DBD0F906","5882008000111111",Buf1);
	printf("DPK = [%s]\n",Buf1);
	memset(Buf1,0,sizeof(Buf1));
	UnionGeneratePBOCSessionKey("EB10F7BC6FEB47B9B027A1F3C4CE607A","5882008000111111",Buf1);
	printf("SessiongKey = [%s]\n",Buf1);
	
	return(0);
}

int UnionIsDebug()
{
	return(1);
}

int UnionGetNameOfLogFile(char *NameOfLogFile)
{
	sprintf(NameOfLogFile,"%s/log/testA.log",getenv("HOME"));
	return(0);
}

long UnionGetSizeOfLogFile()
{
	return(1000000);
}
