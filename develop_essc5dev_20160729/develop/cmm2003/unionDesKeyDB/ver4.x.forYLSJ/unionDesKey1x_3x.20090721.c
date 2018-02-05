/*
	Author:
		HuangBaoxin
	Date:
		2009/07/22
	Desc:
		银联数据 1.0 密钥库加载到共享内存
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>

#include "unionDesKey1x_3x.h"
#include "UnionLog.h"
#include "unionDesKey.h"
#include "unionDesKeyDB.h"
#include "unionDesKeyDBLog.h"
#include "unionErrCode.h"

/*
// 定义密钥类型
typedef struct
{
	char				fullName[40+1];		// 密钥全名，采用owner.keyApp.keyName
	char				value[48+1];		// 密钥密文
	char				checkValue[16+1];	// 密钥校验值
	TUnionDesKeyLength		length;			// 密钥强度
	//TUnionDesKeyProtectMethod	protectMethod;		// 密钥保护方法
	TUnionDesKeyType		type;			// 密钥类型
	TUnionDate			activeDate;		// 密钥有效日期
	TUnionDate			passiveDate;		// 密钥失效日期
} TUnionDesKey_1x;
typedef TUnionDesKey_1x			*PUnionDesKey_1x;

typedef struct
{
	char				fullName[40+1];		// 密钥全名，采用owner.keyApp.keyName
	char				value[48+1];		// 密钥密文
	char				checkValue[16+1];	// 密钥校验值
	TUnionDesKeyLength		length;			// 密钥强度
	//TUnionDesKeyProtectMethod	protectMethod;		// 密钥保护方法
	TUnionDesKeyType		type;			// 密钥类型
	char				activeDate[8+1];	// 密钥有效日期
	long				maxEffectiveDays;	// 密钥最大有效日期
	long				maxUseTimes;		// 密钥最多使用的次数
	long				useTimes;		// 使用次数
	long				lastUpdateTime;		// 最近一次更新时间
	char				container[20+1];	// 

	short				oldVerEffective;	// 旧版本是否生效
	char				oldValue[48+1];		// 旧版本的密钥值
	char				oldCheckValue[16+1];	// 旧版本的密钥校验值
	int				windowBetweenKeyVer;	// 两个密钥版本间的窗口期
#ifdef _UnionDesKey_4_x_
	short				isWritingLocked;	// 是否被写锁住了
#endif
} TUnionDesKey;
*/
// 1.0 密钥结构体转换为 3.0
int UnionDesKey1xToDesKey3x(PUnionDesKey_1x pdesKey1x, PUnionDesKey_1x pdesKey1xOld, PUnionDesKey pdesKey3x)
{
	int	ret;
	long	days;
	char	*p, tmpFullName[40+1];
	memset(tmpFullName, 0, sizeof(tmpFullName));
	
	if ((pdesKey1x == NULL) || (pdesKey3x == NULL))
	{
		UnionUserErrLog("in UnionDesKey1xToDesKey3x::pdesKey1x[%0x],pdesKey3x[%0x]\n", pdesKey1x, pdesKey3x);
		return(errCodeParameter);	
	}
	
	strcpy(pdesKey3x->fullName, pdesKey1x->fullName);
	strcpy(pdesKey3x->value, pdesKey1x->value);
	strcpy(pdesKey3x->checkValue, pdesKey1x->checkValue);
	pdesKey3x->length = pdesKey1x->length;
	pdesKey3x->type = pdesKey1x->type;
	strcpy(pdesKey3x->activeDate, pdesKey1x->activeDate);
	pdesKey3x->maxEffectiveDays = GetDaysBetween2Date(pdesKey1x->activeDate, pdesKey1x->passiveDate);
	
	//UnionUserErrLog("in UnionDesKey1xToDesKey3x::activeDate[%s], passiveDate[%s], maxEffectiveDays[%d]\n",
	//	pdesKey1x->activeDate, pdesKey1x->passiveDate, pdesKey3x->maxEffectiveDays);
	
	pdesKey3x->maxUseTimes = -1;
	pdesKey3x->useTimes = 0;
	time(&(pdesKey3x->lastUpdateTime));
	
	if ((ret = UnionGetOwnerNameOutOfDesKeyFullName(pdesKey3x->fullName, pdesKey3x->container)) <0)
	{
		UnionUserErrLog("in UnionDesKey1xToDesKey3x::UnionGetOwnerNameOutOfDesKeyFullName\n");
		return(ret);	
	}
	
	// 默认支持旧版本
	pdesKey3x->oldVerEffective = 1;
	
	if (pdesKey1xOld != NULL)
	{
		//pdesKey3x->oldVerEffective = 1;
		strcpy(pdesKey3x->oldValue, pdesKey1xOld->value);
		strcpy(pdesKey3x->oldCheckValue, pdesKey1xOld->checkValue);
		pdesKey3x->windowBetweenKeyVer = -1;
	}
	else
	{
		strcpy(pdesKey3x->oldValue, "");
		strcpy(pdesKey3x->oldCheckValue, "");
		pdesKey3x->windowBetweenKeyVer = -1;
	}
	pdesKey3x->isWritingLocked = 0;
	
	return 0;	
}

