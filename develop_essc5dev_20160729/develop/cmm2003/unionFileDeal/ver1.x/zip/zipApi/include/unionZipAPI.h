#ifndef UNION_ZIP_API
#define UNION_ZIP_API

/*
���ܣ����ļ�(�����Ƕ���ļ�)ѹ��Ϊָ����ZIPѹ���ļ�
���룺
destZipFile: ѹ�����ɵ��ļ���
srcFileNameStrs: 
	��ѹ�����ļ������Ҫѹ������ļ���Ҫ��ÿ���ļ��ÿո����������Ҫѹ��Ŀ¼(�µ������ļ�),����Ŀ¼ǰ��"-r"����
	���ӣ�UnionZip("aaa.zip","aaa bbb -r ccc"); �ǽ��ļ�aaa,bbb��Ŀ¼ccc(��ccc�µ������ļ�)ѹ������aaa.zip
*/
int UnionZip(char *destZipFile,char *srcFileNameStrs);

/*
����: ���ļ���ѹ��ָ��·��
����:
	ZipFileName: ����ѹ���ļ���
	destDir�� ��ѹ���ŵ�·��
*/
int UnionUnZip(char *ZipFileName,char *destDir);

#endif

