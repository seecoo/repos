#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "unionErrCode.h"
#include "unionResID.h"
#include "UnionLog.h"

#ifdef _WIN32
#include "unionRECVar.h"
#else
#include "unionREC.h"
#include "UnionTask.h"
#endif

int UnionExcuteSpecAppSuffixOperation(int resID,int serviceID,char *reqStr,int lenOfReqStr);

int UnionExcuteSuffixOperation(int resID,int serviceID,char *reqStr,int lenOfReqStr)
{
	switch (serviceID)
	{
		case	conResCmdQuerySpecRec:		// ��ȡ��¼
		case	conResCmdQueryAllRec:		// ��ѯ���м�¼
		case	conResCmdQueryWithCondition:	// ��ѯ�����������м�¼
			return(0);
		default:
			break;
	}
	switch (resID)
	{
		//��־ע���
		case	conResIDLogTBL:	
			return(UnionReloadLogFileTBL());
		//����ע���
		case	conResIDTaskTBL:
			return UnionReloadTaskTBL();
		//�ͻ���������
		case	conResIDREC:
			return UnionReloadREC();
		//��Կ���ݷ�����
		case	conResIDKeyDBBackuper:
			return UnionLoadKeyDBBackupServerIntoMemory();
		//��Ϣ������
		case	conResIDMsgBuf:
			return UnionReloadMsgBufDef();
			/*
			UnionDisconnectMsgBufMDL();
			return UnionReconnectMsgBufMDLAnyway();
			*/
		//��Կ�������Ļ���
		case	conResIDKDBSvrBuf:
			return UnionReloadKDBSvrBufDef();
			
		//�����Ϣ������
		case	conResIDTransSpierBuf:
			return UnionReloadTransSpierBufDef();
		/*
		//ͨѶ�ͻ��˱�
		case	conResIDCommConf:
		//����Ա��
		case	conResIDOperatorTBL:
		//PK��
		case	conResIDPKDB:
		*/		
		// ����
		case	conResIDLockTBL:
			return UnionReloadLockTBL();
		//���������
		case	conResIDHsmError:
			return UnionReloadSoftErrCodeTBL();
		//ESSC����
		case	conResIDError:
			return UnionReloadErrCodeTranslater();
		/*
		// ��֪���ð����ĸ���
		//�����������
		case	conResIDHsmGrp:
			return UnionAutoLoadSJL06IntoSJL06MDL();
		//��ؿͻ��˿��Ʊ�
		case	conResIDSpierClientTBL:
			return UnionReloadEsscResSpierClientTBL();
		*/
		default:
			return(UnionExcuteSpecAppSuffixOperation(resID,serviceID,reqStr,lenOfReqStr));
			/*
			// essc
			//Ӧ������
			case	conResIDAppConf:
				return UnionReloadAppTable();
			//������Կ�������������
			case	conResIDLocalKMSvr:
				return UnionReloadLocalKMSvr();
			//Զ����Կ�������������
			case	conResIDRemoteKMSvr:
				return UnionReloadRemoteKMSvr();
			*/
			//return 0;
	}
}
