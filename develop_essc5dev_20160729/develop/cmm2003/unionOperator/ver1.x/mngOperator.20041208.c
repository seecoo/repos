//	Wolfgang Wang
//	2003/09/09

#define _UnionTask_3_x_
#define _UnionLogMDL_3_x_

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <time.h>

#include "UnionLog.h"
#include "unionCommand.h"
#include "unionVersion.h"
#include "UnionTask.h"
#include "unionModule.h"

#include "unionOperator.h"

PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
	UnionDisconnectOperatorTable();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);	//return(exit(0));
}

int UnionHelp()
{
	printf("Usage:: %s command\n",UnionGetApplicationName());
	printf(" Command as follows\n");
	printf(" create	maxNumOfOperator passwordMaxEffectiveDays maxIdleTimeBetweenTwoOperations\n");
	printf("  	create a operator table\n");
	printf(" drop	drop operator table\n");
	printf(" reload reload operator table\n");
	printf(" all\n");
	printf(" attr\n");
	printf(" effective passwordMaxEffectiveDays\n");
	printf(" idletime maxIdleTimeBetweenTwoOperations\n");
	printf(" print tellerno\n");
	printf(" insert tellerno name level password\n");
	printf(" delete tellerno\n");
	printf(" level tellerno level\n");
	printf(" password tellerno password\n");
	printf(" name tellerno name\n");
	printf(" logon tellerno password\n");
	printf(" logoff tellerno\n");
	printf(" apply tellerno\n");
	printf(" unlock tellerno\n");
	printf(" outdate tellerno\n");
	return(0);
}

int CreateOperatorTable(int argc,char *argv[])
{
	int		ret = 0;
	long		maxNumOfOperator = -1;			// ���Ĺ�Ա��
	long		passwordMaxEffectiveDays = -1;		// ����������Ч����
	long		maxIdleTimeBetweenTwoOperations = -1;	// һ�ε�¼��������β������������
	char		*p;
	
	if (argc > 0)
		maxNumOfOperator = atol(argv[0]);
	if (argc > 1)
		passwordMaxEffectiveDays = atol(argv[1]);
	if (argc > 2)
		maxIdleTimeBetweenTwoOperations = atol(argv[2]);
loopUnionInput1:
	if (maxNumOfOperator <= 0)
	{
		if (UnionIsQuit(p = UnionInput("�������Ա�������Ŀ::")))
			return(0);
		maxNumOfOperator = atol(p);
		goto loopUnionInput1;
	}
loopUnionInput2:
	if (passwordMaxEffectiveDays <= 0)
	{
		if (UnionIsQuit(p = UnionInput("������������������::")))
			return(0);
		passwordMaxEffectiveDays = atol(p);
		goto loopUnionInput2;
	}
loopUnionInput3:
	if (maxIdleTimeBetweenTwoOperations <= 0)
	{
		if (UnionIsQuit(p = UnionInput("���������β�����������::")))
			return(0);
		maxIdleTimeBetweenTwoOperations = atol(p);
		goto loopUnionInput3;
	}
	
	printf("\n");
	printf("[��Ա�������Ŀ]	[%ld]\n",maxNumOfOperator);
	printf("[�����������]	[%ld]\n",passwordMaxEffectiveDays);
	printf("[�����������]	[%ld]\n",maxIdleTimeBetweenTwoOperations);
	if (UnionConfirm("ȷ�ϴ��������Ĺ�Ա����?"))
	{
		if ((ret = UnionCreateOperatorTable(maxNumOfOperator,passwordMaxEffectiveDays,maxIdleTimeBetweenTwoOperations)) < 0)
			printf("UnionCreateOperatorTable Error! ret = [%d]\n",ret);
		else
			printf("UnionCreateOperatorTable OK!\n");
	}
	return(ret);
}

//int DropKeyDB()
int DropOperatorTable()
{
	int	ret = 0;
	
	if (UnionConfirm("ȷ��ɾ����Ա����?"))
	{
		if ((ret = UnionDeleteOperatorTable()) < 0)
			printf("UnionDeleteOperatorTable Error! ret = [%d]\n",ret);
		else
			printf("UnionDeleteOperatorTable OK!\n");
	}
	return(ret);
}

