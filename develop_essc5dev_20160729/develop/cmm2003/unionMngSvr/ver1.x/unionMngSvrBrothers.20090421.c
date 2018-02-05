// Wolfgang Wang,2009/8/1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "unionResID.h"
#include "unionREC.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"
//#include "unionMngSvrBrothers.h"
#include "unionMngSvrList.h"
#include "unionTableList.h"
#include "unionThisKmcInfo.h"
#include "unionMngSvrDBDataSynJnl.h"
#include "unionMngSvrFileDataSynJnl.h"

char *UnionGetCurrentOperationTellerNo();

/*
���ܣ�
	���󱸷ݷ�����ͬ��һ�����붯��
�������:
	tableName	����
	record		���������Ĳ�����
	lenOfRecord	�������ĳ���
�������:
	��
����ֵ
	>=0		�ɹ�
	<0		ʧ��
*/
int UnionSynchronizeComplexDBInsertToBrothers(char *tableName,char *record,int lenOfRecord)
{
	return(UnionSynchronizeComplexDBOperationToBrothers(tableName,conResCmdInsert,record,lenOfRecord));
}

/*
���ܣ�
	���󱸷ݷ�����ͬ��һ��ɾ������
�������:
	tableName	����
	primaryKey	���������Ĺؼ���
�������:
	��
����ֵ
	>=0		�ɹ�
	<0		ʧ��
*/
int UnionSynchronizeComplexDBDeleteToBrothers(char *tableName,char *primaryKey)
{
	return(UnionSynchronizeComplexDBOperationToBrothers(tableName,conResCmdDelete,primaryKey,strlen(primaryKey)));
}

/*
���ܣ�
	���󱸷ݷ�����ͬ��һ�����¶���
�������:
	tableName	����
	primaryKey	���������Ĺؼ���
	record		���������Ĳ�����
	lenOfRecord	�������ĳ���
�������:
	��
����ֵ
	>=0		�ɹ�
	<0		ʧ��
*/
int UnionSynchronizeComplexDBUpdateToBrothers(char *tableName,char *primaryKey,char *record,int lenOfRecord)
{
	char	tmpBuf[8192+1];
	
	sprintf(tmpBuf,"%s%s",primaryKey,record);
	return(UnionSynchronizeComplexDBOperationToBrothers(tableName,conResCmdUpdate,tmpBuf,strlen(tmpBuf)));
}

/*
���ܣ�
	��ȡ��mngSvr�ı�ʶ
�������:
	��
�������:
	��
����ֵ
	�ɹ���	��mngSvr�ı�ʶ
	ʧ��	��ָ��
*/
char *UnionGetMyMngSvrID()
{
	/*
	char	varName[100];
	
	//sprintf(varName,"myMngSvrID%d",UnionReadResMngClientPort());
	sprintf(varName,"myMngSvrID");
	return(UnionReadStringTypeRECVar(varName));
	*/
	return(getenv("myMngSvrID"));
}

/*
���ܣ�
	��ȡ��mngSvr�������ֵ�
�������:
	maxNum		�ɶ�ȡ�������Ŀ
�������:
	brotherID	�ֵ�mngSvr�ı�ʶ
����ֵ
	>=0		��ȡ����Ŀ
	<0		������
*/
int UnionReadAllMngSvrBrothersOfMine(char brotherID[][128+1],int maxNum)
{
	char	brotherList[1024+1];
	int	ret;
	int	num;
	char	*myMngSvrID;
	
	memset(brotherList,0,sizeof(brotherList));

	if ((myMngSvrID = UnionGetMyMngSvrID()) == NULL || strlen(myMngSvrID) == 0)
	{
		UnionLog("in UnionReadAllMngSvrBrothersOfMine:: myMngSvrID not set\n");
		return(0);
	}
	
	UnionAuditLog("in myMngSvrID=[%s]\n",myMngSvrID);
	if ((ret = UnionReadMngSvrListRecFld(myMngSvrID,conMngSvrListFldNameMngSvrBrothers,brotherList,sizeof(brotherList))) < 0)
	{
		UnionUserErrLog("in UnionReadAllMngSvrBrothersOfMine:: UnionReadMngSvrListRecFld!\n");
		return(ret);
	}
	if ((num = UnionSeprateVarStrIntoVarGrp(brotherList,ret,',',brotherID,maxNum)) < 0)
	{
		UnionUserErrLog("in UnionReadAllMngSvrBrothersOfMine:: UnionSeprateVarStrIntoVarGrp [%04d][%s]\n",ret,brotherList);
		return(num);
	}
	return(num);
}


