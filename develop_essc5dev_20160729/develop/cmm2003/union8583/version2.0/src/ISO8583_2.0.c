//	Author:	Wolfgang Wang
//	Date:	2001/8/29

/*	History Of Modification

	2001/12/18, Wolfgang Wang
	Modify the Function UnionRead8583Field35
	
	2002/1/22, ChenJiaMei
	Modify this file for HSM Server Center
*/

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include "UnionLog.h"

#include "Union8583.h"

// The global variable
struct TABLE8583	g_sRecvTable[129];
struct TABLE8583	g_sSendTable[129];

struct ATTR8583		g_sATTR[129];

unsigned char		g_caBIT[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};


/*
Function:	Initialize 8583 field attribute
Parameters:
		pFilename: Input parameter, Name of file defining 8583 field attribute
Return:
		0:  Success
		<0: Failure
*/
int UnionInit8583Attribute(char *pFileName)
{
	int	iFieldNo,i;
	FILE	*fp;

	if (pFileName == NULL)
	{
		UnionUserErrLog("in UnionInit8583Attribute:: pFileName is NULL point!\n");
		return(ERR_NULL_POINT);
	}

	fp=fopen(pFileName,"r+");
	if (fp==NULL)
	{
		UnionSystemErrLog("In UnionInit8583Attribute:: Can't open the file [%s]!\n",pFileName);
		return(ERR_FILE_OPEN);
	}

	UnionLog("8583 attribute::");
	for (i=0;i<129;i++)
		memset(&(g_sATTR[i]),0,sizeof(struct ATTR8583));
	while (!feof(fp))
	{
		if (fscanf(fp,"%d",&iFieldNo) == EOF)
			break;

		if ((iFieldNo >= 129) || (iFieldNo < 0))
			continue;
		
		if (fscanf(fp,"%d%d",&g_sATTR[iFieldNo].iVarLen,&g_sATTR[iFieldNo].iLen) == EOF)
		{
			fclose(fp);
			return(ERR_FSCANF);
		}

		// Debug
		UnionNullLog("[%d]	[%d]	[%d]\n",iFieldNo,g_sATTR[iFieldNo].iVarLen,g_sATTR[iFieldNo].iLen);
	}
	UnionNullLog("\n");

	fclose(fp);
	
	return(0);
}

/*
Function:	Connect 8583 module.This functions must be called before call other functions of this module.
Parameters:
		p8583DefFile: Input parameter, Name of file defining 8583 field attribute
Return:
		0:  Success
		<0: Failure
*/
int UnionConnect8583MDL(char *p8583DefFile)
{
	int	iRet;

	if (p8583DefFile == NULL)
	{
		UnionUserErrLog("in UnionConnect8583MDL:: p8583DefFile is NULL point!");
		return(ERR_NULL_POINT);
	}

	UnionInit8583RecvBuf();
	UnionInit8583SendBuf();
	if ((iRet = UnionInit8583Attribute(p8583DefFile)) != 0)
	{
		UnionUserErrLog("in UnionConnect8583MDL:: UnionInit8583Attribute fail! return=[%d]\n",iRet);
	}
	return(iRet);
}

/*
Function:	Disonnect 8583 module.This functions must be called if you do not use functions in this module again.
Return:		0
*/
int UnionDisconnect8583MDL()
{
	UnionInit8583RecvBuf();
	UnionInit8583SendBuf();
	return(0);
}

/*
Function:	Initialize receive buffer.This function must be called before unpack 8583.
Return:		0
*/
int UnionInit8583RecvBuf()
{
	int i;

    	for (i=0;i<129;i++)
    	{
       		g_sRecvTable[i].iLen = 0;
       		if (g_sRecvTable[i].pItem)
       		{
           		free(g_sRecvTable[i].pItem);
           		g_sRecvTable[i].pItem = NULL;
       		}
       	}
    	
	return(0);
}

/*
Function:	Initialize send buffer.This function must be called before set 8583 field.
Return:		0
*/
int UnionInit8583SendBuf()
{
	int i;

    	for (i=0;i<129;i++)
    	{
       		g_sSendTable[i].iLen = 0;
       		if (g_sSendTable[i].pItem)
       		{
           		free(g_sSendTable[i].pItem);
           		g_sSendTable[i].pItem = NULL;
       		}
    	}
    	return(0);
}

