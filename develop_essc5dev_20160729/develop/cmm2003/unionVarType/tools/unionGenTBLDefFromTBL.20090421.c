//	Wolfgang Wang
//	2009/4/29

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

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

#include "unionFldGrp.h"
#include "unionRecFile.h"
#include "unionTBLQueryConf.h"
#include "unionTableField.h"
#include "unionTableList.h"
#include "unionDataTBLList.h"
#include "unionOperationAuthorization.h"
#include "unionMenuItemType.h"
//#include "unionLevel2Menu.h"
//#include "unionSecondaryMenu.h"
//#include "unionTableMenu.h"
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
int UnionDeleteOperationOfSpecTable(char *tableName)
{
	int 				ret = 0;
	char 				cond_buf[1024+1];
	TUnionMenuItemOperationDef	rec[48+1];
	int				recNum;
	int				recIndex;

loop:
	memset(&rec,0,sizeof(rec));
	memset(cond_buf, 0, sizeof cond_buf);
	sprintf(cond_buf, "operatingTableName=%s|", tableName);
	// ��ȡ����ʹ���˱���Ĳ���
	if ((recNum = UnionBatchReadMenuItemOperationDefRec(cond_buf,rec,48)) < 0)
	{
		UnionUserErrLog("in UnionDeleteOperationOfSpecTable:: UnionBatchReadMenuItemOperationDefRec [%s]!\n",cond_buf);
		return(recNum);
	}
	if (recNum == 0)
		goto deleteOperation;
	for (recIndex = 0; recIndex < recNum; recIndex++)
	{
		// ����ɾ��ʹ���˱�������Ĳ˵���
		sprintf(cond_buf, "menuName=%s|", rec[recIndex].menuName);
		if ((ret = UnionDeleteUniqueObjectRecord("menuItemDef", cond_buf)) < 0) 
		{
			if ((ret != -100) && (ret != -1403) && (ret != errCodeDatabaseMDL_RecordNotFound))
			{
				UnionUserErrLog("in UnionDeleteOperationOfSpecTable:: UnionDeleteUniqueObjectRecord tablename = %s condition = %s ret = %d\n", "menuItemDef", cond_buf, ret);
				return ret;
			}
		}
	}
	if (recNum >= 48)	// ��������ʹ���˱���Ĳ���
		goto loop;
deleteOperation:
	// delete menuItemOperationDef by name of main menu
	memset(cond_buf, 0, sizeof cond_buf);
	sprintf(cond_buf, "operatingTableName=%s|", tableName);
	if ((ret = UnionDeleteUniqueObjectRecord("menuItemOperationDef", cond_buf)) < 0) 
	{
		if ((ret != -100) && (ret != -1403) && (ret != errCodeDatabaseMDL_RecordNotFound))
		{
			UnionUserErrLog("in UnionDeleteOperationOfSpecTable:: UnionDeleteUniqueObjectRecord tablename = %s condition = %s ret = %d\n", "menuItemOperationDef", cond_buf, ret);
			return ret;
		}
	}
	return 0;
}

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
int UnionDeleteSpecTableByName(char *tableName)
{
	int ret = 0;
	char cond_buf[1024+1];
	
	// delete viewList by name of main menu
	memset(cond_buf, 0, sizeof cond_buf);
	sprintf(cond_buf, "tableName=%s|", tableName);
	if ((ret = UnionDeleteUniqueObjectRecord("viewList", cond_buf)) < 0) 
	{
		if ((ret != -100) && (ret != -1403) && (ret != errCodeDatabaseMDL_RecordNotFound))
		{
			UnionUserErrLog("in UnionDeleteSpecTableByName:: UnionDeleteUniqueObjectRecord tablename = %s condition = %s ret = %d\n", "viewList", cond_buf, ret);
			return ret;
		}
	}
	if ((ret = UnionDeleteUniqueObjectRecord("operationAuthorization", cond_buf)) < 0) 
	{
		if ((ret != -100) && (ret != -1403) && (ret != errCodeDatabaseMDL_RecordNotFound))
		{
			UnionUserErrLog("in UnionDeleteSpecTableByName:: UnionDeleteUniqueObjectRecord tablename = %s condition = %s ret = %d\n", "operationAuthorization", cond_buf, ret);
			return ret;
		}
	}
	// delete menuItemOperationDef by name of main menu
	if ((ret = UnionDeleteOperationOfSpecTable(tableName)) < 0) 
	{
		UnionUserErrLog("in UnionDeleteSpecTableByName:: UnionDeleteOperationOfSpecTable!\n");
		return ret;
	}
	if ((ret = UnionDeleteUniqueObjectRecord("tableList", cond_buf)) < 0) 
	{
		if ((ret != -100) && (ret != -1403) && (ret != errCodeDatabaseMDL_RecordNotFound))
		{
			UnionUserErrLog("in UnionDeleteSpecTableByName:: UnionDeleteUniqueObjectRecord tablename = %s condition = %s ret = %d\n", "tableList", cond_buf, ret);
			return ret;
		}
	}

	return 0;
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
int UnionGenerateFldGrpDefFromFldIDList(char *fldList,int lenOfFldList,TUnionObjectFldDef fldDefGrp[],int maxFldNum)
{
	int			ret;
	TUnionTableField	fldRec;
	int			fldNum,fldIndex;
	char			fldNameGrp[conMaxFldNumPerObject][128];
	PUnionObjectFldDef	pfldDef;
	
	if ((fldList == NULL) || (lenOfFldList < 0))
		return(errCodeParameter);

	// ������嵥
	memset(fldNameGrp,0,sizeof(fldNameGrp));
	if ((ret = UnionSeprateVarStrIntoVarGrp(fldList,lenOfFldList,conSeparableSignOfFieldNameInFldList,fldNameGrp,conMaxFldNumPerObject)) < 0)
	{
		UnionUserErrLog("in UnionGenerateFldGrpDefFromFldIDList:: UnionSeprateVarStrIntoVarGrp [%04d][%s]\n",lenOfFldList,fldList);
		return(ret);
	}
	if ((fldNum = ret) > maxFldNum)
	{
		UnionUserErrLog("in UnionGenerateFldGrpDefFromFldIDList:: fldNum [%d] in [%04d][%s] too much\n",fldNum,lenOfFldList,fldList);
		return(errCodeObjectMDL_FieldNumberTooMuch);
	}
	
	// ��ȡ����
	for (fldIndex = 0; fldIndex < fldNum; fldIndex++)
	{
		memset(&fldRec,0,sizeof(fldRec));
		if (strlen(fldNameGrp[fldIndex]) >= sizeof(fldRec.ID))
		{
			UnionUserErrLog("in UnionGenerateFldGrpDefFromFldIDList:: fldName [%s] too long!\n",fldNameGrp[fldIndex]);
			return(errCodeSQLRecFldNameTooLong);
		}
		strcpy(fldRec.ID,fldNameGrp[fldIndex]);
		if ((ret = UnionReadTableFieldRec(&fldRec)) < 0)
		{
			UnionUserErrLog("in UnionGenerateFldGrpDefFromFldIDList:: UnionReadTableFieldRec [%s]!\n",fldNameGrp[fldIndex]);
			return(ret);
		}
		pfldDef = &(fldDefGrp[fldIndex]);
		strcpy(pfldDef->name,fldRec.fieldName);
		pfldDef->type = fldRec.fieldType;
		pfldDef->size = fldRec.size;
		pfldDef->nullPermitted = fldRec.nullPermitted;
		strcpy(pfldDef->defaultValue,fldRec.defaultValue);
		strcpy(pfldDef->remark,fldRec.remark);
	}
	return(fldNum);
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
int UnionGenerateFldListFromFldIDList(char *fldList,int lenOfFldList,PUnionObjectFldGrp pfldListGrp)
{
	int			ret;
	TUnionTableField	fldRec;
	int			fldNum = 0,fldIndex;
	char			fldNameGrp[100][128];
	
	if ((fldList == NULL) || (lenOfFldList < 0) || (pfldListGrp == NULL))
		return(errCodeParameter);

	// ������嵥
	memset(fldNameGrp,0,sizeof(fldNameGrp));
	if ((ret = UnionSeprateVarStrIntoVarGrp(fldList,lenOfFldList,conSeparableSignOfFieldNameInFldList,fldNameGrp,100)) < 0)
	{
		UnionUserErrLog("in UnionGenerateFldListFromFldIDList:: UnionSeprateVarStrIntoVarGrp [%04d][%s]\n",lenOfFldList,fldList);
		return(ret);
	}
	if ((pfldListGrp->fldNum = ret) > 100)
	{
		UnionUserErrLog("in UnionGenerateFldListFromFldIDList:: fldNum [%d] in [%04d][%s] too much\n",fldNum,lenOfFldList,fldList);
		return(errCodeObjectMDL_FieldNumberTooMuch);
	}
	
	// ��ȡ����
	for (fldIndex = 0; fldIndex < pfldListGrp->fldNum; fldIndex++)
	{
		memset(&fldRec,0,sizeof(fldRec));
		if (strlen(fldNameGrp[fldIndex]) >= sizeof(fldRec.ID))
		{
			UnionUserErrLog("in UnionGenerateFldListFromFldIDList:: fldName [%s] too long!\n",fldNameGrp[fldIndex]);
			return(errCodeSQLRecFldNameTooLong);
		}
		strcpy(fldRec.ID,fldNameGrp[fldIndex]);
		if ((ret = UnionReadTableFieldRec(&fldRec)) < 0)
		{
			UnionUserErrLog("in UnionGenerateFldListFromFldIDList:: UnionReadTableFieldRec [%s] too long!\n",fldNameGrp[fldIndex]);
			return(ret);
		}
		strcpy(pfldListGrp->fldNameGrp[fldIndex],fldRec.fieldName);
	}
	return(pfldListGrp->fldNum);
}

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
int UnionGenerateForeignKeyRecFromFldIDList(char *foreignTBLName,char *myFldList,int lenOfMyFldList,char *foreignFldList,int lenOfForeignFldList,PUnionDefOfRelatedObject pforeignKeyRec)
{
	int			ret;
	TUnionTableField	fldRec;
	int			fldNum,fldIndex;
	char			fldNameGrp[100][128];
	
	if (UnionReadIntTypeRECVar("foreignKeyForbidden") > 0)	// ������ʹ�����
		return(0);
		
	if ((myFldList == NULL) || (lenOfMyFldList < 0) || (pforeignKeyRec == NULL) || (foreignTBLName == NULL) || (lenOfForeignFldList < 0) || (lenOfForeignFldList < 0))
	{
		UnionUserErrLog("in UnionGenerateForeignKeyRecFromFldIDList:: parameter error! lenOfMyFldList = [%d] lenOfForeignFldList = [%d]\n",lenOfMyFldList,lenOfForeignFldList);
		return(errCodeParameter);
	}
	
	if (strlen(foreignTBLName) == 0)
	{
		UnionAuditLog("in UnionGenerateForeignKeyRecFromFldIDList:: foreignTBLName is null length!\n");
		return(0);
	}
	// �����
	strcpy(pforeignKeyRec->objectName,foreignTBLName);
	
	// ��������
	if ((ret = UnionGenerateFldListFromFldIDList(myFldList,lenOfMyFldList,&(pforeignKeyRec->localFldGrp))) < 0)
	{
		UnionUserErrLog("in UnionGenerateForeignKeyRecFromFldIDList:: UnionGenerateFldListFromFldIDList [%04d][%s]\n",lenOfMyFldList,myFldList);
		return(ret);
	}
	if (pforeignKeyRec->localFldGrp.fldNum == 0)
	{
		//UnionLog("in UnionGenerateForeignKeyRecFromFldIDList:: pforeignKeyRec->localFldGrp.fldNum = [%d] in [%s]\n",
		//		pforeignKeyRec->localFldGrp.fldNum,myFldList);
		return(0);
	}
	/*
	if (pforeignKeyRec->localFldGrp.fldNum == 0)
	{
		UnionUserErrLog("in UnionGenerateForeignKeyRecFromFldIDList:: localFldGrp.fldNum is error in  [%04d][%s] for [%s]\n",lenOfMyFldList,myFldList,foreignTBLName);
		return(errCodeObjectMDL_FieldNumberError);
	}
	*/
	// �ⲿ����
	if ((ret = UnionGenerateFldListFromFldIDList(foreignFldList,lenOfForeignFldList,&(pforeignKeyRec->foreignFldGrp))) < 0)
	{
		UnionUserErrLog("in UnionGenerateForeignKeyRecFromFldIDList:: UnionGenerateFldListFromFldIDList [%04d][%s]\n",lenOfForeignFldList,foreignFldList);
		return(ret);
	}
	if (pforeignKeyRec->foreignFldGrp.fldNum == 0)
	{
		//UnionLog("in UnionGenerateForeignKeyRecFromFldIDList:: pforeignKeyRec->foreignFldGrp.fldNum = [%d] in [%s]\n",
		//		pforeignKeyRec->foreignFldGrp.fldNum,myFldList);
		return(0);
	}
	/*
	if (pforeignKeyRec->foreignFldGrp.fldNum == 0)
	{
		UnionUserErrLog("in UnionGenerateForeignKeyRecFromFldIDList:: foreignFldGrp.fldNum is error in  [%04d][%s]\n",lenOfForeignFldList,foreignFldList);
		return(errCodeObjectMDL_FieldNumberError);
	}
	*/	
	if (pforeignKeyRec->foreignFldGrp.fldNum != pforeignKeyRec->localFldGrp.fldNum)
	{
		UnionUserErrLog("in UnionGenerateForeignKeyRecFromFldIDList:: foreignFldGrp.fldNum [%d] in [%04d][%s] != localFldGrp.fldNum [%d] is error in  [%04d][%s]\n",
			pforeignKeyRec->foreignFldGrp.fldNum,lenOfForeignFldList,foreignFldList,
			pforeignKeyRec->localFldGrp.fldNum,lenOfMyFldList,myFldList);
		return(errCodeObjectMDL_FieldNumberError);
	}
	return(pforeignKeyRec->foreignFldGrp.fldNum);
}

/*
����	
	���ݱ���(��tableList���ж���)������һ������ṹ
�������
	tblName		����
�������
	prec		����
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateObjectDefFromTBLDefInTableList(char *tblName,PUnionObject prec)
{
	int			ret;
	TUnionTableList		objDef;
	int			fldNum;
	char			fldNameGrp[100][128];
	char			version[128];
	
	if ((tblName == NULL) || (prec == NULL))
	{
		UnionUserErrLog("in UnionGenerateObjectDefFromTBLDefInTableList:: null pointer!\n");
		return(errCodeParameter);
	}
	
	memset(version,0,sizeof(version));
	//if ((ret = UnionReadFldFromCurrentMngSvrClientReqStr("version",version,sizeof(version))) < 0)
	{	
		// �������¼
		memset(&objDef,0,sizeof(objDef));
		if (strlen(tblName) >= sizeof(objDef.tableName))
		{
			UnionUserErrLog("in UnionGenerateObjectDefFromTBLDefInTableList:: tblName [%s] is too long!\n",tblName);
			return(errCodeParameter);
		}
		strcpy(objDef.tableName,tblName);
		if ((ret = UnionReadTableListRec(&objDef)) < 0)
		{
			UnionUserErrLog("in UnionGenerateObjectDefFromTBLDefInTableList:: UnionReadTableListRec [%s]!\n",tblName);
			/*		
			if ((ret = UnionGenerateObjectDefFromTBLDefInViewList(tblName,prec)) < 0)
			{
				UnionUserErrLog("in UnionGenerateObjectDefFromTBLDefInTableList:: UnionGenerateObjectDefFromTBLDefInViewList [%s]!\n",tblName);
				UnionUserErrLog("in UnionGenerateObjectDefFromTBLDefInTableList:: UnionReadTableListRec [%s]!\n",tblName);
				return(ret);
			}
			*/
			return(ret);
		}
	}
	// ���ݱ����¼���Ա�����и�ֵ
	// ����
	memset(prec,0,sizeof(prec));
	if (strlen(tblName) >= sizeof(prec->name))
	{
		UnionUserErrLog("in UnionGenerateObjectDefFromTBLDefInTableList:: tblName [%s] is too long!\n",tblName);
		return(errCodeParameter);
	}	
	strcpy(prec->name,tblName);
	// ����
	if ((prec->fldNum = UnionGenerateFldGrpDefFromFldIDList(objDef.fieldList,strlen(objDef.fieldList),prec->fldDefGrp,conMaxFldNumPerObject)) < 0)
	{
		UnionUserErrLog("in UnionGenerateObjectDefFromTBLDefInTableList:: UnionGenerateFldGrpDefFromFldIDList for table [%s]!\n",tblName);
		return(prec->fldNum);
	}
	// �ؼ���
	if ((ret = UnionGenerateFldListFromFldIDList(objDef.primaryKey,strlen(objDef.primaryKey),&(prec->primaryKey))) < 0)
	{
		UnionUserErrLog("in UnionGenerateObjectDefFromTBLDefInTableList:: UnionGenerateFldListFromFldIDList for table [%s]!\n",tblName);
		return(ret);
	}
	// Ψһֵ
	if ((ret = UnionGenerateFldListFromFldIDList(objDef.unique1,strlen(objDef.unique1),&(prec->uniqueFldGrp[prec->uniqueFldGrpNum]))) < 0)
	{
		UnionUserErrLog("in UnionGenerateObjectDefFromTBLDefInTableList:: UnionGenerateFldListFromFldIDList for table [%s]!\n",tblName);
		return(ret);
	}
	if (ret > 0)
		prec->uniqueFldGrpNum += 1;
	if ((ret = UnionGenerateFldListFromFldIDList(objDef.unique2,strlen(objDef.unique2),&(prec->uniqueFldGrp[prec->uniqueFldGrpNum]))) < 0)
	{
		UnionUserErrLog("in UnionGenerateObjectDefFromTBLDefInTableList:: UnionGenerateFldListFromFldIDList for table [%s]!\n",tblName);
		return(ret);
	}
	if (ret > 0)
		prec->uniqueFldGrpNum += 1;
	if ((ret = UnionGenerateFldListFromFldIDList(objDef.unique3,strlen(objDef.unique3),&(prec->uniqueFldGrp[prec->uniqueFldGrpNum]))) < 0)
	{
		UnionUserErrLog("in UnionGenerateObjectDefFromTBLDefInTableList:: UnionGenerateFldListFromFldIDList for table [%s]!\n",tblName);
		return(ret);
	}
	if (ret > 0)
		prec->uniqueFldGrpNum += 1;
	if ((ret = UnionGenerateFldListFromFldIDList(objDef.unique4,strlen(objDef.unique4),&(prec->uniqueFldGrp[prec->uniqueFldGrpNum]))) < 0)
	{
		UnionUserErrLog("in UnionGenerateObjectDefFromTBLDefInTableList:: UnionGenerateFldListFromFldIDList for table [%s]!\n",tblName);
		return(ret);
	}
	if (ret > 0)
		prec->foreignFldGrpNum += 1;

	// ���
	if ((ret = UnionGenerateForeignKeyRecFromFldIDList(objDef.foreignTable1,objDef.myKeyGrp1,strlen(objDef.myKeyGrp1),objDef.foreignKey1,strlen(objDef.foreignKey1),&(prec->foreignFldGrp[prec->foreignFldGrpNum]))) < 0)
	{
		UnionUserErrLog("in UnionGenerateObjectDefFromTBLDefInTableList:: UnionGenerateForeignKeyRecFromFldIDList for table [%s]!\n",tblName);
		return(ret);
	}
	if (ret > 0)
	{
		UnionLog("in UnionGenerateObjectDefFromTBLDefInTableList:: 1th foreign key ok!\n");
		prec->foreignFldGrpNum += 1;
	}
	if ((ret = UnionGenerateForeignKeyRecFromFldIDList(objDef.foreignTable2,objDef.myKeyGrp2,strlen(objDef.myKeyGrp2),objDef.foreignKey2,strlen(objDef.foreignKey2),&(prec->foreignFldGrp[prec->foreignFldGrpNum]))) < 0)
	{
		UnionUserErrLog("in UnionGenerateObjectDefFromTBLDefInTableList:: UnionGenerateForeignKeyRecFromFldIDList for table [%s]!\n",tblName);
		return(ret);
	}
	if (ret > 0)
	{
		UnionLog("in UnionGenerateObjectDefFromTBLDefInTableList:: 2th foreign key ok!\n");
		prec->foreignFldGrpNum += 1;
	}
	if ((ret = UnionGenerateForeignKeyRecFromFldIDList(objDef.foreignTable3,objDef.myKeyGrp3,strlen(objDef.myKeyGrp3),objDef.foreignKey3,strlen(objDef.foreignKey3),&(prec->foreignFldGrp[prec->foreignFldGrpNum]))) < 0)
	{
		UnionUserErrLog("in UnionGenerateObjectDefFromTBLDefInTableList:: UnionGenerateForeignKeyRecFromFldIDList for table [%s]!\n",tblName);
		return(ret);
	}
	if (ret > 0)
	{
		UnionLog("in UnionGenerateObjectDefFromTBLDefInTableList:: 3th foreign key ok!\n");
		prec->foreignFldGrpNum += 1;
	}
	if ((ret = UnionGenerateForeignKeyRecFromFldIDList(objDef.foreignTable4,objDef.myKeyGrp4,strlen(objDef.myKeyGrp4),objDef.foreignKey4,strlen(objDef.foreignKey4),&(prec->foreignFldGrp[prec->foreignFldGrpNum]))) < 0)
	{
		UnionUserErrLog("in UnionGenerateObjectDefFromTBLDefInTableList:: UnionGenerateForeignKeyRecFromFldIDList for table [%s]!\n",tblName);
		return(ret);
	}
	if (ret > 0)
	{
		UnionLog("in UnionGenerateObjectDefFromTBLDefInTableList:: 4th foreign key ok!\n");
		prec->foreignFldGrpNum += 1;
	}

	return(0);
}

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
int UnionGenerateObjectDefFromTBLDefInViewList(char *tblName,char *mainMenuName,PUnionObject prec)
{
	int			ret;
	TUnionViewList		viewDef;
	int			fldNum;
	char			fldNameGrp[100][128];
	
	if ((tblName == NULL) || (prec == NULL))
	{
		UnionUserErrLog("in UnionGenerateObjectDefFromTBLDefInViewList:: null pointer!\n");
		return(errCodeParameter);
	}
		
	// �������¼
	memset(&viewDef,0,sizeof(viewDef));
	if (strlen(tblName) >= sizeof(viewDef.ID))
	{
		UnionUserErrLog("in UnionGenerateObjectDefFromTBLDefInViewList:: tblName [%s] is too long!\n",tblName);
		return(errCodeParameter);
	}
	if ((ret = UnionReadViewListRec(tblName,mainMenuName,&viewDef)) < 0)
	{
		UnionUserErrLog("in UnionGenerateObjectDefFromTBLDefInViewList:: UnionReadViewListRec [%s]!\n",tblName);
		return(ret);
	}

	// ���ݱ����¼���Ա�����и�ֵ
	// ����
	memset(prec,0,sizeof(prec));
	if (strlen(tblName) >= sizeof(prec->name))
	{
		UnionUserErrLog("in UnionGenerateObjectDefFromTBLDefInViewList:: tblName [%s] is too long!\n",tblName);
		return(errCodeParameter);
	}
	strcpy(prec->name,tblName);
	// ����
	if ((prec->fldNum = UnionGenerateFldGrpDefFromFldIDList(viewDef.fieldList,strlen(viewDef.fieldList),prec->fldDefGrp,conMaxFldNumPerObject)) < 0)
	{
		UnionUserErrLog("in UnionGenerateObjectDefFromTBLDefInViewList:: UnionGenerateFldGrpDefFromFldIDList for table [%s]!\n",tblName);
		return(prec->fldNum);
	}
	UnionLog("in UnionGenerateObjectDefFromTBLDefInViewList:: fldNum = [%02d] in [%s]\n",prec->fldNum,viewDef.fieldList);
	// �ؼ���
	if ((ret = UnionGenerateFldListFromFldIDList(viewDef.primaryKey,strlen(viewDef.primaryKey),&(prec->primaryKey))) < 0)
	{
		UnionUserErrLog("in UnionGenerateObjectDefFromTBLDefInViewList:: UnionGenerateFldListFromFldIDList for table [%s]!\n",tblName);
		return(ret);
	}
	UnionLog("in UnionGenerateObjectDefFromTBLDefInViewList:: fldNum = [%02d] in [%s]\n",prec->primaryKey.fldNum,viewDef.primaryKey);
	return(0);
}

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
int UnionCreateObjectDefFromTBLDefInTableList(char *tblName,int overwriteMode)
{
	int		ret;
	TUnionObject	rec;
	
	if ((strcmp(tblName,"tableList") == 0) || (strcmp(tblName,"tableField") == 0))
	{
		UnionUserErrLog("in UnionCreateObjectDefFromTBLDefInTableList:: table [%s] is reserved!\n",tblName);
		return(errCodeTableNameReserved);
	}
	
	if (UnionExistsObjectDef(tblName))
	{
		switch (overwriteMode)
		{
			case	conOverwriteModeWhenFileExistsOverwrite:
				break;
			case	conOverwriteModeWhenFileExistsReturn:
				UnionUserErrLog("in UnionCreateObjectDefFromTBLDefInTableList:: %s already exists, not generated!\n",tblName);
				return(errCodeFileAlreadyExists);
			default:
				if (!UnionConfirm("object %s already exists, overwrite it",tblName))
					return(errCodeFileAlreadyExists);
				break;
		}
		if ((ret = UnionDropObjectDef(tblName)) < 0)
		{
			UnionUserErrLog("in UnionCreateObjectDefFromTBLDefInTableList:: UnionDropObjectDef for table [%s]!\n",tblName);
			return(ret);
		}
	}
	memset(&rec,0,sizeof(rec));
	if ((ret = UnionGenerateObjectDefFromTBLDefInTableList(tblName,&rec)) < 0)
	{
		UnionUserErrLog("in UnionCreateObjectDefFromTBLDefInTableList:: UnionGenerateObjectDefFromTBLDefInTableList for table [%s]!\n",tblName);
		return(ret);
	}
	if ((ret = UnionStoreObjectDefAnyway(&rec)) < 0)
	{
		UnionUserErrLog("in UnionCreateObjectDefFromTBLDefInTableList:: UnionStoreObjectDefAnyway for table [%s]!\n",tblName);
		return(ret);
	}
	return(ret);
}

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
int UnionCreateComplexDBTBLCreateSQLFileFromTBLDefInTableList(char *tblName)
{
	int		ret;
	TUnionObject	rec;
	
	if ((strcmp(tblName,"tableList") == 0) || (strcmp(tblName,"tableField") == 0))
	{
		UnionUserErrLog("in UnionCreateComplexDBTBLCreateSQLFileFromTBLDefInTableList:: table [%s] is reserved!\n",tblName);
		return(errCodeTableNameReserved);
	}
	
	memset(&rec,0,sizeof(rec));
	if ((ret = UnionGenerateObjectDefFromTBLDefInTableList(tblName,&rec)) < 0)
	{
		UnionUserErrLog("in UnionCreateComplexDBTBLCreateSQLFileFromTBLDefInTableList:: UnionGenerateObjectDefFromTBLDefInTableList for table [%s]!\n",tblName);
		return(ret);
	}
	if ((ret = UnionCreateComplexDBTBLCreateSqlFile(&rec)) < 0)
	{
		UnionUserErrLog("in UnionCreateComplexDBTBLCreateSQLFileFromTBLDefInTableList:: UnionCreateComplexDBTBLCreateSqlFile for table [%s]!\n",tblName);
		return(ret);
	}
	return(ret);
}


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
int UnionCreateAllObjectDefFromTBLDefInTableList(int modeWhenFileExists)
{
	int			ret;
	char			fileName[512+1];
	int			recNum = 0;
	FILE			*recFileFp = NULL;
	char			recStr[8192*2+1];
	int			lenOfRecStr;
	char			tableName[128];

	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/pid-%d.tmp",getenv("UNIONTEMP"),getpid());

	// �����м�¼
	if ((ret = UnionBatchSelectObjectRecord(conDataTBLListTBLName,"",fileName)) < 0)
	{
		UnionUserErrLog("in UnionCreateAllObjectDefFromTBLDefInTableList:: UnionBatchSelectObjectRecord [%s]!\n",conDataTBLListTBLName);
		return(ret);
	}
	
	// ���ļ�
	if ((recFileFp = fopen(fileName,"r")) == NULL)
	{
		UnionUserErrLog("in UnionCreateAllObjectDefFromTBLDefInTableList:: fopen [%s]\n",fileName);
		goto errExit;
	}
	while (!feof(recFileFp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(recFileFp,recStr,sizeof(recStr))) <= 0)
			continue;
		memset(tableName,0,sizeof(tableName));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"tableName",tableName,sizeof(tableName))) < 0)
		{
			UnionUserErrLog("in UnionCreateAllObjectDefFromTBLDefInTableList:: UnionReadRecFldFromRecStr [%s] from [%s]\n","tableName",recStr);
			goto errExit;
		}
		if ((ret = UnionCreateObjectDefFromTBLDefInTableList(tableName,modeWhenFileExists)) < 0)
		{
			UnionUserErrLog("in UnionCreateAllObjectDefFromTBLDefInTableList:: UnionCreateObjectDefFromTBLDefInTableList!\n");
			continue;
		}
		recNum++;
	}
	ret = recNum;
