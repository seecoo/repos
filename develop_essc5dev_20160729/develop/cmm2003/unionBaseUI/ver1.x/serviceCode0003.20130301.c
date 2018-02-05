
#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#define _UnionTask_3_x_
#define _UnionEnv_3_x_
#define _UnionLogMDL_3_x_
#define _UnionTask_3_2_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <time.h>
#include <sys/dir.h>
#include <sys/stat.h>
#if ( defined __linux__ ) || ( defined _AIX )
#include <sys/statfs.h>
#endif
#include <mntent.h>
#include <sys/vfs.h>

#include "unionREC.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "unionVersion.h"
#include "UnionLog.h"
#include "unionLisence.h"
#include "hsmDefaultFunction.h"
#include "unionHsm.h"
#include "unionCommand.h"
#include "unionModule.h"
#include "UnionTask.h" 
#include "unionMsgBuf7.x.h"
#include "unionCommConf.h"
#include "highCached.h"
#include "unionHighCachedAPI.h"
#include "UnionStr.h"
#include "unionRecFile.h"
#include "baseUIService.h"

int hsm_abnormalNum = 0;
int hardware_abnormalNum = 0;
int commConfMon_abnormalNum = 0;
int msgBufMon_abnormalNum = 0;
int taskMon_abnormalNum = 0;
int msgBufConf_abnormalNum = 0;
int highCached_abnormalNum = 0;
int program_abnormalNum = 0;
int env_abnormalNum = 0;

// �������Ϣ
int hsmRunInfo()
{
	int			ret = 0;
	int			i = 0;
	int			totalNum = 0;
	int			abnormalNum = 0;
	int			unenabledNum = 0;
	char			infoLevel[4+1];
	int			lenList = 0;
	int			lenChnList = 0;
	char			hsmIP[32+1];
	char			tmpBuf[128+1];
	char			sql[1024+1];
	char			hsmGroupID[8+1];
	char			fieldList[1024+1];
	char			fieldListChnName[4096+1];
	PUnionTableDef		ptableDef = NULL;
	int			len = 0;



	// �����ֶ��嵥
	if ((ptableDef = UnionFindTableDef("hsm")) == NULL)
	{
		UnionUserErrLog("in hsmRunInfo:: UnionFindTableDef[%s] not found!\n","hsm");
		return(errCodeDatabaseMDL_TableNotFound);
	}
	
	memset(fieldList,0,sizeof(fieldList));	
	memset(fieldListChnName,0,sizeof(fieldListChnName));	
	for (i = 0; i < ptableDef->fieldNum; i++)
	{
		if (strcmp(ptableDef->fieldDef[i].fieldName,"remark") == 0)
			continue;
		
		lenList += sprintf(fieldList + lenList,"%s,",ptableDef->fieldDef[i].fieldName);	
		lenChnList += sprintf(fieldListChnName + lenChnList,"%s,",ptableDef->fieldDef[i].fieldChnName);
	}

	lenList += sprintf(fieldList + lenList,"%s","infoLevel");	
	lenChnList += sprintf(fieldListChnName + lenChnList,"%s","��Ϣ״��");
	
	// �����ֶ��嵥
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldList",fieldList)) < 0)
	{
		UnionUserErrLog("in hsmRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","fieldList");
		return(ret);
	}

	// �����ֶ��嵥������
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldListChnName",fieldListChnName)) < 0)
	{
		UnionUserErrLog("in hsmRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","fieldListChnName");
		return(ret);
	}

	UnionSetResponseXMLPackageValue("body/totalNum","0");
	UnionSetResponseXMLPackageValue("body/abnormalNum","0");
	//UnionSetResponseXMLPackageValue("body/unenabledNum","0");

	// ��ȡ�������
	len = sprintf(sql,"select hsm.hsmID,hsm.hsmName,hsm.hsmGroupID,hsm.ipAddr,hsm.port,hsm.uses,hsm.status,hsm.enabled from hsm left join hsmGroup on hsm.hsmGroupID = hsmGroup.hsmGroupID order by hsm.hsmGroupID,hsm.hsmID");
	sql[len] = 0;
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in hsmRunInfo:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}
	else if (ret > 0)
	{
		// ��ȡ����
		if ((ret = UnionReadXMLPackageValue("totalNum",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in hsmRunInfo:: UnionReadXMLPackageValue[%s]!\n","totalNum");
			return(ret);
		}
		else
		{
			tmpBuf[ret] = 0;
			totalNum = atoi(tmpBuf);
		}
	}
	else
		totalNum = 0;

	for (i = 1; i <= totalNum; i++)
	{
		memset(infoLevel, 0, sizeof(infoLevel));

		if ((ret = UnionLocateXMLPackage("detail",i)) < 0)
		{
			UnionUserErrLog("in hsmRunInfo:: UnionLocateXMLPackage[%s][%d]!\n","detail",i);
			return(ret);
		}

		if ((ret = UnionLocateResponseNewXMLPackage("body/detail",i)) < 0)
		{
			UnionUserErrLog("in hsmRunInfo:: UnionLocateResponseNewXMLPackage[%s][%d]!\n","body/detail",i);
			return(ret);
		}
		
		//��ȡ�����ID
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("hsmID",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in hsmRunInfo:: UnionReadXMLPackageValue[%s][%d]!\n","hsmID",i);
			return(ret);
		}
		
		if ((ret = UnionSetResponseXMLPackageValue("hsmID",tmpBuf)) < 0)
		{
			UnionUserErrLog("in hsmRunInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","hsmID",i);
			return(ret);
		}

		//��ȡ���������
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("hsmName",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in hsmRunInfo:: UnionReadXMLPackageValue[%s][%d]!\n","hsmName",i);
			return(ret);
		}
		
		if ((ret = UnionSetResponseXMLPackageValue("hsmName",tmpBuf)) < 0)
		{
			UnionUserErrLog("in hsmRunInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","hsmName",i);
			return(ret);
		}

		//��ȡ�������
		memset(hsmGroupID,0,sizeof(hsmGroupID));
		if ((ret = UnionReadXMLPackageValue("hsmGroupID",hsmGroupID,sizeof(hsmGroupID))) < 0)
		{
			UnionUserErrLog("in hsmRunInfo:: UnionReadXMLPackageValue[%s][%d]!\n","hsmGroupID",i);
			return(ret);
		}
		
	if ((ret = UnionSetResponseXMLPackageValue("hsmGroupID",hsmGroupID)) < 0)
		{
			UnionUserErrLog("in hsmRunInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","hsmGroupID",i);
			return(ret);
		}

		//��ȡ�����IP��ַ
		memset(hsmIP,0,sizeof(hsmIP));
		if ((ret = UnionReadXMLPackageValue("ipAddr",hsmIP,sizeof(hsmIP))) < 0)
		{
			UnionUserErrLog("in hsmRunInfo:: UnionReadXMLPackageValue[%s][%d]!\n","ipAddr",i);
			return(ret);
		}
		
		if ((ret = UnionSetResponseXMLPackageValue("ipAddr",hsmIP)) < 0)
		{
			UnionUserErrLog("in hsmRunInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","ipAddr",i);
			return(ret);
		}

		//��ȡ������˿�
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("port",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in hsmRunInfo:: UnionReadXMLPackageValue[%s][%d]!\n","port",i);
			return(ret);
		}
		
		if ((ret = UnionSetResponseXMLPackageValue("port",tmpBuf)) < 0)
		{
			UnionUserErrLog("in hsmRunInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","port",i);
			return(ret);
		}

		//��ȡ�������;
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("uses",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in hsmRunInfo:: UnionReadXMLPackageValue[%s][%d]!\n","uses",i);
			return(ret);
		}
		
		if (atoi(tmpBuf) == 0)
			strcpy(tmpBuf,"��׼");
		else
			strcpy(tmpBuf,"��ӡ");
			
		if ((ret = UnionSetResponseXMLPackageValue("uses",tmpBuf)) < 0)
		{
			UnionUserErrLog("in hsmRunInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","uses",i);
			return(ret);
		}

		// ��ȡ���������
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("enabled",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in hsmRunInfo:: UnionReadXMLPackageValue[%s][%d]!\n","enabled",i);
			return(ret);
		}
		
		if (atoi(tmpBuf) == 1)
			strcpy(tmpBuf,"����");
		else
		{
			unenabledNum++;
			memset(infoLevel, 0, sizeof(infoLevel));
			strcpy(infoLevel,"����");
			strcpy(tmpBuf,"����");
		}

		if ((ret = UnionSetResponseXMLPackageValue("enabled",tmpBuf)) < 0)
		{
			UnionUserErrLog("in hsmRunInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","enabled",i);
			return(ret);
		}

		// ��ȡ�����״̬
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("status",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in hsmRunInfo:: UnionReadXMLPackageValue[%s][%d]!\n","status",i);
			return(ret);
		}

		if (atoi(tmpBuf) == 1)
			strcpy(tmpBuf,"����");
		else
		{
			abnormalNum++;
			memset(infoLevel, 0, sizeof(infoLevel));
			strcpy(infoLevel,"�쳣");
			strcpy(tmpBuf,"�쳣");
		}	
		if ((ret = UnionSetResponseXMLPackageValue("status",tmpBuf)) < 0)
		{
			UnionUserErrLog("in hsmRunInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","status",i);
			return(ret);
		}

		// ��Ϣ״��
		if (strlen(infoLevel) == 0)
			strcpy(infoLevel,"����");
			
		if ((ret = UnionSetResponseXMLPackageValue("infoLevel",infoLevel)) < 0)
		{
			UnionUserErrLog("in hsmRunInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","enabled",i);
			return(ret);
		}
	}

	UnionLocateResponseXMLPackage("",0);

	//��ֵ�ܼ�¼��
	len = sprintf(tmpBuf,"%d",totalNum);
	tmpBuf[len] = 0;
	if ((ret = UnionSetResponseXMLPackageValue("body/totalNum",tmpBuf)) < 0)
	{
		UnionUserErrLog("in hsmRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","body/totalNum");
		return(ret);
	}
		
	abnormalNum -= unenabledNum; // add by chenqy 20151105
	// ��ֵ�쳣��¼��
	len = sprintf(tmpBuf,"%d",abnormalNum);
	tmpBuf[len] = 0;

	if ((ret = UnionSetResponseXMLPackageValue("body/abnormalNum",tmpBuf)) < 0)
	{
		UnionUserErrLog("in hsmRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","body/abnormalNum");
		return(ret);
	}
	/*
	// ��ֵ������¼��
	len = sprintf(tmpBuf,"%d",unenabledNum);
	tmpBuf[len] = 0;
	if ((ret = UnionSetResponseXMLPackageValue("body/unenabledNum",tmpBuf)) < 0)
        {
                UnionUserErrLog("in hsmRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","body/unenabledNum");
                return(ret);
        }*/

	hsm_abnormalNum = abnormalNum;

	return 0;
}

// �������
int hsmGroupInfo()
{
	int			ret = 0;
	int			i = 0;
	int			totalNum = 0;
	int			lenList = 0;
	int			lenChnList = 0;
	char			tmpBuf[128+1];
	char			sql[128+1];
	char			fieldList[1024+1];
	char			fieldListChnName[4096+1];
	PUnionTableDef		ptableDef = NULL;

	// ��ȡ�������
	memset(sql,0,sizeof(sql));
	strcpy(sql,"select * from hsmGroup where hsmGroupID in (select hsmGroupID from hsm)");
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in hsmGroupInfo:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}
	else if (ret > 0)
	{
		// ��ȡ����
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("totalNum",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in hsmGroupInfo:: UnionReadXMLPackageValue[%s]!\n","totalNum");
			return(ret);
		}
		else
			totalNum = atoi(tmpBuf);
	}
	else
		totalNum = 0;
		
	// �����ֶ��嵥
	if ((ptableDef = UnionFindTableDef("hsmGroup")) == NULL)
	{
		UnionUserErrLog("in hsmGroupInfo:: UnionFindTableDef[%s] not found!\n","hsmGroup");
		return(errCodeDatabaseMDL_TableNotFound);
	}
	
	memset(fieldList,0,sizeof(fieldList));
	memset(fieldListChnName,0,sizeof(fieldListChnName));	
	for (i = 0; i < ptableDef->fieldNum; i++)
	{
		if (strcmp(ptableDef->fieldDef[i].fieldName,"remark") == 0)
			continue;
		
		lenList += sprintf(fieldList + lenList,"%s,",ptableDef->fieldDef[i].fieldName);	
		lenChnList += sprintf(fieldListChnName + lenChnList,"%s,",ptableDef->fieldDef[i].fieldChnName);
	}
	fieldList[lenList - 1] = 0;
	fieldListChnName[lenChnList - 1] = 0;
	
	// �����ֶ��嵥
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldList",fieldList)) < 0)
	{
		UnionUserErrLog("in hsmGroupInfo::  UnionSetResponseXMLPackageValue[%s]!\n","fieldList");
		return(ret);
	}

	// �����ֶ��嵥������
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldListChnName",fieldListChnName)) < 0)
	{
		UnionUserErrLog("in hsmGroupInfo::  UnionSetResponseXMLPackageValue[%s]!\n","fieldListChnName");
		return(ret);
	}

	//��ֵ�ܼ�¼��
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d",totalNum);
	if ((ret = UnionSetResponseXMLPackageValue("body/totalNum",tmpBuf)) < 0)
	{
		UnionUserErrLog("in hsmGroupInfo:: UnionSetResponseXMLPackageValue[%s]!\n","body/totalNum");
		return(ret);
	}

	for (i = 1; i <= totalNum; i++)
	{
		if ((ret = UnionLocateXMLPackage("detail",i)) < 0)
		{
			UnionUserErrLog("in hsmGroupInfo:: UnionLocateXMLPackage[%s][%d]!\n","detail",i);
			return(ret);
		}

		if ((ret = UnionLocateResponseNewXMLPackage("body/detail",i)) < 0)
		{
			UnionUserErrLog("in hsmGroupInfo:: UnionLocateResponseNewXMLPackage[%s][%d]!\n","body/detail",i);
			return(ret);
		}
		
		//��ȡ�������ID
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("hsmGroupID",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in hsmGroupInfo:: UnionReadXMLPackageValue[%s][%d]!\n","hsmGroupID",i);
			return(ret);
		}
		
		if ((ret = UnionSetResponseXMLPackageValue("hsmGroupID",tmpBuf)) < 0)
		{
			UnionUserErrLog("in hsmGroupInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","hsmGroupID",i);
			return(ret);
		}

		//��ȡ�����������
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("hsmGroupName",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in hsmGroupInfo:: UnionReadXMLPackageValue[%s][%d]!\n","hsmGroupName",i);
			return(ret);
		}
		
		if ((ret = UnionSetResponseXMLPackageValue("hsmGroupName",tmpBuf)) < 0)
		{
			UnionUserErrLog("in hsmGroupInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","hsmGroupName",i);
			return(ret);
		}

		// �����ָ������
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("hsmCmdVersionID",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in hsmGroupInfo:: UnionReadXMLPackageValue[%s][%d]!\n","hsmCmdVersionID",i);
			return(ret);
		}
		
		if ((ret = UnionSetResponseXMLPackageValue("hsmCmdVersionID",tmpBuf)) < 0)
		{
			UnionUserErrLog("in hsmGroupInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","hsmCmdVersion",i);
			return(ret);
		}

		// ͨѶͷ����
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("lenOfCommHead",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in hsmGroupInfo:: UnionReadXMLPackageValue[%s][%d]!\n","lenOfCommHead",i);
			return(ret);
		}
		
		if ((ret = UnionSetResponseXMLPackageValue("lenOfCommHead",tmpBuf)) < 0)
		{
			UnionUserErrLog("in hsmGroupInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","lenOfCommHead",i);
			return(ret);
		}

		// ��Ϣͷ����
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("lenOfMsgHead",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in hsmGroupInfo:: UnionReadXMLPackageValue[%s][%d]!\n","lenOfMsgHead",i);
			return(ret);
		}
		
		if ((ret = UnionSetResponseXMLPackageValue("lenOfMsgHead",tmpBuf)) < 0)
		{
			UnionUserErrLog("in hsmGroupInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","lenOfMsgHead",i);
			return(ret);
		}


		// ��ȡ̽������ָ�� 
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("reqCmdOfTest",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in hsmGroupInfo:: UnionReadXMLPackageValue[%s]!\n","reqCmdOfTest");
			return(ret);
		}

		if ((ret = UnionSetResponseXMLPackageValue("reqCmdOfTest",tmpBuf)) < 0)
		{
			UnionUserErrLog("in hsmGroupInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","reqCmdOfTest",i);
			return(ret);
		}
	
		// ��ȡ̽����Ӧָ��
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("resCmdOfTest",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in hsmGroupInfo:: UnionReadXMLPackageValue[%s]!\n","resCmdOfTest");
			return(ret);
		}

		if ((ret = UnionSetResponseXMLPackageValue("resCmdOfTest",tmpBuf)) < 0)
		{
			UnionUserErrLog("in hsmGroupInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","resCmdOfTest",i);
			return(ret);
		}

		// ����ָ��
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("sensitiveCmd",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in hsmGroupInfo:: UnionReadXMLPackageValue[%s][%d]!\n","sensitiveCmd",i);
			return(ret);
		}
		
		if ((ret = UnionSetResponseXMLPackageValue("sensitiveCmd",tmpBuf)) < 0)
		{
			UnionUserErrLog("in hsmGroupInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","sensitiveCmd",i);
			return(ret);
		}
	}

	
	return 0;
}

