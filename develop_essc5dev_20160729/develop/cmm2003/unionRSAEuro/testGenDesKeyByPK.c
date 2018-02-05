#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include "unionGenKeyByPK.h"


int main(int argc,char *argv[])
{
	char key[49];
	char dekByPk[1024];
	char pk[]= "E72451C4ED22BE73935BD98749C290B21D44526A3D3FF5A3DF2A4EACBE5E5DB211B6BD5CBC258C193A4D7630538BBB14CA768B5729A4A6F7AF78E9523B4A0D8E28BA18CB02AE29760ABAE7C77BFED76CE0CE431F427B9C275F541B176174E3A4760BC17AFCF4B58BAB12E9259D1DBBDDC81C9BF5E9CCDBFF5E773F7A89BD4955";
	memset(key,0,sizeof(key));
	
	memset(key,0,sizeof(key));
	UnionGen2DESKey(key);
	printf("key=[%s]\n",key);
	memset(dekByPk,0,sizeof(dekByPk));
	UnionEncDesKeyBy1024PK(pk,key,dekByPk);
	printf("key=[%s],dekByPk=[%s]\n",key,dekByPk);
	
	
	memset(dekByPk,0,sizeof(dekByPk));
	memset(key,0,sizeof(key));
	UnionGen2DesKeyBy1024PK(pk,key,dekByPk);
	printf("key=[%s],dekByPk=[%s]\n",key,dekByPk);
	return 0;
}
