// Author:	HuangBaoxin
// Date:	2009/09/27
// ���������������⺯��

#ifndef _UnionDesKeyDBforYLSJ_
#define _UnionDesKeyDBforYLSJ_

#include "unionDesKeyDB.h"

// HuangBaoxin, 2009/09/27
// ѡ����Կ����old��Կ
// valueType=0, ��Կ
// valueType=1  old��Կ
int UnionDesKeyDBOperationByValueType(TUnionKeyDBOperation operation,PUnionDesKey pdesKey, int valueType);

// HuangBaoxin, 2009/09/27
// ѡ�񱣴���Կ����old��Կ
// valueType=0, ��Կ
// valueType=1  old��Կ
int UnionUpdateAllDesKeyFieldsInKeyDBByValueType(PUnionDesKey pkey, int valueType);

#endif
