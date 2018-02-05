// wolfang wang
// 2008/10/3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionProgramDef.h"
#include "UnionLog.h"

/*
����	
	���һ������������ģ������
�������
	nameOfProgram	��������
�������
	nameOfModule	ģ������
	version		�汾
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGetModuleNameAndVersionOfProgram(char *nameOfProgram,char *nameOfModule,char *version)
{
	int	ret;
	TUnionProgramDef	def;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadProgramDefFromDefaultFile(nameOfProgram,&def)) < 0)
	{
		UnionUserErrLog("in UnionGetModuleNameAndVersionOfProgram:: UnionReadProgramDefFromDefaultFile!\n");
		return(ret);
	}
	if (nameOfProgram != NULL)
		strcpy(nameOfModule,def.nameOfModule);
	if (version != NULL)
		strcpy(version,def.version);
	return(0);
}


/*
����	
	��һ������ڴ��ж�ȡһ��������
�������
	str		���崮
	lenOfStr	���崮�ĳ���
�������
	pdef		��������
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadProgramDefFromStr(char *str,int lenOfStr,PUnionProgramDef pdef)
{
	int	ret;
	char	typeTag[128+1];
	
	memset(pdef,0,sizeof(*pdef));

	// ����
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conProgramDefTagNameOfProgram,pdef->nameOfProgram,sizeof(pdef->nameOfProgram))) < 0)
	{
		UnionUserErrLog("in UnionReadProgramDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]!\n",conProgramDefTagNameOfProgram,str);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadProgramDefFromStr:: nameOfProgram must be defined!\n");
		return(errCodeCDPMDL_ProgramNotDefined);
	}
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conProgramDefTagNameOfModule,pdef->nameOfModule,sizeof(pdef->nameOfModule))) < 0)
	{
		UnionUserErrLog("in UnionReadProgramDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]!\n",conProgramDefTagNameOfModule,str);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadProgramDefFromStr:: nameOfProgram must be defined!\n");
		return(errCodeCDPMDL_ModuleNotDefined);
	}
	// �汾
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conProgramDefTagVersion,pdef->version,sizeof(pdef->version))) < 0)
	{
		UnionUserErrLog("in UnionReadProgramDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]!\n",conProgramDefTagVersion,str);
		return(ret);
	}
	UnionReadRecFldFromRecStr(str,lenOfStr,conProgramDefTagRemark,pdef->remark,sizeof(pdef->remark));
	return(0);
}


/* ��ָ�����ͱ�ʶ�Ķ����ӡ���ļ���
�������
	pdef	���ͱ�ʶ����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintProgramDefToFp(PUnionProgramDef pdef,FILE *fp)
{
	FILE	*outFp = stdout;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp != NULL)
		outFp = fp;
	
	fprintf(outFp,"nameOfProgram=%s|version=%s|nameOfModule,remark=%s|\n",pdef->nameOfProgram,pdef->version,pdef->nameOfModule,pdef->remark);
	return(0);
}

/* ��ָ���ļ��ж�������ͱ�ʶ��ӡ���ļ���
�������
	fileName	�ļ�����
	nameOfProgram	��������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintProgramDefInFileToFp(char *fileName,char *nameOfProgram,FILE *fp)
{
	TUnionProgramDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadProgramDefFromSpecFile(fileName,nameOfProgram,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintProgramDefInFileToFp:: UnionReadProgramDefFromSpecFile!\n");
		return(ret);
	}
	return(UnionPrintProgramDefToFp(&def,fp));
}
	
/* ��ָ���ļ��ж�������ͱ�ʶ��ӡ����Ļ��
�������
	pdef	���ͱ�ʶ����
	nameOfProgram	��������
�������
	��
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputProgramDefInFile(char *nameOfProgram,char *fileName)
{
	return(UnionPrintProgramDefInFileToFp(fileName,nameOfProgram,stdout));

}
