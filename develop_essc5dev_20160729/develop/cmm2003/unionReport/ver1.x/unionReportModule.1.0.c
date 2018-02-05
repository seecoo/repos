// author	Wolfgang Wang
// date		2010-5-5

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"

#include "unionReportModule.h"


/*
����
	ִ��һ������һ��IC����ȫ�ļ�������
���������
	ptask			����׼������
�������
	��
����ֵ
	>=0	�ɹ�,�������Ŀ
	<0	������
*/
int UnionExcuteKeyGenerateTask(PUnionIcCardDataProcessTask ptask)
{
	int				ret;
	int				lenOfRecStr;
	int				recNum = 0;
	int				failRecNum = 0;
	int				resCode = 0;

	EXEC SQL BEGIN DECLARE SECTION;
		char	dateTime[14+1];
		char	cardID[128+1];
		char	oriFileName[128+1];
		char	cardNo[40+1];
		char	oriCardRecord[2048+1];
		EXEC SQL VAR cardNo IS STRING(40+1);
		EXEC SQL VAR oriCardRecord IS STRING(2048+1);
		int	status;
	EXEC SQL END DECLARE SECTION;

	// ׼��ִ������
	ptask->status = conIcCardDataProcessTaskStatusKeyGenerating;
	if ((ret = UnionPrepareExcuteKeyGeneratorTask(ptask)) < 0)
	{
		UnionUserErrLog("in UnionExcuteKeyGenerateTask:: UnionPrepareExcuteKeyGeneratorTask ret = [%d]!\n",ret);
		return(ret);
	}
	
	// ��ȡ����Ԥ������ɵĿ���¼	
	memset(dateTime,0,sizeof(dateTime));
	memset(cardID,0,sizeof(cardID));
	memset(oriFileName,0,sizeof(oriFileName));
		
	strcpy(dateTime,ptask->dateTime);
	strcpy(cardID,ptask->cardID);
	strcpy(oriFileName,ptask->oriFileName);
	status = conIcCardDataProcessTaskStatusDataPreDealOK;
	
	UnionProgramerLog("in UnionExcuteKeyGenerateTask:: dateTime = [%s]\n",dateTime);
	UnionProgramerLog("in UnionExcuteKeyGenerateTask:: cardID = [%s]\n",cardID);
	UnionProgramerLog("in UnionExcuteKeyGenerateTask:: oriFileName = [%s]\n",oriFileName);
	UnionProgramerLog("in UnionExcuteKeyGenerateTask:: status = [%d]\n",status);

	EXEC SQL DECLARE keyGenerateCur CURSOR FOR SELECT
		trim(uncardNo),trim(unoriCardRecord)
		FROM iccarddataprocesstaskdetail 
		WHERE ( undateTime = :dateTime AND uncardID = :cardID AND unoriFileName = :oriFileName AND unstatus = :status )
		ORDER BY uncardNo;

	EXEC SQL OPEN keyGenerateCur;
	if ((ret = 0 - abs(sqlca.sqlcode)) < 0)
	{
		UnionUserErrLog("in UnionExcuteKeyGenerateTask:: open keyGenerateCur sqlca.sqlcode = [%d]!\n",sqlca.sqlcode);
		return(ret);
	}

	// ���������¼
	while(1) 
	{
		// ��ȡһ������¼
		memset(cardNo,0,sizeof(cardNo));
		memset(oriCardRecord,0,sizeof(oriCardRecord));
		
		EXEC SQL FETCH keyGenerateCur INTO
			:cardNo,:oriCardRecord;
		if ((ret = 0 - abs(sqlca.sqlcode)) < 0)
		{
			if (ret == -1403)	// û�м�¼��
			{
				UnionAuditLog("in UnionExcuteKeyGenerateTask:: EXEC SQL FETCH CURSOR OF icCardDataProcessTaskDetail,SQLCODE == 1403,recNum = [%d],i.e no record found any more!\n",recNum);
				ret = 0;
				break;
			}
			UnionCloseDatabaseBySpecErrCode(sqlca.sqlcode);
			UnionUserErrLog("in UnionExcuteKeyGenerateTask:: fetch cursor sqlca.sqlcode = [%d] recNum = [%d]!\n",sqlca.sqlcode,recNum);
			goto errExit;
		}
		
		recNum++;
		// ��������¼Ϊ��ǰ��¼
		if ((ret = UnionSetCurrentFileRecStr(oriCardRecord,lenOfRecStr=strlen(oriCardRecord))) < 0)
		{
			UnionUserErrLog("in UnionExcuteKeyGenerateTask:: UnionSetCurrentFileRecStr [%04d][%s] ret = [%d]\n",lenOfRecStr,oriCardRecord,ret);
			goto errExit;
		}
		// ���õ�ǰ����
		UnionSetCurrentCardNo(cardNo);
		// ��KMC���н���
		if ((ret = UnionDPCommWithKMC()) < 0)
		{
			UnionUserErrLog("in UnionExcuteKeyGenerateTask:: UnionDPCommWithKMC for [%s] of [%s-%s-%s] ret = [%d]\n",cardNo,ptask->cardID,ptask->dateTime,ptask->oriFileName,ret);
			goto errExit;
		}
		
		ptask->resCode = 0;
		// ���µ�ǰ���Ĵ�����ϸ��״̬Ϊ��ȫ����ɹ�
		if ((ret = UnionSetDataProcessTaskDetailRecStatus(ptask,cardNo,conIcCardDataProcessTaskStatusKeyGenerated)) < 0)
		{
			UnionUserErrLog("in UnionExcuteKeyGenerateTask:: UnionSetDataProcessTaskDetailRecStatus [%04d][%s] ret = [%d]\n",lenOfRecStr,oriCardRecord,ret);
			goto errExit;
		}
		continue;
errExit:
		resCode = ret;
		ptask->resCode = resCode;
		failRecNum++;
		// ���µ�ǰ���Ĵ�����ϸ��״̬Ϊ��ȫ����ʧ��
		UnionSetDataProcessTaskDetailRecStatus(ptask,cardNo,conIcCardDataProcessTaskStatusGenerationFailed);
		// �ж��Ƿ�����������
		if (!UnionIsContinueTaskWhenDataProcessFail(ptask,conIcCardDataProcessActionListConKeyGeneration))
			break;
		continue;	// ��������
	}
	EXEC SQL CLOSE keyGenerateCur;
	
	ptask->succeedRecNum = (recNum - failRecNum);
	ptask->failedRecNum = failRecNum;
	
	if ((ret == 0) && (recNum > 0) && (failRecNum == 0))
	{
		ptask->status = conIcCardDataProcessTaskStatusKeyGenerated;
		return(UnionFinishExcuteKeyGeneratorTaskSuccessfully(ptask));	// ������״̬Ϊ��ȫ����ɹ�
	}
	else if ((ret == 0) && (recNum == 0) && (failRecNum == 0))
	{
		ptask->status = conIcCardDataProcessTaskStatusKeyGenerateTaskNoMatchRecFound;
		return(UnionFinishExcuteKeyGeneratorTaskSuccessfully(ptask));
	}
	else if ((ret == 0) && ((recNum - failRecNum) > 0) && (failRecNum > 0))
	{
		ptask->status = conIcCardDataProcessTaskStatusGenerationPartSucceed;
		return(UnionFinishExcuteKeyGeneratorTaskSuccessfully(ptask));	// ������״̬Ϊ��ȫ����ɹ�
	}
	else
	{
		ptask->status = conIcCardDataProcessTaskStatusGenerationFailed;
		return(UnionFinishExcuteKeyGeneratorTaskFailure(ptask,ptask->resCode));	// ������״̬Ϊ��ȫ����ʧ��
	}	
}

