//	Date:	2002/6/20,	Wolfgang Wang

#include <stdio.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionDataExch.h"
#include "UnionXOR.h"
#include "UnionLog.h"

int UnionXOR(char *pData1, char *pData2, int nLenOfData, char *pData3)
{
	int	i;
	char	ch1,ch2;

	if (nLenOfData <= 0)
	{
		UnionUserErrLog("in UnionXOR:: nLenOfData [%d] Error!\n",nLenOfData);
		return (errCodeParameter);
	}

	for (i=0;i<nLenOfData;i++)
	{
		ch1=pData1[i];
		if (ch1>='A' && ch1 <='F') 
			ch1=ch1-0x40+0x09;
		else if (ch1>='a' && ch1 <='f') 
			ch1=ch1-0x60+0x09;
		else ch1 -= 0x30;

		ch2=pData2[i];
		if (ch2>='A' && ch2 <='F') 
			ch2=ch2-0x40+0x09;
		else if (ch2>='a' && ch2 <='f') 
			ch2=ch2-0x60+0x09;
		else ch2 -= 0x30;

		ch1=ch1 ^ ch2;

		if (ch1 > 0x09) ch1 = 0x40+(ch1-0x09);
		else ch1 += 0x30;
		pData3[i]=ch1;
	}

	return (nLenOfData);
}