/*
Function:	Set length of 8583 variable length field.
Parameter:	
		iFieldVarLen: Input parameter,number of length bytes
		iFieldLen: Input parameter,max length of the field
		pFieldLenStr: Output parameter,length string of field,include iFieldVarLen bytes.
Return:		0
*/
int FieldLenFormat(int iFieldVarLen,int iFieldLen,unsigned char *pFieldLenStr)
{
	char	*FORMAT[] =
		{
			"%d", "%01d", "%02d", "%03d", "%04d"
		};

	sprintf((char *)pFieldLenStr,FORMAT[iFieldVarLen],iFieldLen);

	return(0);
}

/*
Function:	Set bitmap(field 1) of 8583.
Parameter:	
		pFieldMap: Input and Output parameter,bitmap(field 1) of 8583
		iIndex: Input parameter,index of 8583 field
Return:		0:  Success
		<0: Failure
*/
int UnionSet8583Field_map(unsigned char *pFieldMap,int iIndex)
{
	unsigned char	*p;

	p=pFieldMap;

	// between 1 to 128
	if ((iIndex <= 0) || (iIndex > 128))
    		return(ERR_FIELD_INDEX);

	p += ( (iIndex-1) / 8);
	p[0] = p[0] | g_caBIT[ (iIndex-1) % 8];

	return(0);	
}

/*
Function:	Juage one field exists or not according to bitmap(field 1) of 8583
Parameter:	
		pFieldMap: Input parameter,bitmap(field 1) of 8583
		iIndex: Input parameter,index of 8583 field
Return:
		1: Exist
		0: Not exist
		<0: Failure
*/
int UnionRead8583Field_map(unsigned char *pFieldMap,int iIndex)
{
	unsigned char	*p;

	p=pFieldMap;

	// between 1 to 128
	if ((iIndex <= 0) || (iIndex > 128))
		return(ERR_FIELD_INDEX);

	p += ( (iIndex-1) / 8);

	if (p[0] & g_caBIT[ (iIndex-1) % 8 ])
		return(1);
	else
		return(0);
}

unsigned char *UnionMallocMem(int iSize)
{
	unsigned char	*p;

	if ( !(p = (unsigned char *)malloc(iSize)))
	{
		UnionSystemErrLog("in UnionMallocMem:: Can't malloc the memory for [%d] bytes!\n",iSize);
		return(NULL);
	}
	memset(p,0,iSize);
	return(p);
}

