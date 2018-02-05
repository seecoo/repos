// 2008/7/26
// Wolfang Wang

#ifndef _simuMngSvrLocally_
#define _simuMngSvrLocally_

// 2009/8/1,Wolfgang Wang added
char *UnionGetCurrentOperationTellerNo();

// �ж�һ����Դ�Ƿ��Ǳ�����Դ
int UnionIsReservedRes(int resID);

// ��ȡ��ǰ��Դ��
int UnionGetCurrentResID();

// 2009/3/19����
char *UnionGetCurrentRemoteMngSvrName();

// ��ȡ��ǰ�����
int UnionGetCurrentServiceID();

// ���õ�ǰ�����
int UnionSetCurrentServiceID(int serviceID);

// �ж��Ƿ��Ǳ��ط���mngSvr
int UnionIsReadMngSvrDataLocally();

// ���ñ��ط���mngSvr
void UnionSetSimuMngSvrLocally();

// ��MngSvrͨѶ
// resName,serviceID,reqStr,lenOfReqStr������������ֱ��Ӧ����Ա��ʶ����Դ�������ʶ���������ݣ��������ݳ���
// sizeOfResStr�������������ָʾ�˽������ݵĻ������Ĵ�С
// resStr,���������������Ӧ����
// recvFileName,��������������������NULL����ʾ�������˷�����һ���ļ������ļ��ڱ��صĴ洢���ƣ������NULL����ʾû���ļ�������
// ����ֵ��resStr�����ݵĳ���
int UnionExcuteDBSvrOperation(char *resName,int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved);

// ��MngSvrͨѶ
// tellerNo,resID,serviceID,reqStr,lenOfReqStr������������ֱ��Ӧ����Ա��ʶ����Դ�������ʶ���������ݣ��������ݳ���
// sizeOfResStr�������������ָʾ�˽������ݵĻ������Ĵ�С
// resStr,���������������Ӧ����
// recvFileName,��������������������NULL����ʾ�������˷�����һ���ļ������ļ��ڱ��صĴ洢���ƣ������NULL����ʾû���ļ�������
// ����ֵ��resStr�����ݵĳ���
int UnionReadMngSvrDataLocally(int handle,char *tellerNo,int resID,int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved);

// �Ӽ�¼"��1=��ֵ|��2=��ֵ|��3=��ֵ|��"���ָ�ʽ���У���ֳ��ؼ���ֵ"�ؼ���1=��ֵ|��"
int UnionGetPrimaryKeyFldFromObjectRecord(char *resName,char *record,char *priFld);

/*
����	
	�Զ��ڼ�¼�󸽼Ӽ�¼��������
�������
	recStr		��¼��
	lenOfRecStr	��¼����
	sizeOfRecStr	��¼�������С
�������
	recStr	��¼��
����ֵ
	>=0	�������´���ֵ����
	<0	������
*/
int UnionAutoAppendDBRecInputAttr(char *recStr,int lenOfRecStr,int sizeOfRecStr);

/*
����	
	�Զ��ڼ�¼�󸽼Ӽ�¼�޸�����
�������
	recStr		��¼��
	lenOfRecStr	��¼����
	sizeOfRecStr	��¼�������С
�������
	recStr	��¼��
����ֵ
	>=0	�������´���ֵ����
	<0	������
*/
int UnionAutoAppendDBRecUpdateAttr(char *recStr,int lenOfRecStr,int sizeOfRecStr);

/*
function��
        ��ȡ�����޸ĵ��ֶ������Լ�����
param:
        [IN]:
        resName:        ������
        record:         ���������¼��Ϣ

        [OUT]:
        modFld:         �������������޸ĵ��ֶ����Լ��ֶ�����
return:
        >=0:            ���������޸ĵ��ֶ����Լ��ֶ����Ƶĳ���
        < 0:            ��������
*/
int UnionGetAllowModifyFldFromObjectRecord(char *resName, char *record, char *modFld);

#endif

