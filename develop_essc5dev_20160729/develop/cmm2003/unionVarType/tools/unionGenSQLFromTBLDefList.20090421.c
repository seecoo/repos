//	Wolfgang Wang
//	2009/4/29

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "UnionStr.h"

#include "unionTableList.h"
#include "unionGenSQLFromTBLDefList.h"
#include "unionDataTBLList.h"
#include "unionGenIncFromTBL.h"
#include "unionStoreWay.h"
#include "unionUserInterfaceItemGrp.h"
#include "unionComplexDBDataTransfer.h"
#include "unionUserInterfaceMaintainer.h"
#include "unionGenTBLDefFromTBL.h"
#include "unionRecFile.h"
#include "unionComplexDBRecord.h"

/*
����	
	��ʼ��������
�������
	ptblDefGrp	������ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionResetTBLDefGrp(PUnionTBLDefGrp ptblDefGrp)
{
	if (ptblDefGrp == NULL)
		return(errCodeParameter);

	memset(ptblDefGrp,0,sizeof(*ptblDefGrp));
	return(0);
}

/*
����	
	�ͷű�������
�������
	ptblDefGrp	������ָ��
�������
	��
����ֵ
	��
*/
void UnionFreeTBLDefGrp(PUnionTBLDefGrp ptblDefGrp)
{
	int	index;
	
	if (ptblDefGrp == NULL)
		return;
	for (index = 0; index < ptblDefGrp->num; index++)
	{
		if (ptblDefGrp->ptblDefGrp[index] != NULL)
			free(ptblDefGrp->ptblDefGrp[index]);
	}
	free(ptblDefGrp);
}

/*
����	
	��ʼ������
�������
	name	������
�������
	��
����ֵ
	�ɹ�	��ָ��
	NULL	��ָ��
*/
PUnionTBLDef UnionInitTBLDefFromDefFile(char *name)
{
	PUnionTBLDef	ptblDef;
	
	if ((name == NULL) || (strlen(name) == 0))
	{
		UnionSystemErrLog("in UnionInitTBLDefFromDefFile:: parameter error!\n");
		return(NULL);
	}
		
	if ((ptblDef = (PUnionTBLDef)malloc(sizeof(*ptblDef))) == NULL)
	{
		UnionSystemErrLog("in UnionInitTBLDefFromDefFile:: malloc!\n");
		return(NULL);
	}
	
	if (strlen(name) >= sizeof(ptblDef->name))
	{
		UnionUserErrLog("in UnionInitTBLDefFromDefFile:: name = [%s] too long!\n",name);
		free(ptblDef);
		return(NULL);
	}
	memset(ptblDef,0,sizeof(*ptblDef));
	strcpy(ptblDef->name,name);
	return(ptblDef);
}

/*
����	
	�ӱ����嵥��ʼ�����ϵ
�������
	ptblDefGrp	������ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionInitAllTBLRelationsFromTBLDefList(PUnionTBLDefGrp ptblDefGrp)
{
	int			ret;
	int			index;
		
	if (ptblDefGrp == NULL)
	{
		UnionUserErrLog("in UnionInitAllTBLRelationsFromTBLDefList:: null pointer!\n");
		return(errCodeParameter);
	}
	for (index = 0; index < ptblDefGrp->num; index++)
	{
		if ((ret = UnionRegisterSpecTBLToAllParents(ptblDefGrp->ptblDefGrp[index],ptblDefGrp)) < 0)
		{
			UnionUserErrLog("in UnionInitAllTBLRelationsFromTBLDefList:: UnionRegisterSpecTBLToAllParents [%s]!\n",
				ptblDefGrp->ptblDefGrp[index]->name);
			return(ret);
		}
		ptblDefGrp->relationsNum += ret;
	}
	return(ptblDefGrp->relationsNum);
}

/*
����	
	�ӱ����嵥��ʼ��������
�������
	ptblDefGrp	������ָ��
�������
	��
����ֵ
	�ɹ�	����ָ��
	ʧ��	NULL
*/
PUnionTBLDefGrp UnionInitTBLDefGrpFromTBLDefTBL()
{
	int			ret;
	char			fileName[512+1];
	FILE			*recFileFp = NULL;
	char			recStr[8192*2+1];
	int			lenOfRecStr;
	char			tableName[128+1];
	PUnionTBLDefGrp		ptblDefGrp;
		
	if ((ptblDefGrp = (PUnionTBLDefGrp)malloc(sizeof(*ptblDefGrp))) == NULL)
	{
		UnionSystemErrLog("in UnionInitTBLDefGrpFromTBLDefTBL:: malloc!\n");
		return(NULL);
	}

	if ((ret = UnionResetTBLDefGrp(ptblDefGrp)) < 0)
	{
		UnionUserErrLog("in UnionInitTBLDefGrpFromTBLDefTBL:: UnionResetTBLDefGrp!\n");
		return(NULL);
	}
	
	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/pid-%d.tmp",getenv("UNIONTEMP"),getpid());

	// �����м�¼
	if ((ret = UnionBatchSelectObjectRecord(conDataTBLListTBLName,"",fileName)) < 0)
	{
		UnionUserErrLog("in UnionInitTBLDefGrpFromTBLDefTBL:: UnionBatchSelectObjectRecord [%s]!\n",conDataTBLListTBLName);
		return(NULL);
	}
	
	// ���ļ�
	if ((recFileFp = fopen(fileName,"r")) == NULL)
	{
		UnionUserErrLog("in UnionInitTBLDefGrpFromTBLDefTBL:: fopen [%s]\n",fileName);
		goto errExit;
	}
	while (!feof(recFileFp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(recFileFp,recStr,sizeof(recStr))) <= 0)
			continue;
		if (ptblDefGrp->num >= conMaxNumOfTBLDefPerGrp)
		{
			UnionUserErrLog("in UnionInitTBLDefGrpFromTBLDefTBL:: ptblDefGrp->num = [%d] > maximum numbers defined!\n",ptblDefGrp->num);
			break;
		}
		memset(tableName,0,sizeof(tableName));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"tableName",tableName,sizeof(tableName))) < 0)
		{
			UnionUserErrLog("in UnionInitTBLDefGrpFromTBLDefTBL:: UnionReadRecFldFromRecStr [%s] from [%s]\n","tableName",recStr);
			goto errExit;
		}
		if ((ptblDefGrp->ptblDefGrp[ptblDefGrp->num] = UnionInitTBLDefFromDefFile(tableName)) == NULL)
		{
			UnionUserErrLog("in UnionInitTBLDefGrpFromTBLDefTBL:: UnionInitTBLDefFromDefFile [%s]\n",tableName);
			ret = errCodeObjectMDL_ReadObjectDefinition;
			goto errExit;
		}
		if ((ret = UnionReadIntTypeRecFldFromRecStr(recStr,lenOfRecStr,"storeWay",&(ptblDefGrp->ptblDefGrp[ptblDefGrp->num]->storeWay))) < 0)
		{
			UnionLog("in UnionInitTBLDefGrpFromTBLDefTBL:: UnionReadIntTypeRecFldFromRecStr [%s] from [%s]\n","storeWay",recStr);
			ptblDefGrp->ptblDefGrp[ptblDefGrp->num]->storeWay = conStoreWayDataBase;
			//goto errExit;
		}
		if (ptblDefGrp->ptblDefGrp[ptblDefGrp->num]->storeWay == conStoreWayDataBase)
			ptblDefGrp->dbTBLNum += 1;
		ptblDefGrp->num += 1;
	}
	ret = ptblDefGrp->num;
	UnionLog("in UnionInitTBLDefGrpFromTBLDefTBL:: total table num = [%d] totalDBTBLNum = [%d]\n",ret,ptblDefGrp->dbTBLNum);
