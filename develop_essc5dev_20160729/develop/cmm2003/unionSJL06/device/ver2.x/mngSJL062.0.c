// Author:	Wolfgang Wang
// Date:	2003/09/12
// Version:	1.0

// 2004/06/22，Wolfgang Wang升级为2.0
// 升级是因为修改了TUnionSJL06StaticAttr的结构

#define _UnionSJL06_2_x_Above_	// Added By Wolfgang Wang, 2004/06/22，这个开关是必须的

#define _UnionLogMDL_3_x_
#define _UnionTask_3_x_

#include <stdio.h>
#include <string.h>

#include "unionCommand.h"
#include "sjl06.h"

#include "unionVersion.h"
#include "UnionTask.h"
#include "UnionStr.h"

PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	//return(exit(0));
	exit(0);
}


int UnionHelp()
{
	printf("Usage:: %s command [argument...]\n",UnionGetApplicationName());
	printf(" Where command as follow:\n");
	printf("	create ipAddr ...	Create an sjl06 device\n");
	printf("	delete ipAddr ...	Delete sjl06 devices\n");
	printf("	print  ipAddr ...	Print status of sjl06 devices\n");
	printf("	edit			Edit sjl06 Configuration file\n");

	return(0);
}

int UnionCreateCommandHelp()
{
	printf("input ipAddr to set ipAddr, or input a command.\n");
	printf("valid command:\n");
	printf(" ipAddr		to set ipAddr\n");
	printf(" create		to create a conf file\n");
	printf(" delete		to delete the configuration file\n");
	printf(" print		to print the configuration file\n");
	printf(" status		print the current configuration\n");
	printf(" group		to set hsm group id\n");
	printf(" version	to set hsm command version\n");
	printf(" port		to set hsm port\n");
	printf(" lenoflenfield	to set hsm command length of length field\n");
	printf(" lenofmsgheader to set hsm message header length\n");
	printf(" msgheader	to set msgheader\n");
	printf(" remark		to set remark\n");
	printf(" maxlongconn	to set concurrent long connections\n");
	printf(" registerdate	to set register date\n");
	// Added by Wolfgang Wang, 2004/06/22
	printf(" testCmdReq	to set test command request string\n");
	printf(" testCmdSuccessRes to set test command success response string\n");
	// end of addition of 2004/06/22
	printf(" help or ?	to help\n");
	return(0);
}

int main(int argc,char **argv)
{
	int	ret;
	int	i;
		
	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		return(UnionHelp());
	
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
	
	if (strcasecmp(argv[1],"CREATE") == 0)
	{
		for (i = 0; i < argc - 2; i++)
		{
			if (!UnionConfirm("Are you sure of create configuration file for [%s]",argv[2+i]))
				continue;
			if ((ret = UnionCreateSJL06Conf(argv[2+i])) < 0)
				printf("UnionCreateSJL06Conf [%s] Failure! ret = [%d]\n",argv[2+i],ret);
			else
				printf("UnionCreateSJL06Conf [%s] OK!\n",argv[2+i]);
		}
		if (argc <= 2)
			UnionCreateSJL06ConfArtificially();
		return(UnionTaskActionBeforeExit());
	}
			
	if (strcasecmp(argv[1],"DELETE") == 0)
	{
		for (i = 0; i < argc - 2; i++)
		{
			if (!UnionConfirm("Are you sure of deleting configuration file for [%s]",argv[2+i]))
				continue;
			if ((ret = UnionDeleteSJL06Conf(argv[2+i])) < 0)
				printf("UnionDeleteSJL06Conf [%s] Failure! ret = [%d]\n",argv[2+i],ret);
			else
				printf("UnionDeleteSJL06Conf [%s] OK!\n",argv[2+i]);
		}
		if (argc <= 2)
			UnionDeleteSJL06ConfArtificially();
		return(UnionTaskActionBeforeExit());
	}
	if (strcasecmp(argv[1],"PRINT") == 0)
	{
		for (i = 0; i < argc - 2; i++)
			UnionPrintSJL06Conf(argv[2+i]);
		if (argc - 2 <= 0)
			ret = UnionPrintSJL06Conf(NULL);
		return(UnionTaskActionBeforeExit());
	}
	
	if (strcasecmp(argv[1],"EDIT") == 0)
	{
		ret = UnionEditSJL06Conf();
		return(UnionTaskActionBeforeExit());
	}
	
	UnionHelp();
	
	return(UnionTaskActionBeforeExit());	
}

