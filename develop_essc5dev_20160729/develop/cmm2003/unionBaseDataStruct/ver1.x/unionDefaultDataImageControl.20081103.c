//	Author: ChenJiaMei
//	Date: 2008-8-6

#include <stdio.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>
#include <vcl.h>
#include <Filectrl.hpp>
#endif

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionRecFile.h"
#include "unionErrCode.h"
#include "unionComplexDBObjectFileName.h"
#include "unionComplexDBCommon.h"
#include "unionComplexDBRecordValue.h"
#include "unionRec0.h"
#include "unionDataImageInMemory.h"
#ifndef _WIN32
#include "unionREC.h"
#else
#include "unionRECVar.h"
#endif

// �ж��Ƿ�һ�������ӳ����ص�ӳ����
/*
�������
	objectName	����		
	key		�ؼ���
�������
	��
����ֵ
	1		����
	0		������
*/
int UnionIsUseDataImageInMemory(char *objectName,char *key __attribute__((unused)))
{
	if (strcmp(objectName,"unionREC") == 0)
		return(0);

	/*
	if (strcmp(objectName,"token") == 0 )
		return (1);

	if (strcmp(objectName,"slot")==0 )
		return (1);	

	if (strcmp(objectName,"token") == 0 )
		return (1);

	if (strcmp(objectName,"tokenObject") == 0 )
		return (1);

	if (strcmp(objectName,"tokenObjectAttr") == 0 )
		return (1);
	*/					
	return( 0 );
}
