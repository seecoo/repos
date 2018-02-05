#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "UnionLog.h"
#include "unionErrCode.h"
#include "UnionStr.h"
#include "unionPackage.h"

#ifdef _MXML_
#include "unionMXML.h"
#else
#include "unionXML.h"
#endif
#include "unionXMLPackage.h"

TUnionXMLCtxt	gunionDefaultXMLCtxt = {0};
int		gunionDefaultInit = 0;
TUnionXMLCtxt	gunionRequestXMLCtxt = {0};
int		gunionRequestInit = 0;
TUnionXMLCtxt	gunionResponseXMLCtxt = {0};
int		gunionResponseInit = 0;
TUnionXMLCtxt	gunionRequestRemoteXMLCtxt = {0};
int		gunionRequestRemoteInit = 0;
TUnionXMLCtxt	gunionResponseRemoteXMLCtxt = {0};
int		gunionResponseRemoteInit = 0;

TUnionPackageType	gunionPackageType = PACKAGE_TYPE_XML;
TUnionPackage		gunionRequestPackageCtxt;
TUnionPackage		gunionResponsePackageCtxt;

TUnionPackage		gunionRequestRemotePackageCtxt;
TUnionPackage		gunionResponseRemotePackageCtxt;

TUnionXMLPackageHead	gunionXMLPackageHead;

static char	gunionErrRemark[XML_PACK_SIZE*5];

void UnionSetPackageType(TUnionPackageType packageType)
{
	gunionPackageType = packageType;
	return;
}

TUnionPackageType UnionGetPackageType()
{
	return(gunionPackageType);
}
// 初始化XML包
int UnionInitXMLPackage(char *fileName,char *xmlBuf,int lenOfBuf)
{
	int	ret;
	
	UnionResetXMLPackage();
	
	if ((fileName != NULL) && (strlen(fileName) > 0))
	{
		if ((ret =  UnionXMLInitFromFile(&gunionDefaultXMLCtxt, fileName)) < 0)
		{
			UnionUserErrLog("in UnionInitXMLPackage:: UnionXMLInitFromFile[%s]\n",fileName);
			goto exit_tag;
		}
	}
	else if ((xmlBuf != NULL) && (strlen(xmlBuf) > 0))
	{
		if ((ret =  UnionXMLInitFromBuf(&gunionDefaultXMLCtxt, xmlBuf)) < 0)
		{
			UnionUserErrLog("in UnionInitXMLPackage:: UnionXMLInitFromBuf[%s]!\n",xmlBuf);
			goto exit_tag;
		}
	}
	else
	{
		if ((ret =  UnionXMLInit(&gunionDefaultXMLCtxt, defUnionXMLRootName, defUnionXMLEncoding)) < 0)
		{
			UnionUserErrLog("in UnionInitXMLPackage:: UnionXMLInit!\n");
			goto exit_tag;
		}
	}

exit_tag:
	gunionDefaultInit = 1;
	return(ret);
}


// 初始化请求XML包
int UnionInitRequestXMLPackage(char *fileName,char *xmlBuf,int lenOfBuf)
{
	int	ret = 0;
	
	if (gunionPackageType == PACKAGE_TYPE_V001)
	{
		if (!gunionRequestInit)
			gunionRequestPackageCtxt.bufSize = 0;
		
		if ((xmlBuf != NULL) && (strlen(xmlBuf) > 0))
		{
			if ((ret = UnionUnpackPackage((unsigned char *)xmlBuf,lenOfBuf,&gunionRequestPackageCtxt)) < 0)
			{
				UnionUserErrLog("in UnionInitRequestXMLPackage:: UnionUnpackPackage[%d][%s]\n",lenOfBuf,xmlBuf);
				goto exit_tag;
			}
		}
		else if ((fileName != NULL) && (strlen(fileName) > 0))
		{
			UnionUserErrLog("in UnionInitRequestXMLPackage:: fileName[%s]!\n",fileName);
			return(errCodeParameter);
		}
		else
		{
			UnionPackageSetVersion(&gunionRequestPackageCtxt,PACKAGE_VERSION_001,4);
			if ((ret = UnionInitPackage(&gunionRequestPackageCtxt)) < 0)
			{
				UnionUserErrLog("in UnionInitRequestXMLPackage:: UnionUnpackPackage!\n");
				goto exit_tag;
			}
		}
	}
	else if (gunionPackageType == PACKAGE_TYPE_XML)
	{
		UnionResetRequestXMLPackage();
		
		if ((fileName != NULL) && (strlen(fileName) > 0))
		{
			if ((ret = UnionXMLInitFromFile(&gunionRequestXMLCtxt, fileName)) < 0)
			{
				UnionUserErrLog("in UnionInitRequestXMLPackage:: UnionXMLInitFromFile[%s]\n",fileName);
				goto exit_tag;
			}
		}
		else if ((xmlBuf != NULL) && (strlen(xmlBuf) > 0))
		{
			if ((ret = UnionXMLInitFromBuf(&gunionRequestXMLCtxt, xmlBuf)) < 0)
			{
				UnionUserErrLog("in UnionInitRequestXMLPackage:: UnionXMLInitFromBuf[%s]!\n",xmlBuf);
				goto exit_tag;
			}
		}
		else
		{
			if ((ret = UnionXMLInit(&gunionRequestXMLCtxt, defUnionXMLRootName, defUnionXMLEncoding)) < 0)
			{
				UnionUserErrLog("in UnionInitRequestXMLPackage:: UnionXMLInit!\n");
				goto exit_tag;
			}
#ifdef _MXML_
		gunionRequestXMLCtxt.xmlCache->isNewXML = 0;
#endif
		}
	}

exit_tag:	
	gunionRequestInit = 1;
	return(ret);
}

// 初始化响应XML包
int UnionInitResponseXMLPackage(char *fileName,char *xmlBuf,int lenOfBuf)
{
	int	ret = 0;
	
	if (gunionPackageType == PACKAGE_TYPE_V001)
	{
		if (!gunionResponseInit)
			gunionResponsePackageCtxt.bufSize = 0;
		
		if ((xmlBuf != NULL) && (strlen(xmlBuf) > 0))
		{
			if ((ret = UnionUnpackPackage((unsigned char *)xmlBuf,lenOfBuf,&gunionResponsePackageCtxt)) < 0)
			{
				UnionUserErrLog("in UnionInitResponseXMLPackage:: UnionUnpackPackage[%d][%s]\n",lenOfBuf,xmlBuf);
				goto exit_tag;
			}
		}
		else if ((fileName != NULL) && (strlen(fileName) > 0))
		{
			UnionUserErrLog("in UnionInitResponseXMLPackage:: fileName[%s]!\n",fileName);
			return(errCodeParameter);
		}
		else
		{
			UnionPackageSetVersion(&gunionResponsePackageCtxt,PACKAGE_VERSION_001,4);
			if ((ret = UnionInitPackage(&gunionResponsePackageCtxt)) < 0)
			{
				UnionUserErrLog("in UnionInitResponseXMLPackage:: UnionUnpackPackage!\n");
				goto exit_tag;
			}
		}
	}
	else if (gunionPackageType == PACKAGE_TYPE_XML)
	{
		UnionResetResponseXMLPackage();
		
		if ((fileName != NULL) && (strlen(fileName) > 0))
		{
			if ((ret =  UnionXMLInitFromFile(&gunionResponseXMLCtxt, fileName)) < 0)
			{
				UnionUserErrLog("in UnionInitResponseXMLPackage:: UnionXMLInitFromFile[%s]\n",fileName);
				goto exit_tag;
			}
		}
		else if ((xmlBuf != NULL) && (strlen(xmlBuf) > 0))
		{
			if ((ret =  UnionXMLInitFromBuf(&gunionResponseXMLCtxt, xmlBuf)) < 0)
			{
				UnionUserErrLog("in UnionInitResponseXMLPackage:: UnionXMLInitFromBuf[%s]!\n",xmlBuf);
				goto exit_tag;
			}
		}
		else
		{
			if ((ret =  UnionXMLInit(&gunionResponseXMLCtxt, defUnionXMLRootName, defUnionXMLEncoding)) < 0)
			{
				UnionUserErrLog("in UnionInitResponseXMLPackage:: UnionXMLInit!\n");
				goto exit_tag;
			}
#ifdef _MXML_
			gunionResponseXMLCtxt.xmlCache->isNewXML = 0;
#endif
		}
	}

exit_tag:
	gunionResponseInit = 1;
	return(ret);
}