// 3.0 密钥结构体转换为1.0
int UnionDesKey3xToDesKey1x(PUnionDesKey pdesKey3x, PUnionDesKey_1x pdesKey1x, PUnionDesKey_1x pdesKey1xOld)
{
	int	ret;
	char	appID[40+1], ownerName[40+1], keyName[40+1];
	char	tmpFullName[40+1];
	
	if ((pdesKey1x == NULL) || (pdesKey3x == NULL))
	{
		UnionUserErrLog("in UnionDesKey3xToDesKey1x::pdesKey1x[%0x],pdesKey3x[%0x]\n", pdesKey1x, pdesKey3x);
		return(errCodeParameter);	
	}
	
	strcpy(pdesKey1x->fullName, pdesKey3x->fullName);
	strcpy(pdesKey1x->value, pdesKey3x->value);
	strcpy(pdesKey1x->checkValue, pdesKey3x->checkValue);
	pdesKey1x->length = pdesKey3x->length;
	pdesKey1x->type = pdesKey3x->type;
	strcpy(pdesKey1x->activeDate, pdesKey3x->activeDate);
	AddDateByDatesYMD(pdesKey3x->maxEffectiveDays, pdesKey3x->activeDate, pdesKey1x->passiveDate);
	
	if (pdesKey3x->oldVerEffective)
	{	
		if (pdesKey1xOld == NULL)
		{
			UnionUserErrLog("in UnionDesKey3xToDesKey1x::pdesKey1xOld[%0x]\n", pdesKey1xOld);
			return(errCodeParameter);		
		}
	
		sprintf(pdesKey1xOld->fullName, "%sOLD", pdesKey3x->fullName);
		strcpy(pdesKey1xOld->value, pdesKey3x->oldValue);
		strcpy(pdesKey1xOld->checkValue, pdesKey3x->oldCheckValue);
		pdesKey1xOld->length = pdesKey3x->length;
		pdesKey1xOld->type = pdesKey3x->type;		
		strcpy(pdesKey1xOld->activeDate, pdesKey3x->activeDate);
		strcpy(pdesKey1xOld->passiveDate, pdesKey1x->passiveDate);
	}	
	
	return 0;
}

