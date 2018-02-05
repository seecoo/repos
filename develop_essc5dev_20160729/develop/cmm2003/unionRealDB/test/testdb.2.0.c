// Copyright:	Union Tech.
// Author:	lics
// Date:	2014-04-01
// Version:	2.0

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#include "UnionTask.h"
#include "unionErrCode.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "unionRealBaseDB.h"
#include "unionRealDBCommon.h"
#include "unionXMLPackage.h"


int UnionTaskActionBeforeExit()
{
        exit(1);
}


static char	sch;
//static int	testFlag = 0;
static int	testTimes;
static int	testNums;
static char	tmpBuf[128];
static int 	nFirstFlag = 0;
static int 	nFuncFlag = 0;
static unsigned int 	g_second = 0;
static unsigned int 	g_usecond = 0;

struct timeval 	tvStart;

extern void UnionRedirectLogFileToSpecFile(char *fileName);


void time_start()
{
	gettimeofday(&tvStart,NULL);
	
}

void time_end()
{
	struct timeval  		now;
	
	gettimeofday(&now,NULL);
	if(now.tv_usec > tvStart.tv_usec)
	{
		g_second = now.tv_sec - tvStart.tv_sec;
		g_usecond = now.tv_usec - tvStart.tv_usec;
	}
	else
	{
		g_second = now.tv_sec - tvStart.tv_sec -1;
		g_usecond = now.tv_usec + 1000000 - tvStart.tv_usec;
	}
}


#define NUM_OF_MAINOPT  5
#define NUM_OF_FUNCOPT  5


const char *sarray[NUM_OF_MAINOPT]={
	"*   1  连接测试                *\n",
	"*   2  查询权限测试            *\n",
	"*   3  更新权限测试            *\n",
	"*   4  数据库底层接口测试      *\n",
	"*   q(Q)  退出！               *\n"
};


const char *sFunc[NUM_OF_FUNCOPT]={
	"*  1  testUnionSelectRealDBRecordCounts 测试             *\n",
	"*  2  testUnionSelectRealDBRecord  测试                  *\n",
	"*  3  testUnionExecRealDBSql  测试                       *\n",
	"*  4  testUnionSelectRealDBRecord 测试并输出指定字段结果 *\n",
	"*  r(R)   返回主菜单                                     *\n"
};


void  gra()
{
	nFuncFlag = 0;
	if(nFirstFlag != 0)
	{
		printf("\n\n   Press any key to continue...\n");
		getchar();
		
	}
	
	nFirstFlag = 1;
	
	int i = 0;
	printf("\n\n\n\n\n\n");
	printf("**********************************************************\n");
	printf("* 请输入操作选项：                                       *\n");
  	printf("*                                                        *\n");

  	for(i = 0; i< NUM_OF_MAINOPT ; i++)
  	{
     		printf("%s", sarray[i]);
  	}

  	printf("*                                                        *\n");
  	printf("**********************************************************\n");
}

void  graFunc()
{
	nFirstFlag = 0;
	if(nFuncFlag != 0)
	{
		printf("\n\n   Press any key to continue...\n");
		getchar();
		
	}
	
	nFuncFlag = 1;
		
	int i = 0;
	printf("\n\n\n\n\n\n");
	printf("**********************************************************\n");
	printf("* 请输入操作选项：                                       *\n");
	printf("*                                                        *\n");

	for(i = 0; i< NUM_OF_FUNCOPT ; i++)
	{
		printf("%s", sFunc[i]);
	}

	printf("*                                                        *\n");
	printf("**********************************************************\n");

}

int testConnect()
{
	if(UnionConnectDatabase() != 0)
	{
		UnionUserErrLog("connect to db  failed !\n");
	  	return -1;
	}
	  
    	if(UnionCloseDatabase() != 0)
	{
		UnionUserErrLog("close db failed!\n");
	}
	  
	return 0;
}


int testQuery()
{
	int ret;
 
	if(UnionConnectDatabase() != 0)
	{
      		UnionUserErrLog("connect to db  failed !\n");
	  	return -1;
	}
	  
	ret = UnionSelectRealDBRecordCounts("unionREC", 0, 0);
	  
	if(ret < 0)
	{
		UnionUserErrLog("UnionSelectRealDBRecordCounts error! \n");
	}
	  
	if(UnionCloseDatabase() != 0)
	{
		UnionUserErrLog("close db failed!\n");
	}
	  
	return ret;
}

int testModify()
{
	int ret;
 
	if(UnionConnectDatabase() != 0)
	{
		UnionUserErrLog("connect to db  failed !\n");
	  	return -1;
	}
	  
	ret = UnionExecRealDBSql("update unionREC set varValue = varValue where varSysName = 'maxClassNum'");
						
	if(ret < 0)
	{
		UnionUserErrLog("UnionExecRealDBSql error! \n");
	}
		
	if(UnionCloseDatabase() != 0)
	{
		UnionUserErrLog("close db failed!\n");
	}
	  
	return ret;
	  
}