// taskMon:������Ϣ
int taskMonRunInfo()
{
	int			ret = 0;
	int			i = 0;
	int			j = 0;
	char			fieldList[1024+1];
	char			fieldListChnName[4096+1];
	int			totalNum = 0;
	int			maxClassNum = 0;
	char			tmpBuf[512+1];
	int			abnormalNum = 0;
	char			infoLevel[4+1];

	PUnionTaskClass		pTaskClass = NULL;

	// �����ֶ��嵥
	memset(fieldList,0,sizeof(fieldList));
	strcpy(fieldList,"infoLevel,maxNum,currentNum,name,startCommand,logFile");
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldList",fieldList)) < 0)
	{
		UnionUserErrLog("in taskMonRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","fieldList");
		return(ret);
	}

	// �����ֶ��嵥������
	memset(fieldListChnName,0,sizeof(fieldListChnName));
	strcpy(fieldListChnName,"��Ϣ״��,���������,��ǰ������,������,��������,��־��");
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldListChnName",fieldListChnName)) < 0)
	{
		UnionUserErrLog("in taskMonRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","fieldListChnName");
		return(ret);
	}

	if ((maxClassNum = UnionGetMaxNumOfTaskClass()) <= 0)
	{
		UnionUserErrLog("in taskMonRunInfo:: UnionGetMaxNumOfTaskClass [%d]\n",maxClassNum);
		return(maxClassNum);
	}

	if ((pTaskClass = UnionGetCurrentTaskClassGrp()) == NULL)
	{
		UnionUserErrLog("in taskMonRunInfo:: UnionGetCurrentTaskClassGrp!\n");
		UnionSetResponseRemark("��ȡ������Ϣʧ��");
		return(errCodeParameter);
	}
	
	UnionSetResponseXMLPackageValue("body/totalNum","0");
	UnionSetResponseXMLPackageValue("body/abnormalNum","0");

	for (i = 0; i < maxClassNum; i++)
	{
		memset(infoLevel,0,sizeof(infoLevel));

		if ((pTaskClass + i) == NULL)
			continue;	
		if (strlen((pTaskClass + i)->name) == 0)
			continue;	

	
		//��ȡ�����������
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%d",(pTaskClass + i)->minNum);
		if (atoi(tmpBuf) == 0)
			continue;

		if ((ret = UnionLocateResponseNewXMLPackage("body/detail",totalNum + 1)) < 0)
		{
			UnionUserErrLog("in taskMonRunInfo:: UnionLocateResponseNewXMLPackage[%s][%d]!\n","body/detail",totalNum+1);
			return(ret);
		}
		
		if ((ret = UnionSetResponseXMLPackageValue("maxNum",tmpBuf)) < 0)
		{
			UnionUserErrLog("in taskMonRunInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","maxNum",totalNum+1);
			return(ret);
		}
	
		// ��ȡ��ǰ������
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%d",(pTaskClass + i)->currentNum);
		
		if ((ret = UnionSetResponseXMLPackageValue("currentNum",tmpBuf)) < 0)
		{
			UnionUserErrLog("in taskMonRunInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","currentNum",totalNum+1);
			return(ret);
		}
		
		// ��ȡ������
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%s",(pTaskClass + i)->name);
		
		if ((ret = UnionSetResponseXMLPackageValue("name",tmpBuf)) < 0)
		{
			UnionUserErrLog("in taskMonRunInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","name",totalNum+1);
			return(ret);
		}
		
		if ((memcmp(tmpBuf,"client",6) != 0) && (memcmp(tmpBuf,"defaultTask",11) != 0))
		{
			if (memcmp(tmpBuf,"longTcpipSvr",12) == 0 || memcmp(tmpBuf,"shortTcpipSvr",13) == 0 || memcmp(tmpBuf,"appTask",7) == 0)
			{
				/*if ((pTaskClass + i)->currentNum > 50)	
				{
					strcpy(infoLevel,"�쳣");
					abnormalNum++;
				}*/
			}
			else if ((pTaskClass + i)->minNum != (pTaskClass + i)->currentNum)
			{
				strcpy(infoLevel,"�쳣");
				abnormalNum++;
			}
		}

		// ��ȡ��������
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%s",(pTaskClass + i)->startCmd);
		
		if ((ret = UnionSetResponseXMLPackageValue("startCommand",tmpBuf)) < 0)
		{
			UnionUserErrLog("in taskMonRunInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","startCommand",totalNum+1);
			return(ret);
		}

		// ��ȡ��־��
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%s",(pTaskClass + i)->logFileName);
		
		if ((ret = UnionSetResponseXMLPackageValue("logFile",tmpBuf)) < 0)
		{
			UnionUserErrLog("in taskMonRunInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","logFile",totalNum+1);
			return(ret);
		}
	
		// ��Ϣ״��
		if (strlen(infoLevel) == 0)
			strcpy(infoLevel,"����");
		if ((ret = UnionSetResponseXMLPackageValue("infoLevel",infoLevel)) < 0)
		{
			UnionUserErrLog("in taskMonRunInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","infoLevel",totalNum+1);
			return(ret);
		}
		totalNum++;

		UnionLocateResponseXMLPackage("",0);
		if (strcmp(infoLevel,"�쳣") == 0 && totalNum != 1) 
		{
			for (j = totalNum; j > abnormalNum; j--)
			{
				if ((ret = UnionExchangeIDResponseXMLPackage("body/detail",j,j-1)) < 0)
				{
					UnionUserErrLog("in taskMonRunInfo:: UnionExchangeIDResponseXMLPackage[%s][%d][%d]!\n","infoLevel",j,j-1);
				}
			}
		}
		
	} 
	
	UnionLocateResponseXMLPackage("",0);

	//��ֵ�ܼ�¼��
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d",totalNum);
	if ((ret = UnionSetResponseXMLPackageValue("body/totalNum",tmpBuf)) < 0)
	{
		UnionUserErrLog("in taskMonRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","body/totalNum");
		return(ret);
	}

	// ��ֵ�쳣��¼��
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d",abnormalNum);
	if ((ret = UnionSetResponseXMLPackageValue("body/abnormalNum",tmpBuf)) < 0)
	{
		UnionUserErrLog("in taskMonRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","body/abnormalNum");
		return(ret);
	}

	taskMon_abnormalNum = abnormalNum;

	return 0;
}

// ����������Ϣ
int msgBufConfRunInfo()
{
	int			ret = 0;
	char			tmpBuf[128+1];
	char			maxNumOfMsg[128+1];
	char			maxSizeOfMsg[128+1];
	char			fieldList[1024+1];
	char			fieldListChnName[2048+1];
	int			abnormalNum = 0;	
	int			lenSize = 0;
	int			lenNum = 0;
	int			i = 0;

	PUnionMsgBufHDL		pMsgBufHDL = NULL;

	// �����ֶ��嵥
	memset(fieldList,0,sizeof(fieldList));
	strcpy(fieldList,"userID,groupNumOfMsg,maxSizeOfMsg,maxNumOfMsg,maxStayTime,NumOfFreePos,NumOfOccupiedPos,infoLevel");
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldList",fieldList)) < 0)
	{
		UnionUserErrLog("in msgBufConfRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","fieldList");
		return(ret);
	}

	// �����ֶ��嵥������
	memset(fieldListChnName,0,sizeof(fieldListChnName));
	strcpy(fieldListChnName,"����ID,��Ϣ������,��Ϣ��󳤶�,��Ϣ�������,��Ϣ����ʱ��,��Ϣ��������,��Ϣռ������,��Ϣ״��");
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldListChnName",fieldListChnName)) < 0)
	{
		UnionUserErrLog("in msgBufConfRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","fieldListChnName");
		return(ret);
	}

	
	if ((pMsgBufHDL = UnionGetCurrentMsgBufHDLGrp()) == NULL)
	{
		UnionUserErrLog("in msgBufConfRunInfo:: UnionGetCurrentMsgBufHDLGrp");
		UnionSetResponseRemark("��ȡ����������Ϣʧ��");
		return(errCodeParameter);
	}
	
	UnionSetResponseXMLPackageValue("body/totalNum","1");
	UnionSetResponseXMLPackageValue("body/abnormalNum","0");

	if ((ret = UnionLocateResponseNewXMLPackage("body/detail",1)) < 0)
	{
		UnionUserErrLog("in msgBufConfRunInfo:: UnionLocateResponseNewXMLPackage[%s]!\n","body/detail");
		return(ret);
	}
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d",pMsgBufHDL->userID);
	if ((ret = UnionSetResponseXMLPackageValue("userID",tmpBuf)) < 0)
	{
		UnionUserErrLog("in msgBufConfRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","userID");
		return(ret);
	}


	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d ��",pMsgBufHDL->groupNumOfMsg);
	if ((ret = UnionSetResponseXMLPackageValue("groupNumOfMsg",tmpBuf)) < 0)
	{
		UnionUserErrLog("in msgBufConfRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","groupNumOfMsg");
		return(ret);
	}

	memset(tmpBuf,0,sizeof(tmpBuf));
	for (i = 0; i < pMsgBufHDL->groupNumOfMsg; i++)
	{
		lenSize += sprintf(maxSizeOfMsg + lenSize,"%ld:",pMsgBufHDL->msgGrp[i].maxSizeOfMsg);
		lenNum += sprintf(maxNumOfMsg + lenNum,"%d:",pMsgBufHDL->msgGrp[i].maxNumOfMsg);
	}
	strcpy(maxSizeOfMsg + lenSize - 1," (�ֽ�)");
	if ((ret = UnionSetResponseXMLPackageValue("maxSizeOfMsg",maxSizeOfMsg)) < 0)
	{
		UnionUserErrLog("in msgBufConfRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","maxSizeOfMsg");
		return(ret);
	}

	strcpy(maxNumOfMsg + lenNum - 1," (��)");
	if ((ret = UnionSetResponseXMLPackageValue("maxNumOfMsg",maxNumOfMsg)) < 0)
	{
		UnionUserErrLog("in msgBufConfRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","maxNumOfMsg");
		return(ret);
	}

	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d ��",pMsgBufHDL->maxStayTime);
	if ((ret = UnionSetResponseXMLPackageValue("maxStayTime",tmpBuf)) < 0)
	{
		UnionUserErrLog("in msgBufConfRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","maxStayTime");
		return(ret);
	}
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d ��",UnionGetNumOfFreeMsgBufPos());
	if ((ret = UnionSetResponseXMLPackageValue("NumOfFreePos",tmpBuf)) < 0)
	{
		UnionUserErrLog("in msgBufConfRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","NumOfFreePos");
		return(ret);
	}

	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d ��",UnionGetNumOfOccupiedMsgBufPos());
	if ((ret = UnionSetResponseXMLPackageValue("NumOfOccupiedPos",tmpBuf)) < 0)
	{
		UnionUserErrLog("in msgBufConfRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","NumOfOccupiedPos");
		return(ret);
	}

	// ��Ϣ״��
	memset(tmpBuf,0,sizeof(tmpBuf));
	// modify by chenqy 20151116
	//if (UnionGetNumOfOccupiedMsgBufPos() > 0)
	if (UnionGetNumOfOccupiedMsgBufPos() > UnionGetNumOfFreeMsgBufPos())
	// modify end
	{
		strcpy(tmpBuf,"�쳣");
		abnormalNum++;
	}
	else
		strcpy(tmpBuf,"����");

	if ((ret = UnionSetResponseXMLPackageValue("infoLevel",tmpBuf)) < 0)
	{
		UnionUserErrLog("in msgBufConfRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","NumOfOccupiedPos");
		return(ret);
	}

	UnionLocateResponseXMLPackage("",0);

	// ��ֵ�쳣��¼��
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d",abnormalNum);
	if ((ret = UnionSetResponseXMLPackageValue("body/abnormalNum",tmpBuf)) < 0)
	{
		UnionUserErrLog("in msgBufConfRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","body/abnormalNum");
		return(ret);
	}

	msgBufConf_abnormalNum = abnormalNum;

	return 0;
}

