#include <stdio.h>
#include <string.h>
#include "UnionDes.h"
#include "UnionDataExch.h"
#include "unionBaseDes.h"

int main()
{
	char	data[16+1],key[16+1],result[16+1];
	char	bitData[8+1],bitKey[8+1],bitResult[8+1];
	
	memset(data,0,sizeof(data));
	memset(data,'1',16);
	memset(key,0,sizeof(key));
	memset(key,'1',16);
	memset(result,0,sizeof(result));
	
	UnionCompress128BCDInto64Bits(data,bitData);
	UnionCompress128BCDInto64Bits(key,bitKey);
	UnionCompress128BCDInto64Bits(result,bitResult);
	
	Des(bitData,bitResult,bitKey);
	UnionUncompress64BitsInto128BCD(bitResult,result);
	printf("result = [%s]\n",result);
	_Des(bitResult,bitResult,bitKey);
	UnionUncompress64BitsInto128BCD(bitResult,result);
	printf("result = [%s]\n",result);
	
	UNION_DES(bitData,bitResult,8,bitKey,1);
	UnionUncompress64BitsInto128BCD(bitResult,result);
	printf("result = [%s]\n",result);
	return(0);
}

	