errExit:
	fclose(recFileFp);
	UnionDeleteFile(fileName);
	return(ret);
}

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
int UnionGenerateOperationInfoFromAuthDefTBL(FILE *fp)
{
	int			ret;
	char			fileName[512+1];
	int			recNum = 0;
	FILE			*recFileFp = NULL;
	char			recStr[8192*2+1];
	int			lenOfRecStr;
	char			resID[128+1];
	char			remark[256+1];
	char			serviceID[128+1];
	char			operList[128+1];
	char			listOfOperatorLevel[128+1];
	FILE			*outFp = stdout;
	
	if (fp != NULL)
		outFp = fp;
		
	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/pid-%d.tmp",getenv("UNIONTEMP"),getpid());

	// �����м�¼
	if ((ret = UnionBatchSelectObjectRecord("operationAuthorization","",fileName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateOperationInfoFromAuthDefTBL:: UnionBatchSelectObjectRecord [%s]!\n","operationAuthorization");
		return(ret);
	}
	
	// ���ļ�
	if ((recFileFp = fopen(fileName,"r")) == NULL)
	{
		UnionUserErrLog("in UnionGenerateOperationInfoFromAuthDefTBL:: fopen [%s]\n",fileName);
		goto errExit;
	}
	while (!feof(recFileFp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(recFileFp,recStr,sizeof(recStr))) <= 0)
			continue;
		memset(resID,0,sizeof(resID));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"resID",resID,sizeof(resID))) < 0)
		{
			UnionUserErrLog("in UnionGenerateOperationInfoFromAuthDefTBL:: UnionReadRecFldFromRecStr [%s] from [%s]\n","resID",recStr);
			goto errExit;
		}
		memset(serviceID,0,sizeof(serviceID));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"serviceID",serviceID,sizeof(serviceID))) < 0)
		{
			UnionUserErrLog("in UnionGenerateOperationInfoFromAuthDefTBL:: UnionReadRecFldFromRecStr [%s] from [%s]\n","serviceID",recStr);
			goto errExit;
		}
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"remark",remark,sizeof(remark))) < 0)
		{
			UnionUserErrLog("in UnionGenerateOperationInfoFromAuthDefTBL:: UnionReadRecFldFromRecStr [%s] from [%s]\n","remark",recStr);
			goto errExit;
		}
		memset(listOfOperatorLevel,0,sizeof(listOfOperatorLevel));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"listOfOperatorLevel",listOfOperatorLevel,sizeof(listOfOperatorLevel))) < 0)
		{
			UnionUserErrLog("in UnionGenerateOperationInfoFromAuthDefTBL:: UnionReadRecFldFromRecStr [%s] from [%s]\n","listOfOperatorLevel",recStr);
			goto errExit;
		}
		memset(operList,0,sizeof(operList));
		UnionFilterAllSpecChars(listOfOperatorLevel,strlen(listOfOperatorLevel),',',operList,sizeof(operList));
		fprintf(outFp,"operation=%03d.%03d=%s	%s\n",atoi(resID),atoi(serviceID),operList,remark);
		recNum++;
	}
	ret = recNum;