// ͨѶ��Ϣ
int commConfMonRunInfo()
{
	int			ret = 0;
	int			i = 0;
	int			j = 0;
	char			fieldList[1024+1];
	char			fieldListChnName[2048+1];	
	char			tmpBuf[128+1];
	int			totalNum = 0;
	int			abnormalNum = 0;
	char			infoLevel[4+1];
	time_t			now;
	
	PUnionCommConf		pgunionCommConf = NULL;

	memset(fieldList,0,sizeof(fieldList));
	strcpy(fieldList,"infoLevel,remark,procType,connType,ipAddr,port,connNum,totalNum,freeTime");
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldList",fieldList)) < 0)
	{
		UnionUserErrLog("in commConfMonRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","fieldList");
		return(ret);
	}

	// �����ֶ��嵥������
	memset(fieldListChnName,0,sizeof(fieldListChnName));
	strcpy(fieldListChnName,"��Ϣ״��,��ע,����,����,�Զ�IP��ַ,�˿�,������,����Ŀ,����");
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldListChnName",fieldListChnName)) < 0)
	{
		UnionUserErrLog("in commConfMonRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","fieldListChnName");
		return(ret);
	}

	if ((pgunionCommConf = UnionGetCurrentCommConfGrp()) == NULL)
	{
		UnionUserErrLog("in commConfMonRunInfo:: UnionGetCurrentCommConfGrp!\n");	
		UnionSetResponseRemark("��ȡͨѶ��Ϣʧ��");
		return(errCodeParameter);
	}
	
	UnionSetResponseXMLPackageValue("body/totalNum","0");
	UnionSetResponseXMLPackageValue("body/abnormalNum","0");

	for (i = 0; i < UnionGetMaxNumOfCommConfTBL(); i++)
	{
	
		if (pgunionCommConf == NULL)
		{
			// add by chenqy 20151124
			pgunionCommConf ++;
			// add end
			continue;
		}
		if (strlen(pgunionCommConf->ipAddr) == 0)
		{
			// add by chenqy 20151124
			pgunionCommConf ++;
			// add end 
			continue;
		}

		if ((ret = UnionLocateResponseNewXMLPackage("body/detail",totalNum+1)) < 0)
		{
			UnionUserErrLog("in commConfMonRunInfo:: UnionLocateResponseNewXMLPackage[%s][%d]!\n","body/detail",totalNum+1);
			return(ret);
		}
		
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%s",pgunionCommConf->remark);
		if ((ret = UnionSetResponseXMLPackageValue("remark",tmpBuf)) < 0)
		{
			UnionUserErrLog("in commConfMonRunInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","remark",totalNum+1);
			return(ret);
		}

		memset(tmpBuf,0,sizeof(tmpBuf));
		switch (pgunionCommConf->procType)
		{
			case	conCommServer:
				strcpy(tmpBuf,"������");
				break;
			case	conCommClient:
				strcpy(tmpBuf,"�ͻ���");
				break;
			default:
				strcpy(tmpBuf,"");
				break;
		}
		if ((ret = UnionSetResponseXMLPackageValue("procType",tmpBuf)) < 0)
		{
			UnionUserErrLog("in commConfMonRunInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","procType",totalNum+1);
			return(ret);
		}


		memset(tmpBuf,0,sizeof(tmpBuf));
		switch(pgunionCommConf->connType)
		{
			case	conCommShortConn:
				strcpy(tmpBuf,"������");
				break;
			case	conCommLongConn:
				strcpy(tmpBuf,"������");
				break;
			default:
				strcpy(tmpBuf,"");
				break;
		}
		if ((ret = UnionSetResponseXMLPackageValue("connType",tmpBuf)) < 0)
		{
			UnionUserErrLog("in commConfMonRunInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","connType",totalNum+1);
			return(ret);
		}

		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%s",pgunionCommConf->ipAddr);
		if ((ret = UnionSetResponseXMLPackageValue("ipAddr",tmpBuf)) < 0)
		{
			UnionUserErrLog("in commConfMonRunInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","ipAddr",totalNum+1);
			return(ret);
		}

		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%d",pgunionCommConf->port);
		if ((ret = UnionSetResponseXMLPackageValue("port",tmpBuf)) < 0)
		{
			UnionUserErrLog("in commConfMonRunInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","port",totalNum+1);
			return(ret);
		}

		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%ld",pgunionCommConf->connNum);
		if ((ret = UnionSetResponseXMLPackageValue("connNum",tmpBuf)) < 0)
		{
			UnionUserErrLog("in commConfMonRunInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","connNum",totalNum+1);
			return(ret);
		}

		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%ld",pgunionCommConf->totalNum);
		if ((ret = UnionSetResponseXMLPackageValue("totalNum",tmpBuf)) < 0)
		{
			UnionUserErrLog("in commConfMonRunInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","totalNum",totalNum+1);
			return(ret);
		}

		memset(tmpBuf,0,sizeof(tmpBuf));
		time(&now);
		sprintf(tmpBuf,"%lds",now - pgunionCommConf->lastWorkingTime);

		if ((ret = UnionSetResponseXMLPackageValue("freeTime",tmpBuf)) < 0)
		{
			UnionUserErrLog("in commConfMonRunInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","freeTime",totalNum+1);
			return(ret);
		}

		// ��Ϣ״��
		memset(infoLevel, 0, sizeof(infoLevel));
		if (pgunionCommConf->connType == conCommLongConn)
		{
			if (pgunionCommConf->connNum <= 0)
			{
				abnormalNum++;
				strcpy(infoLevel,"�쳣");
			}
		}

		if (strlen(infoLevel) == 0)
			strcpy(infoLevel,"����");
		if ((ret = UnionSetResponseXMLPackageValue("infoLevel",infoLevel)) < 0)
		{
			UnionUserErrLog("in commConfMonRunInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","infoLevel",totalNum+1);
			return(ret);
		}

		totalNum ++;
		pgunionCommConf ++;

		UnionLocateResponseXMLPackage("",0);
		if (strcmp(infoLevel,"�쳣") == 0 && totalNum != 1) 
		{
			for (j = totalNum; j > abnormalNum; j--)
			{
				if ((ret = UnionExchangeIDResponseXMLPackage("body/detail",j,j-1)) < 0)
				{
					UnionUserErrLog("in commConfMonRunInfo:: UnionExchangeIDResponseXMLPackage[%s][%d][%d]!\n","infoLevel",j,j-1);
				}
			}
		}

	}

	UnionLocateResponseXMLPackage("",0);

	//��ֵ�ܼ�¼��
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d",totalNum);
	if ((ret = UnionSetResponseXMLPackageValue("body/totalNum",tmpBuf)) < 0)
	{
		UnionUserErrLog("in commConfMonRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","body/totalNum");
		return(ret);
	}

	// ��ֵ�쳣��¼��
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d",abnormalNum);
	if ((ret = UnionSetResponseXMLPackageValue("body/abnormalNum",tmpBuf)) < 0)
	{
		UnionUserErrLog("in commConfMonRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","body/abnormalNum");
		return(ret);
	}

	commConfMon_abnormalNum = abnormalNum;

	return 0;
}

// ��������
int msgBufMonRunInfo()
{
	int			i = 0;
	int			ret = 0;
	char			fieldList[1024+1];
	char			fieldListChnName[2048+1];
	char			tmpBuf[128+1];
	int			totalNum = 0;
	time_t			nowTime;
	long			offset;
	int			abnormalNum = 0;
	char			infoLevel[4+1];

	PUnionMsgBufHDL		pMsgBufHDL = NULL;
	unsigned char		*pMsgBuf = NULL;
	PUnionMessageHeader	pmHeader = NULL;

	// �����ֶ��嵥
	memset(fieldList,0,sizeof(fieldList));
	strcpy(fieldList,"msgType,msgIndex,provider,dealer,waitTime,len");
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldList",fieldList)) < 0)
	{
		UnionUserErrLog("in msgBufMonRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","fieldList");
		return(ret);
	}

	// �����ֶ��嵥������
	memset(fieldListChnName,0,sizeof(fieldListChnName));
	strcpy(fieldListChnName,"��Ϣ����,��Ϣ��ʶ,�ṩ��,������,�ȴ�ʱ��,����");
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldListChnName",fieldListChnName)) < 0)
	{
		UnionUserErrLog("in msgBufMonRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","fieldListChnName");
		return(ret);
	}

	if ((pMsgBufHDL = UnionGetCurrentMsgBufHDLGrp()) == NULL)
	{
		UnionUserErrLog("in msgBufMonRunInfo:: UnionGetCurrentMsgBufHDLGrp!\n");
		return(errCodeParameter);
	}
	
	if ((pMsgBuf = UnionGetCurrentMsgBuf()) == NULL)
	{
		UnionUserErrLog("in msgBufMonRunInfo:: UnionGetCurrentMsgBuf!\n");
		return(errCodeParameter);
	}

	UnionSetResponseXMLPackageValue("body/totalNum","0");
	UnionSetResponseXMLPackageValue("body/abnormalNum","0");

	for (i = 0; i < UnionGetAllMaxNumOfMsg(); i++)
	{
		memset(infoLevel,0,sizeof(infoLevel));

		offset = UnionGetOffsetOfMsgBufByIndex(i);

		if ((pmHeader = (PUnionMessageHeader)(pMsgBuf+offset)) == NULL)
			continue;
		if ((pmHeader->type <= 0) || (pmHeader->locked))
			continue;
		
		if ((ret = UnionLocateResponseNewXMLPackage("body/detail",i)) < 0)
		{
			UnionUserErrLog("in msgBufMonRunInfo:: UnionLocateResponseNewXMLPackage[%s][%d]!\n","body/detail",i);
			return(ret);
		}
	
		//��ȡ��Ϣ����
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%ld",pmHeader->type);
		UnionLog("in pmHeader->type[%s]!\n",tmpBuf);
		if ((ret = UnionSetResponseXMLPackageValue("msgType",tmpBuf)) < 0)
		{
			UnionUserErrLog("in msgBufMonRunInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","msgType",i);
			return(ret);
		}

		// ��ȡ��Ϣ��ʶ
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%ld",pmHeader->msgIndex);
		if ((ret = UnionSetResponseXMLPackageValue("msgIndex",tmpBuf)) < 0)
		{
			UnionUserErrLog("in msgBufMonRunInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","msgIndex",i);
			return(ret);
		}

		// ��ȡ�ṩ��
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%d",pmHeader->provider);
		if ((ret = UnionSetResponseXMLPackageValue("provider",tmpBuf)) < 0)
		{
			UnionUserErrLog("in msgBufMonRunInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","provider",i);
			return(ret);
		}


		// ��ȡ������
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%d",pmHeader->dealer);
		if ((ret = UnionSetResponseXMLPackageValue("dealer",tmpBuf)) < 0)
		{
			UnionUserErrLog("in msgBufMonRunInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","dealer",i);
			return(ret);
		}

		// ��ȡ�ȴ�ʱ��
		memset(tmpBuf,0,sizeof(tmpBuf));
		time(&nowTime);
		sprintf(tmpBuf,"%ld",nowTime - pmHeader->time);
		if ((ret = UnionSetResponseXMLPackageValue("waitTime",tmpBuf)) < 0)
		{
			UnionUserErrLog("in msgBufMonRunInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","waitTime",i);
			return(ret);
		}

		// ��ȡ����
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%d",pmHeader->len);
		if ((ret = UnionSetResponseXMLPackageValue("len",tmpBuf)) < 0)
		{
			UnionUserErrLog("in msgBufMonRunInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","len",i);
			return(ret);
		}
		totalNum++;

	}
	
	if (totalNum > 100)
		abnormalNum++;

	UnionLocateResponseXMLPackage("",0);

	//��ֵ�ܼ�¼��
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d",totalNum);
	if ((ret = UnionSetResponseXMLPackageValue("body/totalNum",tmpBuf)) < 0)
	{
		UnionUserErrLog("in msgBufMonRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","body/totalNum");
		return(ret);
	}

	// ��ֵ�쳣��¼��
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d",abnormalNum);
	if ((ret = UnionSetResponseXMLPackageValue("body/abnormalNum",tmpBuf)) < 0)
	{
		UnionUserErrLog("in msgBufMonRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","body/abnormalNum");
		return(ret);
	}

	msgBufMon_abnormalNum = abnormalNum;

	return 0;
}


