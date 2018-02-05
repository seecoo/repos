// wolfang wang
// 2008/10/3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionModuleDef.h"
#include "UnionLog.h"

/*
����	
	���һ��ģ��Ŀ���Ŀ¼��ͷ�ļ�����
�������
	nameOfModulde	ģ������
�������
	devDir		����Ŀ¼
	incFileConf	ͷ�ļ�����
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGetDevDirAndIncFileConfOfModule(char *nameOfModule,char *devDir,char *incFileConf)
{
	int	ret;
	TUnionModuleDef	def;
	char	fullDir[256+1];
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadModuleDefFromDefaultFile(nameOfModule,&def)) < 0)
	{
		UnionUserErrLog("in UnionGetDevDirAndIncFileConfOfModule:: UnionReadModuleDefFromDefaultFile!\n");
		return(ret);
	}
	if (devDir != NULL)
	{
		memset(fullDir,0,sizeof(fullDir));
		if ((ret = UnionReadDirFromStr(def.devDir,-1,fullDir)) < 0)
		{
			UnionUserErrLog("in UnionGetDevDirAndIncFileConfOfModule:: UnionReadDirFromStr [%s]!\n",def.devDir);
			return(ret);
		}
		//UnionLog("devDir = [%s] fullDir = [%s] ret = [%d]\n",def.devDir,fullDir,ret);
		strcpy(devDir,fullDir);
	}
	if (incFileConf != NULL)
		strcpy(incFileConf,def.incFileConf);
	return(0);
}

/*
����	
	��һ������ڴ��ж�ȡһ��ģ�鶨��
�������
	str		���崮
	lenOfStr	���崮�ĳ���
�������
	pdef		��������
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadModuleDefFromStr(char *str,int lenOfStr,PUnionModuleDef pdef)
{
	int	ret;
	char	typeTag[128+1];
	
	memset(pdef,0,sizeof(*pdef));

	// ģ��
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conModuleDefTagNameOfModule,pdef->nameOfModule,sizeof(pdef->nameOfModule))) < 0)
	{
		UnionUserErrLog("in UnionReadModuleDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]!\n",conModuleDefTagNameOfModule,str);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadModuleDefFromStr:: nameOfModule must be defined!\n");
		return(errCodeCDPMDL_ModuleNotDefined);
	}
	// ����Ŀ¼
	if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,conModuleDefTagDevDir,pdef->devDir,sizeof(pdef->devDir))) < 0)
	{
		UnionUserErrLog("in UnionReadModuleDefFromStr:: UnionReadRecFldFromRecStr [%s] from [%s]!\n",conModuleDefTagDevDir,str);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionReadModuleDefFromStr:: nameOfModule must be defined!\n");
		return(errCodeCDPMDL_ModuleDevDirNotDefined);
	}
	UnionReadRecFldFromRecStr(str,lenOfStr,conModuleDefTagIncFileConf,pdef->incFileConf,sizeof(pdef->incFileConf));
	UnionReadRecFldFromRecStr(str,lenOfStr,conModuleDefTagRemark,pdef->remark,sizeof(pdef->remark));
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
int UnionPrintModuleDefToFp(PUnionModuleDef pdef,FILE *fp)
{
	FILE	*outFp = stdout;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp != NULL)
		outFp = fp;
	
	fprintf(outFp,"nameOfModule=%s|devDir=%s|incFileConf=%s|remark=%s|\n",pdef->nameOfModule,pdef->devDir,pdef->incFileConf,pdef->remark);
	return(0);
}

/* ��ָ���ļ��ж�������ͱ�ʶ��ӡ���ļ���
�������
	fileName	�ļ�����
	nameOfModule	��������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintModuleDefInFileToFp(char *fileName,char *nameOfModule,FILE *fp)
{
	TUnionModuleDef	def;
	int		ret;
	
	memset(&def,0,sizeof(def));
	if ((ret = UnionReadModuleDefFromSpecFile(fileName,nameOfModule,&def)) < 0)
	{
		UnionUserErrLog("in UnionPrintModuleDefInFileToFp:: UnionReadModuleDefFromSpecFile!\n");
		return(ret);
	}
	return(UnionPrintModuleDefToFp(&def,fp));
}
	
/* ��ָ���ļ��ж�������ͱ�ʶ��ӡ����Ļ��
�������
	pdef	���ͱ�ʶ����
	nameOfModule	��������
�������
	��
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputModuleDefInFile(char *nameOfModule,char *fileName)
{
	return(UnionPrintModuleDefInFileToFp(fileName,nameOfModule,stdout));

}
