//	Wolfgang Wang
//	2009/4/29

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#include "unionComplexDBCommon.h"
#include "unionComplexDBObjectDef.h"
#include "unionErrCode.h"
#include "unionDatabaseCmd.h"
//#include "unionResID.h"
#include "UnionStr.h"
#include "unionGenIncFromTBL.h"
#include "unionGenTBLDefFromTBL.h"
#include "mngSvrConfFileName.h"
#include "mngSvrCommProtocol.h"

#include "unionComplexField.h"
#include "unionFldGrp.h"
#include "unionRecFile.h"
#include "unionTBLQueryConf.h"
#include "unionTableField.h"
#include "unionTableList.h"
#include "unionDataTBLList.h"
#include "unionOperationList.h"
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
#include "unionUserInterfaceMaintainer.h"
#include "unionUserInterfaceItemGrp.h"
#include "UnionLog.h"
#include "unionComplexDBRecord.h"

/*
����	
	��һ������ֵ�����У���ȡ��Դ���ƺ���Դ����
�������
	valueStr	��ֵ��
	sizeOfResName	��Դ���ƻ����С
�������
	resName		��Դ����
����ֵ
	>=0	�ɹ�,��Դ����
	<0	������
*/
int UnionReadAttrFromFieldMethodDef(char *valueStr,char *resName,int sizeOfResName)
{
	int	len;
	char	*ptr = NULL;
	int	resType = conTblFldValueMethodUseNonRes;
	char	*ptr2 = NULL,tmpChar;
	int	copyLen;
	int	offset = 0;
	
	if ((len = strlen(valueStr)) == 0)
		return(resType);
	if ((ptr = strstr(valueStr,"query::table=")) != NULL)
	{
		offset = strlen("query::table=");
		resType = conTblFldValueMethodUseTable;
	}
	else if ((ptr = strstr(valueStr,"query::table=")) != NULL)
	{
		offset = strlen("form::table=");
		resType = conTblFldValueMethodUseTable;
	}
	else if ((ptr = strstr(valueStr,"useOtherFld::table=")) != NULL)
	{
		offset = strlen("useOtherFld::table=");
		resType = conTblFldValueMethodUseTable;
	}
	else if ((ptr = strstr(valueStr,"otherFld::table=")) != NULL)
	{
		offset = strlen("otherFld::table=");
		resType = conTblFldValueMethodUseTable;
	}
	else if ((ptr = strstr(valueStr,"enum::")) != NULL)
	{
		offset = strlen("enum::");
		resType = conTblFldValueMethodUseEnum;
	}
	else if ((ptr = strstr(valueStr,"auto.enum.")) != NULL)
	{
		offset = strlen("auto.enum.");
		resType = conTblFldValueMethodUseEnum;
	}
	else if ((ptr = strstr(valueStr,"enum.")) != NULL)
	{
		offset = strlen("enum.");
		resType = conTblFldValueMethodUseEnum;
	}
	else if ((ptr = strstr(valueStr,"ruleCols::table=73,condition=fldID=")) != NULL)
	{
		offset = strlen("ruleCols::table=73,condition=fldID=");
		resType = conTblFldValueMethodUseComplexFld;
	}
	else if ((ptr = strstr(valueStr,"complexFld")) != NULL)
	{
		offset = strlen("complexFld");
		resType = conTblFldValueMethodUseComplexFld;
	}
	else if ((ptr = strstr(valueStr,"complexField")) != NULL)
	{
		offset = strlen("complexField");
		resType = conTblFldValueMethodUseComplexFld;
	}
	else
		return(resType);
	
	if (resType == conTblFldValueMethodUseComplexFld)
		copyLen = strlen(ptr);
	else if (resType == conTblFldValueMethodUseTable)
	{
		if ((ptr2 = strstr(ptr,",")) == NULL)
			copyLen = strlen(ptr);
		else
		{
			tmpChar = ptr2[0];
			ptr2[0] = 0;
			copyLen = strlen(ptr);
			ptr2[0] = tmpChar;
		}
	}
	else if (resType == conTblFldValueMethodUseEnum)
	{
		if ((ptr2 = strstr(ptr,"^table=")) == NULL)
			copyLen = strlen(ptr);
		else	// �Ǳ�
		{
			resType = conTblFldValueMethodUseTable;
			ptr = ptr2;
			offset = strlen("^table=");
			if ((ptr2 = strstr(ptr,",")) == NULL)
				copyLen = strlen(ptr);
			else
			{
				tmpChar = ptr2[0];
				ptr2[0] = 0;
				copyLen = strlen(ptr);
				ptr2[0] = tmpChar;
			}
		}
	}		
	copyLen = copyLen - offset;
	if (copyLen >= sizeOfResName)
		copyLen = sizeOfResName - 1;
	if (copyLen < 0)
		copyLen = 0;
	memcpy(resName,ptr+offset,copyLen);
	return(resType);
}

/*
����	
	����һ�����˵��õ���ָ���Ľ�����Ϣ
�������
	mainMenuName	���˵�����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateAllInterfaceItemsOfSpecMainMenu(char *mainMenuName)
{
	int			ret;
	char			menuGrpList[1024+1];
	char			menuGrp[48][128];
	int			menuGrpNum;
	int			index;
	int			lenOfMenuGrpList;
	char			remark[256+1];
	
	memset(remark,0,sizeof(remark));
		
	// �������˵��Ķ����˵����嵥
	memset(menuGrpList,0,sizeof(menuGrpList));
	if ((lenOfMenuGrpList = UnionReadMainMenuRecFld(mainMenuName,conMainMenuFldNameMainMenuItemsList,menuGrpList,sizeof(menuGrpList))) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecMainMenu:: UnionReadMainMenuRecFld [%s]!\n",mainMenuName);
		return(lenOfMenuGrpList);
	}
	
	// ƴ�ֶ����˵���
	if ((menuGrpNum = UnionSeprateVarStrIntoVarGrp(menuGrpList,lenOfMenuGrpList,',',menuGrp,48)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecMainMenu::UnionSeprateVarStrIntoVarGrp [%s] of [%s]!\n",menuGrpList,mainMenuName);
		return(menuGrpNum);
	}
		
	for (index = 0; index < menuGrpNum; index++)
	{
		if ((ret = UnionGenerateAllInterfaceItemsOfSpecMenuGrp(mainMenuName,conMenuItemTypeSecondaryMenuGrp,menuGrp[index])) < 0)
		{
			UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecMainMenu::UnionGenerateAllInterfaceItemsOfSpecMenuGrp [%s][%s]!\n",
					mainMenuName,menuGrp[index]);
			strcpy(remark,"���ɶ����˵������,UnionGenerateAllInterfaceItemsOfSpecMenuGrp");
		}
	}
	return(UnionAddInterfaceItemToItemGrp(remark,conInterfaceItemTagMainMenu,"mainMenuName=%s|",mainMenuName));
}

/*
����	
	�ռ�һ�����˵��õ���ָ���Ľ�����Ϣ
�������
	mainMenuName	���˵�����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionCollectAllInterfaceItemsOfSpecMainMenu(char *mainMenuName)
{
	int	ret;
	
	// �жϽ����ǰ�Ƿ����ڴ���
	if ((ret = UnionInitInterfaceItemGrp(mainMenuName)) < 0)
	{
		UnionUserErrLog("in UnionCollectAllInterfaceItemsOfSpecMainMenu:: UnionInitInterfaceItemGrp!\n");
		return(ret);
	}
	
	if ((ret = UnionGenerateAllInterfaceItemsOfSpecMainMenu(mainMenuName)) < 0)
	{
		UnionUserErrLog("in UnionCollectAllInterfaceItemsOfSpecMainMenu::UnionGenerateAllInterfaceItemsOfSpecMainMenu [%s]!\n",mainMenuName);
		UnionDeleteInterfaceItemGrp();
		return(ret);
	}
	
	return(ret);
}

/*
����	
	�ͷŵ�ǰ�ռ��������˵��õ���ָ���Ľ�����Ϣ
�������
	��
�������
	��
����ֵ
	��
*/
void UnionFreeAllCollectedInterfaceItemsOfSpecMainMenu(char *mainMenuName __attribute__((unused)))
{
	UnionDeleteInterfaceItemGrp();
	return;
}