int InsertOperator(int argc,char *argv[])
{
	int		ret = 0;
	TUnionOperator	oper;
	char		*p;	
	char		password[512];
	
	memset(&oper,0,sizeof(oper));
	if (argc > 0)
	{
		if (strlen(argv[0]) >= sizeof(oper.id))
		{
			printf("��Ա�Ŵ��� [%s]\n",argv[0]);
			return(ret);
		}
		strcpy(oper.id,argv[0]);
	}
	else
	{
loop1:
		if (strlen(p=UnionInput("�������Ա��[%dλ]::",sizeof(oper.id)-1)) >= sizeof(oper.id))
		{
			printf("��Ա�Ŵ���[%s]��������\n",p);
			goto loop1;
		}
		strcpy(oper.id,p);
		if (UnionIsQuit(p))
			return(0);
	}
	if (argc > 1)
	{
		if (strlen(argv[1]) >= sizeof(oper.name))
		{
			printf("��Ա���ƴ���[%s]\n",argv[1]);
			return(ret);
		}
		strcpy(oper.name,argv[1]);
	}
	else
	{
loop2:
		if (strlen(p=UnionInput("�������Ա����[���%dλ]::",sizeof(oper.name)-1)) >= sizeof(oper.name))
		{
			printf("��Ա���ƴ���[%s]��������\n",p);
			goto loop2;
		}
		strcpy(oper.name,p);
		if (UnionIsQuit(p))
			return(0);
	}
	if (argc > 2)
	{
		if ((argv[2][0] > '9') || (argv[2][0] < '0'))
		{
			printf("��Ա�������[%s]\n",argv[2]);
			return(ret);
		}
		oper.level = argv[2][0];
	}
	else
	{
loop3:
		if (UnionIsQuit(p=UnionInput("�������Ա����[0~9]::")))
			return(0);
		if ((p[0] > '9') || (p[0] < '0'))
		{
			printf("��Ա�������[%s]��������\n",p);
			goto loop3;
		}
		oper.level = p[0];
	}
	memset(password,0,sizeof(password));
	if (argc > 3)
	{
		memset(password,0,sizeof(password));
		strcpy(password,argv[3]);
	}
	else
	{
		p=UnionInput("�������Ա����::");
		strcpy(password,p);
		if (UnionIsQuit(p))
			return(0);
	}
	
	printf("\n");
	printf("��Ա��	[%s]\n",oper.id);
	printf("��  ��  [%s]\n",oper.name);
	printf("��  ��  [%c]\n",oper.level);
	printf("��  ��  [%s]\n",password);
	if (!UnionConfirm("ȷ�����Ӹù�Ա��?"))
		return(0);
	if ((ret = UnionInsertOperator(oper.id,oper.name,oper.level,password)) < 0)
		printf("UnionInsertOperator error! ret = [%d]\n",ret);
	else
		printf("UnionInsertOperator OK!\n");

	return(ret);
}

int DeleteOperator(int argc,char *argv[])
{
	int	ret;
	int	i;
	char	*p;
	
	for (i = 0; i < argc; i++)
	{	
		if (!UnionConfirm("ȷ��ɾ����Ա[%s]��?",argv[i]))
			continue;
		if ((ret = UnionDeleteOperator(argv[i])) < 0)
			printf("UnionDeleteOperator [%s] Error! ret = [%d]\n",argv[i],ret);
		else
			printf("UnionDeleteOperator [%s] OK!\n",argv[i]);
	}
	if (argc > 0)
		return(0);

	p = UnionInput("�������Ա��::");
	if (!UnionConfirm("ȷ��ɾ����Ա[%s]��?",p))
		return(0);
	if ((ret = UnionDeleteOperator(argv[i])) < 0)
		printf("UnionDeleteOperator [%s] Error! ret = [%d]\n",p,ret);
	else
		printf("UnionDeleteOperator [%s] OK!\n",p);
	return(ret);
}	

int PrintOperator(int argc,char *argv[])
{
	int	ret;
	int	i;
	
	for (i = 0; i < argc; i++)
	{
		if ((ret = UnionPrintOperator(argv[i])) < 0)
			printf("UnionPrintOperator [%s] Error! ret = [%d]\n",argv[i], ret);
	}
	if (argc > 0)
		return(0);
	return(UnionPrintOperator(UnionInput("�������Ա��::")));
}

