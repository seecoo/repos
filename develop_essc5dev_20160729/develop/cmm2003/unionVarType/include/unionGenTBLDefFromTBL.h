//	Wolfgang Wang
//	2009/4/29

#ifndef _unionGenTBLDefFromTBL_
#define _unionGenTBLDefFromTBL_

#include "unionComplexDBCommon.h"
#include "unionComplexDBObjectDef.h"
#include "unionFldGrp.h"
#include "unionTBLQueryConf.h"
#include "unionMenuItemOperationDef.h"
#include "unionMenuItemDef.h"
#include "unionOperatorType.h"

/*
����	
	ɾ��ָ����Ĳ�������
�������
	tableName	������
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionDeleteOperationOfSpecTable(char *tableName);

/*
����	
	ɾ������
�������
	tableName	������
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionDeleteSpecTableByName(char *tableName);

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
int UnionGenerateComplexFldAssignmentMethodToSpecFile(char *mainMenuName,char *fldID,char *fileName);
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
int UnionGenerateComplexFldAssignmentMethodToFp(char *mainMenuName,char *fldID,FILE *fp);

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
int UnionGenerateMenuDefFileFromMenuNameToFp(char *tellerTypeID,char *mainMenuName,char *menuGrpName,FILE *fp);

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
int UnionWriteMenuItemDefToSpecFile(PUnionMenuItemDef pmenuItemDef,PUnionMenuItemOperationDef prec,FILE *fp, int iDownloadAllMenuItem);

/*
����	
	��ȡָ����ı��Ҽ��˵���
�������
	level			��������
	tableName		����
	mainMenuName		���˵�����
	sizeOfMenuItemList	�˵����嵥����󳤶�
�������
	menuItemList	�˵����嵥
����ֵ
	>=0	�˵����嵥�ĳ���
	<0	������
*/
int UnionReadMenuItemListOfSpecTable(char *tellerTypeID,char *tableName,char *mainMenuName,int menuType,char *menuItemList,int sizeOfMenuItemList);

/*
����	
	��ȡָ���ļ����ָ���Ķ����˵��Ĳ˵���Ĳ���Ȩ���嵥
�������
	level			��������
	mainMenuName		���˵�����
	menuName		�����˵�����
	sizeOfMenuItemList	�˵����嵥����󳤶�
�������
	menuItemList	�˵����嵥
����ֵ
	>=0	�˵����嵥�ĳ���
	<0	������
*/
int UnionReadMenuItemListOfSpecSecondaryMenu(char *tellerTypeID,char *mainMenuName,char *menuName,char *menuItemList,int sizeOfMenuItemList);

/*
����	
	��ȡָ���ļ����ָ�������˵��Ĳ˵���Ĳ���Ȩ���嵥
�������
	tellerTypeID	��������
	menuName	���˵�����
	sizeOfMenuItemList	�˵����嵥����󳤶�
�������
	menuItemList	�˵����嵥
����ֵ
	>=0	�˵����嵥�ĳ���
	<0	������
*/
int UnionReadMenuItemListOfSpecMainMenu(char *tellerTypeID,char *menuName,char *menuItemList,int sizeOfMenuItemList);