/*
����	
	����һ�����˵��õ���ָ���Ľ�����Ϣ���������ָ���ļ���
�������
	mainMenuName	���˵�����
	fileName	�ļ�����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateAllInterfaceItemsOfSpecMainMenuToSpecFile(char *mainMenuName,char *fileName)
{
	int	ret;
	
	// �жϽ����ǰ�Ƿ����ڴ���
	if ((ret = UnionInitInterfaceItemGrp(mainMenuName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecMainMenuToSpecFile:: UnionInitInterfaceItemGrp!\n");
		return(ret);
	}
	
	if ((ret = UnionGenerateAllInterfaceItemsOfSpecMainMenu(mainMenuName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecMainMenuToSpecFile::UnionGenerateAllInterfaceItemsOfSpecMainMenu [%s]!\n",mainMenuName);
		UnionDeleteInterfaceItemGrp();
		return(ret);
	}
	
	if ((ret = UnionPrintInterfaceItemGrpToSpecFile(fileName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecMainMenuToSpecFile::UnionPrintInterfaceItemGrpToSpecFile [%s]!\n",mainMenuName);
		UnionDeleteInterfaceItemGrp();
		return(ret);
	}
	UnionDeleteInterfaceItemGrp();
	return(ret);
}
		
/*
����	
	����һ�����˵��Ĵ��������Ϣ���������ָ���ļ���
�������
	mainMenuName	���˵�����
	fileName	�ļ�����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateAllErrorInterfaceItemsOfSpecMainMenuToSpecFile(char *mainMenuName,char *fileName)
{
	int	ret;
	
	// �жϽ����ǰ�Ƿ����ڴ���
	if ((ret = UnionInitInterfaceItemGrp(mainMenuName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllErrorInterfaceItemsOfSpecMainMenuToSpecFile:: UnionInitInterfaceItemGrp!\n");
		return(ret);
	}
	
	if ((ret = UnionGenerateAllInterfaceItemsOfSpecMainMenu(mainMenuName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllErrorInterfaceItemsOfSpecMainMenuToSpecFile::UnionGenerateAllInterfaceItemsOfSpecMainMenu [%s]!\n",mainMenuName);
		UnionDeleteInterfaceItemGrp();
		return(ret);
	}
	
	if ((ret = UnionPrintErrorInterfaceItemGrpToSpecFile(fileName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllErrorInterfaceItemsOfSpecMainMenuToSpecFile::UnionPrintErrorInterfaceItemGrpToSpecFile [%s]!\n",mainMenuName);
		UnionDeleteInterfaceItemGrp();
		return(ret);
	}
	UnionDeleteInterfaceItemGrp();
	return(ret);
}
		
/*
����	
	����һ���˵����õ���ָ���Ľ�����Ϣ
�������
	mainMenuName	���˵�����
	menuType	�˵�����
	itemGrpName	�˵�������
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateAllInterfaceItemsOfSpecMenuGrp(char *mainMenuName,int menuType,char *menuGrpName)
{
	int			ret;
	char			menuItemsList[1024+1];
	char			itemGrp[48][128];
	int			itemGrpNum;
	int			index;
	int			lenOfMenuItemList;
	char			remark[256+1];
	
	memset(remark,0,sizeof(remark));
	
	if (strlen(menuGrpName) == 0)
		return(0);
		
	// �жϽ����ǰ�Ƿ����ڴ���
	if ((ret = UnionSetInterfaceItemDealCurrently(conInterfaceItemTagMenuGrp,"mainMenuName=%s|tableMenuType=%d|menuGrpName=%s|",mainMenuName,menuType,menuGrpName)) > 0)
		return(0);
	
	//UnionRealNullLog("in UnionGenerateAllInterfaceItemsOfSpecMenuGrp:: deal [%s][%s]...\n",mainMenuName,menuGrpName);
	// �����˵���Ĳ˵����嵥
	memset(menuItemsList,0,sizeof(menuItemsList));
	if ((lenOfMenuItemList = UnionReadMenuDefRecFld(mainMenuName,menuGrpName,conMenuDefFldNameMenuItemsList,menuItemsList,sizeof(menuItemsList))) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecMenuGrp:: UnionReadMenuDefRecFld [%s][%s]!\n",mainMenuName,menuGrpName);
		strcpy(remark,"��ȡ�˵���Ĳ˵����嵥����,UnionReadMenuDefRecFld");
		ret = lenOfMenuItemList;
		goto errExit;
	}
	
	// ƴ�ֶ����˵���
	if ((itemGrpNum = UnionSeprateVarStrIntoVarGrp(menuItemsList,lenOfMenuItemList,',',itemGrp,48)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecMenuGrp::UnionSeprateVarStrIntoVarGrp [%s] of [%s][%s]!\n",menuItemsList,mainMenuName,menuGrpName);
		return(itemGrpNum);
	}
		
	for (index = 0; index < itemGrpNum; index++)
	{
		if ((ret = UnionGenerateAllInterfaceItemsOfSpecMenuItem(mainMenuName,menuGrpName,itemGrp[index])) < 0)
		{
			UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecMenuItem::UnionGenerateAllInterfaceItemsOfSpecMenuGrp [%s][%s][%s]!\n",
					mainMenuName,menuGrpName,itemGrp[index]);
			strcpy(remark,"����ָ���˵������,UnionGenerateAllInterfaceItemsOfSpecMenuItem");
		}
	}
errExit:
	return(UnionAddInterfaceItemToItemGrp(remark,conInterfaceItemTagMenuGrp,"mainMenuName=%s|tableMenuType=%d|menuGrpName=%s|",mainMenuName,menuType,menuGrpName));
}

/*
����	
	����һ���˵����õ���ָ���Ľ�����Ϣ
�������
	mainMenuName	���˵�����
	itemGrpName	�˵�������
	itemName	�˵�������
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateAllInterfaceItemsOfSpecMenuItem(char *mainMenuName,char *menuGrpName,char *itemName)
{
	int			ret;
	TUnionMenuItemDef	menuItemRec;
	char			remark[256+1];
	
	memset(remark,0,sizeof(remark));
	
	// �жϽ����ǰ�Ƿ����ڴ���
	if ((ret = UnionSetInterfaceItemDealCurrently(conInterfaceItemTagMenuItem,"mainMenuName=%s|menuGrpName=%s|menuDisplayname=%s|",mainMenuName,menuGrpName,itemName)) > 0)
		return(0);

	//UnionRealNullLog("in UnionGenerateAllInterfaceItemsOfSpecMenuItem:: deal [%s][%s][%s]...\n",mainMenuName,menuGrpName,itemName);

	// �����˵����
	memset(&menuItemRec,0,sizeof(menuItemRec));
	if ((ret = UnionReadMenuItemDefRec(mainMenuName,menuGrpName,itemName,&menuItemRec)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecMainMenu:: itemName [%s][%s][%s]!\n",mainMenuName,menuGrpName,itemName);
		strcpy(remark,"��ȡ�˵����¼����,UnionReadMenuItemDefRec");
		goto errExit;
	}
	
	// ���ɲ���
	if ((ret = UnionGenerateAllInterfaceItemsOfSpecOperation(mainMenuName,menuItemRec.viewName,menuItemRec.menuName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecView::UnionGenerateAllInterfaceItemsOfSpecOperation [%s] of [%s][%s]\n",
					menuItemRec.menuName,mainMenuName,menuItemRec.viewName);
		strcpy(remark,"���ɲ�������,UnionGenerateAllInterfaceItemsOfSpecOperation");
	}

	// ������ͼ
	if ((ret = UnionGenerateAllInterfaceItemsOfSpecView(mainMenuName,menuItemRec.viewName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecMenuItem::UnionGenerateAllInterfaceItemsOfSpecView [%s] of [%s][%s][%s]!\n",
				menuItemRec.viewName,mainMenuName,menuGrpName,itemName);
		strcpy(remark,"������ͼ����,UnionGenerateAllInterfaceItemsOfSpecView");
		goto errExit;
	}
errExit:	
	return(UnionAddInterfaceItemToItemGrp(remark,conInterfaceItemTagMenuItem,"mainMenuName=%s|menuGrpName=%s|menuDisplayname=%s|",mainMenuName,menuGrpName,itemName));
}

/*
����	
	����һ���˵������õ���ָ���Ľ�����Ϣ
�������
	mainMenuName	���˵�����
	viewName	��ͼ����
	operationName	�˵���������
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateAllInterfaceItemsOfSpecOperation(char *mainMenuName,char *viewName,char *operationName)
{
	int				ret;
	int				resID;
	TUnionMenuItemOperationDef	operationRec;
	char				remark[256+1];
	char				*ptr;
	char				*ptr2;
	
	memset(remark,0,sizeof(remark));
		
	// �жϽ����ǰ�Ƿ����ڴ���
	if ((ret = UnionSetInterfaceItemDealCurrently(conInterfaceItemTagMenuOperation,"mainMenuName=%s|menuName=%s|",mainMenuName,operationName)) > 0)
		return(0);

	// ��ȡ��ͼ����ԴID
	if ((ret = UnionReadViewListRecIntTypeFld(viewName,mainMenuName,conViewListFldNameResID,&resID)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecOperation:: UnionReadViewListRecIntTypeFld [%s][%s]!\n",viewName,mainMenuName);
		strcpy(remark,"��ȡ��ͼ��¼��resID�����,UnionReadViewListRecIntTypeFld");
		goto errExit;
	}
		
	// �����˵���������
	memset(&operationRec,0,sizeof(operationRec));
	if ((ret = UnionReadMenuItemOperationDefRec(operationName,&operationRec)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecOperation:: UnionReadMenuItemOperationDefRec [%s]!\n",operationName);
		strcpy(remark,"��ȡ���������¼����,UnionReadMenuItemOperationDefRec");
		goto errExit;
	}
	
	// ���ɲ�����Ȩ
	if ((ret = UnionGenerateAllInterfaceItemsOfSpecOperationAuth(mainMenuName,resID,operationRec.serviceID)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecOperation::UnionGenerateAllInterfaceItemsOfSpecOperationAuth [%d][%d] of [%s]\n",
					resID,operationRec.serviceID,operationName);
		strcpy(remark,"���ɲ�����Ȩ����,UnionGenerateAllInterfaceItemsOfSpecOperationAuth");
	}

	// ���ɲ�����Ȩ
	if ((ret = UnionGenerateAllInterfaceItemsOfSpecOperationTagDef(mainMenuName,operationRec.serviceName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecOperation::UnionGenerateAllInterfaceItemsOfSpecOperationTagDef [%s]\n",operationRec.serviceName);
		strcpy(remark,"���ɲ�����ʶ,UnionGenerateAllInterfaceItemsOfSpecOperationTagDef");
	}

	// �ж��Ƿ���������
	if (strncmp(operationRec.conditionsMenu,"batchMenu=",10) == 0)
	{
		// ��������
		if ((ret = UnionGenerateAllInterfaceItemsOfSpecMenuGrp(mainMenuName,conMenuItemTypeBatchMenuGrp,operationRec.conditionsMenu+10)) < 0)
		{
			UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecOperation:: UnionGenerateAllInterfaceItemsOfSpecMenuGrp [%s] of [%s][%s]!\n",
					operationName,mainMenuName,operationRec.conditionsMenu+10);
			strcpy(remark,"��������������,UnionGenerateAllInterfaceItemsOfSpecMenuGrp");
		}
	}
	else if ((ptr = strstr(operationRec.conditionsMenu,"appendfld=append.")) != NULL)
	{
		// �Ǻ�����ͼ����
		if ((ptr2 = strstr(ptr,",")) != NULL)
			*ptr2 = 0;
		if ((ret = UnionGenerateAllInterfaceItemsOfSpecView(mainMenuName,ptr+strlen("appendfld=append."))) < 0)
		{
			UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecOperation:: UnionGenerateAllInterfaceItemsOfSpecView [%s] of [%s][%s]!\n",
					operationName,mainMenuName,ptr+strlen("appendfld=append."));
			strcpy(remark,"������ͼ����,UnionGenerateAllInterfaceItemsOfSpecView");
		}
	}
errExit:
	return(UnionAddInterfaceItemToItemGrp(remark,conInterfaceItemTagMenuOperation,"mainMenuName=%s|menuName=%s|",mainMenuName,operationName));
}

/*
����	
	����һ����ͼ�õ���ָ���Ľ�����Ϣ
�������
	mainMenuName	���˵�
	viewName	��ͼ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateAllInterfaceItemsOfSpecView(char *mainMenuName,char *viewName)
{
	int				ret;
	TUnionViewList			viewRec;
	char				remark[256+1];
	
	memset(remark,0,sizeof(remark));
	
	// �жϽ����ǰ�Ƿ����ڴ���
	if ((ret = UnionSetInterfaceItemDealCurrently(conInterfaceItemTagView,"mainMenuName=%s|ID=%s|",mainMenuName,viewName)) > 0)
		return(0);

	// ������ͼ����
	memset(&viewRec,0,sizeof(viewRec));
	if ((ret = UnionReadViewListRec(viewName,mainMenuName,&viewRec)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecMainMenu:: UnionReadViewListRec [%s][%s]!\n",viewName,mainMenuName);
		strcpy(remark,"��ȡ��ͼ��¼����,UnionReadViewListRec");
		goto errExit;
	}

	// ���ɽ����Ҽ��˵�
	if ((ret = UnionGenerateAllInterfaceItemsOfSpecMenuGrp(mainMenuName,conMenuItemTypeFormPopMenu,viewRec.formPopMenuDefFileName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecView:: UnionGenerateAllInterfaceItemsOfSpecMenuGrp [%s][%s]!\n",
				mainMenuName,viewRec.formPopMenuDefFileName);
		strcpy(remark,"���ɽ����Ҽ��˵�����,UnionGenerateAllInterfaceItemsOfSpecMenuGrp");
	}

	// ���ɼ�¼�Ҽ��˵�
	if ((ret = UnionGenerateAllInterfaceItemsOfSpecMenuGrp(mainMenuName,conMenuItemTypeRecPopMenu,viewRec.recPopMenuDefFileName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecView:: UnionGenerateAllInterfaceItemsOfSpecMenuGrp [%s][%s]!\n",
				mainMenuName,viewRec.recPopMenuDefFileName);
		strcpy(remark,"���ɼ�¼�Ҽ��˵�����,UnionGenerateAllInterfaceItemsOfSpecMenuGrp");
	}

	// ����˫������˵�
	if ((ret = UnionGenerateAllInterfaceItemsOfSpecMenuGrp(mainMenuName,conMenuItemTypeDoubleClickMenu,viewRec.recDoubleClickMenuFileName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecView:: UnionGenerateAllInterfaceItemsOfSpecMenuGrp [%s][%s]!\n",
				mainMenuName,viewRec.recDoubleClickMenuFileName);
		strcpy(remark,"����˫����¼�˵�����,UnionGenerateAllInterfaceItemsOfSpecMenuGrp");
	}

	// �������嵥
	if ((ret = UnionGenerateAllInterfaceItemsOfSpecTableFieldList(mainMenuName,viewName,viewRec.fieldList)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecView:: UnionGenerateAllInterfaceItemsOfSpecTableFieldList [%s] of [%s]\n",
					mainMenuName,viewName);
		strcpy(remark,"������ͼ���嵥����,UnionGenerateAllInterfaceItemsOfSpecTableFieldList");
	}
	// ���ɱ�
	if ((ret = UnionGenerateAllInterfaceItemsOfSpecTable(mainMenuName,viewRec.tableName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecView:: UnionGenerateAllInterfaceItemsOfSpecTable [%s] of [%s]\n",
					viewRec.tableName,mainMenuName);
		strcpy(remark,"���ɱ������,UnionGenerateAllInterfaceItemsOfSpecTable");
	}
errExit:	
	return(UnionAddInterfaceItemToItemGrp(remark,conInterfaceItemTagView,"mainMenuName=%s|ID=%s|",mainMenuName,viewName));
}

/*
����	
	�ж�һ�����Ƿ�һ�����˵�ʹ����
�������
	mainMenuName	���˵�
	tableName	������
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionIsTableUsedByMainMenu(char *mainMenuName,char *tableName)
{
	return(UnionIsInterfaceItemExists(conInterfaceItemTagTable,"mainMenuName=%s|tableName=%s|",mainMenuName,tableName));
}

/*
����	
	����һ�����õ���ָ���Ľ�����Ϣ
�������
	mainMenuName	���˵�
	tableName	������
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateAllInterfaceItemsOfSpecTable(char *mainMenuName,char *tableName)
{
	int				ret;
	char				fieldList[8192+1];
	char				thisTableName[80+1];
	char				remark[256+1];
	
	memset(remark,0,sizeof(remark));
	
	memset(thisTableName,0,sizeof(thisTableName));
	if (UnionIsDigitStr(tableName))
	{
		if ((ret =  UnionReadTableListRecFldByResID(atoi(tableName),conTableListFldNameTableName,thisTableName,
			sizeof(thisTableName))) < 0)
		{
			UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecTableFieldList:: UnionReadTableListRecFldByResID [%s]!\n",tableName);
			strcpy(thisTableName,tableName);
			strcpy(remark,"��ȡ������Ƴ���,UnionReadTableListRecFld");
			goto errExit;
		}
	}
	else
		strcpy(thisTableName,tableName);

	// �жϽ����ǰ�Ƿ����ڴ���
	if ((ret = UnionSetInterfaceItemDealCurrently(conInterfaceItemTagTable,"mainMenuName=%s|tableName=%s|",mainMenuName,thisTableName)) > 0)
		return(0);

	// ��������
	memset(fieldList,0,sizeof(fieldList));
	if (UnionIsDigitStr(tableName))
		ret =  UnionReadTableListRecFldByResID(atoi(tableName),conTableListFldNameFieldList,fieldList,sizeof(fieldList));
	else
		ret = UnionReadTableListRecFld(tableName,conTableListFldNameFieldList,fieldList,sizeof(fieldList));
	if (ret < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecMainMenu:: UnionReadTableListRecFld [%s][%s]!\n",thisTableName,mainMenuName);
		strcpy(remark,"��ȡ������嵥����,UnionReadTableListRecFld");
		goto errExit;
	}

	// �������嵥
	if ((ret = UnionGenerateAllInterfaceItemsOfSpecTableFieldList(mainMenuName,thisTableName,fieldList)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecTable:: UnionGenerateAllInterfaceItemsOfSpecTableFieldList [%s] of [%s]\n",
					mainMenuName,thisTableName);
		strcpy(remark,"���ɱ�����嵥����,UnionGenerateAllInterfaceItemsOfSpecTableFieldList");
	}
	
errExit:	
	return(UnionAddInterfaceItemToItemGrp(remark,conInterfaceItemTagTable,"mainMenuName=%s|tableName=%s|",mainMenuName,thisTableName));
}

/*
����	
	����һ�����嵥�õ���ָ���Ľ�����Ϣ
�������
	mainMenuName	���˵�
	tableName	������
	fieldList	���嵥
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateAllInterfaceItemsOfSpecTableFieldList(char *mainMenuName,char *tableName,char *fieldList)
{
	int			ret;
	int			index;
	char			fldGrp[128][128];
	int			fldNum;
	int			failureItems = 0;
	// ƴ�����嵥
	if ((fldNum = UnionSeprateVarStrIntoVarGrp(fieldList,strlen(fieldList),',',fldGrp,128)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecMainMenu::UnionSeprateVarStrIntoVarGrp [%s] of [%s][%s]!\n",
			fieldList,mainMenuName,tableName);
		return(fldNum);
	}
		
	for (index = 0; index < fldNum; index++)
	{
		if ((ret = UnionGenerateAllInterfaceItemsOfSpecTableField(mainMenuName,fldGrp[index])) < 0)
		{
			UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecMainMenu::UnionGenerateAllInterfaceItemsOfSpecTableField [%s] of [%s]!\n",
					fldGrp[index],mainMenuName);
			failureItems++;
		}
	}
	if (failureItems > 0)
		return(errCodeObjectMDL_FieldListDefError);
	else
		return(0);
}

/*
����	
	����һ���������õ���ָ���Ľ�����Ϣ
�������
	mainMenuName	���˵�
	fieldName	������
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateAllInterfaceItemsOfSpecTableField(char *mainMenuName,char *fieldName)
{
	int			ret;
	TUnionTableField	fldRec;
	char			tmpBuf[128+1];
	int			methodType;
	char			remark[256+1];
	
	memset(remark,0,sizeof(remark));
	
	// �жϽ����ǰ�Ƿ����ڴ���
	if ((ret = UnionSetInterfaceItemDealCurrently(conInterfaceItemTagTableField,"mainMenuName=%s|ID=%s|",mainMenuName,fieldName)) > 0)
		return(0);

	// ��������
	memset(&fldRec,0,sizeof(fldRec));
	strcpy(fldRec.ID,fieldName);
	if ((ret = UnionReadTableFieldRec(&fldRec)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecTableField:: UnionReadTableFieldRec [%s][%s]!\n",fieldName,mainMenuName);
		strcpy(remark,"��ȡ�����¼����,UnionReadTableFieldRec");
		goto errExit;
	}
		
	// ����ֵ�����ж�ȡ������
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((methodType = UnionReadAttrFromFieldMethodDef(fldRec.value,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecTableField:: UnionReadAttrFromFieldMethodDef [%s] of [%s][%s]!\n",
				fldRec.value,fieldName,mainMenuName);
		strcpy(remark,"������ֵ��������,UnionReadAttrFromFieldMethodDef");
		goto errExit;
	}
	switch (methodType)
	{
		case	conTblFldValueMethodUseTable:
			// ��
			if ((ret = UnionGenerateAllInterfaceItemsOfSpecTable(mainMenuName,tmpBuf)) < 0)
			{
				UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecTableField::UnionGenerateAllInterfaceItemsOfSpecTable [%s] of [%s][%s]!\n",
					tmpBuf,mainMenuName,fieldName);
				strcpy(remark,"���ɱ������,UnionGenerateAllInterfaceItemsOfSpecTable");
			}
			break;
		case	conTblFldValueMethodUseEnum:
			// ö��
			if ((ret = UnionGenerateAllInterfaceItemsOfSpecEnum(mainMenuName,tmpBuf)) < 0)
			{
				UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecTableField::UnionGenerateAllInterfaceItemsOfSpecEnum [%s] of [%s][%s]!\n",
					tmpBuf,mainMenuName,fieldName);
				strcpy(remark,"����ö�ٳ���,UnionGenerateAllInterfaceItemsOfSpecEnum");
			}
			break;
		case	conTblFldValueMethodUseComplexFld:
			// ������
			if ((ret = UnionGenerateAllInterfaceItemsOfSpecComplexFld(mainMenuName,tmpBuf)) < 0)
			{
				UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecTableField::UnionGenerateAllInterfaceItemsOfSpecComplexFld [%s] of [%s][%s]!\n",
					tmpBuf,mainMenuName,fieldName);
				strcpy(remark,"���ɸ��������,UnionGenerateAllInterfaceItemsOfSpecComplexFld");
			}
			break;
		default:
			break;
	}
errExit:
	return(UnionAddInterfaceItemToItemGrp(remark,conInterfaceItemTagTableField,"mainMenuName=%s|ID=%s|",mainMenuName,fieldName));
}

/*
����	
	����һ��ö���õ���ָ���Ľ�����Ϣ
�������
	mainMenuName	���˵�
	enumName	ö������
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateAllInterfaceItemsOfSpecEnum(char *mainMenuName,char *enumName)
{
	int	ret;
	
	// �жϽ����ǰ�Ƿ����ڴ���
	if ((ret = UnionSetInterfaceItemDealCurrently(conInterfaceItemTagEnum,"mainMenuName=%s|enumFileName=%s|",mainMenuName,enumName)) > 0)
		return(0);
	return(UnionAddInterfaceItemToItemGrp(NULL,conInterfaceItemTagEnum,"mainMenuName=%s|enumFileName=%s|",mainMenuName,enumName));
}

/*
����	
	����һ���������õ���ָ���Ľ�����Ϣ
�������
	mainMenuName	���˵�
	complexFldName	����������
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateAllInterfaceItemsOfSpecComplexFld(char *mainMenuName,char *complexFldName)
{
	int	ret;
	char	fldList[1024+1];
	int	lenOfFldList;
	char	remark[256+1];
	
	memset(remark,0,sizeof(remark));
	
	// �жϽ����ǰ�Ƿ����ڴ���
	if ((ret = UnionSetInterfaceItemDealCurrently(conInterfaceItemTagComplexField,"mainMenuName=%s|complexFieldID=%s|",mainMenuName,complexFldName)) > 0)
		return(0);
	// �ǼǸ��������
	if ((ret = UnionGenerateAllInterfaceItemsOfSpecTable(mainMenuName,"complexField")) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecComplexFld:: UnionGenerateAllInterfaceItemsOfSpecTable [%s]!\n",mainMenuName);
		strcpy(remark,"���ɸ�����������UnionGenerateAllInterfaceItemsOfSpecTable");
	}
	// ����������������嵥
	memset(fldList,0,sizeof(fldList));
	if ((lenOfFldList = UnionReadComplexFieldRecFld(mainMenuName,complexFldName,conComplexFieldFldNameTableFldList,fldList,sizeof(fldList))) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecComplexFld:: UnionReadComplexFieldRecFld [%s]!\n",mainMenuName);
		strcpy(remark,"��ȡ�������¼����,UnionReadComplexFieldRecFld");
		goto errExit;
	}
	// �������嵥
	if ((ret = UnionGenerateAllInterfaceItemsOfSpecTableFieldList(mainMenuName,complexFldName,fldList)) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllInterfaceItemsOfSpecView:: UnionGenerateAllInterfaceItemsOfSpecComplexFld [%s] of [%s]\n",
					mainMenuName,complexFldName);
		strcpy(remark,"���ɸ������嵥����,UnionGenerateAllInterfaceItemsOfSpecTableFieldList");
	}
errExit:
	return(UnionAddInterfaceItemToItemGrp(remark,conInterfaceItemTagComplexField,"mainMenuName=%s|complexFieldID=%s|",mainMenuName,complexFldName));
}

/*
����	
	����һ��������Ȩ�õ���ָ���Ľ�����Ϣ
�������
	mainMenuName	���˵�
	resID		��ԴID
	operationID	����ID
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateAllInterfaceItemsOfSpecOperationAuth(char *mainMenuName,int resID,int operationID)
{
	int	ret;
	
	// �жϽ����ǰ�Ƿ����ڴ���
	if ((ret = UnionSetInterfaceItemDealCurrently(conInterfaceItemTagOperationAuth,"mainMenuName=%s|resID=%d|serviceID=%d|",mainMenuName,resID,operationID)) > 0)
		return(0);
	return(UnionAddInterfaceItemToItemGrp(NULL,conInterfaceItemTagOperationAuth,"mainMenuName=%s|resID=%d|serviceID=%d|",mainMenuName,resID,operationID));
}

/*
����	
	����һ��������ʶ�Ľ�����Ϣ
�������
	mainMenuName	���˵�
	serviceName	������ʶ
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateAllInterfaceItemsOfSpecOperationTagDef(char *mainMenuName,char *serviceName)
{
	int	ret;
	
	// �жϽ����ǰ�Ƿ����ڴ���
	if ((ret = UnionSetInterfaceItemDealCurrently(conInterfaceItemTagOperationTagDef,"mainMenuName=%s|ID=%s|",mainMenuName,serviceName)) > 0)
		return(0);
	return(UnionAddInterfaceItemToItemGrp(NULL,conInterfaceItemTagOperationTagDef,"mainMenuName=%s|ID=%s|",mainMenuName,serviceName));
}

/*
����	
	ɾ������û���õ��Ľ�����,����ɾ������д�뵽ָ���ļ���
�������
	fileName	�ļ�����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionDeleteAllUnusedInterfaceItemsWithOutputToSpecFile(char *fileName)
{
	FILE	*fp;
	
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionUserErrLog("in UnionDeleteAllUnusedInterfaceItemsWithOutputToSpecFile:: fopen [%s]!\n",fileName);
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}
	UnionDeleteAllUnusedInterfaceItems(fp);
	fclose(fp);
	return(0);
}

/*
����	
	ɾ������û���õ��Ľ�����,����ɾ������д�뵽ָ���ļ���
�������
	fp			�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionDeleteAllUnusedInterfaceItems(FILE *fp)
{
	int		ret;
	TUnionMainMenu	mainMenuGrp[24];
	int		mainMenuNum;
	int		index;

	// ѡ������������
	memset(&mainMenuGrp,0,sizeof(mainMenuGrp));
	if ((mainMenuNum = UnionBatchReadMainMenuRec("",mainMenuGrp,24)) < 0)
	{
		UnionUserErrLog("in UnionDeleteAllUnusedInterfaceItems:: mainMenuGrp!\n");
		return(mainMenuNum);
	}
	
	// ��ʼ������������
	if ((ret = UnionInitInterfaceItemGrp("all-mainMenuGrp")) < 0)
	{
		UnionUserErrLog("in UnionDeleteAllUnusedInterfaceItems:: UnionInitInterfaceItemGrp!\n");
		return(ret);
	}
	// ���λ��ÿ�����˵�ʹ�õĽ�����
	for (index = 0; index < mainMenuNum; index++)
	{
		if ((ret = UnionGenerateAllInterfaceItemsOfSpecMainMenu(mainMenuGrp[index].mainMenuName)) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedInterfaceItems::UnionGenerateAllInterfaceItemsOfSpecMainMenu [%s]!\n",mainMenuGrp[index].mainMenuName);
			UnionDeleteInterfaceItemGrp();
			return(ret);
		}
	}
	// ɾ������û���õ��Ľ�����
	for (index = 0; index < mainMenuNum; index++)
	{
		UnionDeleteAllUnusedComplexFldItems(mainMenuGrp[index].mainMenuName,fp);
		UnionDeleteAllUnusedEnumDefItems(mainMenuGrp[index].mainMenuName,fp);
		UnionDeleteAllUnusedViewListItems(mainMenuGrp[index].mainMenuName,fp);
		UnionDeleteAllUnusedMenuItemItems(mainMenuGrp[index].mainMenuName,fp);
		UnionDeleteAllUnusedMenuGrpItems(mainMenuGrp[index].mainMenuName,fp);
		UnionDeleteAllUnusedMainMenuItems(mainMenuGrp[index].mainMenuName,fp);
	}
	UnionDeleteAllUnusedTableFieldItems("",fp);
	UnionDeleteAllUnusedOperationAuthorizationItems("",fp);
	UnionDeleteAllUnusedMenuOperationItems("",fp);
	UnionDeleteAllUnusedOperationListItems("",fp);
	UnionDeleteAllUnusedTableListItems("",fp);
	
	// ɾ������������
	UnionDeleteInterfaceItemGrp();
	return(ret);
}

/*
����	
	ɾ������û���õ��ĸ�����,����ɾ������д�뵽ָ���ļ���
�������
	mainMenuName		���˵�����
	fp			�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionDeleteAllUnusedComplexFldItems(char *mainMenuName,FILE *fp)
{
	int	ret;
	FILE	*outFp = stdout;
	char	fileName[256+1];
	FILE	*inFp;
	char	recStr[8192];
	int	lenOfRecStr;
	int	deleteNum = 0;
	char	complexFieldID[128+1];
		
	char	condition[256+1];
	
	sprintf(condition,"mainMenuName=%s|",mainMenuName);
		
	if (!UnionIsInterfaceItemsGrpInited())
		return(0);
	if (fp != NULL)
		outFp = fp;
	// ѡ�����м�¼
	UnionFormSelfTempFileName(fileName);
	if ((ret = UnionBatchSelectObjectRecord(conTBLNameComplexField,condition,fileName)) < 0)
	{
		UnionUserErrLog("in UnionDeleteAllUnusedComplexFldItems:: UnionBatchSelectObjectRecord!\n");
		return(ret);
	}
	if ((inFp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionDeleteAllUnusedComplexFldItems:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	while (!feof(inFp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(inFp,recStr,sizeof(recStr))) <= 0)
			continue;
		memset(complexFieldID,0,sizeof(complexFieldID));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conComplexFieldFldNameComplexFieldID,complexFieldID,sizeof(complexFieldID))) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedComplexFldItems:: UnionReadRecFldFromRecStr [%s] form [%04d][%s]\n",
					conComplexFieldFldNameComplexFieldID,lenOfRecStr,recStr);
			continue;
		}
		// �ж����Ƿ����
		if (UnionIsInterfaceItemExists(conTBLNameComplexField,"mainMenuName=%s|complexFieldID=%s|",mainMenuName,complexFieldID))
			continue;
		// ������
		// ɾ��
		/*
		if ((ret = UnionDeleteUniqueObjectRecord(conTBLNameComplexField,recStr)) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedComplexFldItems:: UnionDeleteUniqueObjectRecord [%s]!\n",recStr);
			continue;
		}
		*/
		fprintf(outFp,"delete [%s] - [%s][%s] ok!\n",conTBLNameComplexField,mainMenuName,complexFieldID);
		deleteNum++;
	}
	fclose(inFp);
	UnionDeleteSelfTempFile();
	return(deleteNum);
}	