void testUnionSelectRealDBRecordCounts()
{
	int i,j;
	char tName[32];
	char joinCondition[128];
	char condition[128];
	int rnum = 0;
	int ret = 0;
	
        UnionLog("----------    test UnionSelectRealDBRecordCounts  ----------\n");
	printf("please input needed info:\n");
	printf("tableName:");
	fgets(tName, 32, stdin);
	tName[strlen(tName)-1] = 0;
	
	printf("joinCondition:");
	fgets(joinCondition, 128, stdin);
	joinCondition[strlen(joinCondition)-1] = 0;
	
	printf("condition:");
	fgets(condition, 128, stdin);
	condition[strlen(condition)-1] = 0;
	
	UnionLog(" ----------------------------------parent  start---------------------------------------------------\n");
	
	time_start();
	
	for(i = 0; i < testNums; i++)
	{
		if(fork() == 0)
		{
			
			if(UnionConnectDatabase() != 0)
		  	{
		  		UnionUserErrLog("connect to db  failed !\n");
		  		exit(-1);
		  	}
		  
		 	 UnionLog(" --------------------child start------------------------------\n");
		  
			
			for(j = 0; j < testTimes; j++)
			{
				ret = UnionSelectRealDBRecordCounts(tName, joinCondition, condition);
				
				if(ret < 0)
				{
					 UnionUserErrLog("UnionSelectRealDBRecordCounts error! \n");
				   exit(-1);
				}
				
				rnum += ret;
				   
			}
			
			UnionLog("deal record num: %d .\n", rnum);
			UnionLog(" --------------------child end---------------------------------\n");
			
		  	if(UnionCloseDatabase() != 0)
		  	{
		  		UnionUserErrLog("close db failed!\n");
		  	}
			
			exit(0);
		}
		
		usleep(10);
	}
	
	return;
	
}

void  testUnionSelectRealDBRecord()
{
	char *front = NULL, *back = NULL;
	int i, j, k;
	char sql[512];
	char currentPage[32];
	char numOfPerPage[32];
	char fieldList[512];
	int rnum = 0;
	int ret;
        int totalNum = 0;
        char field[32];
	
	UnionLog("----------  test UnionSelectRealDBRecord  ----------\n");
	printf("please input needed info:\n");
	
	printf("sql:");
	fgets(sql, 512, stdin);
	sql[strlen(sql)-1] = 0;

	printf("currentPage:");
	fgets(currentPage, 32, stdin);
	currentPage[strlen(currentPage)-1] = 0;
	
	
	printf("numOfPerPage:");
	fgets(numOfPerPage, 32, stdin);
	numOfPerPage[strlen(numOfPerPage)-1] = 0;
	
	if(sch == '4')
	{
		printf("  input the field names delimited by \",\" with lowcase\n");
		printf("  fieldList:");
		fgets(fieldList, 512, stdin);
		fieldList[strlen(fieldList)-1] = 0;
	}
	
	
	UnionLog(" ----------------------------------parent  start---------------------------------------------------\n");
  
        time_start();
	
	for(k = 0; k < testNums; k++)
	{
		if(fork() == 0)
		{
			
			if(UnionConnectDatabase() != 0)
		       	{
		  		UnionUserErrLog("connect to db  failed !\n");
		  		exit(-1);
		       	}
		  
	                UnionLog(" --------------------child start------------------------------\n");
		 
			
			for(j = 0; j < testTimes; j++)
			{
				ret = UnionSelectRealDBRecord(sql, atoi(currentPage), atoi(numOfPerPage));
				
				if(ret < 0)
				{
				   UnionUserErrLog("testUnionSelectRealDBRecord error! \n");
				   exit(-1);
				}
				
				rnum += ret;
			}
			
			
			if(sch == '4')
			{
				// 读取总数
				memset(tmpBuf,0,sizeof(tmpBuf));
				if ((ret = UnionReadXMLPackageValue("totalNum",tmpBuf,sizeof(tmpBuf))) < 0)
				{
					UnionUserErrLog("UnionReadXMLPackageValue[%s]!\n","totalNum");
					return; 
				}
				else
				{
					totalNum = atoi(tmpBuf);
			  	}
				
				for (i = 1; i <= totalNum; i++)
				{
					if ((ret = UnionLocateXMLPackage("detail",i)) < 0)
					{
						UnionUserErrLog("UnionLocateXMLPackage[%s][%d]!\n","detail",i);
					}
			    
			    		if(fieldList[0] != 0)
			    		{
				    		front = fieldList;
				    		back = strstr(front, ",");
				 	}
			    
			   		while(back != NULL)
			    		{
			    			strncpy(field, front, back-front);
			    			field[back - front] = 0;
			    	
				    		memset(tmpBuf,0,sizeof(tmpBuf));
						if ((ret = UnionReadXMLPackageValue(field,tmpBuf,sizeof(tmpBuf))) < 0)
						{
							UnionUserErrLog("UnionReadRequestXMLPackageValue[%s][%d]!\n",field,i);
						}
						else
						{
							UnionLog("UnionReadRequestXMLPackageValue[%s][%d][%s]!\n",field,i,tmpBuf);
						}
						
						front = back + 1;
						back = strstr(front, ",");
			    		}
			    
			    		if(front != NULL)
			    		{
			   			memset(tmpBuf,0,sizeof(tmpBuf));
						if ((ret = UnionReadXMLPackageValue(front,tmpBuf,sizeof(tmpBuf))) < 0)
						{
							UnionUserErrLog("UnionReadRequestXMLPackageValue[%s][%d]!\n",front,i);
						}
						else
						{
							UnionLog("UnionReadRequestXMLPackageValue[%s][%d][%s]!\n",front,i,tmpBuf);
						}
				  	}

				}
				
			}
			
			UnionLog("deal record num: %d .\n", rnum);
			UnionLog(" --------------------child end---------------------------------\n");
			
		  	if(UnionCloseDatabase() != 0)
		  	{
				UnionUserErrLog("close db failed!\n");
		  	}
			
			exit(0);
		}
		
		usleep(10);
	}
	
	return;
}

