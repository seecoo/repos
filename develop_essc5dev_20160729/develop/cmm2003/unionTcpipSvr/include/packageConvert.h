#ifndef _PackageConvert_H_
#define _PackageConvert_H_

// ��������ת��ΪXML����
int UnionConvertOtherPackageToXMLPackage(unsigned char *buf,int lenOfBuf,char *cliIPAddr);

// XML����ת��Ϊ��������
int UnionConvertXMLPackageToOtherPackage(unsigned char *buf,int sizeOfBuf);

#endif