/*
����	
	ɾ������û���õ���ö�ٶ���,����ɾ������д�뵽ָ���ļ���
�������
	mainMenuName		���˵�����
	fp			�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionDeleteAllUnusedEnumDefItems(char *mainMenuName,FILE *fp)
{
	int	ret;
	FILE	*outFp = stdout;
	char	fileName[256+1];
	FILE	*inFp;
	char	recStr[8192];
	int	lenOfRecStr;
	int	deleteNum = 0;
	char	enumFileName[128+1];
		
	char	condition[256+1];
	
	sprintf(condition,"mainMenuName=%s|",mainMenuName);
		
	if (!UnionIsInterfaceItemsGrpInited())
		return(0);
	if (fp != NULL)
		outFp = fp;
	// ѡ�����м�¼
	UnionFormSelfTempFileName(fileName);
	if ((ret = UnionBatchSelectObjectRecord(conTBLNameEnumValueDef,condition,fileName)) < 0)
	{
		UnionUserErrLog("in UnionDeleteAllUnusedEnumDefItems:: UnionBatchSelectObjectRecord!\n");
		return(ret);
	}
	if ((inFp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionDeleteAllUnusedEnumDefItems:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	while (!feof(inFp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(inFp,recStr,sizeof(recStr))) <= 0)
			continue;
		memset(enumFileName,0,sizeof(enumFileName));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conEnumValueDefFldNameEnumFileName,enumFileName,sizeof(enumFileName))) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedEnumDefItems:: UnionReadRecFldFromRecStr [%s] form [%04d][%s]\n",
					conEnumValueDefFldNameEnumFileName,lenOfRecStr,recStr);
			continue;
		}
		// �ж����Ƿ����
		if (UnionIsInterfaceItemExists(conTBLNameEnumValueDef,"mainMenuName=%s|enumFileName=%s|",mainMenuName,enumFileName))
			continue;
		// ������
		// ɾ��
		/*
		if ((ret = UnionDeleteUniqueObjectRecord(conTBLNameEnumValueDef,recStr)) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedEnumDefItems:: UnionDeleteUniqueObjectRecord [%s]!\n",recStr);
			continue;
		}
		*/
		fprintf(outFp,"delete [%s] - [%s][%s] ok!\n",conTBLNameEnumValueDef,mainMenuName,enumFileName);
		deleteNum++;
	}
	fclose(inFp);
	UnionDeleteSelfTempFile();
	return(deleteNum);
}