/*
����	
	�������嵥(ÿ������tableField���ж���)������һ�������嵥
�������
	fldList		���ʶ�嵥
	lenOfFldList	���ʶ�嵥�ĳ���
	maxFldNum	�������Ŀ
�������
	fldDefGrp	������������
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionGenerateFldGrpDefFromFldIDList(char *fldList,int lenOfFldList,TUnionObjectFldDef fldDefGrp[],int maxFldNum);

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
int UnionGenerateFldListFromFldIDList(char *fldList,int lenOfFldList,PUnionObjectFldGrp pfldListGrp);


/*
����	
	�������嵥������һ������ṹ
�������
	foreignTBLName		�������
	myFldList		�������ʶ�嵥
	lenOfMyFldList		�����ʶ�嵥�ĳ���
	foreignFldList		�������ʶ�嵥
	lenOfForeignFldList	�����ʶ�嵥�ĳ���
�������
	pforeignKeyRec	����������ṹ
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionGenerateForeignKeyRecFromFldIDList(char *foreignTBLName,char *myFldList,int lenOfMyFldList,char *foreignFldList,int lenOfForeignFldList,PUnionDefOfRelatedObject pforeignKeyRec);
/*
����	
	���ݱ���(��tableList���ж���)������һ������ṹ
�������
	tblName		����
�������
	pobject		����
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateObjectDefFromTBLDefInTableList(char *tblName,PUnionObject pobject);

/*
����	
	������ͼ����(��viewList���ж���)������һ������ṹ
�������
	tblName		����
	mainMenuName	���˵�
�������
	prec		����
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateObjectDefFromTBLDefInViewList(char *tblName,char *mainMenuName,PUnionObject prec);

/*
����	
	���ݱ���(��tableList���ж���)������һ�������ļ�
�������
	tblName		����
	overwriteMode	���Ǳ�־������ļ��Ѵ���
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionCreateObjectDefFromTBLDefInTableList(char *tblName,int overwriteMode);

/*
����	
	���ݱ��嵥���еļ�¼���������б�Ķ����ļ�
�������
	modeWhenFileExists	����ļ��Ѵ��ڵĲ�������
�������
	��
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionCreateAllObjectDefFromTBLDefInTableList(int modeWhenFileExists);

/*
����	
	���ݱ���(��tableList���ж���)������һ�������ļ�
�������
	tblName		����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionCreateComplexDBTBLCreateSQLFileFromTBLDefInTableList(char *tblName);

/*
����	
	���ݱ��嵥���еļ�¼���������б�Ľ����ļ�
�������
	��
�������
	��
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionCreateAllComplexDBTBLCreateSQLFileFromTBLDefInTableList();

/*
����	
	���ݱ���(��tableList���ж���)������һ�������sql���
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionCreateObjectDefSQLFromTBLDefInTableList(char *tblName,FILE *fp);

/*
����	
	���ݱ���(��tableList���ж���)������һ�������sql���
�������
	tblName		����
	fileName	�洢���������ļ�
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionCreateObjectDefSQLFromTBLDefInTableListToSpecFile(char *tblName,char *fileName);

/*
����	
	���ݱ��嵥���еļ�¼���������б�Ľ���SQL���
�������
	fp	�ļ����
�������
	��
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionCreateAllObjectDefSQLFromTBLDefInTableList(FILE *fp);

/*
����	
	���ݱ��嵥���еļ�¼���������б�Ľ���SQL���
�������
	fileName	�洢���������ļ�
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionCreateAllObjectDefSQLFromTBLDefInTableListToSpecFile(char *fileName);

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
int UnionGenerateQueryFldGrpDefFromFldIDList(char *fldList,int lenOfFldList,PUnionQueryFldGrp pfldGrp);

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
int UnionGetAllFldNameFromFldIDList(char *fldList,int lenOfFldList,char fldNameGrp[][128+1],int maxNum);

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
int UnionGenerateQueryFldListFromFldIDList(char *fldList,int lenOfFldList,PUnionFldGrp pfldListGrp);

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
int UnionPrintFldRecValueAssignMethodToFp(char *method,FILE *fp);

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
int UnionRenameFldOfSpecQueryFldGrpDef(char *fldList,int lenOfFldList,PUnionQueryFldGrp pfldGrp);

/*
����	
	���ݲ�����Ȩ�����ɲ�����ʾ
�������
	fp	�ļ����
�������
	��
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionGenerateOperationInfoFromAuthDefTBL(FILE *fp);

/*
����	
	���ݲ�����Ȩ�����ɲ�����ʾ
�������
	fileName	�洢���������ļ�
�������
	��
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionGenerateOperationInfoFromAuthDefTBLToSpecFile(char *fileName);

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
	char *loopList,int lenOfLoopList,char *viewLevelList,int lenOfViewLevelList,char *editLevelList,int lenOfEditLevelList,FILE *fp);

/*
����	
	���ݱ���(��tableList���ж���)������һ������ṹ
�������
	tblName		����
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateTBLQueryConfFromTBLDefInTableListToFp(char *tblName,FILE *fp);

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
int UnionGenerateTBLQueryConfFromViewDefToFp(char *tblName,char *mainMenuName,FILE *fp);

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
int UnionGenerateTBLQueryConfFromViewDefToSpecFile(char *tblName,char *mainMenuName,char *fileName);

/*
����	
	����һ��ղ����˵���
�������
	tblName		����
	num		�ղ�����Ŀ
�������
	��
����ֵ
	>=0	����ļ�¼��Ŀ
	<0	������
*/
int UnionGenerateNullTableMenuItemGrp(char *tableName,int num);