errExit:
	fclose(recFileFp);
	UnionDeleteFile(fileName);
	return(ret);
}

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
int UnionGenerateOperationInfoFromAuthDefTBLToSpecFile(char *fileName)
{
	int		ret;
	FILE 		*fp = stdout;
	
	if ((fileName != NULL) && (strlen(fileName) != 0))
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionUserErrLog("in UnionGenerateOperationInfoFromAuthDefTBLToSpecFile:: fopen [%s]\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	if ((ret = UnionGenerateOperationInfoFromAuthDefTBL(fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateOperationInfoFromAuthDefTBLToSpecFile:: UnionGenerateOperationInfoFromAuthDefTBL!\n");
		//return(ret);
	}
	if (fp != stdout)
		fclose(fp);
	return(ret);
}

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
int UnionCreateAllComplexDBTBLCreateSQLFileFromTBLDefInTableList()
{
	int			ret;
	char			fileName[512+1];
	int			recNum = 0;
	FILE			*recFileFp = NULL;
	char			recStr[8192*2+1];
	int			lenOfRecStr;
	char			tableName[128+1];

	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/pid-%d.tmp",getenv("UNIONTEMP"),getpid());

	// �����м�¼
	if ((ret = UnionBatchSelectObjectRecord(conDataTBLListTBLName,"",fileName)) < 0)
	{
		UnionUserErrLog("in UnionCreateAllComplexDBTBLCreateSQLFileFromTBLDefInTableList:: UnionBatchSelectObjectRecord [%s]!\n",conDataTBLListTBLName);
		return(ret);
	}
	
	// ���ļ�
	if ((recFileFp = fopen(fileName,"r")) == NULL)
	{
		UnionUserErrLog("in UnionCreateAllComplexDBTBLCreateSQLFileFromTBLDefInTableList:: fopen [%s]\n",fileName);
		goto errExit;
	}
	while (!feof(recFileFp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(recFileFp,recStr,sizeof(recStr))) <= 0)
			continue;
		memset(tableName,0,sizeof(tableName));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"tableName",tableName,sizeof(tableName))) < 0)
		{
			UnionUserErrLog("in UnionCreateAllComplexDBTBLCreateSQLFileFromTBLDefInTableList:: UnionReadRecFldFromRecStr [%s] from [%s]\n","tableName",recStr);
			goto errExit;
		}
		if ((ret = UnionCreateComplexDBTBLCreateSQLFileFromTBLDefInTableList(tableName)) < 0)
		{
			UnionUserErrLog("in UnionCreateAllComplexDBTBLCreateSQLFileFromTBLDefInTableList:: UnionCreateComplexDBTBLCreateSQLFileFromTBLDefInTableList!\n");
			continue;
		}
		recNum++;
	}
	ret = recNum;
errExit:
	fclose(recFileFp);
	UnionDeleteFile(fileName);
	return(ret);
}

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
int UnionCreateObjectDefSQLFromTBLDefInTableList(char *tblName,FILE *fp)
{
	int		ret;
	TUnionObject	rec;
	
	memset(&rec,0,sizeof(rec));
	if ((ret = UnionGenerateObjectDefFromTBLDefInTableList(tblName,&rec)) < 0)
	{
		UnionUserErrLog("in UnionCreateObjectDefSQLFromTBLDefInTableList:: UnionGenerateObjectDefFromTBLDefInTableList for table [%s]!\n",tblName);
		return(ret);
	}
	if ((ret = UnionCreateSQLFromObjectDefToFp(&rec,fp)) < 0)
	{
		UnionUserErrLog("in UnionCreateObjectDefSQLFromTBLDefInTableList:: UnionGenerateObjectDefFromTBLDefInTableList for table [%s]!\n",tblName);
		return(ret);
	}
	return(ret);
}

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
int UnionCreateObjectDefSQLFromTBLDefInTableListToSpecFile(char *tblName,char *fileName)
{
	int		ret;
	FILE 		*fp = stdout;
	
	if ((fileName != NULL) && (strlen(fileName) != 0))
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionUserErrLog("in UnionCreateObjectDefSQLFromTBLDefInTableListToSpecFile:: fopen [%s] for table [%s]!\n",fileName,tblName);
			return(errCodeUseOSErrCode);
		}
	}
	if ((ret = UnionCreateObjectDefSQLFromTBLDefInTableList(tblName,fp)) < 0)
	{
		UnionUserErrLog("in UnionCreateObjectDefSQLFromTBLDefInTableListToSpecFile:: UnionCreateObjectDefSQLFromTBLDefInTableList for table [%s]!\n",tblName);
		//return(ret);
	}
	if (fp != stdout)
		fclose(fp);
	return(ret);
}

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
int UnionCreateAllObjectDefSQLFromTBLDefInTableList(FILE *fp)
{
	int			ret;
	char			fileName[512+1];
	int			recNum = 0;
	FILE			*recFileFp = NULL;
	char			recStr[8192*2+1];
	int			lenOfRecStr;
	char			tableName[128+1];

	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/pid-%d.tmp",getenv("UNIONTEMP"),getpid());

	// �����м�¼
	if ((ret = UnionBatchSelectObjectRecord(conDataTBLListTBLName,"",fileName)) < 0)
	{
		UnionUserErrLog("in UnionCreateAllObjectDefSQLFromTBLDefInTableList:: UnionBatchSelectObjectRecord [%s]!\n",conDataTBLListTBLName);
		return(ret);
	}
	
	// ���ļ�
	if ((recFileFp = fopen(fileName,"r")) == NULL)
	{
		UnionUserErrLog("in UnionCreateAllObjectDefSQLFromTBLDefInTableList:: fopen [%s]\n",fileName);
		goto errExit;
	}
	while (!feof(recFileFp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(recFileFp,recStr,sizeof(recStr))) <= 0)
			continue;
		memset(tableName,0,sizeof(tableName));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"tableName",tableName,sizeof(tableName))) < 0)
		{
			UnionUserErrLog("in UnionCreateAllObjectDefSQLFromTBLDefInTableList:: UnionReadRecFldFromRecStr [%s] from [%s]\n","tableName",recStr);
			goto errExit;
		}
		if ((ret = UnionCreateObjectDefSQLFromTBLDefInTableList(tableName,fp)) < 0)
		{
			UnionUserErrLog("in UnionCreateAllObjectDefSQLFromTBLDefInTableList:: UnionCreateObjectDefSQLFromTBLDefInTableList!\n");
			continue;
		}
		recNum++;
	}
	ret = recNum;