/*
���ܣ�
	����ָ�����ݷ�����ͬ��һ������
�������:
	mngSvrID	���ݷ������ı�ʶ
	tableName	����
	operation	����
	record		���������Ĳ�����
	lenOfRecord	�������ĳ���
�������:
	��
����ֵ
	>=0		�ɹ�
	<0		ʧ��
*/
int UnionSynchronizeComplexDBOperationToSpecBrother(char *mngSvrID,char *tableName,int operation,char *record,int lenOfRecord)
{
	int			resID;
	int			ret;
	TUnionMngSvrList	rec;
	char			tmpBuf[4192+1];
	int			fileRecved;
	//char		szTmpTableName[32+1];
	char			tellerNo[100];	// 2010-1-22,Wolfang Wang
	
	//memset(szTmpTableName, 0, sizeof(szTmpTableName));
	//strcpy(szTmpTableName, tableName);

	// ��ȡ���Ӧ����Դ
	if ((resID = UnionReadResIDOfSpecTBLName(tableName)) < 0)
	{
		UnionUserErrLog("in UnionSynchronizeComplexDBOperationToSpecBrother:: UnionReadResIDOfSpecTBLName [%s]!\n",tableName);
		return(resID);
	}
	// ��ȡ�ֵܵĶ˿ں͵�ַ
	memset(&rec,0,sizeof(rec));
	if ((ret = UnionReadMngSvrListRec(mngSvrID,&rec)) < 0)	
	{
		UnionUserErrLog("in UnionSynchronizeComplexDBOperationToSpecBrother:: UnionReadMngSvrListRec [%s]!\n",mngSvrID);
		return(ret);
	}
	// ���ֵܷ�������
	// 2010-1-22,Wolfang Wang
	memset(tellerNo,0,sizeof(tellerNo));
	if (UnionReadIntTypeRECVar("useOriginalTellerNo") > 0)
		strcpy(tellerNo,UnionGetCurrentOperationTellerNo());
	else
		strcpy(tellerNo,"backuper");
	// end of addition of 2010-1-22;
	return(UnionCommunicationWithSpecMngSvr(rec.ipAddr,rec.port,tellerNo,resID,operation,
		record,lenOfRecord,tmpBuf,sizeof(tmpBuf),&fileRecved));
}

/*
���ܣ�
	���󱸷ݷ�����ͬ��һ������
�������:
	tableName	����
	operation	����
	record		���������Ĳ�����
	lenOfRecord	�������ĳ���
�������:
	��
����ֵ
	>=0		�ɹ�
	<0		ʧ��
*/
int UnionSynchronizeComplexDBOperationToBrothers(char *tableName,int operation,char *record,int lenOfRecord)
{
	int			ret;
	int			num;
	int			index;
	char			brotherID[10][128];
	int			successNum = 0;
	int			seqNo;
	
	// �ж�Ҫ��ͬ��
	if (!UnionIsSynchronizeComplexDBOperationToBrothers())
		return(0);
	// ��ȡ����Ҫͬ���ķ�����	
	memset(brotherID,0,sizeof(brotherID));
	if ((num = UnionReadAllMngSvrBrothersOfMine(brotherID,10)) < 0)	
	{
		UnionUserErrLog("in UnionSynchronizeComplexDBOperationToBrothers:: UnionReadAllMngSvrBrothersOfMine!\n");
		return(num);
	}
	if (num > 0)
	{
		// ��ȡͬ����ˮ��
		/*
		if ((seqNo = UnionMngSvrGetSeqNoForDBDataSynJnl()) < 0)
		{
			UnionUserErrLog("in UnionSynchronizeComplexDBOperationToBrothers:: UnionMngSvrGetSeqNoForDBDataSynJnl!\n");
			return(seqNo);
		}
		*/
		UnionUserErrLog("in UnionSynchronizeComplexDBOperationToBrothers:: not support UnionMngSvrGetSeqNoForDBDataSynJnl!\n");
	}
	for (index = 0; index < num; index++)
	{
		// ��ָ��������ͬ��, modify by xusj 2010-04-16
		//if ((ret = UnionSynchronizeComplexDBOperationToSpecBrother(brotherID[index],tableName,operation,record,lenOfRecord)) < 0)
		if ((ret = UnionRegComplexDBOperationToDBDataSynJnl(brotherID[index],tableName,operation,record,lenOfRecord,seqNo)) < 0)
			UnionUserErrLog("in UnionSynchronizeComplexDBOperationToBrothers:: UnionSynchronizeComplexDBOperationToSpecBrother tableName=[%s] operation=[%d] [%04d][%s]!\n",tableName,operation,lenOfRecord,record);
		else
		{
			successNum++;
			UnionLog("in UnionSynchronizeComplexDBOperationToBrothers:: UnionRegComplexDBOperationToDBDataSynJnl tableName=[%s] operation=[%d] [%04d][%s] OK!\n",tableName,operation,lenOfRecord,record);
		}
	}
	return(successNum);
}