/*
����	
	���ݱ���(��tableList���ж���)������һ������ṹ
�������
	tblName		����
	fileName	�ļ���
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateTBLQueryConfFromTBLDefInTableListToSpecFile(char *tblName,char *fileName);

/*
����	
	�������嵥(ÿ������tableField���ж���)������һ���������嵥
�������
	fldList		���ʶ�嵥
	lenOfFldList	���ʶ�嵥�ĳ���
�������
	fldRef		�������嵥
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionGenerateQueryFldRefFromFldIDList(char *fldList,int lenOfFldList,char *fldRef);

/*
����	
	���ݱ���(��tableList���ж���)������һ����¼˫������˵�
�������
	tellerTypeID		����Ա����
	tblName		����
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateRecDoubleClickMenuFromTBLDefInTableListToFp(char *tellerTypeID,char *tblName,FILE *fp);

/*
����	
	���ݱ���(��tableList���ж���)������һ����¼˫������˵�
�������
	tellerTypeID	����Ա����
	tblName		����
	fileName	�洢���������ļ�
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateRecDoubleClickMenuFromTBLDefInTableListToSpecFile(char *tellerTypeID,char *tblName,char *fileName);

/*
����	
	���ݱ���(��tableList���ж���)������һ����¼�Ҽ�����˵�
�������
	tellerTypeID		����Ա����
	tblName		����
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateRecPopupMenuFromTBLDefInTableListToFp(char *tellerTypeID,char *tblName,FILE *fp);

/*
����	
	���ݱ���(��tableList���ж���)������һ����¼�Ҽ�����˵�
�������
	tellerTypeID		����Ա����
	tblName		����
	fileName	�洢���������ļ�
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateRecPopupMenuFromTBLDefInTableListToSpecFile(char *tellerTypeID,char *tblName,char *fileName);

/*
����	
	���ݲ˵����嵥������һ������˵�
�������
	tblName		����
	tblAlais	�����
	fldList		�˵����嵥
	lenOfFldList	�˵����嵥����
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateInterfaceMenuFromMenuItemListToFp(char *tblName,char *tblAlais,char *fldList,int lenOfFldList,FILE *fp);

/*
����	
	���ݱ���(��tableList���ж���)������һ������˵�
�������
	tellerTypeID		����Ա����
	tblName		����
	menuType	�˵�����
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateInterfaceMenuFromTBLDefInTableListToFp(char *tellerTypeID,char *tblName,int menuType,FILE *fp);

/*
����	
	���ݱ���(��tableList���ж���)������һ�������Ҽ�����˵�
�������
	tellerTypeID		����Ա����
	tblName		����
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateFormPopupMenuFromTBLDefInTableListToFp(char *tellerTypeID,char *tblName,FILE *fp);

/*
����	
	���ݱ���(��tableList���ж���)������һ�������Ҽ�����˵�
�������
	tellerTypeID		����Ա����
	tblName		����
	fileName	�洢���������ļ�
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateFormPopupMenuFromTBLDefInTableListToSpecFile(char *tellerTypeID,char *tblName,char *fileName);

/*
����	
	���ݱ����ID�������������
�������
	fldID		�����ID
�������
	fldName		�������
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionReadFieldNameByFieldIDInTableField(char *fldID,char *fldName);

/*
����	
	���ݱ���(��tableList���ж���)������һ����
�������
	tblName		����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionCreateObjectFromTBLDefInTableList(char *tblName);

/*
����	
	���ݱ���(��tableList���ж���)������һ����Ĳ����ʿ�
�������
	tblName		����
�������
	��
����ֵ
	>=0	����ļ�¼��Ŀ
	<0	������
*/
int UnionGenerateOperationFromTBLDefInTableList(char *tblName);