/*
����
	��������IC����ȫ�ļ������ʧ�ܼ�¼
���������
	ptask			����׼������
�������
	��
����ֵ
	>=0	�ɹ�,�������Ŀ
	<0	������
*/
int UnionRedoKeyGenerateTask(PUnionIcCardDataProcessTask ptask)
{
	int				ret;
	int				lenOfRecStr;
	int				recNum = 0;
	int				failRecNum = 0;
	int				resCode = 0;

	EXEC SQL BEGIN DECLARE SECTION;
		char	dateTime[14+1];
		char	cardID[128+1];
		char	oriFileName[128+1];
		char	cardNo[40+1];
		char	oriCardRecord[2048+1];
		EXEC SQL VAR cardNo IS STRING(40+1);
		EXEC SQL VAR oriCardRecord IS STRING(2048+1);
		int	status1;
		int	status2;
	EXEC SQL END DECLARE SECTION;

	// ׼��ִ������
	ptask->status = conIcCardDataProcessTaskStatusKeyGenerateRedoing;
	if ((ret = UnionPrepareExcuteKeyGeneratorTask(ptask)) < 0)
	{
		UnionUserErrLog("in UnionRedoKeyGenerateTask:: UnionPrepareExcuteKeyGeneratorTask ret = [%d]!\n",ret);
		return(ret);
	}
	
	// ��ȡ����Ԥ������ɵĿ���¼	
	memset(dateTime,0,sizeof(dateTime));
	memset(cardID,0,sizeof(cardID));
	memset(oriFileName,0,sizeof(oriFileName));
		
	strcpy(dateTime,ptask->dateTime);
	strcpy(cardID,ptask->cardID);
	strcpy(oriFileName,ptask->oriFileName);
	status1 = conIcCardDataProcessTaskStatusGenerationFailed;
	status2 = conIcCardDataProcessTaskStatusKeyGenerateRedoFailed;
	
	UnionProgramerLog("in UnionRedoKeyGenerateTask:: dateTime = [%s]\n",dateTime);
	UnionProgramerLog("in UnionRedoKeyGenerateTask:: cardID = [%s]\n",cardID);
	UnionProgramerLog("in UnionRedoKeyGenerateTask:: oriFileName = [%s]\n",oriFileName);

	EXEC SQL DECLARE redoKeyGenerateCur CURSOR FOR SELECT
		trim(uncardNo),trim(unoriCardRecord)
		FROM iccarddataprocesstaskdetail 
		WHERE ( undateTime = :dateTime AND uncardID = :cardID AND unoriFileName = :oriFileName 
		AND ( unstatus = :status1 OR unstatus = :status2 ) ) 
		ORDER BY uncardNo;

	EXEC SQL OPEN redoKeyGenerateCur;
	if ((ret = 0 - abs(sqlca.sqlcode)) < 0)
	{
		UnionUserErrLog("in UnionRedoKeyGenerateTask:: open redoKeyGenerateCur sqlca.sqlcode = [%d]!\n",sqlca.sqlcode);
		return(ret);
	}

	// ���������¼
	while(1) 
	{
		// ��ȡһ������¼
		memset(cardNo,0,sizeof(cardNo));
		memset(oriCardRecord,0,sizeof(oriCardRecord));
		
		EXEC SQL FETCH redoKeyGenerateCur INTO
			:cardNo,:oriCardRecord;
		if ((ret = 0 - abs(sqlca.sqlcode)) < 0)
		{
			if (ret == -1403)	// û�м�¼��
			{
				UnionAuditLog("in UnionRedoKeyGenerateTask:: EXEC SQL FETCH CURSOR OF icCardDataProcessTaskDetail,SQLCODE == 1403,recNum = [%d],i.e no record found any more!\n",recNum);
				ret = 0;
				break;
			}
			UnionCloseDatabaseBySpecErrCode(sqlca.sqlcode);
			UnionUserErrLog("in UnionRedoKeyGenerateTask:: fetch cursor sqlca.sqlcode = [%d] recNum = [%d]!\n",sqlca.sqlcode,recNum);
			goto errExit;
		}
		
		recNum++;
		// ��������¼Ϊ��ǰ��¼
		if ((ret = UnionSetCurrentFileRecStr(oriCardRecord,lenOfRecStr=strlen(oriCardRecord))) < 0)
		{
			UnionUserErrLog("in UnionRedoKeyGenerateTask:: UnionSetCurrentFileRecStr [%04d][%s] ret = [%d]\n",lenOfRecStr,oriCardRecord,ret);
			goto errExit;
		}
		// ���õ�ǰ����
		UnionSetCurrentCardNo(cardNo);
		// ��KMC���н���
		if ((ret = UnionDPCommWithKMC()) < 0)
		{
			UnionUserErrLog("in UnionRedoKeyGenerateTask:: UnionDPCommWithKMC for [%s] of [%s-%s-%s] ret = [%d]\n",cardNo,ptask->cardID,ptask->dateTime,ptask->oriFileName,ret);
			goto errExit;
		}
		
		ptask->resCode = 0;
		// ���µ�ǰ���Ĵ�����ϸ��״̬Ϊ��ȫ����ɹ�
		if ((ret = UnionSetDataProcessTaskDetailRecStatus(ptask,cardNo,conIcCardDataProcessTaskStatusKeyGenerated)) < 0)
		{
			UnionUserErrLog("in UnionRedoKeyGenerateTask:: UnionSetDataProcessTaskDetailRecStatus [%04d][%s] ret = [%d]\n",lenOfRecStr,oriCardRecord,ret);
			goto errExit;
		}
		continue;
errExit:
		resCode = ret;
		ptask->resCode = resCode;
		failRecNum++;
		// ���µ�ǰ���Ĵ�����ϸ��״̬Ϊ��ȫ����ʧ��
		UnionSetDataProcessTaskDetailRecStatus(ptask,cardNo,conIcCardDataProcessTaskStatusKeyGenerateRedoFailed);
		// �ж��Ƿ�����������
		if (!UnionIsContinueTaskWhenDataProcessFail(ptask,conIcCardDataProcessActionListConKeyGeneration))
			break;
		continue;	// ��������
	}
	EXEC SQL CLOSE redoKeyGenerateCur;
	
	ptask->succeedRecNum = (recNum - failRecNum);
	ptask->failedRecNum = failRecNum;
	
	if ((ret == 0) && (recNum > 0) && (failRecNum == 0))
	{
		ptask->status = conIcCardDataProcessTaskStatusKeyGenerated;
		return(UnionFinishExcuteKeyGeneratorTaskSuccessfully(ptask));	// ������״̬Ϊ��ȫ����ɹ�
	}
	else if ((ret == 0) && (recNum == 0) && (failRecNum == 0))
	{
		ptask->status = conIcCardDataProcessTaskStatusRedoKeyGenerateTaskNoMatchRecFound;
		return(UnionFinishExcuteKeyGeneratorTaskSuccessfully(ptask));
	}
	else
	{
		ptask->status = conIcCardDataProcessTaskStatusKeyGenerateRedoFailed;
		return(UnionFinishExcuteKeyGeneratorTaskFailure(ptask,ptask->resCode));	// ������״̬Ϊ��ȫ����ʧ��
	}	
}