// �Ǽ�����ͬ����ˮ��
int UnionRegComplexDBOperationToDBDataSynJnl(char *mngSvrID, char *tableName, int operation, char *record, int lenOfRecord,long seqNo)
{
	int				ret = -1;
	TUnionMngSvrDBDataSynJnl	tMngSvrDBDataSynJnl;
	char				recStr[8192+1];
	int				lenOfRecStr;
	
	memset(&tMngSvrDBDataSynJnl,0,sizeof(tMngSvrDBDataSynJnl));
	
	memcpy(tMngSvrDBDataSynJnl.date,UnionGetCurrentFullSystemDate(),8);
	tMngSvrDBDataSynJnl.seqNo = seqNo;
	memcpy(tMngSvrDBDataSynJnl.mngSvrID,mngSvrID,strlen(mngSvrID));
	strcpy(tMngSvrDBDataSynJnl.tellerNo,UnionGetCurrentOperationTellerNo());
	memcpy(tMngSvrDBDataSynJnl.tableName,tableName,strlen(tableName));
	tMngSvrDBDataSynJnl.operation = operation;
	tMngSvrDBDataSynJnl.Flag = 0;
	
	if ((ret = UnionTranslateSpecCharInStr('|',record,lenOfRecord,tMngSvrDBDataSynJnl.record,sizeof(tMngSvrDBDataSynJnl.record))) < 0)
	{
		UnionUserErrLog("in UnionRegComplexDBOperationToDBDataSynJnl:: UnionTranslateSpecCharInStr! ret = [%d]\n",ret);
		return ret;
	}
	
	memset(recStr,0,sizeof(recStr));
	lenOfRecStr = UnionFormRecStrFromMngSvrDBDataSynJnlRec(&tMngSvrDBDataSynJnl,recStr,sizeof(recStr));
	if (lenOfRecStr < 0)
	{
		UnionUserErrLog("in UnionRegComplexDBOperationToDBDataSynJnl:: UnionFormRecStrFromMngSvrDBDataSynJnlRec!\n");
		return(ret);
	}
	if ((ret = UnionInsertObjectRecordWithoutSynchronize("mngSvrDBDataSynJnl",recStr,lenOfRecStr)) < 0)
	{
		UnionUserErrLog("in UnionRegComplexDBOperationToDBDataSynJnl:: UnionInsertObjectRecordWithoutSynchronize!\n");
		return(ret);
	}
	return ret;
}

// �Ǽ��ļ�ͬ����ˮ��
int UnionRegFileOperationToFileDataSynJnl(char *tableName, char *dirName,char *fileName, int fileType)
{
	int				ret;
	TUnionMngSvrFileDataSynJnl	tMngSvrFileDataSynJnl;
	char				brotherID[10][128];
	int				successNum;
	int				num;
	int				index;
	char				recStr[8192+1];
	int				lenOfRecStr;
	
	// ��ȡ����Ҫͬ���ķ�����
	memset(brotherID,0,sizeof(brotherID));
	if ((num = UnionReadAllMngSvrBrothersOfMine(brotherID,10)) < 0)	
	{
		UnionUserErrLog("in UnionRegFileOperationToFileDataSynJnl:: UnionReadAllMngSvrBrothersOfMine!\n");
		return(num);
	}
	for (index = 0; index < num; index++)
	{
		memset(&tMngSvrFileDataSynJnl,0,sizeof(tMngSvrFileDataSynJnl));
		
		strcpy(tMngSvrFileDataSynJnl.mngSvrID,brotherID[index]);
		strcpy(tMngSvrFileDataSynJnl.tableName,tableName);
		strcpy(tMngSvrFileDataSynJnl.dirName,dirName);
		strcpy(tMngSvrFileDataSynJnl.fileName,fileName);
		tMngSvrFileDataSynJnl.fileType = fileType;
		tMngSvrFileDataSynJnl.Flag = 0;
		
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionFormRecStrFromMngSvrFileDataSynJnlRec(&tMngSvrFileDataSynJnl,recStr,sizeof(recStr))) < 0)
		{
			UnionUserErrLog("in UnionRegFileOperationToFileDataSynJnl:: UnionInsertObjectRecordWithoutSynchronize!\n");
			continue;
		}
			
		if ((ret = UnionInsertObjectRecordWithoutSynchronize("mngSvrFileDataSynJnl",recStr,lenOfRecStr)) < 0)
		{
			UnionUserErrLog("in UnionRegFileOperationToFileDataSynJnl:: UnionInsertObjectRecordWithoutSynchronize!\n");
		}
		else
		{
			successNum++;
			UnionLog("in UnionRegFileOperationToFileDataSynJnl:: UnionInsertObjectRecordWithoutSynchronize tableName=[%s] [%04d][%s] OK!\n",tableName,lenOfRecStr,recStr);
		}
	}
	
	return(successNum);
}