/*
����	
	���ݱ���(��tableList���ж���)��¼��һ����ĳ�ʼ��¼
�������
	tblName		����
�������
	��
����ֵ
	>=0	����ļ�¼��Ŀ
	<0	������
*/
int UnionInsertInitRecOfSpecTBLOnTBLDefInTableList(char *tblName);

/*
����	
	Ϊһ����������������
�������
	tellerTypeID	����Ա�����ʶ
	tableName	����
�������
	fileRecved	�Ƿ����ļ�����
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionGenerateAllMngSvrConfForTable(char *tellerTypeID,char *tableName,int *fileRecved);

/*
����	
	Ϊһ����������������
�������
	tellerTypeID	����Ա����
	tableName	����
	tableAlais	�����
	isView		�Ƿ�����ͼ��1�ǣ�0����
�������
	fileRecved	�Ƿ����ļ�����
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionGenerateAllMngSvrConfForSpecTBLOrSpecView(char *tellerTypeID,char *tableName,char *tableAlais,int isView,int *fileRecved);

/*
����	
	���ݱ���࣬�������˵�
�������
	��
�������
	��
����ֵ
	>=0	��������Ŀ
	<0	������
*/
int UnionCreateMainMenuByTBLType();

/*
����	
	���ݱ���࣬���������˵�
�������
	nameOf2LevelMenu	�����˵�����
�������
	��
����ֵ
	>=0	��������Ŀ
	<0	������
*/
int UnionCreate2LevelMenuByTBLType(char *nameOf2LevelMenu);

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
int UnionCreate2LevelMenuByMenuGrpName(char *tellerTypeID,char *mainMenuName,char *menuGrpName,char *fileName);

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
int UnionCreateMainMenuByMainMenuName(char *tellerTypeID,char *mainMenuName,char *fileName);

/*
����	
	��ȡָ���ļ����ָ���Ĳ˵���Ĳ���Ȩ���嵥
�������
	tellerTypeID		��������
	mainMenuName		���˵�����
	menuName		�˵�������
	sizeOfMenuItemList	�˵����嵥����󳤶�
�������
	menuItemList	�˵����嵥
����ֵ
	>=0	�˵����嵥�ĳ���
	<0	������
*/
int UnionReadMenuItemListOfSpecMenuGrp(char *tellerTypeID,char *mainMenuName,char *menuGrpName,char *menuItemList,int sizeOfMenuItemList);

/*
����	
	��һ�����������嵥���������ͣ�ת��Ϊ���������嵥������
�������
	oriList		Դ����
	lenOfOriList	Դ���𴮳���
	sizeOfBuf	Ŀ�꼶�𴮻����С
�������
	desList		Ŀ�꼶��
����ֵ
	>=0	�ɹ���Ŀ�꼶�𴮳���
	<0	������
*/
int UnionConvertOperatorLevelListFromIDToLevel(char *oriList,int lenOfOriList,char *desList,int sizeOfBuf);
/*
����	
	���ݱ���࣬�������в˵�
�������
	��
�������
	fileRecved	�Ƿ����ļ�����
����ֵ
	>=0	��������Ŀ
	<0	������
*/
int UnionCreateAllMenuByTBLType(int *fileRecved);

