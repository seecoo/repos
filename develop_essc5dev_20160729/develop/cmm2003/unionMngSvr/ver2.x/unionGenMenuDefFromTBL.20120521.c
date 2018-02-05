//	Wolfgang Wang
//	2009/4/29

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "unionErrCode.h"
#include "unionDatabaseCmd.h"
//#include "unionResID.h"
#include "UnionStr.h"
#include "unionGenIncFromTBL.h"
#include "unionGenTBLDefFromTBL.h"
#include "mngSvrConfFileName.h"
#include "mngSvrCommProtocol.h"

#include "unionFldGrp.h"
#include "unionRecFile.h"
#include "unionTBLQueryConf.h"
#include "unionTableField.h"
#include "unionTableList.h"
#include "unionDataTBLList.h"
#include "unionOperationAuthorization.h"
#include "unionMenuItemType.h"
#include "unionMainMenu.h"
#include "unionMenuDef.h"
#include "unionMenuItemDef.h"
#include "unionMenuItemOperationDef.h"
#include "unionViewList.h"
#include "unionOperatorType.h"
#include "unionEnumValueDef.h"
#include "unionDataPackFldList.h"
#include "unionSecurityServiceDef.h"
#include "unionComplexField.h"
#include "UnionLog.h"

// added 2012-10-25
#include "unionTeller.h"
extern char *UnionGetCurrentOperationTellerNo();
char gunionViewMenu[1024] = "";
// end of addition 2012-10-25

