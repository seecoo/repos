//	Wolfgang Wang, 2008/9/28

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "UnionStr.h"
#include "unionServicePackage.h"
#include "unionErrCode.h"
#include "UnionLog.h"
#include "UnionProc.h"
#ifndef _WIN32
#include "unionREC.h"
#else
#include "unionRECVar.h"
#endif

// ��ȡ����˵��
/*
�������
	serviceID	�������
�������
	��
����ֵ
	����˵����ָ��
	������ָ��
*/
char *UnionFindCmmPackServiceRemark(int serviceID)
{
	char	varName[128];
	
	sprintf(varName,"cmmPackServiceNameOf%03d",serviceID);
	return(UnionReadStringTypeRECVar(varName));
}
			
// ��ȡ���ʶ˵��
/*
�������
	tag	���ʶ
�������
	��
����ֵ
	˵����ָ��
	������ָ��
*/
char *UnionFindCmmPackFldTagRemark(int tag)
{
	char	varName[128];
	
	sprintf(varName,"cmmPackFldTagNameOf%03d",tag);
	return(UnionReadStringTypeRECVar(varName));
}

