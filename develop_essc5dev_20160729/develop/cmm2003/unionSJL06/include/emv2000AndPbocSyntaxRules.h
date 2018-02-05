//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2012-05-25

#ifndef _Emv2000AndPbocSyntaxRules_
#define _Emv2000AndPbocSyntaxRules_

// PBOC mode='0'方式处理PAN
int UnionForm16BytesDisperseDataOfV41A(int lenOfData, char *data, char *disperseData);

/*
功能
	根据规范将数据形成16字节的离散数据
输入参数
	method：离散数据组成方法，0：EMV2000 V4.1选项A；1：EMV2000 V4.1选项B；2：PBOC2.0
	lenOfData：数据长度
	data：数据
输出参数
	disperseData：16字节的离散数据
返回值
	<0：函数执行失败，值为失败的错误码
	>0：函数执行成功，返回disperseData的长度
*/
int UnionForm16BytesDisperseData(int iMode, int lenOfData, char *data, char *disperseData);

int UnionPBOCEMVFormPlainDataBlock(int lenOfData,unsigned char *data,unsigned char *dataBlock);

int UnionPBOCEMVFormPlainDataBlockForSM4(int lenOfData,unsigned char *data,unsigned char *dataBlock);

#endif