/*
Function:	Pack 8583
Parameters:
		pPackBuf: Output parameter, buffer will store 8583 package
		iPackBufSize: Input parameter,size of pPackBuf
Return:
		>0: Pack success,return length of pPackBuf
		<0: Pack Failure
*/
int UnionPack8583(unsigned char *pPackBuf,int iPackBufSize)
{
	int		i;
	int		iTotalLen;	// The return value, Length of 8583 package
	unsigned char	*p;		// Work pointer, point to 8583 package
	unsigned char	*pBitmap;	// Use to set the bit map
	unsigned char	caBuffer[10];
	struct ATTR8583		*psTmpAttr;
    	struct TABLE8583	*psTmpTable;

	if (pPackBuf == NULL)
	{
		UnionUserErrLog("in UnionPack8583:: pPackBuf is NULL point!\n");
		return(ERR_NULL_POINT);
	}
	if (iPackBufSize < 22)		// CommandType(6)+FieldMap(16)
	{
		UnionUserErrLog("in UnionPack8583:: Pack buffer size [%d] is too small!\n",iPackBufSize);
		return(ERR_BUFFER_SIZE_SMALL);
	}

	iTotalLen=0;
	p=pPackBuf;
	pBitmap=pPackBuf + g_sATTR[0].iLen;
	memset(pBitmap,0,g_sATTR[1].iLen);
	g_sSendTable[1].iLen=g_sATTR[1].iLen;

	// 0: CommandType	
    	memcpy(p,g_sSendTable[0].pItem,g_sSendTable[0].iLen);
    	p += g_sSendTable[0].iLen;
    	iTotalLen = g_sSendTable[0].iLen;

	// 1: FieldMap
    	p += g_sSendTable[1].iLen;
    	iTotalLen += g_sSendTable[1].iLen;

	UnionSet8583Field_map(pBitmap,1);

	// Debug Begin
	UnionLog("Before pack 8583");
	for (i = 0; i <= 128; i++)
	{
		if (i == 1)
			continue;
		if (g_sSendTable[i].iLen > 0)
			UnionNullLog("i = %03d buf = *%s*\n",i,g_sSendTable[i].pItem);
	}
	UnionNullLog("\n");
	// Debug End

	// Pack
	for (i = 2; i <= g_sSendTable[1].iLen * 8; i++)
	{
		psTmpAttr=&g_sATTR[i];
        	psTmpTable=&g_sSendTable[i];		

		if(psTmpTable->iLen == 0)		// This field has no content
			continue;

		if (psTmpTable->iLen > psTmpAttr->iLen)
		{
			UnionUserErrLog("in UnionPack8583:: The real length [%d] larger than defined length [%d]! Field index = [%d]\n",psTmpTable->iLen,psTmpAttr->iLen,i);
			return(ERR_FIELD_LENGTH);
		}

		if (psTmpAttr->iVarLen  !=  0) 
		{
			// Set length of field
			memset(caBuffer,0,sizeof(caBuffer));
			FieldLenFormat(psTmpAttr->iVarLen,psTmpTable->iLen,caBuffer);
			iTotalLen += psTmpAttr->iVarLen;
            		if (iTotalLen > iPackBufSize)
			{
				UnionUserErrLog("in UnionPack8583:: The total length [%d] is larger than pack buffer size [%d] while pack length of field [%d]!\n",iTotalLen,iPackBufSize,i);
				return(ERR_BUFFER_SIZE_SMALL);
			}
			memcpy(p,caBuffer,psTmpAttr->iVarLen);
			p += psTmpAttr->iVarLen;
			// Set value of field
			iTotalLen += psTmpTable->iLen;
			if (iTotalLen > iPackBufSize)
			{
				UnionUserErrLog("in UnionPack8583:: The total length [%d] is larger than pack buffer size [%d] while pack value of field [%d]!\n",iTotalLen,iPackBufSize,i);
				return(ERR_BUFFER_SIZE_SMALL);
			}
            		memcpy(p,psTmpTable->pItem,psTmpTable->iLen);
			p += psTmpTable->iLen;
		}
		else
		{
			iTotalLen += psTmpAttr->iLen;
			if (iTotalLen > iPackBufSize)
			{
				UnionUserErrLog("in UnionPack8583:: The total length [%d] is larger than pack buffer size [%d] while pack value of field [%d]!\n",iTotalLen,iPackBufSize,i);
				return(ERR_BUFFER_SIZE_SMALL);
			}
			memset(p,0,psTmpAttr->iLen);
			memcpy(p,psTmpTable->pItem,psTmpTable->iLen);
			//memset(p,'0',psTmpAttr->iLen);
			//memcpy(p+psTmpAttr->iLen-psTmpTable->iLen, psTmpTable->pItem, psTmpTable->iLen);
			p += psTmpAttr->iLen;
		}

		// mark the bitmap
		UnionSet8583Field_map(pBitmap,i);
	}

	if (g_sSendTable[1].pItem)
		free(g_sSendTable[1].pItem);
	g_sSendTable[1].pItem=UnionMallocMem(g_sSendTable[1].iLen+1);
       	if (g_sSendTable[1].pItem == NULL)
       	{
       		UnionSystemErrLog("in UnionPack8583:: Can't malloc the memory to g_sSendTable[1].pItem for [%d] bytes!\n",g_sSendTable[1].iLen+1);
		return(ERR_MALLOC);
	}
	memcpy(g_sSendTable[1].pItem,pBitmap,g_sSendTable[1].iLen);

	return(iTotalLen);
}

