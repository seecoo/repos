#ifndef _UnionDBCursor_
#define _UnionDBCursor_

/*
  描述: 打开游标
  输入: recFileName - 文件名
        recordNum   - 记录数
  输出: 无
  返回: >=0 - 正确
        < 0 - 错误
*/
int UnionOpenCursor(char *recFileName, int recordNum);

/*
  描述: 从游标里取一条记录
  输入: 无
  输出: record - 记录
  返回: >=0 - 正确
        < 0 - 错误
*/
int UnionFetchOneRecord(char *record);

/*
  描述: 关闭游标
  输入: 无
  输出: 无
  返回: >=0 - 正确
        < 0 - 错误
*/
int UnionCloseCursor();

#endif
