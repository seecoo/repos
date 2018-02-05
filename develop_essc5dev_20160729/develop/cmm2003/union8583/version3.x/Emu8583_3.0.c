//	Author:	Wolfgang Wang
//	Date:	2002/7/31


#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>

#include "Union8583_3.x.h"
#include "UnionLog.h"


int UnionInitEmu8583_3_x_Attribute(PUnion8583_3_x p8583_3_x,char *p8583DefFileName)
{
	int	iFieldNo;
	FILE	*fp;
	char	tmpBuf[50];
	int	Ret;
		
	if (p8583_3_x == NULL)
	{
		UnionUserErrLog("in UnionInitEmu8583_3_x_Attribute:: p8583_3_x is NULL point!\n");
		return(-1);
	}
	
	if (p8583DefFileName == NULL)
	{
		UnionUserErrLog("in UnionInitEmu8583_3_x_Attribute:: p8583DefFileName is NULL point!\n");
		return(-1);
	}

	if ((fp=fopen(p8583DefFileName,"r+")) == NULL)
	{
		UnionSystemErrLog("In UnionInitEmu8583_3_x_Attribute:: Can't open the file [%s]!\n",p8583DefFileName);
		return(-1);
	}

	UnionLog("in UnionInitEmu8583_3_x_Attribute::8583 attribute::\n");
	while (!feof(fp))
	{
		if (fscanf(fp,"%d",&iFieldNo) < 0)
		{
			Ret = 0;
			break;
		}

		if ((iFieldNo >= 129) || (iFieldNo < 0))
			continue;
		
		if (fscanf(fp,"%d%d",&((p8583_3_x + iFieldNo)->iVarLen),&((p8583_3_x + iFieldNo)->iLen)) == EOF)
		{
			UnionUserErrLog("in UnionInitEmu8583_3_x_Attribute:: fscanf Index [%d]!\n",iFieldNo);
			Ret = -1;
			break;
		}
		
		UnionNullLog("[%d] [%d] [%d]\n",iFieldNo,(p8583_3_x + iFieldNo)->iVarLen,(p8583_3_x + iFieldNo)->iLen);

		// ����
		if ((p8583_3_x + iFieldNo)->iLen == 0)
			continue;
		
		// ��ֵ̫����Ϊ��ֵ	
		if ((p8583_3_x + iFieldNo)->iLen < 0 || (p8583_3_x + iFieldNo)->iLen > 30000)
		{
			UnionUserErrLog("in UnionInitEmu8583_3_x_Attribute:: Index [%d] Len [%d] outof range\n",
				iFieldNo,(p8583_3_x + iFieldNo)->iLen);
			Ret = -1;
			break;
		}
		
		// �䳤��ĳ�����̫����Ϊ����
		if (((p8583_3_x + iFieldNo)->iVarLen < 0) || ((p8583_3_x + iFieldNo)->iVarLen > 20))
		{
			UnionUserErrLog("in UnionInitEmu8583_3_x_Attribute:: Index [%d] VarLen [%d] outof range\n",
				iFieldNo,(p8583_3_x + iFieldNo)->iVarLen);
			Ret = -1;
			break;
		}
		
		if ((p8583_3_x + iFieldNo)->iVarLen > 0)
		{
			// �䳤��ĳ��������ʾ��󳤶ȡ�
			memset(tmpBuf,0,sizeof(tmpBuf));
			sprintf(tmpBuf,"%d",(p8583_3_x + iFieldNo)->iLen);
			if (strlen(tmpBuf) > (p8583_3_x + iFieldNo)->iVarLen)
			{
				UnionUserErrLog("in UnionInitEmu8583_3_x_Attribute:: Index [%d] VarLen [%d] small than expected [%d]\n",
						iFieldNo,(p8583_3_x + iFieldNo)->iVarLen,strlen(tmpBuf));
				Ret = -1;
				break;
			}
		}
		
		// Ϊ�����洢�ռ䡣
		if (((p8583_3_x + iFieldNo)->pItem = (unsigned char *)malloc((p8583_3_x + iFieldNo)->iLen+1)) == NULL)
		{
			UnionUserErrLog("in UnionInitEmu8583_3_x_Attribute:: malloc Index [%d] error!\n",iFieldNo);
			Ret = -1;
			break;
		}
	}
	UnionNullLog("in UnionInitEmu8583_3_x_Attribute:: Init OK!\n");

	fclose(fp);
	
	return(0);
}

