/* COMPONENT_NAME: ESB Front End Adapter                                 	*/
/* Author:	ESB Wen ZhenHua                               								*/
/*                                                                        */
#ifndef __ESB_FRONT_CAPI
#define __ESB_FRONT_CAPI


#include <stdarg.h>
#include "esb_md5.h"
#include "esb_const.h"

/* 该处定义程序是32位还是64位,IS32BITOS为0表示64位,IS32BITOS为1表示32位 */
#ifndef IS32BITPROGRAM
#define IS32BITPROGRAM 0
#endif

/*#define  WINDOWS*/
#ifdef WINDOWS
#include <basetsd.h>
#define  ATOL( lstr )  _atoi64(lstr)
#define  LTOA( lstr , lval)  sprintf( lstr, "%I64d", lval )
#endif

#ifndef WINDOWS
typedef  long long LONG64 ;
#define  ATOL( lstr )  atoll(lstr)
#define  LTOA( lstr , lval)  sprintf( lstr, "%lld", lval )
#endif


/*#define VCLIB*/
#ifdef VCLIB
#define  VCPRE extern "C"
#endif

#ifndef VCLIB
#define  VCPRE 
#endif

/* config function */
VCPRE int GetConfig(const char *key,char* value);
VCPRE int SetConfig(const char *key,char* value);
VCPRE int saveConfig(void);
VCPRE int InitConfig(void);
VCPRE int setUid(void);
VCPRE int getUid(char user[80]);
VCPRE int setMasterKey(void);
VCPRE int getMasterKey(char key[25]);


/* log function */
/* modify by PengLiyang 20140714 与getY4MD函数合并为getY4MDAndHMS函数 */
VCPRE int getY4MDAndHMS(char *pszY4MD, char *pszHMS);
/*VCPRE int getHMS(char *pszHMS);
VCPRE int getY4MD(char *pszY4MD);
*/
VCPRE void esbDebug(char * file, int line,char * format,...);
VCPRE void esbInfo(char * file, int line,char * format,...);
VCPRE void esbWarn(char * file, int line,char * format,...);
VCPRE void esbError(char * file, int line,char * format,...);
VCPRE void esbLog(char * level,char * file, int line,char * format,va_list var1);


/* XML family function*/
VCPRE void initField(struct stField *stTempField);
VCPRE void initField2( struct stField *stTempField );
VCPRE void initNode(struct stNode *stTempNode);
VCPRE void initNode2(struct stNode *stTempNode);
VCPRE void freeField(struct stField *stTempField);
VCPRE void freeNode(struct stNode *stOriginNode);
VCPRE void debugField(struct stField *stTempField);
VCPRE void debugNode(struct stNode *stOriginNode);
VCPRE void debugTree(struct stNode *stOriginNode);

/* unpack XML to stNode */
VCPRE struct stNode *getTreefromXML(char *sXmlMessage,int iXmlMessageLen);
VCPRE struct stNode *getTreefromXMLCode( char *sXmlMessage, int iXmlMessageLen, const int isGbkFlag );
VCPRE struct stNode * xmlToNode(const char * xml ,int len);
VCPRE struct stNode * xmlToNodeCode(const char * xml ,int len, const int isGbkFlag);

VCPRE struct stNode * getXmlHead(const char * xml, int len, int *offset );
VCPRE int dealSubNode(const char * xml,const int len, int *offset,struct stNode * parentNode,struct stNode * compeerNode,char * parentName, const int isGbkFlag);
VCPRE int gotoend1(const char * xml, int len,int *offset);
VCPRE int gotoend2(const char * xml, int len, int * offset, char * nodeName);
VCPRE int istoend(const char * xml, int len, int * offset, char * nodeName);
VCPRE int findSubNode(const char * xml, int len, int *offset );
VCPRE int getNodeName(const char * xml, int len, int *offset,char * nodeName);
VCPRE int getNodeAttr(const char * xml, int len, int *offset,char * attrName,char * attrValue);
VCPRE int forward(const char * xml, int len, int *offset);

/* pack stNode to XML */
VCPRE int putTree2XML( struct stNode *stOriginNode,char *sXmlMessage,int *iXmlMessageLen,int iTabNum);
VCPRE int putTree2XMLCode( struct stNode *stOriginNode, char *sXmlMessage, int *iXmlMessageLen, int iTabNum, int isGbkFlag);
VCPRE int convContToXml(char * sXmlMessage,char * nodeValue,const int nodeLen, const int isGbkFlag);
VCPRE int convContFromXml(char * xmlValue,const int xmlLen, const int isGbkFlag);
VCPRE int isGB18030(char * vString);
VCPRE int getByteNum(char * vString);

