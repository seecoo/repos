//	Author: zhangyongding
//	Date: 2012-10-19

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libmemcached/memcached.h>
//#include <libmemcached/memcached_util.h> 



#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionREC.h"

#include "highCached.h"
#include "unionMemcached.h"
#include "unionErrCode.h"

//add by lusj 20160106
#include "unionMemcachedConf.h"

#define	defTableNameOfSymmetricKeyDB		"symmetricKeyDB"
//add enf by lusj 	

int				gunionMemcachedConnected = 0;
//int				gunionMemcachedMaster_num = 0;
//int				gunionMemcachedBackup_num = 0;
memcached_st			*gunionMemc_master = NULL;
memcached_st			*gunionMemc_backup = NULL;
//memcached_pool_st		*gunionPool_master = NULL;
//memcached_pool_st		*gunionPool_backup = NULL;

//add by lusj 20160106
extern PUnionMemcachedConf		pgunionMemConf;
extern PUnionMemcachedSvr		pgunionMemcachedSvr;
//add enf by lusj 	

typedef struct
{
	char*	inkey;		// ����key��
	char*	outkey;		// ���Key�б�
	int	outSize;	// ���Key�����С
	int	outlen;		// ���Key����
} TUnionMemcachedDump;
typedef TUnionMemcachedDump	*PUnionMemcachedDump;

int UnionSetMemcachedStatus(memcached_st *shell,const char *group_key, size_t group_key_length,memcached_return rc);

int UnionGetMemcachedStatus(memcached_st *shell,const char *group_key, size_t group_key_length);

void UnionSetMemcachedBehavior(memcached_st *memc_st)
{
	int	timeout;

	if ((timeout = UnionReadIntTypeRECVar("timeoutOfHighCached")) < 100)
		timeout = 100;

	UnionLog("in UnionSetMemcachedBehavior:: timeout[%d]\n",timeout);

	// ʹ��NO-BLOCK����ֹmemcache����ʱ����
	memcached_behavior_set(memc_st, MEMCACHED_BEHAVIOR_NO_BLOCK, 1);
	// ���ӳ�ʱ������
	memcached_behavior_set(memc_st, MEMCACHED_BEHAVIOR_CONNECT_TIMEOUT, 100);		
	memcached_behavior_set(memc_st, MEMCACHED_BEHAVIOR_POLL_TIMEOUT, 100);

	// ��ʱ����ʱ��(��)
	memcached_behavior_set(memc_st, MEMCACHED_BEHAVIOR_RETRY_TIMEOUT, 1) ;

	// dead server���Զ�������Զ�����
	memcached_behavior_set(memc_st, MEMCACHED_BEHAVIOR_DISTRIBUTION,MEMCACHED_DISTRIBUTION_CONSISTENT);
	// ��ʱ����ʱ��(�룩�����������ʱ������������dead��server
	memcached_behavior_set(memc_st, MEMCACHED_BEHAVIOR_DEAD_TIMEOUT,60) ;
	memcached_behavior_set(memc_st, MEMCACHED_BEHAVIOR_SERVER_FAILURE_LIMIT, 1) ;
	memcached_behavior_set(memc_st, MEMCACHED_BEHAVIOR_AUTO_EJECT_HOSTS, true) ;

	// ���ͳ�ʱ��΢��
	memcached_behavior_set(memc_st, MEMCACHED_BEHAVIOR_SND_TIMEOUT, timeout);
	// ���ճ�ʱ��΢��
	memcached_behavior_set(memc_st, MEMCACHED_BEHAVIOR_RCV_TIMEOUT, timeout);

	memcached_behavior_set(memc_st, MEMCACHED_BEHAVIOR_BINARY_PROTOCOL, 0);
}
// ����Memcached
int UnionConnectMemcached()
{
	int			i;
	int			master_num = 0;
	int			backup_num = 0;
	memcached_return	rc;
	memcached_server_st	*master_servers = NULL;   
	memcached_server_st	*backup_servers = NULL;   
	PUnionHighCachedTBL	phighCachedTBL = NULL;

	if (gunionMemcachedConnected)
		return(1);

	// ����Ƿ�ʹ�ø��ٻ���
	if (UnionReadIntTypeRECVar("isUseHighCached") <= 0)
		return(0);

	// ��ȡ���ٻ��涨��
	if ((phighCachedTBL = UnionGetHighCachedTBL()) == NULL)
	{
		UnionUserErrLog("in UnionConnectMemcached:: UnionGetHighCachedTBL!\n");
		return(errCodeParameter);
	}

	for (i = 0; i < phighCachedTBL->realNum; i++)
	{
		// δ����
		if (!phighCachedTBL->rec[i].enabled)
			continue;

		if (phighCachedTBL->rec[i].master)
		{
			if (gunionMemc_master == NULL)
				gunionMemc_master = memcached_create(NULL);

			master_servers = memcached_server_list_append(master_servers, phighCachedTBL->rec[i].ipAddr,phighCachedTBL->rec[i].port, &rc);
			if (rc != MEMCACHED_SUCCESS)
			{
				UnionUserErrLog("in UnionConnectMemcached:: memcached_server_list_append[%s:%d] master rc = [%d] msg = [%s]!\n",phighCachedTBL->rec[i].ipAddr,phighCachedTBL->rec[i].port,rc,memcached_strerror(gunionMemc_master,rc));
				return(errCodeOffsetOfMemcached - abs(rc));
			}
			/*
			   if ((rc = memcached_server_add(gunionMemc_master,phighCachedTBL->rec[i].ipAddr,phighCachedTBL->rec[i].port)) != MEMCACHED_SUCCESS)
			   {
			   UnionUserErrLog("in UnionConnectMemcached:: memcached_server_add[%s:%d] master rc = [%d]!\n",phighCachedTBL->rec[i].ipAddr,phighCachedTBL->rec[i].port,rc);
			   return(errCodeOffsetOfMemcached - abs(rc));
			   }
			 */
			master_num ++;
		}
		else
		{
			if (gunionMemc_backup == NULL)
				gunionMemc_backup = memcached_create(NULL);

			backup_servers = memcached_server_list_append(backup_servers, phighCachedTBL->rec[i].ipAddr,phighCachedTBL->rec[i].port, &rc);
			if (rc != MEMCACHED_SUCCESS)
			{
				UnionUserErrLog("in UnionConnectMemcached:: memcached_server_list_append[%s:%d] backup rc = [%d] msg = [%s]!\n",phighCachedTBL->rec[i].ipAddr,phighCachedTBL->rec[i].port,rc,memcached_strerror(gunionMemc_backup,rc));
				return(errCodeOffsetOfMemcached - abs(rc));
			}

			/*
			   if ((rc = memcached_server_add(gunionMemc_backup,phighCachedTBL->rec[i].ipAddr,phighCachedTBL->rec[i].port)) != MEMCACHED_SUCCESS)
			   {
			   UnionUserErrLog("in UnionConnectMemcached:: memcached_server_add[%s:%d] backup rc = [%d]!\n",phighCachedTBL->rec[i].ipAddr,phighCachedTBL->rec[i].port,rc);
			   return(errCodeOffsetOfMemcached - abs(rc));
			   }
			 */
			backup_num ++;
		}
	}
	if (gunionMemc_master)
	{
		rc = memcached_server_push(gunionMemc_master, master_servers);
		memcached_server_free(master_servers);
		UnionSetMemcachedBehavior(gunionMemc_master);
		gunionMemcachedConnected = 1;
	}

	if (gunionMemc_backup)
	{
		rc = memcached_server_push(gunionMemc_backup, backup_servers);
		memcached_server_free(backup_servers);
		UnionSetMemcachedBehavior(gunionMemc_backup);
	}

	UnionProgramerLog("in UnionConnectMemcached:: master_num[%d] backup_num[%d]\n",master_num,backup_num);
	return(master_num); // modify by leipp Դ�̶�Ϊ1���ȸ�Ϊ[master_num]��ԭ��:���û��һ������ģ��ᵼ�º���ʹ�õ�ʱ�����core��
}

