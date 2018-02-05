/*
Author:	wangcj
Date:	20081223
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int	UnionSetApplicationName(char *appName);
int	UnionEncryptDatabasePassword(char *plainStr, char *pciperPassword);
int	UnionExcutePreDefinedCommand(int argcNum, char **pargv);
int	UnionDecryptDatabasePassword(char *plainPWD, char *ciperPassword);

int UnionTaskActionBeforeExit()
{
	exit(1);
}


char appName[128];
int UnionHelp()
{
	printf("usage:: %s plainPassword ...\n", appName);
	return(0);
}


int main(int argc,char *argv[])
{
	int	index;
	char	ciperPassword[136];
	char	plainPWD[16][128];
	int	ret = 0;
	
	memset(appName, 0, sizeof(appName));
	strncpy(appName, argv[0], sizeof(appName) - 1);

	if (argc < 2)
	{
		printf("usage:: %s plainPassword ...\n", appName);
		return(-1);
	}
	else if(argc > 17)
	{
		printf("usage:: %s too args ...\n", appName);
		return(-2);
	}

	for (index = 1; index < argc; index++)
	{
		memset(plainPWD[index-1], 0, sizeof(plainPWD[index-1]));
		strcpy(plainPWD[index-1], argv[index]);
	}

	UnionSetApplicationName(argv[0]);
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);
	
	for (index = 1; index < argc; index++)
	{
		memset(ciperPassword,0,sizeof(ciperPassword));
		if (strlen(plainPWD[index-1]) != 32 && strlen(plainPWD[index-1]) != 48 && strlen(plainPWD[index-1]) != 64)
		{
			ret = UnionEncryptDatabasePassword(plainPWD[index-1], ciperPassword);
			if (ret < 0)
				printf("plain password [%s] len[%d] > 63\n",plainPWD[index-1],(int)strlen(plainPWD[index-1]));
			else
				printf("ciper password is [%s] of plain password [%s]\n",ciperPassword, plainPWD[index-1]);
		}
		else
		{
			UnionDecryptDatabasePassword(plainPWD[index-1], ciperPassword);
			printf("plain password is [%s] of ciper password [%s]\n",ciperPassword, plainPWD[index-1]);
		}
	}
	return(0);
}

