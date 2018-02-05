// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2003/09/28
// Version:	1.0

#define _UnionTask_3_x_

#include <stdio.h>
#include <string.h>

#include "sjl06.h"
#include "UnionTask.h"
#include "sjl06Cmd.h"
#include "unionCommand.h"
#include "unionVersion.h"
#include "UnionLog.h"


PUnionTaskInstance	ptaskInstance = NULL;
//char			pgPrintFormat[] = ">L>L>L>L>L>030^1>L>030^2>L>030^3>L>023 第^0个成分>L>023 ^P>L>023 ^4>L>023 ^5>L>023 ^6>L>023 ^7>L>023 ^8>L>023 ^9>L";
char			pgPrintFormat[] = ">L>V>V>V>V>029密件 CONFIDENTIAL>064^5>L>001收件人单位姓名：               联系地址：>L>001^0>032^1>L>V>001系统名称：^2>032^6>L>V>061^P>L>V>001分行编号：^3>032^4>L>V>V>V>003^7>L>003^8>V>V>V>L>L";
char			pgPrintFormatForCheckValue[] = ">L>V>V>V>V>V>V>V>003^0>L>003^1>L>003^2>L>003^3>L>003^4>L>003^5>L>003^6>L>003^7>L>003^8>L>003^9>L>L";

