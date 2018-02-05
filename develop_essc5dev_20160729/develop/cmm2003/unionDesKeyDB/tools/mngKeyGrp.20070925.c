// Author:	Wolfgang Wang
// Date:	2005/06/07

#ifndef _UnionDesKeyDB_2_x_
#define _UnionDesKeyDB_2_x_
#endif
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

#define conMaxNumOfDefaultDesKey	20

TUnionDesKey	gunionDefaultAttrDesKeyGrp[conMaxNumOfDefaultDesKey];	// ����ȱʡ��Կ������
int		gunionRealNumOfDefaultDesKey = 0;

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
	printf("Usage:: %s command [-def defineFileName] [-data dataFileName|ownerID]\n",UnionGetApplicationName());
	printf(" where command as follows::\n");
	printf(" insert/delete/print\n");
	
	return(0);
}
	
int UnionInitDefaultDesKey(char *defaultDesKeyDefFileName)
{
	FILE	*fp;
	int	ret;
	char	tmpBuf[8192+1];
	int	lineLen;
	int	lines = 0;
	int	index;
	int	isDefineLine = 0;
	char	type[40+1];
	int	tmpInt;
	
	// ����ȱʡ����
	for (index = 0; index < conMaxNumOfDefaultDesKey; index++)
	{
		memset(&(gunionDefaultAttrDesKeyGrp[index]),0,sizeof(gunionDefaultAttrDesKeyGrp[index]));
		UnionGetFullSystemDate(gunionDefaultAttrDesKeyGrp[index].activeDate);
		gunionDefaultAttrDesKeyGrp[index].maxUseTimes = -1;
		gunionDefaultAttrDesKeyGrp[index].maxEffectiveDays = 36500;
		gunionDefaultAttrDesKeyGrp[index].oldVerEffective = 1;
		gunionDefaultAttrDesKeyGrp[index].windowBetweenKeyVer = 600;
		gunionDefaultAttrDesKeyGrp[index].length = con128BitsDesKey;
		gunionDefaultAttrDesKeyGrp[index].type = conZMK;
		strcpy(gunionDefaultAttrDesKeyGrp[index].container,"null");
		strcpy(gunionDefaultAttrDesKeyGrp[index].fullName,"TEST.null.zmk");
	}
	
	if (defaultDesKeyDefFileName == NULL)
		return(0);
	
	sprintf(tmpBuf,"%s/keyGrpDef/%s.conf",getenv("UNIONETC"),defaultDesKeyDefFileName);
	if ((fp = fopen(tmpBuf,"r")) == NULL)
	{
		UnionUserErrLog("in UnionInitDefaultDesKey:: fopen [%s]\n",tmpBuf);
		return(errCodeUseOSErrCode);
	}
	
	gunionRealNumOfDefaultDesKey = 0;
	while(!feof(fp))
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		// ���ļ��ж�ȡһ��������
		if ((lineLen = UnionReadOneFileLine(fp,tmpBuf)) <= 0)
			continue;
		lines++;
		if (UnionIsUnixShellRemarkLine(tmpBuf))	// ע����
			continue;
		// ����Կȫ��
		if ((ret = UnionReadFldFromRecStr(tmpBuf,"fullName","string",gunionDefaultAttrDesKeyGrp[gunionRealNumOfDefaultDesKey].fullName,sizeof(gunionDefaultAttrDesKeyGrp[gunionRealNumOfDefaultDesKey].fullName)-1)) <= 0)
		{
			continue;	// ���붨����Կȫ��
		}
		// ����Կ����
		if ((ret = UnionReadFldFromRecStr(tmpBuf,"type","string",type,sizeof(type)-1)) <= 0)
		{
			continue;	// ��Կ�����Ǳ������
		}
		gunionDefaultAttrDesKeyGrp[gunionRealNumOfDefaultDesKey].type = UnionConvertDesKeyType(type);
		// ����Կǿ��
		if ((ret = UnionReadFldFromRecStr(tmpBuf,"length","int",(unsigned char *)(&tmpInt),sizeof(tmpInt))) > 0)
			gunionDefaultAttrDesKeyGrp[gunionRealNumOfDefaultDesKey].length = UnionConvertDesKeyLength(tmpInt);
		// ����Կ����
		UnionReadFldFromRecStr(tmpBuf,"value","string",gunionDefaultAttrDesKeyGrp[gunionRealNumOfDefaultDesKey].value,sizeof(gunionDefaultAttrDesKeyGrp[gunionRealNumOfDefaultDesKey].value)-1);
		// ����ԿУ��ֵ
		UnionReadFldFromRecStr(tmpBuf,"checkValue","string",
			gunionDefaultAttrDesKeyGrp[gunionRealNumOfDefaultDesKey].checkValue,
			sizeof(gunionDefaultAttrDesKeyGrp[gunionRealNumOfDefaultDesKey].checkValue)-1);
		// ����Կ��ʼ��Ч��
		UnionReadFldFromRecStr(tmpBuf,"activeDate","string",
			gunionDefaultAttrDesKeyGrp[gunionRealNumOfDefaultDesKey].activeDate,
			sizeof(gunionDefaultAttrDesKeyGrp[gunionRealNumOfDefaultDesKey].activeDate)-1);
		// ����Կ���ʹ�ô���
		UnionReadFldFromRecStr(tmpBuf,"maxUseTimes","long",
			(unsigned char *)(&(gunionDefaultAttrDesKeyGrp[gunionRealNumOfDefaultDesKey].maxUseTimes)),
			sizeof(gunionDefaultAttrDesKeyGrp[gunionRealNumOfDefaultDesKey].maxUseTimes));
		// ����Կ���Ч��
		UnionReadFldFromRecStr(tmpBuf,"maxEffectiveDays","long",
			(unsigned char *)(&(gunionDefaultAttrDesKeyGrp[gunionRealNumOfDefaultDesKey].maxEffectiveDays)),
			sizeof(gunionDefaultAttrDesKeyGrp[gunionRealNumOfDefaultDesKey].maxEffectiveDays));
		// ����Կ�ɰ汾��Ч��־
		UnionReadFldFromRecStr(tmpBuf,"oldVerEffective","int",
			(unsigned char *)(&(gunionDefaultAttrDesKeyGrp[gunionRealNumOfDefaultDesKey].oldVerEffective)),
			sizeof(gunionDefaultAttrDesKeyGrp[gunionRealNumOfDefaultDesKey].oldVerEffective));
		// ����Կ�ɰ�����
		UnionReadFldFromRecStr(tmpBuf,"oldValue","string",gunionDefaultAttrDesKeyGrp[gunionRealNumOfDefaultDesKey].oldValue,sizeof(gunionDefaultAttrDesKeyGrp[gunionRealNumOfDefaultDesKey].oldValue)-1);
		// ����Կ�ɰ�У��ֵ
		UnionReadFldFromRecStr(tmpBuf,"oldCheckValue","string",gunionDefaultAttrDesKeyGrp[gunionRealNumOfDefaultDesKey].oldCheckValue,sizeof(gunionDefaultAttrDesKeyGrp[gunionRealNumOfDefaultDesKey].oldCheckValue)-1);
		// ����Կ������
		UnionReadFldFromRecStr(tmpBuf,"windowBetweenKeyVer","int",
			(unsigned char *)(&(gunionDefaultAttrDesKeyGrp[gunionRealNumOfDefaultDesKey].windowBetweenKeyVer)),
			sizeof(gunionDefaultAttrDesKeyGrp[gunionRealNumOfDefaultDesKey].windowBetweenKeyVer));
		// ����Կ����
		UnionReadFldFromRecStr(tmpBuf,"container","string",gunionDefaultAttrDesKeyGrp[gunionRealNumOfDefaultDesKey].container,sizeof(gunionDefaultAttrDesKeyGrp[gunionRealNumOfDefaultDesKey].container)-1);
		++gunionRealNumOfDefaultDesKey;
	}
	fclose(fp);
	return(gunionRealNumOfDefaultDesKey);
}