int highCachedStatus()
{
	char			fieldList[6+1];
	char			fieldListChnName[4+1];
	char			tmpBuf[121288+1];
	char			status[10+1];
	char			key[128+1];
	char			value[128+1];
	char			readValue[128+1];
	int			ret = 0,i = 0;
	char			sql[128+1];
	char			enabled[1+1];
	int			totalNum = 0;

	memset(status,0,sizeof(status));
	//if ((ret = UnionReadIntTypeRECVar("isUseHighCached")) <= 0)
//		strcpy(status,"δʹ��");

	//����Ƿ�����
	memset(tmpBuf,0,sizeof(tmpBuf));
	memset(key,0,sizeof(key));
	UnionGetFullSystemDateTime(tmpBuf);
	sprintf(key,"unionTestCheckHighCachedKey:%s",tmpBuf);
	memset(value,0,sizeof(value));
	sprintf(value,"unionTestCheckHighCachedValue|%s",tmpBuf);
	memset(status,0,sizeof(status));

	memset(sql,0,sizeof(sql));
	sprintf(sql,"select enabled from highCached");

	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in highCachedStatus:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}

	if ((ret = UnionReadXMLPackageValue("totalNum",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in highCachedStatus:: UnionReadXMLPackageValue[%s]!\n","totalNum");
		return(ret);
	}
	else
		totalNum = atoi(tmpBuf);

	for (i = 0; i < totalNum; i++)
	{
		if ((ret = UnionLocateXMLPackage("detail", 1)) < 0)
		{
			UnionUserErrLog("in highCachedStatus:: UnionLocateXMLPackage[%s]!\n","detail");
			return(ret);
		}

		if ((ret = UnionReadXMLPackageValue("enabled",enabled,sizeof(enabled))) < 0)
		{
			UnionUserErrLog("in highCachedStatus:: UnionReadXMLPackageValue[%s]!\n","enabled");
			return(ret);
		}
		
		if (enabled[0] == '1')	
			break;
	}

	if (i == totalNum)
	{
		strcpy(status,"δ����");
		goto highCachedEnd;
	}

	if ((ret = UnionExistHighCachedValue(key)) < 0)
	{
		UnionUserErrLog("in highCachedStatus:: UnionExistHighCachedValue [%s]!\n",key);
		goto highCachedEnd;
		strcpy(status,"�쳣");
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in highCachedStatus:: UnionExistHighCachedValue [%s]!\n",key);
		goto highCachedEnd;
		strcpy(status,"δ����");
	}

	// ����
	if ((ret = UnionSetHighCachedValue(key,value,sizeof(value),0)) < 0)
	{
		UnionUserErrLog("in highCachedStatus:: UnionSetHighCachedValue error key[%s] value[%s]!\n",key,value);
		goto highCachedEnd;
		strcpy(status,"�쳣");
	}

	// ��ȡ
	memset(readValue,0,sizeof(readValue));
	if ((ret = UnionGetHighCachedValue(key,readValue,sizeof(readValue))) < 0)
	{
		UnionUserErrLog("in highCachedStatus:: UnionGetHighCachedValue error key[%s]!\n",key);
		strcpy(status,"�쳣");
	}

	if (strcmp(readValue,value) != 0)
	{
		UnionUserErrLog("in highCachedStatus::  key[%s]  value[%s]  != readValue[%s]!\n",key,value,readValue);
		strcpy(status,"�쳣");
	}
	else
		strcpy(status,"����");
		
	// ɾ��
	if ((ret = UnionDeleteHighCachedValue(key)) < 0)
	{
		UnionUserErrLog("in highCachedStatus:: UnionDeleteHighCachedValue error key[%s]!\n",key);
		strcpy(status,"�쳣");
	}
	else
		strcpy(status,"����");
	

highCachedEnd:
	// �����ֶ��嵥
	memset(fieldList,0,sizeof(fieldList));
	strcpy(fieldList,"status");
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldList",fieldList)) < 0)
	{
		UnionUserErrLog("in highCachedStatus:: UnionSetResponseXMLPackageValue[%s]!\n","fieldList");
		return(ret);
	}

	// �����ֶ��嵥������
	memset(fieldListChnName,0,sizeof(fieldListChnName));
	strcpy(fieldListChnName,"״̬");
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldListChnName",fieldListChnName)) < 0)
	{
		UnionUserErrLog("in highCachedStatus:: UnionSetResponseXMLPackageValue[%s]!\n","fieldListChnName");
		return(ret);
	}

	UnionSetResponseXMLPackageValue("body/totalNum","1");
	if (strcmp(status,"�쳣") == 0)
		UnionSetResponseXMLPackageValue("body/abnormalNum","1");
	else
		UnionSetResponseXMLPackageValue("body/abnormalNum","0");

	if ((ret = UnionLocateResponseNewXMLPackage("body/detail",1)) < 0)
	{
		UnionUserErrLog("in highCachedStatus:: UnionLocateResponseNewXMLPackage[%s][%d]!\n","body/detail",1);
		return(ret);
	}

	if ((ret = UnionSetResponseXMLPackageValue("status",status)) < 0)
	{
		UnionUserErrLog("in highCachedRunInfo:: UnionSetResponseXMLPackageValue[%s][%d]!\n","ipAddr",i+1);
		return(ret);
	}


	return 0;
}

// �쳣��Ϣ
int abnormalRunInfo()
{
/*	int			ret = 0;
	int			i = 0;
	char			fieldList[1024+1];
	char			fieldListChnName[2048+1];
	int			totalNum = 0;
	int			tmpNum = 0;
	char			tmpBuf[512+1];
	char			hsmID[16+1];
	char			sql[1024+1];
	int			maxClassNum = 0;
	FILE			*fp;
	char			hardValue[16][128+1];
	long			cpuTotal1,cpuTotal2;
	float			cpuPercent;
	long			sizeOfPerMsg;

	struct statfs		diskInfo;
	unsigned long long	blockSize,totalSize,dTotalSize;
	unsigned long long	freeSize,dFreeSize;
	unsigned long long	usedSize,dUsedSize;	
	int			percent  = 0;
	struct mntent		*mountDevice;

	PUnionTaskClass		pTaskClass = NULL;
	PUnionCommConf		pgunionCommConf = NULL;
	PUnionMsgBufHDL		pMsgBufHDL = NULL;
	unsigned char		*pMsgBuf = NULL;
	PUnionMessageHeader	pmHeader = NULL;
	
	// �����ֶ��嵥
	memset(fieldList,0,sizeof(fieldList));
	strcpy(fieldList,"abnormalModule,abnormalInfo");
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldList",fieldList)) < 0)
	{
		UnionUserErrLog("in abnormalRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","fieldList");
		return(ret);
	}

	// �����ֶ��嵥������
	memset(fieldListChnName,0,sizeof(fieldListChnName));
	strcpy(fieldListChnName,"�쳣ģ��,�쳣��Ϣ");
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldListChnName",fieldListChnName)) < 0)
	{
		UnionUserErrLog("in abnormalRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","fieldListChnName");
		return(ret);
	}

	UnionSetResponseXMLPackageValue("body/totalNum","0");
	UnionSetResponseXMLPackageValue("body/abnormalNum","0");

	// �쳣������Ϣ
	if ((pTaskClass = UnionGetCurrentTaskClassGrp()) == NULL)
	{
		UnionUserErrLog("in abnormalRunInfo:: UnionGetCurrentTaskClassGrp!\n");
		UnionSetResponseRemark("��ȡ������Ϣʧ��");
		return(errCodeParameter);
	}
	
	if ((maxClassNum = UnionGetMaxNumOfTaskClass()) <= 0)
	{
		UnionUserErrLog("in abnormalRunInfo:: UnionGetMaxNumOfTaskClass [%d]\n",maxClassNum);
		return(maxClassNum);
	}

	for (i = 0; i < maxClassNum; i++)
	{
		if ((pTaskClass + i) == NULL)
			continue;	
		if (strlen((pTaskClass + i)->name) == 0)
			continue;	
		if ((memcmp((pTaskClass + i)->name, "client", 6) == 0) || (memcmp((pTaskClass + i)->name, "defaultTask", 11) == 0))
			continue;

		// �쳣���
		if ((pTaskClass + i)->minNum != (pTaskClass + i)->currentNum) 
		{
			totalNum++;

			memset(tmpBuf,0,sizeof(tmpBuf));
			sprintf(tmpBuf,"��������%s    �쳣����ǰ������[%d] != ���������[%d]",(pTaskClass + i)->name,(pTaskClass + i)->currentNum,(pTaskClass + i)->minNum); 

			if ((ret = UnionLocateResponseNewXMLPackage("body/detail", totalNum)) < 0)
			{
				UnionUserErrLog("in abnormalRunInfo:: UnionLocateResponseNewXMLPackage taskMon[%d]!\n",totalNum);
				return(ret);
			}

			// �쳣ģ��
			if ((ret = UnionSetResponseXMLPackageValue("abnormalModule","����")) < 0)
			{
				UnionUserErrLog("in abnormalRunInfo:: UnionSetResponseXMLPackageValue taskMon[%s][%d]!\n","abnormalModule",totalNum);
				return(ret);
			}

			// �쳣��Ϣ
			if ((ret = UnionSetResponseXMLPackageValue("abnormalInfo",tmpBuf)) < 0)
			{
				UnionUserErrLog("in abnormalRunInfo:: UnionSetResponseXMLPackageValue taskMon[%s][%d]!\n","abnormalInfo",totalNum);
				return(ret);
			}
		}
	}

	// �쳣ͨѶ��Ϣ
	if ((pgunionCommConf = UnionGetCurrentCommConfGrp()) == NULL)
	{
		UnionUserErrLog("in abnormalRunInfo:: UnionGetCurrentCommConfGrp!\n");	
		UnionSetResponseRemark("��ȡͨѶ��Ϣʧ��");
		return(errCodeParameter);
	}

	for (i = 0; i < UnionGetMaxNumOfCommConfTBL(); i++)
	{
		if (pgunionCommConf == NULL)
			continue;
		if (strlen(pgunionCommConf->ipAddr) == 0)
			continue;

		// �쳣���
		if ((pgunionCommConf->connType == conCommLongConn) && (pgunionCommConf->connNum == 0)) 
		{
			totalNum++;

			memset(tmpBuf,0,sizeof(tmpBuf));
			sprintf(tmpBuf,"IP��%s    ������Ϊ0",pgunionCommConf->ipAddr); 

			if ((ret = UnionLocateResponseNewXMLPackage("body/detail", totalNum)) < 0)
			{
				UnionUserErrLog("in abnormalRunInfo:: UnionLocateResponseNewXMLPackage commConfMon[%d]!\n",totalNum);
				return(ret);
			}

			// �쳣ģ��
			if ((ret = UnionSetResponseXMLPackageValue("abnormalModule","ͨѶ")) < 0)
			{
				UnionUserErrLog("in abnormalRunInfo:: UnionSetResponseXMLPackageValue commConfMon[%s][%d]!\n","abnormalModule",totalNum);
				return(ret);
			}

			// �쳣��Ϣ
			if ((ret = UnionSetResponseXMLPackageValue("abnormalInfo",tmpBuf)) < 0)
			{
				UnionUserErrLog("in abnormalRunInfo:: UnionSetResponseXMLPackageValue commConfMon[%s][%d]!\n","abnormalInfo",totalNum);
				return(ret);
			}
		}
		pgunionCommConf++;
	}

	// �쳣���ܻ���Ϣ
	// ��ȡ�������
	memset(sql,0,sizeof(sql));
	strcpy(sql,"select hsm.hsmID,hsm.hsmName,hsm.hsmGroupID,hsm.ipAddr,hsm.port,hsm.uses,hsm.status,hsm.enabled,hsmGroup.reqCmdOfTest,hsmGroup.resCmdOfTest from hsm left join hsmGroup on hsm.hsmGroupID = hsmGroup.hsmGroupID order by hsm.hsmGroupID,hsm.hsmID");
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in abnormalRunInfo:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}

	// ��ȡ����
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("totalNum",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in abnormalRunInfo:: UnionReadXMLPackageValue hsm[%s]!\n","totalNum");
		return(ret);
	}
	else
		tmpNum = atoi(tmpBuf);

	for (i = 1; i <= tmpNum; i++)
	{
		if ((ret = UnionLocateXMLPackage("detail",i)) < 0)
		{
			UnionUserErrLog("in abnormalRunInfo:: UnionLocateXMLPackage hsm[%s][%d]!\n","detail",i);
			return(ret);
		}
		
		//��ȡ�����ID
		memset(hsmID,0,sizeof(hsmID));
		if ((ret = UnionReadXMLPackageValue("hsmID",hsmID,sizeof(hsmID))) < 0)
		{
			UnionUserErrLog("in abnormalRunInfo:: UnionReadXMLPackageValue hsm[%s][%d]!\n","hsmID",i);
			return(ret);
		}
		
		// ��ȡ�����״̬
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("status",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in abnormalRunInfo:: UnionReadXMLPackageValue hsm[%s][%d]!\n","status",i);
			return(ret);
		}
		
		if (atoi(tmpBuf) == 0)
		{
			totalNum++;

			memset(tmpBuf,0,sizeof(tmpBuf));
			sprintf(tmpBuf,"���ܻ�ID��%s    ״̬�쳣",hsmID);

			if ((ret = UnionLocateResponseNewXMLPackage("body/detail", totalNum)) < 0)
			{
				UnionUserErrLog("in abnormalRunInfo:: UnionLocateResponseNewXMLPackage hsm[%d]!\n",totalNum);
				return(ret);
			}

			// �쳣ģ��
			if ((ret = UnionSetResponseXMLPackageValue("abnormalModule","���ܻ�")) < 0)
			{
				UnionUserErrLog("in abnormalRunInfo:: UnionSetResponseXMLPackageValue hsm[%s][%d]!\n","abnormalModule",totalNum);
				return(ret);
			}

			// �쳣��Ϣ
			if ((ret = UnionSetResponseXMLPackageValue("abnormalInfo",tmpBuf)) < 0)
			{
				UnionUserErrLog("in abnormalRunInfo:: UnionSetResponseXMLPackageValue hsm[%s][%d]!\n","abnormalInfo",totalNum);
				return(ret);
			}
		}
	}

	// ����������Ϣ
	if ((pMsgBufHDL = UnionGetCurrentMsgBufHDLGrp()) == NULL)
	{
		UnionUserErrLog("in abnormalRunInfo:: UnionGetCurrentMsgBufHDLGrp msgBufConf!\n");
		UnionSetResponseRemark("��ȡ����������Ϣʧ��");
		return(errCodeParameter);
	}
	
	tmpNum = UnionGetNumOfOccupiedMsgBufPos();

	if (tmpNum > 0)
	{
		totalNum++; 

		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"�û�ID��%d    �쳣��ռ��λ�õ�����[%d] > 0",pMsgBufHDL->userID,tmpNum);

		if ((ret = UnionLocateResponseNewXMLPackage("body/detail", totalNum)) < 0)
		{
			UnionUserErrLog("in abnormalRunInfo:: UnionLocateResponseNewXMLPackage msgBufConf[%d]!\n",totalNum);
			return(ret);
		}

		// �쳣ģ��
		if ((ret = UnionSetResponseXMLPackageValue("abnormalModule","����������Ϣ")) < 0)
		{
			UnionUserErrLog("in abnormalRunInfo:: UnionSetResponseXMLPackageValue msgBufConf[%s][%d]!\n","abnormalModule",totalNum);
			return(ret);
		}

		// �쳣��Ϣ
		if ((ret = UnionSetResponseXMLPackageValue("abnormalInfo",tmpBuf)) < 0)
		{
			UnionUserErrLog("in abnormalRunInfo:: UnionSetResponseXMLPackageValue msgBufConf[%s][%d]!\n","abnormalInfo",totalNum);
			return(ret);
		}
	}
	
	//�����������쳣��Ϣ
	if ((pMsgBufHDL = UnionGetCurrentMsgBufHDLGrp()) == NULL)
	{
		UnionUserErrLog("in abnormalRunInfo:: UnionGetCurrentMsgBufHDLGrp msgBufMon!\n");
		return(errCodeParameter);
	}
	
	if ((pMsgBuf = UnionGetCurrentMsgBuf()) == NULL)
	{
		UnionUserErrLog("in abnormalRunInfo:: UnionGetCurrentMsgBuf msgBufMon!\n");
		return(errCodeParameter);
	}

	sizeOfPerMsg = sizeof(*pmHeader) + pMsgBufHDL->maxSizeOfMsg;

	tmpNum = 0;
	for (i = 1; i <= pMsgBufHDL->maxNumOfMsg; i++)
	{
		if ((pmHeader = (PUnionMessageHeader)(pMsgBuf + sizeOfPerMsg * i)) == NULL)
			continue;
		if ((pmHeader->type <= 0) || (pmHeader->locked))
			continue;
		tmpNum++;
	}

	if (tmpNum > 100)
	{
		totalNum++; 

		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"������������[%d]̫��",tmpNum);

		if ((ret = UnionLocateResponseNewXMLPackage("body/detail", totalNum)) < 0)
		{
			UnionUserErrLog("in abnormalRunInfo:: UnionLocateResponseNewXMLPackage msgBufMon[%d]!\n",totalNum);
			return(ret);
		}

		// �쳣ģ��
		if ((ret = UnionSetResponseXMLPackageValue("abnormalModule","��������")) < 0)
		{
			UnionUserErrLog("in abnormalRunInfo:: UnionSetResponseXMLPackageValue msgBufMon[%s][%d]!\n","abnormalModule",totalNum);
			return(ret);
		}

		// �쳣��Ϣ
		if ((ret = UnionSetResponseXMLPackageValue("abnormalInfo",tmpBuf)) < 0)
		{
			UnionUserErrLog("in abnormalRunInfo:: UnionSetResponseXMLPackageValue msgBufMon[%s][%d]!\n","abnormalInfo",totalNum);
			return(ret);
		}
	}

	// Ӳ���쳣��Ϣ
	// Ӳ����Ϣ
//#if ( defined __linux__ ) || ( defined __hpux )
#if ( defined __linux__ )
	if ((fp = setmntent("/etc/mtab","r")) == NULL)
	{
		UnionUserErrLog("in abnormalRunInfo:: setmntent[/etc/mtab]!\n");	
		return(errCodeUseOSErrCode);
	}

	while((mountDevice = getmntent(fp)))
	{
		if ((strcmp(mountDevice->mnt_fsname,"rootfs") == 0) || (strcmp(mountDevice->mnt_fsname,"udev") == 0))
			continue;

		if ((ret = statfs(mountDevice->mnt_dir,&diskInfo)) < 0)
		{
			UnionUserErrLog("in abnormalRunInfo:: statfs[%s] error!\n",mountDevice->mnt_dir);
		//	return(ret);
		}
		
		if ((diskInfo.f_blocks > 0))
		{
			// �ܿռ��С
			blockSize = diskInfo.f_bsize;
			totalSize = blockSize * diskInfo.f_blocks;
			dTotalSize = totalSize / 1024;
		
			// ���ÿռ��С
			freeSize = blockSize * diskInfo.f_bavail;
			dFreeSize = freeSize / 1024;
			
			// ���ÿռ��С
			usedSize = totalSize - (blockSize * diskInfo.f_bfree);
			dUsedSize = usedSize / 1024;
		
			// ���ÿռ�ռ�ðٷֱ�
			percent = (diskInfo.f_blocks - diskInfo.f_bfree) * 100 /(diskInfo.f_blocks - diskInfo.f_bfree + diskInfo.f_bavail) +1;

			UnionLog("in Ӳ��:[%d%%]!\n",percent);
			if (percent >= 90)
			{
				totalNum++;

				memset(tmpBuf,0,sizeof(tmpBuf));
				sprintf(tmpBuf,"Ӳ�̣�%s   ��ʹ�ðٷֱ�[%d%%]",mountDevice->mnt_fsname,percent);

				if ((ret = UnionLocateResponseNewXMLPackage("body/detail", totalNum)) < 0)
				{
					UnionUserErrLog("in abnormalRunInfo:: UnionLocateResponseNewXMLPackage hardware!\n");
					return(ret);
				}

				// �쳣ģ��
				if ((ret = UnionSetResponseXMLPackageValue("abnormalModule","Ӳ��")) < 0)
				{
					UnionUserErrLog("in abnormalRunInfo:: UnionSetResponseXMLPackageValue hardware[%s][%d]!\n","abnormalModule",totalNum);
					return(ret);
				}

				// �쳣��Ϣ
				if ((ret = UnionSetResponseXMLPackageValue("abnormalInfo",tmpBuf)) < 0)
				{
					UnionUserErrLog("in abnormalRunInfo:: UnionSetResponseXMLPackageValue hardware[%s][%d]!\n","abnormalInfo",totalNum);
					return(ret);
				}
			}
		}
	} 
	endmntent(fp);
#endif
	// �ڴ���Ϣ	
	if ((fp = fopen("/proc/meminfo","r")) == NULL)
	{
		UnionUserErrLog("in abnormalRunInfo:: fopen[/proc/meminfo] error!\n");
		return(errCodeUseOSErrCode);
	}

	i = 0;
	memset(hardValue[4],0,sizeof(hardValue[4]));
	while(!feof(fp))
	{
		memset(hardValue[i],0,sizeof(hardValue[i]));
		fscanf(fp,"%s%s%s",hardValue[4],hardValue[i],hardValue[4]);
		i++;
		if (i == 2)
			break;
	}
	fclose(fp);

	//percent = 100 * (atoi(hardValue[0]) - atoi(hardValue[1]) - atoi(hardValue[2]) - atoi(hardValue[3]))/atoi(hardValue[0]);
	percent = 100 * (atoi(hardValue[0]) - atoi(hardValue[1])) / atoi(hardValue[0]);
	UnionLog("in �ڴ�:[%d%%]!\n",percent);
	if (percent > 80)
	{
		totalNum++;
					
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"�ڴ�: ��ʹ�ðٷֱ�[%d%%]",percent);

		if ((ret = UnionLocateResponseNewXMLPackage("body/detail", totalNum)) < 0)
		{
			UnionUserErrLog("in abnormalRunInfo:: UnionLocateResponseNewXMLPackage hardware[%d]!\n",totalNum);
			return(ret);
		}

		// �쳣ģ��
		if ((ret = UnionSetResponseXMLPackageValue("abnormalModule","Ӳ��")) < 0)
		{
			UnionUserErrLog("in abnormalRunInfo:: UnionSetResponseXMLPackageValue hardware[%s][%d]!\n","abnormalModule",totalNum);
			return(ret);
		}

		// �쳣��Ϣ
		if ((ret = UnionSetResponseXMLPackageValue("abnormalInfo",tmpBuf)) < 0)
		{
			UnionUserErrLog("in abnormalRunInfo:: UnionSetResponseXMLPackageValue hardware[%s][%d]!\n","abnormalInfo",totalNum);
			return(ret);
		}
	}

	// CPU��Ϣ
	// ��һ�λ�ȡ
	if ((fp = fopen("/proc/stat","r")) == NULL)
	{
		UnionUserErrLog("in abnormalRunInfo:: fopen[/proc/stat] error!\n");
		return(errCodeUseOSErrCode);
	}
	
	memset(hardValue, 0, sizeof(hardValue));
	fscanf(fp,"%s%s%s%s%s%s%s%s",hardValue[0],hardValue[1],hardValue[2],hardValue[3],hardValue[4],hardValue[5],hardValue[6],hardValue[7]);
	fclose(fp);
	
	usleep(50000);

	// �ڶ��λ�ȡ
	if ((fp = fopen("/proc/stat","r")) == NULL)
	{
		UnionUserErrLog("in abnormalRunInfo:: fopen[/proc/stat] error!\n");
		return(errCodeUseOSErrCode);
	}
	
	fscanf(fp,"%s%s%s%s%s%s%s%s",hardValue[0],hardValue[8],hardValue[9],hardValue[10],hardValue[11],hardValue[12],hardValue[13],hardValue[14]);

	fclose(fp);
	
	// ��һ�λ�ȡCPU��ʱ��
	cpuTotal1 = atol(hardValue[1]) + atol(hardValue[2]) + atol(hardValue[3]) + atol(hardValue[4]) + atol(hardValue[5]) + atol(hardValue[6]) + atol(hardValue[7]);

	// �ڶ��λ�ȡCPU��ʱ��
	cpuTotal2 = atol(hardValue[8]) + atol(hardValue[9]) + atol(hardValue[10]) + atol(hardValue[11]) + atol(hardValue[12]) + atol(hardValue[13]) + atol(hardValue[14]);

	// CPUռ�ðٷֱ�
	cpuPercent = 100.0 - 100.0 * (atol(hardValue[11])  - atol(hardValue[4])) / (cpuTotal2 - cpuTotal1); 	
	
	if (cpuPercent > 60.0)
	{
		totalNum++;
					
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"CPUռ����:��ʹ��[%.1f%%]",cpuPercent);

		if ((ret = UnionLocateResponseNewXMLPackage("body/detail", totalNum)) < 0)
		{
			UnionUserErrLog("in abnormalRunInfo:: UnionLocateResponseNewXMLPackage hardware[%d]!\n",totalNum);
			return(ret);
		}

		// �쳣ģ��
		if ((ret = UnionSetResponseXMLPackageValue("abnormalModule","Ӳ��")) < 0)
		{
			UnionUserErrLog("in abnormalRunInfo:: UnionSetResponseXMLPackageValue hardware[%s][%d]!\n","abnormalModule",totalNum);
			return(ret);
		}

		// �쳣��Ϣ
		if ((ret = UnionSetResponseXMLPackageValue("abnormalInfo",tmpBuf)) < 0)
		{
			UnionUserErrLog("in abnormalRunInfo:: UnionSetResponseXMLPackageValue hardware[%s][%d]!\n","abnormalInfo",totalNum);
			return(ret);
		}
	}
	UnionLog("in CPU:[%.1f%%]!\n",cpuPercent);

	UnionLocateResponseXMLPackage("",0);

	//��ֵ�ܼ�¼��
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d",totalNum);
	if ((ret = UnionSetResponseXMLPackageValue("body/totalNum",tmpBuf)) < 0)
	{
		UnionUserErrLog("in abnormalRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","body/totalNum");
		return(ret);
	}
*/
	return 0;
}


// Ӳ����Ϣ
int hardwareRunInfo()
{
	int			ret = 0;
	int			i = 0;
	char			fieldList[1024+1];
	char			fieldListChnName[2048+1];
	int			totalNum = 0;
	int			percent  = 0;
	char			tmpBuf[512+1];
	int			len = 0;
	char			hardInfo[2048+1];
	char			hardValue[16][128+1];
	FILE			*fp;
	char			*ptrLine = NULL;
	char			*database = NULL;
	int			abnormalNum = 0;
	char			infoLevel[4+1];
	long			dTotalSize = 0;
	long			freeSize = 0;
	long			dUsedSize = 0;
	char			tmpSwap[512+1];

#if ( defined __linux__ ) || ( defined __hpux )  
	struct statfs		diskInfo;
	long			blockSize = 0,totalSize = 0;
	long			dFreeSize = 0;
	long			usedSize = 0;
	long			cpuTotal1 = 0,cpuTotal2 = 0;
	float			cpuPercent = 0;
	struct mntent		*mountDevice;
	char			cpuInfo[128+1];
	int			modelFlag = 0;
	int			coreFlag = 0;
	int			phyFlag = 0;
#elif ( defined _AIX )
	char			tmpNum[32+1];
	char			tmpNum1[10+1];
	char			tmpNum2[10+1];
#endif

#if ( defined __hpux )
	char			tmpDir[128];
	char			tmpSize[128];
	char			tmpTotalSize[16];
	char			tmpFreeSize[16];
#endif

	// �����ֶ��嵥
	memset(fieldList,0,sizeof(fieldList));
	strcpy(fieldList,"hardwareType,hardwareInfo,total,used,infoLevel");
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldList",fieldList)) < 0)
	{
		UnionUserErrLog("in hardwareRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","body/fieldList");
		return(ret);
	}

	// �����ֶ��嵥������
	memset(fieldListChnName,0,sizeof(fieldListChnName));
	strcpy(fieldListChnName,"Ӳ������,Ӳ����Ϣ,��С,����,��Ϣ״��");
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldListChnName",fieldListChnName)) < 0)
	{
		UnionUserErrLog("in hardwareRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","body/fieldListChnName");
		return(ret);
	}

	UnionSetResponseXMLPackageValue("body/totalNum","0");
	UnionSetResponseXMLPackageValue("body/abnormalNum","0");

	// ϵͳ��Ϣ
//#if ( defined __linux__ ) || ( defined __hpux )
#if ( defined __linux__ )
	if ((fp = popen("lsb_release -d 2>/dev/null","r")) == NULL)
	{
		UnionLog("in hardwareRunInfo:: popen[lsb_release -d] failed!\n");
		if((fp = popen("cat /etc/issue", "r")) == NULL)
		{
			UnionLog("in hardwareRunInfo:: popen[cat /etc/issue] failed!\n");
			return(errCodeUseOSErrCode);
		}
		else
		{
			memset(hardInfo,0,sizeof(hardInfo));
			while(!feof(fp))
			{
				if ((ptrLine = fgets(hardInfo,sizeof(hardInfo),fp)) == NULL)
				{
					UnionLog("in hardwareRunInfo:: fgets OS\n");
					strcpy(tmpBuf,"ʧ��");
					break;
				}
				UnionLog("in hardwareRunInfo:: hardInfo[%s]!\n",hardInfo);
		
				if ((ptrLine = strstr(hardInfo,"Neokylin")) == NULL)
				{
					UnionLog("in hardwareRunInfo:: Get system info failed!\n");
					break;
				}	
				else
				{
					memset(tmpBuf,0,sizeof(tmpBuf));
					strcpy(tmpBuf,ptrLine);
					UnionFilterHeadAndTailBlank(tmpBuf);
					break;
				}
			}
			pclose(fp);
		}
	}
	else
	{
		memset(hardInfo,0,sizeof(hardInfo));
		while(!feof(fp))
		{
			if ((ptrLine = fgets(hardInfo,sizeof(hardInfo),fp)) == NULL)
			{
				UnionLog("in hardwareRunInfo:: fgets OS\n");
				strcpy(tmpBuf,"ʧ��");
				break;
			}
			UnionLog("in hardwareRunInfo:: hardInfo[%s]!\n",hardInfo);
	
			if ((ptrLine = strstr(hardInfo,"Description:")) == NULL)
			{
				UnionLog("in hardwareRunInfo:: Get system info failed!\n");
				break;
			}	
			else
			{
				memset(tmpBuf,0,sizeof(tmpBuf));
				strcpy(tmpBuf,ptrLine + strlen("Description:"));
				UnionFilterHeadAndTailBlank(tmpBuf);
				break;
			}
		}
		pclose(fp);
	}
#elif ( defined __hpux )
	if ((fp = popen("machinfo|grep Release","r")) == NULL)
	{
		UnionLog("in hardwareRunInfo:: popen[machinfo] failed!\n");
		return(errCodeUseOSErrCode);
	}
	memset(hardInfo,0,sizeof(hardInfo));
	while(!feof(fp))
	{
		if ((ptrLine = fgets(hardInfo,sizeof(hardInfo),fp)) == NULL)
		{
			UnionLog("in hardwareRunInfo:: fgets OS\n");
			strcpy(tmpBuf,"ʧ��");
			break;
		}
		UnionLog("in hardwareRunInfo:: hardInfo[%s]!\n",hardInfo);

		if ((ptrLine = strstr(hardInfo,"Release:")) == NULL)
                {
                        UnionLog("in hardwareRunInfo:: Get system info failed!\n");
                        break;
                }
		else
                {
                        memset(tmpBuf,0,sizeof(tmpBuf));
                        strcpy(tmpBuf,ptrLine + strlen("Release:"));
                        UnionFilterHeadAndTailBlank(tmpBuf);
                        break;
                }
	}
	pclose(fp);
#elif ( defined _AIX )
	if ((fp = popen("uname -M","r")) == NULL)
	{
		UnionLog("in hardwareRunInfo:: popen[uname -M] failed!\n");
		return(errCodeUseOSErrCode);
	}
	memset(hardInfo,0,sizeof(hardInfo));
	while(!feof(fp))
	{
		if ((ptrLine = fgets(hardInfo,sizeof(hardInfo),fp)) == NULL)
		{
			UnionLog("in hardwareRunInfo:: fgets OS\n");
			strcpy(tmpBuf,"ʧ��");
			break;
		}
		UnionLog("in hardwareRunInfo:: hardInfo[%s]!\n",hardInfo);

		len = sprintf(tmpBuf,"%s",ptrLine);
		tmpBuf[len-1] = 0;
		UnionFilterHeadAndTailBlank(tmpBuf);
		break;
	}
	pclose(fp);
#endif
	totalNum++;
	if ((ret = UnionLocateResponseNewXMLPackage("body/detail",totalNum)) < 0)
	{
		UnionUserErrLog("in hardwareRunInfo:: UnionLocateResponseNewXMLPackage OS[%s][%d]!\n","body/detail",totalNum);	
		return(ret);
	}

	// Ӳ������
	if ((ret = UnionSetResponseXMLPackageValue("hardwareType","����ϵͳ")) < 0)
	{
		UnionUserErrLog("in hardwareRunInfo:: UnionSetResponseXMLPackageValue OS[%s][%d]!\n","hardwareType",totalNum);
		return(ret);
	}

	// Ӳ����Ϣ
	if ((ret = UnionSetResponseXMLPackageValue("hardwareInfo",tmpBuf)) < 0)
	{
		UnionUserErrLog("in hardwareRunInfo:: UnionSetResponseXMLPackageValue OS[%s][%d]!\n","hardwareInfo",totalNum);
		return(ret);
	}

	// ��С	
	if ((ret = UnionSetResponseXMLPackageValue("total","")) < 0)
	{
		UnionUserErrLog("in hardwareRunInfo:: UnionSetResponseXMLPackageValue cpu[%s][%d]!\n","total",totalNum);
		return(ret);
	}

	// ����
	if ((ret = UnionSetResponseXMLPackageValue("used","")) < 0)
	{
		UnionUserErrLog("in hardwareRunInfo:: UnionSetResponseXMLPackageValue cpu[%s][%d]!\n","used",totalNum);
		return(ret);
	}

	// ��Ϣ״��
	memset(infoLevel,0,sizeof(infoLevel));
	if (strcmp(tmpBuf,"ʧ��") == 0)
		strcpy(infoLevel,"�쳣");
	else
		strcpy(infoLevel,"����");

	if ((ret = UnionSetResponseXMLPackageValue("infoLevel",infoLevel)) < 0)
	{
		UnionUserErrLog("in hardwareRunInfo:: UnionSetResponseXMLPackageValue OS[%s][%d]!\n","infoLevel",totalNum);
		return(ret);
	}
	
	// ���ݿ�����
	totalNum++;
	if ((ret = UnionLocateResponseNewXMLPackage("body/detail",totalNum)) < 0)
	{
		UnionUserErrLog("in hardwareRunInfo:: UnionLocateResponseNewXMLPackage DB[%s][%d]!\n","body/detail",totalNum);	
		return(ret);
	}

	// Ӳ������ 
	if ((ret = UnionSetResponseXMLPackageValue("hardwareType","���ݿ�")) < 0)
	{
		UnionUserErrLog("in hardwareRunInfo:: UnionSetResponseXMLPackageValue DB[%s][%d]!\n","hardwareType",totalNum);
		return(ret);
	}

	// ��ȡ���ݿ�
	memset(hardInfo,0,sizeof(hardInfo));
	memset(tmpBuf,0,sizeof(tmpBuf));
	database = UnionGetDataBaseType();
	UnionReadDirFromStr("$DBNAME",-1,tmpBuf);
	sprintf(hardInfo,"%s    ���ݿ���:%s",database,tmpBuf);
	if (strcasecmp(database,"ORACLE") == 0)
	{
		UnionReadDirFromStr("$DBUSER",-1,tmpBuf);
		sprintf(hardInfo+strlen(hardInfo),"    �û���:%s",tmpBuf);
	}

	// Ӳ����Ϣ
	if ((ret = UnionSetResponseXMLPackageValue("hardwareInfo",hardInfo)) < 0)
	{
		UnionUserErrLog("in hardwareRunInfo:: UnionSetResponseXMLPackageValue DB[%s][%d]!\n","hardwareInfo",totalNum);
		return(ret);
	}

	// ��С
	if ((ret = UnionSetResponseXMLPackageValue("total","")) < 0)
	{
		UnionUserErrLog("in hardwareRunInfo:: UnionSetResponseXMLPackageValue cpu[%s][%d]!\n","total",totalNum);
		return(ret);
	}

	// ����
	if ((ret = UnionSetResponseXMLPackageValue("used","")) < 0)
	{
		UnionUserErrLog("in hardwareRunInfo:: UnionSetResponseXMLPackageValue cpu[%s][%d]!\n","used",totalNum);
		return(ret);
	}
	
	// ��Ϣ״��
	memset(infoLevel,0,sizeof(infoLevel));
	if (database == NULL)	
	{
		strcpy(infoLevel,"����");
		abnormalNum++;
	}
	else
		strcpy(infoLevel,"����");

	if ((ret = UnionSetResponseXMLPackageValue("infoLevel",infoLevel)) < 0)
	{
		UnionUserErrLog("in hardwareRunInfo:: UnionSetResponseXMLPackageValue DB[%s][%d]!\n","infoLevel",totalNum);
		return(ret);
	}

	// Ӳ����Ϣ
//#if ( defined __linux__ ) || ( defined __hpux )
#if ( defined __linux__ )
	if ((fp = setmntent("/etc/mtab","r")) == NULL)
	{
		UnionUserErrLog("in hardwareRunInfo:: setmntent[/etc/mtab]!\n");	
		return(errCodeUseOSErrCode);
	}

	len = sprintf(hardInfo,"���ص�: ");
	
	while((mountDevice = getmntent(fp)))
	{
		if ((strcmp(mountDevice->mnt_fsname,"rootfs") == 0) || (strcmp(mountDevice->mnt_fsname,"udev") == 0))
			continue;

		if ((ret = statfs(mountDevice->mnt_dir,&diskInfo)) < 0)
		{
			UnionUserErrLog("in hardwareRunInfo:: statfs[%s] error!\n",mountDevice->mnt_dir);
			//return(ret);
		}
		
		if ((diskInfo.f_blocks > 0))
		{
			// �ܿռ��С
			blockSize = diskInfo.f_bsize / 1024;
			totalSize = blockSize * diskInfo.f_blocks / 1024;
			dTotalSize += totalSize;

			// ���ÿռ��С
			freeSize = blockSize * diskInfo.f_bavail / 1024;
			dFreeSize += freeSize;
			
			// ���ÿռ��С
			usedSize = totalSize - (blockSize * diskInfo.f_bfree / 1024);
			dUsedSize += usedSize;

			// ���ÿռ�ռ�ðٷֱ�
			//percent = (diskInfo.f_blocks - diskInfo.f_bfree) * 100 /(diskInfo.f_blocks - diskInfo.f_bfree + diskInfo.f_bavail) +1;
	
			len += sprintf(hardInfo + len,"%s  ",mountDevice->mnt_dir);

			UnionLog("in hardwareRunInfo hardInfo[%s] [%ld] [%ld]\n",mountDevice->mnt_dir,dTotalSize,dUsedSize);

			//percent = dTotalSize * 1.0 / dUsedSize;	
		}
	} 
	hardInfo[len] = 0;
	endmntent(fp);
#elif ( defined __hpux )
        if ((fp = popen("bdf |awk '{print $2,$4,$6}'","r")) == NULL)
        {
                UnionUserErrLog("in hardwareRunInfo:: popen[%s] error!\n","bdf|awk '{print $2,$4,$6}'");
                return(errCodeParameter);
        }
        len = sprintf(hardInfo,"���ص�: ");
	memset(tmpSize, 0, sizeof(tmpSize));
        memset(tmpTotalSize, 0, sizeof(tmpTotalSize));
        memset(tmpFreeSize, 0, sizeof(tmpFreeSize));
        memset(tmpDir, 0, sizeof(tmpDir));
	while((ptrLine = fgets(tmpSize, sizeof(tmpSize), fp)) != NULL)
	{
		sscanf(tmpSize,"%s%s%s",tmpTotalSize,tmpFreeSize,tmpDir);	
                if (atof(tmpTotalSize))
                {
                        dTotalSize += atof(tmpTotalSize);
                        freeSize += atof(tmpFreeSize);
			len += sprintf(hardInfo + len,"%s  ",tmpDir);
                }
		memset(tmpSize, 0, sizeof(tmpSize));
		memset(tmpTotalSize, 0, sizeof(tmpTotalSize));
		memset(tmpFreeSize, 0, sizeof(tmpFreeSize));
		memset(tmpDir, 0, sizeof(tmpDir));
	}
        hardInfo[len] = 0;
        dTotalSize /= 1024;
        freeSize /= 1024;
        dUsedSize = dTotalSize - freeSize;
	UnionLog("in hardwareRunInfo hardInfo[%s] [%ld] [%ld]\n",hardInfo,dTotalSize,dUsedSize);
        pclose(fp);
#elif ( defined _AIX )
	if ((fp = popen("df -m|awk '{print $2,$3}'","r")) == NULL)
	{
		UnionUserErrLog("in hardwareRunInfo:: popen[%s] error!\n","df -m|awk '{print $2,$3}'");
		return(errCodeParameter);
	}
	while(!feof(fp))
	{
		memset(tmpNum,0,sizeof(tmpNum));
		memset(tmpNum1,0,sizeof(tmpNum1));
		memset(tmpNum2,0,sizeof(tmpNum2));
		if ((ptrLine = fgets(tmpNum,sizeof(tmpNum),fp)) == NULL)
		{
			UnionLog("in hardwareRunInfo fgets tmpNum\n");
			//strcpy(tmpBuf,"ʧ��");
			break;
		}
		sscanf(tmpNum,"%s%s",tmpNum1,tmpNum2);
		if (atof(tmpNum1))
		{
			dTotalSize += atoi(tmpNum1);
			freeSize += atoi(tmpNum2);
		}
	}
	dUsedSize = dTotalSize - freeSize;
	pclose(fp);

#endif
	// ������Ӧ
	totalNum++;
	if ((ret = UnionLocateResponseNewXMLPackage("body/detail",totalNum)) < 0)
	{
		UnionUserErrLog("in hardwareRunInfo:: UnionLocateResponseNewXMLPackage disk[%s][%d]!\n","body/detail",totalNum);	
		return(ret);
	}
	
	// Ӳ������
	if ((ret = UnionSetResponseXMLPackageValue("hardwareType","Ӳ��")) < 0)
	{
		UnionUserErrLog("in hardwareRunInfo:: UnionSetResponseXMLPackageValue disk[%s][%d]!\n","hardwareType",totalNum);
		return(ret);
	}

	// Ӳ����Ϣ
	if ((ret = UnionSetResponseXMLPackageValue("hardwareInfo",hardInfo)) < 0)
	{
		UnionUserErrLog("in hardwareRunInfo:: UnionSetResponseXMLPackageValue disk[%s][%d]!\n","hardwareInfo",totalNum);
		return(ret);
	}

	// ��С
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%.1f", dTotalSize * 1.0 / 1024);
	if ((ret = UnionSetResponseXMLPackageValue("total",tmpBuf)) < 0)
	{
		UnionUserErrLog("in hardwareRunInfo:: UnionSetResponseXMLPackageValue disk[%s][%d]!\n","total",totalNum);
		return(ret);
	}


	// ���� 
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%.1f", dUsedSize * 1.0 / 1024);
	if ((ret = UnionSetResponseXMLPackageValue("used",tmpBuf)) < 0)
	{
		UnionUserErrLog("in hardwareRunInfo:: UnionSetResponseXMLPackageValue disk[%s][%d]!\n","used",totalNum);
		return(ret);
	}

	memset(infoLevel,0,sizeof(infoLevel));
	if (percent > 90)
	{
		abnormalNum++;
		strcpy(infoLevel,"����");
	}
	else
		strcpy(infoLevel,"����");
	if ((ret = UnionSetResponseXMLPackageValue("infoLevel",infoLevel)) < 0)
	{
		UnionUserErrLog("in hardwareRunInfo:: UnionSetResponseXMLPackageValue disk[%s][%d]!\n","infoLevel",totalNum);
		return(ret);
	}
	
	// cpu��Ϣ
#if ( defined __linux__ ) || ( defined __hpux )
#if ( defined __linux__ )
	// ��һ�λ�ȡ
	if ((fp = fopen("/proc/stat","r")) == NULL)
	{
		UnionUserErrLog("in hardwareRunInfo:: fopen[/proc/stat] error!\n");
		return(errCodeUseOSErrCode);
	}
	
	memset(hardValue, 0, sizeof(hardValue));
	fscanf(fp,"%s%s%s%s%s%s%s%s",hardValue[0],hardValue[1],hardValue[2],hardValue[3],hardValue[4],hardValue[5],hardValue[6],hardValue[7]);
	fclose(fp);
	
	usleep(50000);

	// �ڶ��λ�ȡ
	if ((fp = fopen("/proc/stat","r")) == NULL)
	{
		UnionUserErrLog("in hardwareRunInfo:: fopen[/proc/stat] error!\n");
		return(errCodeUseOSErrCode);
	}
	
	fscanf(fp,"%s%s%s%s%s%s%s%s",hardValue[0],hardValue[8],hardValue[9],hardValue[10],hardValue[11],hardValue[12],hardValue[13],hardValue[14]);

	fclose(fp);
	
	// ��һ�λ�ȡCPU��ʱ��
	cpuTotal1 = atol(hardValue[1]) + atol(hardValue[2]) + atol(hardValue[3]) + atol(hardValue[4]) + atol(hardValue[5]) + atol(hardValue[6]) + atol(hardValue[7]);

	// �ڶ��λ�ȡCPU��ʱ��
	cpuTotal2 = atol(hardValue[8]) + atol(hardValue[9]) + atol(hardValue[10]) + atol(hardValue[11]) + atol(hardValue[12]) + atol(hardValue[13]) + atol(hardValue[14]);

	// CPUռ�ðٷֱ�
	cpuPercent = 100.0 - 100.0 * (atol(hardValue[11])  - atol(hardValue[4])) / (cpuTotal2 - cpuTotal1); 	

#elif ( defined __hpux )
        if ((fp = popen("sar 1 2|grep Average|awk '{print $5}'","r")) == NULL)
        {
                UnionUserErrLog("in hardwareRunInfo:: popen[%s] error!\n","sar 1 2|grep Average|awk '{print $5}'");
                return(errCodeParameter);
        }
        if(fscanf(fp, "%f", &cpuPercent) != EOF)
        {
                cpuPercent = 100 - cpuPercent;
        }
        pclose(fp);
#endif

	totalNum++;
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"Cpuռ���ʣ�%.1f%%",cpuPercent);

	if ((ret = UnionLocateResponseNewXMLPackage("body/detail",totalNum)) < 0)
	{
		UnionUserErrLog("in hardwareRunInfo:: UnionLocateResponseNewXMLPackage cpu[%s][%d]!\n","body/detail",totalNum);	
		return(ret);
	}
	
	// Ӳ������
	if ((ret = UnionSetResponseXMLPackageValue("hardwareType","CPU")) < 0)
	{
		UnionUserErrLog("in hardwareRunInfo:: UnionSetResponseXMLPackageValue cpu[%s][%d]!\n","hardwareType",totalNum);
		return(ret);
	}

/*	// Ӳ����Ϣ
	if ((ret = UnionSetResponseXMLPackageValue("hardwareInfo",tmpBuf)) < 0)
	{
		UnionUserErrLog("in hardwareRunInfo:: UnionSetResponseXMLPackageValue cpu[%s][%d]!\n","hardwareInfo",totalNum);
		return(ret);
	}
*/
	// ��С
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionSetResponseXMLPackageValue("total",tmpBuf)) < 0)
	{
		UnionUserErrLog("in hardwareRunInfo:: UnionSetResponseXMLPackageValue cpu[%s][%d]!\n","total",totalNum);
		return(ret);
	}

	// ����
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%.1f",cpuPercent);
	if ((ret = UnionSetResponseXMLPackageValue("used",tmpBuf)) < 0)
	{
		UnionUserErrLog("in hardwareRunInfo:: UnionSetResponseXMLPackageValue cpu[%s][%d]!\n","used",totalNum);
		return(ret);
	}
	
	// ��Ϣ״��
	memset(infoLevel, 0, sizeof(infoLevel));
	if (cpuPercent > 50)
	{
		abnormalNum++;
		strcpy(infoLevel,"����");
	}
	else
		strcpy(infoLevel,"����");	
	if ((ret = UnionSetResponseXMLPackageValue("infoLevel",infoLevel)) < 0)
	{
		UnionUserErrLog("in hardwareRunInfo:: UnionSetResponseXMLPackageValue cpu[%s][%d]!\n","infoLevel",totalNum);
		return(ret);
	}