/*
����	
	ɾ������û���õ�����ͼ����,����ɾ������д�뵽ָ���ļ���
�������
	mainMenuName		���˵�����
	fp			�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionDeleteAllUnusedViewListItems(char *mainMenuName,FILE *fp)
{
	int	ret;
	FILE	*outFp = stdout;
	char	fileName[256+1];
	FILE	*inFp;
	char	recStr[8192];
	int	lenOfRecStr;
	int	deleteNum = 0;
	char	ID[128+1];
		
	char	condition[256+1];
	
	sprintf(condition,"mainMenuName=%s|",mainMenuName);
		
	if (!UnionIsInterfaceItemsGrpInited())
		return(0);
	if (fp != NULL)
		outFp = fp;
	// ѡ�����м�¼
	UnionFormSelfTempFileName(fileName);
	if ((ret = UnionBatchSelectObjectRecord(conTBLNameViewList,condition,fileName)) < 0)
	{
		UnionUserErrLog("in UnionDeleteAllUnusedViewListItems:: UnionBatchSelectObjectRecord!\n");
		return(ret);
	}
	if ((inFp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionDeleteAllUnusedViewListItems:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	while (!feof(inFp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(inFp,recStr,sizeof(recStr))) <= 0)
			continue;
		memset(ID,0,sizeof(ID));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conViewListFldNameID,ID,sizeof(ID))) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedViewListItems:: UnionReadRecFldFromRecStr [%s] form [%04d][%s]\n",
					conViewListFldNameID,lenOfRecStr,recStr);
			continue;
		}
		// �ж����Ƿ����
		if (UnionIsInterfaceItemExists(conTBLNameViewList,"mainMenuName=%s|ID=%s|",mainMenuName,ID))
			continue;
		// ������
		// ɾ��
		/*
		if ((ret = UnionDeleteUniqueObjectRecord(conTBLNameViewList,recStr)) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedViewListItems:: UnionDeleteUniqueObjectRecord [%s]!\n",recStr);
			continue;
		}
		*/
		fprintf(outFp,"delete [%s] - [%s][%s] ok!\n",conTBLNameViewList,mainMenuName,ID);
		deleteNum++;
	}
	fclose(inFp);
	UnionDeleteSelfTempFile();
	return(deleteNum);
}