int main(int argc,char *argv[])
{
	int	ret;
	int	index;
	char	defFileName[256+1];
	char	data[256+1];
	
	UnionSetApplicationName(argv[0]);
	
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);
	
	if (argc < 2)
	{
		UnionHelp();
		return(errCodeParameter);
	}

	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	// ��ȡ��Կ�鶨���ļ�
	memset(defFileName,0,sizeof(defFileName));
	if ((ret = UnionReadParFromParGrp(&argv[2],argc-2,"def",defFileName,sizeof(defFileName))) <= 0)
		strcpy(defFileName,"default");
	// ��ȡ����/�����ļ�
	memset(data,0,sizeof(data));
	UnionReadParFromParGrp(&argv[2],argc-2,"data",data,sizeof(data));
		
	// ��ʼ����Կ����
	if ((ret = UnionInitDefaultDesKey(defFileName)) < 0)
	{
		printf("UnionInitDefaultDesKey from file [%s] failure! ret = [%d]\n",defFileName,ret);
		return(UnionTaskActionBeforeExit());
	}
	if (ret = 0)
	{
		printf("no key definition!\n");
		return(UnionTaskActionBeforeExit());
	}

	// ���в���
	if ((strcasecmp(argv[1],"INSERT") == 0) || (strcasecmp(argv[1],"-INSERT") == 0))
		ret = InsertKeyGrp(data);
	else if ((strcasecmp(argv[1],"DELETE") == 0) || (strcasecmp(argv[1],"-DELETE") == 0))
		ret = DeleteKeyGrp(data);
	else
	{
		UnionHelp();
		ret = errCodeParameter;
	}
	
	UnionTaskActionBeforeExit();
	return(ret);
}