int UnionEditSJL06Conf()
{
	int			ret;

	TUnionSJL06		sjl06;
	TUnionSJL06StaticAttr	staticAttr;
	TUnionSJL06DynamicAttr	dynamicAttr;		
	char			*p;

	dynamicAttr.timeoutTimes = 0;
	dynamicAttr.connFailTimes = 0;
	dynamicAttr.abnormalCmdTimes = 0;
	dynamicAttr.normalCmdTimes = 0;
	dynamicAttr.continueFailTimes = 0;
	dynamicAttr.downTimes = 0;
	strcpy(dynamicAttr.lastDownDate,"00000000");
	dynamicAttr.status = '1';
	dynamicAttr.activeLongConn = 0;
		
	memset(&staticAttr,0,sizeof(staticAttr));
	strcpy(staticAttr.hsmGrpID,"001");
	strcpy(staticAttr.hsmCmdVersion,"SJL06");
	staticAttr.port = 8;
	staticAttr.lenOfLenFld = 2;
	staticAttr.lenOfMsgHeader = 0;
	memset(staticAttr.msgHeader,0,sizeof(staticAttr.msgHeader));
	staticAttr.maxConLongConn = 10;
	UnionGetFullSystemDate(staticAttr.registerDate);
	memset(staticAttr.remark,0,sizeof(staticAttr.remark));
	// Added by Wolfgang Wang, 2004/06/22
	strcpy(staticAttr.testCmdReq,"01");
	strcpy(staticAttr.testCmdSuccessRes,"0200");
	// End of Addition of 2004/06/22
		
loopEdit:
	sjl06.dynamicAttr = dynamicAttr;
	sjl06.staticAttr = staticAttr;
	
	if (UnionIsQuit(p = UnionInput("Command>")))
		return(0);
	
	if (UnionIsValidIPAddrStr(p))
	{
		strcpy(staticAttr.ipAddr,p);
		goto loopEdit;
	}
	
	if (strcasecmp(p,"CREATE") == 0)
	{
		UnionPrintSJL06Rec(&sjl06);
	
		if (UnionConfirm("Are you sure of create such a device?"))
		{
			if ((ret = UnionInsertSJL06Rec(&sjl06)) < 0)
				printf("UnionInsertSJL06Rec Error! ret = [%d]\n",ret);
			else
				printf("UnionInsertSJL06Rec OK!\n");
		}
		goto loopEdit;	
	}
	
	if (strcasecmp(p,"STATUS") == 0)
	{
		UnionPrintSJL06Rec(&sjl06);
		goto loopEdit;
	}
	
	if (strcasecmp(p,"PRINT") == 0)
	{
		UnionPrintSJL06Conf(staticAttr.ipAddr);
		goto loopEdit;
	}
	
	if (strcasecmp(p,"DELETE") == 0)
	{
		if (!UnionConfirm("Are you sure of deleteing configuration file of [%s]?",staticAttr.ipAddr))
			goto loopEdit;
		if ((ret = UnionDeleteSJL06Conf(staticAttr.ipAddr)) < 0)
			printf("UnionDeleteSJL06Conf Error! Ret = [%d]\n",ret);
		else
			printf("UnionDeleteSJL06Conf OK!\n");
		goto loopEdit;
	}
	
	if (strcasecmp(p,"IPADDR") == 0)
		goto inputIPAddr;
	if (strcasecmp(p,"GROUP") == 0)
		goto inputHsmGrpID;
	if (strcasecmp(p,"VERSION") == 0)
		goto inputHsmCmdVersion;
	if (strcasecmp(p,"PORT") == 0)
		goto inputPort;
	if (strcasecmp(p,"LENOFLENFLD") == 0)
		goto inputlenOfLenFld;
	if (strcasecmp(p,"LENOFMSGHEADER") == 0)
		goto inputlenOfMsgHeader;
	if (strcasecmp(p,"MSGHEADER") == 0)
		goto inputMessageHeader;
	if (strcasecmp(p,"REMARK") == 0)
		goto inputRemark;
	if (strcasecmp(p,"MAXLONGCONN") == 0)
		goto inputMaxConLongConn;
	if (strcasecmp(p,"REGISTERDATE") == 0)
		goto inputRegisterDate;
	// Added by Wolfgang Wang, 2004/06/22
	if (strcasecmp(p,"TESTCMDREQ") == 0)
		goto inputTestCmdReq;
	if (strcasecmp(p,"TESTCMDSUCCESSRES") == 0)
		goto inputTestCmdSuccessRes;
	// End of Addition of 2004/06/22
	if ((strcasecmp(p,"HELP") == 0) || (strcasecmp(p,"?") == 0))
	{
		UnionCreateCommandHelp();
		goto loopEdit;
	}
	printf("Wrong command!\n");
	UnionCreateCommandHelp();
	goto loopEdit;
		
inputIPAddr:
	if (!UnionIsValidIPAddrStr(p=UnionInput("Input IPAddr Of Hsm::")))
		goto inputIPAddr;
	strcpy(staticAttr.ipAddr,p);
	goto loopEdit;
	
inputHsmGrpID:
	if (strlen(p=UnionInput("Input HsmGrpID (%d Characters)::",sizeof(staticAttr.hsmGrpID) - 1)) != sizeof(staticAttr.hsmGrpID) - 1)
		goto inputHsmGrpID;
	strcpy(staticAttr.hsmGrpID,p);
	goto loopEdit;
	
inputHsmCmdVersion:
	if (strlen(p=UnionInput("Input HsmCmdVersion (%d Characters)::",sizeof(staticAttr.hsmCmdVersion) - 1)) > sizeof(staticAttr.hsmCmdVersion) - 1)
		goto inputHsmCmdVersion;
	if (!UnionIsValidHsmCmdVersion(p))
	{
		printf("Invalid HsmCmd Version!\n");
		goto inputHsmCmdVersion;
	}
	strcpy(staticAttr.hsmCmdVersion,p);
	goto loopEdit;

inputPort:
	if ((staticAttr.port = atoi(UnionInput("Input Hsm Port::"))) <= 0)
		goto inputPort;
	goto loopEdit;

inputlenOfLenFld:
	if ((staticAttr.lenOfLenFld = atoi(UnionInput("Input length of command string length field::"))) < 0)
		goto inputlenOfLenFld;
	goto loopEdit;

inputlenOfMsgHeader:
	if ((staticAttr.lenOfMsgHeader = atoi(UnionInput("Input length of message header::"))) < 0)
		goto inputlenOfMsgHeader;
	goto loopEdit;

inputMessageHeader:
	if (staticAttr.lenOfMsgHeader > 0)
	{
		if (strlen(p=UnionInput("Input message header (%d Characters)::",staticAttr.lenOfMsgHeader)) != staticAttr.lenOfMsgHeader)
			goto inputMessageHeader;
		strcpy(staticAttr.msgHeader,p);
	}
	goto loopEdit;

inputRemark:
	if (strlen(p=UnionInput("Input remark (maximum %d Characters)::\n",sizeof(staticAttr.remark) - 1)) > sizeof(staticAttr.remark) - 1)
		goto inputRemark;
	strcpy(staticAttr.remark,p);
	goto loopEdit;

inputMaxConLongConn:
	if ((staticAttr.maxConLongConn = atoi(UnionInput("Input Maximum Concurrent Long Connection::"))) <= 0)
		goto inputMaxConLongConn;
	goto loopEdit;
	
inputRegisterDate:
	if (!UnionConfirm("Use current system date as the register date?"))
	{
		if (strlen(p=UnionInput("Input registerDate (YYYYMMDD)::\n")) != sizeof(staticAttr.registerDate) - 1)
			goto inputRegisterDate;
		strcpy(staticAttr.registerDate,p);
	}
	else
		UnionGetFullSystemDate(staticAttr.registerDate);
	goto loopEdit;

	// Added by Wolfgang Wang, 2004/06/22
inputTestCmdReq:
	if (strlen(p=UnionInput("Input test command request string (maximum %d Characters)::\n",sizeof(staticAttr.testCmdReq) - 1)) > sizeof(staticAttr.remark) - 1)
		goto inputTestCmdReq;
	strcpy(staticAttr.testCmdReq,p);
	goto loopEdit;

inputTestCmdSuccessRes:
	if (strlen(p=UnionInput("Input test command success response string (maximum %d Characters)::\n",sizeof(staticAttr.testCmdSuccessRes) - 1)) > sizeof(staticAttr.remark) - 1)
		goto inputTestCmdSuccessRes;
	strcpy(staticAttr.testCmdSuccessRes,p);
	goto loopEdit;
	// End of Addition of 2004/06/22

}

