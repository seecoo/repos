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
	sprintf(FileNameOfLogFile,"%s/log/test3.x.log",getenv("HOME"));
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
	sprintf(DefFileName,"%s/etc/testEmu3.x.Def",getenv("HOME"));
	
	if ((p8583 = UnionConnectEmu8583_3_x_MDL(DefFileName)) == NULL)
	{
		printf("UnionConnectEmu8583_3_x_MDL Error!\n");
		return(-1);
	}
	printf("Connect MDL OK!\n");	
	UnionInitEmu8583_3_x(p8583);
	printf("Init Buf OK!\n");
	UnionSetEmu8583_3_x_Field(p8583,1,(unsigned char *)"0200",4);
	UnionSetEmu8583_3_x_Field(p8583,2,(unsigned char *)"Loveyoubaby",11);
	UnionSetEmu8583_3_x_Field(p8583,3,(unsigned char *)"8888888888",10);
	UnionSetEmu8583_3_x_Field(p8583,4,(unsigned char *)"God! Who r u",12);
	UnionSetEmu8583_3_x_Field(p8583,5,(unsigned char *)"I am Wolfgang Wang",18);
	
	memset(Buf,0,sizeof(Buf));
	if ((Len = UnionPackEmu8583_3_x(p8583,Buf,sizeof(Buf))) < 0)
	{
		printf("UnionPackEmu8583_3_x Error!\n");
		UnionDisconnectEmu8583_3_x_MDL(p8583);
		return(-1);
	}
	printf("Len = [%d] [%s]\n",Len,(char *)Buf);
	if (UnionUnpackEmu8583_3_x(p8583,Buf,Len) < 0)
	{
		printf("UnionUnpackEmu8583_3_x Error!\n");
		UnionDisconnectEmu8583_3_x_MDL(p8583);
		return(-1);
	}
	
	memset(Buf,0,sizeof(Buf));
	Len = UnionReadEmu8583_3_x_Field(p8583,1,(unsigned char *)Buf,sizeof(Buf));
	printf("[%d] [%s]\n",Len,Buf);
	
	memset(Buf,0,sizeof(Buf));
	Len = UnionReadEmu8583_3_x_Field(p8583,2,(unsigned char *)Buf,sizeof(Buf));
	printf("[%d] [%s]\n",Len,Buf);
	
	memset(Buf,0,sizeof(Buf));
	Len = UnionReadEmu8583_3_x_Field(p8583,3,(unsigned char *)Buf,sizeof(Buf));
	printf("[%d] [%s]\n",Len,Buf);
	
	memset(Buf,0,sizeof(Buf));
	Len = UnionReadEmu8583_3_x_Field(p8583,4,(unsigned char *)Buf,sizeof(Buf));
	printf("[%d] [%s]\n",Len,Buf);
	
	memset(Buf,0,sizeof(Buf));
	Len = UnionReadEmu8583_3_x_Field(p8583,5,(unsigned char *)Buf,sizeof(Buf));
	printf("[%d] [%s]\n",Len,Buf);
	
	UnionDisconnectEmu8583_3_x_MDL(p8583);
	return(0);
}	
	

