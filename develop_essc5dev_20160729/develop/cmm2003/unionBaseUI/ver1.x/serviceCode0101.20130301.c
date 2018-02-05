//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionREC.h"
#include "UnionMD5.h"
#include "UnionLog.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "baseUIService.h"
#include "UnionStr.h"
#include "sm2.h"
#include "otpInterface.h"


/***************************************
服务代码:	0101
服务名:		用户登录
功能描述:	用户登录
***************************************/
int UnionDealServiceCode0101(PUnionHsmGroupRec phsmGroupRec)
{
	
	 
	int	ret;
	int	i = 0;
	int	len = 0;
	int	totalNum = 0;
	int	loginTimes;
	int	maxFreeTimes;	// 最大空闲时间
	int	maxWrongPasswordTimes = 5;
	int	wrongPasswordTimes = 0;
	long	freeTimes;	// 已空闲的时间
	char	userID[64];
	char	sysID[32];
	char	userName[128];
	char	userRoleList[512];
	char	organization[128];
	char	transTime[32];
	char	createTime[32];
	char	lastLoginTime[32];
	char	tmpBuf[2048];
	char	sql[512];
	char	reqPassword[128+1];
	char	userPassword[128+1];
	char	localPassword[128+1];
	char	identifyCode[128+1];
	char	clientIPAddr[64];
	char	userStatus[32];
	char	loginFlag[32];
	char	lastLoginLocation[64];
	char	lastOperationTime[32];
	char	systemTime[32];
	char	operTermList[2048];
	char	loginSysID[32];
	char	remark[128];
	int	maxOperTerm = 100;
	char	operTermGrp[maxOperTerm][128];
	int	operTermNum = 0;
	char	ipAddr[64];
	char	passwdUpdateTime[32];
	char	userPK[512];
	char	organizationID[64];
	char	*ptr = NULL;
	int userPK_len;
	char x_coordinate[1024];
	char y_coordinate[1024];
	char r_coordinate[1024];
	char s_coordinate[1024];
	char x_coordinate_asc[1024];
	char y_coordinate_asc[1024];
	char r_coordinate_asc[1024];
	char s_coordinate_asc[1024];
	int x_coordinate_len;
	int y_coordinate_len;
	int r_coordinate_len;
	int s_coordinate_len;
	int reqPassword_len;
	int localPassword_len;
	char	loginMode[32];
	int	ukeyVerifyMode = 2;
	int	isVerifyPasswd = 1; // 使用usbK签名时，无需对比密码
	char	otpGrp[3][128];
	int     count;
	char 	QCode[20+1];
	int	offset;
	char	tmp[16+1];
	char	abOfMyself[40+1];
	char	clientVersion[40+1];

	// 读取用户ID
	memset(userID,0,sizeof(userID));
	if ((ret = UnionReadRequestXMLPackageValue("head/userID",userID,sizeof(userID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadRequestXMLPackageValue[%s]!\n","head/userID");
		return(ret);
	}

	// 读取系统ID
	memset(sysID,0,sizeof(sysID));
	if ((ret = UnionReadRequestXMLPackageValue("head/sysID",sysID,sizeof(sysID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadRequestXMLPackageValue[%s]!\n","head/sysID");
		return(ret);
	}

	// 读取时间
	memset(transTime,0,sizeof(transTime));
	if ((ret = UnionReadRequestXMLPackageValue("head/transTime",transTime,sizeof(transTime))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadRequestXMLPackageValue[%s]!\n","head/transTime");
		return(ret);
	}

	// 读取客户端IP
	memset(clientIPAddr,0,sizeof(clientIPAddr));
	if ((ret = UnionReadRequestXMLPackageValue("head/clientIPAddr",clientIPAddr,sizeof(clientIPAddr))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadRequestXMLPackageValue[%s]!\n","head/clientIPAddr");
		return(ret);
	}

	// 读取请求密码
	memset(reqPassword,0,sizeof(reqPassword));
	if ((ret = UnionReadRequestXMLPackageValue("body/password",reqPassword,sizeof(reqPassword))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadRequestXMLPackageValue[%s]!\n","body/password");
		return(ret);
	}

	// 读取UK验证版本
	if ((ret = UnionReadRequestXMLPackageValue("body/ukeyVerifyMode",tmpBuf,sizeof(tmpBuf))) > 0)
	{
		tmpBuf[ret] = 0;
		ukeyVerifyMode = atoi(tmpBuf);
	}
	
	//add by yangw 20160427 begin
	//增加中原银行客户端版本判断，如果客户端没有送版本号，则禁止客户端登陆
	// 读取客户英文简写
	memset(abOfMyself,0,sizeof(abOfMyself));
	if ((ptr = UnionReadStringTypeRECVar("abOfMyself")) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadStringTypeRECVar[%s]!\n","abOfMyself");
		return(errCodeRECMDL_VarNotExists);
	}
	
	strcpy(abOfMyself,ptr);
	if(strcasecmp(abOfMyself,"zyyh")==0)
	{
		// 读取客户端版本
		memset(clientVersion,0,sizeof(clientVersion));
		if ((ret = UnionReadRequestXMLPackageValue("body/clientVersion",clientVersion,sizeof(clientVersion))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0101:: clientVersion !\n");	
			UnionSetResponseRemark("客户端版本过低，请联系管理员");
			return(ret);	
		}
		
	}
	//add by yangw 20160427 begin

	// 读取用户表
	//sprintf(sql,"select * from sysUser where userID = '%s'",userID);
	snprintf(sql,sizeof(sql),"select * from sysUser left join organization on sysUser.organization = organization.organizationName where userID = '%s'",userID);
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}

	// 读取总数量
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("totalNum", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","totalNum");
		return(ret);
	}
	totalNum = atoi(tmpBuf);
	if (totalNum == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: userID[%s] not found!\n",userID);
		return(errCodeOperatorMDL_OperatorNotExists);
	}
	
	UnionLocateXMLPackage("detail", 1);
	
	// 读取用户状态
	memset(userStatus,0,sizeof(userStatus));
	if ((ret = UnionReadXMLPackageValue("userStatus", userStatus, sizeof(userStatus))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","userStatus");
		return(ret);
	}
	
	if (!atoi(userStatus))
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: userStatus[%s]!\n",userStatus);
		return(errCodeOperatorMDL_OperatorIsLocked);
	}
	
	// 读取登录方式
	memset(loginMode,0,sizeof(loginMode));
	if ((ret = UnionReadXMLPackageValue("loginMode", loginMode, sizeof(loginMode))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0001:: UnionReadXMLPackageValue[%s]!\n","loginMode");
		return(ret);
	}

        if(loginMode[0] != '1')//动态口令登录
        {
		// 读取认证码
		memset(identifyCode,0,sizeof(identifyCode));
		if ((ret = UnionReadXMLPackageValue("identifyCode", identifyCode, sizeof(identifyCode))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","identifyCode");
			return(ret);
		}
		
		// 检查认证码
		if (strlen(identifyCode) == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0101:: 未获取认证码!\n");
			return(errCodeOperatorMDL_IdentifyCodeNotExists);
		}
        }
	
	// 读取登录状态
	memset(loginFlag,0,sizeof(loginFlag));
	if ((ret = UnionReadXMLPackageValue("loginFlag", loginFlag, sizeof(loginFlag))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","loginFlag");
		return(ret);
	}
	
	// 读取上次登录地点
	memset(lastLoginLocation,0,sizeof(lastLoginLocation));
	if ((ret = UnionReadXMLPackageValue("lastLoginLocation", lastLoginLocation, sizeof(lastLoginLocation))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","lastLoginLocation");
		return(ret);
	}
	
	memset(lastOperationTime,0,sizeof(lastOperationTime));
	if (atoi(loginFlag))
	{
		// 读取登录系统编号
		memset(loginSysID,0,sizeof(loginSysID));
		if ((ret = UnionReadXMLPackageValue("loginSysID", loginSysID, sizeof(loginSysID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","loginSysID");
			return(ret);
		}
		if (strcmp(sysID,loginSysID) != 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0101:: sysID[%s] != loginSysID[%s]!\n",sysID,loginSysID);
			return(errCodeOperatorMDL_AlreadyLogon);
		}

		if (strcmp(clientIPAddr,lastLoginLocation) != 0)
		{
			if ( (maxFreeTimes = UnionReadIntTypeRECVar("freeTimesOfUser"))<=0)
         			maxFreeTimes = 1800;  // 默认1800秒
         		
         		// 读取最近操作时间
			if ((ret = UnionReadXMLPackageValue("lastOperationTime", lastOperationTime, sizeof(lastOperationTime))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","lastOperationTime");
				return(ret);
			}
			UnionCalcuSecondsPassedAfterSpecTime(lastOperationTime, &freeTimes);
			if ((freeTimes - maxFreeTimes) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0101:: 用户[%s]已经登录，禁止登录!\n",userID);
				return(errCodeOperatorMDL_AlreadyLogon);
			}
		}
	}

	// 密码错误次数
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("wrongPasswordTimes", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","tmpBuf");
		return(ret);
	}
	
	if ((wrongPasswordTimes = atoi(tmpBuf)) >= maxWrongPasswordTimes)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: wrongPasswordTimes[%d] >= %d,锁定用户!\n",wrongPasswordTimes,maxWrongPasswordTimes);
		return(errCodeOperatorMDL_PasswordLocked);
	}

	// 读取密码
	memset(userPassword,0,sizeof(userPassword));
	if ((ret = UnionReadXMLPackageValue("userPassword", userPassword, sizeof(userPassword))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","userPassword");
		return(ret);
	}
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%s%s%s",transTime,identifyCode,userPassword);
	memset(localPassword,0,sizeof(localPassword));
	UnionMD5((unsigned char *)tmpBuf, strlen(tmpBuf), (unsigned char *)localPassword);
	
	//add by yangw 20160422 begin
	//增加密码长度判断，如果为40位，表示从老系统中迁移的数据，提示密码为老格式，强制修改 userPassword
	
	if(strlen(userPassword)== 40)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: userPassword [%s]!\n",userPassword);	
		UnionSetResponseRemark("密码不为最新格式，请重设密码");
		return(errCodeOperatorMDL_OperatorPwdOldEncType);	
	}
	
	//add by yangw 20160422 end
	
	
	
	
	
	
	//验证登陆密码
	if (loginMode[0] == '2' && (ukeyVerifyMode == 1 || ukeyVerifyMode == 2))	 // USBKey登陆
	{
		memset(userPK,0,sizeof(userPK));
		if ((ret = UnionReadXMLPackageValue("userPK", userPK, sizeof(userPK))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","userPK");
			return(ret);
		}
		userPK_len = ret;
		if(memcmp(userPK,"SM2",3) == 0)//edit by yang 20150105 add the branch 
		{
			userPK_len -= 3;
			memmove(userPK,userPK+3,userPK_len);
			userPK[userPK_len] = 0;
			
			reqPassword_len = strlen(reqPassword);
			localPassword_len = strlen(localPassword);
	
			memset(x_coordinate,0,sizeof(x_coordinate));
			memset(y_coordinate,0,sizeof(y_coordinate));
			memset(r_coordinate,0,sizeof(r_coordinate));
			memset(s_coordinate,0,sizeof(s_coordinate));
			memset(x_coordinate_asc,0,sizeof(x_coordinate_asc));
			memset(y_coordinate_asc,0,sizeof(y_coordinate_asc));
			memset(r_coordinate_asc,0,sizeof(r_coordinate_asc));
			memset(s_coordinate_asc,0,sizeof(s_coordinate_asc));
		
			//拆分x y
			memcpy(x_coordinate_asc,userPK,userPK_len/2);
			memcpy(y_coordinate_asc,userPK+userPK_len/2,userPK_len/2);
			
			//拆分签名r s
			memcpy(r_coordinate_asc,reqPassword,reqPassword_len/2);
			memcpy(s_coordinate_asc,reqPassword+reqPassword_len/2,reqPassword_len/2);
			
			x_coordinate_len = aschex_to_bcdhex(x_coordinate_asc,userPK_len/2,x_coordinate);
			y_coordinate_len = aschex_to_bcdhex(y_coordinate_asc,userPK_len/2,y_coordinate);
			r_coordinate_len = aschex_to_bcdhex(r_coordinate_asc,reqPassword_len/2,r_coordinate);
			s_coordinate_len = aschex_to_bcdhex(s_coordinate_asc,reqPassword_len/2,s_coordinate);
						
			ret = sm2_verify((unsigned char *)localPassword,localPassword_len,(unsigned char *)r_coordinate,r_coordinate_len,(unsigned char *)s_coordinate,s_coordinate_len, (unsigned char *)x_coordinate,x_coordinate_len, (unsigned char *)y_coordinate,y_coordinate_len);
			if (ret!=1)
			{
				// 更新用户表
				memset(sql,0,sizeof(sql));
				if (wrongPasswordTimes + 1 < maxWrongPasswordTimes)
					sprintf(sql,"update sysUser set loginFlag = 0,wrongPasswordTimes = %d,identifyCode = '' where userID = '%s'",wrongPasswordTimes + 1,userID);
				else
					sprintf(sql,"update sysUser set loginFlag = 0,wrongPasswordTimes = %d,identifyCode = '',userStatus = 0 where userID = '%s'",wrongPasswordTimes + 1,userID);
		
				if ((ret = UnionExecRealDBSql(sql)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCode0101:: UnionExecRealDBSql[%s]!\n",sql);
				}	
				UnionUserErrLog("in UnionDealServiceCode0101:: sm2_verify failed!\n");
				return(errCodeOperatorMDL_WrongPassword);
			}
			isVerifyPasswd = 0;
		}
		else if(memcmp(userPK,"RSA",3) == 0)
		{
			userPK_len -= 3;
			memmove(userPK,userPK+3,userPK_len);
			userPK[userPK_len] = 0;
		}
	}
	else if(loginMode[0] == '1')//动态口令登录
	{
		memset(userPK,0,sizeof(userPK));
		if ((ret = UnionReadXMLPackageValue("userPK", userPK, sizeof(userPK))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","userPK");
			return(ret);
		}
		//userPK里面存放的是IMEI和activeCode
		memset(otpGrp,0,sizeof(otpGrp));
		if ((count = UnionSeprateVarStrIntoVarGrp(userPK,strlen(userPK),'|',otpGrp,3)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0101:: UnionSeprateVarStrIntoVarGrp otpData = %s !\n",userPK);  //
			return(count);
		}
		
		// 读取挑战码
		memset(QCode,0,sizeof(QCode));
		if ((ret = UnionReadRequestXMLPackageValue("body/QCode",QCode,sizeof(QCode))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadRequestXMLPackageValue[%s]!\n","body/QCode");
		}
		
		//读取偏移量
		memset(tmp,0,sizeof(tmp));
		if ((ret = UnionReadRequestXMLPackageValue("body/offset",tmp,sizeof(tmp))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadRequestXMLPackageValue[%s]!\n","body/offset");
			offset = 0;
		}
		else
		{
			offset = atoi(tmp);
		}
		//验证口令
		if((ret = UnionVerifyOTPEx(otpGrp[0],otpGrp[1],NULL,reqPassword,&offset))<0)
		{
			//口令验证失败
			// 更新用户表
			memset(sql,0,sizeof(sql));
			if (wrongPasswordTimes + 1 < maxWrongPasswordTimes)
				sprintf(sql,"update sysUser set loginFlag = 0,wrongPasswordTimes = %d,identifyCode = '' where userID = '%s'",wrongPasswordTimes + 1,userID);
			else
				sprintf(sql,"update sysUser set loginFlag = 0,wrongPasswordTimes = %d,identifyCode = '',userStatus = 0 where userID = '%s'",wrongPasswordTimes + 1,userID);
	
			if ((ret = UnionExecRealDBSql(sql)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0101:: UnionExecRealDBSql[%s]!\n",sql);
			}	
			UnionUserErrLog("in UnionDealServiceCode0101:: UnionVerifyOTPEx failed!\n");
			return(errCodeOperatorMDL_WrongPassword);
		}
		isVerifyPasswd = 0;
	}

	if (isVerifyPasswd)
	{
		if (strcmp(localPassword,reqPassword) != 0)
		{
			// 更新用户表
			memset(sql,0,sizeof(sql));
			if (wrongPasswordTimes + 1 < maxWrongPasswordTimes)
				sprintf(sql,"update sysUser set loginFlag = 0,wrongPasswordTimes = %d,identifyCode = '' where userID = '%s'",wrongPasswordTimes + 1,userID);
			else
				sprintf(sql,"update sysUser set loginFlag = 0,wrongPasswordTimes = %d,identifyCode = '',userStatus = 0 where userID = '%s'",wrongPasswordTimes + 1,userID);
	
			if ((ret = UnionExecRealDBSql(sql)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0101:: UnionExecRealDBSql[%s]!\n",sql);
				//return(ret);
			}
			
			UnionUserErrLog("in UnionDealServiceCode0101:: tmpBuf = [%s] localPassword[%s] != reqPassword[%s]!\n",tmpBuf,localPassword,reqPassword);
			return(errCodeOperatorMDL_WrongPassword);
		}
	}
	// 读取姓名
	memset(userName,0,sizeof(userName));
	if ((ret = UnionReadXMLPackageValue("userName", userName, sizeof(userName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","userName");
		return(ret);
	}

	// 读取角色
	memset(userRoleList,0,sizeof(userRoleList));
	if ((ret = UnionReadXMLPackageValue("userRoleList", userRoleList, sizeof(userRoleList))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","userRoleList");
		return(ret);
	}

	// 读取机构
	memset(organization,0,sizeof(organization));
	if ((ret = UnionReadXMLPackageValue("organization", organization, sizeof(organization))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","organization");
		return(ret);
	}

	// 读取机构ID
	if ((ret = UnionReadXMLPackageValue("organizationID", organizationID, sizeof(organizationID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","organizationID");
		return(ret);
	}
	organizationID[ret] = 0;


	// 读取登录次数
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("loginTimes", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","loginTimes");
		return(ret);
	}
	loginTimes = atoi(tmpBuf);

	// 读取创建时间
	memset(createTime,0,sizeof(createTime));
	if ((ret = UnionReadXMLPackageValue("createTime", createTime, sizeof(createTime))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","createTime");
		return(ret);
	}

	// 读取上次登录时间
	memset(lastLoginTime,0,sizeof(lastLoginTime));
	if ((ret = UnionReadXMLPackageValue("lastLoginTime", lastLoginTime, sizeof(lastLoginTime))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","lastLoginTime");
		return(ret);
	}
	// 读取操作终端列表
	memset(operTermList,0,sizeof(operTermList));
	if ((ret = UnionReadXMLPackageValue("operTermList", operTermList, sizeof(operTermList))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","operTermList");
		//return(ret);
	}

	// 读取密码更新时间
	if ((ret = UnionReadXMLPackageValue("passwdUpdateTime",passwdUpdateTime,sizeof(passwdUpdateTime))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue [%s]!\n","passwdUpdateTime");	
		return(ret);	
	}
	passwdUpdateTime[ret] = 0;

	// 读取备注
	memset(remark,0,sizeof(remark));
	if ((ret = UnionReadXMLPackageValue("remark", remark, sizeof(remark))) < 0)
	{
		//UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","remark");
		//return(ret);
	}

	if (strlen(operTermList) != 0)
	{
		memset(operTermGrp,0,sizeof(operTermGrp));
		if ((operTermNum = UnionSeprateVarStrIntoVarGrp(operTermList,strlen(operTermList),',',operTermGrp,maxOperTerm)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0101:: UnionSeprateVarStrIntoVarGrp[%s]!\n",operTermList);
			return(operTermNum);
		}

		memset(tmpBuf,0,sizeof(tmpBuf));
		len = 0;
		for (i = 0; i < operTermNum; i++)
		{
			len += sprintf(tmpBuf + len,"'%s',",operTermGrp[i]);
		}

		tmpBuf[len-1] = 0;	

		// 检查IP地址
		memset(sql,0,sizeof(sql));
		sprintf(sql,"select ipAddr from operTerm where termName in (%s)",tmpBuf);
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0101:: UnionSelectRealDBRecord[%s]!\n",sql);
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0101:: ipAddr[%s] is invalid!\n",clientIPAddr);
			UnionSetResponseRemark("操作员IP地址非法");
			return(errCodeInvalidIPAddr);
		}

		// 读取记录数量
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("totalNum", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s]!\n","totalNum");
			return(ret);
		}
		else
			totalNum = atoi(tmpBuf);
		
		for (i = 0; i < totalNum; i++)
		{
			if ((ret = UnionLocateXMLPackage("detail",i+1)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0101:: UnionLocateXMLPackage[%s][%d]!\n","detail",i+1);
				return(ret);

			}

			memset(ipAddr,0,sizeof(ipAddr));
			if ((ret = UnionReadXMLPackageValue("ipAddr", ipAddr, sizeof(ipAddr))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0101:: UnionReadXMLPackageValue[%s][%d]!\n","ipAddr",i+1);
				return(ret);
			}

			if (strcmp(ipAddr,clientIPAddr) == 0)
			{
				break;
			}
		}
		
		if (i >= totalNum)
		{
			UnionUserErrLog("in UnionDealServiceCode0101:: cliIPAddr[%s] != ipAddr[%s]  is invalid[%d]!\n",clientIPAddr,ipAddr,i);
			UnionSetResponseRemark("操作员IP地址非法");
			return(errCodeInvalidIPAddr);
		}
	}

	// 检查密码是否过期
	if ((ret = UnionIsOverduePasswd(passwdUpdateTime,NULL)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionIsOverduePasswd [%s]!\n",passwdUpdateTime);	
		UnionSetResponseRemark("密码已过期，请重设密码");
		return(errCodeOperatorMDL_OperatorPwdOverTime);
	}
	
	
	

	memset(systemTime,0,sizeof(systemTime));
	UnionGetFullSystemDateTime(systemTime);
	
	// 更新用户表
	memset(sql,0,sizeof(sql));
	sprintf(sql,"update sysUser set loginFlag = 1, "
		"loginTimes = loginTimes + 1, "
		"wrongPasswordTimes = 0, "
		"lastLoginLocation = '%s', "
		"lastLoginTime = '%s', "
		"lastOperationTime = '%s', "
		"identifyCode = '', "
		"loginSysID = '%s' "
		"where userID = '%s'",
		clientIPAddr,systemTime,systemTime,sysID,userID);

	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionExecRealDBSql[%s]!\n",sql);
		return(errCodeDatabaseMDL_RecordUpdateFailure);
	}
	
	// 设置响应数据
	// 设置姓名
	if ((ret = UnionSetResponseXMLPackageValue("body/userName",userName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionSetResponseXMLPackageValue[%s]!\n","body/userName");
		return(ret);
	}
	// 设置角色
	if ((ret = UnionSetResponseXMLPackageValue("body/userRoleList",userRoleList)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionSetResponseXMLPackageValue[%s]!\n","body/userRoleList");
		return(ret);
	}
	// 设置组织
	if ((ret = UnionSetResponseXMLPackageValue("body/organization",organization)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionSetResponseXMLPackageValue[%s]!\n","body/organization");
		return(ret);
	}

	// 设置组织ID
	if ((ret = UnionSetResponseXMLPackageValue("body/organizationID",organizationID)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionSetResponseXMLPackageValue[%s]!\n","body/organizationID");
		return(ret);
	}
	// 设置登录次数
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d",loginTimes + 1);
	if ((ret = UnionSetResponseXMLPackageValue("body/loginTimes",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionSetResponseXMLPackageValue[%s]!\n","body/loginTimes");
		return(ret);
	}
	// 设置创建时间
	if ((ret = UnionSetResponseXMLPackageValue("body/createTime",createTime)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionSetResponseXMLPackageValue[%s]!\n","body/createTime");
		return(ret);
	}
	// 设置上次登录时间
	if ((ret = UnionSetResponseXMLPackageValue("body/lastLoginTime",lastLoginTime)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionSetResponseXMLPackageValue[%s]!\n","body/lastLoginTime");
		return(ret);
	}
	// 设置上次登录地点
	if ((ret = UnionSetResponseXMLPackageValue("body/lastLoginLocation",lastLoginLocation)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionSetResponseXMLPackageValue[%s]!\n","body/lastLoginLocation");
		return(ret);
	}
	// 设置操作终端列表
	if ((ret = UnionSetResponseXMLPackageValue("body/operTermList",operTermList)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionSetResponseXMLPackageValue[%s]!\n","body/operTermList");
		return(ret);
	}
	// 设置备注
	if ((ret = UnionSetResponseXMLPackageValue("body/remark",remark)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0101:: UnionSetResponseXMLPackageValue[%s]!\n","body/remark");
		return(ret);
	}
	
	return(0);
}