// 从密钥文件读取一个密钥对象
int UnionReadDesKey1x(char *keyFileName, PUnionDesKey_1x pDesKey1x)
{
	int	ret;
	FILE	*fp;
	char	tmpBuf[512];
	
	memset(tmpBuf, 0, sizeof(tmpBuf));
	sprintf(tmpBuf, "%s/etc/unionKeyDB/%s", getenv("HOME"), keyFileName);
	
	if ((fp = fopen(tmpBuf,"rb")) == NULL)
	{
		UnionSystemErrLog("in UnionReadDesKey:: fopen [%s]!\n",tmpBuf);
		return(-1);
	}
	
	if (fread(pDesKey1x, sizeof(*pDesKey1x), 1, fp) != 1)
	{
		UnionSystemErrLog("in UnionReadDesKey:: fread [%s]!\n",tmpBuf);
		fclose(fp);
		return(-1);
	}
	
	fclose(fp);
	
	memset(tmpBuf, 0, sizeof(tmpBuf));
	strcpy(tmpBuf, keyFileName);
	tmpBuf[strlen(tmpBuf) - strlen(".deskey")] = 0;
	
	if (strcmp(tmpBuf,pDesKey1x->fullName) != 0)
	{
		UnionUserErrLog("in UnionReadDesKey1x:: registered full name [%s] != that of expected [%s]\n",
				pDesKey1x->fullName,tmpBuf);
		return(-1);
	}
	
	return(0);
}

// 保存1.0密钥到密钥文件
int UnionSaveDesKey1x(PUnionDesKey_1x pDesKey1x)
{
	int	ret;
	FILE	*fp;
	char	tmpBuf[512];

	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf, "%s/etc/unionKeyDB/%s.deskey", getenv("HOME"), pDesKey1x->fullName);
	
	if ((fp = fopen(tmpBuf,"wb")) == NULL)
	{
		UnionSystemErrLog("in UnionSaveDesKey1x:: fopen [%s]!\n",tmpBuf);
		return(-1);
	}
		
	if (fwrite(pDesKey1x, sizeof(*pDesKey1x), 1, fp) != 1)
	{
		UnionSystemErrLog("in UnionSaveDesKey1x:: fwrite [%s]!\n",tmpBuf);
		fclose(fp);
		return(-1);
	}
	
	fclose(fp);
	
	return(0);
}

// 删除1.0密钥的密钥文件
int UnionDeleteDesKey1x(PUnionDesKey_1x pDesKey1x)
{
	int	ret;
	char	tmpBuf[512], cmd[512];

	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf, "%s/etc/unionKeyDB/%s.deskey", getenv("HOME"), pDesKey1x->fullName);
	
	memset(cmd,0,sizeof(cmd));
	sprintf(cmd,"rm %s",tmpBuf);
	
	if ((ret = system(cmd)) < 0)
	{
		UnionSystemErrLog("in UnionDeleteDesKey1x:: system [%s]!\n",cmd);
		return(ret);
	}
	
	return(0);
}

// 保存3.x密钥到密钥文件
int UnionSaveDesKey3x(PUnionDesKey pdesKey3x)
{
	TUnionDesKey_1x		desKey1x, desKey1xOld;
	int			ret;
	
	memset(&desKey1x, 0, sizeof(desKey1x));
	memset(&desKey1xOld, 0, sizeof(desKey1xOld));
	
	ret =  UnionDesKey3xToDesKey1x(pdesKey3x, &desKey1x, &desKey1xOld);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionSaveDesKey3x::UnionDesKey3xToDesKey1x");
		return ret;
	}
	
	if ((ret = UnionSaveDesKey1x(&desKey1x)) < 0)
	{
		UnionUserErrLog("in UnionSaveDesKey3x::UnionSaveDesKey1x(desKey1x)");
		return ret;		
	}
	
	if ((pdesKey3x->oldVerEffective) && (strlen(pdesKey3x->oldValue) > 0))
	{
		if ((ret = UnionSaveDesKey1x(&desKey1xOld)) < 0)
		{
			UnionUserErrLog("in UnionSaveDesKey3x::UnionSaveDesKey1x(desKey1xOld)");
			return ret;		
		}
	}
	
	return 0;
}