int UpdateName(int argc,char *argv[])
{
	int		ret = 0;
	TUnionOperator	oper;
	char		*p;	
	
	memset(&oper,0,sizeof(oper));
	if (argc > 0)
	{
		if (strlen(argv[0]) >= sizeof(oper.id))
		{
			printf("��Ա�Ŵ��� [%s]\n",argv[0]);
			return(ret);
		}
		strcpy(oper.id,argv[0]);
	}
	else
	{
loop1:
		if (strlen(p=UnionInput("�������Ա��[%dλ]::",sizeof(oper.id)-1)) >= sizeof(oper.id))
		{
			printf("��Ա�Ŵ���[%s]��������\n",p);
			goto loop1;
		}
		strcpy(oper.id,p);
		if (UnionIsQuit(p))
			return(0);
	}
	if (argc > 1)
	{
		if (strlen(argv[1]) >= sizeof(oper.name))
		{
			printf("��Ա���ƴ���[%s]\n",argv[1]);
			return(ret);
		}
		strcpy(oper.name,argv[1]);
	}
	else
	{
loop2:
		if (strlen(p=UnionInput("�������Ա����[���%dλ]::",sizeof(oper.name)-1)) >= sizeof(oper.name))
		{
			printf("��Ա���ƴ���[%s]��������\n",p);
			goto loop2;
		}
		strcpy(oper.name,p);
		if (UnionIsQuit(p))
			return(0);
	}
	printf("\n");
	printf("��Ա��	[%s]\n",oper.id);
	printf("��  ��  [%s]\n",oper.name);
	if (!UnionConfirm("ȷ���޸Ĺ�Ա������?"))
		return(0);
	if ((ret = UnionUpdateOperatorName(oper.id,oper.name)) < 0)
		printf("UnionUpdateOperatorName error! ret = [%d]\n",ret);
	else
		printf("UnionUpdateOperatorName OK!\n");

	return(ret);
}

int UpdateLevel(int argc,char *argv[])
{
	int		ret = 0;
	TUnionOperator	oper;
	char		*p;	
	
	memset(&oper,0,sizeof(oper));
	if (argc > 0)
	{
		if (strlen(argv[0]) >= sizeof(oper.id))
		{
			printf("��Ա�Ŵ��� [%s]\n",argv[0]);
			return(ret);
		}
		strcpy(oper.id,argv[0]);
	}
	else
	{
loop1:
		if (strlen(p=UnionInput("�������Ա��[%dλ]::",sizeof(oper.id)-1)) >= sizeof(oper.id))
		{
			printf("��Ա�Ŵ���[%s]��������\n",p);
			goto loop1;
		}
		strcpy(oper.id,p);
		if (UnionIsQuit(p))
			return(0);
	}
	if (argc > 1)
	{
		if ((argv[1][0] > '9') || (argv[1][0] < '0'))
		{
			printf("��Ա�������[%s]\n",argv[1]);
			return(ret);
		}
		oper.level = argv[1][0];
	}
	else
	{
loop3:
		if (UnionIsQuit(p=UnionInput("�������Ա����[0~9]::")))
			return(0);
		if ((p[0] > '9') || (p[0] < '0'))
		{
			printf("��Ա�������[%s]��������\n",p);
			goto loop3;
		}
		oper.level = p[0];
	}
	printf("\n");
	printf("��Ա��	[%s]\n",oper.id);
	printf("��  ��  [%c]\n",oper.level);
	if (!UnionConfirm("ȷ���޸Ĺ�Ա������?"))
		return(0);
	if ((ret = UnionUpdateOperatorLevel(oper.id,oper.level)) < 0)
		printf("UnionUpdateOperatorLevel error! ret = [%d]\n",ret);
	else
		printf("UnionUpdateOperatorLevel OK!\n");

	return(ret);
}

