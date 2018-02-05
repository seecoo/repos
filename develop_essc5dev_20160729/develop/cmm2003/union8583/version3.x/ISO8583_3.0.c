//	Author:	Wolfgang Wang
//	Date:	2002/8/31


#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>

#include "Union8583_3.x.h"
#include "UnionLog.h"


int UnionInit8583_3_x_Attribute(PUnion8583_3_x p8583_3_x,char *p8583DefFileName)
{
	int	iFieldNo;
	FILE	*fp;
	char	tmpBuf[50];
	int	Ret;
		
	if (p8583_3_x == NULL)
	{
		UnionUserErrLog("in UnionInit8583_3_x_Attribute:: p8583_3_x is NULL point!\n");
		return(-1);
	}
	
	if (p8583DefFileName == NULL)
	{
		UnionUserErrLog("in UnionInit8583_3_x_Attribute:: p8583DefFileName is NULL point!\n");
		return(-1);
	}

	if ((fp=fopen(p8583DefFileName,"r+")) == NULL)
	{
		UnionSystemErrLog("In UnionInit8583_3_x_Attribute:: Can't open the file [%s]!\n",p8583DefFileName);
		return(-1);
	}

	UnionLog("in UnionInit8583_3_x_Attribute::8583 attribute::\n");
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
			UnionUserErrLog("in UnionInit8583_3_x_Attribute:: fscanf Index [%d]!\n",iFieldNo);
			Ret = -1;
			break;
		}
		
		UnionNullLog("[%d] [%d] [%d]\n",iFieldNo,(p8583_3_x + iFieldNo)->iVarLen,(p8583_3_x + iFieldNo)->iLen);

		// 空域；
		if ((p8583_3_x + iFieldNo)->iLen == 0)
			continue;
		
		// 域值太长或为负值	
		if ((p8583_3_x + iFieldNo)->iLen < 0 || (p8583_3_x + iFieldNo)->iLen > 30000)
		{
			UnionUserErrLog("in UnionInit8583_3_x_Attribute:: Index [%d] Len [%d] outof range\n",
				iFieldNo,(p8583_3_x + iFieldNo)->iLen);
			Ret = -1;
			break;
		}
		
		// 变长域的长度域太长或为负。
		if (((p8583_3_x + iFieldNo)->iVarLen < 0) || ((p8583_3_x + iFieldNo)->iVarLen > 20))
		{
			UnionUserErrLog("in UnionInit8583_3_x_Attribute:: Index [%d] VarLen [%d] outof range\n",
				iFieldNo,(p8583_3_x + iFieldNo)->iVarLen);
			Ret = -1;
			break;
		}
		
		if ((p8583_3_x + iFieldNo)->iVarLen > 0)
		{
			// 变长域的长度域不足表示最大长度。
			memset(tmpBuf,0,sizeof(tmpBuf));
			sprintf(tmpBuf,"%d",(p8583_3_x + iFieldNo)->iLen);
			if (strlen(tmpBuf) > (p8583_3_x + iFieldNo)->iVarLen)
			{
				UnionUserErrLog("in UnionInit8583_3_x_Attribute:: Index [%d] VarLen [%d] small than expected [%d]\n",
						iFieldNo,(p8583_3_x + iFieldNo)->iVarLen,strlen(tmpBuf));
				Ret = -1;
				break;
			}
		}
		
		// 为域分配存储空间。
		if (((p8583_3_x + iFieldNo)->pItem = (unsigned char *)malloc((p8583_3_x + iFieldNo)->iLen+1)) == NULL)
		{
			UnionUserErrLog("in UnionInit8583_3_x_Attribute:: malloc Index [%d] error!\n",iFieldNo);
			Ret = -1;
			break;
		}
	}
	UnionNullLog("in UnionInit8583_3_x_Attribute:: Init OK!\n");

	fclose(fp);
	
	return(0);
}

PUnion8583_3_x UnionConnect8583_3_x_MDL(char *p8583DefFileName)
{
	int		iRet;
	PUnion8583_3_x	p8583_3_x;
	int		i;
	
	if (p8583DefFileName == NULL)
	{
		UnionUserErrLog("in UnionConnect8583_3_x_MDL:: p8583DefFileName is NULL point!");
		return(NULL);
	}

	if ((p8583_3_x = (PUnion8583_3_x)malloc(sizeof(TUnion8583_3_x) * 129)) == NULL)
	{
		UnionSystemErrLog("in UnionConnect8583_3_x_MDL:: malloc!\n");
		return(NULL);
	}
	
	for (i = 0; i < 129; i++)
	{
		(p8583_3_x + i)->iVarLen = 0;
		(p8583_3_x + i)->iLen = 0;
		(p8583_3_x + i)->iRealLen = 0;
		(p8583_3_x + i)->pItem = NULL;
	}
	
	if ((iRet = UnionInit8583_3_x_Attribute(p8583_3_x,p8583DefFileName)) != 0)
	{
		UnionUserErrLog("in UnionConnect8583_3_x_MDL:: UnionInit8583_3_x_Attribute fail! return=[%d]\n",iRet);
		UnionDisconnect8583_3_x_MDL(p8583_3_x);
		return(NULL);
	}
	else
		return(p8583_3_x);
}

