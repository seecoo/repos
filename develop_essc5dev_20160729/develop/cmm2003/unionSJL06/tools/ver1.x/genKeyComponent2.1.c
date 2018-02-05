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
//char			pgPrintFormat[] = ">L>V>V>V>V>029密件 CONFIDENTIAL>064^5>L>001收件人单位姓名：               联系地址：>L>001^0>032^1>L>V>001系统名称：^2>032^6>L>V>061^P>L>V>001分行编号：^3>032^4>L>V>V>V>003^7>L>003^8>V>V>V>L>L";
char			pgPrintFormat[] = ">L>V>V>V>V>029密件 CONFIDENTIAL>L>001收件人单位姓名电话：>021^0>L>001收件人地址邮编：>017^1>L>001寄件人单位姓名电话：>021^2>L>001寄件人地址邮编：>017^3>L>001密钥名称：>011^4>045密钥类型：>055^5>V>V>001密钥明文：>011^P>V>V>001密钥编号：>011^6>025分量序号：>035^7>045密钥生成日期：>060^8>V>V>V>001^9>V>007收件人收到本密件后请按要求保管，作好登记，并报知寄件人。>V>V>V>V>L";
char			pgPrintFormatForCheckValue[] = ">L>V>V>V>V>V>V>V>003^0>L>003^1>L>003^2>L>003^3>L>003^4>L>003^5>L>003^6>L>003^7>L>003^8>L>003^9>L>L";