/*
����	
	ɾ������û���õ��ı�����,����ɾ������д�뵽ָ���ļ���
�������
	mainMenuName		���˵�����
	fp			�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionDeleteAllUnusedTableFieldItems(char *mainMenuName,FILE *fp)
{
	int	ret;
	FILE	*outFp = stdout;
	char	fileName[256+1];
	FILE	*inFp;
	char	recStr[8192];
	int	lenOfRecStr;
	int	deleteNum = 0;
	char	ID[128+1];
		
	char	condition[256+1];
	
	strcpy(condition,"");
		
	if (!UnionIsInterfaceItemsGrpInited())
		return(0);
	if (fp != NULL)
		outFp = fp;
	// ѡ�����м�¼
	UnionFormSelfTempFileName(fileName);
	if ((ret = UnionBatchSelectObjectRecord(conTBLNameTableField,condition,fileName)) < 0)
	{
		UnionUserErrLog("in UnionDeleteAllUnusedTableFieldItems:: UnionBatchSelectObjectRecord!\n");
		return(ret);
	}
	if ((inFp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionDeleteAllUnusedTableFieldItems:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	while (!feof(inFp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(inFp,recStr,sizeof(recStr))) <= 0)
			continue;
		memset(ID,0,sizeof(ID));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conViewListFldNameID,ID,sizeof(ID))) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedTableFieldItems:: UnionReadRecFldFromRecStr [%s] form [%04d][%s]\n",
					conViewListFldNameID,lenOfRecStr,recStr);
			continue;
		}
		// �ж����Ƿ����
		if (UnionIsInterfaceItemContainSpecStrExists(conTBLNameTableField,"|ID=%s|",ID))
			continue;
		// ������
		// ɾ��
		/*
		if ((ret = UnionDeleteUniqueObjectRecord(conTBLNameTableField,recStr)) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedTableFieldItems:: UnionDeleteUniqueObjectRecord [%s]!\n",recStr);
			continue;
		}
		*/
		fprintf(outFp,"delete [%s] - [%s][%s] ok!\n",conTBLNameTableField,mainMenuName,ID);
		deleteNum++;
	}
	fclose(inFp);
	UnionDeleteSelfTempFile();
	return(deleteNum);
}