/* node */
VCPRE struct stNode * creatNode(const char * nodeName,const char * nodeType,const char * nodeValue,struct stNode * parentNode,struct stNode * compeerNode );
VCPRE void setNodeValue(struct stNode * node ,const char * nodeValue );
VCPRE void setNodeTypeValue(struct stNode * node ,const char * nodeType );
VCPRE void setNodeName(struct stNode * node ,const char * nodeName );

/* create XML Tree */
VCPRE struct stNode *putHeadNode(char *sExplain);
VCPRE struct stNode * createXmlNode();
VCPRE struct stNode * addChildToNode(struct stNode * parent,const char * name,int type,struct stField * field);
VCPRE struct stNode * addChildToName(char * parent,const char * name,int type,struct stField * field);
VCPRE struct stNode * findNodeFromTree(const struct stNode * nodeTree,const char * name);
VCPRE struct stNode * getNodeFromTree(const struct stNode * nodeTree,const char * nodeName);
VCPRE int getValueFromTreeLen(struct stNode * nodeTree,char * nodeName,void * vString, int maxLen);
#ifndef getValueFromTree
#define getValueFromTree(nodeTree,nodeName,vString) getValueFromTreeLen(nodeTree,nodeName,vString,sizeof(vString));
#endif
VCPRE struct stNode * addFieldToNode(struct stNode * nodeTree,const char * parentName,const char * fieldName, const char * fieldType, int fieldLen, int fieldScale,const void * fieldCont);

VCPRE struct stNode * addPinFieldToNode(struct stNode * nodeTree,const char * parentName,const char * fieldName,const  char * fieldType, int fieldLen, int fieldScale, const char * fieldPin, const void * fieldCont);
VCPRE int setFieldEncryptmode(struct stField *stTempField,const char * fieldEncryptmode);

/* field */
VCPRE struct stField * createField(const char * fieldName,const char * type,int len,int scale,const void * value);
VCPRE int setFieldName(struct stField *stTempField,const char * fieldName);
VCPRE int setFieldAttr(struct stField *stTempField,const char * type,int len,int scale);
VCPRE int setFieldValue(struct stField *stTempField, const void *vString);
#ifndef getFieldValue
#define getFieldValue(stTempField,vString) getFieldValueLen(stTempField,vString,sizeof(vString));
#endif
VCPRE int getFieldValueLen(struct stField *stTempField, void *vString, int maxLen);

/* conv */
VCPRE int convFieldToNode(const struct stField * srcField,struct stNode * destNode);
VCPRE int convNodeToField(const struct stNode * srcNode,struct stField * destField);

VCPRE long ESB_FileSize(FILE *stream);
VCPRE void ESB_File2String(FILE *stream, char * buf);

/********************** tcpcomm.c function********************************/
VCPRE int dotrans(char *input, char* output, long* len);
/* 带指定交易超时时间的dotrans， add by PengLiyang 20150122 */
VCPRE int dotransTimeOut(char * input,char* output, long * len, const int t_out);
/* 添加交易超时时间参数 modify by PengLiyang 20150122 */
VCPRE int docomm(char * head, char * input, char* output, long* len,const int t_out);
VCPRE int encodeXmlPack(char * input,char * out_value,long * out_len);
VCPRE int decodeWorkKey(char * workey,char * out_value,long *out_len);
VCPRE void CatchSigAlrm(int sig);
VCPRE int Conn2Serv(unsigned int addr, int port, int *sock, int timeOut);
VCPRE int IsReadyToRead(int sock, int timeOut );
VCPRE int IsReadyToWrite(int sock, int timeOut );
VCPRE int ReadPacket(int sock, char *dataBuff, long* olen, int timeOut);
VCPRE int WritePacket(int sock, const char *dataBuff, int length, int timeOut);
VCPRE int Send2Serv(int sock, const char *data, int len, int timeout);
VCPRE int checkTransStatus(struct stNode *nodeTree);
VCPRE int initSock(void);
VCPRE void closeSock(int sock);