// 初始化远程请求XML包
int UnionInitRequestRemoteXMLPackage(char *fileName,char *xmlBuf,int lenOfBuf)
{
	int	ret = 0;
	
	if (gunionPackageType == PACKAGE_TYPE_V001)
	{
		if (!gunionRequestRemoteInit)
			gunionRequestRemotePackageCtxt.bufSize = 0;
		
		if ((xmlBuf != NULL) && (strlen(xmlBuf) > 0))
		{
			if ((ret =  UnionUnpackPackage((unsigned char *)xmlBuf,lenOfBuf,&gunionRequestRemotePackageCtxt)) < 0)
			{
				UnionUserErrLog("in UnionInitRequestRemoteXMLPackage:: UnionUnpackPackage[%d][%s]!\n",lenOfBuf,xmlBuf);
				goto exit_tag;
			}
		}
		else if ((fileName != NULL) && (strlen(fileName) > 0))
		{
			UnionUserErrLog("in UnionInitRequestRemoteXMLPackage:: fileName[%s]!\n",fileName);
			return(errCodeParameter);
		}
		else
		{
			UnionPackageSetVersion(&gunionRequestRemotePackageCtxt,PACKAGE_VERSION_001,4);
			if ((ret = UnionInitPackage(&gunionRequestRemotePackageCtxt)) < 0)
			{
				UnionUserErrLog("in UnionInitRequestRemoteXMLPackage:: UnionInitPackage!\n");
				goto exit_tag;
			}
		}
	}
	else if (gunionPackageType == PACKAGE_TYPE_XML)
	{
		UnionResetRequestRemoteXMLPackage();
	
		if ((fileName != NULL) && (strlen(fileName) > 0))
		{
			if ((ret =  UnionXMLInitFromFile(&gunionRequestRemoteXMLCtxt, fileName)) < 0)
			{
				UnionUserErrLog("in UnionInitRequestRemoteXMLPackage:: UnionXMLInitFromFile[%s]\n",fileName);
				goto exit_tag;
			}
		}
		else if ((xmlBuf != NULL) && (strlen(xmlBuf) > 0))
		{
			if ((ret =  UnionXMLInitFromBuf(&gunionRequestRemoteXMLCtxt, xmlBuf)) < 0)
			{
				UnionUserErrLog("in UnionInitRequestRemoteXMLPackage:: UnionXMLInitFromBuf[%s]!\n",xmlBuf);
				goto exit_tag;
			}
		}
		else
		{
			if ((ret =  UnionXMLInit(&gunionRequestRemoteXMLCtxt, defUnionXMLRootName, defUnionXMLEncoding)) < 0)
			{
				UnionUserErrLog("in UnionInitRequestRemoteXMLPackage:: UnionXMLInit!\n");
				goto exit_tag;
			}
#ifdef _MXML_
		gunionRequestRemoteXMLCtxt.xmlCache->isNewXML = 0;
#endif
		}
	}
exit_tag:	
	gunionRequestRemoteInit = 1;
	return(ret);
}

// 初始化远程响应XML包
int UnionInitResponseRemoteXMLPackage(char *fileName,char *xmlBuf,int lenOfBuf)
{
	int	ret = 0;
	
	if (gunionPackageType == PACKAGE_TYPE_V001)
	{
		if (!gunionResponseRemoteInit)
			gunionResponseRemotePackageCtxt.bufSize = 0;

		if ((xmlBuf != NULL) && (strlen(xmlBuf) > 0))
		{
			if ((ret =  UnionUnpackPackage((unsigned char *)xmlBuf,lenOfBuf,&gunionResponseRemotePackageCtxt)) < 0)
			{
				UnionUserErrLog("in UnionInitResponseRemoteXMLPackage:: UnionXMLInitFromBuf[%d][%s]!\n",lenOfBuf,xmlBuf);
				goto exit_tag;
			}
		}
		else if ((fileName != NULL) && (strlen(fileName) > 0))
		{
			UnionUserErrLog("in UnionInitResponseRemoteXMLPackage:: fileName[%s]\n",fileName);
			goto exit_tag;
		}
		else
		{
			UnionPackageSetVersion(&gunionResponseRemotePackageCtxt,PACKAGE_VERSION_001,4);
			if ((ret = UnionInitPackage(&gunionResponseRemotePackageCtxt)) < 0)
			{
				UnionUserErrLog("in UnionInitResponseRemoteXMLPackage:: UnionInitPackage!\n");
				goto exit_tag;
			}
		}
	}
	else if (gunionPackageType == PACKAGE_TYPE_XML)
	{
		UnionResetResponseRemoteXMLPackage();
	
		if ((fileName != NULL) && (strlen(fileName) > 0))
		{
			if ((ret =  UnionXMLInitFromFile(&gunionResponseRemoteXMLCtxt, fileName)) < 0)
			{
				UnionUserErrLog("in UnionInitResponseRemoteXMLPackage:: UnionXMLInitFromFile[%s]\n",fileName);
				goto exit_tag;
			}
		}
		else if ((xmlBuf != NULL) && (strlen(xmlBuf) > 0))
		{
			if ((ret =  UnionXMLInitFromBuf(&gunionResponseRemoteXMLCtxt, xmlBuf)) < 0)
			{
				UnionUserErrLog("in UnionInitResponseRemoteXMLPackage:: UnionXMLInitFromBuf[%s]!\n",xmlBuf);
				goto exit_tag;
			}
		}
		else 
		{
			if ((ret =  UnionXMLInit(&gunionResponseRemoteXMLCtxt, defUnionXMLRootName, defUnionXMLEncoding)) < 0)
			{
				UnionUserErrLog("in UnionInitResponseRemoteXMLPackage:: UnionXMLInit!\n");
				goto exit_tag;
			}
#ifdef _MXML_
		gunionResponseRemoteXMLCtxt.xmlCache->isNewXML = 0;
#endif
		}
	}

exit_tag:
	gunionResponseRemoteInit = 1;
	return(ret);
}

// XML包写入文件
int UnionXMLPackageToFile(char *fileName)
{
	UnionXMLSetDumpFmt(&gunionDefaultXMLCtxt,1);
	return(UnionXMLDumpToFile(&gunionDefaultXMLCtxt, fileName));
}

// 请求XML包写入文件
int UnionRequestXMLPackageToFile(char *fileName)
{
	UnionXMLSetDumpFmt(&gunionRequestXMLCtxt,1);
	return(UnionXMLDumpToFile(&gunionRequestXMLCtxt, fileName));
}

// 响应XML包写入文件
int UnionResponseXMLPackageToFile(char *fileName)
{
	UnionXMLSetDumpFmt(&gunionResponseXMLCtxt,1);
	return(UnionXMLDumpToFile(&gunionResponseXMLCtxt, fileName));
}

// XML包写入BUF
int UnionXMLPackageToBuf(char *buf,int sizeOfBuf)
{
	UnionXMLSetDumpFmt(&gunionDefaultXMLCtxt,0);
	return(UnionXMLDumpToBuf(&gunionDefaultXMLCtxt, buf,sizeOfBuf));
}

// 请求XML包写入BUF
int UnionRequestXMLPackageToBuf(char *buf,int sizeOfBuf)
{
	if (gunionPackageType == PACKAGE_TYPE_V001)
	{
		return(UnionPackPackage(&gunionRequestPackageCtxt, buf,sizeOfBuf));
	}
	else if (gunionPackageType == PACKAGE_TYPE_XML)
	{
		UnionXMLSetDumpFmt(&gunionRequestXMLCtxt,0);
		return(UnionXMLDumpToBuf(&gunionRequestXMLCtxt, buf, sizeOfBuf));
	}
	return(0);
}

