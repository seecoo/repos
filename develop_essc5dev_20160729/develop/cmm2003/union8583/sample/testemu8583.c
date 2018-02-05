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
        sprintf(NameOfLogFile,"%s/log/testemu8583.log",getenv("HOME"));
        return(0);
}

main()
{
	unsigned char	caBuf[1024],caField[50];
	int		iReturn,iLen;
	char		caFile[50];
	unsigned char BitMap[]={0x78,0x00};

	memset(caFile,0,sizeof(caFile));
	sprintf(caFile,"%s/etc/UnionEmu8583.Def",getenv("HOME"));

	// 连接8583模块
	if ((iReturn=UnionConnectEmu8583MDL(caFile)) < 0)
	{
		UnionUserErrLog("Connect Emu8583 model fail! return=[%d]\n",iReturn);
		return(-1);
	}

	// 初始化打包的域空间
	UnionInitEmu8583SendBuf();
	
	// 设置域的值
	UnionSetEmu8583SendField(0,(unsigned char *)"1010",4);
	UnionSetEmu8583SendField(1,(unsigned char *)"20010117100700",14);
	UnionSetEmu8583SendField(2,(unsigned char *)"00000000001",11);
	UnionSetEmu8583SendField(3,(unsigned char *)"1234567812345678123456781234567812345678",40);
	UnionSetEmu8583SendField(4,(unsigned char *)"0000",4);
	//UnionSetEmu8583SendField(5,(unsigned char *)"UserDefineData",14);

	// 打包
	memset(caBuf,0,sizeof(caBuf));
	iLen=PackEmu8583(BitMap,caBuf,sizeof(caBuf));
	if (iLen < 0)
	{
		UnionLog("Pack Emu8583 fail! return=[%d]\n",iLen);
		UnionDisconnectEmu8583MDL();
		return(-1);
	}
	UnionLog("Pack Emu8583 successfully! Pack buffer length=[%d]\n",iLen);
	UnionMemLog("Pack buffer:",caBuf,iLen);
	
	// 读取发送缓冲区域的值和长度
	UnionLog("Field of send buffer:");
	memset(caField,0,sizeof(caField));
	iReturn=UnionReadEmu8583SendField(0,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",0,iReturn,caField);
	memset(caField,0,sizeof(caField));
	iReturn=UnionReadEmu8583SendField(1,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",1,iReturn,caField);
	memset(caField,0,sizeof(caField));
	iReturn=UnionReadEmu8583SendField(2,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",2,iReturn,caField);
	memset(caField,0,sizeof(caField));
	iReturn=UnionReadEmu8583SendField(3,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",3,iReturn,caField);
	memset(caField,0,sizeof(caField));
	iReturn=UnionReadEmu8583SendField(4,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",4,iReturn,caField);
	memset(caField,0,sizeof(caField));
	iReturn=UnionReadEmu8583SendField(5,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",5,iReturn,caField);
	UnionNullLog("\n");

	// 初始化解包的域空间
	UnionInitEmu8583SendBuf();
	UnionInitEmu8583RecvBuf();

	// 解包
	//iReturn=UnionUnpackEmu8583(caBuf,iLen);
	iReturn=UnpackEmu8583(BitMap,caBuf,iLen);
	if (iReturn < 0)
	{
		UnionLog("Unpack Emu8583 fail! return=[%d]\n",iReturn);
		UnionDisconnectEmu8583MDL();
		return(-1);
	}
	UnionLog("Unpack Emu8583 successfully! return=[%d]\n",iReturn);

	// 读取接收缓冲区域的值和长度
	UnionLog("Field of receive buffer:");
	memset(caField,0,sizeof(caField));
	iLen=UnionReadEmu8583RecvField(0,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",0,iLen,caField);
	memset(caField,0,sizeof(caField));
	iLen=UnionReadEmu8583RecvField(1,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",1,iLen,caField);
	memset(caField,0,sizeof(caField));
	iLen=UnionReadEmu8583RecvField(2,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",2,iLen,caField);
	memset(caField,0,sizeof(caField));
	iLen=UnionReadEmu8583RecvField(3,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",3,iLen,caField);
	memset(caField,0,sizeof(caField));
	iLen=UnionReadEmu8583RecvField(4,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",4,iLen,caField);
	memset(caField,0,sizeof(caField));
	iLen=UnionReadEmu8583RecvField(5,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",5,iLen,caField);
	UnionNullLog("\n");

	UnionSetEmu8583RecvField(0,(unsigned char *)"101010",6);
	UnionSetEmu8583RecvField(1,(unsigned char *)"20010117100705",14);
	UnionSetEmu8583RecvField(2,(unsigned char *)"00000000002",11);
	UnionSetEmu8583RecvField(3,(unsigned char *)"8765432187654321876543218765432187654321",40);
	UnionSetEmu8583RecvField(4,(unsigned char *)"0001",4);
	UnionSetEmu8583RecvField(5,(unsigned char *)"UserDefineData1",15);

	UnionLog("Field of receive buffer:");
	memset(caField,0,sizeof(caField));
	iLen=UnionReadEmu8583RecvField(0,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",0,iLen,caField);
	memset(caField,0,sizeof(caField));
	iLen=UnionReadEmu8583RecvField(1,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",1,iLen,caField);
	memset(caField,0,sizeof(caField));
	iLen=UnionReadEmu8583RecvField(2,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",2,iLen,caField);
	memset(caField,0,sizeof(caField));
	iLen=UnionReadEmu8583RecvField(3,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",3,iLen,caField);
	memset(caField,0,sizeof(caField));
	iLen=UnionReadEmu8583RecvField(4,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",4,iLen,caField);
	memset(caField,0,sizeof(caField));
	iLen=UnionReadEmu8583RecvField(5,caField,sizeof(caField));
	UnionNullLog("Field [%d]: length = [%03d] buffer = [%s]\n",5,iLen,caField);
	UnionNullLog("\n");

	// 断开与8583模块的连接
	UnionDisconnectEmu8583MDL();
	return(0);
}