/******************* tools.c function*************************************/
VCPRE int free_Key(struct stKey * freeKey);
VCPRE int debug_Key(struct stKey * debugKey);
VCPRE int setEsbKey(char * macKey,char * pinKey,char * workDate,long ltime);
VCPRE int packHeadStr(char * input,char * head,int * olen,int flag);
VCPRE int unpackHeadStr(char * rspMsg,int rspLen,char * head,int *headLen,char * xml ,int * xmlLen);
VCPRE int synWorkKey();
VCPRE int findValue(const char * input , char * key ,char * value);
VCPRE int printBytes(char * data,int len,char * name);
VCPRE int decodeKey(unsigned char * indata,int inLen,unsigned char * outdata,int * outLen);
VCPRE int open_base64_encode(const unsigned char *in, int len, unsigned char * out,int * olen);
VCPRE int open_base64_decode(unsigned char *in,int len,char * out,int * olen);
VCPRE int decodeKeyValue(char * key,char * out_value,int *out_len);
VCPRE int updateWorkkey(char * head);
VCPRE int encodeXmlPack(char * input,char * out_value,long * out_len);
VCPRE char * StrPack( char * str );
VCPRE char * GetTimeStamp(char *TimeStamp);


VCPRE int encryptFieldCont(char * outdata, long * outlen, const char *indata , const long inlen);
VCPRE int decryptFieldCont(char * outdata, long * outlen, const char *indata , const long inlen);

VCPRE int getXmlStr(const char * inData,long inDataLen,char * xml ,long * xmlLen,int * checkResult, char * pinKey);
VCPRE int getEncryptFieldCont(const char *inFieldData , const long inlen, char * outFieldData, long * outlen);
VCPRE int getDecryptFieldCont(const char *inFieldData , const long inlen, const char * pinKey, char * outFieldData, long * outlen);
VCPRE int synWorkKey2(const char *recvHead, const int flag);

VCPRE long getMillitmSystemTime();

/*****************  base64.c function ************************************/
VCPRE int base64_encode(unsigned char * bin_data,long bin_size, char * base64_data);
VCPRE int base64_decode(char * base64_data, unsigned char * bin_data,long * bin_size);


/***************** file.c function ***************************************/
VCPRE int ftpget(char * remoteFileName,char * localFileName, char * localFileMd5, int scrtFlag);
VCPRE int ftpput(char * remoteFileName,char * localFileName, char * localFileMd5, int scrtFlag);
VCPRE int cleanResource(stFile * file,stFileBean * bean);
VCPRE int doPutFile(stFile * file,stFileBean * bean);
VCPRE int doGetFile(stFile * file,stFileBean * bean);
VCPRE int writeHead(stFile * file,stFileBean * bean);
VCPRE int readHead(stFile * file,stFileBean * bean);
VCPRE int readCont(stFile * file,stFileBean * bean);
VCPRE int writeCont(stFile * file,stFileBean * bean);
VCPRE int readFile(stFile * file,stFileBean * bean);
VCPRE int writeFile(stFile * file,stFileBean * bean);
VCPRE int finishFile(stFile * file,stFileBean * bean);
VCPRE int citoc(int k,unsigned char* value);
VCPRE int cctoi(int * k,char* value);
VCPRE int goToStr(char * xml,int * offset,int iLen,char * strValue);
VCPRE int getOneField(char * xml,int * offset,int iLen,char * name,char * value);
VCPRE int setDataToBean(stFileBean * bean,char * name,char * value);
VCPRE stFile * initFile(char * remoteFileName,char* localFileName,int putFlag);
VCPRE stFileBean * initFileBean(char * remoteFileName,char * localFileName);
VCPRE int doAuth(stFile * file,stFileBean * bean);
VCPRE int doConnect(stFile * file,stFileBean * bean);
VCPRE void convertBeanToXml(stFileBean * bean);
VCPRE void appendToXml(char * xml,int * offset,char * name,char * value);
VCPRE void convertXmlToBean(stFileBean * bean);


VCPRE int checkResponseSeries(char * head,char * output);

VCPRE int encrypt3des(char * outdata, long * outlen, const char *indata , const long inlen, const char * key);
VCPRE int decrypt3des(char * outdata, long * outlen, const char *indata,const long inlen, const char * key);

VCPRE int send_recv(char *upbuf, char *downbuf, char *errmsg);
VCPRE int communicate(char *upbuf_c, char *downbuf_c, char *errmsg);

/* add by sfg 2014.7.12 使用到esbapi的程序必须首先调用一次且仅能调用一次来InitEsbApi初始化相线程关锁 */
VCPRE void InitEsbApi( void );	
/* 可由应用进程在初始化时指定API配置文件路径，此方法须在调用InitEsbApi之后调用 add  by PengLiyang 2015012 */
VCPRE int SetCfgFilePath(const char * cfgFilePath);
#endif 

