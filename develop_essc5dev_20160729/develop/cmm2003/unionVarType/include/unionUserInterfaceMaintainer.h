//	Wolfgang Wang
//	2009/4/29

#ifndef _unionUserInterfaceMaintainer_
#define _unionUserInterfaceMaintainer_

#include "unionDataTBLList.h"

#define conInterfaceItemTagMainMenu		conTBLNameMainMenu
#define conInterfaceItemTagMenuGrp		conTBLNameMenuDef
#define conInterfaceItemTagMenuItem		conTBLNameMenuItemDef
#define conInterfaceItemTagMenuOperation	conTBLNameMenuItemOperationDef
#define conInterfaceItemTagOperationAuth	conTBLNameOperationAuthorization
#define conInterfaceItemTagView			conTBLNameViewList
#define conInterfaceItemTagTable		conTBLNameTableList
#define conInterfaceItemTagTableField		conTBLNameTableField
#define conInterfaceItemTagComplexField		conTBLNameComplexField
#define conInterfaceItemTagEnum			conTBLNameEnumValueDef
#define conInterfaceItemTagOperationTagDef	conTBLNameOperationList

#define conTblFldValueMethodUseEnum		100
#define conTblFldValueMethodUseTable		101
#define conTblFldValueMethodUseComplexFld	102
#define conTblFldValueMethodUseNonRes		103

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
int UnionReadAttrFromFieldMethodDef(char *valueStr,char *resName,int sizeOfResName);

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
int UnionGenerateAllInterfaceItemsOfSpecMainMenu(char *mainMenuName);

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
int UnionGenerateAllInterfaceItemsOfSpecMainMenuToSpecFile(char *mainMenuName,char *fileName);

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
int UnionIsTableUsedByMainMenu(char *mainMenuName,char *tableName);

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
int UnionCollectAllInterfaceItemsOfSpecMainMenu(char *mainMenuName);

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
void UnionFreeAllCollectedInterfaceItemsOfSpecMainMenu(char *mainMenuName);

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
int UnionGenerateAllInterfaceItemsOfSpecMenuGrp(char *mainMenuName,int menuType,char *menuGrpName);

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
int UnionGenerateAllInterfaceItemsOfSpecMenuItem(char *mainMenuName,char *menuGrpName,char *itemName);

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
int UnionGenerateAllInterfaceItemsOfSpecOperation(char *mainMenuName,char *viewName,char *operationName);

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
int UnionGenerateAllInterfaceItemsOfSpecView(char *mainMenuName,char *viewName);

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
int UnionGenerateAllInterfaceItemsOfSpecTable(char *mainMenuName,char *tableName);

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
int UnionGenerateAllInterfaceItemsOfSpecTableFieldList(char *mainMenuName,char *tableName,char *fieldList);

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
int UnionGenerateAllInterfaceItemsOfSpecTableField(char *mainMenuName,char *fieldName);

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
int UnionGenerateAllInterfaceItemsOfSpecEnum(char *mainMenuName,char *enumName);

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
int UnionGenerateAllInterfaceItemsOfSpecComplexFld(char *mainMenuName,char *complexFldName);

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
int UnionGenerateAllInterfaceItemsOfSpecOperationAuth(char *mainMenuName,int resID,int operationID);

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
int UnionGenerateAllInterfaceItemsOfSpecOperationTagDef(char *mainMenuName,char *serviceName);

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
int UnionDeleteAllUnusedInterfaceItems(FILE *fp);

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
int UnionDeleteAllUnusedInterfaceItemsWithOutputToSpecFile(char *fileName);

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
int UnionDeleteAllUnusedComplexFldItems(char *mainMenuName,FILE *fp);

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
int UnionDeleteAllUnusedEnumDefItems(char *mainMenuName,FILE *fp);

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
int UnionDeleteAllUnusedViewListItems(char *mainMenuName,FILE *fp);

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
int UnionDeleteAllUnusedTableFieldItems(char *mainMenuName,FILE *fp);

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
int UnionDeleteAllUnusedOperationAuthorizationItems(char *mainMenuName,FILE *fp);

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
int UnionDeleteAllUnusedMenuItemItems(char *mainMenuName,FILE *fp);

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
int UnionDeleteAllUnusedMenuGrpItems(char *mainMenuName,FILE *fp);

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
int UnionDeleteAllUnusedMainMenuItems(char *mainMenuName,FILE *fp);

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
int UnionDeleteAllUnusedMenuOperationItems(char *mainMenuName,FILE *fp);

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
int UnionDeleteAllUnusedOperationListItems(char *mainMenuName,FILE *fp);

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
int UnionDeleteAllUnusedTableListItems(char *mainMenuName,FILE *fp);

#endif