/*
����	
	ɾ������û���õ�����Ȩ����,����ɾ������д�뵽ָ���ļ���
�������
	mainMenuName		���˵�����
	fp			�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionDeleteAllUnusedOperationAuthorizationItems(char *mainMenuName,FILE *fp)
{
	int	ret;
	FILE	*outFp = stdout;
	char	fileName[256+1];
	FILE	*inFp;
	char	recStr[8192];
	int	lenOfRecStr;
	int	deleteNum = 0;
	char	resID[128+1];
	char	serviceID[128+1];
		
	char	condition[256+1];
	
	strcpy(condition,"");
		
	if (!UnionIsInterfaceItemsGrpInited())
		return(0);
	if (fp != NULL)
		outFp = fp;
	// ѡ�����м�¼
	UnionFormSelfTempFileName(fileName);
	if ((ret = UnionBatchSelectObjectRecord(conTBLNameOperationAuthorization,condition,fileName)) < 0)
	{
		UnionUserErrLog("in UnionDeleteAllUnusedOperationAuthorizationItems:: UnionBatchSelectObjectRecord!\n");
		return(ret);
	}
	if ((inFp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionDeleteAllUnusedOperationAuthorizationItems:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	while (!feof(inFp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(inFp,recStr,sizeof(recStr))) <= 0)
			continue;
		memset(resID,0,sizeof(resID));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conOperationAuthorizationFldNameResID,resID,sizeof(resID))) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedOperationAuthorizationItems:: UnionReadRecFldFromRecStr [%s] form [%04d][%s]\n",
					conOperationAuthorizationFldNameResID,lenOfRecStr,recStr);
			continue;
		}
		memset(serviceID,0,sizeof(serviceID));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conOperationAuthorizationFldNameServiceID,serviceID,sizeof(serviceID))) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedOperationAuthorizationItems:: UnionReadRecFldFromRecStr [%s] form [%04d][%s]\n",
					conOperationAuthorizationFldNameServiceID,lenOfRecStr,recStr);
			continue;
		}
		// �ж����Ƿ����
		if (UnionIsInterfaceItemContainSpecStrExists(conTBLNameOperationAuthorization,"|resID=%s|serviceID=%s|",resID,serviceID))
			continue;
		// ������
		// ɾ��
		/*
		if ((ret = UnionDeleteUniqueObjectRecord(conTBLNameOperationAuthorization,recStr)) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedOperationAuthorizationItems:: UnionDeleteUniqueObjectRecord [%s]!\n",recStr);
			continue;
		}
		*/
		fprintf(outFp,"delete [%s] - [%s][%s][%s] ok!\n",conTBLNameOperationAuthorization,mainMenuName,resID,serviceID);
		deleteNum++;
	}
	fclose(inFp);
	UnionDeleteSelfTempFile();
	return(deleteNum);
}