// ʹ�ö����Ự����Memcached
int UnionConnectMemcachedForOwnSession(memcached_st *memc_master,memcached_st *memc_backup, int *master_num, int *backup_num)
{
	int			i;
	memcached_return	rc;
	memcached_server_st	*master_servers = NULL;   
	memcached_server_st	*backup_servers = NULL;   
	PUnionHighCachedTBL	phighCachedTBL = NULL;

	// ����Ƿ�ʹ�ø��ٻ���
	if (UnionReadIntTypeRECVar("isUseHighCached") <= 0)
		return(0);

	// ��ȡ���ٻ��涨��
	if ((phighCachedTBL = UnionGetHighCachedTBL()) == NULL)
	{
		UnionUserErrLog("in UnionConnectMemcachedForOwnSession:: UnionGetHighCachedTBL!\n");
		return(errCodeParameter);
	}

	*master_num = 0;
	*backup_num = 0;

	for (i = 0; i < phighCachedTBL->realNum; i++)
	{
		// δ����
		if (!phighCachedTBL->rec[i].enabled)
			continue;

		if (phighCachedTBL->rec[i].master)
		{
			/*
			   if (memc_master == NULL)
			   memc_master = memcached_create(NULL);
			 */
			master_servers = memcached_server_list_append(master_servers, phighCachedTBL->rec[i].ipAddr,phighCachedTBL->rec[i].port, &rc);
			if (rc != MEMCACHED_SUCCESS)
			{
				UnionUserErrLog("in UnionConnectMemcachedForOwnSession:: memcached_server_list_append[%s:%d] master rc = [%d] msg = [%s]!\n",phighCachedTBL->rec[i].ipAddr,phighCachedTBL->rec[i].port,rc,memcached_strerror(gunionMemc_master,rc));
				return(errCodeOffsetOfMemcached - abs(rc));
			}
			*master_num += 1;
		}
		else
		{
			/*
			   if (memc_backup == NULL)
			   memc_backup = memcached_create(NULL);
			 */
			backup_servers = memcached_server_list_append(backup_servers, phighCachedTBL->rec[i].ipAddr,phighCachedTBL->rec[i].port, &rc);
			if (rc != MEMCACHED_SUCCESS)
			{
				UnionUserErrLog("in UnionConnectMemcachedForOwnSession:: memcached_server_list_append[%s:%d] backup rc = [%d] msg = [%s]!\n",phighCachedTBL->rec[i].ipAddr,phighCachedTBL->rec[i].port,rc,memcached_strerror(gunionMemc_backup,rc));
				return(errCodeOffsetOfMemcached - abs(rc));
			}			
			*backup_num += 1;
		}
	}
	if (*master_num > 0)
	{
		rc = memcached_server_push(memc_master, master_servers);
		memcached_server_free(master_servers);
		//UnionSetMemcachedBehavior(memc_master);
	}

	if (*backup_num > 0)
	{
		rc = memcached_server_push(memc_backup, backup_servers);
		memcached_server_free(backup_servers);
		//UnionSetMemcachedBehavior(memc_backup);
	}

	UnionProgramerLog("in UnionConnectMemcachedForOwnSession:: master_num[%d] backup_num[%d]\n",*master_num,*backup_num);
	return(1);
}

