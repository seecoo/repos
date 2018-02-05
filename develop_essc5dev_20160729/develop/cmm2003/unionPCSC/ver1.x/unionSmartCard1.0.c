#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionSmartCard.h"
//#pragma package(smart_init)

SCARDCONTEXT	m_hContext =0; // Card Reader Contex
SCARDHANDLE	m_hCard;		//Handle of CardReader
DWORD		m_ActiveProtocol;	//Used Protocol

char hexlowtoasc(int xxc)
{
	xxc&=0x0f;
	if (xxc<0x0a) xxc+='0';
		else xxc+=0x37;
	return (char)xxc;
}

char hexhightoasc(int xxc)
{
	xxc&=0xf0;
	xxc = xxc>>4;
	if (xxc<0x0a) xxc+='0';
		else xxc+=0x37;
	return (char)xxc;
}

char asctohex(char ch1,char ch2)
{
	char ch;
	if (ch1>='A')
		ch=(char )((ch1-0x37)<<4);
	else
		ch=(char)((ch1-'0')<<4);
	if (ch2>='A')
		ch|=ch2-0x37;
	else
		ch|=ch2-'0';
	return ch;
}

int aschex_to_bcdhex(char aschex[],int len,char bcdhex[])
{
	int i,j;

	if (len % 2 == 0)
		j = len / 2;
	else
		j = len / 2 + 1;

	for (i = 0; i < j; i++)
		bcdhex[i] = asctohex(aschex[2*i],aschex[2*i+1]);

	return(j);
}

int bcdhex_to_aschex(char bcdhex[],int len,char aschex[])
{
	int i;

	for (i=0;i<len;i++)
	{
		aschex[2*i]   = hexhightoasc(bcdhex[i]);
		aschex[2*i+1] = hexlowtoasc(bcdhex[i]);
	}
	
	return(len*2);
}


void UnionReadErrorMsg(long ErrorCode,char *errorMsg)
{
	switch(ErrorCode)
	{
		case SCARD_E_CANCELLED:
			strcpy(errorMsg, "  The action was canceled by an SCardCancel request. ");
			break;
		case SCARD_E_CANT_DISPOSE:
			strcpy(errorMsg, "The system could not dispose of the media in the requested manner.  ");
			break;
		case SCARD_E_CARD_UNSUPPORTED:
			strcpy(errorMsg, "The smart card does not meet minimal requirements for support.  ");
			break;
		case SCARD_E_DUPLICATE_READER:
			strcpy(errorMsg, "The reader driver didn't produce a unique reader name. ");
			break;
		case SCARD_E_INSUFFICIENT_BUFFER:
			strcpy(errorMsg, "The data buffer for returned data is too small for the returned data. ");
			break;
		case SCARD_E_INVALID_ATR:
			strcpy(errorMsg, "An ATR string obtained from the registry is not a valid ATR string.  ");
			break;
		case SCARD_E_INVALID_HANDLE:
			strcpy(errorMsg, "The supplied handle was invalid.  ");
			break;
		case SCARD_E_INVALID_PARAMETER:
			strcpy(errorMsg, "One or more of the supplied parameters could not be properly interpreted.  ");
			break;
		case SCARD_E_INVALID_TARGET:
			strcpy(errorMsg, "Registry startup information is missing or invalid.  ");
			break;
		case SCARD_E_INVALID_VALUE:
			strcpy(errorMsg, "One or more of the supplied parameter values could not be properly interpreted.  ");
			break;
		case SCARD_E_NOT_READY:
			strcpy(errorMsg, "The reader or card is not ready to accept commands.  ");
			break;
		case SCARD_E_NOT_TRANSACTED:
			strcpy(errorMsg, "An attempt was made to end a non-existent transaction.  ");
			break;
		case SCARD_E_NO_MEMORY:
			strcpy(errorMsg, "Not enough memory available to complete this command.  ");
			break;
		case SCARD_E_NO_SERVICE:
			strcpy(errorMsg, "The smart card resource manager is not running.  ");
			break;
		case SCARD_E_NO_SMARTCARD:
			strcpy(errorMsg, "The operation requires a smart card, but no smart card is currently in the device.  ");
			break;
		case SCARD_E_PCI_TOO_SMALL:
			strcpy(errorMsg, "The PCI receive buffer was too small.  ");
			break;
		case SCARD_E_PROTO_MISMATCH:
			strcpy(errorMsg, "The requested protocols are incompatible with the protocol currently in use with the card.  ");
			break;
		case SCARD_E_READER_UNAVAILABLE:
			strcpy(errorMsg, "The specified reader is not currently available for use.  ");
			break;
		case SCARD_E_READER_UNSUPPORTED:
			strcpy(errorMsg, "The reader driver does not meet minimal requirements for support.  ");
			break;
		case SCARD_E_SERVICE_STOPPED:
			strcpy(errorMsg, "The smart card resource manager has shut down.  ");
			break;
		case SCARD_E_SHARING_VIOLATION:
			strcpy(errorMsg, "The smart card cannot be accessed because of other outstanding connections.  ");
			break;
		case SCARD_E_SYSTEM_CANCELLED:
			strcpy(errorMsg, "The action was canceled by the system, presumably to log off or shut down.  ");
			break;
		case SCARD_E_TIMEOUT:
			strcpy(errorMsg, "The user-specified timeout value has expired.  ");
			break;
		case SCARD_E_UNKNOWN_CARD:
			strcpy(errorMsg, "The specified smart card name is not recognized.  ");
			break;
		case SCARD_E_UNKNOWN_READER:
			strcpy(errorMsg, "The specified reader name is not recognized.  ");
			break;
		case SCARD_F_COMM_ERROR:
			strcpy(errorMsg, "An internal communications error has been detected.  ");
			break;
		case SCARD_F_INTERNAL_ERROR:
			strcpy(errorMsg, "An internal consistency check failed.  ");
			break;
		case SCARD_F_UNKNOWN_ERROR:
			strcpy(errorMsg, "An internal error has been detected, but the source is unknown.  ");
			break;
		case SCARD_F_WAITED_TOO_LONG:
			strcpy(errorMsg, "An internal consistency timer has expired.  ");
			break;
		case SCARD_S_SUCCESS:
			strcpy(errorMsg, "No error was encountered.  ");
			break;
		case SCARD_W_REMOVED_CARD:
			strcpy(errorMsg, "The smart card has been removed, so that further communication is not possible.  ");
			break;
		case SCARD_W_RESET_CARD:
			strcpy(errorMsg, "The smart card has been reset, so any shared state information is invalid.  ");
			break;
		case SCARD_W_UNPOWERED_CARD:
			strcpy(errorMsg, "Power has been removed from the smart card, so that further communication is not possible.  ");
			break;
		case SCARD_W_UNRESPONSIVE_CARD:
			strcpy(errorMsg, "The smart card is not responding to a reset.   ");
			break;
		case SCARD_W_UNSUPPORTED_CARD:
			strcpy(errorMsg, "The reader cannot communicate with the card, due to ATR string configuration conflicts.  ");
			break;
		default:
			strcpy(errorMsg, "unknow error");
			break;
	}
	printf("errorMsg:[%s]\n",errorMsg);
}

