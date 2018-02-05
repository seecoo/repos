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
struct TABLE8583	gEmu8583RecvTable[129];
struct TABLE8583	gEmu8583SendTable[129];

struct ATTR8583		gEmu8583Attr[129];

int			gEmu8583FldNum=0;


/*
Function:	Initialize 8583 field attribute
Parameters:
		pFilename: Input parameter, Name of file defining 8583 field attribute
Return:
		0:  Success
		<0: Failure
*/
int UnionInitEmu8583Attribute(char *pFileName)
{
	int	iFieldNo,i;
	FILE	*fp;

	if (pFileName == NULL)
	{
		UnionUserErrLog("in UnionInitEmu8583Attribute:: pFileName is NULL point!\n");
		return(ERR_NULL_POINT);
	}

	fp=fopen(pFileName,"r+");
	if (fp==NULL)
	{
		UnionSystemErrLog("In UnionInitEmu8583Attribute:: Can't open the file [%s]!\n",pFileName);
		return(ERR_FILE_OPEN);
	}

	UnionLog("8583 attribute::");
	for (i=0;i<129;i++)
		memset(&(gEmu8583Attr[i]),0,sizeof(struct ATTR8583));
	gEmu8583FldNum=0;
	while (!feof(fp))
	{
		if (fscanf(fp,"%d",&iFieldNo) == EOF)
			break;

		if ((iFieldNo >= 129) || (iFieldNo < 0))
			continue;
		
		if (fscanf(fp,"%d%d",&gEmu8583Attr[iFieldNo].iVarLen,&gEmu8583Attr[iFieldNo].iLen) == EOF)
		{
			fclose(fp);
			return(ERR_FSCANF);
		}

		gEmu8583FldNum++;

		// Debug
		UnionNullLog("[%d]	[%d]	[%d]\n",iFieldNo,gEmu8583Attr[iFieldNo].iVarLen,gEmu8583Attr[iFieldNo].iLen);
	}
	UnionNullLog("\n");

	UnionLog("in UnionInitEmu8583Attribute:: Field total number = [%d]\n",gEmu8583FldNum);

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
int UnionConnectEmu8583MDL(char *p8583DefFile)
{
	int	iRet;

	if (p8583DefFile == NULL)
	{
		UnionUserErrLog("in UnionConnectEmu8583MDL:: p8583DefFile is NULL point!");
		return(ERR_NULL_POINT);
	}

	UnionInitEmu8583RecvBuf();
	UnionInitEmu8583SendBuf();
	if ((iRet = UnionInitEmu8583Attribute(p8583DefFile)) != 0)
	{
		UnionUserErrLog("in UnionConnectEmu8583MDL:: UnionInitEmu8583Attribute fail! return=[%d]\n",iRet);
	}
	return(iRet);
}

/*
Function:	Disonnect 8583 module.This functions must be called if you do not use functions in this module again.
Return:		0
*/
int UnionDisconnectEmu8583MDL()
{
	UnionInitEmu8583RecvBuf();
	UnionInitEmu8583SendBuf();
	return(0);
}

/*
Function:	Initialize receive buffer.This function must be called before unpack 8583.
Return:		0
*/
int UnionInitEmu8583RecvBuf()
{
	int i;

    	for (i=0;i<129;i++)
    	{
       		gEmu8583RecvTable[i].iLen = 0;
       		if (gEmu8583RecvTable[i].pItem)
       		{
           		free(gEmu8583RecvTable[i].pItem);
           		gEmu8583RecvTable[i].pItem = NULL;
       		}
       	}
    	
	return(0);
}

/*
Function:	Initialize send buffer.This function must be called before set 8583 field.
Return:		0
*/
int UnionInitEmu8583SendBuf()
{
	int i;

    	for (i=0;i<129;i++)
    	{
       		gEmu8583SendTable[i].iLen = 0;
       		if (gEmu8583SendTable[i].pItem)
       		{
           		free(gEmu8583SendTable[i].pItem);
           		gEmu8583SendTable[i].pItem = NULL;
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
int EmuFieldLenFormat(int iFieldVarLen,int iFieldLen,unsigned char *pFieldLenStr)
{
	char	*FORMAT[] =
		{
			"%d", "%01d", "%02d", "%03d", "%04d","%05d","%06d",
		};

	sprintf((char *)pFieldLenStr,FORMAT[iFieldVarLen],iFieldLen);

	return(0);
}

unsigned char *UnionEmuMallocMem(int iSize)
{
	unsigned char	*p;

	if ( !(p = (unsigned char *)malloc(iSize)))
	{
		UnionSystemErrLog("in UnionEmuMallocMem:: Can't malloc the memory for [%d] bytes!\n",iSize);
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
int UnionPackEmu8583(unsigned char *pPackBuf,int iPackBufSize)
{
	int		i;
	int		iTotalLen;	// The return value, Length of 8583 package
	unsigned char	*p;		// Work pointer, point to 8583 package
	unsigned char	caBuffer[10];
	struct ATTR8583		*psTmpAttr;
    	struct TABLE8583	*psTmpTable;

	if (pPackBuf == NULL)
	{
		UnionUserErrLog("in UnionPackEmu8583:: pPackBuf is NULL point!\n");
		return(ERR_NULL_POINT);
	}
	if (iPackBufSize <= 0)
	{
		UnionUserErrLog("in UnionPackEmu8583:: Pack buffer size [%d] is too small!\n",iPackBufSize);
		return(ERR_BUFFER_SIZE_SMALL);
	}

	iTotalLen=0;
	p=pPackBuf;

	// Debug Begin
	UnionLog("Before pack Emu8583");
	for (i = 0; i < gEmu8583FldNum; i++)
	{
		if (gEmu8583SendTable[i].iLen > 0)
			UnionNullLog("i = %03d buf = *%s*\n",i,gEmu8583SendTable[i].pItem);
	}
	UnionNullLog("\n");
	//UnionNullLog("Now Begin to Pack Eum8583!\n");
	// Debug End

	// Pack
	for (i = 0; i < gEmu8583FldNum; i++)
	{
		psTmpAttr=&gEmu8583Attr[i];
        	psTmpTable=&gEmu8583SendTable[i];		

		//UnionNullLog("Now Pack i = [%d] TotalLen = [%d] LenOfItem = [%d] PackBufSize = [%d]\n",i,iTotalLen,psTmpTable->iLen,iPackBufSize);

		if (psTmpTable->iLen > psTmpAttr->iLen)
		{
			UnionUserErrLog("in UnionPackEmu8583:: The real length [%d] is larger than defined length [%d]! Field index = [%d]\n",psTmpTable->iLen,psTmpAttr->iLen,i);
			return(ERR_FIELD_LENGTH);
		}

		if (psTmpAttr->iVarLen  !=  0) 
		{
			// Set length of field
			memset(caBuffer,0,sizeof(caBuffer));
			//UnionNullLog("iVarLen = [%d] iLen = [%d]\n",psTmpAttr->iVarLen,psTmpTable->iLen);
			EmuFieldLenFormat(psTmpAttr->iVarLen,psTmpTable->iLen,caBuffer);
			iTotalLen += psTmpAttr->iVarLen;
            		if (iTotalLen > iPackBufSize)
			{
				UnionUserErrLog("in UnionPackEmu8583:: The total length [%d] is larger than pack buffer size [%d] while pack length of field [%d]!\n",iTotalLen,iPackBufSize,i);
				return(ERR_BUFFER_SIZE_SMALL);
			}
			//UnionNullLog("caBuffer = [%s]\n",caBuffer);
			memcpy(p,caBuffer,psTmpAttr->iVarLen);
			p += psTmpAttr->iVarLen;

			if (psTmpTable->iLen == 0)
				continue;

			// Set value of field
			iTotalLen += psTmpTable->iLen;
			if (iTotalLen > iPackBufSize)
			{
				UnionUserErrLog("in UnionPackEmu8583:: The total length [%d] is larger than pack buffer size [%d] while pack value of field [%d]!\n",iTotalLen,iPackBufSize,i);
				return(ERR_BUFFER_SIZE_SMALL);
			}
			//UnionNullLog("Begin to copy Item!\n");
            		memcpy(p,psTmpTable->pItem,psTmpTable->iLen);
            		//UnionNullLog("Copy Item OK!\n");
			p += psTmpTable->iLen;
		}
		else
		{
			iTotalLen += psTmpAttr->iLen;
			if (iTotalLen > iPackBufSize)
			{
				UnionUserErrLog("in UnionPackEmu8583:: The total length [%d] is larger than pack buffer size [%d] while pack value of field [%d]!\n",iTotalLen,iPackBufSize,i);
				return(ERR_BUFFER_SIZE_SMALL);
			}

			if (psTmpTable->iLen == 0)
			{
				memset(p,' ',psTmpAttr->iLen);
				p += psTmpAttr->iLen;
				continue;
			}

			memset(p,0,psTmpAttr->iLen);
			//memcpy(p+psTmpAttr->iLen-psTmpTable->iLen, psTmpTable->pItem, psTmpTable->iLen);
			memcpy(p,psTmpTable->pItem, psTmpTable->iLen);
			p += psTmpAttr->iLen;
		}
	}
	UnionNullLog("Pack OK!\n");
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
int UnionUnpackEmu8583(unsigned char *pUnpackBuf,int iUnpackBufLen)
{
	int	i,iRet=0;
	unsigned char	caBuffer[1000];
	unsigned char	*p,*pEnd;		

	if (pUnpackBuf == NULL)
	{
		UnionUserErrLog("in UnionUnpackEmu8583:: pUnpackBuf is NULL point!\n");
		return(ERR_NULL_POINT);
	}
	if (iUnpackBufLen <= 0)
	{
		UnionUserErrLog("in UnionUnpackEmu8583:: Unpack buffer length [%d] is error!\n",iUnpackBufLen);
		return(ERR_INVALID_LENGTH);
	}

	p = pUnpackBuf;					// Unpack buffer start
	pEnd = pUnpackBuf+iUnpackBufLen;		// Unpack buffer end

	UnionInitEmu8583RecvBuf();

	// Unpack
	for (i = 0; i < gEmu8583FldNum; i++)
	{
		// Get the length of field
		if ( gEmu8583Attr[i].iVarLen != 0)
		{
			memset(caBuffer,0,sizeof(caBuffer));
			memcpy(caBuffer,p,gEmu8583Attr[i].iVarLen);
			gEmu8583RecvTable[i].iLen=atoi((char *)caBuffer);
			if (gEmu8583RecvTable[i].iLen > gEmu8583Attr[i].iLen)
			{
				iRet = ERR_VARIABLE_LEN;
				break;
			}
			p += gEmu8583Attr[i].iVarLen;
		}
		else
			gEmu8583RecvTable[i].iLen = gEmu8583Attr[i].iLen;

		// Get content of the field
        	if (gEmu8583RecvTable[i].iLen > 0)
        	{
        		if (p == NULL)
			{
				UnionUserErrLog("in UnionUnpackEmu8583:: no data while the flag indicating there is data!\n");
			        iRet = ERR_FIELD_TYPE;
			        break;
  			}

    			gEmu8583RecvTable[i].pItem=UnionEmuMallocMem(gEmu8583RecvTable[i].iLen+1);
       			if (gEmu8583RecvTable[i].pItem == NULL)
       			{
       				UnionSystemErrLog("in UnionUnpackEmu8583:: Can't malloc the memory to gEmu8583RecvTable[%d].pItem for [%d] bytes!\n",i,gEmu8583RecvTable[i].iLen+1);
				iRet = ERR_MALLOC;
				break;
			}
			memcpy(gEmu8583RecvTable[i].pItem,p,gEmu8583RecvTable[i].iLen);
			p += gEmu8583RecvTable[i].iLen;
        	}

		if (p > pEnd)		// Reach the end
        	{
			UnionUserErrLog("in UnionUnpackEmu8583:: Message length not enough! i = [%d]\n",i);
			iRet = ERR_MESSAGE_LEN_NOT_ENOUGH;
			break;
        	}
	}

	// Debug Begin
	UnionLog("After unpack 8583:");
	for (i = 0; i < gEmu8583FldNum; i++)
	{
		memset(caBuffer,0,sizeof(caBuffer));
		if (UnionReadEmu8583RecvField(i,caBuffer,sizeof(caBuffer)) > 0)
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
int UnionSetEmu8583SendField(int iIndex,unsigned char *pFieldBuf,int iFieldBufLen)
{
	if ((iIndex < 0) || (iIndex > 128))
	{
		UnionUserErrLog("in UnionSetEmu8583SendField:: Field index [%d] is invalid!\n",iIndex);
		return(ERR_FIELD_INDEX);
	}
	if (pFieldBuf == NULL)
	{
		UnionUserErrLog("in UnionSetEmu8583SendField:: pFieldBuf is NULL point!\n");
		return(ERR_NULL_POINT);
	}
	if (iFieldBufLen < 0 || iFieldBufLen > MAX_8583FIELD_LEN)
	{
		UnionUserErrLog("in UnionSetEmu8583SendField:: Field buffer length [%d] of field [%d] is error!\n",iFieldBufLen,iIndex);
		return(ERR_INVALID_LENGTH);
	}

	if (iFieldBufLen > gEmu8583Attr[iIndex].iLen)
	{
		UnionUserErrLog("in UnionSetEmu8583SendField:: The field [%d] length [%d] larger than defined length [%d]!\n",iIndex,iFieldBufLen,gEmu8583Attr[iIndex].iLen);
		return(ERR_FIELD_LENGTH);
	}

	if (iFieldBufLen == 0)
	{
		gEmu8583SendTable[iIndex].iLen=0;
		return(0);
	}

	if (gEmu8583SendTable[iIndex].pItem)
		free(gEmu8583SendTable[iIndex].pItem);

	gEmu8583SendTable[iIndex].iLen = iFieldBufLen;
	gEmu8583SendTable[iIndex].pItem=UnionEmuMallocMem(gEmu8583SendTable[iIndex].iLen+1);
       	if (gEmu8583SendTable[iIndex].pItem == NULL)
       	{
       		UnionSystemErrLog("in UnionSetEmu8583SendField:: Can't malloc the memory to gEmu8583SendTable[%d].pItem for [%d] bytes!\n",iIndex,gEmu8583SendTable[iIndex].iLen+1);
		return(ERR_MALLOC);
	}
	memcpy(gEmu8583SendTable[iIndex].pItem,pFieldBuf,gEmu8583SendTable[iIndex].iLen);

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
int UnionReadEmu8583RecvField(int iIndex,unsigned char *pFieldBuf,int iFieldBufSize)
{
	if ((iIndex < 0) || (iIndex > 128))
	{
		UnionUserErrLog("in UnionReadEmu8583RecvField:: Field index [%d] is invalid!\n",iIndex);
		return(ERR_FIELD_INDEX);
	}
	if (pFieldBuf == NULL)
	{
		UnionUserErrLog("in UnionReadEmu8583RecvField:: pFieldBuf is NULL point!\n");
		return(ERR_NULL_POINT);
	}
	if (iFieldBufSize <= 0)
	{
		UnionUserErrLog("in UnionReadEmu8583RecvField:: Field buffer size [%d] is too small!\n",iFieldBufSize);
		return(ERR_BUFFER_SIZE_SMALL);
	}

	if (gEmu8583RecvTable[iIndex].iLen == 0)		// This field has no content
	{
		pFieldBuf[0] = '\0';
		return(0);
	}

	if (gEmu8583RecvTable[iIndex].iLen > iFieldBufSize)
	{
		UnionUserErrLog("in UnionReadEmu8583RecvField:: Length [%d] of field [%d] is larger than size of field [%d]!\n",gEmu8583RecvTable[iIndex].iLen,iIndex,iFieldBufSize);
		return(ERR_BUFFER_SIZE_SMALL);
	}
	memcpy(pFieldBuf,gEmu8583RecvTable[iIndex].pItem,gEmu8583RecvTable[iIndex].iLen);
	return(gEmu8583RecvTable[iIndex].iLen);
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
int UnionSetEmu8583RecvField(int iIndex,unsigned char *pFieldBuf,int iFieldBufLen)
{
	if ((iIndex < 0) || (iIndex > 128))
	{
		UnionUserErrLog("in UnionSetEmu8583RecvField:: Field index [%d] is invalid!\n",iIndex);
		return(ERR_FIELD_INDEX);
	}
	if (pFieldBuf == NULL)
	{
		UnionUserErrLog("in UnionSetEmu8583RecvField:: pFieldBuf is NULL point!\n");
		return(ERR_NULL_POINT);
	}
	if (iFieldBufLen < 0 || iFieldBufLen > MAX_8583FIELD_LEN)
	{
		UnionUserErrLog("in UnionSetEmu8583RecvField:: Field buffer length [%d] of field [%d] is error!\n",iFieldBufLen,iIndex);
		return(ERR_INVALID_LENGTH);
	}

	if (iFieldBufLen > gEmu8583Attr[iIndex].iLen)
	{
		UnionUserErrLog("in UnionSetEmu8583RecvField:: The field [%d] length [%d] larger than defined length [%d]!\n",iIndex,iFieldBufLen,gEmu8583Attr[iIndex].iLen);
		return(ERR_FIELD_LENGTH);
	}

	if (iFieldBufLen == 0)
	{
		gEmu8583RecvTable[iIndex].iLen=0;
		return(0);
	}

	if (gEmu8583RecvTable[iIndex].pItem)
	{
		if (iFieldBufLen <= gEmu8583RecvTable[iIndex].iLen)
		{
			memset(gEmu8583RecvTable[iIndex].pItem,0,gEmu8583RecvTable[iIndex].iLen);
			memcpy(gEmu8583RecvTable[iIndex].pItem,pFieldBuf,iFieldBufLen);
			gEmu8583RecvTable[iIndex].iLen = iFieldBufLen;
			return(0);
		}
		else
			free(gEmu8583RecvTable[iIndex].pItem);
	}

	gEmu8583RecvTable[iIndex].iLen = iFieldBufLen;
	gEmu8583RecvTable[iIndex].pItem=UnionEmuMallocMem(gEmu8583RecvTable[iIndex].iLen+1);
       	if (gEmu8583RecvTable[iIndex].pItem == NULL)
       	{
       		UnionSystemErrLog("in UnionSetEmu8583RecvField:: Can't malloc the memory to gEmu8583RecvTable[%d].pItem for [%d] bytes!\n",iIndex,gEmu8583RecvTable[iIndex].iLen+1);
		return(ERR_MALLOC);
	}
	memcpy(gEmu8583RecvTable[iIndex].pItem,pFieldBuf,gEmu8583RecvTable[iIndex].iLen);

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
int UnionReadEmu8583SendField(int iIndex,unsigned char *pFieldBuf,int iFieldBufSize)
{
	if ((iIndex < 0) || (iIndex > 128))
	{
		UnionUserErrLog("in UnionReadEmu8583SendField:: Field index [%d] is invalid!\n",iIndex);
		return(ERR_FIELD_INDEX);
	}
	if (pFieldBuf == NULL)
	{
		UnionUserErrLog("in UnionReadEmu8583SendField:: pFieldBuf is NULL point!\n");
		return(ERR_NULL_POINT);
	}
	if (iFieldBufSize <= 0)
	{
		UnionUserErrLog("in UnionReadEmu8583SendField:: Field buffer size [%d] is too small!\n",iFieldBufSize);
		return(ERR_BUFFER_SIZE_SMALL);
	}

	if (gEmu8583SendTable[iIndex].iLen == 0)		// This field has no content
	{
		pFieldBuf[0] = '\0';
		return(0);
	}

	if (gEmu8583SendTable[iIndex].iLen > iFieldBufSize)
	{
		UnionUserErrLog("in UnionReadEmu8583SendField:: Length [%d] of field [%d] is larger than size of field [%d]!\n",gEmu8583SendTable[iIndex].iLen,iIndex,iFieldBufSize);
		return(ERR_BUFFER_SIZE_SMALL);
	}
	memcpy(pFieldBuf,gEmu8583SendTable[iIndex].pItem,gEmu8583SendTable[iIndex].iLen);
	return(gEmu8583SendTable[iIndex].iLen);
}