// 保存3.x密钥到密钥文件
// 选择保存密钥或者old密钥
// valueType=0, 密钥
// valueType=1  old密钥
int UnionSaveDesKey3xByValueType(PUnionDesKey pdesKey3x, int valueType)
{
	TUnionDesKey_1x		desKey1x, desKey1xOld;
	int			ret;
	
	memset(&desKey1x, 0, sizeof(desKey1x));
	memset(&desKey1xOld, 0, sizeof(desKey1xOld));
	
	ret =  UnionDesKey3xToDesKey1x(pdesKey3x, &desKey1x, &desKey1xOld);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionSaveDesKey3x::UnionDesKey3xToDesKey1x");
		return ret;
	}
	
	if (valueType == 0)
	{
		if ((ret = UnionSaveDesKey1x(&desKey1x)) < 0)
		{
			UnionUserErrLog("in UnionSaveDesKey3x::UnionSaveDesKey1x(desKey1x)");
			return ret;		
		}
	}
	else
	{
		if ((ret = UnionSaveDesKey1x(&desKey1xOld)) < 0)
		{
			UnionUserErrLog("in UnionSaveDesKey3x::UnionSaveDesKey1x(desKey1xOld)");
			return ret;		
		}		
	}
	
	return 0;
}

// 删除3.x密钥的密钥文件
int UnionDeleteDesKey3x(PUnionDesKey pdesKey3x)
{
	TUnionDesKey_1x		desKey1x, desKey1xOld;
	int			ret;
	
	memset(&desKey1x, 0, sizeof(desKey1x));
	memset(&desKey1xOld, 0, sizeof(desKey1xOld));
	
	ret =  UnionDesKey3xToDesKey1x(pdesKey3x, &desKey1x, &desKey1xOld);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionDeleteDesKey3x::UnionDesKey3xToDesKey1x");
		return ret;
	}
	
	if ((ret = UnionDeleteDesKey1x(&desKey1x)) < 0)
	{
		UnionUserErrLog("in UnionDeleteDesKey3x::UnionDeleteDesKey1x(desKey1x)");
		return ret;		
	}
	
	if ((pdesKey3x->oldVerEffective) && (strlen(pdesKey3x->oldValue) > 0))
	{
		UnionDeleteDesKey1x(&desKey1xOld);
	}
	
	return 0;
}

