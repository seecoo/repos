#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif
#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif
#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif


#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "unionCommand.h"
#include "unionErrCode.h"
#include "UnionStr.h"
#include "unionRealDBCommon.h"
#include "unionRealBaseDB.h"
#include "symmetricKeyDB.h"
#include "unionVersion.h"
#include "UnionTask.h"
#include "unionHsmCmd.h"

int UnionHelp()
{
	printf("Usage:: %s  keyInfoFile  keyNodeFile hsmGroupID(可选,默认为:default)\n",UnionGetApplicationName());
	printf("                  keyInfoFile format:\n");
	printf("                      数据以分号(;)分隔, 第1个是密钥名,第2个是密钥类型,第3个是密钥长度,第4个可选为密钥算法, 最后两个可选是密钥值和校验值:\n");
	printf("                      密钥名的中间名称固定为null\n");
	printf("                      eg:   TE.null.zak;zak;32\n");
	printf("                            TE.null.zak;zak;32;SM4\n");
	printf("                            TE.null.zak;zak;32;DES;2E2001AF27074DDF2FA094B6F3A45F59;3389EE455F53F924\n");
	printf("                            TE.null.zak;zak;32;2E2001AF27074DDF2FA094B6F3A45F59;3389EE455F53F924\n");
	printf("                  keyNodeFile format:\n");
	printf("                      只有一列数据，是密钥名称的中间部分组成\n");

				
	printf("\n");
	printf("Usage:: %s  appID keyType keyLen startNode endNode [DES|SM4] [hsmGroupID] (可选,默认为:default)\n",UnionGetApplicationName());
	return(0);
}

int UnionMngDesKeyDBHelp()
{
	printf("Usage:: %s  read  keyName\n",UnionGetApplicationName());
	return(0);
}

