#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "batchComply.h"

int main(int argc,char *argv[])
{
	char	outputFile[512+1];
	
	if (argc < 3)
	{
		printf("Usage:: %s inputFile outputFile [mainDir]\n",argv[0]);
		return(-2);
	}
	if (argc > 3)
		return(UnionWriteLinkAllBinsBatchFile(argv[1],argv[2],argv[3]));
	else
		return(UnionWriteLinkAllBinsBatchFile(argv[1],argv[2],""));
}

int UnionReadIntTypeRECVar(char *str)
{
        return 0;
}