/*
	从银联数据 1.0 的密钥库加载密钥到共享内存
	不包括OLD密钥
*/
int UnionLoadDesKey1xIntoMemory(PUnionDesKeyDB pdesKeyDB)
{
	int	index, len, ret, check, pos;
	TUnionDesKey_1x		desKey1x;
	TUnionDesKey		desKey3x;
	
	DIR*	dirp;
	struct dirent* dp;
	char	dir[512+1], filename[512+100+1], tmpbuf1[100], tmpbuf2[100];
	struct stat	statbuf;
		
	if (pdesKeyDB == NULL)
	{
		UnionUserErrLog("in UnionLoadDesKey1xIntoMemory::pdesKeyDB[%0x]\n", pdesKeyDB);
		return(errCodeParameter);	
	}
	
	index = 0;
	memset(dir, 0, sizeof(dir));
	memset(filename, 0, sizeof(filename));
	sprintf(dir, "%s/etc/unionKeyDB", getenv("HOME"));
	
	dirp = opendir(dir);
	if (dirp != NULL)
	{
		pdesKeyDB->num = 0;
		
		while ((dp = readdir(dirp)) != NULL)
		{
			strcpy (filename, dir);
			strcat (filename, "/");
			strcat (filename, dp->d_name);
			
			if ((stat(filename, &statbuf) == 0) && (strlen(dp->d_name) > strlen(".deskey")))
			{
				memset(tmpbuf1, 0, sizeof(tmpbuf1));
				strcpy(tmpbuf1, dp->d_name+strlen(dp->d_name)-strlen(".deskey"));				
							
				if (S_ISREG(statbuf.st_mode) && (strcmp(tmpbuf1, ".deskey") == 0))  
				{
					memset(tmpbuf2, 0, sizeof(tmpbuf2));
					strcpy(tmpbuf2, dp->d_name);
					len = strlen(dp->d_name)-strlen(".deskey");					
					tmpbuf2[len] = 0;
					memset(tmpbuf1, 0, sizeof(tmpbuf1));
					strcpy(tmpbuf1, tmpbuf2+strlen(tmpbuf2)-strlen("OLD"));
					if (strcmp(tmpbuf1, "OLD") == 0)
					{
						UnionProgramerLog("in UnionLoadDesKey1xIntoMemory::deskey 111 [%s]\n", dp->d_name);
						continue;
					}
				
					UnionProgramerLog("in UnionLoadDesKey1xIntoMemory::deskey 222 [%s]\n", dp->d_name);
					memset(&desKey1x, 0, sizeof(desKey1x));
					ret = UnionReadDesKey1x(dp->d_name, &desKey1x);
					if (ret < 0)
					{
						UnionUserErrLog("in UnionLoadDesKey1xIntoMemory::UnionReadDesKey1x[%s]\n", dp->d_name);
						closedir(dirp);
						return(ret);
					}
					memset(&desKey3x, 0, sizeof(desKey3x));
					ret = UnionDesKey1xToDesKey3x(&desKey1x, NULL, &desKey3x);
					if (ret < 0)
					{
						UnionUserErrLog("in UnionLoadDesKey1xIntoMemory::UnionDesKey1xToDesKey3x\n");
						closedir(dirp);
						return(ret);
					}
					
					if (pdesKeyDB->num >= pdesKeyDB->maxKeyNum)
					{
						UnionUserErrLog("in UnionLoadDesKey1xIntoMemory::pdesKeyDB->num[%d] "
							"> pdesKeyDB->maxKeyNum\n", pdesKeyDB->num, pdesKeyDB->maxKeyNum);
						closedir(dirp);
						return -1;
					}
					
					// 从小到大排序插入
					for (index = 0; index < pdesKeyDB->num; index++)
					{
						if ((check = strcmp(desKey3x.fullName,(pdesKeyDB->pkey + index)->fullName)) == 0)
						{
							break;
							//UnionUserErrLog("in UnionLoadDesKey1xIntoMemory:: [%s] already exists!\n", desKey3x.fullName);
							//return(errCodeDesKeyDBMDL_KeyAlreadyExists);
						}
						
						if (check > 0)
							continue;
						else
							break;
					}
					pos = index;	// 新记录将插在pos指示的位置
					for (index = pdesKeyDB->num - 1; index >= pos; index--)
					{
						memcpy(pdesKeyDB->pkey+index+1, pdesKeyDB->pkey+index, sizeof(TUnionDesKey));
					}
					memcpy(pdesKeyDB->pkey + pos, &desKey3x, sizeof(TUnionDesKey));

					pdesKeyDB->num ++;
				}
			}		
		}
		closedir(dirp);	
	}
	
	return(pdesKeyDB->num);
}

