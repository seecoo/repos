// Wolfgang Wang, 2002/8/17

#include <stdio.h>
#include <string.h>

#include "UnionMsgBuf3.x.h"

PUnionMsgBufHDL pMsgBufHDL;

int main()
{
	long		IDOfMsg;
	unsigned char	Buf[512];
	int		Choice;
	int		Ret;
	
	if ((pMsgBufHDL = UnionConnectMsgBufMDL(2001,100,512)) == NULL)
	{
		printf("UnionConnectMsgBufMDL Failure!\n");
		return(-1);
	}
	
	for (;;)
	{
		printf("\n\n");
		printf("1. Input Message.\n");
		printf("2. Read Message.\n");
		printf("3. Print Status.\n");
		printf("4. Read First Message.\n");
		printf("5. Delete Specified Message.\n");
		printf("0. Exit.\n");
		printf("Your choice::");
		scanf("%d",&Choice);
		switch (Choice)
		{
			case 0:
				goto MainExit;
			case 1:
				printf("Input IDOfMsg::");
				scanf("%ld",&IDOfMsg);
				printf("Input Message::");
				memset(Buf,0,sizeof(Buf));
				scanf("%s",(char *)Buf);
				if ((Ret = UnionBufferMsg(pMsgBufHDL,Buf,strlen((char *)Buf),IDOfMsg)) < 0)
					printf("UnionBufferMsg Failure!\n");
				else
					printf("UnionBufferMsg OK!\n");
				break;
			case 2:
				printf("Input IDOfMsg::");
				scanf("%ld",&IDOfMsg);
				memset(Buf,0,sizeof(Buf));
				if ((Ret = UnionReadSpecifiedMsg(pMsgBufHDL,Buf,sizeof(Buf),IDOfMsg)) < 0)
					printf("UnionReadSpecifiedMsg Failure!\n");
				else
					printf("Buf = [%s]\n",Buf);
				break;
			case 3:
				UnionPrintStatusOfMsgBufMDL(pMsgBufHDL);
				break;
			case 4:
				memset(Buf,0,sizeof(Buf));
				if ((Ret = UnionReadFirstMsg(pMsgBufHDL,Buf,sizeof(Buf),&IDOfMsg)) < 0)
					printf("UnionReadFirstMsg Failure!\n");
				else
					printf("IDOfMsg = [%ld]\nBuf = [%s]\n",IDOfMsg,Buf);
				break;
			case 5:
				printf("Input IDOfMsg::");
				scanf("%ld",&IDOfMsg);
				Ret = UnionDeleteSpecifiedMsg(pMsgBufHDL,IDOfMsg);
				printf("[%d] Message Deleted.\n",Ret);
				break;
			default:
				break;
		}
	}
				
MainExit:
	if ((Ret = UnionRemoveMsgBufMDL(2001,100)) < 0)
		printf("UnionRemoveMsgBufMDL Failure!\n");
	else
		printf("UnionRemoveMsgBufMDL OK!\n");
	
	return(Ret);
}
		
int UnionIsDebug()
{
	return(1);
}

int UnionGetSizeOfLogFile()
{
	return(10000000);
}

int UnionGetNameOfLogFile(char *NameOfLogFile)
{
	sprintf(NameOfLogFile,"%s/log/testMsgBuf3.0.log",getenv("HOME"));
	return(0);
}