/*
����	
	ɾ������û���õ��Ĳ˵����,����ɾ������д�뵽ָ���ļ���
�������
	mainMenuName		���˵�����
	fp			�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionDeleteAllUnusedMenuItemItems(char *mainMenuName,FILE *fp)
{
	int	ret;
	FILE	*outFp = stdout;
	char	fileName[256+1];
	FILE	*inFp;
	char	recStr[8192];
	int	lenOfRecStr;
	int	deleteNum = 0;
	char	menuGrpName[128+1];
	char	menuDisplayname[128+1];
		
	char	condition[256+1];
	
	sprintf(condition,"mainMenuName=%s|",mainMenuName);
		
	if (!UnionIsInterfaceItemsGrpInited())
		return(0);
	if (fp != NULL)
		outFp = fp;
	// ѡ�����м�¼
	UnionFormSelfTempFileName(fileName);
	if ((ret = UnionBatchSelectObjectRecord(conTBLNameMenuItemDef,condition,fileName)) < 0)
	{
		UnionUserErrLog("in UnionDeleteAllUnusedMenuItemItems:: UnionBatchSelectObjectRecord!\n");
		return(ret);
	}
	if ((inFp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionDeleteAllUnusedMenuItemItems:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	while (!feof(inFp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(inFp,recStr,sizeof(recStr))) <= 0)
			continue;
		memset(menuGrpName,0,sizeof(menuGrpName));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conMenuItemDefFldNameMenuGrpName,menuGrpName,sizeof(menuGrpName))) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedMenuItemItems:: UnionReadRecFldFromRecStr [%s] form [%04d][%s]\n",
					conMenuItemDefFldNameMenuGrpName,lenOfRecStr,recStr);
			continue;
		}
		memset(menuDisplayname,0,sizeof(menuDisplayname));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conMenuItemDefFldNameMenuDisplayname,menuDisplayname,sizeof(menuDisplayname))) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedMenuItemItems:: UnionReadRecFldFromRecStr [%s] form [%04d][%s]\n",
					conMenuItemDefFldNameMenuDisplayname,lenOfRecStr,recStr);
			continue;
		}
		// �ж����Ƿ����
		if (UnionIsInterfaceItemExists(conTBLNameMenuItemDef,"mainMenuName=%s|%s=%s|%s=%s|",
				mainMenuName,conMenuItemDefFldNameMenuGrpName,menuGrpName,conMenuItemDefFldNameMenuDisplayname,menuDisplayname))
			continue;
		// ������
		// ɾ��
		/*
		if ((ret = UnionDeleteUniqueObjectRecord(conTBLNameMenuItemDef,recStr)) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedMenuItemItems:: UnionDeleteUniqueObjectRecord [%s]!\n",recStr);
			continue;
		}
		*/
		fprintf(outFp,"delete [%s] - [%s][%s][%s] ok!\n",conTBLNameMenuItemDef,mainMenuName,menuGrpName,menuDisplayname);
		deleteNum++;
	}
	fclose(inFp);
	UnionDeleteSelfTempFile();
	return(deleteNum);
}

/*
����	
	ɾ������û���õ��Ĳ˵��鶨��,����ɾ������д�뵽ָ���ļ���
�������
	mainMenuName		���˵�����
	fp			�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionDeleteAllUnusedMenuGrpItems(char *mainMenuName,FILE *fp)
{
	int	ret;
	FILE	*outFp = stdout;
	char	fileName[256+1];
	FILE	*inFp;
	char	recStr[8192];
	int	lenOfRecStr;
	int	deleteNum = 0;
	char	menuGrpName[128+1];
		
	char	condition[256+1];
	
	sprintf(condition,"mainMenuName=%s|",mainMenuName);
		
	if (!UnionIsInterfaceItemsGrpInited())
		return(0);
	if (fp != NULL)
		outFp = fp;
	// ѡ�����м�¼
	UnionFormSelfTempFileName(fileName);
	if ((ret = UnionBatchSelectObjectRecord(conTBLNameMenuDef,condition,fileName)) < 0)
	{
		UnionUserErrLog("in UnionDeleteAllUnusedMenuGrpItems:: UnionBatchSelectObjectRecord!\n");
		return(ret);
	}
	if ((inFp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionDeleteAllUnusedMenuGrpItems:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	while (!feof(inFp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(inFp,recStr,sizeof(recStr))) <= 0)
			continue;
		memset(menuGrpName,0,sizeof(menuGrpName));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conMenuDefFldNameMenuGrpName,menuGrpName,sizeof(menuGrpName))) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedMenuGrpItems:: UnionReadRecFldFromRecStr [%s] form [%04d][%s]\n",
					conMenuDefFldNameMenuGrpName,lenOfRecStr,recStr);
			continue;
		}
		// �ж����Ƿ����
		if (UnionIsInterfaceItemExists(conTBLNameMenuDef,"mainMenuName=%s|%s=%s|",
				mainMenuName,conMenuDefFldNameMenuGrpName,menuGrpName))
			continue;
		// ������
		// ɾ��
		/*
		if ((ret = UnionDeleteUniqueObjectRecord(conTBLNameMenuDef,recStr)) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedMenuGrpItems:: UnionDeleteUniqueObjectRecord [%s]!\n",recStr);
			continue;
		}
		*/
		fprintf(outFp,"delete [%s] - [%s][%s] ok!\n",conTBLNameMenuDef,mainMenuName,menuGrpName);
		deleteNum++;
	}
	fclose(inFp);
	UnionDeleteSelfTempFile();
	return(deleteNum);
}