errExit:
	fclose(recFileFp);
	UnionDeleteFile(fileName);
	if (ret < 0)
	{
		UnionFreeTBLDefGrp(ptblDefGrp);
		return(NULL);
	}
	if ((ret = UnionInitAllTBLRelationsFromTBLDefList(ptblDefGrp)) < 0)
	{
		UnionUserErrLog("in UnionInitTBLDefGrpFromTBLDefTBL:: UnionInitAllTBLRelationsFromTBLDefList!\n");
		UnionFreeTBLDefGrp(ptblDefGrp);
		return(NULL);
	}
	UnionLog("in UnionInitTBLDefGrpFromTBLDefTBL:: total relation num = [%d]\n",ptblDefGrp->relationsNum);
	return(ptblDefGrp);
}

/*
����	
	��һ��������������
�������
	name	����
	maxNum	�����������Ի��������������Ŀ
�������
	foreignTBLNameGrp	���������
����ֵ
	>=0	�ɹ��������Ŀ
	<0	������
*/
int UnionReadAllForeignTBLNames(char *name,char foreignTBLNameGrp[][40+1],int maxNum)
{
	int			ret;
	TUnionTableList		objDef;
	int			num = 0;
	
	if ((ret = UnionReadTableListRecOfSpecTBLName(name,&objDef)) < 0)
	{
		UnionUserErrLog("in UnionReadAllForeignTBLNames:: UnionReadTableListRecOfSpecTBLName [%s]!\n",name);
		return(ret);
	}
	if ((strlen(objDef.foreignTable1) != 0) && (strlen(objDef.myKeyGrp1) != 0) && (strlen(objDef.foreignKey1) != 0))
	{
		strcpy(foreignTBLNameGrp[num],objDef.foreignTable1);
		num++;
	}
	if ((strlen(objDef.foreignTable2) != 0) && (strlen(objDef.myKeyGrp2) != 0) && (strlen(objDef.foreignKey2) != 0))
	{
		strcpy(foreignTBLNameGrp[num],objDef.foreignTable2);
		num++;
	}
	if ((strlen(objDef.foreignTable3) != 0) && (strlen(objDef.myKeyGrp3) != 0) && (strlen(objDef.foreignKey3) != 0))
	{
		strcpy(foreignTBLNameGrp[num],objDef.foreignTable3);
		num++;
	}
	if ((strlen(objDef.foreignTable4) != 0) && (strlen(objDef.myKeyGrp4) != 0) && (strlen(objDef.foreignKey4) != 0))
	{
		strcpy(foreignTBLNameGrp[num],objDef.foreignTable4);
		num++;
	}
	return(num);
}