int UnionTaskActionBeforeExit()
{
	UnionDisconnectHighCached();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

int main(int argc,char *argv[])
{
	int			ret;
	FILE			*fpKeyInfo = NULL;
	char			keyInfoFileName[256+1];
	char			keyNodeFileName[256+1];
	int			len = 0;
	char			oriBuf[1024+1];
	char			desBuf[1024+1];
	char			buf[256];
	char			keyType[10+1];
	int			isGenerateKeyValue = 1;
	char			tmpBuf[128+1];
	char			lmkProtectMode[16+1];
	char			hsmGroupID[8+1];

	TUnionSymmetricKeyDB	symmetricKeyDB;

	UnionSetApplicationName(argv[0]);

	if (argc < 3 )
	{
		if (strcasecmp(UnionGetApplicationName(),"mngDesKeyDB") == 0)
			return(UnionMngDesKeyDBHelp());
		else
			return(UnionHelp());
	}
		
		
	// add by chenwd 20151028
	if (strcasecmp(UnionGetApplicationName(),"mngDesKeyDB") == 0)
	{
		if (strcasecmp(argv[1],"read") == 0)
		{
			memset(tmpBuf,0,sizeof(tmpBuf));
			strcpy(tmpBuf,argv[2]);
	
			// 读取对称密钥
			UnionReadKeyInfo(tmpBuf);
	
			return(0);
		}
		else
			return(UnionMngDesKeyDBHelp());
	}
	// add end by chenwd 20151028

	if (argc >=5)
	{
		memset(&symmetricKeyDB,0,sizeof(symmetricKeyDB));

		strcpy(hsmGroupID,"default");
		if (argc > 6)
		{
			if(!memcmp(buf, "SM4", 3))
			{
				symmetricKeyDB.algorithmID = conSymmetricAlgorithmIDOfSM4;
				if(argc > 7)
					strcpy(hsmGroupID,argv[7]);
			}
			else if(!memcmp(buf, "DES", 3))
			{
				symmetricKeyDB.algorithmID = conSymmetricAlgorithmIDOfDES;
				if(argc > 7)
					strcpy(hsmGroupID,argv[7]);
			}
			else if(!memcmp(buf, "SM1", 3))
			{
				symmetricKeyDB.algorithmID = conSymmetricAlgorithmIDOfSM1;
				if(argc > 7)
					strcpy(hsmGroupID,argv[7]);
			}
			else if(!memcmp(buf, "AES", 3))
			{
				symmetricKeyDB.algorithmID = conSymmetricAlgorithmIDOfAES;
				if(argc > 7)
					strcpy(hsmGroupID,argv[7]);
			}
			else
				strcpy(hsmGroupID,argv[6]);
		}

		// 设置密码机组
		UnionSetHsmGroupIDForHsmSvr(hsmGroupID);

		// 获取保护方式
		memset(lmkProtectMode,0,sizeof(lmkProtectMode));
		if ((ret = UnionReadHsmLmkProtectModeByHsmGroupID(hsmGroupID,lmkProtectMode)) < 0)
		{
			printf("in %s:: UnionReadHsmLmkProtectModeByHsmGroupID hsmGroupID[%s] err!\n",UnionGetApplicationName(),hsmGroupID);
			return(errCodeParameter);
		}

		symmetricKeyDB.keyType = UnionConvertSymmetricKeyKeyType(argv[2]);
		symmetricKeyDB.keyLen = UnionConvertKeyKeyLenToInside(atoi(argv[3]));

		//设置默认值
		UnionSetSymmetrickDefaultValue(&symmetricKeyDB);
		strcpy(symmetricKeyDB.keyValue[0].lmkProtectMode,lmkProtectMode);

		if ((ret = UnionCreateKeyByStartNodeAndEndNode(argv[1],argv[2],atoi(argv[4]),atoi(argv[5]),&symmetricKeyDB)) < 0)
		{
			printf("in %s:: UnionReadHsmLmkProtectModeByHsmGroupID hsmGroupID[%s] err!\n",UnionGetApplicationName(),hsmGroupID);
			return(errCodeParameter);
		}

		//UnionCloseDatabase();
		return(UnionTaskActionBeforeExit());
	}

	// 密钥信息文件
	len = sprintf(keyInfoFileName,"%s",argv[1]);	
	keyInfoFileName[len] = 0;

	// 密钥节点文件
	len = sprintf(keyNodeFileName,"%s",argv[2]);	
	keyNodeFileName[len] = 0;

	memset(hsmGroupID,0,sizeof(hsmGroupID));
	if (argc >= 4)
		strcpy(hsmGroupID,argv[3]);
	else
		strcpy(hsmGroupID,"default");

	// 设置密码机组
	UnionSetHsmGroupIDForHsmSvr(hsmGroupID);

	// 获取保护方式
	memset(lmkProtectMode,0,sizeof(lmkProtectMode));
	if ((ret = UnionReadHsmLmkProtectModeByHsmGroupID(hsmGroupID,lmkProtectMode)) < 0)
	{
		printf("in %s:: UnionReadHsmLmkProtectModeByHsmGroupID hsmGroupID[%s] err!\n",UnionGetApplicationName(),hsmGroupID);
		return(errCodeParameter);
	}


	if ((fpKeyInfo = fopen(keyInfoFileName,"r")) == NULL)
	{
		printf("in %s:: fopen[%s] is failed!\n",UnionGetApplicationName(),keyInfoFileName);
		return(errCodeParameter);
	}

	while(!feof(fpKeyInfo))
	{
		memset(&symmetricKeyDB,0,sizeof(symmetricKeyDB));
		isGenerateKeyValue = 1;

		// 读取密钥信息
		if ((ret = UnionReadOneLineFromTxtStr(fpKeyInfo,oriBuf,sizeof(oriBuf))) <= 0)	
			continue;

		oriBuf[ret] = 0;
		UnionFilterAllBlankChars(oriBuf,strlen(oriBuf),desBuf,sizeof(desBuf));

		// 读取密钥名称
		if ((ret = UnionReadFiledFromStr(desBuf,';',1,symmetricKeyDB.keyName)) < 0)
		{
			printf("in %s:: UnionReadFiledFromStr lineData[%s][%d] is failed!\n",UnionGetApplicationName(),desBuf,1);
			goto errExit;
		}

		// 读取密钥类型
		memset(keyType,0,sizeof(keyType));
		if ((ret = UnionReadFiledFromStr(desBuf,';',2,keyType)) < 0)
		{
			printf("in %s:: UnionReadFiledFromStr lineData[%s][%d] is failed!\n",UnionGetApplicationName(),desBuf,2);
			goto errExit;
		}
		symmetricKeyDB.keyType = UnionConvertSymmetricKeyKeyType(keyType);

		// 读取密钥长度
		if ((ret = UnionReadFiledFromStr(desBuf,';',3,tmpBuf)) < 0)
		{
			printf("in %s:: UnionReadFiledFromStr lineData[%s][%d] is failed!\n",UnionGetApplicationName(),desBuf,3);
			goto errExit;
		}
		tmpBuf[ret] = 0;
		symmetricKeyDB.keyLen = UnionConvertKeyKeyLenToInside(atoi(tmpBuf));

		// modified 20160713 支持多算法密钥
		/*
		// 读取密钥值
		if ((ret = UnionReadFiledFromStr(desBuf,';',4,symmetricKeyDB.keyValue[0].keyValue)) > 0)
		{
			// 读取校验值
			if ((ret = UnionReadFiledFromStr(desBuf,';',5,symmetricKeyDB.checkValue)) < 0)
			{
				printf("in %s:: UnionReadFiledFromStr lineData[%s][%d] is failed!\n",UnionGetApplicationName(),desBuf,5);
				goto errExit;
			}
			isGenerateKeyValue = 0;
		}
		*/
		symmetricKeyDB.algorithmID = conSymmetricAlgorithmIDOfDES;
		if ((ret = UnionReadFiledFromStr(desBuf,';',4,buf)) > 0)
		{
			/*
			conSymmetricAlgorithmIDOfDES,
        		conSymmetricAlgorithmIDOfSM4,
        		conSymmetricAlgorithmIDOfSM1,
        		conSymmetricAlgorithmIDOfAES,
			*/
			buf[ret] = 0;
			if(!memcmp(buf, "SM4", 3))
			{
				symmetricKeyDB.algorithmID = conSymmetricAlgorithmIDOfSM4;
				// 读取密钥值
				if ((ret = UnionReadFiledFromStr(desBuf,';',5,symmetricKeyDB.keyValue[0].keyValue)) > 0)
				{
					// 读取校验值
					if ((ret = UnionReadFiledFromStr(desBuf,';',6,symmetricKeyDB.checkValue)) < 0)
					{
						printf("in %s:: UnionReadFiledFromStr lineData[%s][%d] is failed!\n",UnionGetApplicationName(),desBuf,6);
						goto errExit;
					}
					isGenerateKeyValue = 0;
				}
			}
			else if(!memcmp(buf, "DES", 3))
			{
				symmetricKeyDB.algorithmID = conSymmetricAlgorithmIDOfDES;
				// 读取密钥值
				if ((ret = UnionReadFiledFromStr(desBuf,';',5,symmetricKeyDB.keyValue[0].keyValue)) > 0)
				{
					// 读取校验值
					if ((ret = UnionReadFiledFromStr(desBuf,';',6,symmetricKeyDB.checkValue)) < 0)
					{
						printf("in %s:: UnionReadFiledFromStr lineData[%s][%d] is failed!\n",UnionGetApplicationName(),desBuf,6);
						goto errExit;
					}
					isGenerateKeyValue = 0;
				}
			}
			else if(!memcmp(buf, "SM1", 3))
			{
				symmetricKeyDB.algorithmID = conSymmetricAlgorithmIDOfSM1;
				// 读取密钥值
				if ((ret = UnionReadFiledFromStr(desBuf,';',5,symmetricKeyDB.keyValue[0].keyValue)) > 0)
				{
					// 读取校验值
					if ((ret = UnionReadFiledFromStr(desBuf,';',6,symmetricKeyDB.checkValue)) < 0)
					{
						printf("in %s:: UnionReadFiledFromStr lineData[%s][%d] is failed!\n",UnionGetApplicationName(),desBuf,6);
						goto errExit;
					}
					isGenerateKeyValue = 0;
				}
			}
			else if(!memcmp(buf, "AES", 3))
			{
				symmetricKeyDB.algorithmID = conSymmetricAlgorithmIDOfAES;
				// 读取密钥值
				if ((ret = UnionReadFiledFromStr(desBuf,';',5,symmetricKeyDB.keyValue[0].keyValue)) > 0)
				{
					// 读取校验值
					if ((ret = UnionReadFiledFromStr(desBuf,';',6,symmetricKeyDB.checkValue)) < 0)
					{
						printf("in %s:: UnionReadFiledFromStr lineData[%s][%d] is failed!\n",UnionGetApplicationName(),desBuf,6);
						goto errExit;
					}
					isGenerateKeyValue = 0;
				}
			}
			else // 是取密钥值
			{
				strcpy(symmetricKeyDB.keyValue[0].keyValue, buf);
				// 读取校验值
				if ((ret = UnionReadFiledFromStr(desBuf,';',5,symmetricKeyDB.checkValue)) < 0)
				{
					printf("in %s:: UnionReadFiledFromStr lineData[%s][%d] is failed!\n",UnionGetApplicationName(),desBuf,5);
					goto errExit;
				}
				isGenerateKeyValue = 0;
			}
		}
		// end of 20160713

		//设置默认值
		UnionSetSymmetrickDefaultValue(&symmetricKeyDB);
		strcpy(symmetricKeyDB.keyValue[0].lmkProtectMode,lmkProtectMode);

		if ((ret = UnionCreateKeyByKeyNode(keyNodeFileName,isGenerateKeyValue,&symmetricKeyDB)) < 0)
		{
			printf("in %s:: UnionCreateKeyByKeyNode is failed! keyNodeFileName[%s]\n",UnionGetApplicationName(),keyNodeFileName);
			continue;
		}
	}
errExit:
	if (fpKeyInfo != NULL)
		fclose(fpKeyInfo);
	fpKeyInfo = NULL;
	//UnionCloseDatabase();
	return(UnionTaskActionBeforeExit());
}