/*
����	
	ɾ������û���õ������˵�����,����ɾ������д�뵽ָ���ļ���
�������
	mainMenuName		���˵�����
	fp			�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionDeleteAllUnusedMainMenuItems(char *mainMenuName,FILE *fp)
{
	int	ret;
	FILE	*outFp = stdout;
	char	fileName[256+1];
	FILE	*inFp;
	char	recStr[8192];
	int	lenOfRecStr;
	int	deleteNum = 0;
		
	char	condition[256+1];
	
	sprintf(condition,"mainMenuName=%s|",mainMenuName);
		
	if (!UnionIsInterfaceItemsGrpInited())
		return(0);
	if (fp != NULL)
		outFp = fp;
	// ѡ�����м�¼
	UnionFormSelfTempFileName(fileName);
	if ((ret = UnionBatchSelectObjectRecord(conTBLNameMainMenu,condition,fileName)) < 0)
	{
		UnionUserErrLog("in UnionDeleteAllUnusedMainMenuItems:: UnionBatchSelectObjectRecord!\n");
		return(ret);
	}
	if ((inFp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionDeleteAllUnusedMainMenuItems:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	while (!feof(inFp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(inFp,recStr,sizeof(recStr))) <= 0)
			continue;
		// �ж����Ƿ����
		if (UnionIsInterfaceItemExists(conTBLNameMainMenu,"mainMenuName=%s|",mainMenuName))
			continue;
		// ������
		// ɾ��
		/*
		if ((ret = UnionDeleteUniqueObjectRecord(conTBLNameMainMenu,recStr)) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedMainMenuItems:: UnionDeleteUniqueObjectRecord [%s]!\n",recStr);
			continue;
		}
		*/
		fprintf(outFp,"delete [%s] - [%s] ok!\n",conTBLNameMainMenu,mainMenuName);
		deleteNum++;
	}
	fclose(inFp);
	UnionDeleteSelfTempFile();
	return(deleteNum);
}

/*
����	
	ɾ������û���õ��Ĳ˵���������,����ɾ������д�뵽ָ���ļ���
�������
	mainMenuName		���˵�����
	fp			�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionDeleteAllUnusedMenuOperationItems(char *mainMenuName,FILE *fp)
{
	int	ret;
	FILE	*outFp = stdout;
	char	fileName[256+1];
	FILE	*inFp;
	char	recStr[8192];
	int	lenOfRecStr;
	int	deleteNum = 0;
	char	ID[128+1];
		
	char	condition[256+1];
	
	strcpy(condition,"");
		
	if (!UnionIsInterfaceItemsGrpInited())
		return(0);
	if (fp != NULL)
		outFp = fp;
	// ѡ�����м�¼
	UnionFormSelfTempFileName(fileName);
	if ((ret = UnionBatchSelectObjectRecord(conTBLNameMenuItemOperationDef,condition,fileName)) < 0)
	{
		UnionUserErrLog("in UnionDeleteAllUnusedMenuOperationItems:: UnionBatchSelectObjectRecord!\n");
		return(ret);
	}
	if ((inFp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionDeleteAllUnusedMenuOperationItems:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	while (!feof(inFp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(inFp,recStr,sizeof(recStr))) <= 0)
			continue;
		memset(ID,0,sizeof(ID));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conMenuItemOperationDefFldNameMenuName,ID,sizeof(ID))) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedMenuOperationItems:: UnionReadRecFldFromRecStr [%s] form [%04d][%s]\n",
					conMenuItemOperationDefFldNameMenuName,lenOfRecStr,recStr);
			continue;
		}
		// �ж����Ƿ����
		if (UnionIsInterfaceItemContainSpecStrExists(conTBLNameMenuItemOperationDef,"|%s=%s|",conMenuItemOperationDefFldNameMenuName,ID))
			continue;
		// ������
		// ɾ��
		/*
		if ((ret = UnionDeleteUniqueObjectRecord(conTBLNameMenuItemOperationDef,recStr)) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedMenuOperationItems:: UnionDeleteUniqueObjectRecord [%s]!\n",recStr);
			continue;
		}
		*/
		fprintf(outFp,"delete [%s] - [%s][%s] ok!\n",conTBLNameMenuItemOperationDef,mainMenuName,ID);
		deleteNum++;
	}
	fclose(inFp);
	UnionDeleteSelfTempFile();
	return(deleteNum);
}

/*
����	
	ɾ������û���õ��Ĳ����嵥����,����ɾ������д�뵽ָ���ļ���
�������
	mainMenuName		���˵�����
	fp			�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionDeleteAllUnusedOperationListItems(char *mainMenuName,FILE *fp)
{
	int	ret;
	FILE	*outFp = stdout;
	char	fileName[256+1];
	FILE	*inFp;
	char	recStr[8192];
	int	lenOfRecStr;
	int	deleteNum = 0;
	char	ID[128+1];
		
	char	condition[256+1];
	
	strcpy(condition,"");
		
	if (!UnionIsInterfaceItemsGrpInited())
		return(0);
	if (fp != NULL)
		outFp = fp;
	// ѡ�����м�¼
	UnionFormSelfTempFileName(fileName);
	if ((ret = UnionBatchSelectObjectRecord(conTBLNameOperationList,condition,fileName)) < 0)
	{
		UnionUserErrLog("in UnionDeleteAllUnusedOperationListItems:: UnionBatchSelectObjectRecord!\n");
		return(ret);
	}
	if ((inFp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionDeleteAllUnusedOperationListItems:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	while (!feof(inFp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(inFp,recStr,sizeof(recStr))) <= 0)
			continue;
		memset(ID,0,sizeof(ID));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conOperationListFldNameID,ID,sizeof(ID))) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedOperationListItems:: UnionReadRecFldFromRecStr [%s] form [%04d][%s]\n",
					conOperationListFldNameID,lenOfRecStr,recStr);
			continue;
		}
		// �ж����Ƿ����
		if (UnionIsInterfaceItemContainSpecStrExists(conTBLNameOperationList,"|%s=%s|",conOperationListFldNameID,ID))
			continue;
		// ������
		// ɾ��
		/*
		if ((ret = UnionDeleteUniqueObjectRecord(conTBLNameOperationList,recStr)) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedOperationListItems:: UnionDeleteUniqueObjectRecord [%s]!\n",recStr);
			continue;
		}
		*/
		fprintf(outFp,"delete [%s] - [%s][%s] ok!\n",conTBLNameOperationList,mainMenuName,ID);
		deleteNum++;
	}
	fclose(inFp);
	UnionDeleteSelfTempFile();
	return(deleteNum);
}

/*
����	
	ɾ������û���õ��ı���,����ɾ������д�뵽ָ���ļ���
�������
	mainMenuName		���˵�����
	fp			�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionDeleteAllUnusedTableListItems(char *mainMenuName,FILE *fp)
{
	int	ret;
	FILE	*outFp = stdout;
	char	fileName[256+1];
	FILE	*inFp;
	char	recStr[8192];
	int	lenOfRecStr;
	int	deleteNum = 0;
	char	ID[128+1];
		
	char	condition[256+1];
	
	strcpy(condition,"");
		
	if (!UnionIsInterfaceItemsGrpInited())
		return(0);
	if (fp != NULL)
		outFp = fp;
	// ѡ�����м�¼
	UnionFormSelfTempFileName(fileName);
	if ((ret = UnionBatchSelectObjectRecord(conTBLNameTableList,condition,fileName)) < 0)
	{
		UnionUserErrLog("in UnionDeleteAllUnusedTableListItems:: UnionBatchSelectObjectRecord!\n");
		return(ret);
	}
	if ((inFp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionDeleteAllUnusedTableListItems:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	while (!feof(inFp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(inFp,recStr,sizeof(recStr))) <= 0)
			continue;
		memset(ID,0,sizeof(ID));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,conTableListFldNameTableName,ID,sizeof(ID))) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedTableListItems:: UnionReadRecFldFromRecStr [%s] form [%04d][%s]\n",
					conTableListFldNameTableName,lenOfRecStr,recStr);
			continue;
		}
		// �ж����Ƿ����
		if (UnionIsInterfaceItemContainSpecStrExists(conTBLNameTableList,"|%s=%s|",conTableListFldNameTableName,ID))
			continue;
		// ������
		// ɾ��
		/*
		if ((ret = UnionDeleteUniqueObjectRecord(conTBLNameTableList,recStr)) < 0)
		{
			UnionUserErrLog("in UnionDeleteAllUnusedTableListItems:: UnionDeleteUniqueObjectRecord [%s]!\n",recStr);
			continue;
		}
		*/
		fprintf(outFp,"delete [%s] - [%s][%s] ok!\n",conTBLNameTableList,mainMenuName,ID);
		deleteNum++;
	}
	fclose(inFp);
	UnionDeleteSelfTempFile();
	return(deleteNum);
}
