//Author ::linxj
//date	::20140701

#ifndef _MemCachedConf_
#define _MemCachedConf_

#include <libmemcached/memcached.h>

#define MAXNUMOFMEMCACHED 10
#define conMDLNameOfMemcachedConf "UnionMemcachedConfMDL"

typedef struct
{
        char			ipAddr[16];
        int			port;
	int			isBackSvr;
        int			conStats;
	//add by lusj 20160614 保存memcached初始化信息，memcached状态检测时，不进行初始化
        memcached_st *st;
        memcached_return        rc;
        int init_flag;
        //add end

}TUnionMemcachedSvr;
typedef TUnionMemcachedSvr *PUnionMemcachedSvr;

typedef struct
{
	int			realNum;
	PUnionMemcachedSvr	pmemGrp;
}TUnionMemcachedConf;
typedef TUnionMemcachedConf *PUnionMemcachedConf;


int UnionReloadMemcachedConf();

int UnionConnectMemcachedConf();

int UnionCheckMemcachedStatus();

int UnionIsQuit(char *p);

int UnionCheckMemcachedStatus();

extern int UnionSelectRealDBRecordCounts(char *tableName,char *joinCondition,char *condition);

char *UnionPressAnyKey(char *fmt,...);
memcached_st *UnionConnectMemcachedWithDetail(char *ip, int port);

int UnionCheckMemcachedConnectInit(PUnionMemcachedSvr   pmemcached);

int UnionCheckMemcachedConnectOfIPandPort(PUnionMemcachedSvr    pmemcached);

int UnionFlushMemcached_Of_IP(PUnionMemcachedSvr        pmemcached);


int UnionFlushMemcachedNode(PUnionMemcachedSvr  pmemcached);




#endif