#if ( defined __linux__ )
	// ��ȡCpu�ͺ�
	if ((fp = fopen("/proc/cpuinfo","r")) == NULL)
	{
		UnionUserErrLog("in hardwareRunInfo:: fopen[/proc/cpuinfo] error!\n");
		return(errCodeUseOSErrCode);
	}
	memset(hardValue[0],0,sizeof(hardValue[0]));
	memset(hardValue[1],0,sizeof(hardValue[1]));
	memset(hardValue[2],0,sizeof(hardValue[2]));
	strcpy(hardValue[0],"model name");
	strcpy(hardValue[1],"cpu cores");
	strcpy(hardValue[2],"physical id");
	memset(cpuInfo,0,sizeof(cpuInfo));
	while(!feof(fp))
	{
		memset(hardInfo,0,sizeof(hardInfo));
		if ((ptrLine = fgets(hardInfo,sizeof(hardInfo),fp)) == NULL)
		{
			break;
		}
		
		// ��ȡCPU�ͺ�
		if (((ptrLine = strstr(hardInfo,hardValue[0])) != NULL) && !modelFlag)
		{
			memset(tmpBuf,0,sizeof(tmpBuf));
			sprintf(tmpBuf,"%s",ptrLine + strlen(hardValue[0]));
			if ((ptrLine = strstr(hardInfo,":")) != NULL)
			{
				memset(tmpBuf,0,sizeof(tmpBuf));
				sprintf(tmpBuf,"%s",ptrLine + 1);
			}
			UnionFilterHeadAndTailBlank(tmpBuf);

			modelFlag = 1;
			sprintf(cpuInfo,"%s  ",tmpBuf);
		}

		// ��ȡCPU����
		if (((ptrLine = strstr(hardInfo,hardValue[1])) != NULL) && !coreFlag)
		{
			memset(tmpBuf,0,sizeof(tmpBuf));
			sprintf(tmpBuf,"%s",ptrLine + strlen(hardValue[1]));
			if ((ptrLine = strstr(hardInfo,":")) != NULL)
			{
				memset(tmpBuf,0,sizeof(tmpBuf));
				sprintf(tmpBuf,"%s",ptrLine + 1);
			}
			UnionFilterHeadAndTailBlank(tmpBuf);
			if (tmpBuf[strlen(tmpBuf) - 1] == '\n')
				tmpBuf[strlen(tmpBuf)-1] = 0;

			coreFlag = 1;
			UnionFilterHeadAndTailBlank(cpuInfo);
			sprintf(cpuInfo + strlen(cpuInfo),"����: %s      ",tmpBuf);
		}

		if (modelFlag && coreFlag && !phyFlag)
		{
			fseek(fp,0,SEEK_SET);
			phyFlag = 1;
		}
		
		// ��ȡCPU��������
		if (phyFlag)
		{
			if (((ptrLine = strstr(hardInfo,hardValue[2])) != NULL))
			{
				memset(tmpBuf,0,sizeof(tmpBuf));
				sprintf(tmpBuf,"%s",ptrLine + strlen(hardValue[1]));
				if ((ptrLine = strstr(hardInfo,":")) != NULL)
				{
					memset(tmpBuf,0,sizeof(tmpBuf));
					sprintf(tmpBuf,"%s",ptrLine + 1);
				}
				UnionFilterHeadAndTailBlank(tmpBuf);
				if (tmpBuf[strlen(tmpBuf) - 1] == '\n')
					tmpBuf[strlen(tmpBuf)-1] = 0;
			}
		}
	}
	fclose(fp);
	sprintf(cpuInfo + strlen(cpuInfo),"����: %d",atoi(tmpBuf) + 1);
	UnionLog("in CPU[%s]\n",cpuInfo);
	if ((ret = UnionSetResponseXMLPackageValue("hardwareInfo",cpuInfo)) < 0)
	{
		UnionUserErrLog("in hardwareRunInfo:: UnionSetResponseXMLPackageValue cpu[%s][%d]!\n","body/hardwareInfo",totalNum);
		return(ret);
	}
