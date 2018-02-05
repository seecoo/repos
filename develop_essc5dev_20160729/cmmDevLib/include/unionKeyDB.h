//	Wolfgang Wang
//	2003/05/01

#ifndef _UnionKeyDB_
#define _UnionKeyDB_

typedef char		TUnionDate[8+1];	// YYYYMMDD

// 获得密钥对象的存储位置
int UnionGetKeyContainer(char *owner,char *keyApp,char *keyName,char *container);

// 获得密钥全称
int UnionGetFullKeyName(char *owner,char *keyApp,char *keyName,char *fullName);

#endif