/*
Function:	Unpack 8583
Parameters:
		pUnpackBuf: Input parameter, buffer of 8583 package
		iUnpackBufLen: Input parameter, length of pUnpackBuf
Return:
		0:  Success
		<0: Failure
*/
int UnionUnpack8583(unsigned char *pUnpackBuf,int iUnpackBufLen)
{
	int	i,iRet=0;
	unsigned char	caBuffer[1000];
	unsigned char	*p,*pEnd,*pBitmap;		

	if (pUnpackBuf == NULL)
	{
		UnionUserErrLog("in UnionUnpack8583:: pUnpackBuf is NULL point!\n");
		return(ERR_NULL_POINT);
	}
	if (iUnpackBufLen < 22)		// CommandType(6)+FieldMap(16)
	{
		UnionUserErrLog("in UnionUnpack8583:: Unpack buffer length [%d] is error!\n",iUnpackBufLen);
		return(ERR_INVALID_LENGTH);
	}

	p = pUnpackBuf;					// Unpack buffer start
	pEnd = pUnpackBuf+iUnpackBufLen;		// Unpack buffer end
	pBitmap = pUnpackBuf + g_sATTR[0].iLen;		// BITMAP start

	UnionInit8583RecvBuf();

	// 0: CommandType
	memset(caBuffer,0,sizeof(caBuffer));
	memcpy(caBuffer,p,g_sATTR[0].iLen);
	if ((atol((char *)caBuffer) > 0L) && (atol((char *)caBuffer) < 1000000L))
    	{
        	g_sRecvTable[0].iLen = g_sATTR[0].iLen;
        	g_sRecvTable[0].pItem=UnionMallocMem(g_sRecvTable[0].iLen+1);
        	if (g_sRecvTable[0].pItem == NULL)
        	{
        		UnionSystemErrLog("in UnionUnpack8583:: Can't malloc the memory to g_sRecvTable[0].pItem for [%d] bytes!\n",g_sRecvTable[0].iLen+1);
			return(ERR_MALLOC);
		}
		memcpy(g_sRecvTable[0].pItem,p,g_sRecvTable[0].iLen);
		p += g_sATTR[0].iLen;
    	}
	else
    	{
    		UnionUserErrLog("in UnionUnpack8583:: Command type [%s] must be number!\n",caBuffer);
		return(ERR_COMMAND_TYPE_CODE);
    	}

	// 1: FieldMap
	g_sRecvTable[1].iLen = g_sATTR[1].iLen;
	g_sRecvTable[1].pItem=UnionMallocMem(g_sRecvTable[1].iLen+1);
       	if (g_sRecvTable[1].pItem == NULL)
       	{
       		UnionSystemErrLog("in UnionUnpack8583:: Can't malloc the memory to g_sRecvTable[1].pItem for [%d] bytes!\n",g_sRecvTable[1].iLen+1);
		return(ERR_MALLOC);
	}
	memcpy(g_sRecvTable[1].pItem,p,g_sRecvTable[1].iLen);
    	p += g_sRecvTable[1].iLen;

	// Unpack
	for (i = 2; i <= g_sRecvTable[1].iLen*8; i++)
	{
		if (UnionRead8583Field_map(pBitmap,i) != 1)	// This field has no content
			continue;

		// Get length of the field
		if ( g_sATTR[i].iVarLen != 0)
		{
			memset(caBuffer,0,sizeof(caBuffer));
			memcpy(caBuffer,p,g_sATTR[i].iVarLen);
			g_sRecvTable[i].iLen=atoi((char *)caBuffer);
			if (g_sRecvTable[i].iLen > g_sATTR[i].iLen)
			{
				iRet = ERR_VARIABLE_LEN;
				break;
			}
			p += g_sATTR[i].iVarLen;
		}
		else
			g_sRecvTable[i].iLen = g_sATTR[i].iLen;

         	// Get content of the field
        	if (g_sRecvTable[i].iLen > 0)
        	{
        		if (p == NULL)
			{
				UnionUserErrLog("in UnionUnpack8583:: no data while the flag indicating there is data!\n");
			        iRet = ERR_FIELD_TYPE;
			        break;
  			}

    			g_sRecvTable[i].pItem=UnionMallocMem(g_sRecvTable[i].iLen+1);
       			if (g_sRecvTable[i].pItem == NULL)
       			{
       				UnionSystemErrLog("in UnionUnpack8583:: Can't malloc the memory to g_sRecvTable[%d].pItem for [%dl] bytes!\n",i,g_sRecvTable[i].iLen+1);
				iRet = ERR_MALLOC;
				break;
			}
			memcpy(g_sRecvTable[i].pItem,p,g_sRecvTable[i].iLen);
			p += g_sRecvTable[i].iLen;
        	}

		if (p > pEnd)		// Reach the end
        	{
			UnionUserErrLog("in UnionUnpack8583:: Message length not enough! i = [%d]\n",i);
			iRet = ERR_MESSAGE_LEN_NOT_ENOUGH;
			break;
        	}
	}

	// Debug Begin
	UnionLog("After unpack 8583:");
	for (i = 0; i <= 128; i++)
	{
		if (i == 1)
			continue;
		
		memset(caBuffer,0,sizeof(caBuffer));
		if (UnionRead8583RecvField(i,caBuffer,sizeof(caBuffer)) > 0)
			UnionNullLog("i = %03d buf = *%s*\n",i,caBuffer);
	}
	UnionNullLog("\n");
	// Debug End

	return(iRet);	
}

