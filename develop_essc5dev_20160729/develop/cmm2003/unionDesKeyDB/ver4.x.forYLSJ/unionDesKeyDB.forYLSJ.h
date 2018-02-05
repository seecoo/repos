// Author:	HuangBaoxin
// Date:	2009/09/27
// 银联数据新增特殊函数

#ifndef _UnionDesKeyDBforYLSJ_
#define _UnionDesKeyDBforYLSJ_

#include "unionDesKeyDB.h"

// HuangBaoxin, 2009/09/27
// 选择密钥或者old密钥
// valueType=0, 密钥
// valueType=1  old密钥
int UnionDesKeyDBOperationByValueType(TUnionKeyDBOperation operation,PUnionDesKey pdesKey, int valueType);

// HuangBaoxin, 2009/09/27
// 选择保存密钥或者old密钥
// valueType=0, 密钥
// valueType=1  old密钥
int UnionUpdateAllDesKeyFieldsInKeyDBByValueType(PUnionDesKey pkey, int valueType);

#endif