int UnionDisconnect8583_3_x_MDL(PUnion8583_3_x p8583_3_x)
{
	int	i;
	
	if (p8583_3_x == NULL)
	{
		UnionUserErrLog("in UnionDisconnect8583_3_x_MDL:: p8583_3_x is NULL!\n");
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

int UnionInit8583_3_x(PUnion8583_3_x p8583_3_x)
{
	int i;

	if (p8583_3_x == NULL)
	{
		UnionUserErrLog("in UnionInit8583_3_x:: p8583_3_x is NULL point!\n");
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

int UnionSet8583FieldMap(unsigned char *pFieldMap,int iIndex)
{
	unsigned char	*p;
	unsigned char	g_caBIT[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

	p=pFieldMap;

	// between 1 to 128
	if ((iIndex <= 0) || (iIndex > 128))
    		return(-1);

	p += ((iIndex-1) / 8);
	p[0] = p[0] | g_caBIT[(iIndex-1) % 8];

	return(0);	
}

int UnionPack8583_3_x(PUnion8583_3_x p8583_3_x,unsigned char *pPackBuf,int iPackBufSize)
{
	int		i;
	int		iTotalLen;	// The return value, Length of 8583 package
	unsigned char	*p;		// Work pointer, point to 8583 package
	char		tmpBuf[20];
	
	if (p8583_3_x == NULL)
	{
		UnionUserErrLog("in UnionPack8583_3_x:: p8583_3_x is NULL!\n");
		return(-1);
	}

	if (pPackBuf == NULL)
	{
		UnionUserErrLog("in UnionPack8583_3_x:: pPackBuf is NULL point!\n");
		return(-1);
	}
	if (iPackBufSize <= 0)
	{
		UnionUserErrLog("in UnionPack8583_3_x:: Pack buffer size [%d] is too small!\n",iPackBufSize);
		return(-1);
	}

	// Debug Begin
	UnionLog("Before pack 8583");
	for (i = 0; i < 129; i++)
	{
		if ((p8583_3_x + i)->iRealLen > 0)
			UnionNullLog("i = %03d [%05d] buf = *%s*\n",i,(p8583_3_x+i)->iRealLen,(p8583_3_x + i)->pItem);
	}
	UnionNullLog("\n");
	// Debug End

	// Pack
	iTotalLen = 0;
	p = pPackBuf;
	memcpy(p,p8583_3_x->pItem,4);	// 拷贝MTI
	iTotalLen += 4;
	p += 4;
	memset(p,0,16);			// 置BitMap为空。
	iTotalLen += 16;
	p += 16;
	UnionSet8583FieldMap(pPackBuf+4,1);
	
	for (i = 2; i < 129; i++)
	{
		if ((p8583_3_x + i)->iLen <= 0) // 不应该有域值。
			continue;
		if ((p8583_3_x + i)->iRealLen <= 0) // 无域值。
			continue;

		// 检查是否超长了。
		if ((p8583_3_x + i)->iRealLen > (p8583_3_x + i)->iLen)
		{
			UnionUserErrLog("in UnionPack8583_3_x:: RealLen [%d] > DefinedLen [%d]\n",
					(p8583_3_x + i)->iRealLen,(p8583_3_x + i)->iLen);
			return(-1);
		}
		if ((p8583_3_x + i)->iVarLen > 0)	// 变长域
		{
			// 拷贝域长度。
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
		else	// 定长域
		{
			memset(p,' ',(p8583_3_x + i)->iLen);	// 不足位前补空格
			memcpy(p + (p8583_3_x + i)->iLen - (p8583_3_x + i)->iRealLen,(p8583_3_x + i)->pItem,(p8583_3_x + i)->iRealLen);
			iTotalLen += (p8583_3_x + i)->iLen;
			p = pPackBuf + iTotalLen;
		}
		UnionSet8583FieldMap(pPackBuf+4,i);
	}

	UnionNullLog("Pack OK!\n");
	return(iTotalLen);
}

int UnionExist8583Field(unsigned char *pFieldMap,int iIndex)
{
	unsigned char	*p;
	unsigned char	g_caBIT[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

	p=pFieldMap;

	// between 1 to 128
	if ((iIndex <= 0) || (iIndex > 128))
		return(-1);

	p += ( (iIndex-1) / 8);

	if (p[0] & g_caBIT[ (iIndex-1) % 8 ])
		return(1);
	else
		return(0);
}

int UnionUnpack8583_3_x(PUnion8583_3_x p8583_3_x,unsigned char *pUnpackBuf,int iUnpackBufLen)
{
	int		i;
	unsigned char	*p,*pEnd;
	char		tmpBuf[50];

	if (p8583_3_x == NULL)
	{
		UnionUserErrLog("in UnionUnpack8583_3_x:: p8583_3_x is NULL!\n");
		return(-1);
	}

	if (pUnpackBuf == NULL)
	{
		UnionUserErrLog("in UnionUnpack8583_3_x:: pUnpackBuf is NULL point!\n");
		return(-1);
	}
	
	if (iUnpackBufLen <= 4+16)
	{
		UnionUserErrLog("in UnionUnpack8583_3_x:: Unpack buffer length [%d] is error!\n",iUnpackBufLen);
		return(-1);
	}

	UnionInit8583_3_x(p8583_3_x);
	p = pUnpackBuf;					// Unpack buffer start
	pEnd = pUnpackBuf+iUnpackBufLen;		// Unpack buffer end

	// 拷贝MTI
	memcpy(p8583_3_x->pItem,p,4);
	p8583_3_x->iRealLen = 4;
	memcpy((p8583_3_x+1)->pItem,"16",2);
	(p8583_3_x+1)->iRealLen = 2;
	p = p + 4 + 16;		// 跳过MTI及位元
	
	// Unpack
	for (i = 2; i < 129; i++)
	{
		if (!UnionExist8583Field(pUnpackBuf+4,i))	// This field has no content
			continue;

		//if ((p8583_3_x + i)->iLen <= 0)	// 不存在该域
		//	continue;
		
		if ((p8583_3_x + i)->iVarLen == 0)	// 定长域
		{	// 获得域长度
			(p8583_3_x + i)->iRealLen = (p8583_3_x + i)->iLen;
		}
		else	// 变长域
		{
			if (p + (p8583_3_x + i)->iVarLen > pEnd)
			{
				UnionUserErrLog("in UnionUnpack8583_3_x: [%d] out of range Var!\n",i);
				return(-1);
			}
			// 获得变长域的长度
			memset(tmpBuf,0,sizeof(tmpBuf));
			memcpy(tmpBuf,p,(p8583_3_x + i)->iVarLen);
			(p8583_3_x + i)->iRealLen = atoi(tmpBuf);
			p += (p8583_3_x + i)->iVarLen;
		}
		
		// 拷贝域值
		if (p + (p8583_3_x + i)->iRealLen > pEnd)
		{	// 超界
			UnionUserErrLog("in UnionUnpack8583_3_x: [%d] out of range!\n",i);
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

int UnionSet8583_3_x_Field(PUnion8583_3_x p8583_3_x,int iIndex,unsigned char *pFieldBuf,int iFieldBufLen)
{
	if (p8583_3_x == NULL)
	{
		UnionUserErrLog("in UnionSet8583_3_x_Field:: p8583_3_x is NULL!\n");
		return(-1);
	}

	if ((iIndex < 0) || (iIndex > 128))
	{
		UnionUserErrLog("in UnionSet8583_3_x_Field:: Field index [%d] is invalid!\n",iIndex);
		return(-1);
	}
	
	if (pFieldBuf == NULL)
	{
		UnionUserErrLog("in UnionSet8583_3_x_Field:: pFieldBuf is NULL point!\n");
		return(-1);
	}
	
	if (iFieldBufLen < 0)
	{
		UnionUserErrLog("in UnionSet8583_3_x_Field:: Field buffer length [%d] of field [%d] is error!\n",
				iFieldBufLen,iIndex);
		return(-1);
	}

	if (iFieldBufLen > (p8583_3_x + iIndex)->iLen)
	{
		UnionUserErrLog("in UnionSet8583_3_x_Field:: The field [%d] length [%d] larger than defined length [%d]!\n",
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

int UnionRead8583_3_x_Field(PUnion8583_3_x p8583_3_x,int iIndex,unsigned char *pFieldBuf,int iFieldBufSize)
{
	if (p8583_3_x == NULL)
	{
		UnionUserErrLog("in UnionRead8583_3_x_Field:: p8583_3_x is NULL!\n");
		return(-1);
	}
	
	if ((iIndex < 0) || (iIndex > 128))
	{
		UnionUserErrLog("in UnionRead8583_3_x_Field:: Field index [%d] is invalid!\n",iIndex);
		return(-1);
	}
	if (pFieldBuf == NULL)
	{
		UnionUserErrLog("in UnionRead8583_3_x_Field:: pFieldBuf is NULL point!\n");
		return(-1);
	}
	if (iFieldBufSize <= 0)
	{
		UnionUserErrLog("in UnionRead8583_3_x_Field:: Field buffer size [%d] is too small!\n",iFieldBufSize);
		return(-1);
	}

	if ((p8583_3_x + iIndex)->iRealLen == 0)		// This field has no content
	{
		pFieldBuf[0] = '\0';
		return(0);
	}

	if ((p8583_3_x + iIndex)->iRealLen > iFieldBufSize)
	{
		UnionUserErrLog("in UnionRead8583_3_x_Field:: Length [%d] of field [%d] is larger than size of field [%d]!\n",
				(p8583_3_x + iIndex)->iLen,iIndex,iFieldBufSize);
		return(-1);
	}
	memcpy(pFieldBuf,(p8583_3_x + iIndex)->pItem,(p8583_3_x + iIndex)->iRealLen);
	return((p8583_3_x + iIndex)->iRealLen);
}