errExit:
	fclose(recFileFp);
	UnionDeleteFile(fileName);
	return(ret);
}

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
int UnionCreateAllObjectDefSQLFromTBLDefInTableListToSpecFile(char *fileName)
{
	int		ret;
	FILE 		*fp = stdout;
	
	if ((fileName != NULL) && (strlen(fileName) != 0))
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionUserErrLog("in UnionCreateAllObjectDefSQLFromTBLDefInTableListToSpecFile:: fopen [%s]!\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	if ((ret = UnionCreateAllObjectDefSQLFromTBLDefInTableList(fp)) < 0)
	{
		UnionUserErrLog("in UnionCreateAllObjectDefSQLFromTBLDefInTableListToSpecFile:: UnionCreateAllObjectDefSQLFromTBLDefInTableList!\n");
		//return(ret);
	}
	if (fp != stdout)
		fclose(fp);
	return(ret);
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
	int			fldNum = 0,fldIndex;
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
int UnionGenerateTBLQueryConfFromTBLDefInTableListToFp(char *tblName,FILE *fp)
{
	int			ret;
	FILE			*outFp = stdout;
	TUnionTableList		objDef;
	int			fldNum;
	char			fldNameGrp[100][128];
	TUnionTBLQueryConf	queryConf;
	char			tmpBuf[100];
	
	if (tblName == NULL)
	{
		UnionUserErrLog("in UnionGenerateTBLQueryConfFromTBLDefInTableListToFp:: null pointer!\n");
		return(errCodeParameter);
	}
	if (fp != NULL)
		outFp = fp;
		
	// �������¼
	memset(&objDef,0,sizeof(objDef));
	if (strlen(tblName) >= sizeof(objDef.tableName))
	{
		UnionUserErrLog("in UnionGenerateTBLQueryConfFromTBLDefInTableListToFp:: tblName [%s] is too long!\n",tblName);
		return(errCodeParameter);
	}
	strcpy(objDef.tableName,tblName);
	if ((ret = UnionReadTableListRec(&objDef)) < 0)
	{
		UnionUserErrLog("in UnionGenerateTBLQueryConfFromTBLDefInTableListToFp:: UnionReadTableListRec tblName [%s]!\n",tblName);
		return(ret);
	}

	// ���ݱ����¼���Ա�����и�ֵ
	memset(&queryConf,0,sizeof(queryConf));
	strcpy(queryConf.name,objDef.tableName);
	strcpy(queryConf.alais,objDef.tableAlais);
	if (strlen(queryConf.alais) == 0)
		strcpy(queryConf.alais,queryConf.name);
	if (objDef.resID < 1000)
		queryConf.id = objDef.resID;
	else
	{
		sprintf(tmpBuf,"%d",objDef.resID);
		tmpBuf[3] = 0;
		queryConf.id = atoi(tmpBuf);
	}	
	sprintf(queryConf.formIconFileName,"%s����ͼ��",queryConf.alais);
	sprintf(queryConf.formPopMenuDefFileName,"%s�����Ҽ��˵�",queryConf.alais);
	sprintf(queryConf.recIconFileName,"%s��¼ͼ��",queryConf.alais);
	sprintf(queryConf.recPopMenuDefFileName,"%s��¼�Ҽ��˵�",queryConf.alais);
	queryConf.maxNumOfRecPerPage = 32;
	// ��ѯ����
	if ((ret = UnionGenerateQueryFldGrpDefFromFldIDList(objDef.fieldList,strlen(objDef.fieldList),&(queryConf.queryFldGrp))) < 0)
	{
		UnionUserErrLog("in UnionGenerateTBLQueryConfFromTBLDefInTableListToFp:: UnionGenerateQueryFldGrpDefFromFldIDList for table [%s]!\n",tblName);
		return(ret);
	}
	// �ؼ���
	if ((ret = UnionGenerateQueryFldListFromFldIDList(objDef.primaryKey,strlen(objDef.primaryKey),&(queryConf.primaryKeyFldGrp))) < 0)
	{
		UnionUserErrLog("in UnionGenerateTBLQueryConfFromTBLDefInTableListToFp:: UnionGenerateQueryFldListFromFldIDList for table [%s]!\n",tblName);
		return(ret);
	}
	// ������д�뵽�ļ���
	if ((ret = UnionPrintTBLQueryConfToFp(&queryConf,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateTBLQueryConfFromTBLDefInTableListToFp:: UnionPrintTBLQueryConfToFp for table [%s]!\n",tblName);
		return(ret);
	}
	// ��ѯ����
	if ((ret = UnionGenerateAssignmentQueryFldListFromFldIDList(objDef.fieldList,strlen(objDef.fieldList),"",0,
		"",0,"",0,"",0,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateTBLQueryConfFromTBLDefInTableListToFp:: UnionGenerateAssignmentQueryFldListFromFldIDList for table [%s]!\n",tblName);
		return(ret);
	}
	return(0);
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
int UnionGenerateTBLQueryConfFromTBLDefInTableListToSpecFile(char *tblName,char *fileName)
{
	int		ret;
	FILE 		*fp = stdout;
	
	if ((fileName != NULL) && (strlen(fileName) != 0))
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionUserErrLog("in UnionGenerateTBLQueryConfFromTBLDefInTableListToSpecFile:: fopen [%s] for table [%s]!\n",fileName,tblName);
			return(errCodeUseOSErrCode);
		}
	}
	if ((ret = UnionGenerateTBLQueryConfFromTBLDefInTableListToFp(tblName,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateTBLQueryConfFromTBLDefInTableListToSpecFile:: UnionGenerateTBLQueryConfFromTBLDefInTableListToFp for table [%s]!\n",tblName);
		//return(ret);
	}
	if (fp != stdout)
		fclose(fp);
	return(ret);
}

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
int UnionGenerateQueryFldRefFromFldIDList(char *fldList,int lenOfFldList,char *fldRef)
{
	int			ret;
	int			fldNum,fldIndex;
	char			fldNameGrp[100][128];
	int			offset = 0;
	char			fieldName[128];
	
	if ((fldList == NULL) || (lenOfFldList < 0) || (fldRef == NULL))
		return(errCodeParameter);

	// ������嵥
	memset(fldNameGrp,0,sizeof(fldNameGrp));
	if ((fldNum = UnionSeprateVarStrIntoVarGrp(fldList,lenOfFldList,conSeparableSignOfFieldNameInFldList,fldNameGrp,100)) < 0)
	{
		UnionUserErrLog("in UnionGenerateQueryFldRefFromFldIDList:: UnionSeprateVarStrIntoVarGrp [%04d][%s]\n",lenOfFldList,fldList);
		return(fldNum);
	}
	
	// ��ȡ����
	for (fldIndex = 0; fldIndex < fldNum; fldIndex++)
	{
		if (fldIndex >= 1)
		{
			sprintf(fldRef+offset,",");
			offset++;
		}
		memset(fieldName,0,sizeof(fieldName));
		if ((ret = UnionReadFieldNameByFieldIDInTableField(fldNameGrp[fldIndex],fieldName)) < 0)
		{
			UnionUserErrLog("in UnionGenerateQueryFldRefFromFldIDList:: UnionReadFieldNameByFieldIDInTableField [%04d][%s]\n",lenOfFldList,fldList);
			return(fldNum);
		}	
		sprintf(fldRef+offset,"%s=this.%s",fieldName,fldNameGrp[fldIndex]);
		offset += (strlen("this.")+strlen(fldNameGrp[fldIndex])+strlen(fieldName)+1);
	}
	return(fldNum);
}

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
int UnionReadFieldNameByFieldIDInTableField(char *fldID,char *fldName)
{
	TUnionTableField	rec;
	int			ret;
	
	memset(&rec,0,sizeof(rec));
	strcpy(rec.ID,fldID);
	if ((ret = UnionReadTableFieldRec(&rec)) < 0)
	{
		UnionUserErrLog("in UnionReadFieldNameByFieldIDInTableField:: UnionReadTableFieldRec of [%s]\n",fldID);
		return(ret);
	}
	strcpy(fldName,rec.fieldName);
	return(0);
}


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
int UnionGenerateRecDoubleClickMenuFromTBLDefInTableListToFp(char *tellerTypeID,char *tblName,FILE *fp)
{
	return(UnionGenerateInterfaceMenuFromTBLDefInTableListToFp(tellerTypeID,tblName,conMenuItemTypeDoubleClickMenu,fp));
}

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
int UnionGenerateRecDoubleClickMenuFromTBLDefInTableListToSpecFile(char *tellerTypeID,char *tblName,char *fileName)
{
	int		ret;
	FILE 		*fp = stdout;
	
	if ((fileName != NULL) && (strlen(fileName) != 0))
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionUserErrLog("in UnionGenerateRecDoubleClickMenuFromTBLDefInTableListToSpecFile:: fopen [%s]!\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	if ((ret = UnionGenerateRecDoubleClickMenuFromTBLDefInTableListToFp(tellerTypeID,tblName,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateRecDoubleClickMenuFromTBLDefInTableListToSpecFile:: UnionGenerateRecDoubleClickMenuFromTBLDefInTableListToFp!\n");
		//return(ret);
	}
	if (fp != stdout)
		fclose(fp);
	return(ret);
}

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
int UnionGenerateRecPopupMenuFromTBLDefInTableListToFp(char *tellerTypeID,char *tblName,FILE *fp)
{
	return(UnionGenerateInterfaceMenuFromTBLDefInTableListToFp(tellerTypeID,tblName,conMenuItemTypeRecPopMenu,fp));
}

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
int UnionGenerateRecPopupMenuFromTBLDefInTableListToSpecFile(char *tellerTypeID,char *tblName,char *fileName)
{
	int		ret;
	FILE 		*fp = stdout;
	
	if ((fileName != NULL) && (strlen(fileName) != 0))
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionUserErrLog("in UnionGenerateRecPopupMenuFromTBLDefInTableListToSpecFile:: fopen [%s]!\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	if ((ret = UnionGenerateRecPopupMenuFromTBLDefInTableListToFp(tellerTypeID,tblName,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateRecPopupMenuFromTBLDefInTableListToSpecFile:: UnionGenerateRecPopupMenuFromTBLDefInTableListToFp!\n");
		//return(ret);
	}
	if (fp != stdout)
		fclose(fp);
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
int UnionGenerateInterfaceMenuFromTBLDefInTableListToFp(char *tellerTypeID,char *tblName,int menuType,FILE *fp)
{
	UnionUserErrLog("in UnionGenerateInterfaceMenuFromTBLDefInTableListToFp:: invalid service!\n");
	return(errCodeParameter);
}

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
int UnionGenerateInterfaceMenuFromViewDefToFp(char *tellerTypeID,char *viewName,char *mainMenuName,int menuType,FILE *fp)
{
	int			ret;
	TUnionViewList		rec;
	char			menuName[128];
	
	// ����ͼ����
	memset(&rec,0,sizeof(rec));
	if ((ret = UnionReadViewListRec(viewName,mainMenuName,&rec)) < 0)
	{
		UnionUserErrLog("in UnionGenerateInterfaceMenuFromViewDefToFp:: UnionReadViewListRec [%s]!\n",viewName);
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
			UnionUserErrLog("in UnionGenerateInterfaceMenuFromViewDefToFp:: invalid menutype [%d]\n",menuType);
			return(errCodeParameter);
	}
	return(UnionGenerateMenuDefFileFromMenuNameToFp(tellerTypeID,rec.mainMenuName,menuName,fp));
}

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
int UnionGenerateFormPopMenuFromViewDefToSpecFile(char *tellerTypeID,char *viewName,char *mainMenuName,char *fileName)
{
	return(UnionGenerateInterfaceMenuFromViewDefToSpecFile(tellerTypeID,viewName,mainMenuName,conMenuItemTypeFormPopMenu,fileName));
}

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
int UnionGenerateRecPopMenuFromViewDefToSpecFile(char *tellerTypeID,char *viewName,char *mainMenuName,char *fileName)
{
	return(UnionGenerateInterfaceMenuFromViewDefToSpecFile(tellerTypeID,viewName,mainMenuName,conMenuItemTypeRecPopMenu,fileName));
}

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
int UnionGenerateRecDoubleClickMenuFromViewDefToSpecFile(char *tellerTypeID,char *viewName,char *mainMenuName,char *fileName)
{
	return(UnionGenerateInterfaceMenuFromViewDefToSpecFile(tellerTypeID,viewName,mainMenuName,conMenuItemTypeDoubleClickMenu,fileName));
}

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
int UnionGenerateInterfaceMenuFromViewDefToSpecFile(char *tellerTypeID,char *viewName,char *mainMenuName,int menuType,char *fileName)
{
	int		ret;
	FILE 		*fp = stdout;
	
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionUserErrLog("in UnionGenerateInterfaceMenuFromViewDefToSpecFile:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	
	if ((ret = UnionGenerateInterfaceMenuFromViewDefToFp(tellerTypeID,viewName,mainMenuName,menuType,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateInterfaceMenuFromViewDefToSpecFile:: viewName!\n");
		//return(ret);
	}
	fclose(fp);
	return(ret);
}


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
int UnionGenerateFormPopupMenuFromTBLDefInTableListToFp(char *tellerTypeID,char *tblName,FILE *fp)
{
	return(UnionGenerateInterfaceMenuFromTBLDefInTableListToFp(tellerTypeID,tblName,conMenuItemTypeFormPopMenu,fp));
}

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
int UnionGenerateFormPopupMenuFromTBLDefInTableListToSpecFile(char *tellerTypeID,char *tblName,char *fileName)
{
	int		ret;
	FILE 		*fp = stdout;
	
	if ((fileName != NULL) && (strlen(fileName) != 0))
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionUserErrLog("in UnionGenerateFormPopupMenuFromTBLDefInTableListToSpecFile:: fopen [%s]!\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	if ((ret = UnionGenerateFormPopupMenuFromTBLDefInTableListToFp(tellerTypeID,tblName,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateFormPopupMenuFromTBLDefInTableListToSpecFile:: UnionGenerateFormPopupMenuFromTBLDefInTableListToFp!\n");
		//return(ret);
	}
	if (fp != stdout)
		fclose(fp);
	return(ret);
}
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
int UnionCreateObjectFromTBLDefInTableList(char *tblName)
{
	int		ret;
	TUnionObject	rec;
	
	if ((strcmp(tblName,"tableList") == 0) || (strcmp(tblName,"tableField") == 0))
	{
		UnionUserErrLog("in UnionCreateObjectFromTBLDefInTableList:: table [%s] is reserved!\n",tblName);
		return(errCodeTableNameReserved);
	}
	
	memset(&rec,0,sizeof(rec));
	if ((ret = UnionGenerateObjectDefFromTBLDefInTableList(tblName,&rec)) < 0)
	{
		UnionUserErrLog("in UnionCreateObjectFromTBLDefInTableList:: UnionGenerateObjectDefFromTBLDefInTableList for table [%s]!\n",tblName);
		return(ret);
	}
	if ((ret = UnionCreateObjectAnyway(&rec)) < 0)
	{
		UnionUserErrLog("in UnionCreateObjectFromTBLDefInTableList:: UnionCreateObjectAnyway for table [%s]!\n",tblName);
		return(ret);
	}
	return(ret);
}

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
int UnionConvertOperatorLevelListFromIDToLevel(char *oriList,int lenOfOriList,char *desList,int sizeOfBuf)
{
	int		ret;
	char		fldGrp[20][128];
	int		fldNum,fldIndex;
	int		len;
	char		level[128];
	int		offset = 0;
	
	if (lenOfOriList == 0)
		return(0);
		
	if ((fldNum = UnionSeprateVarStrIntoVarGrp(oriList,lenOfOriList,conSeparableSignOfFieldNameInFldList,fldGrp,20)) < 0)
	{
		UnionUserErrLog("in UnionConvertOperatorLevelListFromIDToLevel:: UnionSeprateVarStrIntoVarGrp [%s]\n",oriList);
		return(fldNum);
	}
	for (fldIndex = 0; fldIndex < fldNum; fldIndex++)
	{
		/*
		if (fldIndex > 0)
		{
			sprintf(desList+offset,"%c",conSeparableSignOfFieldNameInFldList);
			offset++;
		}
		*/
		if ((len = strlen(fldGrp[fldIndex])) == 1)
		{
			sprintf(desList+offset,"%s",fldGrp[fldIndex]);
			offset += len;
			continue;
		}
		if ((len = UnionReadOperatorTypeRecFld(fldGrp[fldIndex],conOperatorTypeFldNameMacroValue,level,sizeof(level))) < 0)
		{
			UnionUserErrLog("in UnionConvertOperatorLevelListFromIDToLevel:: UnionReadOperatorTypeRecFld [%s]\n",fldGrp[fldIndex]);
			return(len);
		}
		sprintf(desList+offset,"%c",level[0]);
		offset++;
	}
	desList[offset] = 0;
	return(offset);
}

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
int UnionGenerateNullTableMenuItemGrp(char *tableName,int num)
{
	TUnionMenuItemDef	menuItem;
	int			ret;
	int			index;
	
	for (index = 0; index < num; index++)
	{
		// ��������
		sprintf(menuItem.menuName,"�ղ���");
		// �˵�����ʾ����
		sprintf(menuItem.menuDisplayname,"�ղ���%d",index);
		// ������
		strcpy(menuItem.menuGrpName,tableName);
		if ((ret = UnionInsertMenuItemDefRec(&menuItem)) < 0)
		{
			UnionProgramerLog("in UnionGenerateOperationFromTBLDefInTableList:: UnionInsertMenuItemDefRec [%s] for menuGrp [%s]!\n",menuItem.menuName,menuItem.menuGrpName,tableName);
		}
		/*
		if ((ret = UnionAppendMenuItemToSpecTBLName(tableName,menuItem.menuName,menuItem.tableMenuType)) < 0)
		{
			UnionProgramerLog("in UnionGenerateOperationFromTBLDefInTableList:: UnionAppendMenuItemToSpecTBLName [%s] for table [%s]!\n",menuItem.menuName,tableName);
			continue;
		}
		*/
	}
	return(num);
}

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
int UnionGenerateOperationFromTBLDefInTableList(char *tblName)
{
	int			ret;
	TUnionTableList		objDef;
	int			fldNum;
	char			primaryKeyStr[512+1];
	char			fldNameGrp[100][128];
	TUnionOperationAuthorization	rec;
	TUnionMenuItemOperationDef		menuItem;
	//TUnionMenuItemOperationDef		menuItemOperation;
	int			index;
	int			recNum = 0;
	char			condition[128];
	
	if (tblName == NULL)
	{
		UnionUserErrLog("in UnionGenerateOperationFromTBLDefInTableList:: null pointer!\n");
		return(errCodeParameter);
	}
		
	// ���������
	memset(&objDef,0,sizeof(objDef));
	if (strlen(tblName) >= sizeof(objDef.tableName))
	{
		UnionUserErrLog("in UnionGenerateOperationFromTBLDefInTableList:: tblName [%s] is too long!\n",tblName);
		return(errCodeParameter);
	}
	strcpy(objDef.tableName,tblName);
	if ((ret = UnionReadTableListRec(&objDef)) < 0)
	{
		UnionUserErrLog("in UnionGenerateOperationFromTBLDefInTableList:: UnionReadTableListRec [%s]!\n",tblName);
		return(ret);
	}
	// ������嵥
	memset(fldNameGrp,0,sizeof(fldNameGrp));
	if ((fldNum = UnionSeprateVarStrIntoVarGrp(objDef.operationList,strlen(objDef.operationList),conSeparableSignOfFieldNameInFldList,fldNameGrp,100)) < 0)
	{
		UnionUserErrLog("in UnionGenerateOperationFromTBLDefInTableList:: UnionSeprateVarStrIntoVarGrp [%s]\n",objDef.operationList);
		return(fldNum);
	}
	// ɾ��ԭ�ȶ�������в���
	sprintf(condition,"%s=%d",conOperationAuthorizationFldNameResID,objDef.resID);
	//UnionBatchDeleteObjectRecord(conTBLNameOperationAuthorization,condition);
	for (index = 0; index < fldNum; index++)
	{
		if ((ret = UnionFormDefaultValueOfOperationAuthorizationRec(&rec)) < 0)
		{
			UnionUserErrLog("in UnionGenerateOperationFromTBLDefInTableList:: UnionFormDefaultValueOfOperationAuthorizationRec [%s]\n",objDef.operationList);
			return(ret);
		}
		rec.resID = objDef.resID;
		strcpy(rec.tableName,tblName);
		rec.level = 4;
		if (UnionIsDigitStr(fldNameGrp[index]))
		{
			rec.serviceID = atoi(fldNameGrp[index]);
			UnionReadOperationNameByServiceID(rec.serviceID,rec.serviceName);
		}
		else
		{
			if ((rec.serviceID = UnionReadServiceIDByOperationName(fldNameGrp[index])) < 0)
			{
				UnionUserErrLog("in UnionGenerateOperationFromTBLDefInTableList:: UnionReadServiceIDByOperationName [%s]\n",fldNameGrp[index]);
				return(rec.serviceID);
			}
			strcpy(rec.serviceName,fldNameGrp[index]);
		}
		switch (rec.serviceID)
		{
			case	conDatabaseCmdInsert:
				sprintf(rec.remark,"����%s��¼",objDef.tableAlais);
				if (strlen(rec.serviceName) == 0)
					strcpy(rec.serviceName,"���Ӽ�¼");
				break;
			case	conDatabaseCmdDelete:
				sprintf(rec.remark,"ɾ��%s��¼",objDef.tableAlais);
				if (strlen(rec.serviceName) == 0)
					strcpy(rec.serviceName,"ɾ����¼");
				break;
			case	conDatabaseCmdUpdate:
				sprintf(rec.remark,"��������%s��¼",objDef.tableAlais);
				if (strlen(rec.serviceName) == 0)
					strcpy(rec.serviceName,"���¼�¼");
				break;
			case	conDatabaseCmdQuerySpecRec:
				sprintf(rec.remark,"��ȡ%s��¼",objDef.tableAlais);
				if (strlen(rec.serviceName) == 0)
					strcpy(rec.serviceName,"��ѯָ����¼");
				break;
			case	conDatabaseCmdQueryAllRec:
				sprintf(rec.remark,"��ѯ����%s��¼",objDef.tableAlais);
				if (strlen(rec.serviceName) == 0)
					strcpy(rec.serviceName,"��ѯ���м�¼");
				break;
			default:
				sprintf(rec.remark,"�Զ���%s����",objDef.tableAlais);
				if (strlen(rec.serviceName) == 0)
					strcpy(rec.serviceName,"�Զ������");
				break;
		}
		if (rec.serviceID > 0)
		{
			if ((ret = UnionInsertOperationAuthorizationRec(&rec)) < 0)
			{
				UnionUserErrLog("in UnionGenerateOperationFromTBLDefInTableList:: UnionInsertOperationAuthorizationRec [%s]\n",objDef.operationList);
				continue;
			}
			else
				recNum++;
		}
		// �Զ�����һ���˵���
		memset(&menuItem,0,sizeof(menuItem));
		UnionFormDefaultValueOfMenuItemOperationDefRec(&menuItem);
		// �˵�������
		strcpy(menuItem.menuName,rec.serviceName);
		// ��������
		strcpy(menuItem.operatingTableName,rec.tableName);
		// ����ֵ
		menuItem.serviceID = rec.serviceID;
		// ��������
		strcpy(menuItem.serviceName,rec.serviceName);
		// �˵�����
		memset(primaryKeyStr,0,sizeof(primaryKeyStr));
		if ((ret = UnionGenerateQueryFldRefFromFldIDList(objDef.primaryKey,strlen(objDef.primaryKey),primaryKeyStr)) < 0)
		{
			UnionUserErrLog("in UnionGenerateOperationFromTBLDefInTableList:: UnionGenerateQueryFldRefFromFldIDList for table [%s]!\n",tblName);
			//return(ret);
			continue;
		}
		if (rec.serviceID != conDatabaseCmdQueryAllRec)
		{
				sprintf(menuItem.conditionsMenu,"%s",primaryKeyStr);
		}
		if ((ret = UnionInsertMenuItemOperationDefRec(&menuItem)) < 0)
		{
			UnionProgramerLog("in UnionGenerateOperationFromTBLDefInTableList:: UnionInsertMenuItemOperationDefRec [%s] for table [%s]!\n",menuItem.menuName,tblName);
			//return(ret);
			//continue;
		}
	}
	return(recNum);
}


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
int UnionInsertInitRecOfSpecTBLOnTBLDefInTableList(char *tblName)
{
	int			ret;
	TUnionTableList		objDef;
	int			fldNum;
	char			fldNameGrp[256][128];
	int			index;
	int			recNum = 0;
	int			len;
	
	if (tblName == NULL)
	{
		UnionUserErrLog("in UnionInsertInitRecOfSpecTBLOnTBLDefInTableList:: null pointer!\n");
		return(errCodeParameter);
	}
		
	// ���������
	memset(&objDef,0,sizeof(objDef));
	if (strlen(tblName) >= sizeof(objDef.tableName))
	{
		UnionUserErrLog("in UnionInsertInitRecOfSpecTBLOnTBLDefInTableList:: tblName [%s] is too long!\n",tblName);
		return(errCodeParameter);
	}
	strcpy(objDef.tableName,tblName);
	if ((ret = UnionReadTableListRec(&objDef)) < 0)
	{
		UnionUserErrLog("in UnionInsertInitRecOfSpecTBLOnTBLDefInTableList:: UnionReadTableListRec [%s]!\n",tblName);
		return(ret);
	}
	// ������嵥
	memset(fldNameGrp,0,sizeof(fldNameGrp));
	if ((fldNum = UnionSeprateVarStrIntoVarGrp(objDef.initValueList,strlen(objDef.initValueList),conSeparableSignOfLineInMultiRow,fldNameGrp,256)) < 0)
	{
		UnionUserErrLog("in UnionInsertInitRecOfSpecTBLOnTBLDefInTableList:: UnionSeprateVarStrIntoVarGrp [%s]\n",objDef.initValueList);
		return(fldNum);
	}
	UnionAuditLog("in UnionInsertInitRecOfSpecTBLOnTBLDefInTableList:: initValueList = [%s]\n",objDef.initValueList);
	UnionAuditLog("in UnionInsertInitRecOfSpecTBLOnTBLDefInTableList:: recNum = [%d]\n",fldNum);
	for (index = 0; index < fldNum; index++)
	{
		UnionNullLog("in UnionInsertInitRecOfSpecTBLOnTBLDefInTableList:: recIndex = [%02d][%s]\n",index+1,fldNameGrp[index]);
		if ((len = strlen(fldNameGrp[index])) == 0)
			continue;
		while (len > 0)
		{
			// ȥ����β�Ļس�
			if ((fldNameGrp[index][len-1] == 10) || (fldNameGrp[index][len-1] == 13))
			{
				fldNameGrp[index][len-1] = 0;
				len--;
			}
			else
				break;
		}
		if (len == 0)
			continue;
		UnionConvertOneFldSeperatorInRecStrIntoAnother(fldNameGrp[index],len,';','|',
				fldNameGrp[index],sizeof(fldNameGrp[index]));
		if (fldNameGrp[index][len-1] != '|')
		{
			fldNameGrp[index][len-1] = '|';
			len += 1;
			fldNameGrp[index][len] = 0;
		}
		if ((ret = UnionInsertObjectRecord(objDef.tableName,fldNameGrp[index],len)) < 0)
		{
			UnionUserErrLog("in UnionInsertInitRecOfSpecTBLOnTBLDefInTableList:: UnionInsertObjectRecord [%s] into \n",fldNameGrp[index],objDef.tableName);
			continue;
		}
		recNum++;
	}
	UnionAuditLog("in UnionInsertInitRecOfSpecTBLOnTBLDefInTableList:: inserted recNum = [%d]\n",recNum);
	return(recNum);
}

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
int UnionGenerateAllMngSvrConfForSpecTBLOrSpecView(char *tellerTypeID,char *tableName,char *tableAlais,int isView,int *fileRecved)
{
	int		ret;
	char		*infoFileName;
	char		info[512];
	
	*fileRecved = 0;
	
	infoFileName = UnionGenerateMngSvrTempFile();
	//UnionLog("in UnionGenerateAllMngSvrConfForSpecTBLOrSpecView:: infoFileName = [%s]\n",infoFileName);
	// ��ʼ����¼
	if (!isView)
	{
		if ((ret = UnionInsertInitRecOfSpecTBLOnTBLDefInTableList(tableName)) < 0)
			UnionUserErrLog("in UnionGenerateAllMngSvrConfForSpecTBLOrSpecView:: UnionInsertInitRecOfSpecTBLOnTBLDefInTableList [%s]!\n",tableName);
		sprintf(info,"�ɹ�::��::%s::����ʼ����%d����¼",tableName,ret);
		UnionAppendRecStrToFile(infoFileName,info,strlen(info));
		// Ϊ�����ɲ����ʿ�
		if ((ret = UnionGenerateOperationFromTBLDefInTableList(tableName)) < 0)
			UnionUserErrLog("in UnionGenerateAllMngSvrConfForSpecTBLOrSpecView:: UnionGenerateOperationFromTBLDefInTableList [%s]!\n",tableName);
		sprintf(info,"�ɹ�::��::%s::��������%d��������Ȩ��¼",tableName,ret);
		UnionAppendRecStrToFile(infoFileName,info,strlen(info));
		// ����ö��
		if ((ret = UnionGenerateEnumDefFromTBLToSpecFile(tableName,
			UnionGetEnumConfFileNameByTableAlais(tableAlais))) < 0)
		{
			UnionUserErrLog("in UnionGenerateAllMngSvrConfForSpecTBLOrSpecView:: UnionGenerateEnumDefFromTBLToSpecFile [%s]!\n",tableName);
			sprintf(info,"ʧ��::��::%s::����ö�ٶ���,������=%d",tableName,ret);
		}
		else
			sprintf(info,"�ɹ�::��::%s::����ö�ٶ���",tableName);
		UnionAppendRecStrToFile(infoFileName,info,strlen(info));
		// ���ɽ����SQL���
		if ((ret = UnionCreateObjectDefSQLFromTBLDefInTableListToSpecFile(tableName,
			UnionGetDBTBLCreateSQLFileNameByTableName(tableName))) < 0)
		{
			UnionUserErrLog("in UnionGenerateAllMngSvrConfForSpecTBLOrSpecView:: UnionCreateObjectDefSQLFromTBLDefInTableListToSpecFile [%s]!\n",tableName);
			sprintf(info,"ʧ��::��::%s::���ɽ����SQL���,������=%d",tableName,ret);
		}
		else
			sprintf(info,"�ɹ�::��::%s::���ɽ����SQL���",tableName);
		UnionAppendRecStrToFile(infoFileName,info,strlen(info));
	}
	
	// ���ɽ����Ҽ��˵�
	if ((ret = UnionGenerateFormPopupMenuFromTBLDefInTableListToSpecFile(tellerTypeID,tableName,
		UnionGetFormPopupMenuFileNameByTableAlais(tableAlais))) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllMngSvrConfForSpecTBLOrSpecView:: UnionGenerateFormPopupMenuFromTBLDefInTableListToSpecFile [%s]!\n",tableName);
		sprintf(info,"ʧ��::��::%s::���ɽ����Ҽ��˵�,������=%d",tableName,ret);
	}
	else
		sprintf(info,"�ɹ�::��::%s::���ɽ����Ҽ��˵�",tableName);
	UnionAppendRecStrToFile(infoFileName,info,strlen(info));

	// ���ɼ�¼�Ҽ��˵�
	if ((ret = UnionGenerateRecPopupMenuFromTBLDefInTableListToSpecFile(tellerTypeID,tableName,
		UnionGetRecPopupMenuFileNameByTableAlais(tableAlais))) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllMngSvrConfForSpecTBLOrSpecView:: UnionGenerateRecPopupMenuFromTBLDefInTableListToSpecFile [%s]!\n",tableName);
		sprintf(info,"ʧ��::��::%s::���ɼ�¼�Ҽ��˵�,������=%d",tableName,ret);
	}
	else
		sprintf(info,"�ɹ�::��::%s::���ɼ�¼�Ҽ��˵�",tableName);
	UnionAppendRecStrToFile(infoFileName,info,strlen(info));

	// ���ɼ�¼˫���˵�
	if ((ret = UnionGenerateRecDoubleClickMenuFromTBLDefInTableListToSpecFile(tellerTypeID,tableName,
		UnionGetDoubleClickRecMenuFileNameByTableAlais(tableAlais))) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllMngSvrConfForSpecTBLOrSpecView:: UnionGenerateRecDoubleClickMenuFromTBLDefInTableListToSpecFile [%s]!\n",tableName);
		sprintf(info,"ʧ��::��::%s::���ɼ�¼˫���˵�,������=%d",tableName,ret);
	}
	else
		sprintf(info,"�ɹ�::��::%s::���ɼ�¼˫���˵�",tableName);
	UnionAppendRecStrToFile(infoFileName,info,strlen(info));

	// ���ɱ���ͼ
	if ((ret = UnionGenerateTBLQueryConfFromTBLDefInTableListToSpecFile(tableName,
		UnionGetTBLQueryConfFileNameByTableAlais(tableAlais))) < 0)
	{
		UnionUserErrLog("in UnionGenerateAllMngSvrConfForSpecTBLOrSpecView:: UnionGenerateTBLQueryConfFromTBLDefInTableListToSpecFile [%s]!\n",tableName);
		sprintf(info,"ʧ��::��::%s::���ɱ���ͼ,������=%d",tableName,ret);
	}
	else
		sprintf(info,"�ɹ�::��::%s::���ɱ���ͼ",tableName);
	UnionAppendRecStrToFile(infoFileName,info,strlen(info));

	*fileRecved = 1;

	return(0);
}

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
int UnionGenerateAllMngSvrConfForTable(char *tellerTypeID,char *tableName,int *fileRecved)
{
	int		ret;
	TUnionTableList	rec;
	TUnionViewList	viewRec;
	
	*fileRecved = 0;
	
	if ((tableName == NULL) || (strlen(tableName) >= sizeof(rec.tableName)))
	{
		UnionUserErrLog("in UnionGenerateAllMngSvrConfForTable:: tableName error!\n");
		return(errCodeParameter);
	}
	memset(&rec,0,sizeof(rec));
	strcpy(rec.tableName,tableName);
	if ((ret = UnionReadTableListRec(&rec)) < 0)
	{
		/*
		memset(&viewRec,0,sizeof(viewRec));
		if ((ret = UnionReadViewListRec(tableName,&viewRec)) < 0)
		{
			UnionUserErrLog("in UnionGenerateAllMngSvrConfForTable:: UnionReadViewListRec [%s]!\n",tableName);
			UnionUserErrLog("in UnionGenerateAllMngSvrConfForTable:: UnionReadTableListRec [%s]!\n",tableName);
			return(ret);
		}
		return(UnionGenerateAllMngSvrConfForSpecTBLOrSpecView(tellerTypeID,tableName,viewRec.viewAlais,1,fileRecved));
		*/
		UnionUserErrLog("in UnionGenerateAllMngSvrConfForTable:: UnionReadTableListRec [%s]!\n",tableName);
		return(ret);
	}

	return(UnionGenerateAllMngSvrConfForSpecTBLOrSpecView(tellerTypeID,tableName,rec.tableAlais,0,fileRecved));
}

/*
����	
	���ݱ���࣬�������˵�
�������
	��
�������
	��
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionCreateMainMenuByTBLType()
{
	int			ret;
	char			fileName[512+1];
	int			recNum = 0;
	FILE			*recFileFp = NULL;
	char			recStr[8192*2+1];
	int			lenOfRecStr;
	char			ID[128+1];
	char			*mainMenuFileName;
	FILE			*outFp = NULL;
	
	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/pid-%d.tmp",getenv("UNIONTEMP"),getpid());

	// �����м�¼
	if ((ret = UnionBatchSelectObjectRecord(conTBLNameTableType,"",fileName)) < 0)
	{
		UnionUserErrLog("in UnionCreateMainMenuByTBLType:: UnionBatchSelectObjectRecord [%s]!\n",conTBLNameTableType);
		return(ret);
	}
	
	if ((outFp = fopen(mainMenuFileName = UnionGetMngSvrClientMainMenuFileName(),"w")) < 0)
	{
		UnionUserErrLog("in UnionCreateMainMenuByTBLType:: fopen [%s]!\n",mainMenuFileName);
		return(errCodeUseOSErrCode);
	}
	
	// ���ļ�
	if ((recFileFp = fopen(fileName,"r")) == NULL)
	{
		UnionUserErrLog("in UnionCreateMainMenuByTBLType:: fopen [%s]\n",fileName);
		goto errExit;
	}
	while (!feof(recFileFp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(recFileFp,recStr,sizeof(recStr))) <= 0)
			continue;
		memset(ID,0,sizeof(ID));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"ID",ID,sizeof(ID))) < 0)
		{
			UnionUserErrLog("in UnionCreateMainMenuByTBLType:: UnionReadRecFldFromRecStr [%s] from [%s]\n","ID",recStr);
			goto errExit;
		}
		fprintf(outFp,"menuGrpName=%s|menuName=%s|level=489|\n",ID,ID);
		recNum++;
	}
	ret = recNum;
errExit:
	if (outFp != NULL)
		fclose(outFp);
	if (recFileFp != NULL)
		fclose(recFileFp);
	UnionDeleteFile(fileName);
	return(ret);
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
	���ݱ���࣬���������˵�
�������
	nameOf2LevelMenu	�����˵�����
�������
	��
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionCreate2LevelMenuByTBLType(char *nameOf2LevelMenu)
{
	int			ret;
	char			fileName[512+1];
	int			recNum = 0;
	FILE			*recFileFp = NULL;
	char			recStr[8192*2+1];
	int			lenOfRecStr;
	char			*menuFileName;
	FILE			*outFp = NULL;
	char			condition[128+1];
	char			tblTypeID[128+1];
	char			tableAlais[128+1];
	
	// ��ȡ�����˵���Ӧ�ķ����ʶ
	sprintf(condition,"ID=%s",nameOf2LevelMenu);
	memset(recStr,0,sizeof(recStr));
	if ((lenOfRecStr = UnionSelectUniqueObjectRecordByPrimaryKey(conTBLNameTableType,condition,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionCreate2LevelMenuByTBLType:: UnionSelectUniqueObjectRecordByPrimaryKey [%s]\n",condition);
		return(lenOfRecStr);
	}
	memset(tblTypeID,0,sizeof(tblTypeID));
	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"macroValue",tblTypeID,sizeof(tblTypeID))) < 0)
	{
		UnionUserErrLog("in UnionCreate2LevelMenuByTBLType:: UnionReadRecFldFromRecStr macroValue from [%s]\n",recStr);
		return(ret);
	}
	
	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/pid-%d.tmp",getenv("UNIONTEMP"),getpid());

	// �����м�¼
	sprintf(condition,"tableTypeID=%s",tblTypeID);
	if ((ret = UnionBatchSelectObjectRecord(conTBLNameTableList,condition,fileName)) < 0)
	{
		UnionUserErrLog("in UnionCreate2LevelMenuByTBLType:: UnionBatchSelectObjectRecord [%s]!\n",conTBLNameTableList);
		return(ret);
	}
	
	if ((outFp = fopen(menuFileName = UnionGetMngSvrClient2LevelMenuFileName(nameOf2LevelMenu),"w")) < 0)
	{
		UnionUserErrLog("in UnionCreate2LevelMenuByTBLType:: fopen [%s]!\n",menuFileName);
		return(errCodeUseOSErrCode);
	}
	
	// ���ļ�
	if ((recFileFp = fopen(fileName,"r")) == NULL)
	{
		UnionUserErrLog("in UnionCreate2LevelMenuByTBLType:: fopen [%s]\n",fileName);
		goto errExit;
	}
	while (!feof(recFileFp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(recFileFp,recStr,sizeof(recStr))) <= 0)
			continue;
		memset(tableAlais,0,sizeof(tableAlais));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"tableAlais",tableAlais,sizeof(tableAlais))) < 0)
		{
			UnionUserErrLog("in UnionCreate2LevelMenuByTBLType:: UnionReadRecFldFromRecStr [%s] from [%s]\n","tableAlais",recStr);
			goto errExit;
		}
		fprintf(outFp,"itemName=%s|tblAlais=%s|operationID=51|iconFileName=%s����ͼ��|\n",tableAlais,tableAlais,tableAlais);
		recNum++;
	}
	ret = recNum;
errExit:
	if (outFp != NULL)
		fclose(outFp);
	if (recFileFp != NULL)
		fclose(recFileFp);
	UnionDeleteFile(fileName);
	return(ret);
}

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
int UnionCreateAllMenuByTBLType(int *fileRecved)
{
	int			ret;
	char			fileName[512+1];
	int			recNum = 0;
	FILE			*recFileFp = NULL;
	char			recStr[8192*2+1];
	int			lenOfRecStr;
	char			ID[128+1];
	char			*infoFileName;
	char			info[512+1];
	
	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/pid-%d.tmp",getenv("UNIONTEMP"),getpid());

	*fileRecved = 1;
	infoFileName = UnionGenerateMngSvrTempFile();
	// �������˵�
	if ((ret = UnionCreateMainMenuByTBLType()) < 0)
	{
		UnionUserErrLog("in UnionCreateAllMenuByTBLType:: UnionCreateMainMenuByTBLType!\n");
		sprintf(info,"ʧ��::�������˵�,ret=%d",ret);
	}
	else
		sprintf(info,"�ɹ�::�������˵�");
	UnionAppendRecStrToFile(infoFileName,info,strlen(info));
	
	// �����м�¼
	if ((ret = UnionBatchSelectObjectRecord(conTBLNameTableType,"",fileName)) < 0)
	{
		UnionUserErrLog("in UnionCreateMainMenuByTBLType:: UnionBatchSelectObjectRecord [%s]!\n",conTBLNameTableType);
		return(ret);
	}
	
	// ���ļ�
	if ((recFileFp = fopen(fileName,"r")) == NULL)
	{
		UnionUserErrLog("in UnionCreateMainMenuByTBLType:: fopen [%s]\n",fileName);
		goto errExit;
	}
	while (!feof(recFileFp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(recFileFp,recStr,sizeof(recStr))) <= 0)
			continue;
		memset(ID,0,sizeof(ID));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"ID",ID,sizeof(ID))) < 0)
		{
			UnionUserErrLog("in UnionCreateMainMenuByTBLType:: UnionReadRecFldFromRecStr [%s] from [%s]\n","ID",recStr);
			goto errExit;
		}
		if ((ret = UnionCreate2LevelMenuByTBLType(ID)) < 0)
		{
			UnionUserErrLog("in UnionCreateAllMenuByTBLType:: UnionCreate2LevelMenuByTBLType [%s]!\n",ID);
			sprintf(info,"ʧ��::���ɶ����˵�[%s],ret=%d",ID,ret);
		}
		else
		{
			recNum++;
			sprintf(info,"�ɹ�::���ɶ����˵�[%s]",ID);
		}
		UnionAppendRecStrToFile(infoFileName,info,strlen(info));
	}
	ret = recNum;
errExit:
	fclose(recFileFp);
	UnionDeleteFile(fileName);
	return(ret);
}

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
int UnionReadMenuItemListOfSpecMenuGrp(char *tellerTypeID,char *mainMenuName,char *menuGrpName,char *menuItemList,int sizeOfMenuItemList)
{
	int			ret;
	int			menuItemDefNum = 0;
	TUnionMenuItemDef	menuItemDefRec;
	PUnionMenuItemDef	pmenuItemDefRec;
	int			index;
	int			itemNum = 0;
	int			offset = 0;
	TUnionMenuDef		menuDef;
	char			menuItemNameGrp[48][128+1];

	// ���˵�����
	memset(&menuDef,0,sizeof(menuDef));
	if ((ret = UnionReadMenuDefRec(mainMenuName,menuGrpName,&menuDef)) < 0)
	{
		UnionUserErrLog("in UnionReadMenuItemListOfSpecMenuGrp:: UnionReadMenuDefRec [%s][%s]\n",mainMenuName,menuGrpName);
		return(ret);
	}
	// ���ò˵������Ĳ˵���
	if ((menuItemDefNum = UnionSeprateVarStrIntoVarGrp(menuDef.menuItemsList,strlen(menuDef.menuItemsList),conSeparableSignOfFieldNameInFldList,menuItemNameGrp,48)) < 0)
	{
		UnionUserErrLog("in UnionReadMenuItemListOfSpecMenuGrp:: UnionSeprateVarStrIntoVarGrp [%s]\n",menuDef.menuItemsList);
		return(menuItemDefNum);
	}
	for (index = 0; index < menuItemDefNum; index++)
	{
		// ���˵����
		memset(&menuItemDefRec,0,sizeof(menuItemDefRec));
		if ((ret = UnionReadMenuItemDefRec(mainMenuName,menuGrpName,menuItemNameGrp[index],&menuItemDefRec)) < 0)
		{
			UnionAuditLog("in UnionReadMenuItemListOfSpecMenuGrp:: UnionReadMenuItemDefRec [%s][%s]\n",menuGrpName,menuItemNameGrp[index]);
			continue;
		}
		pmenuItemDefRec = &menuItemDefRec;
		if ((strncmp(pmenuItemDefRec->menuName,"�ղ���",6) != 0) && (pmenuItemDefRec->menuDisplayname[0] != '-'))
		{
			if (UnionIsFldStrInUnionFldListStr(pmenuItemDefRec->visualLevelListOfMenu,strlen(pmenuItemDefRec->visualLevelListOfMenu),',',tellerTypeID) <= 0)
				continue;
		}
		if (itemNum > 0)
		{
			menuItemList[offset] = ',';
			offset++;
		}
		if (strlen(pmenuItemDefRec->menuRealDisplayName) == 0)
		{
			//UnionLog("**** menuRealDisplayName is null!\n");
			strcpy(menuItemList+offset,pmenuItemDefRec->menuDisplayname);
			offset += strlen(pmenuItemDefRec->menuDisplayname);
		}
		else
		{
			strcpy(menuItemList+offset,pmenuItemDefRec->menuRealDisplayName);
			offset += strlen(pmenuItemDefRec->menuRealDisplayName);
		}
		itemNum++;
	}
	return(offset);
}

/*
����	
	��ȡָ���ļ����ָ�������˵��Ĳ˵���Ĳ���Ȩ���嵥
�������
	level		��������
	menuName	���˵�����
	sizeOfMenuItemList	�˵����嵥����󳤶�
�������
	menuItemList	�˵����嵥
����ֵ
	>=0	�˵����嵥�ĳ���
	<0	������
*/
int UnionReadMenuItemListOfSpecMainMenu(char *tellerTypeID,char *menuName,char *menuItemList,int sizeOfMenuItemList)
{
	int	ret;
	char	menuList[2048+1];
	int	index;
	char	itemGrp[32][128];
	char	levelList[512+1];
	int	itemNum;
	int	itemIndex = 0;
	int	offset = 0;
	
	// �����˵��Ĳ˵����嵥
	memset(menuList,0,sizeof(menuList));
	if ((ret = UnionReadMainMenuRecFld(menuName,conMainMenuFldNameMainMenuItemsList,menuList,sizeof(menuList))) < 0)
	{
		UnionUserErrLog("in UnionReadMenuItemListOfSpecMainMenu:: UnionReadMainMenuRecFld! menuName = [%s]\n",menuName);
		return(ret);
	}
	// �۷ֲ˵����嵥
	if ((itemNum = UnionSeprateVarStrIntoVarGrp(menuList,ret,',',itemGrp,32)) < 0)
	{
		UnionUserErrLog("in UnionReadMenuItemListOfSpecMainMenu:: UnionSeprateVarStrIntoVarGrp! [%04d][%s]\n",ret,menuList);
		return(itemNum);
	}
	// ��ȡÿ���˵������Ȩ�嵥
	for (index = 0; index < itemNum; index++)
	{
		memset(levelList,0,sizeof(levelList));
		if ((ret = UnionReadMenuDefRecFld(menuName,itemGrp[index],conMenuDefFldNameVisualLevelListOfMenu,levelList,sizeof(levelList))) < 0)
		{
			UnionAuditLog("in UnionReadMenuItemListOfSpecMainMenu:: UnionReadMenuDefRecFld! [%s][%s] error!\n",menuName,itemGrp[index]);
			continue;
		}
		if (UnionIsFldStrInUnionFldListStr(levelList,ret,',',tellerTypeID) <= 0)
			continue;
		// ������˵����������Ȩ
		if (itemIndex > 0)
		{
			menuItemList[offset] = ',';
			offset++;
		}
		strcpy(menuItemList+offset,itemGrp[index]);
		offset += strlen(itemGrp[index]);
		itemIndex++;
	}
	return(offset);
}

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
int UnionReadMenuItemListOfSpecSecondaryMenu(char *tellerTypeID,char *mainMenuName,char *menuName,char *menuItemList,int sizeOfMenuItemList)
{
	return(UnionReadMenuItemListOfSpecMenuGrp(tellerTypeID,mainMenuName,menuName,menuItemList,sizeOfMenuItemList));
}

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
int UnionReadMenuItemListOfSpecTable(char *tellerTypeID,char *tableName,char *mainMenuName,int menuType,char *menuItemList,int sizeOfMenuItemList)
{
	TUnionViewList		rec;
	int			ret;
		
	// ����ͼ�����¼
	memset(&rec,0,sizeof(rec));
	if ((ret = UnionReadViewListRec(tableName,mainMenuName,&rec)) < 0)
	{
		UnionUserErrLog("in UnionReadMenuItemListOfSpecTable:: UnionReadViewListRec [%s]!\n",tableName);
		return(ret);
	}
	switch (menuType)
	{
		case	conMenuItemTypeFormPopMenu:
			return(UnionReadMenuItemListOfSpecMenuGrp(tellerTypeID,rec.mainMenuName,rec.formPopMenuDefFileName,menuItemList,sizeOfMenuItemList));
		case	conMenuItemTypeRecPopMenu:
			return(UnionReadMenuItemListOfSpecMenuGrp(tellerTypeID,rec.mainMenuName,rec.recPopMenuDefFileName,menuItemList,sizeOfMenuItemList));
		case	conMenuItemTypeDoubleClickMenu:
			return(UnionReadMenuItemListOfSpecMenuGrp(tellerTypeID,rec.mainMenuName,rec.recDoubleClickMenuFileName,menuItemList,sizeOfMenuItemList));
		default:
			UnionUserErrLog("in UnionReadMenuItemListOfSpecTable:: invalid menuType [%d]\n",menuType);
			return(errCodeParameter);
	}
}

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
int UnionGenerateDefaultViewOfTBL(char *tblName,char *mainMenuName,char *viewRecStr,int sizeOfBuf)
{
	int			ret;
	TUnionTableList		tblDef;
	TUnionViewList		viewDef;
	
	if (tblName == NULL)
	{
		UnionUserErrLog("in UnionGenerateDefaultViewOfTBL:: null pointer!\n");
		return(errCodeParameter);
	}
	// ������
	memset(&tblDef,0,sizeof(tblDef));
	strcpy(tblDef.tableName,tblName);
	if ((ret = UnionReadTableListRec(&tblDef)) < 0)
	{
		UnionUserErrLog("in UnionGenerateDefaultViewOfTBL:: UnionReadTableListRec [%s]!\n",tblName);
		return(ret);
	}
	// ���ݱ������ͼ���帳ֵ
	memset(&viewDef,0,sizeof(viewDef));
	strcpy(viewDef.ID,tblDef.tableAlais);
	strcpy(viewDef.viewAlais,tblDef.tableAlais);
	strcpy(viewDef.tableName,tblDef.tableName);
	viewDef.resID = tblDef.resID;
	viewDef.tableTypeID = atoi(tblDef.tableTypeID);
	strcpy(viewDef.primaryKey,tblDef.primaryKey);
	strcpy(viewDef.fieldList,tblDef.fieldList);
	strcpy(viewDef.fieldDisplayList,tblDef.fieldList);
	//strcpy(viewDef.fieldAlaisList,tblDef.fieldList);
	strcpy(viewDef.mainMenuName,mainMenuName);
	viewDef.maxNumOfRecPerPage = 32;
	viewDef.useSeparateTBL = 1;
	strcpy(viewDef.remark,tblDef.remark);
	// �����¼
	if ((ret = UnionInsertViewListRec(&viewDef)) < 0)
	{
		UnionUserErrLog("in UnionGenerateDefaultViewOfTBL:: UnionInsertViewListRec [%s][%s]!\n",viewDef.ID,viewDef.tableName);
		return(ret);
	}
	if ((ret = UnionFormRecStrFromViewListRec(&viewDef,viewRecStr,sizeOfBuf)) < 0)
		
	{
		UnionUserErrLog("in UnionGenerateDefaultViewOfTBL:: UnionFormRecStrFromViewListRec [%s][%s]!\n",viewDef.ID,viewDef.tableName);
		return(ret);
	}
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
int UnionGenerateEnumDefFromTBLForSpecMainMenu(char *tblName,char *mainMenuName,char *enumFileName)
{
	FILE			*recFileFp;
	int			ret;
	char			fileName[512+1];
	int			recNum = 0;
	char			recStr[8192*2+1];
	int			lenOfRecStr;
	char			remark[512+1];
	char			ID[128+1];
	char			macroValue[128+1];
	int			fldNum;
	char			funName[128+1];
	char			thisEnumFileName[128+1];
	char			tblAlais[128+1];
	TUnionEnumValueDef	enumValueDefRec;
	
	if ((ret = UnionExistsMacroDefInTBL(tblName)) <= 0)
		return(ret);
	
	if ((enumFileName == NULL) || (strlen(enumFileName) == 0))	
	{
		memset(tblAlais,0,sizeof(tblAlais));
		if ((ret = UnionReadTableAlaisOfSpecTBLName(tblName,tblAlais)) < 0)
		{
			UnionUserErrLog("in UnionGenerateEnumDefFromTBLForSpecMainMenu:: UnionReadTableAlaisOfSpecTBLName! [%s]\n",tblName);
			return(ret);
		}
		if (strlen(tblAlais) == 0)
			strcpy(thisEnumFileName,tblName);
		else
			strcpy(thisEnumFileName,tblAlais);
	}
	else
		strcpy(thisEnumFileName,enumFileName);
	
	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/pid-%d.tmp",getenv("UNIONTEMP"),getpid());

	// �����м�¼
	if ((ret = UnionBatchSelectObjectRecord(tblName,"",fileName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateEnumDefFromTBLForSpecMainMenu:: UnionBatchSelectObjectRecord [%s]!\n",tblName);
		return(ret);
	}
	
	// ���ļ�
	if ((recFileFp = fopen(fileName,"r")) == NULL)
	{
		UnionUserErrLog("in UnionGenerateEnumDefFromTBLForSpecMainMenu:: fopen [%s]\n",fileName);
		goto errExit;
	}
	while (!feof(recFileFp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(recFileFp,recStr,sizeof(recStr))) <= 0)
			continue;
		memset(ID,0,sizeof(ID));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"ID",ID,sizeof(ID))) < 0)
		{
			UnionUserErrLog("in UnionGenerateEnumDefFromTBLForSpecMainMenu:: UnionReadRecFldFromRecStr [%s] from [%s]\n","ID",recStr);
			goto errExit;
		}
		if (ret == 0)
		{
			UnionUserErrLog("in UnionGenerateEnumDefFromTBLForSpecMainMenu:: ID is null in [%s]\n",recStr);
			ret = errCodeMarcoDefNameNotDefined;
			goto errExit;
		}
		memset(macroValue,0,sizeof(macroValue));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"macroValue",macroValue,sizeof(macroValue))) < 0)
		{
			UnionUserErrLog("in UnionGenerateEnumDefFromTBLForSpecMainMenu:: UnionReadRecFldFromRecStr [%s] from [%s]\n","macroValue",recStr);
			ret = errCodeMarcoDefNameNotDefined;
			goto errExit;
		}
		if (ret == 0)
		{
			UnionUserErrLog("in UnionGenerateEnumDefFromTBLForSpecMainMenu:: ID is null in [%s]\n",recStr);
			ret = errCodeMarcoDefValueNotDefined;
			goto errExit;
		}
		memset(remark,0,sizeof(remark));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"remark",remark,sizeof(remark))) < 0)
		{
			UnionUserErrLog("in UnionGenerateEnumDefFromTBLForSpecMainMenu:: UnionReadRecFldFromRecStr [%s] from [%s]\n","remark",recStr);
			ret = errCodeMarcoDefNameNotDefined;
			goto errExit;
		}
		memset(&enumValueDefRec,0,sizeof(enumValueDefRec));
		strcpy(enumValueDefRec.mainMenuName,mainMenuName);
		strcpy(enumValueDefRec.enumFileName,thisEnumFileName);
		strcpy(enumValueDefRec.itemName,ID);
		strcpy(enumValueDefRec.itemValue,macroValue);
		strcpy(enumValueDefRec.remark,remark);
		if ((ret = UnionInsertEnumValueDefRec(&enumValueDefRec)) < 0)
		{
			UnionUserErrLog("in UnionGenerateEnumDefFromTBLForSpecMainMenu:: UnionInsertEnumValueDefRec [%s] [%s] [%s]\n",mainMenuName,thisEnumFileName,ID);
			continue;
		}
		recNum++;
	}
	ret = recNum;
	UnionLog("in UnionGenerateEnumDefFromTBLForSpecMainMenu:: recNum = [%d]\n",recNum);
errExit:
	fclose(recFileFp);
	UnionDeleteFile(fileName);
	return(ret);
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
	char			fldNameGrp[128][128+1];
	int			fldNum,fldIndex;
	char			tmpBuf[128+1];
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
	char			menuItemNameGrp[48][128+1];
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
int UnionGenerateMenuGrpDefDocToSpecFile(char *mainMenuName,char *menuGrpName,char *fileName)
{
	FILE	*fp;
	int	ret;
	
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionGenerateMenuGrpDefDocToSpecFile:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	if ((ret = UnionGenerateMenuGrpDefDocToFp(mainMenuName,menuGrpName,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateMenuGrpDefDocToSpecFile:: UnionGenerateMenuGrpDefDocToFp [%s][%s]\n",mainMenuName,menuGrpName);
	}
	fclose(fp);
	return(ret);
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
int UnionGenerateMainMenuDefDocToFp(char *mainMenuName,FILE *fp)
{
	int			ret;
	int			index;
	char			itemGrp[48][128+1];
	int			itemNum;
	int			itemIndex = 0;
	char			menuList[2048+1];
	TUnionMenuDef		menuGrpDef;
	FILE			*outFp = stdout;
	TUnionOperatorType 	tellerTypeGrp[16];
	int			tellerTypeNum;
		
	// ��ȡ���й�Ա����
	if ((tellerTypeNum = UnionBatchReadOperatorTypeRec("",tellerTypeGrp,16)) < 0)
	{
		UnionUserErrLog("in UnionGenerateMainMenuDefDocToFp:: UnionBatchReadOperatorTypeRec!\n");
		return(tellerTypeNum);
	}
	
	if (fp != NULL)
		outFp = fp;
		
	// �����˵��Ĳ˵����嵥
	memset(menuList,0,sizeof(menuList));
	if ((ret = UnionReadMainMenuRecFld(mainMenuName,conMainMenuFldNameMainMenuItemsList,menuList,sizeof(menuList))) < 0)
	{
		UnionUserErrLog("in UnionGenerateMainMenuDefDocToFp:: UnionReadMainMenuRecFld! mainMenuName = [%s]\n",mainMenuName);
		return(ret);
	}
	// �۷ֲ˵����嵥
	if ((itemNum = UnionSeprateVarStrIntoVarGrp(menuList,ret,',',itemGrp,48)) < 0)
	{
		UnionUserErrLog("in UnionGenerateMainMenuDefDocToFp:: UnionSeprateVarStrIntoVarGrp! [%04d][%s]\n",ret,menuList);
		return(itemNum);
	}
	// ��ȡÿ���˵������Ȩ�嵥
	for (index = 0; index < itemNum; index++)
	{
		memset(&menuGrpDef,0,sizeof(menuGrpDef));
		if ((ret = UnionReadMenuDefRec(mainMenuName,itemGrp[index],&menuGrpDef)) < 0)
		{
			UnionAuditLog("in UnionGenerateMainMenuDefDocToFp:: UnionReadMenuDefRec! [%s][%s] error!\n",mainMenuName,itemGrp[index]);
			continue;
		}
		fprintf(outFp,"menuGrpName=%s|displayName=%s|",menuGrpDef.menuGrpName,menuGrpDef.displayName);
		UnionPrintAllAuthOperatorLevelIDStrToFp(tellerTypeGrp,tellerTypeNum,"level",menuGrpDef.visualLevelListOfMenu,outFp);
		fprintf(outFp,"\n");
		itemIndex++;
	}
	return(itemIndex);
}

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
int UnionGenerateMainMenuDefDocToSpecFile(char *mainMenuName,char *fileName)
{
	FILE	*fp;
	int	ret;
	
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionGenerateMainMenuDefDocToSpecFile:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	if ((ret = UnionGenerateMainMenuDefDocToFp(mainMenuName,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateMainMenuDefDocToSpecFile:: UnionGenerateMainMenuDefDocToFp [%s]\n",mainMenuName);
	}
	fclose(fp);
	return(ret);
}

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
int UnionGenerateMainMenuForTestToSpecFile(char *tellerTypeID,char *mainMenuName,char *fileName)
{
	int			ret;
	int			recNum = 0;
	FILE			*outFp = NULL;
	TUnionMainMenu		mainMenuRec;
	int			itemNum;
	int			itemIndex;
	char			itemName[100][128+1];
	TUnionMenuDef		menuDefRec;
		
	if ((outFp = fopen(fileName,"w")) == NULL)
	{
		UnionUserErrLog("in UnionGenerateMainMenuForTestToSpecFile:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	
	// �����˵�����
	memset(&mainMenuRec,0,sizeof(mainMenuRec));
	if ((ret = UnionReadMainMenuRec(mainMenuName,&mainMenuRec)) < 0)
	{
		UnionUserErrLog("in UnionGenerateMainMenuForTestToSpecFile:: UnionReadMainMenuRec [%s]!\n",mainMenuName);
		return(ret);
	}
	// ��ֲ˵���
	memset(itemName,0,sizeof(itemName));
	if ((ret = UnionSeprateVarStrIntoVarGrp(mainMenuRec.mainMenuItemsList,strlen(mainMenuRec.mainMenuItemsList),conSeparableSignOfFieldNameInFldList,itemName,100)) < 0)
	{
		UnionUserErrLog("in UnionGenerateMainMenuForTestToSpecFile:: UnionSeprateVarStrIntoVarGrp [%s]\n",mainMenuRec.mainMenuItemsList);
		return(ret);
	}
	itemNum = ret;
	
	for (itemIndex = 0; itemIndex < itemNum; itemIndex++)
	{
		memset(&menuDefRec,0,sizeof(menuDefRec));
		if ((ret = UnionReadMenuDefRec(mainMenuName,itemName[itemIndex],&menuDefRec)) < 0)
		{
			UnionAuditLog("in UnionGenerateMainMenuForTestToSpecFile:: UnionReadMenuDefRec [%s] error\n",itemName[itemIndex]);
			continue;
		}
		if (UnionIsFldStrInUnionFldListStr(menuDefRec.visualLevelListOfMenu,strlen(menuDefRec.visualLevelListOfMenu),',',tellerTypeID) <= 0)
		{
			UnionLog("in UnionGenerateMainMenuForTestToSpecFile:: [%s][%s] not authorized to [%s]\n",mainMenuName,itemName[itemIndex],tellerTypeID);
			continue;
		}
		//fprintf(outFp,"menuGrpName=%s|menuName=%s|level=1023456789|\n",menuDefRec.displayName,menuDefRec.menuGrpName);
		fprintf(outFp,"itemName=%s|tblAlais=NULL|operationID=51|level=9842|iconFileName=|\n",menuDefRec.displayName);
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
int UnionGenerateSecuServiceDefInDataPackFldListFormatToFp(char *cpField, int iLenOfField, char cDel, int iType, FILE *fp)
{
	//FILE *outFp = stdout;
	int ret = 0;
	char stField[200][128+1];
	int iFieldCnt = 0;
	int i = 0;
	TUnionDataPackFldList tDataPackFldList;

	if( cpField == NULL )
	{
		UnionUserErrLog("in UnionGenerateSecuServiceDefInDataPackFldListFormatToFp:: NULL pointer!\n");
		return(ret);
	}

	/*
	if (fp != NULL)
		outFp = fp;
	*/

	memset(stField, 0, sizeof(stField));
	if( (iFieldCnt = UnionSeprateVarStrIntoVarGrp(cpField, iLenOfField, cDel, stField, 200)) < 0 )
	{
		UnionUserErrLog("in UnionGenerateSecuServiceDefInDataPackFldListFormatToFp:: UnionSeprateVarStrIntoVarGrp [%s.%d.%c]!\n", 
			cpField, iLenOfField, cDel);
		return (iFieldCnt);
	}

	for( i = 0; i < iFieldCnt; i++ )
	{
		memset(&tDataPackFldList, 0, sizeof(TUnionDataPackFldList));
		if( (ret = UnionReadDataPackFldListRec(stField[i], &tDataPackFldList)) < 0 )
		{
			UnionUserErrLog("in UnionGenerateSecuServiceDefInDataPackFldListFormatToFp:: UnionReadDataPackFldListRec [%s]!\n", 
				stField[i]);
			return (ret);
		}

		switch(iType)
		{
			case 1: // �����ѡ��
				fprintf(fp, "fldType=0|Optional=0|ID=%s|macroName=%s|macroValue=%d|type=%d|length=%d|varName=%s|varType=%d|remark=%s|\n", 
					tDataPackFldList.ID, tDataPackFldList.macroName, tDataPackFldList.macroValue, tDataPackFldList.type,
					tDataPackFldList.length, tDataPackFldList.varName, tDataPackFldList.varType, tDataPackFldList.remark);
			case 2: // �����ѡ��
				fprintf(fp, "fldType=0|Optional=1|ID=%s|macroName=%s|macroValue=%d|type=%d|length=%d|varName=%s|varType=%d|remark=%s|\n", 
					tDataPackFldList.ID, tDataPackFldList.macroName, tDataPackFldList.macroValue, tDataPackFldList.type,
					tDataPackFldList.length, tDataPackFldList.varName, tDataPackFldList.varType, tDataPackFldList.remark);
			case 3: // ��Ӧ��ѡ��
				fprintf(fp, "fldType=1|Optional=0|ID=%s|macroName=%s|macroValue=%d|type=%d|length=%d|varName=%s|varType=%d|remark=%s|\n", 
					tDataPackFldList.ID, tDataPackFldList.macroName, tDataPackFldList.macroValue, tDataPackFldList.type,
					tDataPackFldList.length, tDataPackFldList.varName, tDataPackFldList.varType, tDataPackFldList.remark);
			case 4: // ��Ӧ��ѡ��
				fprintf(fp, "fldType=1|Optional=1|ID=%s|macroName=%s|macroValue=%d|type=%d|length=%d|varName=%s|varType=%d|remark=%s|\n", 
					tDataPackFldList.ID, tDataPackFldList.macroName, tDataPackFldList.macroValue, tDataPackFldList.type,
					tDataPackFldList.length, tDataPackFldList.varName, tDataPackFldList.varType, tDataPackFldList.remark);
			default:
				UnionUserErrLog("in UnionGenerateSecuServiceDefInDataPackFldListFormatToFp:: iType [%d] error!\n", iType);
				return (errCodeParameter);
		}
	}

	return (ret);
}

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
int UnionGenerateSecuServiceDefDocToFp(int iServiceID, FILE *fp)
{
	FILE *outFp = stdout;
	int ret = 0;
	TUnionSecurityServiceDef tSecuSerDef;

	// ��ȡ��ȫ������
	memset(&tSecuSerDef, 0, sizeof(TUnionSecurityServiceDef));
	if( (ret = UnionReadSecurityServiceDefRec(iServiceID, &tSecuSerDef)) < 0 )
	{
		UnionUserErrLog("in UnionGenerateSecuServiceDefDocToFp:: UnionReadSecurityServiceDefRec [%d]!\n", iServiceID);
		return(ret);
	}
	if (fp != NULL)
		outFp = fp;

	// ��������ѡ��
	if( (ret = UnionGenerateSecuServiceDefInDataPackFldListFormatToFp(tSecuSerDef.requestRequiredField, 
		strlen(tSecuSerDef.requestRequiredField), ',', 1, fp)) < 0 )
	{
		UnionUserErrLog("in UnionGenerateSecuServiceDefDocToFp:: UnionGenerateSecuServiceDefInDataPackFldListFormatToFp [%s]!\n",
			tSecuSerDef.requestRequiredField);
		return(ret);
	}

	// ��������ѡ��
	if( (ret = UnionGenerateSecuServiceDefInDataPackFldListFormatToFp(tSecuSerDef.requestOptionalField, 
		strlen(tSecuSerDef.requestOptionalField), ',', 2, fp)) < 0 )
	{
		UnionUserErrLog("in UnionGenerateSecuServiceDefDocToFp:: UnionGenerateSecuServiceDefInDataPackFldListFormatToFp [%s]!\n",
			tSecuSerDef.requestOptionalField);
		return(ret);
	}

	// �����Ӧ��ѡ��
	if( (ret = UnionGenerateSecuServiceDefInDataPackFldListFormatToFp(tSecuSerDef.responseRequiredField, 
		strlen(tSecuSerDef.responseRequiredField), ',', 3, fp)) < 0 )
	{
		UnionUserErrLog("in UnionGenerateSecuServiceDefDocToFp:: UnionGenerateSecuServiceDefInDataPackFldListFormatToFp [%s]!\n",
			tSecuSerDef.responseRequiredField);
		return(ret);
	}

	// �����Ӧ��ѡ��
	if( (ret = UnionGenerateSecuServiceDefInDataPackFldListFormatToFp(tSecuSerDef.responseOptionalField, 
		strlen(tSecuSerDef.responseOptionalField), ',', 4, fp)) < 0 )
	{
		UnionUserErrLog("in UnionGenerateSecuServiceDefDocToFp:: UnionGenerateSecuServiceDefInDataPackFldListFormatToFp [%s]!\n",
			tSecuSerDef.responseOptionalField);
		return(ret);
	}

	return(ret);
}

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
int UnionGenerateSecuServiceDefDocToSpecFile(int iServiceID, char *fileName)
{
	FILE *outFp = NULL;
	int ret = 0;

	if( fileName == NULL )
	{
		UnionUserErrLog("in UnionGenerateSecuServiceDefDocToSpecFile:: NULL pointer!\n");
		return(errCodeParameter);
	}

	if( (outFp = fopen(fileName,"w")) == NULL )
	{
		UnionUserErrLog("in UnionGenerateSecuServiceDefDocToSpecFile:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}

	if( (ret = UnionGenerateSecuServiceDefDocToFp(iServiceID, outFp)) < 0 )
	{
		UnionUserErrLog("in UnionGenerateSecuServiceDefDocToSpecFile:: UnionGenerateSecuServiceDefDocToFp [%d]!\n", iServiceID);
		fclose(outFp);
		return(ret);
	}

	fclose(outFp);
	return(ret);
}

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
int UnionCreateFldGrpIntoSQLToString(char *flag, PUnionObjectFldGrp pfldGrp, char *str, int iSizeofStr)
{
	char	flagTag[128];
	int	fldNum = 0;
	char	prefixFldName[128+1];
	int iOffset = 0;
	
	if( pfldGrp == NULL || str == NULL )
	{
		UnionUserErrLog("in UnionCreateFldGrpIntoSQLToString:: NULL pointer!\n");
		return(errCodeParameter);
	}

	if( flag == NULL )
		strcpy(flagTag, "");
	else
		strcpy(flagTag, flag);	
	for( fldNum = 0; fldNum < pfldGrp->fldNum; fldNum++ )
	{
		// 2009/6/23 ����������
		memset(prefixFldName, 0, sizeof(prefixFldName));
		UnionChargeFieldNameToDBFieldName(pfldGrp->fldNameGrp[fldNum], prefixFldName);
		
		if (fldNum == 0)
		{
			if( strlen(flagTag) == 0 )
				iOffset += sprintf(str+iOffset, "	(%s", prefixFldName);
			else
				iOffset += sprintf(str+iOffset, "	%s (%s", flagTag, prefixFldName);
		}
		else
			iOffset += sprintf(str+iOffset, ",%s", prefixFldName);
	}
	if (pfldGrp->fldNum > 0)
	{
		iOffset += sprintf(str+iOffset, ") ");
	}

	if( iOffset >= iSizeofStr )
	{
		UnionUserErrLog("in UnionCreateFldGrpIntoSQLToString:: iSizeofStr=[%d] is too small!\n", iSizeofStr);
		return (errCodeParameter);
	}

	return(iOffset);
}

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
int UnionGetCreateSQLFromObjectDef(PUnionObject prec, char *cpCreateSQL, int iSizeOfCreateSQL)
{
	int	ret = 0;
	int	fldNum = 0;
	PUnionObjectFldDef	pfldDef;
	char	typeName[40+1];
	int	index = 0;
	PUnionDefOfRelatedObject	prelation;
	int	dhNeeded = 0;
	char	prefixFldName[128+1];
	int	mustBeNotNullFld = 0;
	int iOffset = 0;
	
	if( prec == NULL || cpCreateSQL == NULL )
	{
		UnionUserErrLog("in UnionGetCreateSQLFromObjectDef:: NULL pointer!\n");
		return(errCodeParameter);
	}
	
	// д�������
	iOffset = 0;
	iOffset += sprintf(cpCreateSQL+iOffset, "create table %s(", prec->name);
	// ������
	for( fldNum = 0; fldNum < prec->fldNum; fldNum++ )
	{
		pfldDef = &(prec->fldDefGrp[fldNum]);
		memset(typeName,0,sizeof(typeName));
		UnionGetFldTypeName(pfldDef->type,typeName);

		// 2009/6/23 ����������
		memset(prefixFldName,0,sizeof(prefixFldName));
		UnionChargeFieldNameToDBFieldName(pfldDef->name, prefixFldName);

		switch (pfldDef->type)
		{
			case	conObjectFldType_String:
				if (pfldDef->size >= 254)
					iOffset += sprintf(cpCreateSQL+iOffset, " %s		varchar",prefixFldName);
				else
					iOffset += sprintf(cpCreateSQL+iOffset, "	%s		%s",prefixFldName,typeName);
					iOffset += sprintf(cpCreateSQL+iOffset, "(%d)",UnionGetValidFldSizeOfDatabase(pfldDef->size));
				break;
			case	conObjectFldType_Bit:
				iOffset += sprintf(cpCreateSQL+iOffset, "	%s		%s",prefixFldName,typeName);
				iOffset += sprintf(cpCreateSQL+iOffset, "(%d)",UnionGetValidFldSizeOfDatabase(pfldDef->size));
				break;
			case	conObjectFldType_Bool:
				iOffset += sprintf(cpCreateSQL+iOffset, "	%s		%s",prefixFldName,"integer");
				break;
			case	conObjectFldType_Double:
				if (UnionReadIntTypeRECVar("isDBUseOracle") > 0)
					iOffset += sprintf(cpCreateSQL+iOffset, " %s              number(12,2)",prefixFldName);
				else
					iOffset += sprintf(cpCreateSQL+iOffset, "	%s		%s",prefixFldName,typeName);
				break;
			default:
				iOffset += sprintf(cpCreateSQL+iOffset, "	%s		%s",prefixFldName,typeName);
				break;
		}
		mustBeNotNullFld = 0;
		if ((UnionIsPrimaryKeyFldOfObject(prec,pfldDef->name)) || (UnionIsUniqueFldOfObject(prec,pfldDef->name)))
		{
			// �ǹؼ��ֻ�Ψһֵ�����벻Ϊ����
			mustBeNotNullFld = 1;
			if (pfldDef->nullPermitted)
				pfldDef->nullPermitted = 0;
		}
		if (strlen(pfldDef->defaultValue) > 0)
		{
			// ������ȱʡֵ
			if (mustBeNotNullFld)	// ��ֵ���벻Ϊ��
				iOffset += sprintf(cpCreateSQL+iOffset, " not null");
			else
			{
				switch (pfldDef->type)
				{
					case	conObjectFldType_String:
					case	conObjectFldType_Bit:
						iOffset += sprintf(cpCreateSQL+iOffset, " default '%s'",pfldDef->defaultValue);
						break;
					default:
						iOffset += sprintf(cpCreateSQL+iOffset, " default %s",pfldDef->defaultValue);
						break;
				}
			}
		}
		else	// û����ȱʡֵ
		{
			if (!pfldDef->nullPermitted)
				iOffset += sprintf(cpCreateSQL+iOffset, " not null");
		}
		iOffset += sprintf(cpCreateSQL+iOffset, ",");
	}

	// ���ؼ���
	if( (ret = UnionCreateFldGrpIntoSQLToString("primary key ", &(prec->primaryKey), cpCreateSQL+iOffset, 
		iSizeOfCreateSQL-iOffset)) < 0 )
	{
		UnionUserErrLog("in UnionGetCreateSQLFromObjectDef:: UnionCreateFldGrpIntoSQLToString for primary key of [%s]\n", prec->name);
		return(ret);
	}
	iOffset += ret;
	dhNeeded = 1;
	for( index = 0; index < prec->uniqueFldGrpNum; index++ )
	{
		if( dhNeeded )
		{
			iOffset += sprintf(cpCreateSQL+iOffset, ",");
			dhNeeded = 0;
		}
		if( (ret = UnionCreateFldGrpIntoSQLToString("unique ", &(prec->uniqueFldGrp[index]), cpCreateSQL+iOffset, 
			iSizeOfCreateSQL-iOffset)) < 0 )
		{
			UnionUserErrLog("in UnionGetCreateSQLFromObjectDef:: UnionCreateFldGrpIntoSQLToString for the [%dth] unique of [%s]\n",
				index, prec->name);
			return(ret);
		}
		iOffset += ret;
		dhNeeded = 1;
	}
	// ���ⲿ�ؼ���
	for (index = 0; index < prec->foreignFldGrpNum; index++)
	{
		prelation = &(prec->foreignFldGrp[index]);
		if (strlen(prelation->objectName) == 0)
			continue;
		if (dhNeeded)
		{
			iOffset += sprintf(cpCreateSQL+iOffset, ",");
			dhNeeded = 0;
		}
		iOffset += sprintf(cpCreateSQL+iOffset, "	foreign key ");
		if( (ret = UnionCreateFldGrpIntoSQLToString("", &(prelation->localFldGrp), cpCreateSQL+iOffset, 
			iSizeOfCreateSQL-iOffset)) < 0 )
		{
			UnionUserErrLog("in UnionGetCreateSQLFromObjectDef:: UnionCreateFldGrpIntoSQLToString for the [%dth] foreign key of [%s]\n",
				index, prec->name);
			return(ret);
		}
		iOffset += ret;
		iOffset += sprintf(cpCreateSQL+iOffset, "references ");
		if( (ret = UnionCreateFldGrpIntoSQLToString(prelation->objectName, &(prelation->foreignFldGrp), cpCreateSQL+iOffset, 
			iSizeOfCreateSQL-iOffset)) < 0 )
		{
			UnionUserErrLog("in UnionGetCreateSQLFromObjectDef:: UnionCreateFldGrpIntoSQLToString for the [%dth] foreign key of [%s]\n",
				index, prec->name);
			return(ret);
		}
		iOffset += ret;
		dhNeeded = 1;
	}
	iOffset += sprintf(cpCreateSQL+iOffset, ");");

	if( iOffset >= iSizeOfCreateSQL )
	{
		UnionUserErrLog("in UnionGetCreateSQLFromObjectDef:: iSizeOfCreateSQL=[%d] is too small!\n", iSizeOfCreateSQL);
		return (errCodeParameter);
	}

	return(iOffset);
}

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
int UnionCreateTableByTBLDefInTableList(char *tblName)
{
	int	ret = 0;
	TUnionObject	rec;
	char szCreateSQL[1024+1];

	memset(&rec, 0, sizeof(rec));
	if( (ret = UnionGenerateObjectDefFromTBLDefInTableList(tblName, &rec)) < 0 )
	{
		UnionUserErrLog("in UnionCreateTableByTBLDefInTableList:: UnionGenerateObjectDefFromTBLDefInTableList for table [%s]!\n",
			tblName);
		return(ret);
	}

	memset(szCreateSQL, 0, sizeof(szCreateSQL));
	if( (ret = UnionGetCreateSQLFromObjectDef(&rec, szCreateSQL, sizeof(szCreateSQL))) < 0 )
	{
		UnionUserErrLog("in UnionCreateTableByTBLDefInTableList:: UnionGetCreateSQLFromObjectDef for table [%s]!\n", tblName);
		return(ret);
	}

	UnionLog("in UnionCreateTableByTBLDefInTableList:: szCreateSQL=[%s]\n", szCreateSQL);

	if( (ret = UnionExecRealDBSql(szCreateSQL)) < 0 )
	{
		UnionUserErrLog("in UnionCreateTableByTBLDefInTableList:: UnionExecRealDBSql [%s]!\n", szCreateSQL);
		return(ret);
	}

	return(ret);
}

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