int UpdatePassword(int argc,char *argv[])
{
	int		ret = 0;
	TUnionOperator	oper;
	char		*p;	
	char		password[512];
	
	memset(&oper,0,sizeof(oper));
	if (argc > 0)
	{
		if (strlen(argv[0]) >= sizeof(oper.id))
		{
			printf("��Ա�Ŵ��� [%s]\n",argv[0]);
			return(ret);
		}
		strcpy(oper.id,argv[0]);
	}
	else
	{
loop1:
		if (strlen(p=UnionInput("�������Ա��[%dλ]::",sizeof(oper.id)-1)) >= sizeof(oper.id))
		{
			printf("��Ա�Ŵ���[%s]��������\n",p);
			goto loop1;
		}
		strcpy(oper.id,p);
		if (UnionIsQuit(p))
			return(0);
	}
	memset(password,0,sizeof(password));
	if (argc > 1)
	{
		memset(password,0,sizeof(password));
		strcpy(password,argv[1]);
	}
	else
	{
		p=UnionInput("�������Ա����::");
		strcpy(password,p);
		if (UnionIsQuit(p))
			return(0);
	}
	
	printf("\n");
	printf("��Ա��	[%s]\n",oper.id);
	printf("��  ��  [%s]\n",password);
	if (!UnionConfirm("ȷ���޸Ĺ�Ա������?"))
		return(0);
	if ((ret = UnionUpdateOperatorPassword(oper.id,password)) < 0)
		printf("UnionUpdateOperatorPassword error! ret = [%d]\n",ret);
	else
		printf("UnionUpdateOperatorPassword OK!\n");

	return(ret);
}


int UnionSetEffectiveDays(int argc,char *argv[])
{
	int		ret = 0;
	long		passwordMaxEffectiveDays = -1;		// ����������Ч����
	char		*p;
	
	if (argc > 0)
		passwordMaxEffectiveDays = atol(argv[0]);
loopUnionInput2:
	if (passwordMaxEffectiveDays <= 0)
	{
		if (UnionIsQuit(p = UnionInput("������������������::")))
			return(0);
		passwordMaxEffectiveDays = atol(p);
		goto loopUnionInput2;
	}
	
	printf("\n");
	printf("[�����������]	[%ld]\n",passwordMaxEffectiveDays);
	if (UnionConfirm("ȷ���޸��������������?"))
	{
		if ((ret = UnionUpdateOperatorPasswordMaxEffectiveDays(passwordMaxEffectiveDays)) < 0)
			printf("UnionUpdateOperatorPasswordMaxEffectiveDays Error! ret = [%d]\n",ret);
		else
			printf("UnionUpdateOperatorPasswordMaxEffectiveDays OK!\n");
	}
	return(ret);
}

int UnionSetIdleTime(int argc,char *argv[])
{
	int		ret = 0;
	long		maxIdleTimeBetweenTwoOperations = -1;	// һ�ε�¼��������β������������
	char		*p;
	
	if (argc > 0)
		maxIdleTimeBetweenTwoOperations = atol(argv[0]);
loopUnionInput3:
	if (maxIdleTimeBetweenTwoOperations <= 0)
	{
		if (UnionIsQuit(p = UnionInput("���������β�����������::")))
			return(0);
		maxIdleTimeBetweenTwoOperations = atol(p);
		goto loopUnionInput3;
	}
	
	printf("\n");
	printf("[�����������]	[%ld]\n",maxIdleTimeBetweenTwoOperations);
	if (UnionConfirm("ȷ���޸Ĳ����������?"))
	{
		if ((ret = UnionUpdateOperatorMaxIdleTimeBetweenTwoOperations(maxIdleTimeBetweenTwoOperations)) < 0)
			printf("UnionUpdateOperatorMaxIdleTimeBetweenTwoOperations Error! ret = [%d]\n",ret);
		else
			printf("UnionUpdateOperatorMaxIdleTimeBetweenTwoOperations OK!\n");
	}
	return(ret);
}

