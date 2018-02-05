//	Wolfgang Wang
//	2009/4/29

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "unionTransferComplexDBDef.h"

int main(int argc,char *argv[])
{
	int	ret;
	int	index;
	
	if (argc < 2)
	{
		printf("usage::%s tablename\n");
		return(-1);
	}
	
	for (index = 1; index < argc; index++)
	{
		if ((ret = UnionInsertComplexDBObjectDefIntoTableList(argv[index])) < 0)
			printf("UnionInsertComplexDBObjectDefIntoTableList [%s] failure! ret = [%d]\n",argv[index],ret);
		else
			printf("UnionInsertComplexDBObjectDefIntoTableList [%s] OK! ret = [%d]\n",argv[index],ret);
	}
	return(ret);
}

