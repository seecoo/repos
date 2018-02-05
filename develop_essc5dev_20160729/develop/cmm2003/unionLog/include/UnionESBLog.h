// Author:	lusj
// Date:	20160322
//����:��ݸũ��ESBϵͳ������־��ʽ


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <zlog.h>
#include <LOGS.h>


unsigned long get_file_size(const char *LogFileName);
int	BackupLogFile(const char *LogFileName);
void UnionESBStartLog(char *CONSUMER_ID,char	*SERVICE_CODE,char *CONSUMER_SEQ_NO);
void UnionESBEndLog(int result_flag,char *CONSUMER_ID,char	*SERVICE_CODE,char *CONSUMER_SEQ_NO);
void UnionESBErrLog(char *err_code,char *err_MSG,char *CONSUMER_ID,char	*SERVICE_CODE,char *CONSUMER_SEQ_NO);
void UnionPrintLogToLogFile(char *tital);
void UnionSendESBTranscationLog(unsigned char *TranceLog,int lenOfTranceLog);
void UnionRecESBTranscationLog(unsigned char *TranceLog,int lenOfTranceLog);
void UnionPrintTranscationLogToLogFile(char *tital);

