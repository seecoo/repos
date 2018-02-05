/*
Author:	zhangyongding
Date:	20081223
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "UnionStr.h"

int UnionEncryptDatabasePassword(char *plainPassword,char *ciperPassword)
{
	strcpy(ciperPassword,plainPassword);
	return(0);
}

int UnionDecryptDatabasePassword(char *ciperPassword,char *plainPassword)
{
	strcpy(plainPassword,ciperPassword);
	return(0);
}
