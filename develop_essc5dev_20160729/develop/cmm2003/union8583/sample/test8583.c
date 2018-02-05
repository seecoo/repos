#include <stdio.h>
#include <string.h>
#include "../include/Union8583.h"
#include "UnionLog.h"

// These four functions is writen for test

long UnionGetSizeOfLogFile()
{
	return(10000000);
}

int UnionIsDebug()
{
	return(1);
}

int UnionGetNameOfLogFile(char *NameOfLogFile)
{
        sprintf(NameOfLogFile,"%s/log/test8583.log",getenv("HOME"));
        return(0);
}

main()
{
	unsigned char	caBuf[1024],caField[50];
	int		iReturn,iLen;
	char		caFile[50];

	memset(caFile,0,sizeof(caFile));
	sprintf(caFile,"%s/etc/Union8583.Def",getenv("HOME"));

	// 连接8583模块
	if ((iReturn=UnionConnect8583MDL(caFile)) < 0)
	{
		UnionUserErrLog("Connect 8583 model fail! return=[%d]\n",iReturn);
		return(-1);
	}

	// 初始化打包的域空间
	UnionInit8583SendBuf();
	
	// 设置域的值
	UnionSet8583SendField(0,(unsigned char *)"101011",6);
	UnionSet8583SendField(10,(unsigned char *)"20010117100700",14);
	UnionSet8583SendField(11,(unsigned char *)"000000000001",12);
	UnionSet8583SendField(15,(unsigned char *)"1",1);
	UnionSet8583SendField(16,(unsigned char *)"KeyID",5);
	UnionSet8583SendField(20,(unsigned char *)"0001",4);
	UnionSet8583SendField(36,(unsigned char *)"HoldID",6);
	UnionSet8583SendField(56,(unsigned char *)"ForwardID",9);
	UnionSet8583SendField(60,(unsigned char *)"RequestParameter",16);
	UnionSet8583SendField(80,(unsigned char *)"0",1);
	UnionSet8583SendField(87,(unsigned char *)"0005",4);
	UnionSet8583SendField(90,(unsigned char *)"00",2);
	UnionSet8583SendField(100,(unsigned char *)"ResponseParameter",17);
	UnionSet8583SendField(120,(unsigned char *)"0",1);
	UnionSet8583SendField(121,(unsigned char *)"1",1);
	UnionSet8583SendField(122,(unsigned char *)"0123456789ABCDEF",16);
	UnionSet8583SendField(127,(unsigned char *)"87654321",8);
	UnionSet8583SendField(128,(unsigned char *)"12345678",8);

	// 打包
	memset(caBuf,0,sizeof(caBuf));
	iLen=UnionPack8583(caBuf,sizeof(caBuf));
	if (iLen < 0)
	{
		UnionLog("Pack 8583 fail! return=[%d]\n",iLen);
		UnionDisconnect8583MDL();
		return(-1);
	}
	UnionLog("Pack 8583 successfully! Pack buffer length=[%d]\n",iLen);
	UnionMemLog("Pack buffer:",caBuf,iLen);
	
	// 读取发送缓冲区域的值和长度
	memset(caField,0,sizeof(caField));
	iReturn=UnionRead8583SendField(1,caField,sizeof(caField));
	UnionMemLog("Send Buffer Bitmap",caField,iReturn);

	UnionLog("Field of send buffer:");
	memset(caField,0,sizeof(caField));
	iReturn=UnionRead8583SendField(0,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",0,iReturn,caField);
	memset(caField,0,sizeof(caField));
	iReturn=UnionRead8583SendField(10,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",10,iReturn,caField);
	memset(caField,0,sizeof(caField));
	iReturn=UnionRead8583SendField(11,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",11,iReturn,caField);
	memset(caField,0,sizeof(caField));
	iReturn=UnionRead8583SendField(15,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",15,iReturn,caField);
	memset(caField,0,sizeof(caField));
	iReturn=UnionRead8583SendField(16,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",16,iReturn,caField);
	memset(caField,0,sizeof(caField));
	iReturn=UnionRead8583SendField(20,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",20,iReturn,caField);
	memset(caField,0,sizeof(caField));
	iReturn=UnionRead8583SendField(36,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",36,iReturn,caField);
	memset(caField,0,sizeof(caField));
	iReturn=UnionRead8583SendField(56,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",56,iReturn,caField);
	memset(caField,0,sizeof(caField));
	iReturn=UnionRead8583SendField(60,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",60,iReturn,caField);
	memset(caField,0,sizeof(caField));
	iReturn=UnionRead8583SendField(80,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",80,iReturn,caField);
	memset(caField,0,sizeof(caField));
	iReturn=UnionRead8583SendField(87,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",87,iReturn,caField);
	memset(caField,0,sizeof(caField));
	iReturn=UnionRead8583SendField(90,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",90,iReturn,caField);
	memset(caField,0,sizeof(caField));
	iReturn=UnionRead8583SendField(120,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",120,iReturn,caField);
	memset(caField,0,sizeof(caField));
	iReturn=UnionRead8583SendField(121,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",121,iReturn,caField);
	memset(caField,0,sizeof(caField));
	iReturn=UnionRead8583SendField(122,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",122,iReturn,caField);
	memset(caField,0,sizeof(caField));
	iReturn=UnionRead8583SendField(127,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",127,iReturn,caField);
	memset(caField,0,sizeof(caField));
	iReturn=UnionRead8583SendField(128,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",128,iReturn,caField);
	UnionNullLog("\n");

	// 初始化解包的域空间
	UnionInit8583SendBuf();
	UnionInit8583RecvBuf();

	// 解包
	iReturn=UnionUnpack8583(caBuf,iLen);
	if (iReturn < 0)
	{
		UnionLog("Unpack 8583 fail! return=[%d]\n",iReturn);
		UnionDisconnect8583MDL();
		return(-1);
	}
	UnionLog("Unpack 8583 successfully! return=[%d]\n",iReturn);

	// 读取接收缓冲区域的值和长度
	memset(caField,0,sizeof(caField));
	iLen=UnionRead8583RecvField(1,caField,sizeof(caField));
	UnionMemLog("Receive Buffer Bitmap",caField,iLen);

	UnionLog("Field of receive buffer:");
	memset(caField,0,sizeof(caField));
	iLen=UnionRead8583RecvField(0,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",0,iLen,caField);
	memset(caField,0,sizeof(caField));
	iLen=UnionRead8583RecvField(10,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",10,iLen,caField);
	memset(caField,0,sizeof(caField));
	iLen=UnionRead8583RecvField(11,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",11,iLen,caField);
	memset(caField,0,sizeof(caField));
	iLen=UnionRead8583RecvField(15,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",15,iLen,caField);
	memset(caField,0,sizeof(caField));
	iLen=UnionRead8583RecvField(16,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",16,iLen,caField);
	memset(caField,0,sizeof(caField));
	iLen=UnionRead8583RecvField(20,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",20,iLen,caField);
	memset(caField,0,sizeof(caField));
	iLen=UnionRead8583RecvField(36,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",36,iLen,caField);
	memset(caField,0,sizeof(caField));
	iLen=UnionRead8583RecvField(56,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",56,iLen,caField);
	memset(caField,0,sizeof(caField));
	iLen=UnionRead8583RecvField(60,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",60,iLen,caField);
	memset(caField,0,sizeof(caField));
	iLen=UnionRead8583RecvField(80,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",80,iLen,caField);
	memset(caField,0,sizeof(caField));
	iLen=UnionRead8583RecvField(87,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",87,iLen,caField);
	memset(caField,0,sizeof(caField));
	iLen=UnionRead8583RecvField(90,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",90,iLen,caField);
	memset(caField,0,sizeof(caField));
	iLen=UnionRead8583RecvField(120,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",120,iLen,caField);
	memset(caField,0,sizeof(caField));
	iLen=UnionRead8583RecvField(121,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",121,iLen,caField);
	memset(caField,0,sizeof(caField));
	iLen=UnionRead8583RecvField(122,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",122,iLen,caField);
	memset(caField,0,sizeof(caField));
	iLen=UnionRead8583RecvField(127,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",127,iLen,caField);
	memset(caField,0,sizeof(caField));
	iLen=UnionRead8583RecvField(128,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",128,iLen,caField);
	UnionNullLog("\n");

	UnionSet8583RecvField(80,(unsigned char *)"1",1);
	UnionSet8583RecvField(87,(unsigned char *)"0006",4);
	UnionSet8583RecvField(90,(unsigned char *)"01",2);
	UnionSet8583RecvField(100,(unsigned char *)"ResponseParameter1",18);
	UnionSet8583RecvField(120,(unsigned char *)"1",1);
	UnionSet8583RecvField(121,(unsigned char *)"2",1);
	UnionSet8583RecvField(122,(unsigned char *)"FEDCBA9876543210",16);
	UnionSet8583RecvField(127,(unsigned char *)"AAAAAAAA",8);
	UnionSet8583RecvField(128,(unsigned char *)"BBBBBBBB",8);

	UnionLog("Field of receive buffer:");
	memset(caField,0,sizeof(caField));
	iLen=UnionRead8583RecvField(80,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",80,iLen,caField);
	memset(caField,0,sizeof(caField));
	iLen=UnionRead8583RecvField(87,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",87,iLen,caField);
	memset(caField,0,sizeof(caField));
	iLen=UnionRead8583RecvField(90,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",90,iLen,caField);
	memset(caField,0,sizeof(caField));
	iLen=UnionRead8583RecvField(100,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",100,iLen,caField);
	memset(caField,0,sizeof(caField));
	iLen=UnionRead8583RecvField(120,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",120,iLen,caField);
	memset(caField,0,sizeof(caField));
	iLen=UnionRead8583RecvField(121,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",121,iLen,caField);
	memset(caField,0,sizeof(caField));
	iLen=UnionRead8583RecvField(122,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",122,iLen,caField);
	memset(caField,0,sizeof(caField));
	iLen=UnionRead8583RecvField(127,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",127,iLen,caField);
	memset(caField,0,sizeof(caField));
	iLen=UnionRead8583RecvField(128,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",128,iLen,caField);
	UnionNullLog("\n");

	// 断开与8583模块的连接
	UnionDisconnect8583MDL();
	return(0);
}