// 建立连接
bool UnionCardConnect(char *cardReaderName,char *pATR)
{
	bool	bRetval = false;
	DWORD	dwAP=0;
	DWORD	dwLen=0;
	LONG	res;
	char	tmpBuf[128+1];
	char	errMsg[128+1];
 	SCARD_READERSTATE ReaderState;
	LPTSTR	pmszReaders =   NULL;   
	LPTSTR	pReader     =   NULL;   

 	//UnionCardDisconnect();

	// 建立设备上下文
	res = SCardEstablishContext(SCARD_SCOPE_SYSTEM ,NULL,NULL,&m_hContext);
	if(res != SCARD_S_SUCCESS)
	{
		printf("in UnionCardConnect:: 1111111111111\n");
		memset(errMsg,0,sizeof(errMsg));
		UnionReadErrorMsg(res,errMsg);
		m_hContext = 0;
		return false;
	}

	dwLen = SCARD_AUTOALLOCATE;
	res = SCardListReaders(m_hContext,NULL,(LPTSTR)&pmszReaders,&dwLen);
	if(res == SCARD_S_SUCCESS)
	{
		printf("in UnionCardConnect:: dwLen[%d]\n",dwLen);
		pReader = pmszReaders;
		while ( '\0' != pReader ) 
		{
			res = SCardConnect(m_hContext,pReader,SCARD_SHARE_SHARED,SCARD_PROTOCOL_T0|SCARD_PROTOCOL_T1,&m_hCard,&dwAP);
			if (SCARD_S_SUCCESS == res)
			{
				printf("in UnionCardConnect:: SCardConnect success\n");
				bRetval = true;
				break;
			}
			else
			{
				pReader = pReader + strlen(pReader) + 1; 
			}
		}
		//res = SCardFreeMemory(m_hContext,pmszReaders);
		//if(res != SCARD_S_SUCCESS)
			//printf("Failed SCardFreeMemory\n");
	}
	else
	{
		printf("in UnionCardConnect:: 2222222222222\n");
		UnionReadErrorMsg(res,errMsg);
	}

	if (bRetval == true)
	{
		/*
		if ((0xF0 & ReaderState.rgbAtr[1]) == 0x60)
			m_ActiveProtocol = SCARD_PROTOCOL_T0;
		else if((0xF0 & ReaderState.rgbAtr[1]) == 0xE0)
			m_ActiveProtocol = SCARD_PROTOCOL_T1;
		else
			m_ActiveProtocol = SCARD_PROTOCOL_T1 | SCARD_PROTOCOL_T0;
		*/
		res = SCardReconnect(m_hCard,
			SCARD_SHARE_EXCLUSIVE  ,
			SCARD_RESET_CARD | SCARD_UNPOWER_CARD,
			SCARD_PROTOCOL_T1 | SCARD_PROTOCOL_T0,
			&m_ActiveProtocol);
		if (SCARD_S_SUCCESS == res)
		{
			char *cAtr = (char *)malloc(ReaderState.cbAtr * 2 + 1);
			memset(cAtr, 0, ReaderState.cbAtr * 2 + 1);
			bcdhex_to_aschex(ReaderState.rgbAtr,ReaderState.cbAtr,cAtr);
			pATR = cAtr;
			free(cAtr);
		}
		else
		{
		printf("in UnionCardConnect:: 33333333333333333\n");
			UnionReadErrorMsg(res,errMsg);
			bRetval = false;
		}
	}

	if (bRetval == false)
	{
		if (m_hCard != 0)
			SCardDisconnect(m_hCard, SCARD_UNPOWER_CARD);
		m_hCard = 0;
	}
	return bRetval;
}