/*
	从银联数据 1.0 的密钥库把 OLD密钥 加载密钥到共享内存
*/
int UnionLoadOldDesKey1xIntoMemory(PUnionDesKeyDB pdesKeyDB)
{
	int	index, len, ret, pos;
	TUnionDesKey_1x		desKey1x;
	TUnionDesKey		desKey3x;
	char	fullName[40+1];
	
	DIR*	dirp;
	struct dirent* dp;
	char	dir[512+1], filename[512+100+1], tmpbuf1[100], tmpbuf2[100];
	struct stat	statbuf;
		
	if (pdesKeyDB == NULL)
	{
		UnionUserErrLog("in UnionLoadOldDesKey1xIntoMemory::pdesKeyDB[%0x]\n", pdesKeyDB);
		return(errCodeParameter);	
	}
	
	index = 0;
	memset(dir, 0, sizeof(dir));
	memset(filename, 0, sizeof(filename));
	sprintf(dir, "%s/etc/unionKeyDB", getenv("HOME"));
	
	dirp = opendir(dir);
	if (dirp != NULL)  
	{
		while ((dp = readdir(dirp)) != NULL)
		{
			strcpy (filename, dir);
			strcat (filename, "/");
			strcat (filename, dp->d_name);
			
			if ((stat(filename, &statbuf) == 0) && (strlen(dp->d_name) > strlen(".deskey")))
			{
				memset(tmpbuf1, 0, sizeof(tmpbuf1));
				strcpy(tmpbuf1, dp->d_name+strlen(dp->d_name)-strlen(".deskey"));				
							
				if (S_ISREG(statbuf.st_mode) && (strcmp(tmpbuf1, ".deskey") == 0))  
				{
					memset(tmpbuf2, 0, sizeof(tmpbuf2));
					strcpy(tmpbuf2, dp->d_name);
					len = strlen(dp->d_name)-strlen(".deskey");					
					tmpbuf2[len] = 0;
					memset(tmpbuf1, 0, sizeof(tmpbuf1));
					strcpy(tmpbuf1, tmpbuf2+strlen(tmpbuf2)-strlen("OLD"));
					
					if (strcmp(tmpbuf1, "OLD") != 0)
					{
						//UnionUserErrLog("in UnionLoadOldDesKey1xIntoMemory::deskey 111 [%s]\n", dp->d_name);
						continue;
					}
				
					//UnionUserErrLog("in UnionLoadOldDesKey1xIntoMemory::deskey 222 [%s]\n", dp->d_name);
					
					memset(&desKey1x, 0, sizeof(desKey1x));
					ret = UnionReadDesKey1x(dp->d_name, &desKey1x);
					if (ret < 0)
					{
						UnionUserErrLog("in UnionLoadOldDesKey1xIntoMemory::UnionReadDesKey1x[%s]\n", dp->d_name);
						closedir(dirp);
						return(ret);
					}
					
					memset(fullName, 0, sizeof(fullName));
					strcpy(fullName, dp->d_name);
					len = strlen(dp->d_name) - strlen("OLD.deskey");				
					fullName[len] = 0;
					
					if ((pos = UnionFindDesKeyPosInKeyDB2(fullName, pdesKeyDB)) < 0)
					{
						UnionUserErrLog("in UnionLoadOldDesKey1xIntoMemory::UnionFindDesKeyPosInKeyDB2[%s]\n", fullName);
					}
					else
					{
						//UnionUserErrLog("in UnionLoadOldDesKey1xIntoMemory::pos[%d], fullname[%s]\n", pos, (pdesKeyDB->pkey + pos)->fullName);
						(pdesKeyDB->pkey + pos)->oldVerEffective = 1;
						strcpy((pdesKeyDB->pkey + pos)->oldValue, desKey1x.value);
						strcpy((pdesKeyDB->pkey + pos)->oldCheckValue, desKey1x.checkValue);
						(pdesKeyDB->pkey + pos)->windowBetweenKeyVer = -1;
					}
				}
			}		
		}
		closedir(dirp);	
	}
	
	return(0);
}