// 响应XML包写入BUF
int UnionResponseXMLPackageToBuf(char *buf,int sizeOfBuf)
{
	if (gunionPackageType == PACKAGE_TYPE_V001)
	{
		return(UnionPackPackage(&gunionResponsePackageCtxt, buf,sizeOfBuf));
	}
	else if (gunionPackageType == PACKAGE_TYPE_XML)
	{
		UnionXMLSetDumpFmt(&gunionResponseXMLCtxt,0);
		return(UnionXMLDumpToBuf(&gunionResponseXMLCtxt, buf,sizeOfBuf));
	}
	return(0);
}

// 远程请求XML包写入BUF
int UnionRequestRemoteXMLPackageToBuf(char *buf,int sizeOfBuf)
{
	
        if (gunionPackageType == PACKAGE_TYPE_V001)
        {
                return(UnionPackPackage(&gunionRequestRemotePackageCtxt, buf,sizeOfBuf));
        }
        else if (gunionPackageType == PACKAGE_TYPE_XML)
        {
		UnionXMLSetDumpFmt(&gunionRequestRemoteXMLCtxt,0);
        	return(UnionXMLDumpToBuf(&gunionRequestRemoteXMLCtxt, buf,sizeOfBuf));
        }
        return(0);
}

// 远程响应XML包写入BUF
int UnionResponseRemoteXMLPackageToBuf(char *buf,int sizeOfBuf)
{
	if (gunionPackageType == PACKAGE_TYPE_V001)
        {
                return(UnionPackPackage(&gunionResponseRemotePackageCtxt, buf,sizeOfBuf));
        }
        else if (gunionPackageType == PACKAGE_TYPE_XML)
        {
		UnionXMLSetDumpFmt(&gunionResponseRemoteXMLCtxt,0);
        	return(UnionXMLDumpToBuf(&gunionResponseRemoteXMLCtxt, buf,sizeOfBuf));
        }
	return(0);
}

// 释放XML包
void UnionFreeXMLPackage()
{
	if (gunionDefaultInit)
		UnionXMLFree(&gunionDefaultXMLCtxt);
	
	gunionDefaultInit = 0;
	return;
}

// 释放请求XML包
void UnionFreeRequestXMLPackage()
{
	if (gunionRequestInit)
	{
		if (gunionPackageType == PACKAGE_TYPE_V001)
			UnionFreePackage(&gunionRequestPackageCtxt);
		else if (gunionPackageType == PACKAGE_TYPE_XML)
			UnionXMLFree(&gunionRequestXMLCtxt);
	}
	gunionRequestInit = 0;
	return;
}

// 释放响应XML包
void UnionFreeResponseXMLPackage()
{
	if (gunionResponseInit)
	{
		if (gunionPackageType == PACKAGE_TYPE_V001)
			UnionFreePackage(&gunionResponsePackageCtxt);
		else if (gunionPackageType == PACKAGE_TYPE_XML)
			UnionXMLFree(&gunionResponseXMLCtxt);
	}

	gunionResponseInit = 0;
	return;
}

// 释放远程请求XML包
void UnionFreeRequestRemoteXMLPackage()
{
	if (gunionRequestRemoteInit)
	{
		if (gunionPackageType == PACKAGE_TYPE_V001)
			UnionFreePackage(&gunionRequestRemotePackageCtxt);
		else if (gunionPackageType == PACKAGE_TYPE_XML)
			UnionXMLFree(&gunionRequestRemoteXMLCtxt);
	}
	
	gunionRequestRemoteInit = 0;
	return;
}

// 释放远程响应XML包
void UnionFreeResponseRemoteXMLPackage()
{
	if (gunionResponseRemoteInit)
	{
		if (gunionPackageType == PACKAGE_TYPE_V001)
			UnionFreePackage(&gunionResponseRemotePackageCtxt);
		else if (gunionPackageType == PACKAGE_TYPE_XML)	
			UnionXMLFree(&gunionResponseRemoteXMLCtxt);
	}
	
	gunionResponseRemoteInit = 0;
	return;
}

// 重置XML包
void UnionResetXMLPackage()
{
	if (gunionDefaultInit)
		UnionXMLReset(&gunionDefaultXMLCtxt);
	return;
}

// 重置请求XML包
void UnionResetRequestXMLPackage()
{
	if (gunionRequestInit)
	{
		if (gunionPackageType == PACKAGE_TYPE_V001)
			return;
		else if (gunionPackageType == PACKAGE_TYPE_XML)
			UnionXMLReset(&gunionRequestXMLCtxt);
	}

	return;
}

// 重置响应XML包
void UnionResetResponseXMLPackage()
{
	if (gunionRequestInit)
	{
		if (gunionPackageType == PACKAGE_TYPE_V001)
			return;
		else if (gunionPackageType == PACKAGE_TYPE_XML)
			UnionXMLReset(&gunionResponseXMLCtxt);
	}

	return;
}

// 重置远程请求XML包
void UnionResetRequestRemoteXMLPackage()
{
	if (gunionRequestRemoteInit)
		UnionXMLReset(&gunionRequestRemoteXMLCtxt);
	return;
}

// 重置远程响应XML包
void UnionResetResponseRemoteXMLPackage()
{
	if (gunionResponseRemoteInit)
		UnionXMLReset(&gunionResponseRemoteXMLCtxt);
	return;
}

// 打印日志
int UnionLogXMLPackage()
{
	return(UnionXMLLog(&gunionDefaultXMLCtxt));
}