#endif
#endif

	// �ڴ���Ϣ
//#if ( defined __linux__ ) || ( defined __hpux )
#if ( defined __linux__ )
	if ((fp = fopen("/proc/meminfo","r")) == NULL)
	{
		UnionUserErrLog("in hardwareRunInfo:: fopen[/proc/meminfo] error!\n");
		return(errCodeUseOSErrCode);
	}
	len = 0;
	memset(hardValue[0],0,sizeof(hardValue[0]));
	strcpy(hardValue[0],"MemTotal");
	memset(hardValue[1],0,sizeof(hardValue[1]));
	strcpy(hardValue[1],"MemFree");
	memset(hardValue[2],0,sizeof(hardValue[2]));
	strcpy(hardValue[2],"Buffers");
	memset(hardValue[3],0,sizeof(hardValue[3]));
	strcpy(hardValue[3],"Cached");
	memset(hardValue[4],0,sizeof(hardValue[4]));
	strcpy(hardValue[4],"SwapTotal");
	memset(hardValue[5],0,sizeof(hardValue[5]));
	strcpy(hardValue[5],"SwapFree");

	memset(hardInfo,0,sizeof(hardInfo));
	memset(tmpSwap,0,sizeof(tmpSwap));
	for (i = 0; i < 6; i++)
	{
		while(!feof(fp))
		{
			memset(hardValue[6],0,sizeof(hardValue[6]));
			memset(hardValue[7],0,sizeof(hardValue[7]));
			memset(tmpBuf,0,sizeof(tmpBuf));
			fscanf(fp,"%s%s%s",tmpBuf,hardValue[6],hardValue[7]);
			if (strncmp(tmpBuf,hardValue[i],strlen(hardValue[i])) == 0)
			{
				memset(hardValue[i],0,sizeof(hardValue[i]));
				strcpy(hardValue[i],hardValue[6]);
			/*	if (i >= 4)
					len += sprintf(tmpSwap + len,"%s %.3fG    ",tmpBuf,atol(hardValue[i]) * 1.000 / 1024 / 1024);
				else
				{
					len += sprintf(hardInfo + len,"%s %.3fG    ",tmpBuf,atol(hardValue[i]) * 1.000 / 1024 /1024);
				}*/
				if (i == 3)
					len = 0;
				break;
			}
		}
	}