int OperatorLogon(int argc,char *argv[])
{
	int		ret = 0;
	TUnionOperator	oper;
	char		*p;	
	char		password[512];
	
	memset(&oper,0,sizeof(oper));
	if (argc > 0)
	{
		if (strlen(argv[0]) >= sizeof(oper.id))
		{
			printf("��Ա�Ŵ��� [%s]\n",argv[0]);
			return(ret);
		}
		strcpy(oper.id,argv[0]);
	}
	else
	{
loop1:
		if (strlen(p=UnionInput("�������Ա��[%dλ]::",sizeof(oper.id)-1)) >= sizeof(oper.id))
		{
			printf("��Ա�Ŵ���[%s]��������\n",p);
			goto loop1;
		}
		strcpy(oper.id,p);
		if (UnionIsQuit(p))
			return(0);
	}
	memset(password,0,sizeof(password));
	if (argc > 1)
	{
		memset(password,0,sizeof(password));
		strcpy(password,argv[1]);
	}
	else
	{
		p=UnionInput("�������Ա����::");
		strcpy(password,p);
		if (UnionIsQuit(p))
			return(0);
	}
	
	if ((ret = UnionOperatorLogon(oper.id,password)) < 0)
		printf("UnionOperatorLogon error! ret = [%d]\n",ret);
	else
		printf("UnionOperatorLogon OK!\n");

	return(ret);
}

int OperatorLogoff(int argc,char *argv[])
{
	int		ret = 0;
	TUnionOperator	oper;
	char		*p;	
	
	memset(&oper,0,sizeof(oper));
	if (argc > 0)
	{
		if (strlen(argv[0]) >= sizeof(oper.id))
		{
			printf("��Ա�Ŵ��� [%s]\n",argv[0]);
			return(ret);
		}
		strcpy(oper.id,argv[0]);
	}
	else
	{
loop1:
		if (strlen(p=UnionInput("�������Ա��[%dλ]::",sizeof(oper.id)-1)) >= sizeof(oper.id))
		{
			printf("��Ա�Ŵ���[%s]��������\n",p);
			goto loop1;
		}
		strcpy(oper.id,p);
		if (UnionIsQuit(p))
			return(0);
	}
	
	if ((ret = UnionOperatorLogoff(oper.id)) < 0)
		printf("UnionOperatorLogoff error! ret = [%d]\n",ret);
	else
		printf("UnionOperatorLogoff OK!\n");

	return(ret);
}

int OperatorApply(int argc,char *argv[])
{
	int		ret = 0;
	TUnionOperator	oper;
	char		*p;	
	
	memset(&oper,0,sizeof(oper));
	if (argc > 0)
	{
		if (strlen(argv[0]) >= sizeof(oper.id))
		{
			printf("��Ա�Ŵ��� [%s]\n",argv[0]);
			return(ret);
		}
		strcpy(oper.id,argv[0]);
	}
	else
	{
loop1:
		if (strlen(p=UnionInput("�������Ա��[%dλ]::",sizeof(oper.id)-1)) >= sizeof(oper.id))
		{
			printf("��Ա�Ŵ���[%s]��������\n",p);
			goto loop1;
		}
		strcpy(oper.id,p);
		if (UnionIsQuit(p))
			return(0);
	}
	
	if ((ret = UnionApplyOperatorOperation(oper.id)) < 0)
		printf("UnionApplyOperatorOperation error! ret = [%d]\n",ret);
	else
		printf("UnionApplyOperatorOperation OK!\n");

	return(ret);
}

int UnlockOperator(int argc,char *argv[])
{
	int		ret = 0;
	TUnionOperator	oper;
	char		*p;	
	
	memset(&oper,0,sizeof(oper));
	if (argc > 0)
	{
		if (strlen(argv[0]) >= sizeof(oper.id))
		{
			printf("��Ա�Ŵ��� [%s]\n",argv[0]);
			return(ret);
		}
		strcpy(oper.id,argv[0]);
	}
	else
	{
loop1:
		if (strlen(p=UnionInput("�������Ա��[%dλ]::",sizeof(oper.id)-1)) >= sizeof(oper.id))
		{
			printf("��Ա�Ŵ���[%s]��������\n",p);
			goto loop1;
		}
		strcpy(oper.id,p);
		if (UnionIsQuit(p))
			return(0);
	}
	
	if ((ret = UnionUnlockOperatorPassword(oper.id)) < 0)
		printf("UnionUnlockOperatorPassword error! ret = [%d]\n",ret);
	else
		printf("UnionUnlockOperatorPassword OK!\n");

	return(ret);
}

