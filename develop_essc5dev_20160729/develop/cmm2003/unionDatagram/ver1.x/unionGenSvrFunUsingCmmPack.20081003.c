//	Wolfgang Wang
//	2008/2/25

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "unionVarValue.h"
#include "unionFunDef.h"
#include "unionGenFunTestProgram.h"
#include "unionAutoGenFileFunGrp.h"
#include "unionErrCode.h"
#include "unionDatagramDef.h"
#include "unionDatagramClassDef.h"
#include "UnionLog.h"

/*
����	
	����һ���Ա�������и�ֵ�ĺ���
�������
	funName		��������
	pdef		����ָ��
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateSvrCmmPackFldSetCodes(char *funName,PUnionDatagramDef pdef,FILE *fp)
{
	int				ret;
	FILE				*outFp=stdout;
	int				index;

	if (pdef == NULL)
		return(errCodeParameter);
		
	if (fp != NULL)
		outFp = fp;
	fprintf(outFp,"\n        // ���¶���Ӧ���Ľ��и�ֵ\n");
	fprintf(outFp,"        // ��ʼ����Ӧ����\n");
	fprintf(outFp,"        UnionInitCmmPackFldDataList(ppackRes);\n");
	for (index = 0; index < pdef->fldNum; index++)
	{
		if (pdef->fldGrp[index].isRequest)
			continue;
		fprintf(outFp,"        if ((ret = UnionPutCmmPackFldIntoFldDataList(%s,strlen(%s),%s,ppackRes)) < 0)\n",pdef->fldGrp[index].fldID,pdef->fldGrp[index].alais,pdef->fldGrp[index].alais);
		fprintf(outFp,"        {\n");
		fprintf(outFp,"                UnionUserErrLog(\"in %s:: UnionPutCmmPackFldIntoFldDataList %s! ret = [%%d]\\n\",ret);\n",funName,pdef->fldGrp[index].fldID);
		fprintf(outFp,"                return(ret);\n");
		fprintf(outFp,"        }\n");
	}
	return(0);
}

/*
����	
	����һ���ӱ������ֵ����ĺ���
�������
	funName		��������
	pdef		����ָ��
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateSvrCmmPackFldReadCodes(char *funName,PUnionDatagramDef pdef,FILE *fp)
{
	int				ret;
	FILE				*outFp=stdout;
	int				index;
	int				isOptional;
	
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp != NULL)
		outFp = fp;
	fprintf(outFp,"\n        // ���´ӱ������ֵ\n");
	for (index = 0; index < pdef->fldNum; index++)
	{
		if (!pdef->fldGrp[index].isRequest)	// ����������
			continue;
		fprintf(outFp,"        if ((ret = UnionReadCmmPackFldFromFldDataList(ppackReq,%s,sizeof(%s),%s)) < 0)\n",pdef->fldGrp[index].fldID,pdef->fldGrp[index].alais,pdef->fldGrp[index].alais);
continueWrite:
		fprintf(outFp,"        {\n");
		fprintf(outFp,"                if (!(isOptional = %d))\n",pdef->fldGrp[index].optional);
		fprintf(outFp,"                {       // ��ѡ�򱨴�\n"); 
		fprintf(outFp,"                        UnionUserErrLog(\"in %s:: UnionReadCmmPackFldFromFldDataList %s! ret = [%%d]\\n\",ret);\n",funName,pdef->fldGrp[index].fldID);
		fprintf(outFp,"                        return(ret);\n");
		fprintf(outFp,"                }\n");
		fprintf(outFp,"        }\n");
	}
	return(0);
}

/*
����	
	������������
�������
	pdef		����ָ��
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateSvrCmmPackVarDeclarionCodes(PUnionDatagramDef pdef,FILE *fp)
{
	FILE	*outFp = stdout;
	int	index;
		
	if (pdef == NULL)
		return(errCodeParameter);
	if (fp != NULL)
		outFp = fp;
	fprintf(outFp,"{\n");
	for (index = 0; index < pdef->fldNum; index++)
	{
		fprintf(outFp,"        char        %s[%d+1];\n",pdef->fldGrp[index].alais,pdef->fldGrp[index].len);
	}
	fprintf(outFp,"        int         ret;\n");
	fprintf(outFp,"        int         isOptional;\n");
	fprintf(outFp,"\n");
	fprintf(outFp,"        // ��ʼ������\n");
	for (index = 0; index < pdef->fldNum; index++)
	{
		fprintf(outFp,"        memset(%s,0,sizeof(%s));\n",pdef->fldGrp[index].alais,pdef->fldGrp[index].alais);
	}
	return(0);
}

/*
����	
	�����������ͨѶ�Ĵ���
�������
	fp		�ļ����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateNullCmmPackFunCodes(FILE *fp)
{
	FILE	*outFp = stdout;
	int	ret;
	
	if (fp != NULL)
		outFp = fp;

	fprintf(outFp,"\n        // �ڴ˴����ӹ��ܴ���\n");
	fprintf(outFp,"\n");
	fprintf(outFp,"        // ���ܴ������ӽ���\n\n");
	return(0);
}	

/*
����	
	ƴװ��������
�������
	appID		Ӧ����;
	datagramID	���ı�ʶ
�������
	funName		��������
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateSvrCmmPackFunNameOfService(char *appID,char *datagramID,char *funName)
{
	TUnionDatagramDef	def;
	int			ret;
		
	// ����������
	memset(&def,0,sizeof(&def));
	if ((ret = UnionReadSpecDatagramDefFromFile(appID,datagramID,&def)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSvrCmmPackFunNameOfService:: UnionReadSpecDatagramDefFromFile [%s][%s]!\n",appID,datagramID);
		return(ret);
	}
	sprintf(funName,"UnionService%s",def.serviceID);
	return(0);
}

/*
����	
	ƴװ��������
�������
	appID		Ӧ����;
	datagramID	���ı�ʶ
	version		����汾
�������
	programName	����ȫ������
	incFileConf	����ͷ�ļ����ļ�����
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateSvrCmmPackProgramNameOfService(char *appID,char *datagramID,char *version,char *incFileConf,char *programName)
{
	char	devDir[256+1];
	int	ret;
	char	nameOfModuleOfSvr[64+1];
	char	funName[128+1];
	
	memset(nameOfModuleOfSvr,0,sizeof(nameOfModuleOfSvr));
	if ((ret = UnionGetNameOfModuleSvrOfSpecDatagramClass(appID,nameOfModuleOfSvr)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSvrCmmPackProgramNameOfService:: UnionGetNameOfModuleSvrOfSpecDatagramClass!\n");
		return(ret);
	}
	memset(devDir,0,sizeof(devDir));
	if ((ret = UnionGetDevDirAndIncFileConfOfModule(nameOfModuleOfSvr,devDir,incFileConf)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSvrCmmPackProgramNameOfService:: UnionGetDevDirAndIncFileConfOfModule [%s]!\n",nameOfModuleOfSvr);
		return(ret);
	}
	memset(funName,0,sizeof(funName));
	if ((ret = UnionGenerateSvrCmmPackFunNameOfService(appID,datagramID,funName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSvrCmmPackProgramNameOfService:: UnionGenerateSvrCmmPackFunNameOfService!\n");
		return(ret);
	}
	sprintf(programName,"%s/%s.%s.c",devDir,funName,version);
	return(0);
}

/*
����	
	����һ������˺���
�������
	appID		Ӧ����;
	datagramID	���ı�ʶ
	incConfFileName	���ɵĺ���Ҫʹ�õ�ͷ�ļ�
	fp		�ļ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateSvrFunUsingCmmPackToFp(char *appID,char *datagramID,char *incConfFileName,FILE *fp)
{
	int			ret;
	FILE			*outFp = stdout;
	TUnionDatagramDef	def;
	int			index;
	char			funName[128+1];
	
	// ����������
	memset(&def,0,sizeof(&def));
	if ((ret = UnionReadSpecDatagramDefFromFile(appID,datagramID,&def)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSvrFunUsingCmmPackToFp:: UnionReadSpecDatagramDefFromFile [%s][%s]!\n",appID,datagramID);
		return(ret);
	}
	if (fp != NULL)
		outFp = fp;

	// д������ͷ�ļ���Ϣ
	if ((ret = UnionAddIncludeFileToFile(incConfFileName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSvrFunUsingCmmPackToFp:: UnionAddIncludeFileToFile!\n");
		goto errExit;
	}
	// ��������ļ�
	fprintf(outFp,"#ifndef _unionCmmPackData_\n");
	fprintf(outFp,"#include \"unionCmmPackData.h\"\n");
	fprintf(outFp,"#endif\n");
	fprintf(outFp,"\n");
	
	// ƴװ��������
	memset(funName,0,sizeof(funName));
	if ((ret = UnionGenerateSvrCmmPackFunNameOfService(appID,datagramID,funName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSvrFunUsingCmmPackToFp:: UnionGenerateSvrCmmPackFunNameOfService [%s]!\n",funName);
		goto errExit;
	}
	// ����������д�뵽������
	fprintf(outFp,"int %s(PUnionCmmPackData ppackReq,PUnionCmmPackData ppackRes)\n",funName);

	// ����������������
	if ((ret = UnionGenerateSvrCmmPackVarDeclarionCodes(&def,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSvrFunUsingCmmPackToFp:: UnionGenerateSvrCmmPackVarDeclarionCodes [%s]!\n",funName);
		goto errExit;
	}

	// �����ӱ��Ķ�ֵ�Ĵ���
	if ((ret = UnionGenerateSvrCmmPackFldReadCodes(funName,&def,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSvrFunUsingCmmPackToFp:: UnionGenerateSvrCmmPackFldReadCodes [%s]!\n",funName);
		goto errExit;
	}
	
	// �������ܴ���
	if ((ret = UnionGenerateNullCmmPackFunCodes(fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSvrFunUsingCmmPackToFp:: UnionGenerateNullCmmPackFunCodes [%s]!\n",funName);
		goto errExit;
	}

	// �������ĸ�ֵ����
	if ((ret = UnionGenerateSvrCmmPackFldSetCodes(funName,&def,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSvrFunUsingCmmPackToFp:: UnionGenerateSvrCmmPackFldSetCodes [%s]!\n",funName);
		goto errExit;
	}


	// ������β����
	fprintf(outFp,"\n");
	fprintf(outFp,"        return(ret);\n");
	fprintf(outFp,"}\n\n");
errExit:
	if (fp != stdout)
		fclose(fp);
	return(ret);
}

/*
����	
	����һ������˺���
�������
	appID		Ӧ����;
	datagramID	���ı�ʶ
	version		����汾
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateSvrFunUsingCmmPack(char *appID,char *datagramID,char *version)
{
	char	fileName[256+1];
	char	incFileConf[256+1];
	int	ret;
	FILE	*fp;
	
	memset(fileName,0,sizeof(fileName));
	memset(incFileConf,0,sizeof(incFileConf));
	if ((ret = UnionGenerateSvrCmmPackProgramNameOfService(appID,datagramID,version,incFileConf,fileName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSvrFunUsingCmmPack:: UnionGenerateSvrCmmPackProgramNameOfService!\n");
		return(ret);
	}
	if ((fp = fopen(fileName,"w")) == 0)
	{
		UnionSystemErrLog("in UnionGenerateSvrFunUsingCmmPack:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	if ((ret = UnionGenerateSvrFunUsingCmmPackToFp(appID,datagramID,incFileConf,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSvrFunUsingCmmPack:: UnionGenerateSvrFunUsingCmmPackToFp [%s][%s]!\n",appID,datagramID);
		fclose(fp);
		return(ret);
	}
	fclose(fp);
	return(0);
}