/*
	查找密钥所在的位置
*/
int UnionFindDesKeyPosInKeyDB2(char *fullName, PUnionDesKeyDB pdesKeyDB)
{
	int	ret;
	long    index=0;
	//int	pos;
	long	left;
	long	right;
	int	check;
	
	left = 0;
	right = pdesKeyDB->num - 1;
	index = (left+right)/2;
	while (1) 
	{
		if ((left > right) || (left >= pdesKeyDB->num) || (right < 0))
		{
			ret = errCodeDesKeyDBMDL_KeyNonExists;
			goto exitRightNow;
		}
		if ((strcmp(fullName,(pdesKeyDB->pkey+left)->fullName) < 0) || 
			(strcmp(fullName,(pdesKeyDB->pkey+right)->fullName) > 0))
		{
			ret = errCodeDesKeyDBMDL_KeyNonExists;
			//UnionNullLog("fullName = [%s] left = [%s] right = [%s] len = [%d]\n",fullName,(pdesKeyDB->pkey+left)->fullName,(pdesKeyDB->pkey+right)->fullName,40);
			goto exitRightNow;
		}
		index = (left + right) / 2;
		//UnionNullLog("index = [%d] fullName = [%s] index = [%s] len = [%d]\n",index,fullName,(pdesKeyDB->pkey+index)->fullName,40);	
		if ((check = strcmp(fullName,(pdesKeyDB->pkey+index)->fullName)) == 0)
			break;
		//UnionNullLog("check = [%d]\n",check);
		if (check > 0)
		{
			left = index+1;
			continue;
		}
		if (check < 0)
		{
			right = index-1;
			continue;
		}
	}
		
	return(index);
	
exitRightNow:
	//UnionNullLog("in UnionFindDesKeyPosInKeyDB:: index = [%ld] left = [%ld] right = [%ld]\n",index,left,right);
	return(ret);
}

/* 求日期tt与日期ss相差的天数  格式为YYYYMMDD */
int GetDaysBetween2Date(char *ss, char *tt)
{
	int	tmp,month1,day1,year1,month2,day2,year2,year;
	long	days1,days2,days;
	char	today[10],str[10],date[10];
	int	ret;
	
	memset(today,0,sizeof(today));
	strcpy(today, ss);
	
	memset(date, 0, sizeof(date));
	strcpy(date, tt);
	
	memset(str,0,sizeof(str));
	memcpy(str,date+4,2);
	month1 = atoi(str);
	memset(str,0,sizeof(str));
	memcpy(str,date+6,2);
	day1 = atoi(str);
	memset(str,0,sizeof(str));
	memcpy(str,date,4);
	year1 = atoi(str);
	switch (month1)
	{
		case	1:
			days1 = day1;
			break;
		case	2:
			days1 = 31 + day1;
			break;
		case	3:
			days1 = 31 + day1;
			break;
		case	4:
			days1 = 31 * 2 + day1;
			break;
		case	5:
			days1 = 31 * 2 + 30 + day1;
			break;
		case	6:
			days1 = 31 * 3 + 30 + day1;
			break;
		case	7:
			days1 = 31 * 3 + 30 * 2 + day1;
			break;
		case	8:
			days1 = 31 * 4 + 30 * 2 + day1;
			break;
		case	9:
			days1 = 31 * 5 + 30 * 2 + day1;
			break;
		case	10:
			days1 = 31 * 5 + 30 * 3 + day1;
			break;
		case	11:
			days1 = 31 * 6 + 30 * 3 + day1;
			break;
		case	12:
			days1 = 31 * 6 + 30 * 4 + day1;
			break;
		default:
			return(errCodeParameter);
	}
	if (month1 > 2)
	{
		if ((year1 % 4 == 0) && (year1 % 100 != 0))
			days1 += 29;
		else
			days1 += 28;
	}
		

	memset(str,0,sizeof(str));
	memcpy(str,today+4,2);
	month2 = atoi(str);
	memset(str,0,sizeof(str));
	memcpy(str,today+6,2);
	day2 = atoi(str);
	memset(str,0,sizeof(str));
	memcpy(str,today,4);
	year2 = atoi(str);
	switch (month2)
	{
		case	1:
			days2 = day2;
			break;
		case	2:
			days2 = 31 + day2;
			break;
		case	3:
			days2 = 31 + day2;
			break;
		case	4:
			days2 = 31 * 2 + day2;
			break;
		case	5:
			days2 = 31 * 2 + 30 + day2;
			break;
		case	6:
			days2 = 31 * 3 + 30 + day2;
			break;
		case	7:
			days2 = 31 * 3 + 30 * 2 + day2;
			break;
		case	8:
			days2 = 31 * 4 + 30 * 2 + day2;
			break;
		case	9:
			days2 = 31 * 5 + 30 * 2 + day2;
			break;
		case	10:
			days2 = 31 * 5 + 30 * 3 + day2;
			break;
		case	11:
			days2 = 31 * 6 + 30 * 3 + day2;
			break;
		case	12:
			days2 = 31 * 6 + 30 * 4 + day2;
			break;
		default:
			return(errCodeParameter);
	}
	if (month2 > 2)
	{
		if ((year2 % 4 == 0) && (year2 % 100 != 0))
			days2 += 29;
		else
			days2 += 28;
	}
	//printf("date = [%s] today = [%s]\n",date,today);
	//printf("days1 = [%ld] days2 = [%ld]\n",days1,days2);
	days = days1 - days2;
	for (year = year2; year < year1; year++)
	{
		if ((year % 4 == 0) && (year % 100 != 0))
			days += 366;
		else
			days += 365;
	}
	return(days);
}

