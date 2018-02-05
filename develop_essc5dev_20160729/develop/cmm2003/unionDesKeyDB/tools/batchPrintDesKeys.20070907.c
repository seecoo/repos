// Author:	Wolfgang Wang
// Date:	2005/06/07

#define _UnionDesKeyDB_2_x_
#define _UnionLogMDL_3_x_

#include <stdio.h>
#include <string.h>

#ifndef _UnionDesKey_3_x_
#define _UnionDesKey_3_x_
#endif
#include "unionDesKeyDB.h"


#include "unionErrCode.h"
#include "sjl06.h"
#ifndef _RacalCmdForNewRacal_
#define _RacalCmdForNewRacal_
#endif
#include "sjl06Cmd.h"
#include "UnionStr.h"
#include "UnionSocket.h"
#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif
#include "UnionTask.h"
#include "unionCommand.h"

#ifndef _UnionEnv_3_x_
#define _UnionEnv_3_x_
#endif
#include "UnionEnv.h"
#include "UnionLog.h"

TUnionDesKey		gunionDefaultDesKey;
int			gunionComponentNum = 2;
TUnionSJL06		gunionSJL06;
char			gunionParam[20][80];
int			gunionParamNum = 0;

PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	UnionDisconnectDesKeyDB();
	exit(0);	//return(exit(0));
}

int UnionHelp()
{
	printf("Usage:: %s keyDefFile dataFile [printFormatFile]\n",UnionGetApplicationName());
	
	return(0);
}
	
int UnionInitDefinition(char *defaultDefinitionFileName)
{
	FILE	*fp;
	int	ret;
	char	tmpBuf[8192+1];
	int	lineLen;
	int	lines = 0;
	int	isDefineLine = 0;
	char	type[40+1];
	int	index;
	char	varName[40+1];
	int	tmpInt;
		
	memset(&(gunionDefaultDesKey),0,sizeof(gunionDefaultDesKey));
	gunionDefaultDesKey.length = con128BitsDesKey;
	gunionDefaultDesKey.type = conZMK;
	
	memset(&gunionSJL06,0,sizeof(gunionSJL06));
	
	if (defaultDefinitionFileName == NULL)
		return(0);
	
	if ((fp = fopen(defaultDefinitionFileName,"r")) == NULL)
	{
		UnionUserErrLog("in UnionInitDefinition:: fopen [%s]\n",defaultDefinitionFileName);
		return(errCodeUseOSErrCode);
	}
	
	for (index = 0; index < 20; index++)
	{
		memset(gunionParam[index],0,sizeof(gunionParam[index]));
	}
	while(!feof(fp))
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		// 从文件中读取一个定义行
		if ((lineLen = UnionReadOneFileLine(fp,tmpBuf)) <= 0)
			continue;
		lines++;
		if (UnionIsUnixShellRemarkLine(tmpBuf))	// 注释行
			continue;
		// 读密码机IP地址
		if ((ret = UnionReadFldFromRecStr(tmpBuf,"hsmIPAddr","string",gunionSJL06.staticAttr.ipAddr,sizeof(gunionSJL06.staticAttr.ipAddr)-1)) > 0)
		{
			if ((ret = UnionSelectSJL06Rec(&gunionSJL06)) < 0)
			{
				printf("UnionSelectSJL06Rec %s error! ret = [%d]\n",gunionSJL06.staticAttr.ipAddr,ret);
				return(ret);
			}
			gunionSJL06.dynamicAttr.status = conOnlineSJL06;
			continue;
		}	
		if ((ret = UnionReadFldFromRecStr(tmpBuf,"fullName","string",gunionDefaultDesKey.fullName,sizeof(gunionDefaultDesKey.fullName)-1)) > 0)
			continue;
		// 读密钥全名
		if ((ret = UnionReadFldFromRecStr(tmpBuf,"fullName","string",gunionDefaultDesKey.fullName,sizeof(gunionDefaultDesKey.fullName)-1)) > 0)
			continue;
		// 读密钥类型
		if ((ret = UnionReadFldFromRecStr(tmpBuf,"type","string",type,sizeof(type)-1)) > 0)
		{
			gunionDefaultDesKey.type = UnionConvertDesKeyType(type);
			continue;
		}
		// 读密钥强度
		if ((ret = UnionReadFldFromRecStr(tmpBuf,"length","int",(unsigned char *)(&tmpInt),sizeof(tmpInt))) > 0)
		{
			gunionDefaultDesKey.length = UnionConvertDesKeyLength(tmpInt);
			continue;
		}
		// 读密钥成分数量
		if ((ret = UnionReadFldFromRecStr(tmpBuf,"componentNum","int",(unsigned char *)(&gunionComponentNum),sizeof(gunionComponentNum))) > 0)
			continue;
		// 读打印参数
		for (index = 0; index < 20; index++)
		{
			sprintf(varName,"par%02d",index);
			if ((ret = UnionReadFldFromRecStr(tmpBuf,varName,"string",gunionParam[index],sizeof(gunionParam[index]))) > 0)
			{
				gunionParamNum++;
				break;
			}
		}
	}
	fclose(fp);
	return(0);
}

