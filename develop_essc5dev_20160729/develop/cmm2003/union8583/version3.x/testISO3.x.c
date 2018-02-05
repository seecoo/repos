#include <stdio.h>
#include <string.h>

#include "Union8583_3.x.h"

int UnionIsDebug()
{
	return(1);
}

long UnionGetSizeOfLogFile()
{
	return(1000000);
}

int UnionGetNameOfLogFile(char *FileNameOfLogFile)
{
	sprintf(FileNameOfLogFile,"%s/log/testISO3.x.log",getenv("HOME"));
	return(0);
}

int main()
{
	int		Ret;
	PUnion8583_3_x	p8583;
	char		DefFileName[512];
	unsigned char	Buf[1024];
	int		Len;
	
	memset(DefFileName,0,sizeof(DefFileName));
	sprintf(DefFileName,"%s/etc/ISO8583_3.x.Def",getenv("HOME"));
	
	if ((p8583 = UnionConnect8583_3_x_MDL(DefFileName)) == NULL)
	{
		printf("UnionConnect8583_3_x_MDL Error!\n");
		return(-1);
	}
	printf("Connect MDL OK!\n");	
	UnionInit8583_3_x(p8583);
	printf("Init Buf OK!\n");
	UnionSet8583_3_x_Field(p8583,0,(unsigned char *)"0200",4);
	UnionSet8583_3_x_Field(p8583,1,(unsigned char *)"16",2);
	//UnionSet8583_3_x_Field(p8583,2,(unsigned char *)"Loveyoubaby",11);
	UnionSet8583_3_x_Field(p8583,3,(unsigned char *)"300000",6);
	UnionSet8583_3_x_Field(p8583,11,(unsigned char *)"God! Who r u",12);
	UnionSet8583_3_x_Field(p8583,56,(unsigned char *)"88888888",8);
	
	memset(Buf,0,sizeof(Buf));
	if ((Len = UnionPack8583_3_x(p8583,Buf,sizeof(Buf))) < 0)
	{
		printf("UnionPack8583_3_x Error!\n");
		UnionDisconnect8583_3_x_MDL(p8583);
		return(-1);
	}
	printf("Len = [%d] [%s]\n",Len,(char *)Buf);
	if (UnionUnpack8583_3_x(p8583,Buf,Len) < 0)
	{
		printf("UnionUnpack8583_3_x Error!\n");
		UnionDisconnect8583_3_x_MDL(p8583);
		return(-1);
	}
	
	memset(Buf,0,sizeof(Buf));
	Len = UnionRead8583_3_x_Field(p8583,0,(unsigned char *)Buf,sizeof(Buf));
	printf("[%d] [%s]\n",Len,Buf);
	
	memset(Buf,0,sizeof(Buf));
	Len = UnionRead8583_3_x_Field(p8583,1,(unsigned char *)Buf,sizeof(Buf));
	printf("[%d] [%s]\n",Len,Buf);
	
	
	memset(Buf,0,sizeof(Buf));
	Len = UnionRead8583_3_x_Field(p8583,3,(unsigned char *)Buf,sizeof(Buf));
	printf("[%d] [%s]\n",Len,Buf);
	
	memset(Buf,0,sizeof(Buf));
	Len = UnionRead8583_3_x_Field(p8583,11,(unsigned char *)Buf,sizeof(Buf));
	printf("[%d] [%s]\n",Len,Buf);
	
	memset(Buf,0,sizeof(Buf));
	Len = UnionRead8583_3_x_Field(p8583,56,(unsigned char *)Buf,sizeof(Buf));
	printf("[%d] [%s]\n",Len,Buf);
	
	UnionDisconnect8583_3_x_MDL(p8583);
	return(0);
}	
	

