#ifndef _UnionDBCursor_
#define _UnionDBCursor_

/*
  ����: ���α�
  ����: recFileName - �ļ���
        recordNum   - ��¼��
  ���: ��
  ����: >=0 - ��ȷ
        < 0 - ����
*/
int UnionOpenCursor(char *recFileName, int recordNum);

/*
  ����: ���α���ȡһ����¼
  ����: ��
  ���: record - ��¼
  ����: >=0 - ��ȷ
        < 0 - ����
*/
int UnionFetchOneRecord(char *record);

/*
  ����: �ر��α�
  ����: ��
  ���: ��
  ����: >=0 - ��ȷ
        < 0 - ����
*/
int UnionCloseCursor();

#endif