// �ر�Memcached
int UnionCloseMemcached()
{
	if (!gunionMemcachedConnected)
	{
		//UnionProgramerLog("in UnionCloseMemcached:: Already disconnected to the  Memcached Server !\n");
		return(0);
	}

	if (gunionMemc_master)
	{
		memcached_free(gunionMemc_master);
		gunionMemc_master = NULL;
	}
	if (gunionMemc_backup)
	{
		memcached_free(gunionMemc_backup);
		gunionMemc_backup = NULL;
	}
	gunionMemcachedConnected = 0;
	return(0);
}

int UnionCloseOneMemcached(memcached_st *server)
{
	if (server)
	{
		memcached_free(server);
		server = NULL;
	}

	return(0);
}

// ʹ�ö����Ự�ر�Memcached
int UnionCloseMemcachedForOwnSession(memcached_st *memc_master,memcached_st *memc_backup)
{
	if (memc_master)
		memcached_free(memc_master);
	if (memc_backup)
		memcached_free(memc_backup);
	return(0);
}

// ���Memcached
int UnionFlushMemcached()
{
	int			ret;
	int			master_ret = -1;
	int			backup_ret = -1;
	memcached_return	rc;
	time_t			expiration = 0;

	if ((ret = UnionConnectMemcached()) < 0)
	{
		UnionUserErrLog("in UnionFlushMemcached:: UnionConnectMemcached!\n");
		return(ret);
	}
	else if (ret == 0)
		return(ret);

	if (gunionMemc_master)
	{
		if ((rc = memcached_flush(gunionMemc_master,expiration)) != MEMCACHED_SUCCESS)
		{
			UnionUserErrLog("in UnionFlushMemcached:: memcached_flush master rc = [%d] msg = [%s]!\n",rc,memcached_strerror(gunionMemc_master,rc));
			master_ret = errCodeOffsetOfMemcached - abs(rc);
		}
		else
			master_ret = 0;
	}
	if (gunionMemc_backup)
	{
		if ((rc = memcached_flush(gunionMemc_backup,expiration)) != MEMCACHED_SUCCESS)
		{
			UnionUserErrLog("in UnionFlushMemcached:: memcached_flush backup rc = [%d] msg = [%s]!\n",rc,memcached_strerror(gunionMemc_backup,rc));
			backup_ret = errCodeOffsetOfMemcached - abs(rc);
		}
		else
			backup_ret = 0;
	}
	if ((master_ret < 0) && (backup_ret < 0))
	{
		UnionUserErrLog("in UnionFlushMemcached:: master_ret = [%d] backup_ret = [%d]!\n",master_ret,backup_ret);		
		return(master_ret);
	}
	else
		return(1);
}

// ����Memcachedֵ
int UnionSetMemcachedValue(char *key,char *value,int lenOfValue,int expiration)
{
	int			ret;
	int			master_ret = -1;
	int			backup_ret = -1;
	uint32_t		flags = 0;
	memcached_return	rc;

	if ((ret = UnionConnectMemcached()) < 0)
	{
		UnionUserErrLog("in UnionSetMemcachedValue:: UnionConnectMemcached!\n");
		return(ret);
	}
	else if (ret == 0)
		return(ret);

	if ((rc = memcached_set(gunionMemc_master, key, strlen(key), value, lenOfValue, expiration, flags)) != MEMCACHED_SUCCESS)
	{
		UnionUserErrLog("in UnionSetMemcachedValue:: memcached_set master rc = [%d] msg = [%s]!\n",rc,memcached_strerror(gunionMemc_master,rc));
		master_ret = errCodeOffsetOfMemcached - abs(rc);

		// add by lusj 20160106 ״̬�޸�
		UnionSetMemcachedStatus(gunionMemc_master, key, strlen(key),rc);
		//add end by lusj 
	}
	else
	{
		master_ret = 0;

		// add by lusj 20160106 ״̬�޸�
		UnionSetMemcachedStatus(gunionMemc_master, key, strlen(key),rc);
		//add end by lusj 
	}

	// modify by leipp 20151214
	if (master_ret < 0)
	{
		UnionUserErrLog("in UnionSetMemcachedValue:: master_ret = [%d]!\n",master_ret);		
		//modify by lusj 20160107 ״̬��Ϊ�쳣�����طǸ������ϲ�Ӧ�ø������ݿ�,�������ڱ�memcached�����memcached����
	//	return(master_ret);
		//modify end by lusj 
	}
	// modify end

	if (gunionMemc_backup)
	{
		if ((rc = memcached_set(gunionMemc_backup, key, strlen(key), value, lenOfValue, expiration, flags)) != MEMCACHED_SUCCESS)
		{
			UnionUserErrLog("in UnionSetMemcachedValue:: memcached_set backup rc = [%d] msg =[%s]!\n",rc,memcached_strerror(gunionMemc_backup,rc));
			backup_ret = errCodeOffsetOfMemcached - abs(rc);

			// add by lusj 20160106 ״̬�޸�
			UnionSetMemcachedStatus(gunionMemc_backup, key, strlen(key),rc);
			//add end by lusj 
		}
		else
		{
			backup_ret = 0;

			// add by lusj 20160106 ״̬�޸�
			UnionSetMemcachedStatus(gunionMemc_backup, key, strlen(key),rc);
			//add end by lusj 
		}

		// modify by leipp 20151214
		if (backup_ret < 0)
		{
			UnionUserErrLog("in UnionSetMemcachedValue:: backup_ret = [%d]!\n",backup_ret);		
			UnionDeleteMemcachedValue(key);
		//	return(backup_ret);
		        //modify by lusj 20160107 ״̬��Ϊ�쳣�����طǸ������ϲ�Ӧ�ø������ݿ�
			return(lenOfValue);
               	 	//modify end by lusj 	
		}
	}

	return(lenOfValue);
}