/*
����	
	��һ������Ǽǵ���������
�������
	ptblDef		����
	ptblDefGrp	������ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionRegisterSpecTBLToAllParents(PUnionTBLDef ptblDef,PUnionTBLDefGrp ptblDefGrp)
{
	int			index;
	PUnionTBLDef		pparentDef;
	char			foreignTBLNameGrp[20][40+1];
	int			foreignTBLNum = 0;
		
	if ((ptblDef == NULL) || (ptblDefGrp == NULL))
	{
		UnionUserErrLog("in UnionRegisterSpecTBLToAllParents:: null pointer!\n");
		return(errCodeParameter);
	}
	//UnionLog("in UnionRegisterSpecTBLToAllParents:: foreignTBLNum = [%d] of [%s]\n",foreignTBLNum,ptblDef->name);
	if ((foreignTBLNum = UnionReadAllForeignTBLNames(ptblDef->name,foreignTBLNameGrp,20)) < 0)
	{
		UnionUserErrLog("in UnionRegisterSpecTBLToAllParents:: UnionReadAllForeignTBLNames [%s]!\n",ptblDef->name);
		return(foreignTBLNum);
	}
	//UnionLog("in UnionRegisterSpecTBLToAllParents:: foreignTBLNum = [%d] of [%s]\n",foreignTBLNum,ptblDef->name);
	for (index = 0; index < foreignTBLNum; index++)
	{
		if ((pparentDef = UnionFindSpecTBLDefInTBLDefGrp(ptblDefGrp,foreignTBLNameGrp[index])) == NULL)
		{
			UnionUserErrLog("in UnionRegisterSpecTBLToAllParents:: UnionFindSpecTBLDefInTBLDefGrp [%s]!\n",
				foreignTBLNameGrp[index]);
			return(errCodeObjectMDL_ReadObjectDefinition);
		}
		if (pparentDef->childrenNum >= conMaxNumOfChildrenTBL)
		{
			UnionUserErrLog("in UnionRegisterSpecTBLToAllParents:: childrenNum = [%d] of [%s] too much!\n",pparentDef->childrenNum,pparentDef->name);
			return(errCodeEsscMDL_TooManyChildTBLs);
		}
		pparentDef->children[pparentDef->childrenNum] = ptblDef;
		pparentDef->childrenNum += 1;
		if (ptblDef->parentNum >= conMaxNumOfChildrenTBL)
		{
			UnionUserErrLog("in UnionRegisterSpecTBLToAllParents:: childrenNum = [%d] of [%s] too much!\n",pparentDef->parentNum,ptblDef->name);
			return(errCodeEsscMDL_TooManyParentTBLs);
		}
		ptblDef->parents[ptblDef->parentNum] = pparentDef;
		ptblDef->parentNum += 1;
	}
	return(foreignTBLNum);
}

/*
����	
	��һ������Ǽǵ���������
�������
	ptblDefGrp	������ָ��
�������
	��
����ֵ
	�ɹ�	ָ��
	ʧ��	NULL
*/
PUnionTBLDef UnionFindSpecTBLDefInTBLDefGrp(PUnionTBLDefGrp ptblDefGrp,char *name)
{
	int			index;
		
	if ((name == NULL) || (ptblDefGrp == NULL))
	{
		UnionUserErrLog("in UnionFindSpecTBLDefInTBLDefGrp:: null pointer!\n");
		return(NULL);
	}
	
	for (index = 0; index < ptblDefGrp->num; index++)
	{
		if (strcmp(name,ptblDefGrp->ptblDefGrp[index]->name) == 0)
			return(ptblDefGrp->ptblDefGrp[index]);
	}
	UnionUserErrLog("in UnionFindSpecTBLDefInTBLDefGrp:: %s not found!\n",name);
	return(NULL);
}