// 关闭连接
bool UnionCardDisconnect()
{
	if (m_hCard != 0)
	{
		SCardDisconnect(m_hCard, SCARD_UNPOWER_CARD);
		m_hCard = 0;
	}
	return true;
}

//Execute APDU COMMAND , szCMD' Format is Hex String
int UnionRunAPDU(char *pRequestStr, int lenOfReq,char *pResponseStr,int sizeofBuf)
{
	unsigned int	len;
	char	  	pcResult;
	unsigned char	tmpBuf[4096+1];
	unsigned char	CMD[4096+1];
	char		errMsg[256+1];
	DWORD		REPLen;
	DWORD		CMDLen;
	unsigned char	cResponse[4096+1];
	SCARD_IO_REQUEST	ScardIoRequest;

	if(!m_hCard)
		return -1;
	
	CMDLen = lenOfReq / 2;

	memset(CMD,0, sizeof(CMD));
	aschex_to_bcdhex(pRequestStr,lenOfReq,CMD);

	ScardIoRequest.dwProtocol = m_ActiveProtocol;
	ScardIoRequest.cbPciLength = sizeof(SCARD_IO_REQUEST);

	memset(cResponse,0, sizeof(cResponse));
	REPLen = sizeof(cResponse)-1;

	LONG res = SCardTransmit(m_hCard, &ScardIoRequest, CMD, CMDLen, NULL, cResponse,&REPLen);
	if(res != SCARD_S_SUCCESS)
	{
		memset(errMsg,0,sizeof(errMsg));
		UnionReadErrorMsg(res,errMsg);
		return -1;
	}
	if ((REPLen / 2) > sizeofBuf - 1)
	{
		return(-1);
	}
	bcdhex_to_aschex(cResponse,REPLen,pResponseStr);
	return(REPLen / 2);
}

// 获取所有读卡器的名字
int UnionGetCardListReaders(LPTSTR pReaders[],int maxNum)
{
	int	i = 0;
	int	count = 0;
	int	len;
	long	res;
	char	errMsg[256+1];
	char	sReaderName[128+1];;
	char	mszReaders[2048+1];
	//DWORD dwLen   = SCARD_AUTOALLOCATE;
	DWORD	dwLen;
	DWORD	dwAP;
	LPTSTR	pReader;

	UnionCardReleaseContex();

	res = SCardEstablishContext(SCARD_SCOPE_SYSTEM ,NULL,NULL,&m_hContext);
	if(res != SCARD_S_SUCCESS)
	{
		memset(errMsg,0,sizeof(errMsg));
		UnionReadErrorMsg(res,errMsg);
		m_hContext = 0;
		return -1;
	}

	printf("m_hContext[%d]\n",m_hContext);
	memset(mszReaders,0,sizeof(mszReaders));
	dwLen = -1;
	res = SCardListReaders(m_hContext,NULL,mszReaders,&dwLen);
	if(res == SCARD_S_SUCCESS)
	{
		printf("mszReaders[%s] dwLen[%d]\n",mszReaders,dwLen);
		pReader = (LPTSTR)mszReaders;
		
		while (*pReader !='\0' ) 
		{
			pReaders[count++]=pReader; 
			pReader = pReader + strlen(pReader) + 1; 
		}
		/*
		for (i = 0,count = 0,len = 0; i < dwLen - 1; i++)
		{
			if (count >= maxNum)
				return(count);
	
			if (len == 0)
			{
				memset(pReaders[count],0,sizeof(pReaders[count]));
				strcpy(pReaders[count],mszReaders);
			}
			
			if (mszReaders[i] == '\0')
			{
				if (len > 0)
					count++;
				len=0;
				continue;
			}
			len++;
		}
		*/
	}
	return(count);
}

// 关闭一个已经建立的描述表
void UnionCardReleaseContex(void)
{
	if (m_hContext != 0)
		SCardReleaseContext(m_hContext);
}