PUnion8583_3_x UnionConnectEmu8583_3_x_MDL(char *p8583DefFileName)
{
	int		iRet;
	PUnion8583_3_x	p8583_3_x;
	int		i;
	
	if (p8583DefFileName == NULL)
	{
		UnionUserErrLog("in UnionConnectEmu8583_3_x_MDL:: p8583DefFileName is NULL point!");
		return(NULL);
	}

	if ((p8583_3_x = (PUnion8583_3_x)malloc(sizeof(TUnion8583_3_x) * 129)) == NULL)
	{
		UnionSystemErrLog("in UnionConnectEmu8583_3_x_MDL:: malloc!\n");
		return(NULL);
	}
	
	for (i = 0; i < 129; i++)
	{
		(p8583_3_x + i)->iVarLen = 0;
		(p8583_3_x + i)->iLen = 0;
		(p8583_3_x + i)->iRealLen = 0;
		(p8583_3_x + i)->pItem = NULL;
	}
	
	if ((iRet = UnionInitEmu8583_3_x_Attribute(p8583_3_x,p8583DefFileName)) != 0)
	{
		UnionUserErrLog("in UnionConnectEmu8583_3_x_MDL:: UnionInitEmu8583_3_x_Attribute fail! return=[%d]\n",iRet);
		UnionDisconnectEmu8583_3_x_MDL(p8583_3_x);
		return(NULL);
	}
	else
		return(p8583_3_x);
}

int UnionDisconnectEmu8583_3_x_MDL(PUnion8583_3_x p8583_3_x)
{
	int	i;
	
	if (p8583_3_x == NULL)
	{
		UnionUserErrLog("in UnionDisconnectEmu8583_3_x_MDL:: p8583_3_x is NULL!\n");
		return(-1);
	}
	for (i = 0; i < 129; i++)
	{
		if ((p8583_3_x + i)->pItem != NULL)
			free((p8583_3_x + i)->pItem);
	}
	
	free(p8583_3_x);
	
	return(0);
}

int UnionInitEmu8583_3_x(PUnion8583_3_x p8583_3_x)
{
	int i;

	if (p8583_3_x == NULL)
	{
		UnionUserErrLog("in UnionInitEmu8583_3_x:: p8583_3_x is NULL point!\n");
		return(-1);
	}
	
    	for (i = 0; i < 129; i++)
    	{
    		(p8583_3_x + i)->iRealLen = 0;
    		if ((p8583_3_x + i)->pItem != NULL)
    			memset((p8583_3_x + i)->pItem,0,(p8583_3_x + i)->iLen+1);
    	}
    	
    	return(0);
    	
}