int InsertOneGrp(char *thisOwner)
{
	long	index;
	char	idOfApp[40+1];
	char	suffix[40+1];
	char	owner[40+1];
	long	realKeyNum = 0;
	int	ret;
			
	for (index = 0; index < gunionRealNumOfDefaultDesKey; index++)
	{
		memset(idOfApp,0,sizeof(idOfApp));	
		memset(suffix,0,sizeof(suffix));	
		memset(owner,0,sizeof(owner));
		if ((ret = UnionAnalysisDesKeyFullName(gunionDefaultAttrDesKeyGrp[index].fullName,idOfApp,owner,suffix)) < 0)
			continue;
		if (strlen(idOfApp) + strlen(suffix) + strlen(thisOwner) + 2 >= sizeof(gunionDefaultAttrDesKeyGrp[index].fullName))
		{
			printf("keyName = [%s.%s.%s] too long!\n",idOfApp,thisOwner,suffix);
			continue;
		}
		sprintf(gunionDefaultAttrDesKeyGrp[index].fullName,"%s.%s.%s",idOfApp,thisOwner,suffix);
		if ((ret = UnionInsertDesKeyIntoKeyDB(&(gunionDefaultAttrDesKeyGrp[index]))) < 0)
			printf("insert [%s] error! ret = [%d]\n",gunionDefaultAttrDesKeyGrp[index].fullName,ret);
		else
		{
			printf("insert [%s] ok!\n",gunionDefaultAttrDesKeyGrp[index].fullName,ret);
			realKeyNum++;
		}
	}
	return(realKeyNum);
}

int InsertKeyGrp(char *data)
{
	int	ret;
	FILE	*fp;
	char	tmpBuf[1024+1];
	long	realRecNum = 0,realKeyNum = 0;
	char	*ptr;
	
	// �ж��Ƿ�����ļ�������Կ
	if ((data == NULL) || (strlen(data) == 0))
		goto insertOwner;
	sprintf(tmpBuf,"%s/keyGrpDef/%s",getenv("UNIONETC"),data);
	fp = fopen(tmpBuf,"r");
	if (fp == NULL)			
		goto insertOwner;
	
	if (!UnionConfirm("Are you sure of insert keys according to file [%s]?",tmpBuf))
		return(errCodeUserSelectExit);
		
	while (!feof(fp))
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		UnionReadOneFileLine(fp,tmpBuf);
		if (UnionIsUnixShellRemarkLine(tmpBuf))
			continue;
		realRecNum++;
		if ((ret = InsertOneGrp(tmpBuf)) < 0)
			continue;
		realKeyNum += ret;
	}
	fclose(fp);
	printf("realRecNum = [%04ld] realKeyNum = [%04ld]\n",realRecNum,realKeyNum);
	return(realKeyNum);

