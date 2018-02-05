//add by 张树斌 20140718
#include <time.h>
#include <stdio.h>

#include "UnionLog.h"
#include "unionRealBaseDB.h"

int UnionTaskActionBeforeExit()
{
	exit(0);
}

static int realInsert(char *flag,int min,int max)
{
	int	len = 0, ret;
	char	sql[10240];
	int	successNum = 0;
	int	failNum = 0;
	char	keyName[128];
	int	i = 0;

	for (i = min; i <= max;i++)
	{
		len = 0;
		//for (j = 0; j < 20; j++,i++)
		{
			snprintf(keyName,sizeof(keyName),"HOST.%06d.zmk",i);
			len += snprintf(sql+len,sizeof(sql)-len,"insert into desKeyDB (unfullName) values('%s')",keyName);
			/*if (strcasecmp(flag,"zmk") == 0)
			{
				snprintf(keyName,sizeof(keyName),"HOST.1%06d.zmk",i);
				len += snprintf(sql+len,sizeof(sql)-len,"insert into symmetricKeyDB(keyName,keyGroup,algorithmID,keyType,keyLen,inputFlag,outputFlag,effectiveDays,status,oldVersionKeyIsUsed,checkValue,creatorType,creator,createTime)values('%s','TEST',0,2,128,1,1,0,1,1,'A4F0568043DC9205',1,'TEST','20150428120000');", keyName);
				len += snprintf(sql + len,sizeof(sql) - len,"insert into symmetricKeyValue(keyName,lmkProtectMode,keyValue) values('%s','01','3544D6D774388A10098A552EFB5CF5FD');", keyName);
			}
			else
			{
				snprintf(keyName,sizeof(keyName),"HOST.1%06d.zak",i);
				len += snprintf(sql+len,sizeof(sql)-len,"insert into symmetricKeyDB(keyName,keyGroup,algorithmID,keyType,keyLen,inputFlag,outputFlag,effectiveDays,status,oldVersionKeyIsUsed,creatorType,creator,createTime)values('%s','TEST',0,1,128,1,1,0,1,1,1,'TEST','20150428120000');", keyName);
			}*/
			
			//snprintf(keyName,sizeof(keyName),"HOST.1%06d.zmk",i);
			//len += snprintf(sql+len,sizeof(sql)-len,"update symmetricKeyDB set activeDate ='20150428' where keyName='%s';", keyName);
			//snprintf(keyName,sizeof(keyName),"HOST.1%06d.zak",i);
			//len += snprintf(sql+len,sizeof(sql)-len,"update symmetricKeyDB set activeDate ='20150428' where keyName='%s';", keyName);
		}

		if ((ret = UnionExecRealDBSql2(0,sql)) < 0)
		{
			UnionUserErrLog("in realInsert:: UnionExecRealDBSql2 ret = [%d] sql = [%s]!\n",ret,sql);
			failNum++;
			continue;
		}
		successNum++;
	}
	printf("密钥类型[%s]  总数[%d]   成功数[%d]  失败数[%d]!\n",flag,successNum+failNum,successNum,failNum);
	return(1);
}
int main(int argc,char *argv[])
{
	if (argc < 4)	
	{
		printf("Uages:: 程序  密钥类型  开始数目 结束数目");
		return 0;
	}
	realInsert(argv[1],atoi(argv[2]),atoi(argv[3]));
	return 0;
}
