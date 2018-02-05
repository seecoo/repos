#ifndef _unionFileDeal_
#define _unionFileDeal_

/*
 * ʹ��tar������һ���ļ�
 * ���룺packName - �����ƣ�fileName - �ļ�����(��·��)
 * �������
 * ���أ�>=0,�ɹ�����<0,ʧ��
 */
int  UnionPackOneFileWithTarCmd(char *packName, char *fileName);

/*
 * ʹ��tar������һ��Ŀ¼
 * ���룺packName - �����ƣ�dir - Ŀ¼����
 * �������
 * ���أ�>=0,�ɹ�����<0,ʧ��
 */
int  UnionPackOneDirectoryWithTarCmd(char *packName, char *dir);

/*
 * ʹ��tar������
 * ���룺packName - ������; dirName - ���Ŀ¼; maxNum - fileList�����������
 * �����fileList - �ļ��嵥
 * ���أ�>=0,�ɹ�����<0,ʧ��
 */
int  UnionUnPackWithTarCmd(char *packName, char *dirName, int maxNum, char fileList[][128+1]);

#endif