// ��ȡMemcachedֵ
int UnionGetMemcachedValue(char *key,char *value,int sizeofBuf)
{
	int		ret;
	size_t		lenOfValue = 0;
	uint32_t	flags = 0;
	char		*result = NULL;
	memcached_return	rc;
	int		num = 0;

	//����һ�� add by linxj 20151229
	while(num <=1)
	{
		if ((ret = UnionConnectMemcached()) < 0)
		{
			UnionUserErrLog("in UnionGetMemcachedValue:: UnionConnectMemcached!\n");
			return(ret);
		}
		else if (ret == 0)
			return(ret);
		//add begin by lusj 20160106 ���Ա�memcached����get����
		if ((ret = UnionGetMemcachedStatus(gunionMemc_master, key, strlen(key))) == 0)
		{
			UnionUserErrLog("in UnionGetMemcachedValue:: UnionGetMemcachedStatus:: gunionMemc_master_Status = 0!\n");
			if (gunionMemc_backup)
			{
				//add begin by lusj 20160106 ���Ա�memcached����get����
				if ((ret = UnionGetMemcachedStatus(gunionMemc_backup, key, strlen(key))) == 0)
				{
					UnionUserErrLog("in UnionGetMemcachedValue:: UnionGetMemcachedStatus:: gunionMemc_backup status= 0!\n");
					UnionCloseMemcached();
					return(errCodeSckCommMDL_InvalidWorkingMode);
				}
				//add end by lusj 

				result = memcached_get(gunionMemc_backup, key, strlen(key), &lenOfValue, &flags, &rc);
				if (rc != MEMCACHED_SUCCESS)
				{
					if (rc != MEMCACHED_NOTFOUND)
					{
						UnionUserErrLog("in UnionGetMemcachedValue:: memcached_get backup rc = [%d] msg = [%s]!\n",rc,memcached_strerror(gunionMemc_backup,rc));

						// add by leipp 20151111,���Ͽ����ӻ���ִ������
						//if (rc != MEMCACHED_SERVER_TEMPORARILY_DISABLED)
						UnionCloseMemcached();
						// add end
					}
					//modify begin  by lusj 20151231 ��һ�����������ɹ������˳������ǶϿ�memcached�� ���в��� 
					if(num > 0)	
						return(errCodeOffsetOfMemcached - abs(rc));
					//modify end by lusj 20151231
				}// add begin by lusj 20151231   ��memcached��һ�λ�ȡ�ɹ���ֱ�ӷ��أ����������Բ���
				else if (rc == MEMCACHED_SUCCESS)
				{
					if (lenOfValue > sizeofBuf - 1)
						lenOfValue = sizeofBuf - 1;

					memcpy(value,result,lenOfValue);
					free(result);
					return(lenOfValue);
				}
				//add end by lusj 20151231
			}
			num++;
		}
		//add end by lusj 

		result = memcached_get(gunionMemc_master, key, strlen(key), &lenOfValue, &flags, &rc);

		if (rc != MEMCACHED_SUCCESS)
		{

			if (rc != MEMCACHED_NOTFOUND)
			{
				UnionUserErrLog("in UnionGetMemcachedValue:: memcached_get master rc = [%d] msg = [%s]!\n",rc,memcached_strerror(gunionMemc_master,rc));
			}

			if (gunionMemc_backup)
			{
				//add begin by lusj 20160106 ���Ա�memcached����get����
				if ((ret = UnionGetMemcachedStatus(gunionMemc_backup, key, strlen(key))) == 0)
				{
					UnionUserErrLog("in UnionGetMemcachedValue:: UnionGetMemcachedStatus:: gunionMemc_backup status= 0!\n");
					UnionCloseMemcached();
					return(errCodeSckCommMDL_InvalidWorkingMode);
				}
				//add end by lusj 

				result = memcached_get(gunionMemc_backup, key, strlen(key), &lenOfValue, &flags, &rc);
				if (rc != MEMCACHED_SUCCESS)
				{
					if (rc != MEMCACHED_NOTFOUND)
					{
						UnionUserErrLog("in UnionGetMemcachedValue:: memcached_get backup rc = [%d] msg = [%s]!\n",rc,memcached_strerror(gunionMemc_backup,rc));

						// add by leipp 20151111,���Ͽ����ӻ���ִ������
						//if (rc != MEMCACHED_SERVER_TEMPORARILY_DISABLED)
						UnionCloseMemcached();
						// add end
					}
					//modify begin  by lusj 20151231 ��һ�����������ɹ������˳������ǶϿ�memcached�� ���в��� 
					if(num > 0)	
						return(errCodeOffsetOfMemcached - abs(rc));
					//modify end by lusj 20151231
				}// add begin by lusj 20151231   ��memcached��һ�λ�ȡ�ɹ���ֱ�ӷ��أ����������Բ���
				else if (rc == MEMCACHED_SUCCESS)
				{
					if (lenOfValue > sizeofBuf - 1)
						lenOfValue = sizeofBuf - 1;

					memcpy(value,result,lenOfValue);
					free(result);
					return(lenOfValue);
				}
				//add end by lusj 20151231

			}
			else
			{
				// add by leipp 20151111,���Ͽ����ӻ���ִ������
				if (rc != MEMCACHED_NOTFOUND)
					UnionCloseMemcached();
				// add end

				//modify begin  by lusj 20151231 ��һ�������ɹ������˳��� �������Բ���
				if (num > 0)
				{
					return(errCodeOffsetOfMemcached - abs(rc));
				}
				//modify end by lusj 20151231
			}
		}// add begin by lusj 20151231   ��memcached��һ�λ�ȡ�ɹ���ֱ�ӷ��أ����������Բ���     
		else if (rc == MEMCACHED_SUCCESS)
		{
			if (lenOfValue > sizeofBuf - 1)
				lenOfValue = sizeofBuf - 1;

			memcpy(value,result,lenOfValue);
			free(result);
			return(lenOfValue);	
		}
		//add end by lusj 20151231
		num++;
	}

	return(lenOfValue);
}

