//	Wolfgang Wang
//	2003/05/01

#ifndef _UnionKeyDB_
#define _UnionKeyDB_

typedef char		TUnionDate[8+1];	// YYYYMMDD

// �����Կ����Ĵ洢λ��
int UnionGetKeyContainer(char *owner,char *keyApp,char *keyName,char *container);

// �����Կȫ��
int UnionGetFullKeyName(char *owner,char *keyApp,char *keyName,char *fullName);

#endif