#endif
	fclose(fp);
	for (i = 0; i < 2; i++)
	{
		totalNum++;
		if ((ret = UnionLocateResponseNewXMLPackage("body/detail",totalNum)) < 0)
		{
			UnionUserErrLog("in hardwareRunInfo:: UnionLocateResponseNewXMLPackage mem[%s][%d]!\n","body/detail",totalNum);	
			return(ret);
		}

		// Ӳ������
		memset(tmpBuf,0,sizeof(tmpBuf));
		if (i == 0)
			strcpy(tmpBuf,"�ڴ�");
		else
			strcpy(tmpBuf,"�����ռ�");
		if ((ret = UnionSetResponseXMLPackageValue("hardwareType",tmpBuf)) < 0)
		{
			UnionUserErrLog("in hardwareRunInfo:: UnionSetResponseXMLPackageValue mem[%s][%d]!\n","hardwareType",totalNum);
			return(ret);
		}

		// Ӳ����Ϣ
		memset(tmpBuf,0,sizeof(tmpBuf));
		if (i == 0)
			strcpy(tmpBuf,hardInfo);
		else
			strcpy(tmpBuf,tmpSwap);
		if ((ret = UnionSetResponseXMLPackageValue("hardwareInfo",tmpBuf)) < 0)
		{
			UnionUserErrLog("in hardwareRunInfo:: UnionSetResponseXMLPackageValue mem[%s][%d]!\n","hardwareInfo",totalNum);
			return(ret);
		}

		// ��С
		memset(tmpBuf,0,sizeof(tmpBuf));
		if (i == 0)
			sprintf(tmpBuf,"%.3f",atol(hardValue[0]) * 1.0 / 1024 / 1024);
		else
			sprintf(tmpBuf,"%.3f",atol(hardValue[4]) * 1.0 / 1024 / 1024);
		if ((ret = UnionSetResponseXMLPackageValue("total",tmpBuf)) < 0)
		{
			UnionUserErrLog("in hardwareRunInfo:: UnionSetResponseXMLPackageValue cpu[%s][%d]!\n","total",totalNum);
			return(ret);
		}

		// ����
		memset(tmpBuf,0,sizeof(tmpBuf));
		if (i == 0)
			sprintf(tmpBuf,"%.3f",(atol(hardValue[0]) - atol(hardValue[1]) - atol(hardValue[2]) - atol(hardValue[3])) * 1.000 / 1024 / 1024);
		else
			sprintf(tmpBuf,"%.3f",(atol(hardValue[4]) - atol(hardValue[5])) * 1.0 / 1024 / 1024);
		if ((ret = UnionSetResponseXMLPackageValue("used",tmpBuf)) < 0)
		{
			UnionUserErrLog("in hardwareRunInfo:: UnionSetResponseXMLPackageValue cpu[%s][%d]!\n","used",totalNum);
			return(ret);
		}

		// ��Ϣ״��
		memset(infoLevel,0,sizeof(infoLevel));
		if (i == 0)
			percent = 100 * (atol(hardValue[0]) - atol(hardValue[1]) -atol(hardValue[2]) - atol(hardValue[3])) / atol(hardValue[0]);
		else
			percent = 100 * (atol(hardValue[4]) - atol(hardValue[5])) / atol(hardValue[0]);
		
		if (percent > 80.0)
		{
			abnormalNum++;
			strcpy(infoLevel,"����");
		}
		else
			strcpy(infoLevel,"����");	
		if ((ret = UnionSetResponseXMLPackageValue("infoLevel",infoLevel)) < 0)
		{
			UnionUserErrLog("in hardwareRunInfo:: UnionSetResponseXMLPackageValue mem[%s][%d]!\n","infoLevel",totalNum);
			return(ret);
		}
	}


	//��ֵ�ܼ�¼��
	UnionLocateResponseXMLPackage("",0);

	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d",totalNum);
	if ((ret = UnionSetResponseXMLPackageValue("body/totalNum",tmpBuf)) < 0)
	{
		UnionUserErrLog("in hardwareRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","body/totalNum");
		return(ret);
	}

	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d",abnormalNum);
	if ((ret = UnionSetResponseXMLPackageValue("body/abnormalNum",tmpBuf)) < 0)
	{
		UnionUserErrLog("in hardwareRunInfo:: UnionSetResponseXMLPackageValue[%s]!\n","body/abnormalNum");
		return(ret);
	}

	hardware_abnormalNum = abnormalNum;

	return 0;
}

// ������Ϣ
int programInfo()
{
	int		ret = 0;
	char		fieldList[512+1];
	char		fieldListChnName[512+1];
/*	char		tmpBuf[128+1];
	char		fileDir[512+1];
	char		fileFullName[1024+1];
	char		fileName[128+1];
	int		i = 0,j = 0;
	int		totalNum = 0;
	int		fileNum = 0;
	char		fileTime[128+1];
	struct dirent	**nameList;
	struct stat	fileInfo;
	struct tm	*fTime;
	FILE		*fp;
	char		*ptrLine = NULL;
	int		abnormalNum = 0;
	int		len = 0;
*/
	// �����ֶ��嵥
	memset(fieldList,0,sizeof(fieldList));
	strcpy(fieldList,"programName,generateTime");
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldList",fieldList)) < 0)
	{
		UnionUserErrLog("in programInfo:: UnionSetResponseXMLPackageValue[%s]!\n","fieldList");
		return(ret);
	}

	// �����ֶ��嵥������
	memset(fieldListChnName,0,sizeof(fieldListChnName));
	strcpy(fieldListChnName,"������,����ʱ��");
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldListChnName",fieldListChnName)) < 0)
	{
		UnionUserErrLog("in programInfo:: UnionSetResponseXMLPackageValue[%s]!\n","fieldListChnName");
		return(ret);
	}

	UnionSetResponseXMLPackageValue("body/totalNum","0");
	UnionSetResponseXMLPackageValue("body/abnormalNum","0");

	// ��ȡ����Ŀ¼
/*	memset(tmpBuf,0,sizeof(tmpBuf));
	memset(fileDir,0,sizeof(fileDir));
	strcpy(tmpBuf,"$UNIONREC/bin");
	UnionReadDirFromStr(tmpBuf,-1,fileDir);

	
	// ��Ŀ¼
	if ((fileNum = scandir(fileDir,&nameList,0,alphasort)) < 0)
	{
		UnionUserErrLog("in programInfo:: scandir[%s]!\n",fileDir);
		return(fileNum);
	}
	

	for (i = 0; i < fileNum; i++)
	{
		memset(fileName,0,sizeof(fileName));
		memcpy(fileName,nameList[i]->d_name,nameList[i]->d_reclen);
		free(nameList[i]);

		if (strcmp(fileName,"dlVerion-so") == 0 || fileName[0] == '.' || memcmp(&fileName[strlen(fileName)-3],".so",3) == 0)
		{
			continue;
		}
		
		memset(fileFullName,0,sizeof(fileFullName));
		sprintf(fileFullName,"%s/%s",fileDir,fileName);
		
		if ((ret = stat(fileFullName,&fileInfo)) < 0)
		{
			UnionUserErrLog("in programInfo:: stat[%s] error!\n",fileFullName);
			free(nameList);
			return(ret);
		}
		
		totalNum++;
		if ((ret = UnionLocateResponseNewXMLPackage("body/detail",totalNum)) < 0)
		{
			UnionUserErrLog("in programInfo:: UnionLocateResponseNewXMLPackage[%s]!\n","body/detail");	
			free(nameList);
			return(ret);
		}

		// ���ó�����
		if ((ret = UnionSetResponseXMLPackageValue("programName",fileName)) < 0)
		{
			UnionUserErrLog("in programInfo:: UnionSetResponseXMLPackageValue[%s]!\n","programName");
			free(nameList);
			return(ret);
		}

		
		if (strcmp(fileName,"inputDataToDB") == 0 || strcmp(fileName,"memcached") == 0 || strcmp(fileName,"outputDataFromDB") == 0)
		{
			memset(fileTime,0,sizeof(fileTime));
			fTime = localtime(&fileInfo.st_ctime);
			strftime(fileTime,sizeof(fileTime),"%F %T",fTime);
			goto	setResponse;	
		}

		len = sprintf(tmpBuf,"%s version 2>&1",fileName);
		tmpBuf[len] = 0;

		if ((fp = popen(tmpBuf,"r")) == NULL)
		{
			UnionUserErrLog("in programInfo:: fopen[info.txt] error!\n");
			free(nameList);
			return(errCodeUseOSErrCode);
		}

		while(!feof(fp))
		{
			memset(tmpBuf, 0, sizeof(tmpBuf));
			memset(fileTime, 0, sizeof(fileTime));
			if (fgets(tmpBuf,sizeof(tmpBuf),fp) == NULL)
			{
				UnionLog("in programInfo:: fgets [%s]! \n",fileName);
				break;
			}
			if ((ptrLine = strstr(tmpBuf,"����ʱ��")) != NULL)
			{
				memset(fileTime, 0, sizeof(fileTime));
				strcpy(fileTime,ptrLine + 9);
				UnionFilterHeadAndTailBlank(fileTime);
				break;
			}
		}
		pclose(fp);

		// ���ó�������ʱ��
setResponse:
		if (strlen(fileTime) == 0)
		{
			abnormalNum ++;	
		}

		if ((ret = UnionSetResponseXMLPackageValue("generateTime",fileTime)) < 0)
		{
			UnionUserErrLog("in programInfo:: UnionSetResponseXMLPackageValue[%s]!\n","generateTime");
			free(nameList);
			return(ret);
		}

		UnionLocateResponseXMLPackage("",0);
		if (strlen(fileTime) == 0 && totalNum != 1) 
		{
			for (j = totalNum; j > abnormalNum; j--)
			{
				if ((ret = UnionExchangeIDResponseXMLPackage("body/detail",j,j-1)) < 0)
				{
					UnionUserErrLog("in taskMonRunInfo:: UnionExchangeIDResponseXMLPackage[%s][%d][%d]!\n","infoLevel",j,j-1);
				}
			}
		}

	}
	free(nameList);
	
	//ɾ���������ļ�
	if (access("info.txt",0) == 0)
	{
		if ((ret = unlink("info.txt")) < 0)
		{
			UnionUserErrLog("in programInfo:: unlink[info.txt] error");
			return(ret);
		}
	}

	UnionLocateResponseXMLPackage("",0);

	//��ֵ�ܼ�¼��
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d",totalNum);
	if ((ret = UnionSetResponseXMLPackageValue("body/totalNum",tmpBuf)) < 0)
	{
		UnionUserErrLog("in programInfo:: UnionSetResponseXMLPackageValue[%s]!\n","body/totalNum");
		return(ret);
	}
	*/
	return 0;
}

// ����������Ϣ
int envInfo()
{
	int		i = 0;
	int		j = 0;
	int		ret = 0;
	char		fieldList[512+1];
	char		fieldListChnName[512+1];
	char		envValue[1024+1];
	int		envMaxNum = 20;
	int		totalNum = 0;
	char		tmpBuf[128+1];
	char		infoLevel[4+1];
	char		envList[20][128+1] = {"$UNIONREC","$UNIONETC","$UNIONLOG","$UNIONTEMP","$DBUSER","$DBNAME"};
	int		abnormalNum = 0;

	// �����ֶ��嵥
	memset(fieldList,0,sizeof(fieldList));
	strcpy(fieldList,"infoLevel,envName,envValue");
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldList",fieldList)) < 0)
	{
		UnionUserErrLog("in envInfo:: UnionSetResponseXMLPackageValue[%s]!\n","fieldList");
		return(ret);
	}

	// �����ֶ��嵥������
	memset(fieldListChnName,0,sizeof(fieldListChnName));
	strcpy(fieldListChnName,"��Ϣ״��,������,����ֵ");
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldListChnName",fieldListChnName)) < 0)
	{
		UnionUserErrLog("in envInfo:: UnionSetResponseXMLPackageValue[%s]!\n","fieldListChnName");
		return(ret);
	}

	if ((ret = UnionSetResponseXMLPackageValue("body/totalNum","0")) < 0)
	{
		UnionUserErrLog("in envInfo:: UnionSetResponseXMLPackageValue!\n");
		return(ret);
	}

	for (i = 0; i < envMaxNum; i++)
	{
		memset(infoLevel,0,sizeof(infoLevel));

		if (strlen(envList[i]) == 0)
			continue;

		if ((strcmp(UnionGetDataBaseType(),"SQLITE") == 0) && (strcmp(envList[i],"$DBUSER") == 0))
			continue;

		totalNum++;
		if ((ret = UnionLocateResponseNewXMLPackage("body/detail", totalNum)) < 0)
		{
			UnionUserErrLog("in envInfo:: UnionLocateResponseNewXMLPackage[%d]!\n",totalNum);
			return(ret);
		}

		memset(envValue,0,sizeof(envValue));
		if((ret = UnionReadDirFromStr(envList[i],-1,envValue)) < 0)
		{
			strcpy(infoLevel,"����");
			memset(envValue,0,sizeof(envValue));
			sprintf(envValue,"%s ������",envList[i]);
			abnormalNum ++;
		}
		else if (i <= 3)
		{
			if (access(envValue,F_OK) <0)
			{
				memset(envValue,0,sizeof(envValue));
				sprintf(envValue,"%s ·��������",envList[i]);
				strcpy(infoLevel,"����");
				abnormalNum ++;
			}	
		}		

		// ���ñ�����
		if ((ret = UnionSetResponseXMLPackageValue("envName",envList[i])) < 0)
		{
			UnionUserErrLog("in envInfo:: UnionSetResponseXMLPackageValue envName[%s]!\n",envList[i]);
			return(ret);
		}

		// ���ñ���ֵ
		if ((ret = UnionSetResponseXMLPackageValue("envValue",envValue)) < 0)
		{
			UnionUserErrLog("in envInfo:: UnionSetResponseXMLPackageValue envValue[%s]!\n",envValue);
			return(ret);
		}

		// ������Ϣ״��
		if (strlen(infoLevel) == 0)
			strcpy(infoLevel,"����");
		if ((ret = UnionSetResponseXMLPackageValue("infoLevel",infoLevel)) < 0)
		{
			UnionUserErrLog("in envInfo:: UnionSetResponseXMLPackageValue envValue[%s]!\n",infoLevel);
			return(ret);
		}

		UnionLocateResponseXMLPackage("",0);
		if (strcmp(infoLevel,"����") == 0 && totalNum != 1) 
		{
			for (j = totalNum; j > abnormalNum; j--)
			{
				if ((ret = UnionExchangeIDResponseXMLPackage("body/detail",j,j-1)) < 0)
				{
					UnionUserErrLog("in commConfMonRunInfo:: UnionExchangeIDResponseXMLPackage[%s][%d][%d]!\n","infoLevel",j,j-1);
				}
			}
		}
	}

	UnionLocateResponseXMLPackage("",0);

	//��ֵ�ܼ�¼��
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d",totalNum);
	if ((ret = UnionSetResponseXMLPackageValue("body/totalNum",tmpBuf)) < 0)
	{
		UnionUserErrLog("in envInfo:: UnionSetResponseXMLPackageValue[%s]!\n","body/totalNum");
		return(ret);
	}

	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d",abnormalNum);
	if ((ret = UnionSetResponseXMLPackageValue("body/abnormalNum",tmpBuf)) < 0)
	{
		UnionUserErrLog("in envInfo:: UnionSetResponseXMLPackageValue[%s]!\n","body/abnormalNum");
		return(ret);
	}

	env_abnormalNum = abnormalNum;

	return 0;
}

