// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2006/08/08
// Version:	1.0

#ifndef _mngSvrConfFileName_
#define _mngSvrConfFileName_

// ���mngSvr�ͻ��������ļ���Ŀ¼
char *UnionGetMngSvrClientConfFileDir();

// ���mngSvr�ͻ���ö�����Ͷ����ļ���
char *UnionGetEnumConfFileNameByTableAlais(char *tableAlais);

// ���mngSvr�ͻ�����ͼ�����ļ���
char *UnionGetTBLQueryConfFileNameByTableAlais(char *tableAlais);

// ���mngSvr�ͻ��˼�¼˫���˵��ļ���
char *UnionGetDoubleClickRecMenuFileNameByTableAlais(char *tableAlais);

// ���mngSvr�ͻ��˼�¼�Ҽ��˵��ļ���
char *UnionGetRecPopupMenuFileNameByTableAlais(char *tableAlais);

// ���mngSvr�ͻ��˽����Ҽ��˵��ļ���
char *UnionGetFormPopupMenuFileNameByTableAlais(char *tableAlais);

// ���mngSvr����˱����ļ���
char *UnionGetComplexDBTBLDefFileNameByTableName(char *tableName);

// ���mngSvr��������ݿ⽨���ļ���
char *UnionGetDBTBLCreateSQLFileNameByTableName(char *tableName);

// ������˵�����
char *UnionGetMngSvrClientMainMenuFileName();

// ��ö����˵�����
char *UnionGetMngSvrClient2LevelMenuFileName(char *nameOf2LevelMenu);

#endif