void testUnionExecRealDBSql()
{
	int i, j;
	char sql[512];
	int rnum = 0;
	int ret;

	
	UnionLog("--------------  test UnionExecRealDBSql  ---------------\n");
	printf("please input needed info:\n");
	printf("sql:");
        fgets(sql, 512, stdin);
        sql[strlen(sql)-1] = 0;
	
	UnionLog(" ----------------------------------parent  start---------------------------------------------------\n");
	
	time_start();
	
	for(i = 0; i < testNums; i++)
	{
		
		if(fork() == 0)
		{
			
			if(UnionConnectDatabase() != 0)
		  	{
		  		UnionUserErrLog("connect to db  failed !\n");
		  		exit(-1);
		  	}
		  
		  	UnionLog(" --------------------child start------------------------------\n");
		  
			
			for(j = 0; j < testTimes; j++)
			{
				ret = UnionExecRealDBSql(sql);
								
				if(ret < 0)
				{
					UnionUserErrLog("testUnionExecRealDBSql error! \n");
				   	exit(-1);
				}
				
				rnum += ret;
			}
			
			UnionLog("deal record num: %d .\n", rnum);
			UnionLog(" --------------------child end---------------------------------\n");
			
		  	if(UnionCloseDatabase() != 0)
		  	{
		  		UnionUserErrLog("close db failed!\n");
		  	}
			
			exit(0);
		}
		
		usleep(10);
	}
	
	return;
}


void help(char *p)
{
	printf("Usage:: %s flag times num \n", p);
	printf("   %s 1 loopTimes  pnum  //testUnionSelectRealDBRecordCounts\n", p);
	printf("   %s 2 loopTimes  pnum  //testUnionSelectRealDBRecord\n", p);
	printf("   %s 3 loopTimes  pnum  //testUnionExecRealDBSql\n", p);
	printf("   %s 4 loopTimes  pnum  //testUnionSelectRealDBRecord  and print the selected data. \n", p);
}

int testFunc(char *p)
{ 
	int	status;
	int	tempNums;
  
funcTag:
  
  	graFunc();
  
  	printf(" :");
	sch = getchar();
	getchar();
	
	if(sch != 'R' && sch != 'r')
	{
		if(sch == '\n')
		{
			goto funcTag;
		} 
		memset(tmpBuf, 0x00, sizeof(tmpBuf));
		printf("\n输入并发进程个数：");
		fgets(tmpBuf, 24, stdin);
		
		testNums = atoi(tmpBuf);
		if(testNums <= 0)
		{
			printf("\n     参数输入有误， 请检查后重新输入\n");
			
			goto funcTag;
		}
			
		memset(tmpBuf, 0x00, sizeof(tmpBuf));
		printf("\n输入每个进程执行操作的次数：");
		fgets(tmpBuf, 24, stdin);
		
		testTimes = atoi(tmpBuf);
		if(testTimes <= 0)
		{
			printf("\n     参数输入有误， 请检查后重新输入\n");
			
			goto funcTag;
		}
	}
	
	switch(sch)
	{
		case '1':
			testUnionSelectRealDBRecordCounts();
			break;
			
	  	case '2':
	  	case '4':
	 	  	testUnionSelectRealDBRecord();
	 	  	break;
	 	  
	 	case '3':
	 		testUnionExecRealDBSql();
	 		break;
	 	
	 	case 'R':
	 	case 'r':
	 		return 0;
	 		
	 	default:
		printf("\n     选项输入有误， 请检查后重新输入\n");
     	
     		goto funcTag;
	 		
	}
	
	tempNums = testNums;
	
	while(1)
	{
		wait(&status);
		tempNums--;
	 
	 	if(tempNums == 0)
	 	break;
	}
	
	time_end();
	
	printf("\n        测试结果： 测试完成，具体信息查看日志文件[%s]， 耗时： %d 秒 %d 微秒.\n", p, g_second, g_usecond);
	
	UnionLog(" ------------------------------------parent  end---------------------------------------------------\n");
	
  	goto funcTag;

	return(0);
}