// ɾ��Memcachedֵ
int UnionDeleteMemcachedValue(char *key)
{
	int		ret;
	int		master_ret = -1;
	int		backup_ret = -1;
	time_t		expiration = 0;
	memcached_return	rc;

	if ((ret = UnionConnectMemcached()) < 0)
	{
		UnionUserErrLog("in UnionDeleteMemcachedValue:: UnionConnectMemcached!\n");
		return(ret);
	}
	else if (ret == 0)
		return(ret);

	if ((rc = memcached_delete(gunionMemc_master, key, strlen(key), expiration)) != MEMCACHED_SUCCESS)
	{
		if (rc != MEMCACHED_NOTFOUND)
		{
			UnionUserErrLog("in UnionDeleteMemcachedValue:: memcached_delete master rc = [%d] msg = [%s]!\n",rc,memcached_strerror(gunionMemc_master,rc));
			master_ret = errCodeOffsetOfMemcached - abs(rc);
		}
		else
			master_ret = 0;
	}
	else
		master_ret = 0;

	if (gunionMemc_backup)
	{
		if ((rc = memcached_delete(gunionMemc_backup, key, strlen(key), expiration)) != MEMCACHED_SUCCESS)
		{
			if (rc != MEMCACHED_NOTFOUND)
			{
				UnionUserErrLog("in UnionDeleteMemcachedValue:: memcached_delete backup rc = [%d] msg = [%s]!\n",rc,memcached_strerror(gunionMemc_backup,rc));
				backup_ret = errCodeOffsetOfMemcached - abs(rc);
			}
			else
				backup_ret = 0;
		}
		else
			backup_ret = 0;
	}

	if ((master_ret < 0) && (backup_ret < 0))
	{
		UnionUserErrLog("in UnionDeleteMemcachedValue:: master_ret = [%d] backup_ret = [%d]!\n",master_ret,backup_ret);		
		return(master_ret);
	}
	else
		return(1);
}

//����Ƿ����
int UnionExistMemcachedValue(char *key)
{
	int			ret;
	memcached_return	rc;

	if ((ret = UnionConnectMemcached()) < 0)
	{
		UnionUserErrLog("in UnionExistMemcachedValue:: UnionConnectMemcached!\n");
		return(ret);
	}
	else if (ret == 0)
		return(ret);

	if ((rc = memcached_exist(gunionMemc_master, key, strlen(key))) != MEMCACHED_SUCCESS)
	{
		if (rc != MEMCACHED_NOTFOUND)
			UnionUserErrLog("in UnionExistMemcachedValue:: memcached_exist master rc = [%d] msg = [%s]!\n",rc,memcached_strerror(gunionMemc_master,rc));

		if (gunionMemc_backup)
		{
			if ((rc = memcached_exist(gunionMemc_backup, key, strlen(key))) != MEMCACHED_SUCCESS)
			{
				if (rc != MEMCACHED_NOTFOUND)
				{
					UnionUserErrLog("in UnionExistMemcachedValue:: memcached_exist backup rc = [%d] msg = [%s]!\n",rc,memcached_strerror(gunionMemc_backup,rc));
					return(errCodeOffsetOfMemcached - abs(rc));
				}
				else
					return(0);
			}
		}
		else
		{
			if (rc != MEMCACHED_NOTFOUND)
				return(errCodeOffsetOfMemcached - abs(rc));
			else
				return(0);
		}
	}
	return(1);
}