/*
Function:	Set value of 8583 field for send buffer
Parameters:
		iIndex: Input parameter,index of 8583 field
		pFieldBuf: Input parameter,value of 8583 field
		iFieldBufLen: Input parameter,length of 8583 field value
Return:
		0:  Success
		<0: Failure
*/
int UnionSet8583SendField(int iIndex,unsigned char *pFieldBuf,int iFieldBufLen)
{
	if ((iIndex < 0) || (iIndex > 128))
	{
		UnionUserErrLog("in UnionSet8583SendField:: Field index [%d] is invalid!\n",iIndex);
		return(ERR_FIELD_INDEX);
	}
	if (pFieldBuf == NULL)
	{
		UnionUserErrLog("in UnionSet8583SendField:: pFieldBuf is NULL point!\n");
		return(ERR_NULL_POINT);
	}
	if (iFieldBufLen < 0 || iFieldBufLen > MAX_8583FIELD_LEN)
	{
		UnionUserErrLog("in UnionSet8583SendField:: Field buffer length [%d] of field [%d] is error!\n",iFieldBufLen,iIndex);
		return(ERR_INVALID_LENGTH);
	}

	if (iFieldBufLen > g_sATTR[iIndex].iLen)
	{
		UnionUserErrLog("in UnionSet8583SendField:: The field [%d] length [%d] larger than defined length [%d]!\n",iIndex,iFieldBufLen,g_sATTR[iIndex].iLen);
		return(ERR_FIELD_LENGTH);
	}

	if (iFieldBufLen == 0)
	{
		g_sSendTable[iIndex].iLen=0;
		return(0);
	}

	if (g_sSendTable[iIndex].pItem)
		free(g_sSendTable[iIndex].pItem);

	g_sSendTable[iIndex].iLen = iFieldBufLen;
	g_sSendTable[iIndex].pItem=UnionMallocMem(g_sSendTable[iIndex].iLen+1);
       	if (g_sSendTable[iIndex].pItem == NULL)
       	{
       		UnionSystemErrLog("in UnionSet8583SendField:: Can't malloc the memory to g_sSendTable[%d].pItem for [%d] bytes!\n",iIndex,g_sSendTable[iIndex].iLen+1);
		return(ERR_MALLOC);
	}
	memcpy(g_sSendTable[iIndex].pItem,pFieldBuf,g_sSendTable[iIndex].iLen);

	return(0);
}

/*
Function:	Get value of 8583 field for receive buffer
Parameters:
		iIndex: Input parameter,index of 8583 field
		pFieldBuf: Output parameter,buffer will store value of 8583 field
		iFieldBufSize: Input parameter,size of pFieldBuf
Return:
		>0: Success,return length of 8583 field value
		<0: Failure
*/
int UnionRead8583RecvField(int iIndex,unsigned char *pFieldBuf,int iFieldBufSize)
{
	if ((iIndex < 0) || (iIndex > 128))
	{
		UnionUserErrLog("in UnionRead8583RecvField:: Field index [%d] is invalid!\n",iIndex);
		return(ERR_FIELD_INDEX);
	}
	if (pFieldBuf == NULL)
	{
		UnionUserErrLog("in UnionRead8583RecvField:: pFieldBuf is NULL point!\n");
		return(ERR_NULL_POINT);
	}
	if (iFieldBufSize <= 0)
	{
		UnionUserErrLog("in UnionRead8583RecvField:: Field buffer size [%d] is too small!\n",iFieldBufSize);
		return(ERR_BUFFER_SIZE_SMALL);
	}

	if (g_sRecvTable[iIndex].iLen == 0)		// This field has no content
	{
		pFieldBuf[0] = '\0';
		return(0);
	}

	if (g_sRecvTable[iIndex].iLen > iFieldBufSize)
	{
		UnionUserErrLog("in UnionRead8583RecvField:: Length [%d] of field [%d] is larger than size of field [%d]!\n",g_sRecvTable[iIndex].iLen,iIndex,iFieldBufSize);
		return(ERR_BUFFER_SIZE_SMALL);
	}
	memcpy(pFieldBuf,g_sRecvTable[iIndex].pItem,g_sRecvTable[iIndex].iLen);
	return(g_sRecvTable[iIndex].iLen);
}