int UnionIsAuthMenuItem(PUnionMenuDef menuGrp, char *menuItemName, char *viewMenu)
{
	char			authList[1024];
	char			keyword[64];

	if(viewMenu == NULL || strlen(viewMenu) == 0)
	{
		return(1);
	}
	if(menuGrp->tableMenuType != 1 && menuGrp->tableMenuType != 2 && menuGrp->tableMenuType != 3)
	{
		return(1);
	}

	sprintf(keyword, ",%s,", menuItemName);
	sprintf(authList, ",%s,", viewMenu);
	if(strstr(authList, keyword) != NULL)
	{
		UnionLog("in UnionIsAuthMenuItem:: mainMenuName = [%s] menuGrpName = [%s] menuItemName = [%s] is found in operator.viewMenu [%s]!\n", menuGrp->mainMenuName, menuGrp->menuGrpName, menuItemName, viewMenu);
		return(1);
	}
	UnionLog("in UnionIsAuthMenuItem:: mainMenuName = [%s] menuGrpName = [%s] menuItemName = [%s] is not found in operator.viewMenu [%s]!\n", menuGrp->mainMenuName, menuGrp->menuGrpName, menuItemName, viewMenu);
	return(0);
}
/*
����	
	��һ���˵����д���ļ���
�������
	pmenuItemDef	�˵����
	prec		�˵����������
	fp		�ļ�ָ��
	iDownloadAllMenuItem	�Ƿ���Ҫ��ȡ���в������������
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionWriteMenuItemDefToSpecFile(PUnionMenuItemDef pmenuItemDef,PUnionMenuItemOperationDef prec,FILE *fp, int iDownloadAllMenuItem)
{
	int	ret;
	FILE	*outFp = stdout;
	int	lenOfProperty;
	char	property[32][128];
	int	propertyNum;
	int	index;
	
	if (fp != NULL)
		outFp = fp;
		
	if ((pmenuItemDef->menuDisplayname == NULL) || (prec == NULL))
		return(errCodeParameter);

	if ((strcmp(prec->menuName,"�ָ���") == 0) || (pmenuItemDef->menuDisplayname[0] == '-'))
	{
		fprintf(outFp,"itemName=-|tblAlais=null|\n");
		return(0);
	}
	if (strlen(pmenuItemDef->menuRealDisplayName) == 0)
		fprintf(outFp,"itemName=%s|",pmenuItemDef->menuDisplayname);
	else		
		fprintf(outFp,"itemName=%s|",pmenuItemDef->menuRealDisplayName);
	//fprintf(outFp,"itemName=%s|tblAlais=%s|operationID=%d|",pmenuItemDef->menuDisplayname,pmenuItemDef->viewName,prec->serviceID);
	fprintf(outFp,"tblAlais=%s|operationID=%d|",pmenuItemDef->viewName,prec->serviceID);
	if (strlen(prec->conditionsMenu) != 0)
		fprintf(outFp,"condition=%s|",prec->conditionsMenu);
	// modify by wangk 2010-3-24
	if( iDownloadAllMenuItem == 1 )
	{
		fprintf(outFp,"level=%s|", pmenuItemDef->visualLevelListOfMenu);
	}
	else
	{
		fprintf(outFp,"level=1234567890|");
	}
	// modify end
	//if ((propertyNum = UnionSeprateVarStrIntoVarGrp(pmenuItemDef->otherProperty,strlen(pmenuItemDef->otherProperty),
	//	conSeparableSignOfFieldNameInFldList,property,32)) < 0)
	if ((propertyNum = UnionSeprateVarStrIntoVarGrp(pmenuItemDef->otherProperty,strlen(pmenuItemDef->otherProperty),
		'?',property,32)) < 0)
	{
		UnionUserErrLog("in UnionWriteMenuItemDefToSpecFile:: UnionSeprateVarStrIntoVarGrp [%s]\n",pmenuItemDef->otherProperty);
		return(propertyNum);
	}
	//UnionLog("*** propertyNum = [%d]\n",propertyNum);
	for (index = 0; index < propertyNum; index++)
		fprintf(outFp,"%s|",property[index]);
	
	if (prec->isExistsSuffixOperation)
	{
		fprintf(outFp,"suffixOperationTblAlais=%s|",prec->suffixOperationTblAlais);
		fprintf(outFp,"suffixOperationTblID=%d|",prec->suffixOperationTblID);
		fprintf(outFp,"suffixOperationName=%s|",prec->suffixOperationName);
		fprintf(outFp,"suffixOperationID=%d|",prec->suffixOperationID);
		if (strlen(prec->suffixOperationCondition) > 0)
			fprintf(outFp,"suffixOperationCondition=%s|",prec->suffixOperationCondition);
	}
	fprintf(outFp,"isOperatorSingleRec=%d|",prec->isSingleOperation);
	fprintf(outFp,"isNeedConfirm=%d|",prec->isNeedConfirm);
	fprintf(outFp,"showProgressBar=%d|",prec->showProgressBar);
	if (strlen(prec->beforeAction) > 0)
		fprintf(outFp,"beforeAction=%s|",prec->beforeAction);
	if (strlen(prec->remark) > 0)
		fprintf(outFp,"remark=%s|",prec->remark);
 	fprintf(outFp,"showStyle=%d|",prec->mainFrmShowStyle);
 	if (strlen(pmenuItemDef->svrName) > 0)
 		fprintf(outFp,"svrName=%s|",pmenuItemDef->svrName);
 	fprintf(outFp,"\n");
	return(0);		
}
/*
����	
	���ݶ����˵����ƣ����������˵�
�������
	tellerTypeID	����Ա��ʶ
	mainMenuName	���˵�����
	menuGrpName	�����˵�����
	fileName		���ɵĲ˵��ļ�����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionCreate2LevelMenuByMenuGrpName(char *tellerTypeID,char *mainMenuName,char *menuGrpName,char *fileName)
{
	FILE			*outFp = NULL;
	int			ret;
	
	TUnionTeller		operator;

	if ((outFp = fopen(fileName,"w")) == NULL)
	{
		UnionUserErrLog("in UnionCreate2LevelMenuByMenuGrpName:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}

	memset(&operator, 0, sizeof(operator));
        UnionReadOperatorRec(UnionGetCurrentOperationTellerNo(), &operator);
        strcpy(gunionViewMenu, operator.viewMenu);

	if ((ret = UnionGenerateMenuDefFileFromMenuNameToFp(tellerTypeID,mainMenuName,menuGrpName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionCreate2LevelMenuByMenuGrpName:: UnionGenerateMenuDefFileFromMenuNameToFp [%s]!\n",menuGrpName);
		//return(ret);
	}
	fclose(outFp);
	return(ret);
}	

/*
����	
	���ݲ˵������ƣ�����һ���˵������ļ�
�������
	tellerTypeID	����Ա����
	mainMenuName	���˵�����
	menuGrpName	�˵�����
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateMenuDefFileFromMenuNameToFp(char *tellerTypeID,char *mainMenuName,char *menuGrpName,FILE *fp)
{
	int			ret;
	int			menuItemDefNum = 0;
	FILE			*outFp = NULL;
	TUnionMenuItemDef	menuItemDefRec;
	PUnionMenuItemDef	pmenuItemDefRec;
	int			index;
	TUnionMenuItemOperationDef	menuItemOperationDefRec;
	int			itemNum = 0;
	TUnionMenuDef		menuDef;
	char			menuItemNameGrp[48][128];
	char	szDownloadAllMenuItem[32];
	int		iDownloadAllMenuItem = 0;
	char			*pmenuName = NULL;
	
	if (fp != NULL)
		outFp = fp;
	
	// ���˵�����
	memset(&menuDef,0,sizeof(menuDef));
	if ((ret = UnionReadMenuDefRec(mainMenuName,menuGrpName,&menuDef)) < 0)
	{
		UnionUserErrLog("in UnionGenerateMenuDefFileFromMenuNameToFp:: UnionReadMenuDefRec [%s][%s]\n",mainMenuName,menuGrpName);
		return(ret);
	}
	if (UnionIsFldStrInUnionFldListStr(menuDef.visualLevelListOfMenu,strlen(menuDef.visualLevelListOfMenu),',',tellerTypeID) <= 0)
	{
		UnionAuditLog("in UnionGenerateMenuDefFileFromMenuNameToFp:: [%s][%s] not authorized to [%s]\n",mainMenuName,menuGrpName,tellerTypeID);
		return(0);
		//return(errCodeEsscMDL_NoOperationAuthority);	// ��������2010-8-2ɾ����
	}
	// ���ò˵������Ĳ˵���
	if ((menuItemDefNum = UnionSeprateVarStrIntoVarGrp(menuDef.menuItemsList,strlen(menuDef.menuItemsList),',',menuItemNameGrp,48)) < 0)
	{
		UnionUserErrLog("in UnionGenerateMenuDefFileFromMenuNameToFp:: UnionSeprateVarStrIntoVarGrp [%s]\n",menuDef.menuItemsList);
		return(menuItemDefNum);
	}
	//UnionLog("in UnionGenerateMenuDefFileFromMenuNameToFp:: menuItemDefNum = [%d] of [%s]\n",menuItemDefNum,menuGrpName);

	// add by wangk 2010-3-24
	// �ж��Ƿ���Ҫ��ȡ���в������������
	memset(szDownloadAllMenuItem, 0, sizeof(szDownloadAllMenuItem));
	if( (ret = UnionReadFldFromCurrentMngSvrClientReqStr("downloadAllMenuItem", szDownloadAllMenuItem, sizeof(szDownloadAllMenuItem))) < 0 )
	{
		iDownloadAllMenuItem = 0;
	}
	if( strcmp(szDownloadAllMenuItem, "1") == 0 )
	{
		iDownloadAllMenuItem = 1;
	}
	else
	{
		iDownloadAllMenuItem = 0;
	}
	// add end wangk

	for (index = 0; index < menuItemDefNum; index++)
	{
		// ���˵����
		memset(&menuItemDefRec,0,sizeof(menuItemDefRec));
		if ((ret = UnionReadMenuItemDefRec(mainMenuName,menuGrpName,menuItemNameGrp[index],&menuItemDefRec)) < 0)
		{
			UnionAuditLog("in UnionGenerateMenuDefFileFromMenuNameToFp:: UnionReadMenuItemDefRec [%s][%s] not exists\n",menuGrpName,menuItemNameGrp[index]);
			continue;
		}

		// added 2012-10-25
		if(strlen(menuItemDefRec.menuRealDisplayName) != 0)
		{
			pmenuName = menuItemDefRec.menuRealDisplayName;	
		}
		else
		{
			pmenuName = menuItemDefRec.menuDisplayname;
		}
		UnionLog("in UnionGenerateMenuDefFileFromMenuNameToFp:: UnionIsAuthMenuItem menuItemDefRec.menuRealDisplayName [%s]\n", pmenuName);
		if(!UnionIsAuthMenuItem(&menuDef, pmenuName,  gunionViewMenu))
		{
			continue;
		}
		// end of addition 2012-10-25

		pmenuItemDefRec = &menuItemDefRec;
		if ((strncmp(pmenuItemDefRec->menuName,"�ղ���",6) != 0) && (pmenuItemDefRec->menuDisplayname[0] != '-'))
		{
			if( iDownloadAllMenuItem != 1 ) // ֻ��ȡ��ǰ����Ա���������
			{
				if (UnionIsFldStrInUnionFldListStr(pmenuItemDefRec->visualLevelListOfMenu,strlen(pmenuItemDefRec->visualLevelListOfMenu),',',tellerTypeID) <= 0)
				{
					UnionLog("in UnionGenerateMenuDefFileFromMenuNameToFp:: [%s] not authorized to [%s]\n",menuItemNameGrp[index],tellerTypeID);
					continue;
				}
			}
		}
		// ���˵����Ӧ�Ĳ�������
		memset(&menuItemOperationDefRec,0,sizeof(menuItemOperationDefRec));
		if ((ret = UnionReadMenuItemOperationDefRec(pmenuItemDefRec->menuName,&menuItemOperationDefRec)) < 0)
		{
			UnionUserErrLog("in UnionGenerateMenuDefFileFromMenuNameToFp:: UnionReadMenuItemOperationDefRec [%s]\n",pmenuItemDefRec->menuName);
			return(ret);
		}
		if ((ret = UnionWriteMenuItemDefToSpecFile(pmenuItemDefRec,&menuItemOperationDefRec,outFp, iDownloadAllMenuItem)) < 0)
		{
			UnionUserErrLog("in UnionGenerateMenuDefFileFromMenuNameToFp:: UnionReadMenuItemOperationDefRec [%s]\n",pmenuItemDefRec->menuName);
			return(ret);
		}
		//UnionLog("in UnionGenerateMenuDefFileFromMenuNameToFp:: [%s] authorized to [%s]\n",menuItemNameGrp[index],tellerTypeID);
		itemNum++;
	}
	return(itemNum);
}
/*
����	
	�������˵����ƣ��������˵�
�������
	tellerTypeID	����Ա�����ʶ
	mainMenuName	���˵�����
	fileName		���ɵĲ˵��ļ�����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionCreateMainMenuByMainMenuName(char *tellerTypeID,char *mainMenuName,char *fileName)
{
	int			ret;
	int			recNum = 0;
	FILE			*outFp = NULL;
	TUnionMainMenu		mainMenuRec;
	int			itemNum;
	int			itemIndex;
	char			itemName[100][128];
	TUnionMenuDef		menuDefRec;
		
	if ((outFp = fopen(fileName,"w")) == NULL)
	{
		UnionUserErrLog("in UnionCreateMainMenuByMainMenuName:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	
	// �����˵�����
	memset(&mainMenuRec,0,sizeof(mainMenuRec));
	if ((ret = UnionReadMainMenuRec(mainMenuName,&mainMenuRec)) < 0)
	{
		UnionUserErrLog("in UnionCreateMainMenuByMainMenuName:: UnionReadMainMenuRec [%s]!\n",mainMenuName);
		return(ret);
	}
	// ��ֲ˵���
	memset(itemName,0,sizeof(itemName));
	if ((ret = UnionSeprateVarStrIntoVarGrp(mainMenuRec.mainMenuItemsList,strlen(mainMenuRec.mainMenuItemsList),conSeparableSignOfFieldNameInFldList,itemName,100)) < 0)
	{
		UnionUserErrLog("in UnionCreateMainMenuByMainMenuName:: UnionSeprateVarStrIntoVarGrp [%s]\n",mainMenuRec.mainMenuItemsList);
		return(ret);
	}
	itemNum = ret;
	
	for (itemIndex = 0; itemIndex < itemNum; itemIndex++)
	{
		memset(&menuDefRec,0,sizeof(menuDefRec));
		if ((ret = UnionReadMenuDefRec(mainMenuName,itemName[itemIndex],&menuDefRec)) < 0)
		{
			UnionAuditLog("in UnionCreateMainMenuByMainMenuName:: UnionReadMenuDefRec [%s] error\n",itemName[itemIndex]);
			continue;
		}
		if (UnionIsFldStrInUnionFldListStr(menuDefRec.visualLevelListOfMenu,strlen(menuDefRec.visualLevelListOfMenu),',',tellerTypeID) <= 0)
		{
			UnionLog("in UnionCreateMainMenuByMainMenuName:: [%s][%s] not authorized to [%s]\n",mainMenuName,itemName[itemIndex],tellerTypeID);
			continue;
		}
		fprintf(outFp,"menuGrpName=%s|menuName=%s|level=1023456789|\n",menuDefRec.displayName,menuDefRec.menuGrpName);
		recNum++;
	}
	ret = recNum;
errExit:
	if (outFp != NULL)
		fclose(outFp);
	return(ret);
}

/*
����	
	������ͼ����(��viewList���ж���)������һ������ṹ
�������
	tblName		����
	mainMenuName	���˵�����
	fileName	�ļ�����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateTBLQueryConfFromViewDefToSpecFile(char *tblName,char *mainMenuName,char *fileName)
{
	int		ret;
	FILE 		*fp = stdout;
	
	if ((fileName != NULL) && (strlen(fileName) != 0))
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionUserErrLog("in UnionGenerateTBLQueryConfFromViewDefToSpecFile:: fopen [%s] for table [%s]!\n",fileName,tblName);
			return(errCodeUseOSErrCode);
		}
	}
	if ((ret = UnionGenerateTBLQueryConfFromViewDefToFp(tblName,mainMenuName,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateTBLQueryConfFromViewDefToSpecFile:: UnionGenerateTBLQueryConfFromTBLDefInTableListToFp for table [%s]!\n",tblName);
		//return(ret);
	}
	if (fp != stdout)
		fclose(fp);
	return(ret);
}

/*
����	
	������ͼ����(��viewList���ж���)������һ������ṹ
�������
	tblName		����
	mainMenuName	���˵�����
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateTBLQueryConfFromViewDefToFp(char *tblName,char *mainMenuName,FILE *fp)
{
	int			ret;
	FILE			*outFp = stdout;
	TUnionViewList		rec;
	int			fldNum;
	char			fldNameGrp[100][128];
	TUnionTBLQueryConf	queryConf;
	char			recMainMenuName[100];
	
	if (tblName == NULL)
	{
		UnionUserErrLog("in UnionGenerateTBLQueryConfFromViewDefToFp:: null pointer!\n");
		return(errCodeParameter);
	}
	if ((mainMenuName == NULL) || (strlen(mainMenuName) == 0))
		strcpy(recMainMenuName,"������Կ����ϵͳ");
	else
		strcpy(recMainMenuName,mainMenuName);
		
	if (fp != NULL)
		outFp = fp;
		
	// �������¼
	memset(&rec,0,sizeof(rec));
	if ((ret = UnionReadViewListRec(tblName,recMainMenuName,&rec)) < 0)
	{
		UnionUserErrLog("in UnionGenerateTBLQueryConfFromViewDefToFp:: UnionReadViewListRec [%s][%s]!\n",tblName,recMainMenuName);
		return(ret);
	}

	// ���ݱ����¼���Ա�����и�ֵ
	memset(&queryConf,0,sizeof(queryConf));
	strcpy(queryConf.name,rec.tableName);
	strcpy(queryConf.alais,rec.ID);
	//if (strlen(queryConf.alais) == 0)
	//	strcpy(queryConf.alais,queryConf.name);
	queryConf.id = rec.resID % 1000;
	strcpy(queryConf.formIconFileName,rec.formIconFileName);
	strcpy(queryConf.formPopMenuDefFileName,rec.formPopMenuDefFileName);
	strcpy(queryConf.recIconFileName,rec.recIconFileName);
	strcpy(queryConf.recPopMenuDefFileName,rec.recPopMenuDefFileName);
	strcpy(queryConf.formIconFileName,rec.formIconFileName);
	strcpy(queryConf.recDoubleClickMenuFileName,rec.recDoubleClickMenuFileName);
	queryConf.maxNumOfRecPerPage = rec.maxNumOfRecPerPage;
	queryConf.useSeparateTBL = rec.useSeparateTBL;

	UnionConvertOneFldSeperatorInRecStrIntoAnother(rec.fieldAlaisList,strlen(rec.fieldAlaisList),',','|',rec.fieldAlaisList,sizeof(rec.fieldAlaisList));
	// ��ѯ����
	if ((ret = UnionGenerateQueryFldGrpDefFromFldIDList(rec.fieldDisplayList,strlen(rec.fieldDisplayList),&(queryConf.queryFldGrp))) < 0)
	{
		UnionUserErrLog("in UnionGenerateTBLQueryConfFromViewDefToFp:: UnionGenerateQueryFldGrpDefFromFldIDList for view [%s]!\n",tblName);
		return(ret);
	}
	// ����ѯ���е���ʹ�ñ����滻
	UnionRenameFldOfSpecQueryFldGrpDef(rec.fieldAlaisList,strlen(rec.fieldAlaisList),&(queryConf.queryFldGrp));
	
	// �ؼ���
	if ((ret = UnionGenerateQueryFldListFromFldIDList(rec.primaryKey,strlen(rec.primaryKey),&(queryConf.primaryKeyFldGrp))) < 0)
	{
		UnionUserErrLog("in UnionGenerateTBLQueryConfFromViewDefToFp:: UnionGenerateQueryFldListFromFldIDList for view [%s]!\n",tblName);
		return(ret);
	}
	// ������д�뵽�ļ���
	if ((ret = UnionPrintTBLQueryConfToFp(&queryConf,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateTBLQueryConfFromViewDefToFp:: UnionPrintTBLQueryConfToFp for view [%s]!\n",tblName);
		return(ret);
	}
	// ��ѯ����
	if ((ret = UnionGenerateAssignmentQueryFldListFromFldIDList(rec.fieldList,strlen(rec.fieldList),
		rec.fieldAlaisList,strlen(rec.fieldAlaisList),
		rec.loopList,strlen(rec.loopList),
		rec.viewLevel,strlen(rec.viewLevel),
		rec.editLevel,strlen(rec.editLevel),
		outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateTBLQueryConfFromViewDefToFp:: UnionGenerateAssignmentQueryFldListFromFldIDList for view [%s]!\n",tblName);
		return(ret);
	}

	// add by wangk 2010-3-24
	// ����ͼȱʡ����д�������ļ���
	fprintf(outFp, "\n#��ͼȱʡ����\n");
	fprintf(outFp, "autoCondition=%s\n", rec.viewCondition);
	// add end wangk

	// ��ģ����ѯ����д�������ļ���
	fprintf(outFp,"\n#ģ����ѯ����\n");
	UnionConvertOneFldSeperatorInRecStrIntoAnother(rec.viewQueryDef,strlen(rec.viewQueryDef),'?','|',rec.viewQueryDef,sizeof(rec.viewQueryDef));
	fprintf(outFp,"sqlqueryfld=%s\n",rec.viewQueryDef);
	return(0);
}
/*
����	
	��һ����ͼ�Ĳ�����Ȩд�뵽�ļ���
�������
	mainMenuName	���˵�����
	viewName	��ͼ������
	fileName	�ļ�����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateViewAuthorizationToSpecFile(char *mainMenuName,char *viewName,char *fileName)
{
	FILE	*fp = stdout;
	int	ret;
	
	if ((fileName != NULL) && (strlen(fileName) != 0))
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionGenerateViewAuthorizationToSpecFile:: fopen [%s]\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	
	if ((ret = UnionGenerateViewAuthorizationToFp(mainMenuName,viewName,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateViewAuthorizationToSpecFile:: UnionGenerateViewAuthorizationToFp [%s][%s]!\n",mainMenuName,viewName);
	}
	
	if (fp != stdout)
		fclose(fp);
	return(ret);
}
/*
����	
	������ͼ����(��tableList���ж���)������һ����ͼ���浯���˵������ĵ�
�������
	viewName		����
	mainMenuName		���˵�����
	fileName		�ļ�����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateFormPopMenuDefDocFromViewDefToSpecFile(char *viewName,char *mainMenuName,char *fileName)
{
	return(UnionGenerateInterfaceMenuDefDocFromViewDefToSpecFile(viewName,mainMenuName,conMenuItemTypeFormPopMenu,fileName));
}

/*
����	
	������ͼ����(��tableList���ж���)������һ����ͼ��¼�����˵������ĵ�
�������
	viewName		����
	mainMenuName		���˵�����
	fileName		�ļ�����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateRecPopMenuDefDocFromViewDefToSpecFile(char *viewName,char *mainMenuName,char *fileName)
{
	return(UnionGenerateInterfaceMenuDefDocFromViewDefToSpecFile(viewName,mainMenuName,conMenuItemTypeRecPopMenu,fileName));
}

/*
����	
	������ͼ����(��tableList���ж���)������һ����ͼ��¼�����˵������ĵ�
�������
	viewName		����
	mainMenuName		���˵�����
	fileName		�ļ�����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateRecDoubleClickMenuDefDocFromViewDefToSpecFile(char *viewName,char *mainMenuName,char *fileName)
{
	return(UnionGenerateInterfaceMenuDefDocFromViewDefToSpecFile(viewName,mainMenuName,conMenuItemTypeDoubleClickMenu,fileName));
}

/*
����	
	������ͼ����(��tableList���ж���)������һ������˵������ĵ�
�������
	viewName	��ͼ��
	mainMenuName	���˵�����
	menuType	�˵�����
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateInterfaceMenuDefDocFromViewDefToFp(char *viewName,char *mainMenuName,int menuType,FILE *fp)
{
	int			ret;
	TUnionViewList		rec;
	char			menuName[128];
	
	// ����ͼ����
	memset(&rec,0,sizeof(rec));
	if ((ret = UnionReadViewListRec(viewName,mainMenuName,&rec)) < 0)
	{
		UnionUserErrLog("in UnionGenerateInterfaceMenuDefDocFromViewDefToFp:: UnionReadViewListRec [%s]!\n",viewName);
		return(ret);
	}
	switch (menuType)
	{
		case	conMenuItemTypeFormPopMenu:
			strcpy(menuName,rec.formPopMenuDefFileName);
			break;
		case	conMenuItemTypeRecPopMenu:
			strcpy(menuName,rec.recPopMenuDefFileName);
			break;
		case	conMenuItemTypeDoubleClickMenu:
			strcpy(menuName,rec.recDoubleClickMenuFileName);
			break;
		default:
			UnionUserErrLog("in UnionGenerateInterfaceMenuDefDocFromViewDefToFp:: invalid menutype [%d]\n",menuType);
			return(errCodeParameter);
	}
	return(UnionGenerateMenuGrpDefDocToFp(rec.mainMenuName,menuName,fp));
}

/*
����	
	������ͼ����(��tableList���ж���)������һ������˵������ĵ�
�������
	viewName	��ͼ��
	mainMenuName	���˵�����
	menuType	�˵�����
	fileName		�ļ�����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateInterfaceMenuDefDocFromViewDefToSpecFile(char *viewName,char *mainMenuName,int menuType,char *fileName)
{
	FILE	*fp;
	int	ret;
	
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionGenerateInterfaceMenuDefDocFromViewDefToSpecFile:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	if ((ret = UnionGenerateInterfaceMenuDefDocFromViewDefToFp(viewName,mainMenuName,menuType,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateInterfaceMenuDefDocFromViewDefToSpecFile:: UnionGenerateInterfaceMenuDefDocFromViewDefToFp [%s][%s]\n",viewName,mainMenuName);
	}
	fclose(fp);
	return(ret);
}

/*
����	
	��ȡ�������˵�����
�������
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateAllMainMenuDefToFp(FILE *fp)
{
	FILE		*outFp = stdout;
	int		ret;
	TUnionMainMenu	mainMenuGrp[128];
	PUnionMainMenu	prec;
	int		menuNum;
	int		index;

	// ��ȡ�������˵�	
	if ((menuNum = UnionBatchReadMainMenuRec("",mainMenuGrp,128)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllMainMenuDefToFp:: UnionBatchReadMainMenuRec!\n");
		return(menuNum);
	}
	if (fp != NULL)
		outFp = fp;
	// ��һ���˵�д�뵽�ļ���	
	for (index = 0; index < menuNum; index++)
	{
		prec = &(mainMenuGrp[index]);
		fprintf(outFp,"name=%s|iconFileName=%s|mainMenuName=%s|useServerMenu=%d|useMenuItemButton=%d|\n",
			prec->mainMenuName,prec->iconFileName,prec->mainMenuName,
			prec->useServerMenu,prec->useMenuItemButton);
	}
	return(menuNum);
}



/*
����	
	��ȡ�������˵�����
�������
	
	fileName		���ɵĲ˵��ļ�����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateAllMainMenuDefToSpecFile(char *fileName)
{
	FILE	*fp;
	int	ret;
	
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionGenerateAllMainMenuDefToSpecFile:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	if ((ret = UnionGenerateAllMainMenuDefToFp(fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllMainMenuDefToSpecFile:: UnionGenerateAllMainMenuDefToFp\n");
	}
	fclose(fp);
	return(ret);
}

/*
����	
	����ö���ļ����ƣ�����һ��ö�ٶ����ļ�
�������
	tellerTypeID	����Ա����
	mainMenuName	���˵�����
	enumFileName	ö���ļ�����
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateEnumValueDefFileToFp(char *tellerTypeID,char *mainMenuName,char *enumFileName,FILE *fp)
{
	int			ret;
	int			recNum = 0;
	FILE			*outFp = NULL;
	TUnionEnumValueDef	enumValueDef[128];
	PUnionEnumValueDef	penumValueDef;
	int			index;
	char			condition[256+1];
	int			len;
	
	if (fp != NULL)
		outFp = fp;
	
	// ��ö�ٶ���
	sprintf(condition,"mainMenuName=%s|enumFileName=%s|",mainMenuName,enumFileName);
	if ((recNum = UnionBatchReadEnumValueDefRec(condition,enumValueDef,128)) < 0)
	{
		UnionUserErrLog("in UnionGenerateEnumValueDefFileToFp:: UnionBatchReadEnumValueDefRec [%s][%s]\n",mainMenuName,enumFileName);
		return(recNum);
	}
	fprintf(outFp,"remark=|value=|\n");
	for (index = 0; index < recNum; index++)
	{
		penumValueDef = &(enumValueDef[index]);
		if (((len = strlen(penumValueDef->visualLevelListOfEnum)) > 0) && 
			(UnionIsFldStrInUnionFldListStr(penumValueDef->visualLevelListOfEnum,len,',',tellerTypeID) <= 0))
		{
			continue;
		}
		fprintf(outFp,"remark=%s|value=%s|\n",penumValueDef->itemName,penumValueDef->itemValue);
	}
	return(recNum);
}
/*
����	
	����ö���ļ����ƣ�����һ��ö�ٶ����ļ�
�������
	tellerTypeID	����Ա����
	mainMenuName	���˵�����
	enumFileName	ö���ļ�����
	fileName		���ɵĲ˵��ļ�����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateEnumValueDefFile(char *tellerTypeID,char *mainMenuName,char *enumFileName,char *fileName)
{
	FILE			*outFp = NULL;
	int			ret;
	
	if ((outFp = fopen(fileName,"w")) == NULL)
	{
		UnionUserErrLog("in UnionGenerateEnumValueDefFile:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}

	if ((ret = UnionGenerateEnumValueDefFileToFp(tellerTypeID,mainMenuName,enumFileName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateEnumValueDefFile:: UnionGenerateEnumValueDefFileToFp [%s][%s]!\n",mainMenuName,enumFileName);
		//return(ret);
	}
	fclose(outFp);
	return(ret);
}	
/*
����	
	�������嵥(ÿ������tableField���ж���)������һ�������嵥
�������
	fldList		���ʶ�嵥
	lenOfFldList	���ʶ�嵥�ĳ���
�������
	pfldGrp		����������
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionGenerateQueryFldGrpDefFromFldIDList(char *fldList,int lenOfFldList,PUnionQueryFldGrp pfldGrp)
{
	int			ret;
	TUnionTableField	fldRec;
	int			fldNum,fldIndex;
	char			fldNameGrp[conMaxFldNumPerObject][128];
	//PUnionObjectFldDef	pfldDef;
	
	if ((fldList == NULL) || (lenOfFldList < 0) || (pfldGrp == NULL))
		return(errCodeParameter);

	// ������嵥
	memset(fldNameGrp,0,sizeof(fldNameGrp));
	if ((ret = UnionSeprateVarStrIntoVarGrp(fldList,lenOfFldList,conSeparableSignOfFieldNameInFldList,fldNameGrp,conMaxFldNumPerObject)) < 0)
	{
		UnionUserErrLog("in UnionGenerateQueryFldGrpDefFromFldIDList:: UnionSeprateVarStrIntoVarGrp [%04d][%s]\n",lenOfFldList,fldList);
		return(ret);
	}
	if ((fldNum = ret) > conMaxNumOfQueryFld)
	{
		UnionUserErrLog("in UnionGenerateQueryFldGrpDefFromFldIDList:: fldNum [%d] in [%04d][%s] too much\n",fldNum,lenOfFldList,fldList);
		return(errCodeObjectMDL_FieldNumberTooMuch);
	}
	
	// ��ȡ����
	for (fldIndex = 0; fldIndex < fldNum; fldIndex++)
	{
		memset(&fldRec,0,sizeof(fldRec));
		if (strlen(fldNameGrp[fldIndex]) >= sizeof(fldRec.ID))
		{
			UnionUserErrLog("in UnionGenerateQueryFldGrpDefFromFldIDList:: fldName [%s] too long!\n",fldNameGrp[fldIndex]);
			return(errCodeSQLRecFldNameTooLong);
		}
		strcpy(fldRec.ID,fldNameGrp[fldIndex]);
		if ((ret = UnionReadTableFieldRec(&fldRec)) < 0)
		{
			UnionUserErrLog("in UnionGenerateQueryFldGrpDefFromFldIDList:: UnionReadTableFieldRec [%s]!\n",fldNameGrp[fldIndex]);
			return(ret);
		}
		strcpy(pfldGrp->fldName[fldIndex],fldRec.fieldName);
		strcpy(pfldGrp->fldAlais[fldIndex],fldRec.ID);
	}
	return(pfldGrp->fldNum = fldNum);
}
/*
����	
	���������ʹ��ָ���ı����滻
�������
	fldList		���ʶ�嵥
	lenOfFldList	���ʶ�嵥�ĳ���
�������
	pfldGrp		�滻�������
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionRenameFldOfSpecQueryFldGrpDef(char *fldList,int lenOfFldList,PUnionQueryFldGrp pfldGrp)
{
	int			ret;
	int			fldIndex;
	int			fldAlaisLen;
	char			fldAlaisName[100];
		
	if ((fldList == NULL) || (lenOfFldList < 0) || (pfldGrp == NULL))
		return(errCodeParameter);


	// ��ȡ����
	for (fldIndex = 0; fldIndex < pfldGrp->fldNum; fldIndex++)
	{
		memset(fldAlaisName,0,sizeof(fldAlaisName));
		if ((ret = UnionReadRecFldFromRecStr(fldList,lenOfFldList,pfldGrp->fldAlais[fldIndex],fldAlaisName,sizeof(fldAlaisName))) > 0)
			strcpy(pfldGrp->fldAlais[fldIndex],fldAlaisName);
	}
	return(pfldGrp->fldNum);
}
/*
����	
	���ظ�����ĸ�ֵ�������ļ���
�������
	mainMenuName	���˵�����
	fldID		���ʶ����
	fileName	�ļ�����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateComplexFldAssignmentMethodToSpecFile(char *mainMenuName,char *fldID,char *fileName)
{
	int		ret;
	FILE 		*fp = stdout;
	
	if ((fileName != NULL) && (strlen(fileName) != 0))
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionUserErrLog("in UnionGenerateComplexFldAssignmentMethodToSpecFile:: fopen [%s]\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	if ((ret = UnionGenerateComplexFldAssignmentMethodToFp(mainMenuName,fldID,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateComplexFldAssignmentMethodToSpecFile:: UnionGenerateComplexFldAssignmentMethodToFp!\n");
		//return(ret);
	}
	if (fp != stdout)
		fclose(fp);
	return(ret);
}

/*
����	
	�������嵥(ÿ������tableField���ж���)������һ�������嵥
�������
	fldList		���ʶ�嵥
	lenOfFldList	���ʶ�嵥�ĳ���
�������
	pfldListGrp	����������
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionGenerateQueryFldListFromFldIDList(char *fldList,int lenOfFldList,PUnionFldGrp pfldListGrp)
{
	int			ret;
	TUnionTableField	fldRec;
	int			fldNum = 0,fldIndex = 0;
	char			fldNameGrp[100][128];
	
	if ((fldList == NULL) || (lenOfFldList < 0) || (pfldListGrp == NULL))
		return(errCodeParameter);

	// ������嵥
	memset(fldNameGrp,0,sizeof(fldNameGrp));
	if ((ret = UnionSeprateVarStrIntoVarGrp(fldList,lenOfFldList,conSeparableSignOfFieldNameInFldList,fldNameGrp,100)) < 0)
	{
		UnionUserErrLog("in UnionGenerateQueryFldListFromFldIDList:: UnionSeprateVarStrIntoVarGrp [%04d][%s]\n",lenOfFldList,fldList);
		return(ret);
	}
	if ((pfldListGrp->fldNum = ret) > conMaxNumOfQueryFld)
	{
		UnionUserErrLog("in UnionGenerateQueryFldListFromFldIDList:: fldNum [%d] in [%04d][%s] too much\n",fldNum,lenOfFldList,fldList);
		return(errCodeObjectMDL_FieldNumberTooMuch);
	}
	
	// ��ȡ����
	for (fldIndex = 0; fldIndex < pfldListGrp->fldNum; fldIndex++)
	{
		memset(&fldRec,0,sizeof(fldRec));
		if (strlen(fldNameGrp[fldIndex]) >= sizeof(fldRec.ID))
		{
			UnionUserErrLog("in UnionGenerateQueryFldListFromFldIDList:: fldName [%s] too long!\n",fldNameGrp[fldIndex]);
			return(errCodeSQLRecFldNameTooLong);
		}
		strcpy(fldRec.ID,fldNameGrp[fldIndex]);
		if ((ret = UnionReadTableFieldRec(&fldRec)) < 0)
		{
			UnionUserErrLog("in UnionGenerateQueryFldListFromFldIDList:: UnionReadTableFieldRec [%s] too long!\n",fldNameGrp[fldIndex]);
			return(ret);
		}
		strcpy(pfldListGrp->fldName[fldIndex],fldRec.fieldName);
	}
	return(pfldListGrp->fldNum);
}
/*
����	
	�������嵥(ÿ������tableField���ж���)������һ������������
�������
	fldList		���ʶ�嵥
	lenOfFldList	���ʶ�嵥�ĳ���
	maxNum		������Խ��յ����������
�������
	fldName		����������������
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionGetAllFldNameFromFldIDList(char *fldList,int lenOfFldList,char fldNameGrp[][128+1],int maxNum)
{
	TUnionQueryFldGrp	fldGrp;
	int			fldNum,fldIndex;
	int			ret;
	
	if ((ret = UnionGenerateQueryFldGrpDefFromFldIDList(fldList,lenOfFldList,&fldGrp)) < 0)
	{
		UnionUserErrLog("in UnionGetAllFldNameFromFldIDList:: UnionGenerateQueryFldGrpDefFromFldIDList [%04d][%s]\n",lenOfFldList,fldList);
		return(ret);
	}
	for (fldIndex = 0,fldNum = fldGrp.fldNum; (fldIndex < fldNum) && (fldIndex < maxNum); fldIndex++)
	{
		strcpy(fldNameGrp[fldIndex],fldGrp.fldName[fldIndex]);
	}
	return(fldNum);
}
/*
����	
	�������嵥(ÿ������tableField���ж���)��дһ����ֵ����
�������
	fldList		���ʶ�嵥
	lenOfFldList	���ʶ�嵥�ĳ���
	fieldAlaisList	������嵥
	fieldAlaisListLen	���������
	loopList	ѭ��¼������嵥
	lenOfLoopList	ѭ��¼������嵥�ĳ���
	viewLevelList	�����򼶱��嵥
	lenOfViewLevelList	�����򼶱��嵥����
	editLevelList	�ɱ༭�򼶱��嵥
	lenOfEditLevelList	�ɱ༭�򼶱��嵥����
	fp		�ļ����
�������
	��
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionGenerateAssignmentQueryFldListFromFldIDList(char *fldList,int lenOfFldList,char *fieldAlaisList,int fieldAlaisListLen,
	char *loopList,int lenOfLoopList,char *viewLevelList,int lenOfViewLevelList,char *editLevelList,int lenOfEditLevelList,FILE *fp)
{
	int			ret;
	TUnionTableField	fldRec;
	int			fldNum,fldIndex;
	char			fldNameGrp[100][128];
	FILE			*outFp = stdout;
	char			fldAlaisName[100];
	char			fldTypeName[100];
	char			levelList[512+1];
	char			tmpBuf[512+1];
	int			len;
	
	if ((fldList == NULL) || (lenOfFldList < 0))
		return(errCodeParameter);
	if (fp != NULL)
		outFp = fp;
		
	// ������嵥
	memset(fldNameGrp,0,sizeof(fldNameGrp));
	if ((fldNum = UnionSeprateVarStrIntoVarGrp(fldList,lenOfFldList,conSeparableSignOfFieldNameInFldList,fldNameGrp,100)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAssignmentQueryFldListFromFldIDList:: UnionSeprateVarStrIntoVarGrp [%04d][%s]\n",lenOfFldList,fldList);
		return(fldNum);
	}
	
	fprintf(outFp,"\n");
	//UnionLog("in UnionGenerateAssignmentQueryFldListFromFldIDList:: *********\n");
	//UnionNullLog("[%04d][%s]\n[%04d][%s]\n[%04d][%s]\n",lenOfLoopList,loopList,lenOfViewLevelList,viewLevelList,lenOfEditLevelList,editLevelList);
	// ��ȡ����
	for (fldIndex = 0; fldIndex < fldNum; fldIndex++)
	{
		memset(&fldRec,0,sizeof(fldRec));
		if (strlen(fldNameGrp[fldIndex]) >= sizeof(fldRec.ID))
		{
			UnionUserErrLog("in UnionGenerateAssignmentQueryFldListFromFldIDList:: fldName [%s] too long!\n",fldNameGrp[fldIndex]);
			return(errCodeSQLRecFldNameTooLong);
		}
		strcpy(fldRec.ID,fldNameGrp[fldIndex]);
		if ((ret = UnionReadTableFieldRec(&fldRec)) < 0)
		{
			UnionUserErrLog("in UnionGenerateAssignmentQueryFldListFromFldIDList:: UnionReadTableFieldRec [%s] too long!\n",fldNameGrp[fldIndex]);
			return(ret);
		}
		memset(fldAlaisName,0,sizeof(fldAlaisName));
		if ((ret = UnionReadRecFldFromRecStr(fieldAlaisList,fieldAlaisListLen,fldRec.ID,fldAlaisName,sizeof(fldAlaisName))) <= 0)
			strcpy(fldAlaisName,fldRec.ID);
		fprintf(outFp,"valueFldName=%s|fldAlais=%s|",fldRec.fieldName,fldAlaisName);
		if ((ret = UnionPrintFldRecValueAssignMethodToFp(fldRec.value,outFp)) < 0)
		{
			UnionUserErrLog("in UnionGenerateAssignmentQueryFldListFromFldIDList:: UnionPrintFldRecValueAssignMethodToFp!\n");
			return(ret);
		}
		//fprintf(outFp,"value=%s|",fldRec.value);
		memset(fldTypeName,0,sizeof(fldTypeName));
		UnionConvertTBLFldTypeIntoName(fldRec.fieldType,fldTypeName);
		if (fldRec.nullPermitted)
			fprintf(outFp,"valueConstraint=%s|",fldRec.valueConstraint);
		else
		{
			fprintf(outFp,"valueConstraint=notnull");
			if (strlen(fldRec.valueConstraint) != 0)	
				fprintf(outFp," %s",fldRec.valueConstraint);
			fprintf(outFp,"|");
		}
		fprintf(outFp,"help=%s|type=%s|size=%d|",fldRec.remark,fldTypeName,fldRec.size);
		//UnionLog("fldName = [%s][%s]\n",fldRec.fieldName,fldAlaisName);
		if (UnionIsFldStrInUnionFldListStr(loopList,lenOfLoopList,',',fldNameGrp[fldIndex]))	// ���Ƿ���ѭ����ֵ��
			fprintf(outFp,"loop=1|");
		// ���Ƿ��ǿɼ�
		memset(tmpBuf,0,sizeof(tmpBuf));
		len = UnionConvertOneFldSeperatorInRecStrIntoAnother(viewLevelList,lenOfViewLevelList,',','|',tmpBuf,sizeof(tmpBuf));
		memset(levelList,0,sizeof(levelList));
		if ((ret = UnionReadRecFldFromRecStr(tmpBuf,len,fldNameGrp[fldIndex],levelList,sizeof(levelList))) > 0)
			fprintf(outFp,"viewLevel=%s|",levelList);
		// ���Ƿ��ǿɱ༭
		memset(tmpBuf,0,sizeof(tmpBuf));
		len = UnionConvertOneFldSeperatorInRecStrIntoAnother(editLevelList,lenOfEditLevelList,',','|',tmpBuf,sizeof(tmpBuf));
		memset(levelList,0,sizeof(levelList));
		if ((ret = UnionReadRecFldFromRecStr(tmpBuf,len,fldNameGrp[fldIndex],levelList,sizeof(levelList))) > 0)
			fprintf(outFp,"editLevel=%s|",levelList);
		fprintf(outFp,"\n");
	}
	return(fldNum);
}
/*
����	
	��һ����ͼ�Ĳ�����Ȩд�뵽�ļ���
�������
	mainMenuName	���˵�����
	viewName	��ͼ������
	fp		�ļ�����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateViewAuthorizationToFp(char *mainMenuName,char *viewName,FILE *fp)
{
	FILE			*outFp = stdout;
	int			ret;
	TUnionViewList		viewRec;
	char			fldNameGrp[128][128];
	int			fldNum,fldIndex;
	char			tmpBuf[128];
	int			displayIndex;
	char			tellerLevelStr[20];
	
	if (fp != NULL)
		outFp = fp;
	
	// ��ȡ���й�Ա����
	memset(tellerLevelStr,0,sizeof(tellerLevelStr));
	if ((ret = UnionReadAllOperatorLevelStr(tellerLevelStr)) < 0)
	{
		UnionUserErrLog("in UnionGenerateViewAuthorizationToFp:: UnionReadAllOperatorLevelStr!\n");
		return(ret);
	}
			
	// ��ȡ��ͼ
	memset(&viewRec,0,sizeof(viewRec));
	if ((ret = UnionReadViewListRec(viewName,mainMenuName,&viewRec)) < 0)
	{
		UnionUserErrLog("in UnionGenerateViewAuthorizationToFp:: UnionReadViewListRec [%s] [%s]!\n",viewName,mainMenuName);
		return(ret);
	}
	UnionConvertOneFldSeperatorInRecStrIntoAnother(viewRec.fieldAlaisList,strlen(viewRec.fieldAlaisList),',','|',viewRec.fieldAlaisList,sizeof(viewRec.fieldAlaisList));
	UnionConvertOneFldSeperatorInRecStrIntoAnother(viewRec.viewLevel,strlen(viewRec.viewLevel),',','|',viewRec.viewLevel,sizeof(viewRec.viewLevel));
	UnionConvertOneFldSeperatorInRecStrIntoAnother(viewRec.editLevel,strlen(viewRec.editLevel),',','|',viewRec.editLevel,sizeof(viewRec.editLevel));

	// �����ͼ���õı����嵥
	memset(fldNameGrp,0,sizeof(fldNameGrp));
	if ((fldNum = UnionSeprateVarStrIntoVarGrp(viewRec.fieldList,strlen(viewRec.fieldList),conSeparableSignOfFieldNameInFldList,fldNameGrp,128)) < 0)
	{
		UnionUserErrLog("in UnionGenerateViewAuthorizationToFp:: UnionSeprateVarStrIntoVarGrp [%04d][%s]\n",strlen(viewRec.fieldList),viewRec.fieldList);
		return(fldNum);
	}
	
	//fprintf(outFp,"\n");
	for (fldIndex = 0; fldIndex < fldNum; fldIndex++)
	{
		fprintf(outFp,"ID=%s|",fldNameGrp[fldIndex]);
		// �������Ƿ�����ͼ��
		if ((displayIndex = UnionIsFldStrInUnionFldListStr(viewRec.fieldDisplayList,strlen(viewRec.fieldDisplayList),
			',',fldNameGrp[fldIndex])) <= 0)
		{
			// ������ͼ��
			fprintf(outFp,"isVisualable=0|displayIndex=|displayName=|isBatchInputFld=0|");
			UnionPrintAllOperatorLevelStrToFp(tellerLevelStr,"visualLevel",0,outFp);			
			UnionPrintAllOperatorLevelStrToFp(tellerLevelStr,"editLevel",0,outFp);		
			fprintf(outFp,"\n");	
			continue;
		}			
		// ����ͼ��
		fprintf(outFp,"isVisualable=1|displayIndex=%d|",displayIndex);
		// ���һ�����Ƿ�Ҫ������
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadRecFldFromRecStr(viewRec.fieldAlaisList,strlen(viewRec.fieldAlaisList),fldNameGrp[fldIndex],tmpBuf,sizeof(tmpBuf))) <= 0)
			strcpy(tmpBuf,fldNameGrp[fldIndex]);
		fprintf(outFp,"displayName=%s|",tmpBuf);
		// �Ƿ���������
		if (UnionIsFldStrInUnionFldListStr(viewRec.loopList,strlen(viewRec.loopList),',',fldNameGrp[fldIndex]) > 0)
			fprintf(outFp,"isBatchInputFld=1|");
		else
			fprintf(outFp,"isBatchInputFld=0|");
		// ���Ӽ���
		if ((ret = UnionReadRecFldFromRecStr(viewRec.viewLevel,strlen(viewRec.viewLevel),fldNameGrp[fldIndex],tmpBuf,sizeof(tmpBuf))) <= 0)
			sprintf(tmpBuf,"0123456789");
		UnionPrintAllAuthOperatorLevelStrToFp(tellerLevelStr,"visualLevel",tmpBuf,outFp);
		// �ɱ༭����
		if ((ret = UnionReadRecFldFromRecStr(viewRec.editLevel,strlen(viewRec.editLevel),fldNameGrp[fldIndex],tmpBuf,sizeof(tmpBuf))) <= 0)
			sprintf(tmpBuf,"0123456789");
		UnionPrintAllAuthOperatorLevelStrToFp(tellerLevelStr,"editLevel",tmpBuf,outFp);
		fprintf(outFp,"\n");
	}
	return(0);
}

/*
����	
	����һ���˵���Ķ����ĵ�
�������
	mainMenuName	���˵�����
	menuGrpName	�˵�����
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateMenuGrpDefDocToFp(char *mainMenuName,char *menuGrpName,FILE *fp)
{
	int			ret;
	int			menuItemDefNum = 0;
	FILE			*outFp = NULL;
	TUnionMenuItemDef	menuItemDefRec;
	PUnionMenuItemDef	pmenuItemDefRec;
	int			index;
	int			itemNum = 0;
	TUnionMenuDef		menuDef;
	char			menuItemNameGrp[48][128];
	TUnionOperatorType 	tellerTypeGrp[16];
	int			tellerTypeNum;
		
	// ��ȡ���й�Ա����
	if ((tellerTypeNum = UnionBatchReadOperatorTypeRec("",tellerTypeGrp,16)) < 0)
	{
		UnionUserErrLog("in UnionGenerateMenuGrpDefDocToFp:: UnionBatchReadOperatorTypeRec!\n");
		return(tellerTypeNum);
	}

	if (fp != NULL)
		outFp = fp;
	
	// ���˵��鶨��
	memset(&menuDef,0,sizeof(menuDef));
	if ((ret = UnionReadMenuDefRec(mainMenuName,menuGrpName,&menuDef)) < 0)
	{
		UnionUserErrLog("in UnionGenerateMenuGrpDefDocToFp:: UnionReadMenuDefRec [%s][%s]\n",mainMenuName,menuGrpName);
		return(ret);
	}
	// ���ò˵������Ĳ˵���
	if ((menuItemDefNum = UnionSeprateVarStrIntoVarGrp(menuDef.menuItemsList,strlen(menuDef.menuItemsList),',',menuItemNameGrp,48)) < 0)
	{
		UnionUserErrLog("in UnionGenerateMenuGrpDefDocToFp:: UnionSeprateVarStrIntoVarGrp [%s]\n",menuDef.menuItemsList);
		return(menuItemDefNum);
	}
	for (index = 0; index < menuItemDefNum; index++)
	{
		// ���˵����
		memset(&menuItemDefRec,0,sizeof(menuItemDefRec));
		if ((ret = UnionReadMenuItemDefRec(mainMenuName,menuGrpName,menuItemNameGrp[index],&menuItemDefRec)) < 0)
		{
			UnionAuditLog("in UnionGenerateMenuGrpDefDocToFp:: UnionReadMenuItemDefRec [%s][%s] not exists\n",menuGrpName,menuItemNameGrp[index]);
			continue;
		}
		pmenuItemDefRec = &menuItemDefRec;
		fprintf(outFp,"menuDisplayname=%s|menuName=%s|viewName=%s|svrName=%s|otherProperty=%s|remark=%s|",
			pmenuItemDefRec->menuDisplayname,
			pmenuItemDefRec->menuName,
			pmenuItemDefRec->viewName,
			pmenuItemDefRec->svrName,
			pmenuItemDefRec->otherProperty,
			pmenuItemDefRec->remark);
		UnionPrintAllAuthOperatorLevelIDStrToFp(tellerTypeGrp,tellerTypeNum,"level",pmenuItemDefRec->visualLevelListOfMenu,outFp);
		fprintf(outFp,"\n");
		itemNum++;
	}
	return(itemNum);
}

/*
����	
	���ظ�����ĸ�ֵ�������ļ���
�������
	mainMenuName	���˵�����
	fldID		���ʶ����
	fp		�ļ���ʶ
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateComplexFldAssignmentMethodToFp(char *mainMenuName,char *fldID,FILE *fp)
{
	int			ret;
	TUnionComplexField	fldRec;
	TUnionTableField	fldDef;
	char			fldNameGrp[48][128];
	int			fldNum;
	FILE			*outFp = stdout;
	int			index;
	char			seperatorTag[100];
	char			fldIndexTag[10];
	int			len;
	char			colFldList[512+1];
	int			offset = 0;
	int			fldNameLen;
	
	memset(&fldRec,0,sizeof(fldRec));
	if ((ret = UnionReadComplexFieldRec(mainMenuName,fldID,&fldRec)) < 0)
	{
		UnionUserErrLog("in UnionGenerateComplexFldAssignmentMethodToFp:: UnionReadComplexFieldRec mainMenuName = [%s] fldID = [%s]\n",mainMenuName,fldID);
		return(ret);
	}
	
	if (fp != NULL)
		outFp = fp;
	
	// ������嵥
	memset(fldNameGrp,0,sizeof(fldNameGrp));
	if ((fldNum = UnionSeprateVarStrIntoVarGrp(fldRec.tableFldList,strlen(fldRec.tableFldList),conSeparableSignOfFieldNameInFldList,fldNameGrp,48)) < 0)
	{
		UnionUserErrLog("in UnionGenerateComplexFldAssignmentMethodToFp:: UnionSeprateVarStrIntoVarGrp [%04d][%s]\n",strlen(fldRec.tableFldList),fldRec.tableFldList);
		return(fldNum);
	}

	fprintf(outFp,"colNums=%d\n",fldNum);
	fprintf(outFp,"maxNumOfLine=%d\n",fldRec.maxNumOfLine);
	fprintf(outFp,"minNumOfLine=%d\n",fldRec.minNumOfLine);
	fprintf(outFp,"colConnectStrDefault=%s\n",fldRec.fieldSeparator);
	fprintf(outFp,"LineConnectStr=%s\n",fldRec.lineSeparator);
	//fprintf(outFp,"%s\n",fldRec.tableFldList);
	for (index = 0; index < fldNum; index++)
	{
		memset(&fldDef,0,sizeof(fldDef));
		if (strlen(fldNameGrp[index]) >= sizeof(fldDef.ID))
		{
			UnionUserErrLog("in UnionGenerateComplexFldAssignmentMethodToFp:: fldName [%s] too long!\n",fldNameGrp[index]);
			return(errCodeSQLRecFldNameTooLong);
		}
		strcpy(fldDef.ID,fldNameGrp[index]);
		if ((ret = UnionReadTableFieldRec(&fldDef)) < 0)
		{
			UnionUserErrLog("in UnionGenerateComplexFldAssignmentMethodToFp:: UnionReadTableFieldRec [%s]!\n",fldNameGrp[index]);
			return(ret);
		}
		fprintf(outFp,"assignValue%d=%s\n",index,fldDef.value);
		if (index > 0)
		{
			colFldList[offset] = ',';
			offset++;
		}
		memcpy(colFldList+offset,fldDef.fieldName,fldNameLen=strlen(fldDef.fieldName));
		offset += fldNameLen;
	}
	colFldList[offset] = 0;
	fprintf(outFp,"colNames=%s\n",colFldList);
	len = strlen(fldRec.specialFieldSeparators);	
	UnionConvertOneFldSeperatorInRecStrIntoAnother(fldRec.specialFieldSeparators,len,',','|',fldRec.specialFieldSeparators,sizeof(fldRec.specialFieldSeparators));
	UnionLog("in UnionGenerateComplexFldAssignmentMethodToFp:: maxNumOfLine = [%d]\n",fldRec.maxNumOfLine);
	for (index = 0; index < fldRec.maxNumOfLine; index++)
	{
		memset(seperatorTag,0,sizeof(seperatorTag));
		sprintf(fldIndexTag,"%d",index);
		if ((ret = UnionReadRecFldFromRecStr(fldRec.specialFieldSeparators,len,fldIndexTag,seperatorTag,sizeof(seperatorTag))) > 0)
			fprintf(outFp,"colConnectStr%d=%s\n",index,seperatorTag);
	}

	len = strlen(fldRec.specialLineTag);	
	UnionConvertOneFldSeperatorInRecStrIntoAnother(fldRec.specialLineTag,len,',','|',fldRec.specialLineTag,sizeof(fldRec.specialLineTag));
	for (index = 0; index <  fldRec.maxNumOfLine; index++)
	{
		memset(seperatorTag,0,sizeof(seperatorTag));
		sprintf(fldIndexTag,"%d",index);
		if ((ret = UnionReadRecFldFromRecStr(fldRec.specialLineTag,len,fldIndexTag,seperatorTag,sizeof(seperatorTag))) > 0)
			fprintf(outFp,"lineContent%d=%s\n",index,seperatorTag);
	}
	return(fldNum);
}
/*
����	
	��һ����ֵ����д�뵽�ļ���
�������
	method		��ֵ����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionPrintFldRecValueAssignMethodToFp(char *method,FILE *fp)
{
	FILE	*outFp = stdout;
	char	tmpBuf[512+1];
	int	ret;
	
	if (fp != NULL)
		outFp = fp;
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionConvertValueAssignMethod(method,tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionPrintFldRecValueAssignMethodToFp:: UnionConvertValueAssignMethod [%s]!\n",method);
		return(ret);
	}
	fprintf(outFp,"value=%s|",tmpBuf);
	return(0);
}

/*
����	
	�����м�����ʾ���ļ���
�������
	levelStr	���в���Ա���𹹳ɵĴ�
	levelName	������ʾ����
	authoSet	��Ȩ��־
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionPrintAllOperatorLevelStrToFp(char *levelStr,char *levelName,int authSet,FILE *fp)
{
	int	offset;
	int	len;
	FILE	*outFp = stdout;
	
	if (fp != NULL)
		outFp = fp;
	len = strlen(levelStr);
	for (offset = 0; offset < len; offset++)
	{
		fprintf(outFp,"%s%c=",levelName,levelStr[offset]);
		if (authSet != 0)
			fprintf(outFp,"%d|",authSet);
		else
			fprintf(outFp,"|");
	}
	return(offset);
}

/*
����	
	��ȡ���в���Ա����
�������
	��
�������
	levelStr	���в���Ա���𹹳ɵĴ�
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionReadAllOperatorLevelStr(char *levelStr)
{
	TUnionOperatorType 	tellerTypeGrp[16];
	int			tellerTypeNum;
	int			ret;
		
	// ��ȡ���й�Ա����
	if ((tellerTypeNum = UnionBatchReadOperatorTypeRec("",tellerTypeGrp,16)) < 0)
	{
		UnionUserErrLog("in UnionReadAllOperatorLevelStr:: UnionBatchReadOperatorTypeRec!\n");
		return(tellerTypeNum);
	}
	return(UnionFormAllOperatorLevelStr(tellerTypeGrp,tellerTypeNum,levelStr));
}

/*
����	
	����Ȩ������ʾ���ļ���
�������
	levelStr	���в���Ա���𹹳ɵĴ�
	levelName	������ʾ����
	authLevelStr	��Ȩ����
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionPrintAllAuthOperatorLevelStrToFp(char *levelStr,char *levelName,char *authLevelStr,FILE *fp)
{
	int	offset;
	int	len;
	FILE	*outFp = stdout;
	int	levelGrp[10];
	int	levelIndex;
	int	level;
	int	authLevelNum = 0;
		
	for (levelIndex = 0; levelIndex < 10; levelIndex++)
		levelGrp[levelIndex] = 0;
	len = strlen(levelStr);
	for (offset = 0; offset < len; offset++)
	{
		level = levelStr[offset] - '0';
		if ((level >= 10) || (level < 0))
			continue;
		levelGrp[level] = 1;
	}
	if (fp != NULL)
		outFp = fp;
	len = strlen(authLevelStr);
	for (offset = 0; offset < len; offset++)
	{
		level = authLevelStr[offset] - '0';
		if ((level >= 10) || (level < 0))
			continue;
		if (levelGrp[level] != 1)
			continue;
		levelGrp[level] += 1;
	}
	for (levelIndex = 0; levelIndex < 10; levelIndex++)
	{
		switch (levelGrp[levelIndex])
		{
			case	1:
				fprintf(outFp,"%s%d=|",levelName,levelIndex);
				break;
			case	2:
				fprintf(outFp,"%s%d=1|",levelName,levelIndex);
				authLevelNum++;
				break;
			default:
				break;
		}
	}
	return(authLevelNum);
}
			
/*
����	
	����Ȩ������ʾ���ļ���
�������
	tellerTypeGrp	���в���Ա������
	tellerTypeNum	����Ա��������
	levelName	������ʾ����
	authLevelIDStr	��Ȩ�����ʶ��
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionPrintAllAuthOperatorLevelIDStrToFp(TUnionOperatorType tellerTypeGrp[],int tellerTypeNum,char *levelName,char *authLevelIDStr,FILE *fp)
{
	int			ret;
	char			authLevelStr[40];
	char			levelStr[40];
	
	memset(levelStr,0,sizeof(levelStr));
	UnionFormAllOperatorLevelStr(tellerTypeGrp,tellerTypeNum,levelStr);
	//UnionNullLog("*** [%s]\n",levelStr);
	memset(authLevelStr,0,sizeof(authLevelStr));
	UnionReadAllOperatorLevelStrOfSpecLevelIDList(authLevelIDStr,tellerTypeGrp,tellerTypeNum,authLevelStr);
	//UnionNullLog("*** [%s]\n",authLevelStr);
	return(UnionPrintAllAuthOperatorLevelStrToFp(levelStr,levelName,authLevelStr,fp));
}

/*
����	
	������в���Ա�����嵥��
�������
	tellerTypeGrp	���в���Ա������
	tellerTypeNum	����Ա��������
�������
	levelStr	����Ա���𹹳ɵĴ�
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionFormAllOperatorLevelStr(TUnionOperatorType tellerTypeGrp[],int tellerTypeNum,char *levelStr)
{
	int			ret;
	int			levelIndex;
	int			offset = 0;
	int			levelGrp[10];
		
	for (levelIndex = 0; levelIndex < 10; levelIndex++)
		levelGrp[levelIndex] = 0;
	for (levelIndex = 0; levelIndex < tellerTypeNum; levelIndex++)
	{
		if ((tellerTypeGrp[levelIndex].macroValue >= 10) || (tellerTypeGrp[levelIndex].macroValue < 0))
			continue;
		levelGrp[tellerTypeGrp[levelIndex].macroValue] = 1;
	}
	for (levelIndex = 0; levelIndex < 10; levelIndex++)
	{
		if (levelGrp[levelIndex] == 0)
			continue;
		levelStr[offset] = levelIndex + '0';
		offset++;
	}
	levelStr[offset] = 0;
	return(offset);
}

/*
����	
	��ȡ����Ա�����ʶ�嵥��Ӧ�Ĳ���Ա�����嵥
�������
	tellerTypeGrp	���в���Ա������
	tellerTypeNum	����Ա��������
	levelIDStr	����Ա�����ʶ�嵥
�������
	levelStr	����Ա���𹹳ɵĴ�
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionReadAllOperatorLevelStrOfSpecLevelIDList(char *levelIDStr,TUnionOperatorType tellerTypeGrp[],int tellerTypeNum,char *levelStr)
{
	int			ret;
	int			levelIndex;
	int			len;
	int			offset = 0;
	
	len = strlen(levelIDStr);
	//UnionNullLog("xxxx [%s] = [%s]\n",levelIDStr);
	for (levelIndex = 0; levelIndex < tellerTypeNum; levelIndex++)
	{
		if (UnionIsFldStrInUnionFldListStr(levelIDStr,len,',',tellerTypeGrp[levelIndex].ID) <= 0)
			continue;
		if ((tellerTypeGrp[levelIndex].macroValue >= 10) || (tellerTypeGrp[levelIndex].macroValue < 0))
			continue;
		levelStr[offset] = tellerTypeGrp[levelIndex].macroValue % 10 + '0';
		offset++;
	}
	levelStr[offset] = 0;
	return(offset);
}

