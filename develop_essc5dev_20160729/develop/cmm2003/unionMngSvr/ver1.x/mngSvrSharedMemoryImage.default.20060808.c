// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2008/11/10
// Version:	1.0

// �ļ�������

#include <stdio.h>
#include <string.h>

#include "mngSvrServicePackage.h"
#include "unionErrCode.h"
#include "mngSvrCommProtocol.h"
#include "UnionLog.h"
#include "unionResID.h"
#include "unionTableField.h"

/* ����
	�����ڴ�ӳ�����������
�������
	handle		socket���
	resID		��Դ��ʶ
	serviceID	������
	reqStr	����
	lenOfReqStr	���󴮳���
	sizeOfResStr	��Ӧ������Ĵ�С
�������
	resStr		��Ӧ��
	fileRecved	�Ƿ����ļ����յ�
����ֵ
	>=0	�ɹ�
	<0	ʧ�ܣ�������
*/
int UnionExcuteMngSvrSharedMemoryImageService(int handle,int resID,int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	UnionProgramerLog("in UnionExcuteMngSvrSharedMemoryImageService:: invalid fileSvr command [%d]\n",serviceID);
	return(errCodeEsscMDL_InvalidService);
}