// 打印请求日志
int UnionLogRequestXMLPackage()
{
	int	ret = -1;
	int	len = 0;
	char	serviceCode[XML_PACK_SIZE];
	char	tag[XML_PACK_SIZE * 4];
	char	value[XML_PACK_SIZE * 1000];
	char	tmpValue[XML_PACK_SIZE * 1000];
	char	numOfComponent[8];
	char	componentGrp[9][50];
	int	i = 0;
	

	// modified 2015-05-05
	/*
	if (gunionPackageType == PACKAGE_TYPE_V001)
		ret = UnionReadPackageFld(&gunionRequestPackageCtxt,"head/serviceCode",16,serviceCode,sizeof(serviceCode));
	else if (gunionPackageType == PACKAGE_TYPE_XML)
		ret = UnionXMLGet(&gunionRequestXMLCtxt, "head/serviceCode", 0, serviceCode, sizeof(serviceCode));
	*/
	ret = UnionReadRequestXMLPackageValue("head/serviceCode", serviceCode, sizeof(serviceCode));
	// end of modification 2015-05-05
	if (ret > 0)
	{
		serviceCode[ret] = 0;
		if ((memcmp(serviceCode,"E140",4) == 0)	// 加密PIN
			|| (memcmp(serviceCode,"E160",4) == 0)	//加密数据
			|| (memcmp(serviceCode,"E172",4) == 0)	//公钥加密
			|| (memcmp(serviceCode,"K006",4) == 0)
			|| (memcmp(serviceCode,"EF62",4) == 0)
			|| (memcmp(serviceCode,"EF91",4) == 0)
			|| (memcmp(serviceCode,"EEI3",4) == 0))  //加密敏感数据
		{
			if (memcmp(serviceCode,"E140",4) == 0)
				snprintf(tag,sizeof(tag),"%s","body/plainPin");
			if (memcmp(serviceCode,"E160",4) == 0)
				snprintf(tag,sizeof(tag),"%s","body/data");
			if (memcmp(serviceCode,"E172",4) == 0)
				snprintf(tag,sizeof(tag),"%s","body/plainData");
                        if (memcmp(serviceCode,"K006",4) == 0)
                        {	
                        	len = UnionReadRequestXMLPackageValue("body/senseData", value, sizeof(value));
				if(len > 0)					
                                	snprintf(tag,sizeof(tag),"%s","body/senseData");
				else
					snprintf(tag,sizeof(tag),"%s","body/plainData");
				memset(value,0,sizeof(value));
				
                        }
			if (memcmp(serviceCode,"EF62",4) == 0)
				snprintf(tag,sizeof(tag),"%s","body/plainPin");
			if (memcmp(serviceCode,"EEI3",4) == 0)
				snprintf(tag,sizeof(tag),"%s","body/plainPin");
			if (memcmp(serviceCode,"EF91",4) == 0)
				snprintf(tag,sizeof(tag),"%s","body/plainPin");
			// modified 2015-05-05
			/*
			if (gunionPackageType == PACKAGE_TYPE_V001)
				len = UnionReadPackageFld(&gunionRequestPackageCtxt,tag,strlen(tag),value,sizeof(value));
			else if (gunionPackageType == PACKAGE_TYPE_XML)
				len = UnionXMLGet(&gunionRequestXMLCtxt, tag, 0, value, sizeof(value));
			*/
			len = UnionReadRequestXMLPackageValue(tag, value, sizeof(value));
			// end of modification 2015-05-05
			if (len > 0)
			{
				value[len] = 0;
				memset(tmpValue,'*',len);
				tmpValue[len] = 0;
				/* modified 2015-05-05
				if (gunionPackageType == PACKAGE_TYPE_V001)
					UnionPutPackageFld(&gunionRequestPackageCtxt,tag,strlen(tag),tmpValue,len);
				else if (gunionPackageType == PACKAGE_TYPE_XML)
					UnionXMLPut(&gunionRequestXMLCtxt, tag, tmpValue, 0);
				*/
				UnionSetRequestXMLPackageValue(tag, tmpValue);
				// end of modification 2015-05-05
			}
			else
				goto normalLog;
		}
		else if (memcmp(serviceCode,"EEW1",4) == 0)	//
		{
			ret = UnionReadRequestXMLPackageValue("body/numOfComponent", numOfComponent, sizeof(numOfComponent));
			if (ret > 0)
			{
				for (i = 0; i < atoi(numOfComponent); i++)
				{
					snprintf(tag,sizeof(tag),"body/component%d",i+1);
					len = UnionReadRequestXMLPackageValue(tag, componentGrp[i], sizeof(componentGrp[i]));
					if (len > 0)
					{
						componentGrp[i][len] = 0;
						memset(tmpValue,'*',len);
						tmpValue[len] = 0;
						UnionSetRequestXMLPackageValue(tag, tmpValue);
					}
					else
						goto normalLog;
				}
			}
		}
		else
			goto normalLog;

		UnionNullLog("***** 请求报文 *****::\n");
		if (gunionPackageType == PACKAGE_TYPE_V001)
		{
			UnionLogPackage(&gunionRequestPackageCtxt);
			if (memcmp(serviceCode,"EEW1",4) == 0)
			{
				for (i = 0; i < atoi(numOfComponent); i++)
				{
					snprintf(tag,sizeof(tag),"body/component%d",i+1);
					UnionPutPackageFld(&gunionRequestPackageCtxt,tag,strlen(tag),componentGrp[i],strlen(componentGrp[i]));
				}
			}
			else
				UnionPutPackageFld(&gunionRequestPackageCtxt,tag,strlen(tag),value,len);
			ret = 0;
		}
		else if (gunionPackageType == PACKAGE_TYPE_XML)
		{
			ret = UnionXMLLog(&gunionRequestXMLCtxt);
			if (memcmp(serviceCode,"EEW1",4) == 0)
			{
				for (i = 0; i < atoi(numOfComponent); i++)
				{
					snprintf(tag,sizeof(tag),"body/component%d",i+1);
					UnionXMLPut(&gunionRequestXMLCtxt, tag, componentGrp[i], 0);
				}
			}
			else
				UnionXMLPut(&gunionRequestXMLCtxt, tag, value, 0);
		}
		return(ret);
	}

normalLog:
	UnionNullLog("***** 请求报文 *****::\n");
	if (gunionPackageType == PACKAGE_TYPE_V001)
	{
		UnionLogPackage(&gunionRequestPackageCtxt);
		return(0);
	}
	else if (gunionPackageType == PACKAGE_TYPE_XML)
		return(UnionXMLLog(&gunionRequestXMLCtxt));
	else
		return(errCodeParameter);
}

// 打印响应日志
int UnionLogResponseXMLPackage()
{
	int	ret = -1;
	int	len = 0;
	char	serviceCode[XML_PACK_SIZE];
	char	tag[XML_PACK_SIZE*4];
	char	value[XML_PACK_SIZE * 1000];
	char	tmpValue[XML_PACK_SIZE * 1000];
	
	/* modified 2015-05-05
	if (gunionPackageType == PACKAGE_TYPE_V001)
		ret = UnionReadPackageFld(&gunionResponsePackageCtxt,"head/serviceCode",16,serviceCode,sizeof(serviceCode));
	else if (gunionPackageType == PACKAGE_TYPE_XML)
		ret = UnionXMLGet(&gunionResponseXMLCtxt, "head/serviceCode", 0, serviceCode, sizeof(serviceCode));
	else
		return(errCodeParameter);
	*/

	ret = UnionReadResponseXMLPackageValue("head/serviceCode", serviceCode, sizeof(serviceCode));
	// end of 2015-05-05

	if (ret > 0)
	{
		serviceCode[ret] = 0;
		if ((memcmp(serviceCode,"E141",4) == 0)		// 解密PIN
			|| (memcmp(serviceCode,"E161",4) == 0)	//解密数据
			|| (memcmp(serviceCode,"K007",4) == 0)	// kms解密
			|| (memcmp(serviceCode,"EF93",4) == 0)	// 解密PIN
			|| (memcmp(serviceCode,"E173",4) == 0))	//私钥解密
		{
			if (memcmp(serviceCode,"E141",4) == 0)
				snprintf(tag,sizeof(tag),"%s","body/plainPin");
			else if (memcmp(serviceCode,"E161",4) == 0)
				snprintf(tag,sizeof(tag),"%s","body/data");
			else if (memcmp(serviceCode,"E173",4) == 0)
				snprintf(tag,sizeof(tag),"%s","body/plainData");
			else if (memcmp(serviceCode,"K007",4) == 0)
				{	
					len = UnionReadResponseXMLPackageValue("body/senseData", value, sizeof(value));
					if(len > 0)
						snprintf(tag,sizeof(tag),"%s","body/senseData");
					else
						snprintf(tag,sizeof(tag),"%s","body/plainData");
					memset(value,0,sizeof(value));
				}
			else if (memcmp(serviceCode,"EF93",4) == 0)
				snprintf(tag,sizeof(tag),"%s","body/plainPin");
			/* modified 2015-05-05
			if (gunionPackageType == PACKAGE_TYPE_V001)
				len = UnionReadPackageFld(&gunionResponsePackageCtxt,tag,strlen(tag),value,sizeof(value));
			else if (gunionPackageType == PACKAGE_TYPE_XML)
				len = UnionXMLGet(&gunionResponseXMLCtxt, tag, 0, value, sizeof(value));
			*/
			len = UnionReadResponseXMLPackageValue(tag, value, sizeof(value));
			// end of 2015-05-05

			if (len > 0)
			{
				value[len] = 0;
				memset(tmpValue,'*',len);
				tmpValue[len] = 0;
				// modified 2015-05-05
				/*
				if (gunionPackageType == PACKAGE_TYPE_V001)
					UnionPutPackageFld(&gunionResponsePackageCtxt,tag,strlen(tag),tmpValue,len);
				else if (gunionPackageType == PACKAGE_TYPE_XML)
					UnionXMLPut(&gunionResponseXMLCtxt, tag, tmpValue, 0);
				*/
				UnionSetResponseXMLPackageValue(tag, tmpValue);
				// end of modification 2015-05-05
			}
			else
				goto normalLog;
		}
		else
			goto normalLog;

		UnionNullLog("***** 响应报文 *****::\n");
		if (gunionPackageType == PACKAGE_TYPE_V001)
		{
			UnionLogPackage(&gunionResponsePackageCtxt);
			UnionPutPackageFld(&gunionResponsePackageCtxt,tag,strlen(tag),value,len);
			ret = 0;
		}
		else if (gunionPackageType == PACKAGE_TYPE_XML)
		{
			ret = UnionXMLLog(&gunionResponseXMLCtxt);		
			UnionXMLPut(&gunionResponseXMLCtxt, tag, value, 0);
		}
		return(ret);
	}

normalLog:
	UnionNullLog("***** 响应报文 *****::\n");
	if (gunionPackageType == PACKAGE_TYPE_V001)
	{
		UnionLogPackage(&gunionResponsePackageCtxt);
		return(0);
	}
	else if (gunionPackageType == PACKAGE_TYPE_XML)
		return(UnionXMLLog(&gunionResponseXMLCtxt));
	else
		return(errCodeParameter);
}