/*
����	
	������ͼ����(��tableList���ж���)������һ������˵�
�������
	tellerTypeID		����Ա����
	viewName		����
	mainMenuName	���˵�����
	menuType	�˵�����
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateInterfaceMenuFromViewDefToFp(char *tellerTypeID,char *viewName,char *mainMenuName,int menuType,FILE *fp);

/*
����	
	������ͼ����(��tableList���ж���)������һ������˵�
�������
	viewName		����
	mainMenuName		���˵�����
	tellerTypeID		����Ա����
	menuType	�˵�����
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateInterfaceMenuFromViewDefToSpecFile(char *tellerTypeID,char *viewName,char *mainMenuName,int menuType,char *fileName);

/*
����	
	������ͼ����(��tableList���ж���)������һ����ͼ���浯���˵�
�������
	viewName		����
	mainMenuName		���˵�����
	tellerTypeID		����Ա����
	menuType	�˵�����
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateFormPopMenuFromViewDefToSpecFile(char *tellerTypeID,char *viewName,char *mainMenuName,char *fileName);

/*
����	
	������ͼ����(��tableList���ж���)������һ����ͼ��¼�����˵�
�������
	viewName		����
	mainMenuName		���˵�����
	tellerTypeID		����Ա����
	menuType	�˵�����
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateRecPopMenuFromViewDefToSpecFile(char *tellerTypeID,char *viewName,char *mainMenuName,char *fileName);

/*
����	
	������ͼ����(��tableList���ж���)������һ��˫����¼�����˵�
�������
	viewName		����
	mainMenuName		���˵�����
	tellerTypeID		����Ա����
	menuType	�˵�����
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateRecDoubleClickMenuFromViewDefToSpecFile(char *tellerTypeID,char *viewName,char *mainMenuName,char *fileName);

/*
����	
	���ݱ���(��tableList���ж���)������һ�����ȱʡ��ͼ����
�������
	tblName		����
	mainMenuName	���˵�����
	sizeOfBuf	�����С
�������
	viewRecStr	��ͼ���崮
����ֵ
	>=0	��ͼ���崮�ĳ���
	<0	������
*/
int UnionGenerateDefaultViewOfTBL(char *tblName,char *mainMenuName,char *viewRecStr,int sizeOfBuf);

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
int UnionGenerateEnumValueDefFileToFp(char *tellerTypeID,char *mainMenuName,char *enumFileName,FILE *fp);

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
int UnionGenerateEnumValueDefFile(char *tellerTypeID,char *mainMenuName,char *enumFileName,char *fileName);