int UnionTaskActionBeforeExit()
{
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

int main(int argc,char *argv[])
{
	int	ret,iInput;
	char	ipAddr[80],caKeyName[50],caKeyNo[20];
	char	type[10];
	int	length;
	char	format[10][80];
	int	componentNum;
	int	formatNum,formatIndex;
	char	*p;
	char	tmpBuf[100];
	
	if (argc > 1)
		return(UnionHelp());
	
	UnionSetApplicationName(argv[0]);

	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"genKeyComponent")) == NULL)
	{
		printf("in genKeyComponent::UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());

	memset(ipAddr,0,sizeof(ipAddr));
	//if (argc >= 2)
		//strcpy(ipAddr,argv[1]);
inputIPAddr:
	if (!UnionIsValidIPAddrStr(ipAddr))
	{
		if (strlen(ipAddr) != 0)
			printf("Waringing!! Invalid IPAddr [%s]!\n",ipAddr);
		if (UnionIsQuit(p = UnionInput("请输入加密机的IP地址：")))
			return(UnionTaskActionBeforeExit());
		memset(ipAddr,0,sizeof(ipAddr));
		strcpy(ipAddr,p);
		goto inputIPAddr;
	}
	
	memset(tmpBuf,0,sizeof(tmpBuf));
inputName:
	if (strlen(tmpBuf) == 0 || strlen(tmpBuf) > 40)
	{
		p = UnionInput("请输入密钥的名称(小于40个字符)：");
		strcpy(tmpBuf,p);
		if (UnionIsQuit(p))
			return(UnionTaskActionBeforeExit());
		goto inputName;
	}
	memset(caKeyName,0,sizeof(caKeyName));
	strcpy(caKeyName,tmpBuf);

	memset(tmpBuf,0,sizeof(tmpBuf));
inputKeyNo:
	if (strlen(tmpBuf) == 0 || strlen(tmpBuf) > 10)
	{
		p = UnionInput("请输入密钥的编号(小于10个字符)：");
		strcpy(tmpBuf,p);
		if (UnionIsQuit(p))
			return(UnionTaskActionBeforeExit());
		goto inputKeyNo;
	}
	memset(caKeyNo,0,sizeof(caKeyNo));
	strcpy(caKeyNo,tmpBuf);

	memset(type,0,sizeof(type));
	//if (argc >= 3)
		//strcpy(type,argv[2]);
inputType:
	if (!UnionIsValidDesKeyType(UnionConvertDesKeyType(type)))
	{
		if (strlen(type) != 0)
			printf("Waringing!! Invalid key type [%s]!\n",type);
		if (UnionIsQuit(p = UnionInput("请输入密钥的类型：")))
			return(UnionTaskActionBeforeExit());
		strcpy(type,p);
		goto inputType;
	}
	
	length = -100;
	//if (argc >= 4)
		//length = atoi(argv[3]);
inputLength:
	if (!UnionIsValidDesKeyLength(UnionConvertDesKeyLength(length)))
	{
		if (length != -100)
			printf("Waringing!! Invalid key length [%d]!\n",length);
		if (UnionIsQuit(p = UnionInput("请输入密钥的长度(16/32/48)：")))
			return(UnionTaskActionBeforeExit());
		length = atoi(p);
		goto inputLength;
	}
	
	componentNum = -100;
	//if (argc >= 5)
		//componentNum = atoi(argv[4]);
inputComponentNum:
	if (componentNum <= 0 || componentNum > 9)
	{
		if (componentNum != -100)
			printf("Waringing!! Invalid key component num [%d]!\n",componentNum);
		if (UnionIsQuit(p = UnionInput("请输入密钥成分的数量(1-9)：")))
			return(UnionTaskActionBeforeExit());
		componentNum = atoi(p);
		goto inputComponentNum;
	}

	formatNum = 10;
	for (formatIndex = 0; formatIndex < formatNum; formatIndex++)
		memset(format[formatIndex],0,sizeof(format[formatIndex]));
	
	for (formatIndex = 0,formatNum = 0;formatNum < 9;formatIndex++)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		iInput = 0;
		switch(formatIndex)
		{
		case 0:		// 收件人单位姓名电话
			printf("\n收件人单位姓名电话：XX银行科技部XXX（020-87311073）\n");
			if (UnionConfirm("是否重新输入收件人单位姓名电话?") == 0)
				strcpy(format[formatIndex],"XX银行科技部XXX（020-87311073）");
			else
			{
				iInput = 1;
				strcpy(tmpBuf,"请输入：收件人单位姓名电话(小于30个字符)\n");
			}
			break;
		case 1:		// 收件人地址邮编
			printf("\n收件人地址邮编：广州市农林下路83号广发银行大厦14楼科技部（邮编：510080）\n");
			if (UnionConfirm("是否重新输入收件人地址邮编?") == 0)
				strcpy(format[formatIndex],"广州市农林下路83号广发银行大厦14楼科技部（邮编：510080）");
			else
			{
				iInput = 1;
				strcpy(tmpBuf,"请输入：收件人地址邮编(小于30个字符)\n");
			}
			break;
		case 2:		// 寄件人单位姓名电话
			printf("\n寄件人单位姓名电话：XX银行北京分行科技部杨阳（010-222288888）\n");
			if (UnionConfirm("是否重新输入寄件人单位姓名电话?") == 0)
				strcpy(format[formatIndex],"XX银行北京分行科技部杨阳（010-222288888）");
			else
			{
				iInput = 1;
				strcpy(tmpBuf,"请输入：寄件人单位姓名电话(小于30个字符)\n");
			}
			break;
		case 3:		// 寄件人地址邮编
			printf("\n寄件人地址邮编：北京市东直门三家苍名门大厦22楼（邮编：100200）\n");
			if (UnionConfirm("是否重新输入寄件人地址邮编?") == 0)
				strcpy(format[formatIndex],"北京市东直门三家苍名门大厦22楼（邮编：100200）");
			else
			{
				iInput = 1;
				strcpy(tmpBuf,"请输入：寄件人地址邮编(小于30个字符)\n");
			}
			break;
		case 4:		// 密钥名称
			strcpy(format[formatIndex],caKeyName);
			break;
		case 5:		// 密钥类型
			strcpy(format[formatIndex],type);
			break;
		case 6:		// 密钥编号
			strcpy(format[formatIndex],caKeyNo);
			break;
		case 8:		// 密钥生成日期
			UnionGetFullSystemDate(format[formatIndex]);
			break;
		default:
			sprintf(tmpBuf,"Input Key Description%02d (quit to finish input)::\n",formatIndex);
			break;
		}
		formatNum++;
		if (iInput == 0)
			continue;
		p = UnionInput(tmpBuf);
		memset(format[formatIndex],0,sizeof(format[formatIndex]));
		strcpy(format[formatIndex],p);
		if (UnionIsQuit(p))
			break;
	}

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
	char		fld[11][80];
	char		tmpBuf[100],keyByLMK[10][50];
	
	printf("\n加密机的IP地址:	%s\n",ipAddr);
	printf("密钥名称:	%s\n",argv[4]);
	printf("密钥编号:	%s\n",argv[6]);
	printf("密钥类型:	%s\n",type);
	printf("密钥长度:	%d\n",length);
	printf("密钥成分数量:	%d\n",componentNum);
	printf("密钥生成日期:	%s\n",argv[8]);
	printf("收件人单位姓名电话: %s\n",argv[0]);
	printf("收件人地址邮编: %s\n",argv[1]);
	printf("寄件人单位姓名电话: %s\n",argv[2]);
	printf("寄件人地址邮编: %s\n\n",argv[3]);

	UnionSuccessLog("\n生成密钥要素：\n");
	UnionNullLog("加密机的IP地址:	%s\n",ipAddr);
	UnionNullLog("密钥名称:	%s\n",argv[4]);
	UnionNullLog("密钥编号:	%s\n",argv[6]);
	UnionNullLog("密钥类型:	%s\n",type);
	UnionNullLog("密钥长度:	%d\n",length);
	UnionNullLog("密钥成分数量:	%d\n",componentNum);
	UnionNullLog("密钥生成日期:	%s\n",argv[8]);
	UnionNullLog("收件人单位姓名电话: %s\n",argv[0]);
	UnionNullLog("收件人地址邮编: %s\n",argv[1]);
	UnionNullLog("寄件人单位姓名电话: %s\n",argv[2]);
	UnionNullLog("寄件人地址邮编: %s\n\n",argv[3]);

	if (!UnionConfirm("确认产生密钥成分吗?"))
		return(UnionTaskActionBeforeExit());

	memset(&sjl06,0,sizeof(sjl06));
	strcpy(sjl06.staticAttr.ipAddr,ipAddr);
	if ((ret = UnionSelectSJL06Rec(&sjl06)) < 0)
	{
		UnionUserErrLog("in UnionGenerateKeyComponent:: UnionSelectSJL06Rec [%s] Error\n",ipAddr);
		printf("UnionSelectSJL06Rec [%s] Error\n",ipAddr);
		return(UnionTaskActionBeforeExit());
	}
	sjl06.dynamicAttr.status = conOnlineSJL06;
	
	// 设置密钥明文成分的打印格式
	memset(resBuf,0,sizeof(resBuf));
	if ((ret = RacalCmdPA(-1,&sjl06,pgPrintFormat,resBuf)) < 0)
	{
		UnionUserErrLog("in UnionGenerateKeyComponent:: RacalCmdPA [%s] Error\n",ipAddr);
		printf("RacalCmdPA [%s] Error\n",ipAddr);
		return(UnionTaskActionBeforeExit());
	}
	
	if (strncmp(resBuf,"00",2) == 0)
	{
		UnionSuccessLog("in UnionGenerateKeyComponent:: Set Print Format for [%s] OK!\n",ipAddr);
		printf("Set Print Format for [%s] OK!\n",ipAddr);
	}
	else
	{
		UnionUserErrLog("in UnionGenerateKeyComponent:: Set Print Format for [%s] Failure! errCode = [%s]!\n",ipAddr,resBuf);
		printf("Set Print Format for [%s] Failure! errCode = [%s]!\n",ipAddr,resBuf);
		return(UnionTaskActionBeforeExit());
	}

	// 给打印密钥信封格式中的每个域进行赋值
	for (i = 0; (i < argc) && (i < 10); i++)
	{
		memset(fld[i],0,sizeof(fld[i]));
		strcpy(fld[i],argv[i]);
	}
	memset(fld[9],0,sizeof(fld[9]));
	strcpy(fld[9],"注意：如果你拾到本信封或发现此信封曾被拆阅，请立即通知寄件人或收件人。");
	memset(fld[10],0,sizeof(fld[10]));
	strcpy(fld[10],"收件人收到本密件后请按要求保管，作好登记，并报知寄件人。");

	UnionSuccessLog("in UnionGenerateKeyComponent:: Fields of printing key:\n");
	for (j=0;j<argc+2;j++)
	{
		if (j == 7)
			continue;
		UnionNullLog("field [%d] = [%s]\n",j,fld[j]);
	}
	UnionNullLog("field number = [%d]\n",argc+2);

	// 打印每个分量的明文成分
	for (i = 0; i < componentNum; i++)
	{
		// 分量序号
		memset(fld[7],0,sizeof(fld[7]));
		sprintf(fld[7],"%02d",i+1);
		UnionNullLog("\nfield[7] = [%s]\n",fld[7]);
		UnionNullLog("in UnionGenerateKeyComponent:: Generate and print the %sth key component:\n",fld[7]);
		
		memset(resBuf,0,sizeof(resBuf));
		if ((ret = RacalCmdA2(-1,&sjl06,UnionConvertDesKeyType(type),UnionConvertDesKeyLength(length),argc+2,fld,resBuf,resBuf+60)) < 0)
		{
			UnionUserErrLog("in UnionGenerateKeyComponent:: RacalCmdA2 [%s] Error\n",ipAddr);
			printf("RacalCmdA2 [%s] Error\n",ipAddr);
			return(UnionTaskActionBeforeExit());
		}
		if (strncmp(resBuf+60,"00",2) == 0)
		{
			UnionSuccessLog("in UnionGenerateKeyComponent:: Generate and print [%0d]th OK! key encrypted by LMK=[%s]\n",i+1,resBuf);
			printf("Generate and print [%0d]th OK! key encrypted by LMK=[%s]\n",i+1,resBuf);
		}
		else
		{
			UnionUserErrLog("in UnionGenerateKeyComponent:: Generate and print [%0d]th Error! errCode = [%s]\n",i+1,resBuf+60);
			printf("Generate and print [%0d]th Error! errCode = [%s]\n",i+1,resBuf+60);
			return(UnionTaskActionBeforeExit());
		}
		memset(keyByLMK[i],0,sizeof(keyByLMK[i]));
		memcpy(keyByLMK[i],resBuf,ret);
	}
	UnionLog("in UnionGenerateKeyComponent:: Generate key OK!\n");
	printf("Generate key OK!\n");
	
	// 打印每个分量的校验值和总校验值
	ret=PrintKeyCheckValue(&sjl06,keyByLMK,componentNum,UnionConvertDesKeyType(type),UnionConvertDesKeyLength(length));
	if (ret < 0)
	{
		UnionUserErrLog("in UnionGenerateKeyComponent:: PrintKeyCheckValue fail! return=[%d]\n",ret);
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
			UnionUserErrLog("in PrintKeyCheckValue:: RacalCmdBU Error! return=[%d]\n",ret);
			printf("RacalCmdBU Error! return=[%d]\n",ret);
			return(-1);
		}
		if (strncmp(errCode,"00",2) == 0)
		{
			UnionSuccessLog("in PrintKeyCheckValue:: Generate [%0d]th key check value OK! check value=[%s]\n",i+1,checkValue[i]);
			printf("Generate [%0d]th key check value OK! check value=[%s]\n",i+1,checkValue[i]);
		}
		else
		{
			UnionUserErrLog("in PrintKeyCheckValue:: Generate [%0d]th key check value Error! errCode = [%s]\n",i+1,errCode);
			printf("Generate [%0d]th key check value Error! errCode = [%s]\n",i+1,errCode);
			return(-1);
		}
	}
	UnionNullLog("in PrintKeyCheckValue:: Key components and their checkvalue:\n");
	for (i=0;i<partKeyNum;i++)
	{
		UnionNullLog("[%d] key component=[%s] check value=[%s]\n",i,partKey[i],checkValue[i]);
	}
	UnionNullLog("Number of key component = [%d]\n",partKeyNum);
	
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
			UnionUserErrLog("in PrintKeyCheckValue:: RacalCmdA4 Error! return=[%d]\n",ret);
			printf("RacalCmdA4 Error! return=[%d]\n",ret);
			return(-1);
		}
		if (strncmp(errCode,"00",2) == 0)
		{
			UnionSuccessLog("in PrintKeyCheckValue:: Generate total key check value OK!\n");
			printf("Generate total key check value OK!\n");
		}
		else
		{
			UnionUserErrLog("in PrintKeyCheckValue:: Generate total key check value Error! errCode = [%s]\n",errCode);
			printf("Generate total key check value Error! errCode = [%s]\n",errCode);
			return(-1);
		}
	}
	UnionNullLog("in PrintKeyCheckValue:: total key=[%s] check value=[%s]\n\n",keyByLMK,checkValue[9]);
	printf("total key by LMK=[%s] check value=[%s]\n",keyByLMK,checkValue[9]);
	
	// 设置打印密钥校验值的格式
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = RacalCmdPA(-1,pSJL06,pgPrintFormatForCheckValue,errCode)) < 0)
	{
		UnionUserErrLog("in PrintKeyCheckValue:: RacalCmdPA Error! errCode=[%s]\n",errCode);
		printf("RacalCmdPA Error! errCode=[%s]\n",errCode);
		return(-1);
	}
	
	if (strncmp(errCode,"00",2) == 0)
	{
		UnionSuccessLog("in PrintKeyCheckValue:: Set Print Format for print key check value OK!\n");
		printf("Set Print Format for print key check value OK!\n");
	}
	else
	{
		UnionUserErrLog("in PrintKeyCheckValue:: Set Print Format for print key check value Failure! errCode = [%s]!\n",errCode);
		printf("Set Print Format for print key check value Failure! errCode = [%s]!\n",errCode);
		return(-1);
	}
	
	// 打印各个密钥成分的校验值以及合成密钥的校验值
	fldNum = partKeyNum + 1;
	for (i=0;i<partKeyNum;i++)
	{
		memset(fld[i],0,sizeof(fld[i]));
		sprintf(fld[i],"第%d个密钥分量的校验值: %s",i+1,checkValue[i]);
	}
	memset(fld[partKeyNum],0,sizeof(fld[partKeyNum]));
	sprintf(fld[partKeyNum],"总的密钥校验值: %s",checkValue[9]);

	UnionNullLog("in PrintKeyCheckValue:: Fields of printing key checkvalue:\n");
	for (i=0;i<fldNum;i++)
	{
		UnionNullLog("field [%d] = [%s]\n",i,fld[i]);
	}
	UnionNullLog("field number = [%d]\n",fldNum);
	
	if ((ret = RacalCmdA2(-1,pSJL06,keyType,keyLength,fldNum,fld,tmpBuf,errCode)) < 0)
	{
		UnionUserErrLog("in PrintKeyCheckValue:: RacalCmdA2 Error! errCode=[%s]\n",errCode);
		printf("RacalCmdA2 Error! errCode=[%s]\n",errCode);
		return(-1);
	}
	if (strncmp(errCode,"00",2) == 0)
	{
		UnionSuccessLog("in PrintKeyCheckValue:: Print key check value OK!\n");
		printf("Print key check value OK!\n");
	}
	else
	{
		UnionUserErrLog("in PrintKeyCheckValue:: Print key check value failure! errCode = [%s]\n",errCode);
		printf("Print key check value failure! errCode = [%s]\n",errCode);
		return(-1);
	}

	return(0);
}