// ��ѯ��ֵ�ص�����
memcached_return_t callback_dump_get_keys(const memcached_st *ptr,const char *key,size_t key_length,void *context)
{
	PUnionMemcachedDump	pmemDump = (PUnionMemcachedDump)context;

	//if ((pmemDump->inkey == NULL) || (strlen(pmemDump->inkey) == 0) || strstr((char *)key,pmemDump->inkey) != NULL)
	if (UnionStrComp((char *)key,pmemDump->inkey))
	{
		if ((pmemDump->outlen + key_length + 1) < pmemDump->outSize)
			pmemDump->outlen += sprintf(pmemDump->outkey + pmemDump->outlen,"%s,",key);
	}

	return MEMCACHED_SUCCESS;
}

/* 
   ���ܣ�	������ָ��ģʽ��ƥ������õ�keys
   ������	reqKey[in]		�������
   resKeys[out]		��Ӧ�����б����ŷָ�
   sizeofBuf[in]		��Ӧ�����б��С
   ����ֵ��>=0			�ɹ�������������
   <0			ʧ�ܣ����ش�����
 */
int  UnionGetMemcachedKeysByAlikeName(char *reqKey,char *resKeys,int sizeofBuf)
{
	int			ret;
	int			len = 0;
	int			master_num = 0;
	int			backup_num = 0;
	int			master_ret = -1;
	int			backup_ret = -1;
	char			tmpKey[1024];
	memcached_return	rc;
	memcached_st 		*memc_master = NULL;
	memcached_st		*memc_backup = NULL;
	memcached_dump_fn 	fnCall;
	TUnionMemcachedDump	tmemDump;

	memc_master = memcached_create(NULL);
	memc_backup = memcached_create(NULL);

	if ((ret = UnionConnectMemcachedForOwnSession(memc_master,memc_backup,&master_num,&backup_num)) < 0)
	{
		UnionUserErrLog("in UnionGetMemcachedKeysByAlikeName:: UnionConnectMemcachedForOwnSession ret = [%d]!\n",ret);
		UnionCloseMemcachedForOwnSession(memc_master,memc_backup);	
		return(ret);
	}

	fnCall = callback_dump_get_keys;

	// ����ͨ���
	if ((reqKey == NULL) || (strlen(reqKey) == 0))
		snprintf(tmpKey,sizeof(tmpKey),"%s","*");
	else
		snprintf(tmpKey,sizeof(tmpKey),"*%s*",reqKey);


	// �������������
	len += snprintf(resKeys + len,sizeofBuf - len,"*** ������ ***,");

	if (master_num > 0)
	{
		memcached_stat(memc_master,NULL,&rc);
		if (rc != MEMCACHED_SUCCESS)
		{
			UnionUserErrLog("in UnionGetMemcachedKeysByAlikeName:: memcached_stat memc_master rc = [%d] msg = [%s]!\n",rc,memcached_strerror(memc_master,rc));
			master_ret = errCodeOffsetOfMemcached - abs(rc);
		}
		else
		{
			memset(&tmemDump,0,sizeof(tmemDump));
			tmemDump.inkey = tmpKey;
			tmemDump.outkey = resKeys + len;
			tmemDump.outSize = sizeofBuf - len;
			tmemDump.outlen = 0;

			if ((rc = memcached_dump(memc_master,&fnCall,&tmemDump,1)) != MEMCACHED_SUCCESS)
			{
				UnionUserErrLog("in UnionGetMemcachedKeysByAlikeName:: memcached_dump memc_master rc = [%d] msg = [%s]!\n",rc,memcached_strerror(memc_master,rc));
				master_ret = errCodeOffsetOfMemcached - abs(rc);
			}
			else
			{
				master_ret = 0;
				len += tmemDump.outlen;
			}
		}
	}

	// �������������
	len += snprintf(resKeys + len,sizeofBuf - len,"*** ������ ***,");

	if (backup_num)
	{
		memcached_stat(memc_backup,NULL,&rc);
		if (rc != MEMCACHED_SUCCESS)
		{
			UnionUserErrLog("in UnionGetMemcachedKeysByAlikeName:: memcached_stat memc_backup rc = [%d] msg = [%s]!\n",rc,memcached_strerror(memc_backup,rc));
			backup_ret = errCodeOffsetOfMemcached - abs(rc);
		}
		else
		{
			memset(&tmemDump,0,sizeof(tmemDump));
			tmemDump.inkey = tmpKey;
			tmemDump.outkey = resKeys + len;
			tmemDump.outSize = sizeofBuf - len;
			tmemDump.outlen = 0;
			if ((rc = memcached_dump(memc_backup,&fnCall,&tmemDump,1)) != MEMCACHED_SUCCESS)
			{
				UnionUserErrLog("in UnionGetMemcachedKeysByAlikeName:: memcached_dump memc_backup rc = [%d] msg = [%s]!\n",rc,memcached_strerror(memc_backup,rc));
				backup_ret = errCodeOffsetOfMemcached - abs(rc);
			}
			else
			{
				backup_ret = 0;
				len += tmemDump.outlen;
			}
		}
	}

	UnionCloseMemcachedForOwnSession(memc_master,memc_backup);	

	if ((master_ret < 0) && (backup_ret < 0))
	{
		UnionUserErrLog("in UnionGetMemcachedKeysByAlikeName:: master_ret = [%d] backup_ret = [%d]!\n",master_ret,backup_ret);		
		return(master_ret);
	}
	else
		return(len);
}