/*
����	
	���ݱ��еļ�¼��Ϊһ�����˵�����enum��һ����¼��Ӧһ��enumֵ
�������
	tblName		����
	mainMenuName	���˵�����
	enumFileName	ö������
�������
	��
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionGenerateEnumDefFromTBLForSpecMainMenu(char *tblName,char *mainMenuName,char *enumFileName);

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
int UnionGenerateViewAuthorizationToFp(char *mainMenuName,char *viewName,FILE *fp);

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
int UnionGenerateViewAuthorizationToSpecFile(char *mainMenuName,char *viewName,char *fileName);

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
int UnionReadAllOperatorLevelStr(char *levelStr);

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
int UnionPrintAllOperatorLevelStrToFp(char *levelStr,char *levelName,int authoSet,FILE *fp);

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
int UnionPrintAllAuthOperatorLevelStrToFp(char *levelStr,char *levelName,char *authLevelStr,FILE *fp);

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
int UnionPrintAllAuthOperatorLevelIDStrToFp(TUnionOperatorType tellerTypeGrp[],int tellerTypeNum,char *levelName,char *authLevelIDStr,FILE *fp);

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
int UnionReadAllOperatorLevelStrOfSpecLevelIDList(char *levelIDStr,TUnionOperatorType tellerTypeGrp[],int tellerTypeNum,char *levelStr);

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
int UnionFormAllOperatorLevelStr(TUnionOperatorType tellerTypeGrp[],int tellerTypeNum,char *levelStr);

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
int UnionGenerateMenuGrpDefDocToFp(char *mainMenuName,char *menuGrpName,FILE *fp);

/*
����	
	����һ���˵���Ķ����ĵ�
�������
	mainMenuName	���˵�����
	menuGrpName	�˵�����
	fileName	�ļ���
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateMenuGrpDefDocToSpecFile(char *mainMenuName,char *menuGrpName,char *fileName);


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
int UnionGenerateInterfaceMenuDefDocFromViewDefToFp(char *viewName,char *mainMenuName,int menuType,FILE *fp);

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
int UnionGenerateInterfaceMenuDefDocFromViewDefToSpecFile(char *viewName,char *mainMenuName,int menuType,char *fileName);
	
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
int UnionGenerateFormPopMenuDefDocFromViewDefToSpecFile(char *viewName,char *mainMenuName,char *fileName);

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
int UnionGenerateRecPopMenuDefDocFromViewDefToSpecFile(char *viewName,char *mainMenuName,char *fileName);

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
int UnionGenerateRecDoubleClickMenuDefDocFromViewDefToSpecFile(char *viewName,char *mainMenuName,char *fileName);

/*
����	
	����ָ�����˵������ĵ�
�������
	mainMenuName		���˵�����
	fp			�ļ�ָ��
�������
	��
����ֵ
	>=0			�˵����嵥�ĳ���
	<0			������
*/
int UnionGenerateMainMenuDefDocToFp(char *mainMenuName,FILE *fp);
/*
����	
	����ָ�����˵������ĵ�
�������
	mainMenuName		���˵�����
	fileName		�ļ�����
�������
	��
����ֵ
	>=0			�˵����嵥�ĳ���
	<0			������
*/
int UnionGenerateMainMenuDefDocToSpecFile(char *mainMenuName,char *fileName);

/*
����	
	�������˵����ƣ��������˵���ʾ�ļ�
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
int UnionGenerateMainMenuForTestToSpecFile(char *tellerTypeID,char *mainMenuName,char *fileName);

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
int UnionGenerateAllMainMenuDefToFp(FILE *fp);

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
int UnionGenerateAllMainMenuDefToSpecFile(char *fileName);

/*
����	
	�����ֵ,��ȡ����������
�������
	cpField		��ȫ�����ж�������������Ӧ��
	iLenOfField	�򳤶�
	cDel		��ķָ���
	iType		�������,1:�����ѡ��,2:�����ѡ��,3:��Ӧ��ѡ��,4:��Ӧ��ѡ��
	fp			�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateSecuServiceDefInDataPackFldListFormatToFp(char *cpField, int iLenOfField, char cDel, int iType, FILE *fp);

/*
����	
	��ȡ����������
�������
	iServiceID	�������
	fp			�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateSecuServiceDefDocToFp(int iServiceID, FILE *fp);

/*
����	
	���ݷ��������������������ļ�
�������
	iServiceID	�������
	fileName	���ɵĲ˵��ļ�����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateSecuServiceDefDocToSpecFile(int iServiceID, char *fileName);

/*
����	
	��һ������д�ɴ�
�������
	flag		����ı�ʶ
	pfldGrp		����
	iSizeofStr	�ַ�����������С
�������
	str			�ַ���
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionCreateFldGrpIntoSQLToString(char *flag, PUnionObjectFldGrp pfldGrp, char *str, int iSizeofStr);

/*
����	
	��һ�����崴��SQL���
�������
	fldList		���ʶ�嵥
	lenOfFldList	���ʶ�嵥�ĳ���
	fp		�ļ����
�������
	��
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionGetCreateSQLFromObjectDef(PUnionObject prec, char *cpCreateSQL, int iSizeOfCreateSQL);

/*
����	
	�����ݿ��н���
�������
	tblName		����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionCreateTableByTBLDefInTableList(char *tblName);

#endif