insertOwner:
	if ((data != NULL) && (strlen(data) != 0))
		goto insertOneGrp;

	ptr = UnionInput("input owner id::");
	strcpy(tmpBuf,ptr);
	if (UnionIsQuit(ptr))
		return(realKeyNum);
	if (!UnionConfirm("Are you sure of inserting keys of [%s]",tmpBuf))
		goto insertOwner;
	if ((ret = InsertOneGrp(tmpBuf)) > 0)
		realKeyNum += ret;
	goto insertOwner;
		
insertOneGrp:
	strcpy(tmpBuf,data);
	if (!UnionConfirm("Are you sure of inserting keys of [%s]",tmpBuf))
		return(realKeyNum);
	if ((ret = InsertOneGrp(tmpBuf)) > 0)
		realKeyNum += ret;
	return(realKeyNum);
}

int DeleteOneGrp(char *thisOwner)
{
	int	index;
	char	idOfApp[40+1];
	char	suffix[40+1];
	char	owner[40+1];
	long	realKeyNum = 0;
	int	ret;
	
	for (index = 0; index < gunionRealNumOfDefaultDesKey; index++)
	{
		memset(idOfApp,0,sizeof(idOfApp));	
		memset(suffix,0,sizeof(suffix));	
		memset(owner,0,sizeof(owner));
		if ((ret = UnionAnalysisDesKeyFullName(gunionDefaultAttrDesKeyGrp[index].fullName,idOfApp,owner,suffix)) < 0)
			continue;
		if (strlen(idOfApp) + strlen(suffix) + strlen(thisOwner) + 2 >= sizeof(gunionDefaultAttrDesKeyGrp[index].fullName))
		{
			printf("keyName = [%s.%s.%s] too long!\n",idOfApp,thisOwner,suffix);
			continue;
		}
		sprintf(gunionDefaultAttrDesKeyGrp[index].fullName,"%s.%s.%s",idOfApp,thisOwner,suffix);
		if ((ret = UnionDeleteDesKeyFromKeyDB(gunionDefaultAttrDesKeyGrp[index].fullName)) < 0)
			printf("delete [%s] error! ret = [%d]\n",gunionDefaultAttrDesKeyGrp[index].fullName,ret);
		else
		{
			printf("delete [%s] ok!\n",gunionDefaultAttrDesKeyGrp[index].fullName,ret);
			realKeyNum++;
		}
	}
	return(realKeyNum);
}
	
int DeleteKeyGrp(char *data)
{
	int	ret;
	FILE	*fp;
	char	tmpBuf[1024+1];
	long	realRecNum = 0,realKeyNum = 0;
	char	*ptr;
	
	// �ж��Ƿ�����ļ�������Կ
	if ((data == NULL) || (strlen(data) == 0))
		goto deleteOwner;
	sprintf(tmpBuf,"%s/keyGrpDef/%s",getenv("UNIONETC"),data);
	fp = fopen(tmpBuf,"r");
	if (fp == NULL)			
		goto deleteOwner;
	
	if (!UnionConfirm("Are you sure of deleting keys according to file [%s]?",tmpBuf))
		return(errCodeUserSelectExit);
		
	while (!feof(fp))
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		UnionReadOneFileLine(fp,tmpBuf);
		if (UnionIsUnixShellRemarkLine(tmpBuf))
			continue;
		realRecNum++;
		if ((ret = DeleteOneGrp(tmpBuf)) < 0)
			continue;
		realKeyNum += ret;
	}
	fclose(fp);
	printf("realRecNum = [%04ld] realKeyNum = [%04ld]\n",realRecNum,realKeyNum);
	return(realKeyNum);

deleteOwner:
	if ((data != NULL) && (strlen(data) != 0))
		goto deleteOneGrp;

	ptr = UnionInput("input owner id::");
	strcpy(tmpBuf,ptr);
	if (UnionIsQuit(ptr))
		return(realKeyNum);
	if (!UnionConfirm("Are you sure of deleting keys of [%s]",tmpBuf))
		goto deleteOwner;
	if ((ret = DeleteOneGrp(tmpBuf)) > 0)
		realKeyNum += ret;
	goto deleteOwner;
		
deleteOneGrp:
	strcpy(tmpBuf,data);
	if (!UnionConfirm("Are you sure of deleting keys of [%s]",tmpBuf))
		return(realKeyNum);
	if ((ret = DeleteOneGrp(tmpBuf)) > 0)
		realKeyNum += ret;
	return(realKeyNum);
}