/*
����	
	�ж�һ����ĸ����Ƿ���ȫ������
�������
	ptblDef	������ָ��
	fp	�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionIsAllParentsOfSpecTBLDefCreated(PUnionTBLDef ptblDef)
{
	int			index;
		
	if (ptblDef == NULL)
	{
		UnionUserErrLog("in UnionIsAllParentsOfSpecTBLDefCreated:: null pointer!\n");
		return(errCodeParameter);
	}

	for (index = 0; index < ptblDef->parentNum; index++)
	{
		if (!ptblDef->parents[index]->created)
		{
			UnionLog("in UnionIsAllParentsOfSpecTBLDefCreated:: parent [%s] of [%s] not created!\n",
				ptblDef->parents[index]->name,ptblDef->name);
			return(0);
		}
	}
	return(1);
}

/*
����	
	��ӡһ�����SQL���
�������
	ptblDef	������ָ��
	fp	�ļ�ָ��
	mainMenuName	���˵�����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionCreateSQLForSpecTBLDef(PUnionTBLDef ptblDef,FILE *fp,char *mainMenuName)
{
	int			ret;
		
	if (ptblDef == NULL)
	{
		UnionUserErrLog("in UnionCreateSQLForSpecTBLDef:: null pointer!\n");
		return(errCodeParameter);
	}
	if (ptblDef->created)
		return(0);
	if ((mainMenuName != NULL) && (strlen(mainMenuName) != 0))
	{
		if (!UnionIsTableUsedByMainMenu(mainMenuName,ptblDef->name))
		{
			UnionLog("***** [%s] [%s] not exists!\n",mainMenuName,ptblDef->name);
			ptblDef->created = 1;
			return(1);
		}
	}
	if ((ret = UnionIsAllParentsOfSpecTBLDefCreated(ptblDef)) < 0)
	{
		UnionUserErrLog("in UnionCreateSQLForSpecTBLDef:: UnionIsAllParentsOfSpecTBLDefCreated!\n");
		return(ret);
	}
	if (ret == 0)
	{
		UnionAuditLog("in UnionCreateSQLForSpecTBLDef:: [%s] still has some parents not created!\n",ptblDef->name);
		return(errCodeObjectMDL_ForeignRecordNotExist);
	}
	if ((ret = UnionCreateObjectDefSQLFromTBLDefInTableList(ptblDef->name,fp)) < 0)
	{
		UnionUserErrLog("in UnionCreateSQLForSpecTBLDef:: UnionCreateObjectDefSQLFromTBLDefInTableList!\n");
		return(ret);
	}
	ptblDef->created = 1;
	return(1);
}


/*
����	
	��ӡ���б��SQL���
�������
	ptblDefGrp	������ָ��
	fp		�ļ�ָ��
	mainMenuName	���˵�����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionCreateSQLForAllTBLDef(PUnionTBLDefGrp ptblDefGrp,FILE *fp,char *mainMenuName)
{
	int			ret;
	int			index;
	int			createdNum = 0;
	int			loopTimes = 0;
	
	if (ptblDefGrp == NULL)
	{
		UnionUserErrLog("in UnionCreateSQLForAllTBLDef:: null pointer!\n");
		return(errCodeParameter);
	}
	if ((mainMenuName != NULL) && (strlen(mainMenuName) != 0))
	{
		if ((ret = UnionCollectAllInterfaceItemsOfSpecMainMenu(mainMenuName)) < 0)
		{
			UnionUserErrLog("in UnionCreateSQLForAllTBLDef:: UnionCollectAllInterfaceItemsOfSpecMainMenu [%s]!\n",mainMenuName);
			return(ret);
		}
	}		
loop:
	if (++loopTimes > 100000)
	{
		UnionUserErrLog("in UnionCreateSQLForAllTBLDef:: loopTimes = [%d]\n",loopTimes);
		return(errCodeTooManyLoopTimes);
	}
	for (index = 0; index < ptblDefGrp->num; index++)
	{
		if (ptblDefGrp->ptblDefGrp[index]->storeWay != conStoreWayDataBase)
			continue;
		if ((ret = UnionCreateSQLForSpecTBLDef(ptblDefGrp->ptblDefGrp[index],fp,mainMenuName)) < 0)
		{
			if (errCodeObjectMDL_ForeignRecordNotExist != ret)
			{
				UnionUserErrLog("in UnionCreateSQLForAllTBLDef:: UnionCreateSQLForSpecTBLDef!\n");
				return(ret);
			}
			continue;
		}
		if (ret > 0)
			createdNum++;
	}
	if (createdNum >= ptblDefGrp->dbTBLNum)
	{
		UnionAuditLog("in UnionCreateSQLForAllTBLDef:: create [%d] tables sql ok!\n",createdNum);
		UnionFreeAllCollectedInterfaceItemsOfSpecMainMenu(mainMenuName);
		return(createdNum);
	}
	goto loop;
}

/*
����	
	��ӡ���б��SQL���
�������
	fp		�ļ�ָ��
	mainMenuName	���˵�����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionCreateSQLForAllTBLDefInTableListOfSpecMainMenu(FILE *fp,char *mainMenuName)
{
	int			ret;
	PUnionTBLDefGrp		ptblDefGrp;
	
	if ((ptblDefGrp = UnionInitTBLDefGrpFromTBLDefTBL()) == NULL)
	{
		UnionUserErrLog("in UnionCreateSQLForAllTBLDefInTableListOfSpecMainMenu:: UnionInitTBLDefGrpFromTBLDefTBL!\n");
		return(errCodeEsscMDL_TBLDefGrpInitFailure);
	}
	if ((ret = UnionCreateSQLForAllTBLDef(ptblDefGrp,fp,mainMenuName)) < 0)
	{
		UnionUserErrLog("in UnionCreateSQLForAllTBLDefInTableListOfSpecMainMenu:: UnionCreateSQLForAllTBLDef!\n");
	}
	UnionFreeTBLDefGrp(ptblDefGrp);
	UnionLog("in UnionCreateSQLForAllTBLDefInTableListOfSpecMainMenu:: [%04d] tables ok!\n",ret);
	return(ret);
}

/*
����	
	��ӡ���б��SQL���
�������
	fileName	�ļ���
	mainMenuName	���˵�����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionCreateSQLForAllTBLDefInTableListOfSpecMainMenuToSpecFile(char *fileName,char *mainMenuName)
{
	int			ret;
	FILE			*fp;
	
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionCreateSQLForAllTBLDefInTableListOfSpecMainMenuToSpecFile:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	if ((ret = UnionCreateSQLForAllTBLDefInTableListOfSpecMainMenu(fp,mainMenuName)) < 0)
	{
		UnionUserErrLog("in UnionCreateSQLForAllTBLDefInTableListOfSpecMainMenuToSpecFile:: UnionCreateSQLForAllTBLDefInTableListOfSpecMainMenu [%s]!\n",fileName);
	}
	fclose(fp);
	UnionLog("in UnionCreateSQLForAllTBLDefInTableListOfSpecMainMenuToSpecFile:: [%04d] tables ok!\n",ret);
	return(ret);
}

/*
����	
	��ӡ���б��SQL���
�������
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionCreateSQLForAllTBLDefInTableList(FILE *fp)
{
	int			ret;
	PUnionTBLDefGrp		ptblDefGrp;
	
	if ((ptblDefGrp = UnionInitTBLDefGrpFromTBLDefTBL()) == NULL)
	{
		UnionUserErrLog("in UnionCreateSQLForAllTBLDefInTableList:: UnionInitTBLDefGrpFromTBLDefTBL!\n");
		return(errCodeEsscMDL_TBLDefGrpInitFailure);
	}
	if ((ret = UnionCreateSQLForAllTBLDef(ptblDefGrp,fp,NULL)) < 0)
	{
		UnionUserErrLog("in UnionCreateSQLForAllTBLDefInTableList:: UnionCreateSQLForAllTBLDef!\n");
	}
	UnionFreeTBLDefGrp(ptblDefGrp);
	UnionLog("in UnionCreateSQLForAllTBLDefInTableList:: [%04d] tables ok!\n",ret);
	return(ret);
}

/*
����	
	��ӡ���б��SQL���
�������
	fileName	�ļ���
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionCreateSQLForAllTBLDefInTableListToSpecFile(char *fileName)
{
	int			ret;
	FILE			*fp;
	
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionCreateSQLForAllTBLDefInTableListToSpecFile:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	if ((ret = UnionCreateSQLForAllTBLDefInTableList(fp)) < 0)
	{
		UnionUserErrLog("in UnionCreateSQLForAllTBLDefInTableListToSpecFile:: UnionCreateSQLForAllTBLDefInTableList [%s]!\n",fileName);
	}
	fclose(fp);
	UnionLog("in UnionCreateSQLForAllTBLDefInTableListToSpecFile:: [%04d] tables ok!\n",ret);
	return(ret);
}

/*
����	
	�������б������
�������
	dir		�������ļ�Ŀ¼
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionOutputAllRecForAllTBLDef(char *dir)
{
	int			ret;
	int			index;
	int			num = 0;
	char			fileName[512];
	PUnionTBLDefGrp		ptblDefGrp;
	
	if ((ptblDefGrp = UnionInitTBLDefGrpFromTBLDefTBL()) == NULL)
	{
		UnionUserErrLog("in UnionOutputAllRecForAllTBLDef:: UnionInitTBLDefGrpFromTBLDefTBL!\n");
		return(errCodeEsscMDL_TBLDefGrpInitFailure);
	}
	for (index = 0; index < ptblDefGrp->num; index++)
	{
		if (ptblDefGrp->ptblDefGrp[index]->storeWay != conStoreWayDataBase)
			continue;
		if ((dir != NULL) && (strlen(dir) != 0))
			sprintf(fileName,"%s/%s.txt",dir,ptblDefGrp->ptblDefGrp[index]->name);
		else
			sprintf(fileName,"%s.txt",ptblDefGrp->ptblDefGrp[index]->name);
		if ((ret = UnionOutputAllRecFromSpecTBL(ptblDefGrp->ptblDefGrp[index]->name,"",fileName)) < 0)
		{
			UnionUserErrLog("in UnionOutputAllRecForAllTBLDef:: UnionOutputAllRecFromSpecTBL [%s]!\n",ptblDefGrp->ptblDefGrp[index]->name);
			return(ret);
		}
		UnionAuditLog("in UnionOutputAllRecForAllTBLDef:: [%06d] records output from [%s]\n",ret,ptblDefGrp->ptblDefGrp[index]->name);
		num++;
	}
	return(num);
}

/*
����	
	��ӡһ��������
�������
	ptblDef	������ָ��
	prefix		������ǰ׺
	fp	�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionPrintTBLNameForSpecTBLDef(PUnionTBLDef ptblDef,char *prefix,FILE *fp)
{
	int			ret;
	FILE			*outFp = stdout;
	
	if (fp != NULL)
		outFp = fp;
			
	if (ptblDef == NULL)
	{
		UnionUserErrLog("in UnionPrintTBLNameForSpecTBLDef:: null pointer!\n");
		return(errCodeParameter);
	}
	if (ptblDef->created)
		return(0);
	if ((ret = UnionIsAllParentsOfSpecTBLDefCreated(ptblDef)) < 0)
	{
		UnionUserErrLog("in UnionPrintTBLNameForSpecTBLDef:: UnionIsAllParentsOfSpecTBLDefCreated!\n");
		return(ret);
	}
	if (ret == 0)
	{
		UnionAuditLog("in UnionPrintTBLNameForSpecTBLDef:: [%s] still has some parents not created!\n",ptblDef->name);
		return(errCodeObjectMDL_ForeignRecordNotExist);
	}
	if ((prefix == NULL) || (strlen(prefix) == 0))
		fprintf(outFp,"%s\n",ptblDef->name);
	else
		fprintf(outFp,"%s %s\n",prefix,ptblDef->name);
	ptblDef->created = 1;
	return(1);
}

/*
����	
	��ӡ���б�����
�������
	prefix		������ǰ׺
	ptblDefGrp	������ָ��
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionPrintTBLNameForAllTBLDef(PUnionTBLDefGrp ptblDefGrp,char *prefix,FILE *fp)
{
	int			ret;
	int			index;
	int			createdNum = 0;
	int			loopTimes = 0;
		
	if (ptblDefGrp == NULL)
	{
		UnionUserErrLog("in UnionPrintTBLNameForAllTBLDef:: null pointer!\n");
		return(errCodeParameter);
	}
loop:
	if (++loopTimes > 100)
	{
		UnionUserErrLog("in UnionPrintTBLNameForAllTBLDef:: loopTimes = [%d]\n",loopTimes);
		return(errCodeTooManyLoopTimes);
	}
	for (index = 0; index < ptblDefGrp->num; index++)
	{
		if (ptblDefGrp->ptblDefGrp[index]->storeWay != conStoreWayDataBase)
			continue;
		if ((ret = UnionPrintTBLNameForSpecTBLDef(ptblDefGrp->ptblDefGrp[index],prefix,fp)) < 0)
		{
			if (errCodeObjectMDL_ForeignRecordNotExist != ret)
			{
				UnionUserErrLog("in UnionPrintTBLNameForAllTBLDef:: UnionPrintTBLNameForSpecTBLDef!\n");
				return(ret);
			}
			continue;
		}
		if (ret > 0)
			createdNum++;
	}
	if (createdNum >= ptblDefGrp->dbTBLNum)
	{
		UnionAuditLog("in UnionPrintTBLNameForAllTBLDef:: create [%d] tables sql ok!\n",createdNum);
		return(createdNum);
	}
	goto loop;
}

/*
����	
	��ӡ���б�����
�������
	prefix		������ǰ׺
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionPrintTBLNameForAllTBLDefInTableList(char *prefix,FILE *fp)
{
	int			ret;
	PUnionTBLDefGrp		ptblDefGrp;
	
	if ((ptblDefGrp = UnionInitTBLDefGrpFromTBLDefTBL()) == NULL)
	{
		UnionUserErrLog("in UnionPrintTBLNameForAllTBLDefInTableList:: UnionInitTBLDefGrpFromTBLDefTBL!\n");
		return(errCodeEsscMDL_TBLDefGrpInitFailure);
	}
	if ((ret = UnionPrintTBLNameForAllTBLDef(ptblDefGrp,prefix,fp)) < 0)
	{
		UnionUserErrLog("in UnionPrintTBLNameForAllTBLDefInTableList:: UnionPrintTBLNameForAllTBLDef!\n");
	}
	UnionFreeTBLDefGrp(ptblDefGrp);
	return(ret);
}

/*
����	
	��ӡ���б�����
�������
	prefix		������ǰ׺
	fileName	�ļ���
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionPrintTBLNameForAllTBLDefInTableListToSpecFile(char *prefix,char *fileName)
{
	int			ret;
	FILE			*fp;
	
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionPrintTBLNameForAllTBLDefInTableListToSpecFile:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	if ((ret = UnionPrintTBLNameForAllTBLDefInTableList(prefix,fp)) < 0)
	{
		UnionUserErrLog("in UnionPrintTBLNameForAllTBLDefInTableListToSpecFile:: UnionPrintTBLNameForAllTBLDefInTableList [%s]!\n",fileName);
	}
	fclose(fp);
	return(ret);
}


/*
����	
	��ʾ���б��ϵ
�������
	ptblDefGrp	������ָ��
	displayParents	1,��ʾ����0,��ʾ�ӱ�2��ʾ���й�ϵ
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionPrintRelationsForAllTBLDef(PUnionTBLDefGrp ptblDefGrp,int displayParents,FILE *fp)
{
	int			ret;
	int			index;
	int			num = 0;
		
	if (ptblDefGrp == NULL)
	{
		UnionUserErrLog("in UnionPrintRelationsForAllTBLDef:: null pointer!\n");
		return(errCodeParameter);
	}

	for (index = 0; index < ptblDefGrp->num; index++)
	{
		if (ptblDefGrp->ptblDefGrp[index]->storeWay != conStoreWayDataBase)
			continue;
		switch (displayParents)
		{
			case	0:
				if ((ret = UnionPrintChildrenOfSpecTBLDef(ptblDefGrp->ptblDefGrp[index],fp)) < 0)
				{
					UnionUserErrLog("in UnionPrintRelationsForAllTBLDef:: UnionPrintChildrenOfSpecTBLDef!\n");
					return(ret);
				}
				break;
			case	1:
				if ((ret = UnionPrintParentsOfSpecTBLDef(ptblDefGrp->ptblDefGrp[index],fp)) < 0)
				{
					UnionUserErrLog("in UnionPrintRelationsForAllTBLDef:: UnionPrintParentsOfSpecTBLDef!\n");
					return(ret);
				}
				break;
			default:
				if ((ret = UnionPrintRelationsOfSpecTBLDef(ptblDefGrp->ptblDefGrp[index],fp)) < 0)
				{
					UnionUserErrLog("in UnionPrintRelationsForAllTBLDef:: UnionPrintRelationsOfSpecTBLDef!\n");
					return(ret);
				}
				break;
		}
		num += ret;
	}
	return(num);
}

/*
����	
	��ʾ���б��ϵ
�������
	displayParents	1,��ʾ����0,��ʾ�ӱ�2��ʾ���й�ϵ
	fileName	�ļ�����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionPrintRelationsForAllTBLDefToSpecFile(int displayParents,char *fileName)
{
	int			ret;
	PUnionTBLDefGrp		ptblDefGrp;
	FILE			*fp;
	
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionPrintRelationsForAllTBLDefToSpecFile:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	if ((ptblDefGrp = UnionInitTBLDefGrpFromTBLDefTBL()) == NULL)
	{
		UnionUserErrLog("in UnionPrintRelationsForAllTBLDefToSpecFile:: UnionInitTBLDefGrpFromTBLDefTBL!\n");
		fclose(fp);
		return(errCodeEsscMDL_TBLDefGrpInitFailure);
	}
	if ((ret = UnionPrintRelationsForAllTBLDef(ptblDefGrp,displayParents,fp)) < 0)
	{
		UnionUserErrLog("in UnionPrintRelationsForAllTBLDefToSpecFile:: UnionPrintRelationsForAllTBLDef!\n");
	}
	UnionFreeTBLDefGrp(ptblDefGrp);
	fclose(fp);
	return(ret);
}

/*
����	
	��ʾ����ӱ�
�������
	ptblDef		��ָ��
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionPrintChildrenOfSpecTBLDef(PUnionTBLDef ptblDef,FILE *fp)
{
	FILE	*outFp = stdout;
	int	index;
	
	if (fp != NULL)
		outFp = fp;
	if (ptblDef == NULL)
		return(errCodeParameter);
	if (ptblDef->childrenNum <= 0)
		return(0);
	fprintf(outFp,"��[%30s]���ӱ�����[%04d],�嵥::",ptblDef->name,ptblDef->childrenNum);
	for (index = 0; index < ptblDef->childrenNum; index++)
	{
		if (index > 0)
			fprintf(outFp,",");
		fprintf(outFp,"%s",ptblDef->children[index]->name);
	}
	fprintf(outFp,"\n");
	return(ptblDef->childrenNum);
}

/*
����	
	��ʾ��ĸ���
�������
	ptblDef		��ָ��
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionPrintParentsOfSpecTBLDef(PUnionTBLDef ptblDef,FILE *fp)
{
	FILE	*outFp = stdout;
	int	index;
	
	if (fp != NULL)
		outFp = fp;
	if (ptblDef == NULL)
		return(errCodeParameter);
	if (ptblDef->parentNum <= 0)
		return(0);
	fprintf(outFp,"��[%30s]�ĸ�������[%04d],�嵥::",ptblDef->name,ptblDef->parentNum);
	for (index = 0; index < ptblDef->parentNum; index++)
	{
		if (index > 0)
			fprintf(outFp,",");
		fprintf(outFp,"%s",ptblDef->parents[index]->name);
	}
	fprintf(outFp,"\n");
	return(ptblDef->parentNum);
}

/*
����	
	��ʾ��Ĺ�ϵ
�������
	ptblDef		��ָ��
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionPrintRelationsOfSpecTBLDef(PUnionTBLDef ptblDef,FILE *fp)
{
	int	ret;
	int	num;
	
	if ((ret = UnionPrintChildrenOfSpecTBLDef(ptblDef,fp)) < 0)
		return(ret);
	num = ret;
	if ((ret = UnionPrintParentsOfSpecTBLDef(ptblDef,fp)) < 0)
		return(ret);
	num += ret;
	return(num);
}

/*
����	
	��ʾָ��������й�ϵ
�������
	ptblDefGrp	������ָ��
	tableName	����
	displayParents	1,��ʾ����0,��ʾ�ӱ�2��ʾ���й�ϵ
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionPrintRelationsForSpecTBLDef(PUnionTBLDefGrp ptblDefGrp,char *tableName,int displayParents,FILE *fp)
{
	int			ret;
	int			index;
		
	if ((ptblDefGrp == NULL) || (tableName == NULL))
	{
		UnionUserErrLog("in UnionPrintRelationsForSpecTBLDef:: null pointer!\n");
		return(errCodeParameter);
	}

	for (index = 0; index < ptblDefGrp->num; index++)
	{
		if ((ptblDefGrp->ptblDefGrp[index]->storeWay != conStoreWayDataBase) || (strcmp(ptblDefGrp->ptblDefGrp[index]->name,tableName) != 0))
			continue;
		switch (displayParents)
		{
			case	0:
				if ((ret = UnionPrintChildrenOfSpecTBLDef(ptblDefGrp->ptblDefGrp[index],fp)) < 0)
				{
					UnionUserErrLog("in UnionPrintRelationsForSpecTBLDef:: UnionPrintChildrenOfSpecTBLDef!\n");
					return(ret);
				}
				break;
			case	1:
				if ((ret = UnionPrintParentsOfSpecTBLDef(ptblDefGrp->ptblDefGrp[index],fp)) < 0)
				{
					UnionUserErrLog("in UnionPrintRelationsForSpecTBLDef:: UnionPrintParentsOfSpecTBLDef!\n");
					return(ret);
				}
				break;
			default:
				if ((ret = UnionPrintRelationsOfSpecTBLDef(ptblDefGrp->ptblDefGrp[index],fp)) < 0)
				{
					UnionUserErrLog("in UnionPrintRelationsForSpecTBLDef:: UnionPrintRelationsOfSpecTBLDef!\n");
					return(ret);
				}
				break;
		}
		return(ret);
	}
	UnionUserErrLog("in UnionPrintRelationsForSpecTBLDef:: [%s] not defined!\n",tableName);
	return(errCodeObjectMDL_ObjectDefinitionNotExists);
}


/*
����	
	��ʾָ��������й�ϵ
�������
	tableName	����
	displayParents	1,��ʾ����0,��ʾ�ӱ�2��ʾ���й�ϵ
	fileName	�ļ���
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionPrintRelationsForSpecTBLDefToSpecFile(char *tableName,int displayParents,char *fileName)
{
	int			ret;
	PUnionTBLDefGrp		ptblDefGrp;
	FILE			*fp;
	
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionPrintRelationsForSpecTBLDefToSpecFile:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	if ((ptblDefGrp = UnionInitTBLDefGrpFromTBLDefTBL()) == NULL)
	{
		UnionUserErrLog("in UnionPrintRelationsForSpecTBLDefToSpecFile:: UnionInitTBLDefGrpFromTBLDefTBL!\n");
		fclose(fp);
		return(errCodeEsscMDL_TBLDefGrpInitFailure);
	}
	if ((ret = UnionPrintRelationsForSpecTBLDef(ptblDefGrp,tableName,displayParents,fp)) < 0)
	{
		UnionUserErrLog("in UnionPrintRelationsForSpecTBLDefToSpecFile:: UnionPrintRelationsForSpecTBLDef!\n");
	}
	UnionFreeTBLDefGrp(ptblDefGrp);
	fclose(fp);
	return(ret);
}

/*
����	
	�ж�һ������ӱ��Ƿ���ȫ������
�������
	ptblDef	������ָ��
	fp	�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionIsAllChildrenOfSpecTBLDefCreated(PUnionTBLDef ptblDef)
{
	int			index;
		
	if (ptblDef == NULL)
	{
		UnionUserErrLog("in UnionIsAllChildrenOfSpecTBLDefCreated:: null pointer!\n");
		return(errCodeParameter);
	}

	for (index = 0; index < ptblDef->childrenNum; index++)
	{
		if (!ptblDef->children[index]->created)
			return(0);
	}
	return(1);
}

/*
����	
	��ӡһ�����ɾ�����SQL���
�������
	ptblDef	������ָ��
	fp	�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionCreateDropTBLSQLForSpecTBLDef(PUnionTBLDef ptblDef,FILE *fp)
{
	int			ret;
	FILE			*outFp = stdout;
		
	if (ptblDef == NULL)
	{
		UnionUserErrLog("in UnionCreateDropTBLSQLForSpecTBLDef:: null pointer!\n");
		return(errCodeParameter);
	}
	if (ptblDef->created)
		return(0);
	if ((ret = UnionIsAllChildrenOfSpecTBLDefCreated(ptblDef)) < 0)
	{
		UnionUserErrLog("in UnionCreateDropTBLSQLForSpecTBLDef:: UnionIsAllChildrenOfSpecTBLDefCreated!\n");
		return(ret);
	}
	if (ret == 0)
	{
		UnionAuditLog("in UnionCreateDropTBLSQLForSpecTBLDef:: [%s] still has some children not droped!\n",ptblDef->name);
		return(errCodeObjectMDL_ForeignRecordNotExist);
	}
	if (fp != NULL)
		outFp = fp;
	fprintf(outFp,"drop table %s;\n",ptblDef->name);
	ptblDef->created = 1;
	return(1);
}

/*
����	
	��ӡ���б��ɾ�����SQL���
�������
	ptblDefGrp	������ָ��
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionCreateDropTBLSQLForAllTBLDef(PUnionTBLDefGrp ptblDefGrp,FILE *fp)
{
	int			ret;
	int			index;
	int			createdNum = 0;
	int			loopTimes = 0;
		
	if (ptblDefGrp == NULL)
	{
		UnionUserErrLog("in UnionCreateDropTBLSQLForAllTBLDef:: null pointer!\n");
		return(errCodeParameter);
	}
loop:
	if (++loopTimes > 100)
	{
		UnionUserErrLog("in UnionCreateDropTBLSQLForAllTBLDef:: loopTimes = [%d] createNum = [%d] dbTBLNum = [%d]\n",loopTimes,createdNum,ptblDefGrp->dbTBLNum);
		return(errCodeTooManyLoopTimes);
	}
	for (index = 0; index < ptblDefGrp->num; index++)
	{
		if (ptblDefGrp->ptblDefGrp[index]->storeWay != conStoreWayDataBase)
			continue;
		if ((ret = UnionCreateDropTBLSQLForSpecTBLDef(ptblDefGrp->ptblDefGrp[index],fp)) < 0)
		{
			if (errCodeObjectMDL_ForeignRecordNotExist != ret)
			{
				UnionUserErrLog("in UnionCreateDropTBLSQLForAllTBLDef:: UnionCreateDropTBLSQLForSpecTBLDef!\n");
				return(ret);
			}
			continue;
		}
		if (ret > 0)
			createdNum++;
	}
	if (createdNum >= ptblDefGrp->dbTBLNum)
	{
		UnionAuditLog("in UnionCreateDropTBLSQLForAllTBLDef:: create [%d] tables sql ok!\n",createdNum);
		return(createdNum);
	}
	goto loop;
}

/*
����	
	��ӡ���б��ɾ�����SQL���
�������
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionCreateDropTBLSQLForAllTBLDefInTableList(FILE *fp)
{
	int			ret;
	PUnionTBLDefGrp		ptblDefGrp;
	
	if ((ptblDefGrp = UnionInitTBLDefGrpFromTBLDefTBL()) == NULL)
	{
		UnionUserErrLog("in UnionCreateDropTBLSQLForAllTBLDefInTableList:: UnionInitTBLDefGrpFromTBLDefTBL!\n");
		return(errCodeEsscMDL_TBLDefGrpInitFailure);
	}
	if ((ret = UnionCreateDropTBLSQLForAllTBLDef(ptblDefGrp,fp)) < 0)
	{
		UnionUserErrLog("in UnionCreateDropTBLSQLForAllTBLDefInTableList:: UnionCreateDropTBLSQLForAllTBLDef!\n");
	}
	UnionFreeTBLDefGrp(ptblDefGrp);
	return(ret);
}

/*
����	
	��ӡ���б��ɾ�����SQL���
�������
	fileName	�ļ���
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionCreateDropTBLSQLForAllTBLDefInTableListToSpecFile(char *fileName)
{
	int			ret;
	FILE			*fp;
	
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionCreateDropTBLSQLForAllTBLDefInTableListToSpecFile:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	if ((ret = UnionCreateDropTBLSQLForAllTBLDefInTableList(fp)) < 0)
	{
		UnionUserErrLog("in UnionCreateDropTBLSQLForAllTBLDefInTableListToSpecFile:: UnionCreateDropTBLSQLForAllTBLDefInTableList [%s]!\n",fileName);
	}
	fclose(fp);
	return(ret);
}