/* 求某个日期ss加上i天后的日期, i是负数是表示ss之前的日期  格式为YYYYMMDD */
int AddDateByDatesYMD(int i, char *ss, char *tt)
{
/*
   char tmp1[20];
   time_t timer;
   struct tm stm,newtimer;
   
   if ( i==0 ) {
     strncpy(tt,ss,8);  tt[8]=0;
     return (0);
   }

   strncpy( tmp1,ss,4 ); tmp1[4]=0;
   stm.tm_year=atoi(tmp1)-1900;
   strncpy( tmp1,ss+4,2 );  tmp1[2]=0;
   stm.tm_mon=atoi(tmp1)-1 ;
   strncpy( tmp1,ss+6,2 );  tmp1[2]=0;
   stm.tm_mday=atoi(tmp1)+i ;
   stm.tm_hour=0;    stm.tm_min =0;    stm.tm_sec =1;
   timer = mktime( &stm );

   newtimer = *localtime(&timer);
   sprintf( tt,"%04u%02u%02u",newtimer.tm_year+1900,newtimer.tm_mon+1,newtimer.tm_mday);
   return(0);
*/
	return(UnionDecideDateAfterSpecDate(ss, i ,tt));
}


// 初始化密钥库共享内存
int	gunionInitDesKeyDB1xFlag = 0;

int UnionInitDesKeyDB1x()
{
	int	ret, maxKeyNum;
	char	*p;
	
	if (gunionInitDesKeyDB1xFlag)
		return 0;

	if ((ret = UnionConnectDesKeyDB()) >= 0)
	{
		gunionInitDesKeyDB1xFlag = 1;
		return 0;
	}
	
	if ((p = getenv("UNIONMAXKEYNUM")) == NULL)
	{
		UnionUserErrLog("in UnionInitDesKeyDB1x::getenv(UNIONMAXKEYNUM) return null\n");
		return -1;
	}
	
	maxKeyNum = atoi(p);
	if (maxKeyNum <= 0)
	{
		UnionUserErrLog("in UnionInitDesKeyDB1x::getenv(UNIONMAXKEYNUM) return [%s]\n", p);
		return -1;
	}
	
	if ((ret = UnionCreateDesKeyDB(maxKeyNum)) < 0)
	{
		UnionUserErrLog("UnionInitDesKeyDB1x ::UnionCreateDesKeyDB! ret = [%d]\n",ret);
		return ret;
	}

	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("UnionInitDesKeyDB1x ::UnionConnectDesKeyDB! ret = [%d]\n",ret);
		return ret;
	}
		
	gunionInitDesKeyDB1xFlag = 1;
	return 0;
}