// 打印远程请求日志
int UnionLogRequestRemoteXMLPackage()
{
	int	ret;
	char	serviceCode[XML_PACK_SIZE];
	char	tag[XML_PACK_SIZE*4];
	char	value[XML_PACK_SIZE*32];
	char	tmpValue[XML_PACK_SIZE*32];
	int len;
	
	if(UnionIsDisplaySensitiveInfo()) // 不打印敏感信息日志
	{
		goto normalLog;
	}
	
	if (gunionPackageType == PACKAGE_TYPE_XML)
	{
		if ((ret = UnionXMLGet(&gunionRequestRemoteXMLCtxt, "head/serviceCode", 0, serviceCode, sizeof(serviceCode))) > 0)
		{
			serviceCode[ret] = 0;
			if (memcmp(serviceCode,"E140",4) == 0)	// 加密PIN
			{
				snprintf(tag,sizeof(tag),"%s","body/plainPin");
				if ((ret = UnionXMLGet(&gunionRequestRemoteXMLCtxt, tag, 0, value, sizeof(value))) > 0)
				{
					value[ret] = 0;
					memset(tmpValue,'*',ret);
					tmpValue[ret] = 0;
					UnionXMLPut(&gunionRequestRemoteXMLCtxt, tag, tmpValue, 0);
				}
				else
					goto normalLog;
			}
			else
				goto normalLog;
	
			UnionNullLog("***** 远程请求报文 *****::\n");
			ret = UnionXMLLog(&gunionRequestRemoteXMLCtxt);
			
			UnionXMLPut(&gunionRequestXMLCtxt, tag, value, 0);
			return(ret);
		}
	}
	else if (gunionPackageType == PACKAGE_TYPE_V001)  //add by yangw 20160222
	{
		//UnionReadPackageFld(&gunionResponsePackageCtxt,nodeName,strlen(nodeName),value,sizeOfBuf)) < 0)
		if ((ret = UnionReadPackageFld(&gunionRequestRemotePackageCtxt, "head/serviceCode", strlen("head/serviceCode"),serviceCode, sizeof(serviceCode))) > 0)
		{
			serviceCode[ret] = 0;
			if (memcmp(serviceCode,"E140",4) == 0)	// 加密PIN
			{
				len = snprintf(tag,sizeof(tag),"%s","body/plainPin");
				if ((ret = UnionReadPackageFld(&gunionRequestRemotePackageCtxt, tag, len, value, sizeof(value))) > 0)
				{
					value[ret] = 0;
					memset(tmpValue,'*',ret);
					tmpValue[ret] = 0;
					//UnionXMLPut(&gunionRequestRemoteXMLCtxt, tag, tmpValue, 0);
					UnionPutPackageFld(&gunionRequestRemotePackageCtxt,tag,strlen(tag),tmpValue,ret);
				}
				else
					goto normalLog;
			}
			else
				goto normalLog;
	
			UnionNullLog("***** 远程请求报文 *****::\n");
			UnionLogPackage(&gunionRequestRemotePackageCtxt);
			//UnionXMLPut(&gunionRequestXMLCtxt, tag, value, 0);
			UnionPutPackageFld(&gunionRequestRemotePackageCtxt,tag,strlen(tag),value,ret);
			return(ret);
		}
		
	}

normalLog:
	UnionNullLog("***** 远程请求报文 *****::\n");	
	if (gunionPackageType == PACKAGE_TYPE_V001)//add by yangw 20160222
	{
		UnionLogPackage(&gunionRequestRemotePackageCtxt);
		return(0);
	}
	else if (gunionPackageType == PACKAGE_TYPE_XML)
		return(UnionXMLLog(&gunionRequestRemoteXMLCtxt));
	else
		return(errCodeParameter);
}

// 打印远程响应日志
int UnionLogResponseRemoteXMLPackage()
{
	int	ret;
	char	serviceCode[XML_PACK_SIZE];
	char	tag[XML_PACK_SIZE*4];
	char	value[XML_PACK_SIZE * 1000];
	char	tmpValue[XML_PACK_SIZE * 1000];
	
	if(UnionIsDisplaySensitiveInfo()) // 不打印敏感信息日志
	{
		goto normalLog;
	}
	if (gunionPackageType == PACKAGE_TYPE_XML)
	{
		if ((ret = UnionXMLGet(&gunionResponseRemoteXMLCtxt, "head/serviceCode", 0, serviceCode, sizeof(serviceCode))) > 0)
		{
			serviceCode[ret] = 0;
			if ((memcmp(serviceCode,"E141",4) == 0)	// 解密PIN
				|| (memcmp(serviceCode,"E161",4) == 0)	//解密数据
				|| (memcmp(serviceCode,"E173",4) == 0))	//私钥解密
			{
				if (memcmp(serviceCode,"E141",4) == 0)
					snprintf(tag,sizeof(tag),"%s","body/plainPin");
				else if (memcmp(serviceCode,"E161",4) == 0)
					snprintf(tag,sizeof(tag),"%s","body/data");
				else if (memcmp(serviceCode,"E173",4) == 0)
					snprintf(tag,sizeof(tag),"%s","body/plainData");
				if ((ret = UnionXMLGet(&gunionResponseRemoteXMLCtxt, tag, 0, value, sizeof(value))) > 0)
				{
					value[ret] = 0;
					memset(tmpValue,'*',ret);
					tmpValue[ret] = 0;
					UnionXMLPut(&gunionResponseRemoteXMLCtxt, tag, tmpValue, 0);
				}
				else
					goto normalLog;
			}
			else
				goto normalLog;
	
			UnionNullLog("***** 远程响应报文 *****::\n");
			ret = UnionXMLLog(&gunionResponseRemoteXMLCtxt);
			
			UnionXMLPut(&gunionResponseRemoteXMLCtxt, tag, value, 0);
			return(ret);
		}
	}
	else if (gunionPackageType == PACKAGE_TYPE_V001)  //add by yangw 20160222
	{
		if ((ret = UnionReadPackageFld(&gunionResponseRemotePackageCtxt, "head/serviceCode", strlen("head/serviceCode"), serviceCode, sizeof(serviceCode))) > 0)
		{
			serviceCode[ret] = 0;
			if ((memcmp(serviceCode,"E141",4) == 0)	// 解密PIN
				|| (memcmp(serviceCode,"E161",4) == 0)	//解密数据
				|| (memcmp(serviceCode,"E173",4) == 0))	//私钥解密
			{
				if (memcmp(serviceCode,"E141",4) == 0)
					snprintf(tag,sizeof(tag),"%s","body/plainPin");
				else if (memcmp(serviceCode,"E161",4) == 0)
					snprintf(tag,sizeof(tag),"%s","body/data");
				else if (memcmp(serviceCode,"E173",4) == 0)
					snprintf(tag,sizeof(tag),"%s","body/plainData");
				if ((ret = UnionReadPackageFld(&gunionResponseRemotePackageCtxt, tag, strlen(tag), value, sizeof(value))) > 0)
				{
					value[ret] = 0;
					memset(tmpValue,'*',ret);
					tmpValue[ret] = 0;
					UnionPutPackageFld(&gunionResponseRemotePackageCtxt, tag, strlen(tag),tmpValue, ret);
				}
				else
					goto normalLog;
			}
			else
				goto normalLog;
	
			UnionNullLog("***** 远程响应报文 *****::\n");
			//ret = UnionXMLLog(&gunionResponseRemoteXMLCtxt);
			UnionLogPackage(&gunionResponseRemotePackageCtxt);
			UnionPutPackageFld(&gunionResponseRemotePackageCtxt, tag,  strlen(tag),value, ret);
			return(ret);
		}
	}

normalLog:
	UnionNullLog("***** 远程响应报文 *****::\n");
	//return(UnionXMLLog(&gunionResponseRemoteXMLCtxt));
	if (gunionPackageType == PACKAGE_TYPE_V001)//add by yangw 20160222
	{
		UnionLogPackage(&gunionResponseRemotePackageCtxt);
		return(0);
	}
	else if (gunionPackageType == PACKAGE_TYPE_XML)
		return(UnionXMLLog(&gunionResponseRemoteXMLCtxt));
	else
		return(errCodeParameter);
}