int UnionTaskActionBeforeExit()
{
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

int main(int argc,char *argv[])
{
	int	ret;
	char	ipAddr[80];
	char	type[10];
	int	length;
	char	format[10][80];
	int	componentNum;
	int	formatNum,formatIndex;
	char	*p;
	char	tmpBuf[100];
	
	UnionSetApplicationName(argv[0]);
	//if (argc < 2)
	//	return(UnionHelp());
		
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"genKeyComponent")) == NULL)
	{
		printf("in genKeyComponent::UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());

	memset(ipAddr,0,sizeof(ipAddr));
	if (argc >= 2)
		strcpy(ipAddr,argv[1]);
inputIPAddr:
	if (!UnionIsValidIPAddrStr(ipAddr))
	{
		if (strlen(ipAddr) != 0)
			printf("Waringing!! Invalid IPAddr [%s]!\n",ipAddr);
		//if (UnionIsQuit(p = UnionInput("Input Hsm IPAddr::")))
		if (UnionIsQuit(p = UnionInput("请输入加密机的IP地址：")))
			return(UnionTaskActionBeforeExit());
		memset(ipAddr,0,sizeof(ipAddr));
		strcpy(ipAddr,p);
		goto inputIPAddr;
	}
	
	memset(type,0,sizeof(type));
	if (argc >= 3)
		strcpy(type,argv[2]);
inputType:
	if (UnionConvertDesKeyType(type) < 0)
	{
		if (strlen(type) != 0)
			printf("Waringing!! Invalid key type [%s]!\n",type);
		//if (UnionIsQuit(p = UnionInput("Input Key Type::")))
		if (UnionIsQuit(p = UnionInput("请输入密钥的类型：")))
			return(UnionTaskActionBeforeExit());
		UnionToUpper(p,type);
		//strcpy(type,p);
		goto inputType;
	}
	
	length = -100;
	if (argc >= 4)
		length = atoi(argv[3]);
inputLength:
	if (UnionConvertDesKeyLength(length) < 0)
	{
		if (length != -100)
			printf("Waringing!! Invalid key length [%d]!\n",length);
		//if (UnionIsQuit(p = UnionInput("Input Key Length::")))
		if (UnionIsQuit(p = UnionInput("请输入密钥的长度(16/32/48)：")))
			return(UnionTaskActionBeforeExit());
		length = atoi(p);
		goto inputLength;
	}
	
	componentNum = -100;
	if (argc >= 5)
		componentNum = atoi(argv[4]);
inputComponentNum:
	if (componentNum <= 0 || componentNum > 9)
	{
		if (componentNum != -100)
			printf("Waringing!! Invalid key component num [%d]!\n",componentNum);
		//if (UnionIsQuit(p = UnionInput("Input Key Component Num::")))
		if (UnionIsQuit(p = UnionInput("请输入密钥成分的数量(1-9)：")))
			return(UnionTaskActionBeforeExit());
		componentNum = atoi(p);
		goto inputComponentNum;
	}

	formatNum = argc - 5;
	for (formatIndex = 0; formatIndex < formatNum && formatIndex < 10; formatIndex++)
	{
		memset(format[formatIndex],0,sizeof(format[formatIndex]));
		strcpy(format[formatIndex],argv[5+formatIndex]);
	}
	/* Mary delete, 2004-3-23
	if (formatNum <= 0)
	{
		for (formatIndex = 0,formatNum = 0;formatNum < 10;formatIndex++)
		{
			p = UnionInput("Input Key Description%02d (quit to finish input)::\n",formatIndex);
			memset(format[formatIndex],0,sizeof(format[formatIndex]));
			strcpy(format[formatIndex],p);
			if (UnionIsQuit(p))
				break;
			formatNum++;
		}
	}
	*/
	// Mary add begin, 2003-3-23
	if (formatNum <= 0)
	{
		for (formatIndex = 0,formatNum = 0;formatNum < 10;formatIndex++)
		{
			memset(tmpBuf,0,sizeof(tmpBuf));
			switch(formatIndex)
			{
			case 0:
				strcpy(tmpBuf,"请输入：收件人单位姓名(小于15个字符)\n");
				break;
			case 1:
				strcpy(tmpBuf,"请输入：联系地址(小于25个字符)\n");
				break;
			case 2:
				strcpy(tmpBuf,"请输入：系统名称(小于15个字符)\n");
				break;
			case 3:
				strcpy(tmpBuf,"请输入：分行编号(小于15个字符)\n");
				break;
			default:
				sprintf(tmpBuf,"Input Key Description%02d (quit to finish input)::\n",formatIndex);
				break;
			}
			if (formatIndex > 3)
				break;
			//p = UnionInput("Input Key Description%02d (quit to finish input)::\n",formatIndex);
			p = UnionInput(tmpBuf);
			memset(format[formatIndex],0,sizeof(format[formatIndex]));
			strcpy(format[formatIndex],p);
			if (UnionIsQuit(p))
				break;
			formatNum++;
		}
	}
	// Mary add end, 2003-3-23
	return(UnionGenerateKeyComponent(ipAddr,type,length,componentNum,formatNum,format));
}

int UnionHelp()
{
	//printf("Usage:: %s hsmIPAddr keyName keyLength number application owner keyName\n",UnionGetApplicationName());
	printf("Usage:: %s\n",UnionGetApplicationName());
	return(0);
}
	
int UnionGenerateKeyComponent(char *ipAddr,char *type,int length,int componentNum,int argc,char argv[][80])
{
	char		resBuf[100];
	int		len;
	int		ret;
	TUnionSJL06	sjl06;
	int		i,j;
	char		fld[10][80];
	char		tmpBuf[100],keyByLMK[10][50];
	
	printf("Hsm IPAddr         [%s]\n",ipAddr);
	printf("Key Type           [%s]\n",type);
	printf("Key Length         [%d]\n",length);
	printf("Component Num      [%d]\n",componentNum);
	printf("Description Num    [%d]\n",argc);
	for (i = 0; i < argc; i++)
		printf("Description %02d     [%s]\n",i,argv[i]);
	if (!UnionConfirm("Are you sure of generate key components?"))
		return(UnionTaskActionBeforeExit());

	memset(&sjl06,0,sizeof(sjl06));
	strcpy(sjl06.staticAttr.ipAddr,ipAddr);
	if ((ret = UnionSelectSJL06Rec(&sjl06)) < 0)
	{
		printf("UnionSelectSJL06Rec [%s] Error\n",ipAddr);
		return(UnionTaskActionBeforeExit());
	}
	sjl06.dynamicAttr.status = conOnlineSJL06;
	
	memset(resBuf,0,sizeof(resBuf));
	if ((ret = RacalCmdPA(-1,&sjl06,pgPrintFormat,resBuf)) < 0)
	{
		printf("RacalCmdPA [%s] Error\n",ipAddr);
		return(UnionTaskActionBeforeExit());
	}
	
	if (strncmp(resBuf,"00",2) == 0)
		printf("Set Print Format for [%s] OK!\n",ipAddr);
	else
	{
		printf("Set Print Format for [%s] Failure! errCode = [%s]!\n",ipAddr,resBuf);
		return(UnionTaskActionBeforeExit());
	}
	
	for (i = 0; (i < argc) && (i < 10); i++)
	{
		memset(fld[i],0,sizeof(fld[i]));
		strcpy(fld[i],argv[i]);
	}

	// 增加以下打印要素
	// 日期
	memset(fld[5],0,sizeof(fld[5]));
	UnionGetFullSystemDate(fld[5]);
	// 密钥类型
	memset(fld[6],0,sizeof(fld[6]));
	memset(tmpBuf,0,sizeof(tmpBuf));
	UnionToUpper(type,tmpBuf);
	sprintf(fld[6],"密钥类型：%s",tmpBuf);
	// 注释1
	memset(fld[7],0,sizeof(fld[7]));
	strcpy(fld[7],"注意：如果你拾到本信封或发现此信封曾被拆阅，请立即通知本行");
	// 注释2
	memset(fld[8],0,sizeof(fld[8]));
	strcpy(fld[8],"收到本密件后请按要求保管，作好登记，并报知发件人");

	UnionNullLog("\n");
	for (j=0;j<argc+5;j++)
	{
		if (j == 4)
			continue;
		UnionNullLog("parameter[%d] = [%s]\n",j,fld[j]);
	}
	UnionNullLog("field number = [%d]\n",argc+5);

	for (i = 0; i < componentNum; i++)
	{
		// 分量序号
		memset(fld[4],0,sizeof(fld[4]));
		sprintf(fld[4],"分量序号：%02d",i+1);
		UnionNullLog("\nparameter[%d] = [%s]\n",4,fld[4]);
		
		memset(resBuf,0,sizeof(resBuf));
		if ((ret = RacalCmdA2(-1,&sjl06,UnionConvertDesKeyType(type),UnionConvertDesKeyLength(length),argc+5,fld,resBuf,resBuf+60)) < 0)
		{
			printf("RacalCmdA2 [%s] Error\n",ipAddr);
			return(UnionTaskActionBeforeExit());
		}
		if (strncmp(resBuf+60,"00",2) == 0)
			printf("Generate [%0d]th OK! key encrypted by LMK=[%s]\n",i+1,resBuf);
		else
		{
			printf("Generate [%0d]th Error! errCode = [%s]\n",i+1,resBuf+60);
			return(UnionTaskActionBeforeExit());
		}
		memset(keyByLMK[i],0,sizeof(keyByLMK[i]));
		memcpy(keyByLMK[i],resBuf,ret);
	}
	printf("Generate key OK!\n");
	
	ret=PrintKeyCheckValue(&sjl06,keyByLMK,componentNum,UnionConvertDesKeyType(type),UnionConvertDesKeyLength(length));
	if (ret < 0)
	{
		printf("PrintKeyCheckValue fail! return=[%d]\n",ret);
		return(UnionTaskActionBeforeExit());
	}
	
	return(UnionTaskActionBeforeExit());
}

int PrintKeyCheckValue(PUnionSJL06 pSJL06,char partKey[][50],int partKeyNum,\
		TUnionDesKeyType keyType,TUnionDesKeyLength keyLength)
{
	int	ret,i,fldNum;
	char	checkValue[10][20],fld[10][80];
	char	errCode[10],keyByLMK[50],tmpBuf[200];
	
	// 为每个密钥成分生成校验值
	for (i=0;i<partKeyNum;i++)
	{
		memset(checkValue[i],0,sizeof(checkValue[i]));
		ret=RacalCmdBU(-1,pSJL06,keyType,keyLength,partKey[i],checkValue[i],errCode);
		if (ret < 0)
		{
			printf("RacalCmdBU Error! return=[%d]\n",ret);
			return(UnionTaskActionBeforeExit());
		}
		if (strncmp(errCode,"00",2) == 0)
			printf("Generate [%0d]th key check value OK! check value=[%s]\n",i+1,checkValue[i]);
		else
		{
			printf("Generate [%0d]th key check value Error! errCode = [%s]\n",i+1,errCode);
			return(UnionTaskActionBeforeExit());
		}
	}
	UnionNullLog("\n");
	for (i=0;i<partKeyNum;i++)
	{
		UnionNullLog("[%d] key component=[%s] check value=[%s]\n",i,partKey[i],checkValue[i]);
	}
	UnionNullLog("partKeyNum = [%d]\n",partKeyNum);
	
	// 将各个密钥成分合成一把密钥，并生成合成密钥的校验值
	memset(checkValue[9],0,sizeof(checkValue[9]));
	memset(keyByLMK,0,sizeof(keyByLMK));
	if (partKeyNum == 1)
	{
		strcpy(checkValue[9],checkValue[0]);
	}
	else
	{
		ret=RacalCmdA4(-1,pSJL06,keyType,keyLength,partKeyNum,partKey,keyByLMK,checkValue[9],errCode);
		if (ret < 0)
		{
			printf("RacalCmdA4 Error! return=[%d]\n",ret);
			return(UnionTaskActionBeforeExit());
		}
		if (strncmp(errCode,"00",2) == 0)
			printf("Generate total key check value OK!\n");
		else
		{
			printf("Generate total key check value Error! errCode = [%s]\n",errCode);
			return(UnionTaskActionBeforeExit());
		}
	}
	printf("total key by LMK=[%s] check value=[%s]\n",keyByLMK,checkValue[9]);
	UnionNullLog("total key=[%s] check value=[%s]\n\n",keyByLMK,checkValue[9]);
	
	// 设置打印密钥校验值的格式
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = RacalCmdPA(-1,pSJL06,pgPrintFormatForCheckValue,errCode)) < 0)
	{
		printf("RacalCmdPA Error! errCode=[%s]\n",errCode);
		return(UnionTaskActionBeforeExit());
	}
	
	if (strncmp(errCode,"00",2) == 0)
		printf("Set Print Format for print key check value OK!\n");
	else
	{
		printf("Set Print Format for print key check value Failure! errCode = [%s]!\n",errCode);
		return(UnionTaskActionBeforeExit());
	}
	
	// 打印各个密钥成分的校验值以及合成密钥的校验值
	fldNum = partKeyNum + 1;
	for (i=0;i<partKeyNum;i++)
	{
		memset(fld[i],0,sizeof(fld[i]));
		sprintf(fld[i],"第%d个密钥成分的校验值: %s",i+1,checkValue[i]);
	}
	memset(fld[partKeyNum],0,sizeof(fld[partKeyNum]));
	sprintf(fld[partKeyNum],"总的密钥校验值: %s",checkValue[9]);

	UnionNullLog("\n");
	for (i=0;i<fldNum;i++)
	{
		UnionNullLog("field [%d] = [%s]\n",i,fld[i]);
	}
	UnionNullLog("fldNum = [%d]\n",fldNum);
	
	if ((ret = RacalCmdA2(-1,pSJL06,keyType,keyLength,fldNum,fld,tmpBuf,errCode)) < 0)
	{
		printf("RacalCmdA2 Error! errCode=[%s]\n",errCode);
		return(UnionTaskActionBeforeExit());
	}
	if (strncmp(errCode,"00",2) == 0)
		printf("Print key check value OK!\n");
	else
	{
		printf("Print key check value failure! errCode = [%s]\n",errCode);
		return(UnionTaskActionBeforeExit());
	}

	return(0);
}
