#ifndef _PackageConvert_H_
#define _PackageConvert_H_

// 其他报文转换为XML报文
int UnionConvertOtherPackageToXMLPackage(unsigned char *buf,int lenOfBuf,char *cliIPAddr);

// XML报文转换为其他报文
int UnionConvertXMLPackageToOtherPackage(unsigned char *buf,int sizeOfBuf);

#endif