// ɾ����ֵ�ص�����
memcached_return_t callback_dump_delte_keys(const memcached_st *ptr,const char *key,size_t key_length,void *context)
{
	char	*inkey = (char *)context;

	//if ((inkey == NULL) || (strlen(inkey) == 0) || strstr((char *)key,inkey) != NULL)
	if (UnionStrComp((char *)key,inkey))
	{
		UnionDeleteMemcachedValue((char *)key);
		//UnionProgramerLog("in callback_dump_delte_keys:: delete key[%s]\n",key);
	}

	return MEMCACHED_SUCCESS;
}

/* 
   ���ܣ�	ɾ����ָ��ģʽ��ƥ������õ�keys
   ������	reqKey[in]		�������
   ����ֵ��>=0			�ɹ�������������
   <0			ʧ�ܣ����ش�����
 */
int  UnionDeleteMemcachedKeysByAlikeName(char *key)
{
	int	ret;
	int	master_num = 0;
	int	backup_num = 0;
	int	master_ret = -1;
	int	backup_ret = -1;
	char	tmpKey[1024];
	memcached_return	rc;
	memcached_st 		*memc_master = NULL;
	memcached_st		*memc_backup = NULL;
	memcached_dump_fn 	fnCall;

	memc_master = memcached_create(NULL);
	memc_backup = memcached_create(NULL);

	if ((ret = UnionConnectMemcachedForOwnSession(memc_master,memc_backup,&master_num,&backup_num)) < 0)
	{
		UnionUserErrLog("in UnionDeleteMemcachedKeysByAlikeName:: UnionConnectMemcachedForOwnSession ret = [%d]!\n",ret);
		UnionCloseMemcachedForOwnSession(memc_master,memc_backup);	
		return(ret);
	}

	fnCall = callback_dump_delte_keys;

	// ����ͨ���
	if ((key == NULL) || (strlen(key) == 0))
		snprintf(tmpKey,sizeof(tmpKey),"%s","*");
	else
		snprintf(tmpKey,sizeof(tmpKey),"*%s*",key);

	// �������������
	if (master_num)
	{
		memcached_stat(memc_master,NULL,&rc);
		if (rc != MEMCACHED_SUCCESS)
		{
			UnionUserErrLog("in UnionGetMemcachedKeysByAlikeName:: memcached_stat memc_master rc = [%d] msg = [%s]!\n",rc,memcached_strerror(memc_master,rc));
			master_ret = errCodeOffsetOfMemcached - abs(rc);
		}
		else
		{
			if ((rc = memcached_dump(memc_master,&fnCall,tmpKey,1)) != MEMCACHED_SUCCESS)
			{
				UnionUserErrLog("in UnionDeleteMemcachedKeysByAlikeName:: memcached_dump memc_master rc = [%d] msg = [%s]!\n",rc,memcached_strerror(memc_master,rc));
				master_ret = errCodeOffsetOfMemcached - abs(rc);
			}
			else
				master_ret = 0;
		}
	}

	// �������������
	if (backup_num)
	{
		memcached_stat(memc_backup,NULL,&rc);
		if (rc != MEMCACHED_SUCCESS)
		{
			UnionUserErrLog("in UnionGetMemcachedKeysByAlikeName:: memcached_stat memc_backup rc = [%d] msg = [%s]!\n",rc,memcached_strerror(memc_backup,rc));
			backup_ret = errCodeOffsetOfMemcached - abs(rc);
		}
		else
		{
			if ((rc = memcached_dump(memc_backup,&fnCall,tmpKey,1)) != MEMCACHED_SUCCESS)
			{
				UnionUserErrLog("in UnionDeleteMemcachedKeysByAlikeName:: memcached_dump memc_backup rc = [%d] msg = [%s]!\n",rc,memcached_strerror(memc_backup,rc));
				backup_ret = errCodeOffsetOfMemcached - abs(rc);
			}
			else
				backup_ret = 0;
		}
	}

	UnionCloseMemcachedForOwnSession(memc_master,memc_backup);

	if ((master_ret < 0) && (backup_ret < 0))
	{
		UnionUserErrLog("in UnionDeleteMemcachedKeysByAlikeName:: master_ret = [%d] backup_ret = [%d]!\n",master_ret,backup_ret);		
		return(master_ret);
	}
	else
		return(1);
}

/* 
   ���ܣ�	����IP��ַ�Ͷ˿�,����ָ��memcached��״̬
   ������	ipAddr[in]		IP��ַ
   port[in]		�˿ں�
   ����ֵ��>=0			�ɹ�
   <0			ʧ�ܣ����ش�����
 */