int main(int argc,char *argv[])
{
	int		ret;
	FILE		*fp;
	int		index;
	char		tmpBuf[1024+1];
	char		idOfApp[40+1];
	char		suffix[40+1];
	char		owner[40+1];
	long		realRecNum = 0,successPrintNum = 0,successUpdateDesKeyDBNum = 0;
	TUnionDesKey	desKey;
	int		keyExistsInKeyDB = 0;
	
	UnionSetApplicationName(argv[0]);
	
	if (argc < 3)
	{
		UnionHelp();
		return(errCodeParameter);
	}

	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	// 设置打印格式
	if (argc >= 4)
	{
		if ((ret = UnionSetKeyPrintFormat(argv[3])) < 0)
		{
			printf("UnionSetKeyPrintFormat from [%s] failure! ret = [%d]\n",argv[3],ret);
			return(UnionTaskActionBeforeExit());
		}
		else
			printf("UnionSetKeyPrintFormat from [%s] ok!\n",argv[3]);
	}
	else
	{
		if ((ret = UnionSetKeyPrintFormat(NULL)) < 0)
		{
			printf("UnionSetKeyPrintFormat failure! ret = [%d]\n",ret);
			return(UnionTaskActionBeforeExit());
		}
		else
			printf("UnionSetKeyPrintFormat ok!\n");
	}
		
	// 初始化密钥定义
	if ((ret = UnionInitDefinition(argv[1])) < 0)
	{
		printf("UnionInitDefinition failure! ret = [%d]\n",ret);
		return(UnionTaskActionBeforeExit());
	}
	if (ret = 0)
	{
		printf("no printing definition!\n");
		return(UnionTaskActionBeforeExit());
	}

	// 根据数据文件插入密钥
	if ((fp = fopen(argv[2],"r")) == NULL)
	{
		printf("fopen [%s] error!\n",argv[2]);
		return(UnionTaskActionBeforeExit());
	}
	while (!feof(fp))
	{
		// 从文件中读取一条数据
		memset(tmpBuf,0,sizeof(tmpBuf));
		UnionReadOneFileLine(fp,tmpBuf);
		if (UnionIsUnixShellRemarkLine(tmpBuf))
			continue;
		realRecNum++;
		// 组装要打印的密钥名称
		memset(idOfApp,0,sizeof(idOfApp));	
		memset(suffix,0,sizeof(suffix));	
		memset(owner,0,sizeof(owner));
		if ((ret = UnionAnalysisDesKeyFullName(gunionDefaultDesKey.fullName,idOfApp,owner,suffix)) < 0)
			continue;
		if (strlen(idOfApp) + strlen(suffix) + strlen(tmpBuf) + 2 >= sizeof(gunionDefaultDesKey.fullName))
		{
			printf("keyName = [%s.%s.%s] too long!\n",idOfApp,tmpBuf,suffix);
			continue;
		}
		sprintf(gunionDefaultDesKey.fullName,"%s.%s.%s",idOfApp,tmpBuf,suffix);
		// 从密钥库中读取密钥
		memcpy(&desKey,&gunionDefaultDesKey,sizeof(desKey));
		if ((ret = UnionReadDesKeyFromKeyDBAnyway(&desKey)) < 0)
		{
			printf("UnionReadDesKeyFromKeyDBAnyway [%s] failure! ret = [%d]\n",desKey.fullName,ret);
			keyExistsInKeyDB = 0;
		}
		else
			keyExistsInKeyDB = 1;
		// 打印密钥
		if ((ret = UnionGenerateKeyComponent(&gunionSJL06,&desKey,gunionComponentNum,gunionParamNum,gunionParam)) < 0)
		{
			printf("UnionGenerateKeyComponent [%s] failure! ret = [%d]\n",desKey.fullName,ret);
			continue;
		}
		else
		{
			printf("print key [%s] ok!\n",desKey.fullName);
			successPrintNum++;
		}
		// 更新密钥值
		if (!keyExistsInKeyDB)
			continue;
		if ((ret = UnionUpdateDesKeyValueInKeyDB(desKey.fullName,desKey.value,desKey.checkValue)) < 0)
			printf("UnionUpdateDesKeyValueInKeyDB [%s] failure! ret = [%d]\n",desKey.fullName,ret);
		else
		{
			printf("update [%s] ok!\n",desKey.fullName);
			successUpdateDesKeyDBNum++;
		}
	}
	printf("realRecNum = [%04ld]\nsuccessPrintNum = [%04ld]\nsuccessUpdateDesKeyDBNum = [%04ld]\n",
		realRecNum,successPrintNum,successUpdateDesKeyDBNum);
	fclose(fp);
	return(UnionTaskActionBeforeExit());
}