int UnionPackEmu8583_3_x(PUnion8583_3_x p8583_3_x,unsigned char *pPackBuf,int iPackBufSize)
{
	int		i;
	int		iTotalLen;	// The return value, Length of 8583 package
	unsigned char	*p;		// Work pointer, point to 8583 package
	char		tmpBuf[20];
	
	if (p8583_3_x == NULL)
	{
		UnionUserErrLog("in UnionPackEmu8583_3_x:: p8583_3_x is NULL!\n");
		return(-1);
	}

	if (pPackBuf == NULL)
	{
		UnionUserErrLog("in UnionPackEmu8583_3_x:: pPackBuf is NULL point!\n");
		return(-1);
	}
	if (iPackBufSize <= 0)
	{
		UnionUserErrLog("in UnionPackEmu8583_3_x:: Pack buffer size [%d] is too small!\n",iPackBufSize);
		return(-1);
	}

	// Debug Begin
	UnionLog("Before pack Emu8583");
	for (i = 0; i < 129; i++)
	{
		if ((p8583_3_x + i)->iRealLen > 0)
			UnionNullLog("i = %03d [%05d] buf = *%s*\n",i,(p8583_3_x+i)->iRealLen,(p8583_3_x + i)->pItem);
	}
	UnionNullLog("\n");
	// Debug End

	// Pack
	iTotalLen=0;
	p=pPackBuf;
	for (i = 0; i < 129; i++)
	{
		if ((p8583_3_x + i)->iLen <= 0) // ����ֵ��
			continue;
		// ����Ƿ񳬳��ˡ�
		if ((p8583_3_x + i)->iRealLen > (p8583_3_x + i)->iLen)
		{
			UnionUserErrLog("in UnionPackEmu8583_3_x:: RealLen [%d] > DefinedLen [%d]\n",
					(p8583_3_x + i)->iRealLen,(p8583_3_x + i)->iLen);
			return(-1);
		}
		if ((p8583_3_x + i)->iVarLen > 0)	// �䳤��
		{
			// �����򳤶ȡ�
			memset(tmpBuf,0,sizeof(tmpBuf));
			sprintf(tmpBuf,"%d",(p8583_3_x + i)->iRealLen);
			memset(p,'0',(p8583_3_x + i)->iVarLen);
			memcpy(p + (p8583_3_x + i)->iVarLen - strlen(tmpBuf),tmpBuf,strlen(tmpBuf));
			iTotalLen += (p8583_3_x + i)->iVarLen;
			p = pPackBuf + iTotalLen;
			memcpy(p,(p8583_3_x + i)->pItem,(p8583_3_x + i)->iRealLen);
			iTotalLen += (p8583_3_x + i)->iRealLen;
			p = pPackBuf + iTotalLen;
		}
		else	// ������
		{
			memset(p,' ',(p8583_3_x + i)->iLen);	// ����λǰ���ո�
			memcpy(p + (p8583_3_x + i)->iLen - (p8583_3_x + i)->iRealLen,(p8583_3_x + i)->pItem,(p8583_3_x + i)->iRealLen);
			iTotalLen += (p8583_3_x + i)->iLen;
			p = pPackBuf + iTotalLen;
		}
	}

	UnionNullLog("Pack OK!\n");
	return(iTotalLen);
}

int UnionUnpackEmu8583_3_x(PUnion8583_3_x p8583_3_x,unsigned char *pUnpackBuf,int iUnpackBufLen)
{
	int		i;
	unsigned char	*p,*pEnd;
	char		tmpBuf[50];

	if (p8583_3_x == NULL)
	{
		UnionUserErrLog("in UnionUnpackEmu8583_3_x:: p8583_3_x is NULL!\n");
		return(-1);
	}

	if (pUnpackBuf == NULL)
	{
		UnionUserErrLog("in UnionUnpackEmu8583_3_x:: pUnpackBuf is NULL point!\n");
		return(-1);
	}
	
	if (iUnpackBufLen <= 0)
	{
		UnionUserErrLog("in UnionUnpackEmu8583_3_x:: Unpack buffer length [%d] is error!\n",iUnpackBufLen);
		return(-1);
	}

	UnionInitEmu8583_3_x(p8583_3_x);
	p = pUnpackBuf;					// Unpack buffer start
	pEnd = pUnpackBuf+iUnpackBufLen;		// Unpack buffer end

	// Unpack
	for (i = 0; i < 129; i++)
	{
		if ((p8583_3_x + i)->iLen <= 0)	// �����ڸ���
			continue;
		if ((p8583_3_x + i)->iVarLen == 0)	// ������
		{	// ����򳤶�
			(p8583_3_x + i)->iRealLen = (p8583_3_x + i)->iLen;
		}
		else	// �䳤��
		{
			if (p + (p8583_3_x + i)->iVarLen > pEnd)
			{
				UnionUserErrLog("in UnionUnpackEmu8583_3_x: [%d] out of range Var!\n",i);
				return(-1);
			}
			// ��ñ䳤��ĳ���
			memset(tmpBuf,0,sizeof(tmpBuf));
			memcpy(tmpBuf,p,(p8583_3_x + i)->iVarLen);
			(p8583_3_x + i)->iRealLen = atoi(tmpBuf);
			p += (p8583_3_x + i)->iVarLen;
		}
		
		// ������ֵ
		if (p + (p8583_3_x + i)->iRealLen > pEnd)
		{	// ����
			UnionUserErrLog("in UnionUnpackEmu8583_3_x: [%d] out of range!\n",i);
			return(-1);
		}
		memcpy((p8583_3_x + i)->pItem,p,(p8583_3_x + i)->iRealLen);
		p += (p8583_3_x + i)->iRealLen;
	}

	// Debug Begin
	UnionLog("After unpack 8583:\n");
	for (i = 0; i < 129; i++)
	{
		if ((p8583_3_x + i)->iRealLen > 0)
			UnionNullLog("i = %03d buf = *%s*\n",i,(p8583_3_x + i)->pItem);
	}
	UnionNullLog("\n");
	// Debug End

	return(0);	
}