// 根据节点名字获取路径
int UnionGetXMLPackagePath(char *nodeName,char *path,int sizeofBuf)
{
	int	len;
	
	if ((nodeName == NULL) || (strlen(nodeName) == 0))
		len = snprintf(path,sizeofBuf,"/%s",defUnionXMLRootName);
	else if (nodeName[0] != '/')
		len = snprintf(path,sizeofBuf,"/%s/%s",defUnionXMLRootName,nodeName);
	else
		len = snprintf(path,sizeofBuf,"%s",nodeName);
	path[len] = 0;
	return(len);
}

// 定位节点
int UnionLocateXMLPackage(char *nodeName,int id)
{
	int	ret;
	char	path[XML_PACK_SIZE*10];
	
	UnionGetXMLPackagePath(nodeName,path,sizeof(path));
		
	if ((ret = UnionXMLLocate(&gunionDefaultXMLCtxt, path, id)) < 0)
	{
		if (ret != errCodeObjectMDL_FieldNotExist)
			UnionUserErrLog("in UnionLocateXMLPackage:: UnionXMLLocate[%s]!\n",path);
		return(ret);
	}
	return(ret);
}

// 定位请求包
int UnionLocateRequestXMLPackage(char *nodeName,int id)
{
	if (gunionPackageType == PACKAGE_TYPE_XML)
	{
		int	ret;
		char	path[XML_PACK_SIZE*10];
		
		UnionGetXMLPackagePath(nodeName,path,sizeof(path));
		
		if ((ret = UnionXMLLocate(&gunionRequestXMLCtxt, path, id)) < 0)
		{
			if (ret != errCodeObjectMDL_FieldNotExist)
				UnionUserErrLog("in UnionLocateRequestXMLPackage:: UnionXMLLocate[%s]!\n",path);
			return(ret);
		}
	
		return(ret);
	}
	else
		return(0);
}

// 定位响应包
int UnionLocateResponseXMLPackage(char *nodeName,int id)
{
	if (gunionPackageType == PACKAGE_TYPE_XML)
	{
		int	ret;
		char	path[XML_PACK_SIZE*10];
		
		UnionGetXMLPackagePath(nodeName,path,sizeof(path));
		
		if ((ret = UnionXMLLocate(&gunionResponseXMLCtxt, path, id)) < 0)
		{
			if (ret != errCodeObjectMDL_FieldNotExist)
				UnionUserErrLog("in UnionLocateResponseXMLPackage:: UnionXMLLocate[%s]!\n",path);
			return(ret);
		}
	
		return(ret);
	}
	else
		return(0);
}

// 定位远程请求包
int UnionLocateRequestRemoteXMLPackage(char *nodeName,int id)
{
	int	ret;
	char	path[XML_PACK_SIZE*10];
	
	UnionGetXMLPackagePath(nodeName,path,sizeof(path));
	
	if ((ret = UnionXMLLocate(&gunionRequestRemoteXMLCtxt, path, id)) < 0)
	{
		if (ret != errCodeObjectMDL_FieldNotExist)
			UnionUserErrLog("in UnionLocateRequestRemoteXMLPackage:: UnionXMLLocate[%s]!\n",path);
		return(ret);
	}

	return(ret);
}

// 定位响应包
int UnionLocateResponseRemoteXMLPackage(char *nodeName,int id)
{
	int	ret;
	char	path[XML_PACK_SIZE*10];
	
	UnionGetXMLPackagePath(nodeName,path,sizeof(path));
	
	if ((ret = UnionXMLLocate(&gunionResponseRemoteXMLCtxt, path, id)) < 0)
	{
		if (ret != errCodeObjectMDL_FieldNotExist)
			UnionUserErrLog("in UnionLocateResponseRemoteXMLPackage:: UnionXMLLocate[%s]!\n",path);
		return(ret);
	}

	return(ret);
}

// 增加新节点并定位
int UnionLocateNewXMLPackage(char *nodeName,int id)
{
	int	ret;
	char	path[XML_PACK_SIZE*10];
	
	UnionGetXMLPackagePath(nodeName,path,sizeof(path));
	
	if ((ret = UnionXMLLocateNew(&gunionDefaultXMLCtxt, path, id)) < 0)
	{
		UnionUserErrLog("in UnionLocateNewXMLPackage:: UnionXMLLocateNew[%s]!\n",path);
		return(ret);
	}
	return(ret);
}

// 请求包增加新节点并定位
int UnionLocateRequsetNewXMLPackage(char *nodeName,int id)
{
	if (gunionPackageType == PACKAGE_TYPE_XML)
	{
		int	ret;
		char	path[XML_PACK_SIZE*10];
		
		UnionGetXMLPackagePath(nodeName,path,sizeof(path));
		
		if ((ret = UnionXMLLocateNew(&gunionRequestXMLCtxt, path, id)) < 0)
		{
			UnionUserErrLog("in UnionLocateRequsetNewXMLPackage:: UnionXMLLocateNew[%s]!\n",path);
			return(ret);
		}
		return(ret);
	}
	else
		return(0);
}

// 响应包增加新节点并定位
int UnionLocateResponseNewXMLPackage(char *nodeName,int id)
{
	if (gunionPackageType == PACKAGE_TYPE_XML)
	{
		int	ret;
		char	path[XML_PACK_SIZE*10];
		
		UnionGetXMLPackagePath(nodeName,path,sizeof(path));
		
		if ((ret = UnionXMLLocateNew(&gunionResponseXMLCtxt, path, id)) < 0)
		{
			UnionUserErrLog("in UnionLocateResponseNewXMLPackage:: UnionXMLLocateNew[%s]!\n",path);
			return(ret);
		}
		return(ret);
	}
	else
		return(0);
}

// 读取字段值
int UnionReadXMLPackageValue(char *nodeName,char *value,int sizeOfBuf)
{
	int	ret = 0;
	
	if ((ret = UnionXMLGet(&gunionDefaultXMLCtxt, nodeName, 0, value, sizeOfBuf)) < 0)
	{
		if (ret != errCodeObjectMDL_FieldNotExist)
			UnionUserErrLog("in UnionReadXMLPackageValue:: UnionXMLGet[%s]!\n", nodeName);
		return(ret);
	}
	return(ret);
}

// 读取请求报文字段值
int UnionReadRequestXMLPackageValue(char *nodeName,char *value,int sizeOfBuf)
{
	int	ret = 0;

	if (gunionPackageType == PACKAGE_TYPE_V001)
	{
		if ((ret = UnionReadPackageFld(&gunionRequestPackageCtxt,nodeName,strlen(nodeName),value,sizeOfBuf)) < 0)
		{
			if (ret != errCodeEsscMDL_EsscPackageFldNotFound)
				UnionUserErrLog("in UnionReadRequestXMLPackageValue:: UnionReadPackageFld[%s]!\n", nodeName);
			return(ret);
		}
	}
	else if (gunionPackageType == PACKAGE_TYPE_XML)
	{
		if ((ret = UnionXMLGet(&gunionRequestXMLCtxt, nodeName, 0, value, sizeOfBuf)) < 0)
		{
			if (ret != errCodeObjectMDL_FieldNotExist)
				UnionUserErrLog("in UnionReadRequestXMLPackageValue:: UnionXMLGet[%s]!\n", nodeName);
			return(ret);
		}
	}
	return(ret);
}

