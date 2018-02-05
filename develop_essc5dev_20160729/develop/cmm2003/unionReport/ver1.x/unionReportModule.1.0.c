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
功能
	执行一个生成一个IC卡安全文件的任务
输入参数：
	ptask			数据准备任务
输出参数
	无
返回值
	>=0	成功,域的总数目
	<0	错误码
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

	// 准备执行任务
	ptask->status = conIcCardDataProcessTaskStatusKeyGenerating;
	if ((ret = UnionPrepareExcuteKeyGeneratorTask(ptask)) < 0)
	{
		UnionUserErrLog("in UnionExcuteKeyGenerateTask:: UnionPrepareExcuteKeyGeneratorTask ret = [%d]!\n",ret);
		return(ret);
	}
	
	// 读取所有预处理完成的卡记录	
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

	// 逐条处理记录
	while(1) 
	{
		// 读取一个卡记录
		memset(cardNo,0,sizeof(cardNo));
		memset(oriCardRecord,0,sizeof(oriCardRecord));
		
		EXEC SQL FETCH keyGenerateCur INTO
			:cardNo,:oriCardRecord;
		if ((ret = 0 - abs(sqlca.sqlcode)) < 0)
		{
			if (ret == -1403)	// 没有记录了
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
		// 置主机记录为当前记录
		if ((ret = UnionSetCurrentFileRecStr(oriCardRecord,lenOfRecStr=strlen(oriCardRecord))) < 0)
		{
			UnionUserErrLog("in UnionExcuteKeyGenerateTask:: UnionSetCurrentFileRecStr [%04d][%s] ret = [%d]\n",lenOfRecStr,oriCardRecord,ret);
			goto errExit;
		}
		// 设置当前卡号
		UnionSetCurrentCardNo(cardNo);
		// 与KMC进行交互
		if ((ret = UnionDPCommWithKMC()) < 0)
		{
			UnionUserErrLog("in UnionExcuteKeyGenerateTask:: UnionDPCommWithKMC for [%s] of [%s-%s-%s] ret = [%d]\n",cardNo,ptask->cardID,ptask->dateTime,ptask->oriFileName,ret);
			goto errExit;
		}
		
		ptask->resCode = 0;
		// 更新当前卡的处理明细的状态为安全处理成功
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
		// 更新当前卡的处理明细的状态为安全处理失败
		UnionSetDataProcessTaskDetailRecStatus(ptask,cardNo,conIcCardDataProcessTaskStatusGenerationFailed);
		// 判断是否出错继续处理
		if (!UnionIsContinueTaskWhenDataProcessFail(ptask,conIcCardDataProcessActionListConKeyGeneration))
			break;
		continue;	// 继续处理
	}
	EXEC SQL CLOSE keyGenerateCur;
	
	ptask->succeedRecNum = (recNum - failRecNum);
	ptask->failedRecNum = failRecNum;
	
	if ((ret == 0) && (recNum > 0) && (failRecNum == 0))
	{
		ptask->status = conIcCardDataProcessTaskStatusKeyGenerated;
		return(UnionFinishExcuteKeyGeneratorTaskSuccessfully(ptask));	// 置任务状态为安全处理成功
	}
	else if ((ret == 0) && (recNum == 0) && (failRecNum == 0))
	{
		ptask->status = conIcCardDataProcessTaskStatusKeyGenerateTaskNoMatchRecFound;
		return(UnionFinishExcuteKeyGeneratorTaskSuccessfully(ptask));
	}
	else if ((ret == 0) && ((recNum - failRecNum) > 0) && (failRecNum > 0))
	{
		ptask->status = conIcCardDataProcessTaskStatusGenerationPartSucceed;
		return(UnionFinishExcuteKeyGeneratorTaskSuccessfully(ptask));	// 置任务状态为安全处理成功
	}
	else
	{
		ptask->status = conIcCardDataProcessTaskStatusGenerationFailed;
		return(UnionFinishExcuteKeyGeneratorTaskFailure(ptask,ptask->resCode));	// 置任务状态为安全处理失败
	}	
}

/*
功能
	重做生成IC卡安全文件任务的失败记录
输入参数：
	ptask			数据准备任务
输出参数
	无
返回值
	>=0	成功,域的总数目
	<0	错误码
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

	// 准备执行任务
	ptask->status = conIcCardDataProcessTaskStatusKeyGenerateRedoing;
	if ((ret = UnionPrepareExcuteKeyGeneratorTask(ptask)) < 0)
	{
		UnionUserErrLog("in UnionRedoKeyGenerateTask:: UnionPrepareExcuteKeyGeneratorTask ret = [%d]!\n",ret);
		return(ret);
	}
	
	// 读取所有预处理完成的卡记录	
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

	// 逐条处理记录
	while(1) 
	{
		// 读取一个卡记录
		memset(cardNo,0,sizeof(cardNo));
		memset(oriCardRecord,0,sizeof(oriCardRecord));
		
		EXEC SQL FETCH redoKeyGenerateCur INTO
			:cardNo,:oriCardRecord;
		if ((ret = 0 - abs(sqlca.sqlcode)) < 0)
		{
			if (ret == -1403)	// 没有记录了
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
		// 置主机记录为当前记录
		if ((ret = UnionSetCurrentFileRecStr(oriCardRecord,lenOfRecStr=strlen(oriCardRecord))) < 0)
		{
			UnionUserErrLog("in UnionRedoKeyGenerateTask:: UnionSetCurrentFileRecStr [%04d][%s] ret = [%d]\n",lenOfRecStr,oriCardRecord,ret);
			goto errExit;
		}
		// 设置当前卡号
		UnionSetCurrentCardNo(cardNo);
		// 与KMC进行交互
		if ((ret = UnionDPCommWithKMC()) < 0)
		{
			UnionUserErrLog("in UnionRedoKeyGenerateTask:: UnionDPCommWithKMC for [%s] of [%s-%s-%s] ret = [%d]\n",cardNo,ptask->cardID,ptask->dateTime,ptask->oriFileName,ret);
			goto errExit;
		}
		
		ptask->resCode = 0;
		// 更新当前卡的处理明细的状态为安全处理成功
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
		// 更新当前卡的处理明细的状态为安全处理失败
		UnionSetDataProcessTaskDetailRecStatus(ptask,cardNo,conIcCardDataProcessTaskStatusKeyGenerateRedoFailed);
		// 判断是否出错继续处理
		if (!UnionIsContinueTaskWhenDataProcessFail(ptask,conIcCardDataProcessActionListConKeyGeneration))
			break;
		continue;	// 继续处理
	}
	EXEC SQL CLOSE redoKeyGenerateCur;
	
	ptask->succeedRecNum = (recNum - failRecNum);
	ptask->failedRecNum = failRecNum;
	
	if ((ret == 0) && (recNum > 0) && (failRecNum == 0))
	{
		ptask->status = conIcCardDataProcessTaskStatusKeyGenerated;
		return(UnionFinishExcuteKeyGeneratorTaskSuccessfully(ptask));	// 置任务状态为安全处理成功
	}
	else if ((ret == 0) && (recNum == 0) && (failRecNum == 0))
	{
		ptask->status = conIcCardDataProcessTaskStatusRedoKeyGenerateTaskNoMatchRecFound;
		return(UnionFinishExcuteKeyGeneratorTaskSuccessfully(ptask));
	}
	else
	{
		ptask->status = conIcCardDataProcessTaskStatusKeyGenerateRedoFailed;
		return(UnionFinishExcuteKeyGeneratorTaskFailure(ptask,ptask->resCode));	// 置任务状态为安全处理失败
	}	
}
