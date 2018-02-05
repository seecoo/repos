#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <errno.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>


#include "symmetricKeyDB.h"

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionREC.h"

#include "highCached.h"
#include "unionMemcached.h"
#include "unionErrCode.h"


#define	maxPidNum	1000


int UnionTaskActionBeforeExit()
{
        return(0);
}

int main(int argc, char **argv)
{
	char value[1024];
	char key[128];
	int ret,len;
	int num,i,childNum;
	pid_t	childPidNum[maxPidNum],pid;
	char		highCachedKey[256+1];


	TUnionSymmetricKeyDB		symmetricKeyDB;
	PUnionSymmetricKeyDB psymmetricKeyDB;

	psymmetricKeyDB=&symmetricKeyDB;


	memset(value,0,sizeof(value));
        memset(key,0,sizeof(key));

	memcpy(key,argv[2],strlen(argv[2]));

	childNum=atoi(argv[3]);
	num=atoi(argv[4]);

	
	strcpy(psymmetricKeyDB->keyName,key);

	if ((ret = UnionTransformSymmetricKeyName(psymmetricKeyDB->keyName,sizeof(psymmetricKeyDB->keyName))) < 0)
	{
		UnionUserErrLog("in UnionCreateSymmetricKeyDB:: UnionTransformSymmetricKeyName[%s]!\n",psymmetricKeyDB->keyName);
		return(ret);
	}

	len = UnionGetKeyOfHighCachedForSymmetricKeyDB(key,highCachedKey);
	highCachedKey[len] = 0;
		
	//printf("*********** highCachedKey[%s] !*************\n",highCachedKey);	

	if ((ret = UnionConnectMemcached()) <0)
	{
		UnionUserErrLog("UnionSetMemcachedValue::UnionConnectMemcached!\n");
		return(ret);
	}	

	
	for (i = 0; i < childNum; i++)
	{
		if ((childPidNum[i] = fork()) < 0)
		{
			printf("创建进程失败!\n");
			exit(0);
		}
		else if (childPidNum[i] == 0)
		{
			goto childProcess;
		}
	}

	for (i = 0; i < childNum; i++)
	{
		if ((pid = wait(NULL)) < 0)
		{
			printf("wait error!\n");
		}
		else
			printf("getpid[%d] is over!\n",pid);
	}

	
	exit(0);

childProcess:

	for(i=0;i<num;i++)
	{
		if((strcasecmp(argv[1],"-GET"))==0)
		{
			if((ret=UnionGetMemcachedValue(highCachedKey,value,sizeof(value)))<0)
			{
				UnionUserErrLog("in UnionGetMemcachedValue:: error!\n");
				printf("in UnionGetMemcachedValue:: error!\n");
			}
			else
			{
				if ((ret = UnionSymmetricKeyDBStringToStruct(value,psymmetricKeyDB)) < 0)
				{
					UnionUserErrLog("in UnionReadSymmetricKeyDBRec:: UnionSymmetricKeyDBStringToStruct keyDBStr[%s]!\n",value);
					return(ret);
				}					
				UnionProgramerLog("Get_NUM[%d]::keyName[%s]::value[%s] highCached!\n",i+1,key,psymmetricKeyDB->keyValue[0].keyValue);
		//		printf("keyName_value[%s] highCached!\n",psymmetricKeyDB->keyValue[0].keyValue);
			///	printf("in UnionGetMemcachedValue::OK: value[%s]!\n",value);
			//	UnionLog("in UnionGetMemcachedValue::OK: value[%s]!\n",value);
			}
			//UnionTestMemcachedIP(highCachedKey, strlen(highCachedKey));	
		}
		else 
		{
			UnionUserErrLog("in UnionGetMemcachedValue:: commd error!\n");
			printf("in UnionGetMemcachedValue:: commd error!\n");
			break;
		}
	}
	exit(0);	

}