// 读取响应报文字段值
int UnionReadResponseXMLPackageValue(char *nodeName,char *value,int sizeOfBuf)
{
	int	ret = 0;

	if (gunionPackageType == PACKAGE_TYPE_V001)
	{
		if ((ret = UnionReadPackageFld(&gunionResponsePackageCtxt,nodeName,strlen(nodeName),value,sizeOfBuf)) < 0)
		{
			if (ret != errCodeEsscMDL_EsscPackageFldNotFound)
				UnionUserErrLog("in UnionReadResponseXMLPackageValue:: UnionReadPackageFld[%s]!\n", nodeName);
			return(ret);
		}
	}
	else if (gunionPackageType == PACKAGE_TYPE_XML)
	{
		if ((ret = UnionXMLGet(&gunionResponseXMLCtxt, nodeName, 0, value, sizeOfBuf)) < 0)
		{
			if (ret != errCodeObjectMDL_FieldNotExist)
				UnionUserErrLog("in UnionReadResponseXMLPackageValue:: UnionXMLGet[%s]!\n", nodeName);
			return(ret);
		}
	}
	return(ret);
}

// 读取远程请求报文字段值
int UnionReadRequestRemoteXMLPackageValue(char *nodeName,char *value,int sizeOfBuf)
{
	int	ret = 0;

	if (gunionPackageType == PACKAGE_TYPE_V001)
	{
		if ((ret = UnionReadPackageFld(&gunionRequestRemotePackageCtxt,nodeName,strlen(nodeName),value,sizeOfBuf)) < 0)
		{
			if (ret != errCodeEsscMDL_EsscPackageFldNotFound)
				UnionUserErrLog("in UnionReadRequestRemoteXMLPackageValue:: UnionReadPackageFld[%s]!\n", nodeName);
			return(ret);
		}

	}
	else if (gunionPackageType == PACKAGE_TYPE_XML)	
	{
		if ((ret = UnionXMLGet(&gunionRequestRemoteXMLCtxt, nodeName, 0, value, sizeOfBuf)) < 0)
		{
			if (ret != errCodeObjectMDL_FieldNotExist)
				UnionUserErrLog("in UnionReadRequestRemoteXMLPackageValue:: UnionXMLGet[%s]!\n", nodeName);
			return(ret);
		}
	}
	return(ret);
}

// 读取远程响应报文字段值
int UnionReadResponseRemoteXMLPackageValue(char *nodeName,char *value,int sizeOfBuf)
{
	int	ret = 0;

	if (gunionPackageType == PACKAGE_TYPE_V001)
	{
		if ((ret = UnionReadPackageFld(&gunionResponseRemotePackageCtxt,nodeName,strlen(nodeName),value,sizeOfBuf)) < 0)
		{
			if (ret != errCodeEsscMDL_EsscPackageFldNotFound)
				UnionUserErrLog("in UnionReadResponseRemoteXMLPackageValue:: UnionReadPackageFld[%s]!\n", nodeName);
			return(ret);
		}

	}
	else if (gunionPackageType == PACKAGE_TYPE_XML)
	{
		if ((ret = UnionXMLGet(&gunionResponseRemoteXMLCtxt, nodeName, 0, value, sizeOfBuf)) < 0)
		{
			if (ret != errCodeObjectMDL_FieldNotExist)
				UnionUserErrLog("in UnionReadResponseRemoteXMLPackageValue:: UnionXMLGet[%s]!\n", nodeName);
			return(ret);
		}
	}
	return(ret);
}

// 读取节点下所有字段值
int UnionSelectXMLPackageValue(char *nodeName,char *value,int sizeOfBuf)
{
	return(UnionXMLSelectGet(&gunionDefaultXMLCtxt, nodeName, 0, value, sizeOfBuf));
}

// 读取请求报文节点下所有字段值
int UnionSelectRequestXMLPackageValue(char *nodeName,char *value,int sizeOfBuf)
{
	if (gunionPackageType == PACKAGE_TYPE_XML)
		return(UnionXMLSelectGet(&gunionRequestXMLCtxt, nodeName, 0, value, sizeOfBuf));
	else
		return(0);
}

// 读取响应报文节点下所有字段值
int UnionSelectResponseXMLPackageValue(char *nodeName,char *value,int sizeOfBuf)
{
	if (gunionPackageType == PACKAGE_TYPE_XML)
		return(UnionXMLSelectGet(&gunionResponseXMLCtxt, nodeName, 0, value, sizeOfBuf));
	else
		return(0);
}

// 读取远程请求报文节点下所有字段值
int UnionSelectRequestRemoteXMLPackageValue(char *nodeName,char *value,int sizeOfBuf)
{
	return(UnionXMLSelectGet(&gunionRequestRemoteXMLCtxt, nodeName, 0, value, sizeOfBuf));
}

// 读取远程响应报文节点下所有字段值
int UnionSelectResponseRemoteXMLPackageValue(char *nodeName,char *value,int sizeOfBuf)
{
	return(UnionXMLSelectGet(&gunionResponseRemoteXMLCtxt, nodeName, 0, value, sizeOfBuf));
}

// 设置字段值
int UnionSetXMLPackageValue(char *nodeName,char *value)
{
	return(UnionXMLPut(&gunionDefaultXMLCtxt, nodeName, value, 0));
}

// 设置请求报文字段值
int UnionSetRequestXMLPackageValue(char *nodeName,char *value)
{
	if (gunionPackageType == PACKAGE_TYPE_V001)
		return(UnionPutPackageFld(&gunionRequestPackageCtxt,nodeName,strlen(nodeName),value,strlen(value)));
	else if (gunionPackageType == PACKAGE_TYPE_XML)
		return(UnionXMLPut(&gunionRequestXMLCtxt, nodeName, value, 0));
	else
		return(0);
}

// add by leipp 2015/08/06
// 设置请求报文字段值
int UnionSetRequestXMLPackageBitValue(char *nodeName,int lenOfValue,char *value)
{
	if (gunionPackageType == PACKAGE_TYPE_V001)
		return(UnionPutPackageFld(&gunionRequestPackageCtxt,nodeName,strlen(nodeName),value,lenOfValue));
	else if (gunionPackageType == PACKAGE_TYPE_XML)
		return(UnionXMLPut(&gunionRequestXMLCtxt, nodeName, value, 0));
	else
		return(0);
}
// by leipp end

// 设置响应报文字段值
int UnionSetResponseXMLPackageValue(char *nodeName,char *value)
{
	if (gunionPackageType == PACKAGE_TYPE_V001)
		return(UnionPutPackageFld(&gunionResponsePackageCtxt,nodeName,strlen(nodeName),value,strlen(value)));
	else if (gunionPackageType == PACKAGE_TYPE_XML)
		return(UnionXMLPut(&gunionResponseXMLCtxt, nodeName, value, 0));
	else
		return(0);
}

// 设置远程请求报文字段值
int UnionSetRequestRemoteXMLPackageValue(char *nodeName,char *value)
{
	if (gunionPackageType == PACKAGE_TYPE_V001)
		return(UnionPutPackageFld(&gunionRequestRemotePackageCtxt,nodeName,strlen(nodeName),value,strlen(value)));
	else if (gunionPackageType == PACKAGE_TYPE_XML)
		return(UnionXMLPut(&gunionRequestRemoteXMLCtxt, nodeName, value, 0));
	else
		return(0);
}

// 设置远程响应报文字段值
int UnionSetResponseRemoteXMLPackageValue(char *nodeName,char *value)
{
	if (gunionPackageType == PACKAGE_TYPE_V001)
		return(UnionPutPackageFld(&gunionResponseRemotePackageCtxt,nodeName,strlen(nodeName),value,strlen(value)));
	else if (gunionPackageType == PACKAGE_TYPE_XML)
		return(UnionXMLPut(&gunionResponseRemoteXMLCtxt, nodeName, value, 0));
	else
		return(0);
}

// 删除节点
int UnionDeleteXMLPackageNode(char *nodeName,int id)
{
	return(UnionXMLDelete(&gunionDefaultXMLCtxt, nodeName, id));
}