int highMemcachedInfo()
{
	int		ret = 0;
	int		i = 0;
	int		len = 0;
	int		totalNum = 0;
	char		sql[128+1];
	char		tmpBuf[12+1];
	char		ipAddr[16+1];
	char		master[1+1];
	char		enabled[1+1];
	int		port = 0;
	char		fieldList[128+1];
	char		fieldListChnName[128+1];
	char		status[16+1];
	int		abnormalNum = 0;
	int		j = 0;
	
	len = sprintf(sql,"select * from highCached");	
	sql[len] = 0;

	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in highMemcachedInfo:: UnionSelectRealDBRecord ret = [%d]  sql[%s]!\n",ret,sql);
		return(ret);
	}
	else if (ret == 0)
		return 0;

	// �����ֶ��嵥
	len = sprintf(fieldList,"status,ipAddr,port,master,enabled");
	fieldList[len] = 0;
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldList",fieldList)) < 0)
	{
		UnionUserErrLog("in highMemcachedInfo:: UnionSetResponseXMLPackageValue[%s]!\n","fieldList");
		return(ret);
	}

	// �����ֶ��嵥������
	len = sprintf(fieldListChnName,"״̬,IP��ַ,�˿�,������ʶ,����");
	fieldListChnName[len] = 0;
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldListChnName",fieldListChnName)) < 0)
	{
		UnionUserErrLog("in highMemcachedInfo:: UnionSetResponseXMLPackageValue[%s]!\n","fieldListChnName");
		return(ret);
	}


	if ((ret = UnionReadXMLPackageValue("totalNum",tmpBuf,sizeof(tmpBuf))) < 0)	
	{
		UnionUserErrLog("in highMemcachedInfo:: UnionReadXMLPackageValue [%s]!\n","totalNum");
		return(ret);
	}
	tmpBuf[ret] = 0;
	totalNum = atoi(tmpBuf);

	UnionSetResponseXMLPackageValue("body/totalNum",tmpBuf);
	UnionSetResponseXMLPackageValue("body/abnormalNum","0");

	for (i = 1; i <= totalNum; i++)
	{
		if ((ret = UnionLocateXMLPackage("detail",i)) < 0)
		{
			UnionUserErrLog("in highMemcachedInfo:: UnionLocateXMLPackage [%s][%d]!\n","detail",i);
			return(ret);
		}

		// IP��ַ
		if ((ret = UnionReadXMLPackageValue("ipAddr",ipAddr,sizeof(ipAddr))) < 0)
		{
			UnionUserErrLog("in highMemcachedInfo:: UnionReadXMLPackageValue [%s][%d]!\n","ipAddr",i);
			return(ret);
		}

		// �˿ں�
		if ((ret = UnionReadXMLPackageValue("port",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in highMemcachedInfo:: UnionReadXMLPackageValue [%s][%d]!\n","port",i);
			return(ret);
		}
		tmpBuf[ret] = 0;
		port = atoi(tmpBuf);
		
		// ������ʶ master	
		memset(master,0,sizeof(master));
		if ((ret = UnionReadXMLPackageValue("master",master,sizeof(master))) < 0)
		{
			UnionUserErrLog("in highMemcachedInfo:: UnionReadXMLPackageValue [%s][%d]!\n","master",i);
			return(ret);
		}

		// ����
		memset(enabled,0,sizeof(enabled));
		if ((ret = UnionReadXMLPackageValue("enabled",enabled,sizeof(enabled))) < 0)
		{
			UnionUserErrLog("in highMemcachedInfo:: UnionReadXMLPackageValue [%s][%d]!\n","enabled",i);
			return(ret);
		}

		if (enabled[0] == '0')
		{
			len = sprintf(status,"��");	
			status[len] = 0;
		}

		if ((ret = UnionLocateResponseNewXMLPackage("body/detail",i)) < 0)
		{
			UnionUserErrLog("in highMemcachedInfo:: UnionLocateResponseNewXMLPackage [%s][%d]!\n","body/detail",i);
			return(ret);
		}

		for (j = 0; j < 3 && enabled[0] == '1'; j++)
		{
			if ((ret = UnionTestHighCachedStatus(ipAddr,port)) > 0)	
			{
				len = sprintf(status,"����");	
				status[len] = 0;
				break;
			}
			else
			{
				len = sprintf(status,"�쳣");	
				status[len] = 0;
			}
		}

		if (strcmp(status,"�쳣") == 0)
			abnormalNum ++;
		
		// ״̬
		if ((ret = UnionSetResponseXMLPackageValue("status",status)) < 0)
		{
			UnionUserErrLog("in highMemcachedInfo:: UnionSetResponseXMLPackageValue [%s][%d]!\n","status",i);
			return(ret);
		}

		// IP��ַ
		if ((ret = UnionSetResponseXMLPackageValue("ipAddr",ipAddr)) < 0)
		{
			UnionUserErrLog("in highMemcachedInfo:: UnionSetResponseXMLPackageValue [%s][%d]!\n","ipAddr",i);
			return(ret);
		}

		// �˿�
		len = sprintf(tmpBuf,"%d",port);
		tmpBuf[len] = 0;
		if ((ret = UnionSetResponseXMLPackageValue("port",tmpBuf)) < 0)
		{
			UnionUserErrLog("in highMemcachedInfo:: UnionSetResponseXMLPackageValue [%s][%d]!\n","port",i);
			return(ret);
		}

		// ������ʶ
		len = sprintf(tmpBuf,"%s",master[0] == '1' ?"��":"��");
		tmpBuf[len] = 0;
		if ((ret = UnionSetResponseXMLPackageValue("master",tmpBuf)) < 0)
		{
			UnionUserErrLog("in highMemcachedInfo:: UnionSetResponseXMLPackageValue [%s][%d]!\n","master",i);
			return(ret);
		}

		// ����״̬
		len = sprintf(tmpBuf,"%s",enabled[0] == '1' ?"����":"����");
		tmpBuf[len] = 0;
		if ((ret = UnionSetResponseXMLPackageValue("enabled",tmpBuf)) < 0)
		{
			UnionUserErrLog("in highMemcachedInfo:: UnionSetResponseXMLPackageValue [%s][%d]!\n","enabled",i);
			return(ret);
		}

		UnionLocateResponseXMLPackage("",0);
		if (strcmp(status,"�쳣") == 0 && i != 1) 
		{
			for (j = i; j > abnormalNum; j--)
			{
				if ((ret = UnionExchangeIDResponseXMLPackage("body/detail",j,j-1)) < 0)
				{
					UnionUserErrLog("in highMemcachedInfo:: UnionExchangeIDResponseXMLPackage[%s][%d][%d]!\n","infoLevel",j,j-1);
				}
			}
		}
	}

	UnionLocateResponseXMLPackage("",0);

	//��ֵ�쳣¼��
	len = sprintf(tmpBuf,"%d",abnormalNum);
	tmpBuf[len] = 0;
	if ((ret = UnionSetResponseXMLPackageValue("body/abnormalNum",tmpBuf)) < 0)
	{
		UnionUserErrLog("in highMemcachedInfo:: UnionSetResponseXMLPackageValue[%s]!\n","body/abnormalNum");
		return(ret);
	}

	highCached_abnormalNum = abnormalNum;

	return (totalNum);
}

/*********************************
�������:	0003
������:		��ȡϵͳ������Ϣ
��������:	��ȡϵͳ������Ϣ
**********************************/
int UnionDealServiceCode0003(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	char	runID[20+1];
	char	tmpBuf[1024+1];
	char	fieldList[1024+1];
	char	fieldListChnName[1024+1];
	int	totalNum = 0;
	int	len = 0;
	int	isDisplayHigh = 1;

	// ��ȡ���б�ʶ
	memset(runID,0,sizeof(runID));
	if ((ret = UnionReadRequestXMLPackageValue("body/runID",runID,sizeof(runID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0003:: UnionReadRequestXMLPackageValue[%s]!\n","body/runID");
		return(ret);
	}

	// ���ܻ�������Ϣ
	if (strcmp(runID,"hsm") == 0)
	{
		if ((ret = hsmRunInfo()) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0003:: hsmRunInfo!\n");
			return(ret);
		}
	} // ������Ϣ
	else if (strcmp(runID,"taskMon") == 0)
	{
		if ((ret = taskMonRunInfo()) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0003:: taskMonRunInfo!\n");
			return(ret);
		}
	} // ����������Ϣ
	else if (strcmp(runID,"msgBufConf") == 0)
	{
		if ((ret = msgBufConfRunInfo()) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0003:: msgBufConfRunInfo!\n");
			return(ret);
		}
	} // ͨѶ��Ϣ
	else if (strcmp(runID,"commConfMon") == 0)
	{
		if ((ret = commConfMonRunInfo()) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0003:: commConfMonRunInfo!\n");	
			return(ret);
		}
	} // ��������
	else if (strcmp(runID,"msgBufMon") == 0)
	{
		if ((ret = msgBufMonRunInfo()) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0003:: msgBufMonRunInfo!\n");
			return(ret);
		}
	} // ������Ϣ
	else if (strcmp(runID,"programInfo") == 0)
	{
		if ((ret = programInfo()) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0003:: programInfo!\n");
			return(ret);
		}
	} // Ӳ����Ϣ
	else if (strcmp(runID,"hardware") == 0)
	{
		if ((ret = hardwareRunInfo()) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0003:: hardwareRunInfo!\n");
			return(ret);
		}
	} // ����������Ϣ
	else if (strcmp(runID,"env") == 0)
	{
		if ((ret = envInfo()) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0003:: envInfo!\n");
			return(ret);
		}
	}
	else if (strcmp(runID,"hsmGroup") == 0)
	{
		if ((ret = hsmGroupInfo()) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0003:: hsmGroupInfo!\n");
			return(ret);
		}
	}
	else if (strcmp(runID,"highCached") == 0)
	{
		if ((ret = highMemcachedInfo()) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0003:: highMemcachedInfo!\n");
			return(ret);
		}
	}
	else if (strcmp(runID,"abnormal") == 0)
	{
		hsmRunInfo();
		taskMonRunInfo();
		msgBufConfRunInfo();
		commConfMonRunInfo();
		msgBufMonRunInfo();
		programInfo();
		hardwareRunInfo();
		envInfo();
		isDisplayHigh = highMemcachedInfo();

		// �����ֶ��嵥
		if (isDisplayHigh > 0)
			len = sprintf(fieldList,"hsm,hardware,commConfMon,msgBufMon,taskMon,msgBufConf,highCached,env");
		else
			len = sprintf(fieldList,"hsm,hardware,commConfMon,msgBufMon,taskMon,msgBufConf,env");

		fieldList[len] = 0;
		if ((ret = UnionSetResponseXMLPackageValue("body/fieldList",fieldList)) < 0)
		{
			UnionUserErrLog("in highMemcachedInfo:: UnionSetResponseXMLPackageValue[%s]!\n","fieldList");
			return(ret);
		}

		// �����ֶ��嵥������
		if (isDisplayHigh > 0)
			len = sprintf(fieldListChnName,"�����,Ӳ����Ϣ,ͨѶ��Ϣ,��������,������Ϣ,����������Ϣ,���ٻ���,����������Ϣ");
		else
			len = sprintf(fieldListChnName,"�����,Ӳ����Ϣ,ͨѶ��Ϣ,��������,������Ϣ,����������Ϣ,����������Ϣ");
		fieldListChnName[len] = 0;

		if ((ret = UnionSetResponseXMLPackageValue("body/fieldListChnName",fieldListChnName)) < 0)
		{
			UnionUserErrLog("in highMemcachedInfo:: UnionSetResponseXMLPackageValue[%s]!\n","fieldListChnName");
			return(ret);
		}

		if (isDisplayHigh > 0)
			totalNum = hsm_abnormalNum + hardware_abnormalNum + commConfMon_abnormalNum + msgBufMon_abnormalNum + taskMon_abnormalNum + msgBufConf_abnormalNum + highCached_abnormalNum + env_abnormalNum;
		else
			totalNum = hsm_abnormalNum + hardware_abnormalNum + commConfMon_abnormalNum + msgBufMon_abnormalNum + taskMon_abnormalNum + msgBufConf_abnormalNum + env_abnormalNum;

		len = sprintf(tmpBuf,"%d",totalNum);
		tmpBuf[len] = 0;
		UnionSetResponseXMLPackageValue("body/totalNum",tmpBuf);

		if (isDisplayHigh > 0)
			len = sprintf(tmpBuf,"%d,%d,%d,%d,%d,%d,%d,%d",hsm_abnormalNum,hardware_abnormalNum,commConfMon_abnormalNum,msgBufMon_abnormalNum,taskMon_abnormalNum,msgBufConf_abnormalNum,highCached_abnormalNum,env_abnormalNum);
		else
			len = sprintf(tmpBuf,"%d,%d,%d,%d,%d,%d,%d",hsm_abnormalNum,hardware_abnormalNum,commConfMon_abnormalNum,msgBufMon_abnormalNum,taskMon_abnormalNum,msgBufConf_abnormalNum,env_abnormalNum);
		tmpBuf[len] = 0;
		if ((ret = UnionSetResponseXMLPackageValue("body/abnormalNum",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0003:: runID[%s]  UnionSetResponseXMLPackageValue!\n",runID);
			return(ret);
		}
		
	}
	else
	{
		UnionUserErrLog("in UnionDealServiceCode0003:: runID[%s] error!\n",runID);
		UnionSetResponseRemark("[%s]δʶ��",runID);
		return(errCodeParameter);
	} 

	return 0;
}