int OperatorOutdate(int argc,char *argv[])
{
	int		ret;
	TUnionOperator	oper;
	char		*p;	
	
	memset(&oper,0,sizeof(oper));
	if (argc > 0)
	{
		if (strlen(argv[0]) >= sizeof(oper.id))
		{
			printf("��Ա�Ŵ��� [%s]\n",argv[0]);
			return(ret);
		}
		strcpy(oper.id,argv[0]);
	}
	else
	{
loop1:
		if (strlen(p=UnionInput("�������Ա��[%dλ]::",sizeof(oper.id)-1)) >= sizeof(oper.id))
		{
			printf("��Ա�Ŵ���[%s]��������\n",p);
			goto loop1;
		}
		strcpy(oper.id,p);
		if (UnionIsQuit(p))
			return(0);
	}
	
	printf("��Ա���뻹��%ld�����!\n",UnionGetOperatorDaysBeforePasswordOutdate(oper.id));

	return(ret);
}

int main(int argc,char **argv)
{
	int	ret;
	
	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		return(UnionHelp());
		
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
		
	if (strcasecmp(argv[1],"CREATE") == 0)
		ret = CreateOperatorTable(argc-2,&argv[2]);
	else if (strcasecmp(argv[1],"DROP") == 0)
	{
		if ((ret = UnionDeleteOperatorTable()) < 0)
			printf("UnionDeleteOperatorTable Error! ret = [%d]\n",ret);
		else
			printf("UnionDeleteOperatorTable OK!\n");
	}
	else if (strcasecmp(argv[1],"RELOAD") == 0)
	{
		if (UnionConfirm("Are you sure of reloading the OperatorTBL?"))
		{
			if ((ret = UnionReloadOperatorTable()) < 0)
				printf("UnionReloadOperatorTable Error! ret = [%d]\n",ret);
			else
				printf("UnionReloadOperatorTable OK!\n");
		}
	}
	else if ((strcasecmp(argv[1],"RELOADANYWAY") == 0) || (strcasecmp(argv[1],"-RELOADANYWAY") == 0))
	{
		if ((ret = UnionReloadOperatorTable()) < 0)
			printf("UnionReloadOperatorTable Error! ret = [%d]\n",ret);
		else
			printf("UnionReloadOperatorTable OK!\n");
	}
	else if (strcasecmp(argv[1],"ALL") == 0)
		ret = UnionPrintOperatorTableToFile(stdout);
	else if (strcasecmp(argv[1],"INSERT") == 0)
		ret = InsertOperator(argc-2,&argv[2]);
	else if (strcasecmp(argv[1],"DELETE") == 0)
		ret = DeleteOperator(argc-2,&argv[2]);
	else if (strcasecmp(argv[1],"PRINT") == 0)
		ret = PrintOperator(argc-2,&argv[2]);
	else if (strcasecmp(argv[1],"ATTR") == 0)
	{
		if ((ret = UnionPrintOperatorTableAttrToFile(stdout)) < 0)
			printf("UnionPrintOperatorTableAttrToFile Error! ret = [%d]\n",ret);
	}	
	else if (strcasecmp(argv[1],"EFFECTIVE") == 0)
		ret = UnionSetEffectiveDays(argc-1,&argv[2]);
	else if (strcasecmp(argv[1],"IDLETIME") == 0)
		ret = UnionSetIdleTime(argc-1,&argv[2]);
	else if (strcasecmp(argv[1],"NAME") == 0)
		ret = UpdateName(argc-2,&argv[2]);
	else if (strcasecmp(argv[1],"PASSWORD") == 0)
		ret = UpdatePassword(argc-2,&argv[2]);
	else if (strcasecmp(argv[1],"LEVEL") == 0)
		ret = UpdateLevel(argc-2,&argv[2]);
	else if (strcasecmp(argv[1],"LOGON") == 0)
		ret = OperatorLogon(argc-2,&argv[2]);
	else if (strcasecmp(argv[1],"LOGOFF") == 0)
		ret = OperatorLogoff(argc-2,&argv[2]);
	else if (strcasecmp(argv[1],"APPLY") == 0)
		ret = OperatorApply(argc-2,&argv[2]);
	else if (strcasecmp(argv[1],"UNLOCK") == 0)
		ret = UnlockOperator(argc-2,&argv[2]);
	else if (strcasecmp(argv[1],"OUTDATE") == 0)
		ret = OperatorOutdate(argc-2,&argv[2]);
	else
		UnionHelp();
		
	return(UnionTaskActionBeforeExit());
}