// 删除请求节点
int UnionDeleteRequestXMLPackageNode(char *nodeName,int id)
{
	if (gunionPackageType == PACKAGE_TYPE_XML)		
		return(UnionXMLDelete(&gunionRequestXMLCtxt, nodeName, id));
	else
		return(0);
}

// 删除响应节点
int UnionDeleteResponseXMLPackageNode(char *nodeName,int id)
{
	if (gunionPackageType == PACKAGE_TYPE_XML)	
		return(UnionXMLDelete(&gunionResponseXMLCtxt, nodeName, id));
	// added 2015-08-26
	else if (gunionPackageType == PACKAGE_TYPE_V001)	
		return(UnionClearPackageGrpByName(&gunionResponsePackageCtxt, nodeName));
	// end of addition 2015-08-26
	else
		return(0);
}

// 删除远程请求节点
int UnionDeleteRequestRemoteXMLPackageNode(char *nodeName,int id)
{
	if (gunionPackageType == PACKAGE_TYPE_XML)
		return(UnionXMLDelete(&gunionRequestRemoteXMLCtxt, nodeName, id));
	else
		return(0);
}

// 删除远程响应节点
int UnionDeleteResponseRemoteXMLPackageNode(char *nodeName,int id)
{
	if (gunionPackageType == PACKAGE_TYPE_XML)
		return(UnionXMLDelete(&gunionResponseRemoteXMLCtxt, nodeName, id));
	else
		return(0);
}

// 交换两个同路径节点的ID值
int UnionExchangeIDXMLPackage(char *nodeName,int id1,int id2)
{
	return(UnionXMLExchgId(&gunionDefaultXMLCtxt, nodeName, id1, id2));
}

// 交换两个同路径请求节点的ID值
int UnionExchangeIDRequestXMLPackage(char *nodeName,int id1,int id2)
{
	if (gunionPackageType == PACKAGE_TYPE_XML)
		return(UnionXMLExchgId(&gunionRequestXMLCtxt, nodeName, id1, id2));
	else
		return(0);
}

// 交换两个同路径响应节点的ID值
int UnionExchangeIDResponseXMLPackage(char *nodeName,int id1,int id2)
{
	if (gunionPackageType == PACKAGE_TYPE_XML)
		return(UnionXMLExchgId(&gunionResponseXMLCtxt, nodeName, id1, id2));
	else
		return(0);
}

// 设置响应码
void UnionSetResponseRemark(char *fmt,...)
{
	va_list args;
	
	va_start(args,fmt);
	vsnprintf(gunionErrRemark,sizeof(gunionErrRemark),fmt,args);
	va_end(args);

	return;
}

int UnionGetResponseRemark(char *errRemark,int sizeofBuf)
{
	int	len;
	
	len = snprintf(errRemark,sizeofBuf,"%s",gunionErrRemark);
	gunionErrRemark[0] = 0;
	return(len);
}

// 初始化响应报文头
int UnionInitHeadOfResponseXMLPackage()
{
	int	len;
	char	tmpBuf[XML_PACK_SIZE*4];
	
	
	// 初始化XML响应包
	if ((len = UnionInitResponseXMLPackage(NULL, NULL,0)) < 0)
	{
		UnionUserErrLog("in UnionInitHeadOfResponseXMLPackage:: UnionInitResponseXMLPackage!\n");
		return(len);
	}

	// 设置服务码
	if ((len = UnionReadRequestXMLPackageValue("head/serviceCode",gunionXMLPackageHead.serviceCode,sizeof(gunionXMLPackageHead.serviceCode))) < 0)
	{
		UnionUserErrLog("in UnionInitHeadOfResponseXMLPackage:: UnionReadRequestXMLPackageValue[%s]!\n","head/serviceCode");
		return(len);
	}
	UnionSetResponseXMLPackageValue("head/serviceCode",gunionXMLPackageHead.serviceCode);

	// 设置系统ID
	if ((len = UnionReadRequestXMLPackageValue("head/sysID",gunionXMLPackageHead.sysID,sizeof(gunionXMLPackageHead.sysID))) < 0)
	{
		UnionUserErrLog("in UnionInitHeadOfResponseXMLPackage:: UnionReadRequestXMLPackageValue[%s]!\n","head/sysID");
		return(len);
	}
	UnionSetResponseXMLPackageValue("head/sysID",gunionXMLPackageHead.sysID);

	if (UnionIsUITrans(gunionXMLPackageHead.sysID))
	{
		gunionXMLPackageHead.isUI = 1;
		// 设置应用ID
		if ((len = UnionReadRequestXMLPackageValue("head/userID",gunionXMLPackageHead.userID,sizeof(gunionXMLPackageHead.userID))) < 0)
		{
			UnionUserErrLog("in UnionInitHeadOfResponseXMLPackage:: UnionReadRequestXMLPackageValue[%s]!\n","head/userID");
			return(len);
		}
		gunionXMLPackageHead.userID[len] = 0;	
		UnionSetResponseXMLPackageValue("head/userID",gunionXMLPackageHead.userID);
	}
	else
	{
		gunionXMLPackageHead.isUI = 0;
		// 设置应用ID
		if ((len = UnionReadRequestXMLPackageValue("head/appID",gunionXMLPackageHead.appID,sizeof(gunionXMLPackageHead.appID))) < 0)
		{
			UnionUserErrLog("in UnionInitHeadOfResponseXMLPackage:: UnionReadRequestXMLPackageValue[%s]!\n","head/appID");
			return(len);
		}
		tmpBuf[len] = 0;	
		UnionSetResponseXMLPackageValue("head/appID",gunionXMLPackageHead.appID);
	}

	// 设置客户端IP地址
	if ((len = UnionReadRequestXMLPackageValue("head/clientIPAddr",gunionXMLPackageHead.clientIPAddr,sizeof(gunionXMLPackageHead.clientIPAddr))) < 0)
	{
		UnionUserErrLog("in UnionInitHeadOfResponseXMLPackage:: UnionReadRequestXMLPackageValue[%s]!\n","head/clientIPAddr");
		return(len);
	}
	gunionXMLPackageHead.clientIPAddr[len] = 0;	
	UnionSetResponseXMLPackageValue("head/clientIPAddr",gunionXMLPackageHead.clientIPAddr);

	// 设置传输时间
	if ((len = UnionReadRequestXMLPackageValue("head/transTime",gunionXMLPackageHead.transTime,sizeof(gunionXMLPackageHead.transTime))) < 0)
	{
		UnionUserErrLog("in UnionInitHeadOfResponseXMLPackage:: UnionReadRequestXMLPackageValue[%s]!\n","head/transTime");
		return(len);
	}
	gunionXMLPackageHead.transTime[len] = 0;	
	UnionSetResponseXMLPackageValue("head/transTime",gunionXMLPackageHead.transTime);

	// 设置传输标志
	if ((len = UnionReadRequestXMLPackageValue("head/transFlag",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionInitHeadOfResponseXMLPackage:: UnionReadRequestXMLPackageValue[%s]!\n","head/transFlag");
		return(len);
	}
	tmpBuf[len] = 0;
	if (tmpBuf[0] != '1')		// 非请求
	{
		UnionUserErrLog("in UnionInitHeadOfResponseXMLPackage:: transFlag[%s] != 1!\n",tmpBuf);
		return(errCodeAPIPackageNotRequest);
	}
	UnionSetResponseXMLPackageValue("head/transFlag","0");

	// 设置用户信息
	if ((len = UnionReadRequestXMLPackageValue("head/userInfo",gunionXMLPackageHead.userInfo,sizeof(gunionXMLPackageHead.userInfo))) > 0)
	{
		gunionXMLPackageHead.userInfo[len] = 0;	
		UnionSetResponseXMLPackageValue("head/userInfo",gunionXMLPackageHead.userInfo);
	}
	else
		gunionXMLPackageHead.userInfo[0] = 0;
	
	if (gunionXMLPackageHead.isUI)
	{
		UnionSetResponseXMLPackageValue("head/hash","0");
		UnionSetResponseXMLPackageValue("head/displayBody","0");
	}
	return(0);
}

PUnionXMLPackageHead UnionGetXMLPackageHead()
{
	return(&gunionXMLPackageHead);
}
