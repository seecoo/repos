#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "UnionStr.h"

#include "defaultIBMPinOffsetDef.h"

char gdefaultIBMDefaultPinOffset[]        =    "FFFFFFFFFFFFFFFF";
char gdefaultIBMDecimalizationTable[]     =    "0123456789012345";
char gdefaultIBMUserDefinedData[]         =    "6127N3469134";
// 广发
char gdefaultIBMCGBDefinedData[]         =    "N56789ABCDEF";
int gdefaultIBMMinPINLength               =    4;		//最小PIN的长度

int UnionGetIBMMinPINLength()
{
	return(gdefaultIBMMinPINLength);
}

char *UnionGetIBMDecimalizationTable()
{
	return(gdefaultIBMDecimalizationTable);
}

char *UnionGetIBMUserDefinedData()
{
	if (strcasecmp(UnionGetIDOfCustomization(),"cgb") == 0)
		return(gdefaultIBMCGBDefinedData);
	else
		return(gdefaultIBMUserDefinedData);
}

char *UnionGetIBMUserDefinedDataByAccNo(char *accNo)
{
	if (strcasecmp(UnionGetIDOfCustomization(),"UnionPay") == 0)
		sprintf(gdefaultIBMUserDefinedData,"%.5sN%.6s",accNo+6,accNo);
	return(gdefaultIBMUserDefinedData);
}

int UnionGetIBMPinCheckData(char *checkData)
{
	int	i;
	int	flag = 0;
	
	srand((unsigned)time(NULL));
	
	flag = rand() % 12;
	for (i = 0; i < 12; i++)
	{
		if (i == flag)
			checkData[i] = 'N';
		else
			checkData[i] = '0' + (int)(10.0 * rand()/(RAND_MAX));
	}
	checkData[12] = 0;
	return(12);
}

int UnionGenerateIBMDecimalizationTable(char *decimalTable)
{
	int	i;
	
	srand((unsigned)time(NULL));
	
	for (i = 0; i < 16; i++)
	{
		decimalTable[i] = '0' + (int)(10.0 * rand()/(RAND_MAX));
	}
	decimalTable[16] = 0;
	return(16);
}