int UnionSetEmu8583_3_x_Field(PUnion8583_3_x p8583_3_x,int iIndex,unsigned char *pFieldBuf,int iFieldBufLen)
{
	if (p8583_3_x == NULL)
	{
		UnionUserErrLog("in UnionSetEmu8583_3_x_Field:: p8583_3_x is NULL!\n");
		return(-1);
	}

	if ((iIndex < 0) || (iIndex > 128))
	{
		UnionUserErrLog("in UnionSetEmu8583_3_x_Field:: Field index [%d] is invalid!\n",iIndex);
		return(-1);
	}
	
	if (pFieldBuf == NULL)
	{
		UnionUserErrLog("in UnionSetEmu8583_3_x_Field:: pFieldBuf is NULL point!\n");
		return(-1);
	}
	
	if (iFieldBufLen < 0)
	{
		UnionUserErrLog("in UnionSetEmu8583_3_x_Field:: Field buffer length [%d] of field [%d] is error!\n",
				iFieldBufLen,iIndex);
		return(-1);
	}

	if (iFieldBufLen > (p8583_3_x + iIndex)->iLen)
	{
		UnionUserErrLog("in UnionSetEmu8583_3_x_Field:: The field [%d] length [%d] larger than defined length [%d]!\n",
				iIndex,iFieldBufLen,(p8583_3_x + iIndex)->iLen);
		return(-1);
	}

	(p8583_3_x + iIndex)->iRealLen = iFieldBufLen;
	if ((p8583_3_x + iIndex)->iRealLen == 0)
	{
		return(0);
	}
	memset((p8583_3_x + iIndex)->pItem,0,(p8583_3_x + iIndex)->iLen+1);
	memcpy((p8583_3_x + iIndex)->pItem,pFieldBuf,(p8583_3_x + iIndex)->iRealLen);
	
	return(0);
}

int UnionReadEmu8583_3_x_Field(PUnion8583_3_x p8583_3_x,int iIndex,unsigned char *pFieldBuf,int iFieldBufSize)
{
	if (p8583_3_x == NULL)
	{
		UnionUserErrLog("in UnionReadEmu8583_3_x_Field:: p8583_3_x is NULL!\n");
		return(-1);
	}
	
	if ((iIndex < 0) || (iIndex > 128))
	{
		UnionUserErrLog("in UnionReadEmu8583_3_x_Field:: Field index [%d] is invalid!\n",iIndex);
		return(-1);
	}
	if (pFieldBuf == NULL)
	{
		UnionUserErrLog("in UnionReadEmu8583_3_x_Field:: pFieldBuf is NULL point!\n");
		return(-1);
	}
	if (iFieldBufSize <= 0)
	{
		UnionUserErrLog("in UnionReadEmu8583_3_x_Field:: Field buffer size [%d] is too small!\n",iFieldBufSize);
		return(-1);
	}

	if ((p8583_3_x + iIndex)->iRealLen == 0)		// This field has no content
	{
		pFieldBuf[0] = '\0';
		return(0);
	}

	if ((p8583_3_x + iIndex)->iRealLen > iFieldBufSize)
	{
		UnionUserErrLog("in UnionReadEmu8583_3_x_Field:: Length [%d] of field [%d] is larger than size of field [%d]!\n",
				(p8583_3_x + iIndex)->iLen,iIndex,iFieldBufSize);
		return(-1);
	}
	memcpy(pFieldBuf,(p8583_3_x + iIndex)->pItem,(p8583_3_x + iIndex)->iRealLen);
	return((p8583_3_x + iIndex)->iRealLen);
}