int UnionTestMemcachedStatus(char *ipAddr, int port)
{
	memcached_st			*master = NULL;
	memcached_return		rc = 0;
	uint32_t                	flags = 0;
	char				key[128+1];
	char				oriValue[128+1];
	int				lenOfValue = 0;
	size_t				lenValue = 0;
	int				len = 0;
	char				*result = NULL;
	time_t          		expiration = 0;
	char				curValue[128+1];	

	master = memcached_create(NULL);

	rc = memcached_server_add(master, ipAddr, port);
	if (MEMCACHED_SUCCESS != rc)
	{
		UnionUserErrLog("in UnionTestMemcachedStatus:: memcached_server_add rc = [%d] msg = [%s]!\n",rc,memcached_strerror(master,rc));
		memcached_free(master);
		return(errCodeOffsetOfMemcached - abs(rc));
	}

	UnionSetMemcachedBehavior(master);

	// �����ͼ�ֵ
	len = sprintf(key,"testMemcached:123456789");
	key[len] = 0;
	lenOfValue = sprintf(oriValue,"this is a test");
	oriValue[lenOfValue] = 0;

	// ����ֵ
	if ((rc = memcached_set(master, key, strlen(key), oriValue, lenOfValue, 0, flags)) != MEMCACHED_SUCCESS)
	{
		UnionUserErrLog("in UnionTestMemcachedStatus:: memcached_set rc = [%d] msg = [%s]!\n",rc,memcached_strerror(master,rc));
		memcached_free(master);
		return(errCodeOffsetOfMemcached - abs(rc));
	}

	// ��ȡֵ
	result = memcached_get(master, key, strlen(key),&lenValue, &flags, &rc);
	if (rc != MEMCACHED_SUCCESS)
	{
		UnionUserErrLog("in UnionTestMemcachedStatus:: memcached_get rc = [%d] msg = [%s]!\n",rc,memcached_strerror(master,rc));
		memcached_free(master);
		free(result);
		return(errCodeOffsetOfMemcached - abs(rc));
	}

	// �Ա�ֵ
	len = sprintf(curValue,result,lenValue);
	curValue[len] = 0;
	if (strcmp(oriValue,curValue) != 0)
	{
		UnionUserErrLog("in UnionTestMemcachedStatus:: key[%s] oriValue[%s] != curValue[%s]!\n",key,oriValue,curValue);
		memcached_free(master);
		free(result);
		return(errCodeParameter);
	}

	// ɾ��ֵ
	if ((rc = memcached_delete(master, key, strlen(key), expiration)) != MEMCACHED_SUCCESS)
	{
		UnionLog("in UnionTestMemcachedStatus:: memcached_delete master rc = [%d] msg = [%s]!\n",rc,memcached_strerror(master,rc));
		//return(errCodeOffsetOfMemcached - abs(rc));
	}

	free(result);
	memcached_free(master);
	return 1;
}


//add begin by lusj 20160104
//�ı�memcached ״̬������״̬��Ϊ�쳣
int UnionSetMemcachedStatus(memcached_st *shell,const char *group_key, size_t group_key_length,memcached_return	rc)
{
	int                                     x,ret;
	int                                     port;
	int					totalNum = -1;
	char                                    port_buf[5];
	char                                    IP[32];

	PUnionMemcachedSvr             	 	pmemcached=NULL;


	memset(IP,0,sizeof(IP));
	memset(port_buf,0,sizeof(port_buf));
	UnionGetMemcachedHost(shell,group_key, group_key_length,IP,port_buf);
	port = atoi(port_buf);


	if ((ret = UnionConnectMemcachedConf()) < 0)
	{
		UnionLog("in UnionSetMemcachedStatus ::UnionConnectMemcachedConf:: magent setup failed!\n");
		return(ret);
	}

	for (x=0;x<pgunionMemConf->realNum;x++)
	{
		pmemcached = pgunionMemcachedSvr + x;

		if((strcmp(IP,pmemcached->ipAddr)==0) && (port == pmemcached->port))
		{
			if(rc == MEMCACHED_SUCCESS)
			{
				if(pmemcached->conStats == 0)
				{
					//modify  by lusj 201151214 ������ݿ⣬�������������
					if ((totalNum = UnionSelectRealDBRecordCounts(defTableNameOfSymmetricKeyDB,NULL,NULL))>0)
					{
						if((ret = UnionFlushMemcached_Of_IP(pmemcached))<0)
						{
							UnionUserErrLog("in UnionSetMemcachedStatus :: UnionFlushMemcached_Of_IP::UnionFlushMemcachedNode!\n");
							return(ret);
						}
					}
				}
				pmemcached->conStats = 1;	
			}
			else
				pmemcached->conStats = 0;

			UnionLog("in UnionSetMemcachedStatus:: IP:port::[%s][%d] Stats = %d !\n",IP,port,pmemcached->conStats);
		}
	}

	return (0);
}


//��ȡmemcached״̬
int UnionGetMemcachedStatus(memcached_st *shell,const char *group_key, size_t group_key_length)
{
	int                                     x,ret;
	int					conStats = -1;
	int                                     port;
	char                                    port_buf[5];
	char                                    IP[32];

	PUnionMemcachedSvr             	 	pmemcached=NULL;


	memset(IP,0,sizeof(IP));
	memset(port_buf,0,sizeof(port_buf));
	UnionGetMemcachedHost(shell,group_key, group_key_length,IP,port_buf);
	port = atoi(port_buf);

	if ((ret = UnionConnectMemcachedConf()) < 0)
	{
		UnionLog("in UnionSetMemcachedStatus ::UnionConnectMemcachedConf:: magent setup failed!\n");
		return(ret);
	}

	for (x=0;x<pgunionMemConf->realNum;x++)
	{
		pmemcached = pgunionMemcachedSvr + x;

		if((strcmp(IP,pmemcached->ipAddr)==0) && (port == pmemcached->port))
			conStats = pmemcached->conStats;
	}

	if(conStats == 1)
		return (1);
	else
		return (0);
}

//add end by lusj  20160104