int main(int argc, char** argv)
{
	char	c;
	char	logName[256];
	int ret = 0;
	
	memset(logName, 0, sizeof(logName));
	strcpy(logName, "unknown.log");

        if(argc > 1)
        {
         	if(strcmp(argv[1], "version") == 0)
         	{
           		printf("usage: %s\n", argv[0]);
           		exit(0);
         	}
        }
	
graTag:
		
	gra();
	
	printf(" :");
	c = getchar();
	getchar();
  
	switch(c)
  	{
  		case '1':
  			time_start();
  			ret = testConnect();
  			time_end();
  		
  			if(ret == 0)
  			printf("\n        测试结果： 数据库连接成功[DBUSER: %s,  DBNAME: %s]， 耗时： %d 秒 %d 微秒.\n", getenv("DBUSER"), getenv("DBNAME"), g_second, g_usecond);
  			else
  			printf("\n        测试结果： 数据库连接失败[DBUSER: %s,  DBNAME: %s]， 耗时： %d 秒 %d 微秒.\n", getenv("DBUSER"), getenv("DBNAME"), g_second, g_usecond);
  			
			break;
  	
  		case '2':
  		 	time_start();
  		 	ret = testQuery();
  		 	time_end();
  		 
  		 	if(ret < 0)
         		printf("\n        测试结果： 数据表查询失败， 耗时： %d 秒 %d 微秒.\n", g_second, g_usecond);
       			else
         		printf("\n        测试结果： 数据表查询成功， 受影响记录数：%d， 耗时： %d 秒 %d 微秒.\n", ret, g_second, g_usecond);
         
       			break;
       
       
     		case '3':
     			time_start();
     			ret = testModify();
     			time_end();
   		 
  			if(ret < 0)
         		printf("\n        测试结果： 数据表更新失败， 耗时： %d 秒 %d 微秒.\n", g_second, g_usecond);
      			else
         		printf("\n        测试结果： 数据表更新成功， 受影响记录数：%d， 耗时： %d 秒 %d 微秒.\n", ret, g_second, g_usecond);
         
       			break;
       
          	
     		case '4':
     			ret = testFunc(logName);
     	
      			break;
     	
     		case 'q':
     		case 'Q':
     		exit(0);
     	
     		default:
     			printf("\n     选项输入有误， 请检查后重新输入\n");
     	
  			
	}
  
	goto graTag;
	
}




/*

int main(int argc, char** argv))
{ 
	char	c;
	int	status;
	int	tempNums;
  

	if(argc < 3)	
	{
		help(argv[0]);
		exit(-1);
	}
	
	
	testFlag = atoi(argv[1]);
	testTimes = atoi(argv[2]);
	testNums = atoi(argv[3]);
	
	Tag:
	
  	switch(testFlag)
	{
		case 1:
			testUnionSelectRealDBRecordCounts();
			
			break;
			
	  	case 2:
	  	case 4:
	 	  	testUnionSelectRealDBRecord();
	 	  	break;
	 	  
	 	case 3:
	 		testUnionExecRealDBSql();
	 		break;
	 		
	 	default:
	 		break;
	 		
	}
	
	tempNums = testNums;
	
	while(1)
	{
		wait(&status);
	 	tempNums--;
	 
	 	if(tempNums == 0)
	 	break;
	}
	

	
	memset(tmpBuf, 0, sizeof(tmpBuf));
	UnionGetCurrentDateTimeInMacroFormat(tmpBuf);
	
	printf("\n--------------------all  end------------------------------time: %s\n\n", tmpBuf);
	
	printf("\n\n   -----continue test.... Y/N?\n\n:");
	
	c = getchar();
	getchar();
	if(c == 'Y' || c-32 == 'Y')
  	{
     		goto Tag;
  	}

	return(0);
}

*/