/*
Function:	Set value of 8583 field for receive buffer
Parameters:
		iIndex: Input parameter,index of 8583 field
		pFieldBuf: Input parameter,value of 8583 field
		iFieldBufLen: Input parameter,length of 8583 field value
Return:
		0:  Success
		<0: Failure
*/
int UnionSet8583RecvField(int iIndex,unsigned char *pFieldBuf,int iFieldBufLen)
{
	if ((iIndex < 0) || (iIndex > 128))
	{
		UnionUserErrLog("in UnionSet8583RecvField:: Field index [%d] is invalid!\n",iIndex);
		return(ERR_FIELD_INDEX);
	}
	if (pFieldBuf == NULL)
	{
		UnionUserErrLog("in UnionSet8583RecvField:: pFieldBuf is NULL point!\n");
		return(ERR_NULL_POINT);
	}
	if (iFieldBufLen < 0 || iFieldBufLen > MAX_8583FIELD_LEN)
	{
		UnionUserErrLog("in UnionSet8583RecvField:: Field buffer length [%d] of field [%d] is error!\n",iFieldBufLen,iIndex);
		return(ERR_INVALID_LENGTH);
	}

	if (iFieldBufLen > g_sATTR[iIndex].iLen)
	{
		UnionUserErrLog("in UnionSet8583RecvField:: The field [%d] length [%d] larger than defined length [%d]!\n",iIndex,iFieldBufLen,g_sATTR[iIndex].iLen);
		return(ERR_FIELD_LENGTH);
	}

	if (iFieldBufLen == 0)
	{
		g_sRecvTable[iIndex].iLen=0;
		return(0);
	}

	if (g_sRecvTable[iIndex].pItem)
	{
		if (iFieldBufLen <= g_sRecvTable[iIndex].iLen)
		{
			memset(g_sRecvTable[iIndex].pItem,0,g_sRecvTable[iIndex].iLen);
			memcpy(g_sRecvTable[iIndex].pItem,pFieldBuf,iFieldBufLen);
			g_sRecvTable[iIndex].iLen = iFieldBufLen;
			return(0);
		}
		else
			free(g_sRecvTable[iIndex].pItem);
	}

	g_sRecvTable[iIndex].iLen = iFieldBufLen;
	g_sRecvTable[iIndex].pItem=UnionMallocMem(g_sRecvTable[iIndex].iLen+1);
       	if (g_sRecvTable[iIndex].pItem == NULL)
       	{
       		UnionSystemErrLog("in UnionSet8583RecvField:: Can't malloc the memory to g_sRecvTable[%d].pItem for [%d] bytes!\n",iIndex,g_sRecvTable[iIndex].iLen+1);
		return(ERR_MALLOC);
	}
	memcpy(g_sRecvTable[iIndex].pItem,pFieldBuf,g_sRecvTable[iIndex].iLen);

	return(0);
}

/*
Function:	Get value of 8583 field send buffer
Parameters:
		iIndex: Input parameter,index of 8583 field
		pFieldBuf: Output parameter,buffer will store value of 8583 field
		iFieldBufSize: Input parameter,size of pFieldBuf
Return:
		>0: Success,return length of 8583 field value
		<0: Failure
*/
int UnionRead8583SendField(int iIndex,unsigned char *pFieldBuf,int iFieldBufSize)
{
	if ((iIndex < 0) || (iIndex > 128))
	{
		UnionUserErrLog("in UnionRead8583SendField:: Field index [%d] is invalid!\n",iIndex);
		return(ERR_FIELD_INDEX);
	}
	if (pFieldBuf == NULL)
	{
		UnionUserErrLog("in UnionRead8583SendField:: pFieldBuf is NULL point!\n");
		return(ERR_NULL_POINT);
	}
	if (iFieldBufSize <= 0)
	{
		UnionUserErrLog("in UnionRead8583SendField:: Field buffer size [%d] is too small!\n",iFieldBufSize);
		return(ERR_BUFFER_SIZE_SMALL);
	}

	if (g_sSendTable[iIndex].iLen == 0)		// This field has no content
	{
		pFieldBuf[0] = '\0';
		return(0);
	}

	if (g_sSendTable[iIndex].iLen > iFieldBufSize)
	{
		UnionUserErrLog("in UnionRead8583SendField:: Length [%d] of field [%d] is larger than size of field [%d]!\n",g_sSendTable[iIndex].iLen,iIndex,iFieldBufSize);
		return(ERR_BUFFER_SIZE_SMALL);
	}
	memcpy(pFieldBuf,g_sSendTable[iIndex].pItem,g_sSendTable[iIndex].iLen);
	return(g_sSendTable[iIndex].iLen);
}


