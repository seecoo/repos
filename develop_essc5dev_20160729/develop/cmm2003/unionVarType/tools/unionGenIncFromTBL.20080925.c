//	Wolfgang Wang
//	2009/4/29

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "unionComplexDBCommon.h"
#include "unionComplexDBCommon.h"
#include "unionErrCode.h"
#include "UnionStr.h"
#include "mngSvrCommProtocol.h"

#include "unionGenIncFromTBL.h"
#include "unionTableDesignDoc.h"
#include "UnionLog.h"

int UnionReadAllVersionNoOfSpecTBLDef(char *tblName,char versionGrp[][64+1],int maxNum)
{
	int	num;
	TUnionTableDesignDoc	recGrp[64];
	int	index;
	char	condition[128];
	
	sprintf(condition,"tableName=%s|",tblName);
	
	if ((num = UnionBatchReadTableDesignDocRec(condition,recGrp,64)) < 0)
	{
		UnionUserErrLog("in UnionReadAllVersionNoOfSpecTBLDef:: UnionBatchReadTableDesignDocRec!\n");
		return(num);
	}
	for (index = 0; (index < num) && (index < maxNum); index++)
		strcpy(versionGrp[index],recGrp[index].version);
		
	//strcpy(versionGrp[0],"2.0");
	//strcpy(versionGrp[1],"2.2.1");
	return(index);
}

/*
����	
	���ݱ�������ñ������
�������
	tblName		����
�������
	pobjDef		�������
����ֵ
	��
*/
int UnionReadObjectDefFromDesignDoc(char *tblName,PUnionObject pobjDef)
{
	int			ret;
	//TUnionObject		objDef;
	int			useTableList = 1;
	char			version[128+1];
	char			tmpBuf[128+1];
		
	if (tblName == NULL)
		return(errCodeParameter);
	
	memset(pobjDef,0,sizeof(*pobjDef));
	if ((ret = UnionReadIntTypeFldFromCurrentMngSvrClientReqStr("useTableDefRec",&useTableList)) < 0)
		useTableList = 0;
	if (!useTableList)
	{
		if ((ret = UnionReadObjectDef(tblName,pobjDef)) < 0)
		{
			UnionUserErrLog("in UnionReadObjectDefFromDesignDoc:: UnionReadObjectDef [%s]!\n",tblName);
			return(ret);
		}
		UnionLog("in UnionReadObjectDefFromDesignDoc:: UnionReadObjectDef OK!\n");
	}
	else
	{
		if ((ret = UnionGenerateObjectDefFromTBLDefInTableList(tblName,pobjDef)) < 0)
		{
			UnionUserErrLog("in UnionReadObjectDefFromDesignDoc:: UnionGenerateObjectDefFromTBLDefInTableList [%s]!\n",tblName);
			return(ret);
		}
		UnionLog("in UnionReadObjectDefFromDesignDoc:: UnionGenerateObjectDefFromTBLDefInTableList OK!\n");
	}
	return(ret);
}

/*
����	
	���ݱ�������ñ�ṹ����ͷ�ļ�ȫ������·��)
�������
	tblName		����
�������
	funName		ͷ�ļ���
����ֵ
	��
*/
void UnionFormFullTableDefStructHeadFileName(char *tblName,char *incFileName)
{
	char	programName[512+1];
	
	UnionFormTableDefStructHeadFileName(tblName,programName);
	sprintf(incFileName,"%s/%s",getenv("UNIONDATAINCDIR"),programName);
	return;
}


/*
����	
	���ݱ�������ñ��ͷ�ļ���
�������
	tblName		����
�������
	funName		ͷ�ļ���
����ֵ
	��
*/
void UnionFormTableDefStructHeadFileName(char *tblName,char *incFileName)
{
	char	programName[128+1];
	
	UnionFormStructTagNameOfCurrentVersionTBLDefFromTBLName(tblName,programName);
	sprintf(incFileName,"%s.h",programName);
	return;
}


/*
����	
	���ݱ�������õ�ǰ�ṹ�汾��ʶ��
�������
	tblName		����
	version		�汾
�������
	funName		ͷ�ļ���
����ֵ
	��
*/
void UnionFormStructTagNameOfCurrentVersionTBLDefFromTBLName(char *tblName,char *incFileName)
{
	char	programName[128+1];
	char	version[128];
	int	len;
	
	memset(version,0,sizeof(version));
	if ((len = UnionReadTableListRecVersionOfSpecTBLName(tblName,version,sizeof(version))) <= 0)
		strcpy(version,getenv("UNIONCURRENTSRCVERSION"));
	UnionFormStructTagNameOfSpecVersionTBLDefFromTBLName(tblName,version,incFileName);
	return;
}

/*
����	
	���ݱ�������ýṹ�汾��ʶ��
�������
	tblName		����
	version		�汾
�������
	funName		ͷ�ļ���
����ֵ
	��
*/
void UnionFormStructTagNameOfSpecVersionTBLDefFromTBLName(char *tblName,char *version,char *incFileName)
{
	char	programName[128+1];
	char	verStr[128+1];
	
	memset(verStr,0,sizeof(verStr));
	UnionConvertOneFldSeperatorInRecStrIntoAnother(version,strlen(version),'.','_',verStr,sizeof(verStr));
	UnionFormTBLProgramNameFromTBLName(tblName,programName);
	sprintf(incFileName,"%s_struct_%s",programName,verStr);
	return;
}


/*
����	
	���ݱ�������ó�����
�������
	tblName		����
�������
	funName		������
����ֵ
	��
*/
void UnionFormTBLProgramNameFromTBLName(char *tblName,char *programName)
{
	char	structName[128+1];
	
	UnionFormStructNameFromTBLName(tblName,structName);
	sprintf(programName,"union%s",structName);
	return;
}

/*
����	
	���ݱ��������ͷ�ļ���
�������
	tblName		����
�������
	funName		ͷ�ļ���
����ֵ
	��
*/
void UnionFormIncFileNameOfTBLProgramFromTBLName(char *tblName,char *incFileName)
{
	char	programName[128+1];
	
	UnionFormTBLProgramNameFromTBLName(tblName,programName);
	sprintf(incFileName,"%s.h",programName);
	return;
}

/*
����	
	���ݱ��������ͷ�ļ���(����·��)
�������
	tblName		����
�������
	funName		ͷ�ļ���
����ֵ
	��
*/
void UnionFormFullIncFileNameOfTBLProgramFromTBLName(char *tblName,char *incFileName)
{
	char	programName[128+1];
	
	UnionFormIncFileNameOfTBLProgramFromTBLName(tblName,programName);
	sprintf(incFileName,"%s/%s",getenv("UNIONDATAINCDIR"),programName);
	return;
}

/*
����	
	���ݱ��������ͷ�ļ���(����·��)
�������
	tblName		����
�������
	funName		ͷ�ļ���
����ֵ
	��
*/
void UnionFormFullFileNameOfTBLProgramFromTBLName(char *tblName,char *incFileName)
{
	char	programName[128+1];
	
	UnionFormIncFileNameOfTBLProgramFromTBLName(tblName,programName);
	sprintf(incFileName,"%s/%s",getenv("UNIONDATAINCDIR"),programName);
	return;
}

/*
����	
	���ݱ��������C�ļ���(����·���Ͱ汾)
�������
	tblName		����
�������
	funName		c�ļ���
����ֵ
	��
*/
void UnionFormFullCFileNameOfTBLProgramFromTBLName(char *tblName,char *cFileName)
{
	char	programName[128+1];
	char	dir[256];
	char	version[128];
	int	len;
	
	memset(version,0,sizeof(version));
	if ((len = UnionReadTableListRecVersionOfSpecTBLName(tblName,version,sizeof(version))) <= 0)
		strcpy(version,getenv("UNIONCURRENTSRCVERSION"));
	
	UnionFormTBLProgramNameFromTBLName(tblName,programName);
	sprintf(cFileName,"%s/%s.%s.c",getenv("UNIONDATASRCDIR"),programName,version);
	return;
}

/*
����	
	���ݱ�������ñ�����Ӧ�ĺ궨������
�������
	tblName		����
�������
	funName		�궨����
����ֵ
	��
*/
void UnionFormTBLNameConstNameFromTBLName(char *tblName,char *constName)
{
	char	structName[128+1];
	
	UnionFormStructNameFromTBLName(tblName,structName);
	sprintf(constName,"conTBLName%s",structName);
	return;
}

/*
����	
	���ݱ�������ñ���Դ��Ӧ�ĺ궨������
�������
	tblName		����
�������
	funName		�궨����
����ֵ
	��
*/
void UnionFormTBLResIDConstNameFromTBLName(char *tblName,char *constName)
{
	char	structName[128+1];
	
	UnionFormStructNameFromTBLName(tblName,structName);
	sprintf(constName,"conResID%s",structName);
	return;
}

/*
����	
	���ݱ�����ƴװһ������¼�ṹת��Ϊ��¼�ִ��ĺ�������
�������
	tblName		����
�������
	funName		��������
����ֵ
	��
*/
void UnionFormConvertStructIntoRecStrFunNameFromTBLName(char *tblName,char *funName)
{
	char	structName[128+1];
	
	UnionFormStructNameFromTBLName(tblName,structName);
	sprintf(funName,"UnionFormRecStrFrom%sRec",structName);
	return;
}

/*
����	
	���ݱ�����ƴװһ������¼�ṹ�еĹؼ���ת��Ϊ��¼�ִ��ĺ�������
�������
	tblName		����
�������
	funName		��������
����ֵ
	��
*/
void UnionFormConvertPrimaryKeyInStructIntoRecStrFunNameFromTBLName(char *tblName,char *funName)
{
	char	structName[128+1];
	
	UnionFormStructNameFromTBLName(tblName,structName);
	sprintf(funName,"UnionFormPrimaryKeyRecStrFrom%sRec",structName);
	return;
}

/*
����	
	���ݱ�����ƴװһ������¼�ṹ�еķǹؼ���ת��Ϊ��¼�ִ��ĺ�������
�������
	tblName		����
�������
	funName		��������
����ֵ
	��
*/
void UnionFormConvertNonPrimaryKeyInStructIntoRecStrFunNameFromTBLName(char *tblName,char *funName)
{
	char	structName[128+1];
	
	UnionFormStructNameFromTBLName(tblName,structName);
	sprintf(funName,"UnionFormNonPrimaryKeyRecStrFrom%sRec",structName);
	return;
}

/*
����	
	���ݱ�����ƴװһ�������¼�ĺ�������
�������
	tblName		����
�������
	funName		��������
����ֵ
	��
*/
void UnionFormInsertRecFunNameFromTBLName(char *tblName,char *funName)
{
	char	structName[128+1];
	
	UnionFormStructNameFromTBLName(tblName,structName);
	sprintf(funName,"UnionInsert%sRec",structName);
	return;
}

/*
����	
	���ݱ�����ƴװһ����ȡ��¼�ĺ�������
�������
	tblName		����
�������
	funName		��������
����ֵ
	��
*/
void UnionFormReadRecFunNameFromTBLName(char *tblName,char *funName)
{
	char	structName[128+1];
	
	UnionFormStructNameFromTBLName(tblName,structName);
	sprintf(funName,"UnionRead%sRec",structName);
	return;
}

/*
����	
	���ݱ�����ƴװһ����ȡ��¼����ĺ�������
�������
	tblName		����
�������
	funName		��������
����ֵ
	��
*/
void UnionFormReadRecFldFunNameFromTBLName(char *tblName,char *funName)
{
	char	structName[128+1];
	
	UnionFormStructNameFromTBLName(tblName,structName);
	sprintf(funName,"UnionRead%sRecFld",structName);
	return;
}

/*
����	
	���ݱ�����ƴװһ����ȡ��¼����ĺ�������
�������
	tblName		����
�������
	funName		��������
����ֵ
	��
*/
void UnionFormReadRecIntTypeFldFunNameFromTBLName(char *tblName,char *funName)
{
	char	structName[128+1];
	
	UnionFormStructNameFromTBLName(tblName,structName);
	sprintf(funName,"UnionRead%sRecIntTypeFld",structName);
	return;
}

/*
����	
	���ݱ�����ƴװһ���޸ļ�¼�ĺ�������
�������
	tblName		����
�������
	funName		��������
����ֵ
	��
*/
void UnionFormUpdateRecFunNameFromTBLName(char *tblName,char *funName)
{
	char	structName[128+1];
	
	UnionFormStructNameFromTBLName(tblName,structName);
	sprintf(funName,"UnionUpdate%sRec",structName);
	return;
}

/*
����	
	���ݱ�����ƴװһ��ɾ����¼�ĺ�������
�������
	tblName		����
�������
	funName		��������
����ֵ
	��
*/
void UnionFormDeleteRecFunNameFromTBLName(char *tblName,char *funName)
{
	char	structName[128+1];
	
	UnionFormStructNameFromTBLName(tblName,structName);
	sprintf(funName,"UnionDelete%sRec",structName);
	return;
}

/*
����	
	���ݱ�����ƴװһ���޸ļ�¼ָ����ĺ�������
�������
	tblName		����
�������
	funName		��������
����ֵ
	��
*/
void UnionFormUpdateSpecFldRecFunNameFromTBLName(char *tblName,char *funName)
{
	char	structName[128+1];
	
	UnionFormStructNameFromTBLName(tblName,structName);
	sprintf(funName,"UnionUpdateSpecFldOf%sRec",structName);
	return;
}

/*
����	
	���ݱ�����ƴװһ���޸ļ�¼ָ��������ĺ�������
�������
	tblName		����
�������
	funName		��������
����ֵ
	��
*/
void UnionFormUpdateIntTypeSpecFldRecFunNameFromTBLName(char *tblName,char *funName)
{
	char	structName[128+1];
	
	UnionFormStructNameFromTBLName(tblName,structName);
	sprintf(funName,"UnionUpdateIntTypeSpecFldOf%sRec",structName);
	return;
}

/*
����	
	���ݱ�����ƴװһ������¼�ִ�ת��Ϊ��¼�ṹ�ĺ�������
�������
	tblName		����
�������
	funName		��������
����ֵ
	��
*/
void UnionFormConvertRecStrIntoStructFunNameFromTBLName(char *tblName,char *funName)
{
	char	structName[128+1];
	
	UnionFormStructNameFromTBLName(tblName,structName);
	sprintf(funName,"UnionForm%sRecFromRecStr",structName);
	return;
}

/*
����	
	���ݱ�����ƴװһ������¼�ִ�ת��Ϊ��¼�ṹ�Ĺؼ��ֵĺ�������
�������
	tblName		����
�������
	funName		��������
����ֵ
	��
*/
void UnionFormConvertRecStrIntoStructPrimaryKeyFunNameFromTBLName(char *tblName,char *funName)
{
	char	structName[128+1];
	
	UnionFormStructNameFromTBLName(tblName,structName);
	sprintf(funName,"UnionFormPrimaryKeyOf%sRecFromRecStr",structName);
	return;
}

/*
����	
	���ݱ�����ƴװһ�����ȱʡ��ֵ��������
�������
	tblName		����
�������
	funName		��������
����ֵ
	��
*/
void UnionFormStructDefaultValueSetFunNameFromTBLName(char *tblName,char *funName)
{
	char	structName[128+1];
	
	UnionFormStructNameFromTBLName(tblName,structName);
	sprintf(funName,"UnionFormDefaultValueOf%sRec",structName);
	return;
}

/*
����	
	���ݱ�����ƴװһ�����������ȡ���ݼ�¼��������
�������
	tblName		����
�������
	funName		��������
����ֵ
	��
*/
void UnionFormBatchReadRecFunNameFromTBLName(char *tblName,char *funName)
{
	char	structName[128+1];
	
	UnionFormStructNameFromTBLName(tblName,structName);
	sprintf(funName,"UnionBatchRead%sRec",structName);
	return;
}


/*
����	
	���ݱ�����ƴװһ����������ƺ궨����
�������
	tblName		����
	fldName		����
�������
	fldDefName	�����ƺ궨����
����ֵ
	��
*/
void UnionFormStructFldDefNameFromTBLName(char *tblName,char *fldName,char *fldDefName)
{
	char	structName[128+1];
	
	UnionFormStructNameFromTBLName(tblName,structName);
	sprintf(fldDefName,"con%sFldName%c%s",structName,toupper(fldName[0]),&(fldName[1]));
	return;
}

/*
����	
	���ݱ������ͺ궨������ƴװһ���궨����
�������
	tblName		����
	macroTag	���ʶ
�������
	macroName	������
����ֵ
	��
*/
void UnionFormMacroNameFromTBLName(char *tblName,char *macroTag,char *macroName)
{
	sprintf(macroName,"con%c%s%c%s",toupper(tblName[0]),&(tblName[1]),toupper(macroTag[0]),&(macroTag[1]));
	return;
}


/*
����	
	���ݱ�����ƴװһ���ṹ����
�������
	tblName		����
�������
	structName	�ṹ����
����ֵ
	��
*/
void UnionFormStructNameFromTBLName(char *tblName,char *structName)
{
	sprintf(structName,"%c%s",toupper(tblName[0]),&(tblName[1]));
	return;
}

/*
����	
	���ݱ�����ƴװһ���ṹ����
�������
	tblName		����
	isPointer	�Ƿ���Ϊָ�룬1�ǣ�0��
�������
	structDefName	�ṹ����
����ֵ
	��
*/
void UnionFormStructDefNameFromTBLName(char *tblName,int isPointer,char *structDefName)
{
	sprintf(structDefName,"TUnion%c%s",toupper(tblName[0]),&(tblName[1]));
	if (isPointer)
		structDefName[0] = 'P';
	return;
}

/*
����	
	���ݱ��壬����һ�����ݽṹ����
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionGenerateStructDefFromTBLDef(char *tblName,FILE *fp)
{
	FILE			*outFp = stdout;
	int			ret;
	TUnionObject		objDef;
	int			fldNum;
	PUnionObjectFldDef	pfldDef;
	char			fldDefName[128+1];
	char			structDefName[128+1];
	char			structPtrDefName[128+1];
	
	if (tblName == NULL)
		return(errCodeParameter);
		
	if (fp != NULL)
		outFp = fp;
	
	// ������
	memset(&objDef,0,sizeof(objDef));
	if ((ret = UnionReadObjectDefFromDesignDoc(tblName,&objDef)) < 0)
	{
		UnionUserErrLog("in UnionGenerateStructDefFromTBLDef:: UnionReadObjectDefFromDesignDoc [%s]!\n",tblName);
		return(ret);
	}
	// ��������������
	fprintf(outFp,"\n// ��������������\n"); 
	for (fldNum = 0; fldNum < objDef.fldNum; fldNum++)
	{
		pfldDef = &(objDef.fldDefGrp[fldNum]);
		UnionFormStructFldDefNameFromTBLName(objDef.name,pfldDef->name,fldDefName);
		fprintf(outFp,"#define %s		\"%s\"\n",fldDefName,pfldDef->name);
	}
	// ������¼��Ӧ�Ľṹ
	// �ṹ����	
	memset(structDefName,0,sizeof(structDefName));
	UnionFormStructDefNameFromTBLName(objDef.name,0,structDefName);
	fprintf(outFp,"\n");
	fprintf(outFp,"/*\n");
	fprintf(outFp,"*** �ṹ��Ӧ�ı���Ϣ ***\n");
	fprintf(outFp,"����::          %s\n",tblName);
	fprintf(outFp,"�����궨��::    conTBLName%c%s\n",toupper(tblName[0]),tblName+1);
	fprintf(outFp,"����Դ�궨��::  conResID%c%s\n",toupper(tblName[0]),tblName+1);
	fprintf(outFp,"�ؼ���::        ");
	for (fldNum = 0; fldNum < objDef.primaryKey.fldNum; fldNum++)
	{
		if (fldNum >= 1)
			fprintf(outFp,",");
		fprintf(outFp,"%s",objDef.primaryKey.fldNameGrp[fldNum]);
	}
	fprintf(outFp,"\n");
	fprintf(outFp,"*/\n");
	fprintf(outFp,"\n// ������¼��Ӧ�Ľṹ\n"); 
	fprintf(outFp,"typedef struct\n");
	fprintf(outFp,"{\n");
	for (fldNum = 0; fldNum < objDef.fldNum; fldNum++)
	{
		pfldDef = &(objDef.fldDefGrp[fldNum]);
		// �������
		switch (pfldDef->type)
		{
			case	conObjectFldType_String:
				fprintf(outFp,"	char	");
				break;
			case	conObjectFldType_Int:
				fprintf(outFp,"	int	");
				break;
			case	conObjectFldType_Double:
				fprintf(outFp,"	double	");
				break;
			case	conObjectFldType_Bit:
				fprintf(outFp,"	unsigned char	");
				break;
			case	conObjectFldType_Bool:
				fprintf(outFp,"	int	");
				break;
			case	conObjectFldType_Long:
				fprintf(outFp,"	long	");
				break;
			default:
				fprintf(outFp,"	void	");
				break;
		}
		// ����
		fprintf(outFp,"%s",pfldDef->name);
		// ��С
		if (pfldDef->size > 0)
		{
			switch (pfldDef->type)
			{
				case	conObjectFldType_String:
					if (pfldDef->size > 1)
						fprintf(outFp,"[%d+1]",pfldDef->size);
					break;
				case	conObjectFldType_Bit:
					fprintf(outFp,"[%d+1]",pfldDef->size);
					break;
				default:
					break;
			}
		}
		fprintf(outFp,";");
		if (strlen(pfldDef->remark) > 0)
			fprintf(outFp,"	// %s",pfldDef->remark);
		fprintf(outFp,"\n");
	}
	fprintf(outFp,"} %s;\n",structDefName);
	memset(structPtrDefName,0,sizeof(structPtrDefName));
	UnionFormStructDefNameFromTBLName(objDef.name,1,structPtrDefName);
	fprintf(outFp,"typedef %s	*%s;\n",structDefName,structPtrDefName);
	return(objDef.fldNum);
}

/*
����	
	���ݱ��壬�ж��Ƿ�ҪΪһ������궨��
�������
	tblName		����
�������
	��
����ֵ
	1	��Ҫ
	0	����Ҫ
	<0	������
*/
int UnionExistsMacroDefInTBL(char *tblName)
{
	int			ret;
	TUnionObject		objDef;
	int			fldNum;
	PUnionObjectFldDef	pfldDef;
	int			macroNameExists = 0,macroValueExists = 0;
	
	if (tblName == NULL)
		return(errCodeParameter);
		
	// ������
	memset(&objDef,0,sizeof(objDef));
	if ((ret = UnionReadObjectDefFromDesignDoc(tblName,&objDef)) < 0)
	{
		UnionUserErrLog("in UnionExistsMacroDefInTBL:: UnionReadObjectDefFromDesignDoc [%s]!\n",tblName);
		return(ret);
	}
	for (fldNum = 0; fldNum < objDef.fldNum; fldNum++)
	{
		pfldDef = &(objDef.fldDefGrp[fldNum]);
		if (strcmp(pfldDef->name,"macroName") == 0)
			macroNameExists = 1;
		else if (strcmp(pfldDef->name,"macroValue") == 0)
			macroValueExists = 1;
		if (macroNameExists && macroValueExists)
			return(1);
	}
	return(0);
}

/*
����	
	���ݱ��еļ�¼������꣬һ����¼��Ӧһ���궨��
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionGenerateConstDefFromTBL(char *tblName,FILE *fp)
{
	FILE			*outFp = stdout;
	int			ret;
	char			fileName[512+1];
	int			recNum = 0;
	FILE			*recFileFp = NULL;
	char			recStr[8192*2+1];
	int			lenOfRecStr;
	char			remark[512+1];
	char			macroName[128+1];
	char			macroTag[128+1];
	char			macroValue[128+1];
	TUnionObject		objDef;
	int			fldNum;
	PUnionObjectFldDef	pfldDef;
	char			funName[128+1];
	char			ID[40+1];

	if ((ret = UnionExistsMacroDefInTBL(tblName)) <= 0)
		return(ret);
		
	if (fp != NULL)
		outFp = fp;

	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/pid-%d.tmp",getenv("UNIONTEMP"),getpid());

	// �����м�¼
	if ((ret = UnionBatchSelectObjectRecord(tblName,"",fileName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateConstDefFromTBL:: UnionBatchSelectObjectRecord [%s]!\n",tblName);
		return(ret);
	}
	
	// ���ļ�
	if ((recFileFp = fopen(fileName,"r")) == NULL)
	{
		UnionUserErrLog("in UnionGenerateConstDefFromTBL:: fopen [%s]\n",fileName);
		goto errExit;
	}
	while (!feof(recFileFp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(recFileFp,recStr,sizeof(recStr))) <= 0)
			continue;
		memset(remark,0,sizeof(remark));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"remark",remark,sizeof(remark))) < 0)
		{
			UnionUserErrLog("in UnionGenerateConstDefFromTBL:: UnionReadRecFldFromRecStr [%s] from [%s]\n","remark",recStr);
			goto errExit;
		}
		memset(ID,0,sizeof(ID));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"ID",ID,sizeof(ID))) < 0)
		{
			UnionUserErrLog("in UnionGenerateConstDefFromTBL:: UnionReadRecFldFromRecStr [%s] from [%s]\n","ID",recStr);
			goto errExit;
		}
		memset(macroTag,0,sizeof(macroTag));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"macroName",macroTag,sizeof(macroTag))) < 0)
		{
			UnionUserErrLog("in UnionGenerateConstDefFromTBL:: UnionReadRecFldFromRecStr [%s] from [%s]\n","macroName",recStr);
			goto errExit;
		}
		memset(macroName,0,sizeof(macroName));
		UnionFormMacroNameFromTBLName(tblName,macroTag,macroName);
		if (ret == 0)
		{
			UnionUserErrLog("in UnionGenerateConstDefFromTBL:: macroName is null in [%s]\n",recStr);
			ret = errCodeMarcoDefNameNotDefined;
			goto errExit;
		}
		memset(macroValue,0,sizeof(macroValue));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"macroValue",macroValue,sizeof(macroValue))) < 0)
		{
			UnionUserErrLog("in UnionGenerateConstDefFromTBL:: UnionReadRecFldFromRecStr [%s] from [%s]\n","macroValue",recStr);
			goto errExit;
		}
		if (ret == 0)
		{
			UnionUserErrLog("in UnionGenerateConstDefFromTBL:: macroName is null in [%s]\n",recStr);
			ret = errCodeMarcoDefValueNotDefined;
			goto errExit;
		}
		if (strlen(remark) != 0)
			fprintf(outFp,"// %s\n",remark);
		else
			fprintf(outFp,"// %s\n",ID);
		if (UnionIsDecimalStr(macroValue))
			fprintf(outFp,"#define %s	%s\n",macroName,macroValue);
		else
		{
			if (strlen(macroValue) == 1)
				fprintf(outFp,"#define %s	'%s'\n",macroName,macroValue);
			else
				//fprintf(outFp,"#define %s	\"%s\"\n",macroName,macroValue);
				fprintf(outFp,"#define %s	(%s)\n",macroName,macroValue);
		}
		recNum++;
	}
	ret = recNum;
	if (recNum <= 0)
		UnionLog("in UnionGenerateConstDefFromTBL:: **** no const defined! for table [%s]!\n",tblName);
errExit:
	fclose(recFileFp);
	UnionDeleteFile(fileName);
	return(ret);
}


/*
����	
	���ݱ��壬����һ�����ݽṹȱʡ��ֵ����������
�������
	tblName		����
	includeFunBody	�Ƿ���������壬1,������0������
	fp		�ļ����
�������
	funName		�����ĺ�������
����ֵ
	��
*/
void UnionGenerateStructDefaultValueSetFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp)
{
	char	structPtrDefName[128+1];
	
	memset(structPtrDefName,0,sizeof(structPtrDefName));
	UnionFormStructDefNameFromTBLName(tblName,1,structPtrDefName);
	UnionFormStructDefaultValueSetFunNameFromTBLName(tblName,funName);
	fprintf(outFp,"\n");
	fprintf(outFp,"\n/*\n���ܣ�\n");
	fprintf(outFp,"	�ṹ��ȱʡ��ֵ����\n");
	fprintf(outFp,"�������:\n");
	fprintf(outFp,"	prec	��¼ָ��\n");
	fprintf(outFp,"�������:\n");
	fprintf(outFp,"	��\n");
	fprintf(outFp,"����ֵ\n");
	fprintf(outFp,"	>=0		�ɹ�,ƴװ�ļ�¼���ĳ���\n");
	fprintf(outFp,"	<0		ʧ��,������\n");
	fprintf(outFp,"*/\n");
	fprintf(outFp,"int %s(%s prec)",funName,structPtrDefName);
	if (!includeFunBody)
		fprintf(outFp,";\n");
	else
		fprintf(outFp,"\n");
	return;
}

/*
����	
	���ݱ��壬����������ȡ��¼�ĺ���������
�������
	tblName		����
	includeFunBody	�Ƿ���������壬1,������0������
	fp		�ļ����
�������
	funName		�����ĺ�������
����ֵ
	��
*/
void UnionGenerateBatchReadRecFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp)
{
	char	structPtrDefName[128+1];
	
	memset(structPtrDefName,0,sizeof(structPtrDefName));
	UnionFormStructDefNameFromTBLName(tblName,0,structPtrDefName);
	UnionFormBatchReadRecFunNameFromTBLName(tblName,funName);
	fprintf(outFp,"\n");
	fprintf(outFp,"\n/*\n���ܣ�\n");
	fprintf(outFp,"	����������������ȡ����\n");
	fprintf(outFp,"�������:\n");
	fprintf(outFp,"	condition	�����ִ�\n");
	fprintf(outFp,"	maxRecNum		��ȡ������¼����\n");
	fprintf(outFp,"�������:\n");
	fprintf(outFp,"	recGrp		��¼����\n");
	fprintf(outFp,"����ֵ\n");
	fprintf(outFp,"	>=0		�ɹ�,��ȡ�ļ�¼����\n");
	fprintf(outFp,"	<0		ʧ��,������\n");
	fprintf(outFp,"*/\n");
	fprintf(outFp,"int %s(char *condition,%s recGrp[],int maxRecNum)",funName,structPtrDefName);
	if (!includeFunBody)
		fprintf(outFp,";\n");
	else
		fprintf(outFp,"\n");
	return;
}


/*
����	
	���ݱ��壬����һ�����ݽṹȱʡ��ֵ����
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateStructDefaultValueSetFunFromTBLDef(char *tblName,FILE *fp)
{
	FILE			*outFp = stdout;
	int			ret;
	TUnionObject		objDef;
	int			fldNum;
	PUnionObjectFldDef	pfldDef;
	char			funName[128+1];
	char			defaultValue[512+1];
	
	if (tblName == NULL)
		return(errCodeParameter);
		
	if (fp != NULL)
		outFp = fp;
	
	// ������
	memset(&objDef,0,sizeof(objDef));
	if ((ret = UnionReadObjectDefFromDesignDoc(tblName,&objDef)) < 0)
	{
		UnionUserErrLog("in UnionGenerateStructDefaultValueSetFunFromTBLDef:: UnionReadObjectDefFromDesignDoc [%s]!\n",tblName);
		return(ret);
	}
	// ��������	
	UnionGenerateStructDefaultValueSetFunDeclarationFromTBLDef(tblName,1,funName,outFp);
	// ������
	fprintf(outFp,"{\n");
	// �ֲ���������
	fprintf(outFp,"	int	ret;\n");
	fprintf(outFp,"\n");
	// �����ж�
	fprintf(outFp,"	if (prec == NULL)\n");
	fprintf(outFp,"	{\n");
	fprintf(outFp,"		UnionUserErrLog(\"in %s:: prec is null!\\n\");\n",funName);
	fprintf(outFp,"		return(UnionSetUserDefinedErrorCode(errCodeNullPointer));\n");
	fprintf(outFp,"	}\n");
	// ��ʼ������
	fprintf(outFp,"	memset(prec,0,sizeof(*prec));\n");
	for (fldNum = 0; fldNum < objDef.fldNum; fldNum++)
	{
		pfldDef = &(objDef.fldDefGrp[fldNum]);
		if (strlen(pfldDef->defaultValue) == 0)	// û��Ϊ������ȱʡֵ
			continue;
		// �������
		switch (pfldDef->type)
		{
			case	conObjectFldType_String:
				if (pfldDef->size > 1)
				{
					memset(defaultValue,0,sizeof(defaultValue));
					UnionConvertDefaultValue(pfldDef->defaultValue,defaultValue);
					fprintf(outFp,"	if (strlen(\"%s\") >= sizeof(prec->%s))\n",defaultValue,pfldDef->name);
					fprintf(outFp,"	{\n");
					fprintf(outFp,"		UnionUserErrLog(\"in %s:: default value of [%s] = [%s] is too long!\\n\");\n",funName,pfldDef->name,defaultValue);
					fprintf(outFp,"		return(UnionSetUserDefinedErrorCode(errCodeDefaultValueTooLong));\n");
					fprintf(outFp,"	}\n");
					fprintf(outFp,"	strcpy(prec->%s,\"%s\");\n",pfldDef->name,pfldDef->defaultValue);
				}
				else
					fprintf(outFp,"	prec->%s = '%c';\n",pfldDef->name,pfldDef->defaultValue[0]);
				break;
			case	conObjectFldType_Int:
			case	conObjectFldType_Double:
				fprintf(outFp,"	prec->%s = %s;\n",pfldDef->name,pfldDef->defaultValue);
				break;
			case	conObjectFldType_Bit:
				fprintf(outFp,"	if (strlen(\"%s\") > sizeof(prec->%s) * 2)\n",pfldDef->defaultValue,pfldDef->name);
				fprintf(outFp,"	{\n");
				fprintf(outFp,"		UnionUserErrLog(\"in %s:: default value of [%s] = [%s] is too long!\\n);\n",funName,pfldDef->name,pfldDef->defaultValue);
				fprintf(outFp,"		return(UnionSetUserDefinedErrorCode(errCodeDefaultValueTooLong));\n");
				fprintf(outFp,"	}\n");
				fprintf(outFp,"	aschex_to_bcdhex(\"%s\",strlen(\"%s\"),prec->%s);\n",pfldDef->defaultValue,pfldDef->defaultValue,pfldDef->name);
				break;
			case	conObjectFldType_Bool:
				UnionToUpperCase(pfldDef->defaultValue);
				if (strcmp(pfldDef->defaultValue,"TRUE") == 0)
					fprintf(outFp,"	prec->%s = 1;\n",pfldDef->name);
				else
					fprintf(outFp,"	prec->%s = 0;\n",pfldDef->name);
				break;
			default:
				UnionAuditLog("in UnionGenerateStructDefaultValueSetFunFromTBLDef:: invalid fld type = [%d]\n",pfldDef->type);
				break;
		}
	}
	fprintf(outFp,"	return(0);\n");
	fprintf(outFp,"}\n");
	return(0);
}


/*
����	
	���ݱ��壬����һ��������ȡ��¼�ĺ���
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateBatchReadRecFunFromTBLDef(char *tblName,FILE *fp)
{
	FILE			*outFp = stdout;
	int			ret;
	TUnionObject		objDef;
	int			fldNum;
	char			tableName[128+1];
	char			structName[128+1];
	char			readRecFromRecStrFunName[128+1];
	char			funName[128+1];
	
	if (tblName == NULL)
		return(errCodeParameter);
		
	if (fp != NULL)
		outFp = fp;
	
	// ������
	memset(&objDef,0,sizeof(objDef));
	if ((ret = UnionReadObjectDefFromDesignDoc(tblName,&objDef)) < 0)
	{
		UnionUserErrLog("in UnionGenerateBatchReadRecFunFromTBLDef:: UnionReadObjectDefFromDesignDoc [%s]!\n",tblName);
		return(ret);
	}
	memset(structName,0,sizeof(structName));
	memset(readRecFromRecStrFunName,0,sizeof(readRecFromRecStrFunName));
	UnionFormStructDefNameFromTBLName(tblName,0,structName);
	UnionFormConvertRecStrIntoStructFunNameFromTBLName(tblName,readRecFromRecStrFunName);
	// ��������	
	memset(funName,0,sizeof(funName));
	UnionGenerateBatchReadRecFunDeclarationFromTBLDef(tblName,1,funName,outFp);
	// ������
	fprintf(outFp,"{\n");
	// �ֲ���������
	fprintf(outFp,"	int	ret;\n");
	fprintf(outFp,"	char	fileName[512+1];\n");
	fprintf(outFp,"	int	recNum = 0;\n");
	fprintf(outFp,"	FILE	*recFileFp = NULL;\n");
	fprintf(outFp,"	char	recStr[8192*2+1];\n");
	fprintf(outFp,"	int	lenOfRecStr;\n");
	fprintf(outFp,"	char	tableName[128+1];\n");
	fprintf(outFp,"	%s	rec;\n",structName);
	fprintf(outFp,"\n");
	fprintf(outFp,"	memset(fileName,0,sizeof(fileName));\n");
	fprintf(outFp,"	sprintf(fileName,\"%%s/pid-%%d.tmp\",getenv(\"UNIONTEMP\"),getpid());\n");
	memset(tableName,0,sizeof(tableName));
	UnionFormTBLNameConstNameFromTBLName(tblName,tableName);
	fprintf(outFp,"\n");
	fprintf(outFp,"// �����м�¼\n");
	fprintf(outFp,"	if ((ret = UnionBatchSelectObjectRecord(%s,condition,fileName)) < 0)\n",tableName);
	fprintf(outFp,"	{\n");
	fprintf(outFp,"		UnionUserErrLog(\"in %s:: UnionBatchSelectObjectRecord from [%%s] on [%%s]!\\n\",%s,condition);\n",funName,tableName);
	fprintf(outFp,"		return(ret);\n");
	fprintf(outFp,"	}\n");
	
	fprintf(outFp,"	// ���ļ�\n");
	fprintf(outFp,"	if ((recFileFp = fopen(fileName,\"r\")) == NULL)\n");
	fprintf(outFp,"	{\n");
	fprintf(outFp,"		UnionUserErrLog(\"in %s:: fopen [%%s]\\n\",fileName);\n",funName);
	fprintf(outFp,"		goto errExit;\n");
	fprintf(outFp,"	}\n");
	fprintf(outFp,"	while (!feof(recFileFp))\n");
	fprintf(outFp,"	{\n");
	fprintf(outFp,"		if (recNum >= maxRecNum)\n");
	fprintf(outFp,"		{\n");
	fprintf(outFp,"			UnionLog(\"in %s:: still exists records satisfy the condition [%%s]!\\n\",condition);\n",funName);
	fprintf(outFp,"			ret = recNum;\n");
	fprintf(outFp,"			goto errExit;\n");
	fprintf(outFp,"		}\n");
	fprintf(outFp,"		memset(recStr,0,sizeof(recStr));\n");
	fprintf(outFp,"		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(recFileFp,recStr,sizeof(recStr))) <= 0)\n");
	fprintf(outFp,"			continue;\n");
	fprintf(outFp,"		memset(&rec,0,sizeof(rec));\n");
	fprintf(outFp,"		if ((ret = %s(recStr,lenOfRecStr,&rec)) < 0)\n",readRecFromRecStrFunName);
	fprintf(outFp,"		{\n");
	fprintf(outFp,"			UnionUserErrLog(\"in %s:: %s from [%%04d] [%%s]\\n\",lenOfRecStr,recStr);\n",funName,readRecFromRecStrFunName);
	fprintf(outFp,"			goto errExit;\n");
	fprintf(outFp,"		}\n");
	fprintf(outFp,"		memcpy(&(recGrp[recNum]),&rec,sizeof(rec));\n");
	fprintf(outFp,"		recNum++;\n");
	fprintf(outFp,"	}\n");
	fprintf(outFp,"	ret = recNum;\n");
	fprintf(outFp,"errExit:\n");
	fprintf(outFp,"	fclose(recFileFp);\n");
	fprintf(outFp,"	UnionDeleteFile(fileName);\n");
	fprintf(outFp,"	return(ret);\n");
	fprintf(outFp,"\n");
	fprintf(outFp,"}\n");
	return(0);
}

/*
����	
	���ݱ��壬����һ������¼�ṹת��Ϊ��¼�ִ��ĺ���������
�������
	tblName		����
	includeFunBody	�Ƿ���������壬1,������0������
	fp		�ļ����
�������
	funName		�����ĺ�������
����ֵ
	��
*/
void UnionGenerateConvertStructIntoRecStrFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp)
{
	char	structPtrDefName[128+1];
	
	memset(structPtrDefName,0,sizeof(structPtrDefName));
	UnionFormStructDefNameFromTBLName(tblName,1,structPtrDefName);
	UnionFormConvertStructIntoRecStrFunNameFromTBLName(tblName,funName);
	fprintf(outFp,"\n");
	fprintf(outFp,"\n/*\n���ܣ�\n");
	fprintf(outFp,"	����¼�ṹת��Ϊ��¼�ִ��ĺ���\n");
	fprintf(outFp,"�������:\n");
	fprintf(outFp,"	prec	��¼ָ��\n");
	fprintf(outFp,"	sizeOfBuf	���ռ�¼�ִ��Ļ���Ĵ�С\n");
	fprintf(outFp,"�������:\n");
	fprintf(outFp,"	recStr	��¼�ִ�\n");
	fprintf(outFp,"����ֵ\n");
	fprintf(outFp,"	>=0		�ɹ�,ƴװ�ļ�¼���ĳ���\n");
	fprintf(outFp,"	<0		ʧ��,������\n");
	fprintf(outFp,"*/\n");
	fprintf(outFp,"int %s(%s prec,char *recStr,int sizeOfBuf)",funName,structPtrDefName);
	if (!includeFunBody)
		fprintf(outFp,";\n");
	else
		fprintf(outFp,"\n");
	return;
}

/*
����	
	���ݱ��壬����һ������¼�ṹ�йؼ���ת��Ϊ��¼�ִ��ĺ���������
�������
	tblName		����
	includeFunBody	�Ƿ���������壬1,������0������
	fp		�ļ����
�������
	funName		�����ĺ�������
����ֵ
	��
*/
void UnionGenerateConvertPrimaryKeyInStructIntoRecStrFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp)
{
	char	structPtrDefName[128+1];
	
	memset(structPtrDefName,0,sizeof(structPtrDefName));
	UnionFormStructDefNameFromTBLName(tblName,1,structPtrDefName);
	UnionFormConvertPrimaryKeyInStructIntoRecStrFunNameFromTBLName(tblName,funName);
	fprintf(outFp,"\n");
	fprintf(outFp,"\n/*\n���ܣ�\n");
	fprintf(outFp,"	����¼�ṹת��Ϊ��¼�ִ��ĺ���\n");
	fprintf(outFp,"�������:\n");
	fprintf(outFp,"	prec	��¼ָ��\n");
	fprintf(outFp,"	sizeOfBuf	���ռ�¼�ִ��Ļ���Ĵ�С\n");
	fprintf(outFp,"�������:\n");
	fprintf(outFp,"	recStr	��¼�ִ�\n");
	fprintf(outFp,"����ֵ\n");
	fprintf(outFp,"	>=0		�ɹ�,ƴװ�ļ�¼���ĳ���\n");
	fprintf(outFp,"	<0		ʧ��,������\n");
	fprintf(outFp,"*/\n");
	fprintf(outFp,"int %s(%s prec,char *recStr,int sizeOfBuf)",funName,structPtrDefName);
	if (!includeFunBody)
		fprintf(outFp,";\n");
	else
		fprintf(outFp,"\n");
	return;
}

/*
����	
	���ݱ��壬����һ������¼�ṹ�зǹؼ���ת��Ϊ��¼�ִ��ĺ���������
�������
	tblName		����
	includeFunBody	�Ƿ���������壬1,������0������
	fp		�ļ����
�������
	funName		�����ĺ�������
����ֵ
	��
*/
void UnionGenerateConvertNonPrimaryKeyInStructIntoRecStrFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp)
{
	char	structPtrDefName[128+1];
	
	memset(structPtrDefName,0,sizeof(structPtrDefName));
	UnionFormStructDefNameFromTBLName(tblName,1,structPtrDefName);
	UnionFormConvertNonPrimaryKeyInStructIntoRecStrFunNameFromTBLName(tblName,funName);
	fprintf(outFp,"\n");
	fprintf(outFp,"\n/*\n���ܣ�\n");
	fprintf(outFp,"	����¼�ṹת��Ϊ��¼�ִ��ĺ���\n");
	fprintf(outFp,"�������:\n");
	fprintf(outFp,"	prec	��¼ָ��\n");
	fprintf(outFp,"	sizeOfBuf	���ռ�¼�ִ��Ļ���Ĵ�С\n");
	fprintf(outFp,"�������:\n");
	fprintf(outFp,"	recStr	��¼�ִ�\n");
	fprintf(outFp,"����ֵ\n");
	fprintf(outFp,"	>=0		�ɹ�,ƴװ�ļ�¼���ĳ���\n");
	fprintf(outFp,"	<0		ʧ��,������\n");
	fprintf(outFp,"*/\n");
	fprintf(outFp,"int %s(%s prec,char *recStr,int sizeOfBuf)",funName,structPtrDefName);
	if (!includeFunBody)
		fprintf(outFp,";\n");
	else
		fprintf(outFp,"\n");
	return;
}

/*
����	
	���ݱ��壬����һ������¼�ṹת��Ϊ��¼�ִ��ĺ���
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateConvertStructIntoRecStrFunFromTBLDef(char *tblName,FILE *fp)
{
	FILE			*outFp = stdout;
	int			ret;
	TUnionObject		objDef;
	int			fldNum;
	PUnionObjectFldDef	pfldDef;
	char			fldDefName[128+1];
	char			funName[128+1];
	char			fun1Name[128+1];
	char			fldFunName[128+1];
	
	if (tblName == NULL)
		return(errCodeParameter);
		
	if (fp != NULL)
		outFp = fp;
	
	// ������
	memset(&objDef,0,sizeof(objDef));
	if ((ret = UnionReadObjectDefFromDesignDoc(tblName,&objDef)) < 0)
	{
		UnionUserErrLog("in UnionGenerateConvertStructIntoRecStrFunFromTBLDef:: UnionReadObjectDefFromDesignDoc [%s]!\n",tblName);
		return(ret);
	}
	// ��������	
	UnionGenerateConvertStructIntoRecStrFunDeclarationFromTBLDef(tblName,1,funName,outFp);
	// ������
	fprintf(outFp,"{\n");
	// �ֲ���������
	fprintf(outFp,"	int	ret;\n");
	fprintf(outFp,"	int	offset = 0;\n");
	fprintf(outFp,"\n");
	// �����ж�
	fprintf(outFp,"	if ((prec == NULL) || (recStr == NULL))\n");
	fprintf(outFp,"	{\n");
	fprintf(outFp,"		UnionUserErrLog(\"in %s:: prec or recStr is null!\\n\");\n",funName);
	fprintf(outFp,"		return(UnionSetUserDefinedErrorCode(errCodeNullPointer));\n");
	fprintf(outFp,"	}\n");

	UnionFormConvertPrimaryKeyInStructIntoRecStrFunNameFromTBLName(tblName,fun1Name);
	fprintf(outFp,"	if ((ret = %s(prec,recStr,sizeOfBuf)) < 0)\n",fun1Name);
	fprintf(outFp,"	{\n");
	fprintf(outFp,"		UnionUserErrLog(\"in %s:: %s!\\n\");\n",funName,fun1Name);
	fprintf(outFp,"		return(UnionSetUserDefinedErrorCode(ret));\n");
	fprintf(outFp,"	}\n");
	fprintf(outFp,"	offset = ret;\n");
	UnionFormConvertNonPrimaryKeyInStructIntoRecStrFunNameFromTBLName(tblName,fun1Name);
	fprintf(outFp,"	if ((ret = %s(prec,recStr+offset,sizeOfBuf-offset)) < 0)\n",fun1Name);
	fprintf(outFp,"	{\n");
	fprintf(outFp,"		UnionUserErrLog(\"in %s:: %s!\\n\");\n",funName,fun1Name);
	fprintf(outFp,"		return(UnionSetUserDefinedErrorCode(ret));\n");
	fprintf(outFp,"	}\n");
	fprintf(outFp,"	offset += ret;\n");
	fprintf(outFp,"	return(offset);\n");
	fprintf(outFp,"}\n");
	return(0);
}

/*
����	
	���ݱ��壬����һ������¼��ת��Ϊ��¼�ṹ�ĺ���������
�������
	tblName		����
	includeFunBody	�Ƿ���������壬1,������0������
	fp		�ļ����
�������
	funName		�����ĺ�������
����ֵ
	��
*/
void UnionGenerateConvertRecStrIntoStructFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp)
{
	char	structPtrDefName[128+1];
	
	memset(structPtrDefName,0,sizeof(structPtrDefName));
	UnionFormStructDefNameFromTBLName(tblName,1,structPtrDefName);
	UnionFormConvertRecStrIntoStructFunNameFromTBLName(tblName,funName);
	fprintf(outFp,"\n");
	fprintf(outFp,"\n/*\n���ܣ�\n");
	fprintf(outFp,"	����¼�ṹת��Ϊ��¼�ִ��ĺ���\n");
	fprintf(outFp,"�������:\n");
	fprintf(outFp,"	recStr	��¼�ִ�\n");
	fprintf(outFp,"	lenOfRecStr	��¼������\n");
	fprintf(outFp,"�������:\n");
	fprintf(outFp,"	prec	��¼ָ��\n");
	fprintf(outFp,"����ֵ\n");
	fprintf(outFp,"	>=0		����������Ŀ\n");
	fprintf(outFp,"	<0		ʧ��,������\n");
	fprintf(outFp,"*/\n");
	fprintf(outFp,"int %s(char *recStr,int lenOfRecStr,%s prec)",funName,structPtrDefName);
	if (!includeFunBody)
		fprintf(outFp,";\n");
	else
		fprintf(outFp,"\n");
	return;
}

/*
����	
	���ݱ��壬����һ������¼��ת��Ϊ��¼�ṹ�ĺ���
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateConvertRecStrIntoStructFunFromTBLDef(char *tblName,FILE *fp)
{
	FILE			*outFp = stdout;
	int			ret;
	TUnionObject		objDef;
	int			fldNum;
	PUnionObjectFldDef	pfldDef;
	char			fldDefName[128+1];
	char			funName[128+1];
	char			defaultValueSetFunName[128+1];
	char			fldFunName[128+1];
	
	if (tblName == NULL)
		return(errCodeParameter);
		
	if (fp != NULL)
		outFp = fp;
	
	// ������
	memset(&objDef,0,sizeof(objDef));
	if ((ret = UnionReadObjectDefFromDesignDoc(tblName,&objDef)) < 0)
	{
		UnionUserErrLog("in UnionGenerateConvertRecStrIntoStructFunFromTBLDef:: UnionReadObjectDefFromDesignDoc [%s]!\n",tblName);
		return(ret);
	}
	// ��������
	UnionGenerateConvertRecStrIntoStructFunDeclarationFromTBLDef(tblName,1,funName,outFp);
	// ������
	fprintf(outFp,"{\n");
	// �ֲ���������
	fprintf(outFp,"	int	ret;\n");
	fprintf(outFp,"	int	offset = 0;\n");
	fprintf(outFp,"\n");
	// �����ж�
	fprintf(outFp,"	if ((prec == NULL) || (recStr == NULL))\n");
	fprintf(outFp,"	{\n");
	fprintf(outFp,"		UnionUserErrLog(\"in %s:: prec or recStr is null!\\n\");\n",funName);
	fprintf(outFp,"		return(UnionSetUserDefinedErrorCode(errCodeNullPointer));\n");
	fprintf(outFp,"	}\n");
	// ��ʼ������
	memset(defaultValueSetFunName,0,sizeof(defaultValueSetFunName));
	UnionFormStructDefaultValueSetFunNameFromTBLName(objDef.name,defaultValueSetFunName);
	fprintf(outFp,"	%s(prec);\n",defaultValueSetFunName);
	for (fldNum = 0; fldNum < objDef.fldNum; fldNum++)
	{
		pfldDef = &(objDef.fldDefGrp[fldNum]);
		UnionFormStructFldDefNameFromTBLName(objDef.name,pfldDef->name,fldDefName);
		// �������
		switch (pfldDef->type)
		{
			case	conObjectFldType_String:
				if (pfldDef->size > 1)
				{
					fprintf(outFp,"	ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,%s,prec->%s,sizeof(prec->%s));\n",fldDefName,pfldDef->name,pfldDef->name);
					strcpy(fldFunName,"UnionReadRecFldFromRecStr");
				}
				else
				{
					fprintf(outFp,"	ret = UnionReadCharTypeRecFldFromRecStr(recStr,lenOfRecStr,%s,&(prec->%s));\n",fldDefName,pfldDef->name);
					strcpy(fldFunName,"UnionReadCharTypeRecFldFromRecStr");
				}
				break;
			case	conObjectFldType_Int:
			case	conObjectFldType_Bool:
				fprintf(outFp,"	ret = UnionReadIntTypeRecFldFromRecStr(recStr,lenOfRecStr,%s,&(prec->%s));\n",fldDefName,pfldDef->name);
				strcpy(fldFunName,"UnionReadIntTypeRecFldFromRecStr");
				break;
			case	conObjectFldType_Double:
				fprintf(outFp,"	ret = UnionReadDoubleTypeRecFldFromRecStr(recStr,lenOfRecStr,%s,&(prec->%s));\n",fldDefName,pfldDef->name);
				strcpy(fldFunName,"UnionReadDoubleTypeRecFldFromRecStr");
				break;
			case	conObjectFldType_Bit:
				fprintf(outFp,"	ret = UnionReadBitsTypeRecFldFromRecStr(recStr,lenOfRecStr,%s,prec->%s,sizeof(prec->%s));\n",fldDefName,pfldDef->name,pfldDef->name);
				strcpy(fldFunName,"UnionReadBitsTypeRecFldFromRecStr");
				break;
			default:
				fprintf(outFp,"	ret = UnionReadBitsTypeRecFldFromRecStr(recStr,lenOfRecStr,%s,prec->%s,sizeof(prec->%s));\n",fldDefName,pfldDef->name,pfldDef->name);
				strcpy(fldFunName,"UnionReadBitsTypeRecFldFromRecStr");
				break;
		}
		fprintf(outFp,"	if (ret < 0)\n");
		fprintf(outFp,"	{\n");
		fprintf(outFp,"		if (!%d)\n",pfldDef->nullPermitted);
		fprintf(outFp,"		{\n");
		fprintf(outFp,"			UnionUserErrLog(\"in %s:: %s fld [%s] error!\\n\");\n",funName,fldFunName,pfldDef->name);
		fprintf(outFp,"			return(UnionSetUserDefinedErrorCode(ret));\n");
		fprintf(outFp,"		}\n");
		fprintf(outFp,"	}\n");
	}
	fprintf(outFp,"	return(0);\n");
	fprintf(outFp,"}\n");
	return(0);
}

/*
����	
	���ݱ��壬����һ��������м�¼�ĺ���������
�������
	tblName		����
	includeFunBody	�Ƿ���������壬1,������0������
	fp		�ļ����
�������
	funName		�����ĺ�������
����ֵ
	��
*/
void UnionGenerateInsertRecFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp)
{
	char	structPtrDefName[128+1];
	
	memset(structPtrDefName,0,sizeof(structPtrDefName));
	UnionFormStructDefNameFromTBLName(tblName,1,structPtrDefName);
	UnionFormInsertRecFunNameFromTBLName(tblName,funName);
	fprintf(outFp,"\n");
	fprintf(outFp,"\n/*\n���ܣ�\n");
	fprintf(outFp,"	����һ����¼\n");
	fprintf(outFp,"�������:\n");
	fprintf(outFp,"	prec	��¼ָ��\n");
	fprintf(outFp,"�������:\n");
	fprintf(outFp,"	prec	��¼ָ��\n");
	fprintf(outFp,"����ֵ\n");
	fprintf(outFp,"	>=0		�ɹ������ؼ�¼�Ĵ�С\n");
	fprintf(outFp,"	<0		ʧ��,������\n");
	fprintf(outFp,"*/\n");
	fprintf(outFp,"int %s(%s prec)",funName,structPtrDefName);
	if (!includeFunBody)
		fprintf(outFp,";\n");
	else
		fprintf(outFp,"\n");
	return;
}

/*
����	
	���ݱ��壬����һ������¼������еĺ���
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateInsertRecFunFromTBLDef(char *tblName,FILE *fp)
{
	FILE			*outFp = stdout;
	int			ret;
	char			funName[128+1];
	char			structName[128+1];
	char			tblNameConst[128+1];
	char			recStrAndStructFunName[128+1];
	
	if (tblName == NULL)
		return(errCodeParameter);
		
	if (fp != NULL)
		outFp = fp;
	
	memset(structName,0,sizeof(structName));
	UnionFormStructNameFromTBLName(tblName,structName);
	// ��������	
	UnionGenerateInsertRecFunDeclarationFromTBLDef(tblName,1,funName,outFp);
	// ������
	fprintf(outFp,"{\n");
	// �ֲ���������
	fprintf(outFp,"	int	ret;\n");
	fprintf(outFp,"	int	lenOfRecStr;\n");
	fprintf(outFp,"	char	recStr[8192*4+1];\n");
	fprintf(outFp,"\n");
	// �����ж�
	fprintf(outFp,"	//�Ӽ�¼ƴװ��\n");
	fprintf(outFp,"	memset(recStr,0,sizeof(recStr));\n");
	memset(recStrAndStructFunName,0,sizeof(recStrAndStructFunName));
	UnionFormConvertStructIntoRecStrFunNameFromTBLName(tblName,recStrAndStructFunName);
	fprintf(outFp,"	if ((lenOfRecStr = %s(prec,recStr,sizeof(recStr))) < 0)\n",recStrAndStructFunName);
	fprintf(outFp,"	{\n");
	fprintf(outFp,"		UnionUserErrLog(\"in %s:: %s!\\n\");\n",funName,recStrAndStructFunName);
	fprintf(outFp,"		return(UnionSetUserDefinedErrorCode(lenOfRecStr));\n");
	fprintf(outFp,"	}\n");
	fprintf(outFp,"\n");
	
	fprintf(outFp,"	//�����Զ�������\n");
	fprintf(outFp,"	if ((lenOfRecStr = UnionAutoAppendDBRecInputAttr(recStr,lenOfRecStr,sizeof(recStr))) < 0)\n");
	fprintf(outFp,"	{\n");
	fprintf(outFp,"		UnionUserErrLog(\"in %s:: UnionAutoAppendDBRecInputAttr!\\n\");\n",funName);
	fprintf(outFp,"		return(UnionSetUserDefinedErrorCode(lenOfRecStr));\n");
	fprintf(outFp,"	}\n");
	fprintf(outFp,"\n");
	
	fprintf(outFp,"	//�����¼\n");
	UnionFormTBLNameConstNameFromTBLName(tblName,tblNameConst);
	fprintf(outFp,"	if ((ret = UnionInsertObjectRecord(%s,recStr,lenOfRecStr)) < 0)\n",tblNameConst);
	fprintf(outFp,"	{\n");
	fprintf(outFp,"		UnionUserErrLog(\"in %s:: UnionInsertObjectRecord!\\n\");\n",funName);
	fprintf(outFp,"		return(UnionSetUserDefinedErrorCode(ret));\n");
	fprintf(outFp,"	}\n");
	fprintf(outFp,"\n");
	fprintf(outFp,"	return(ret);\n");
	fprintf(outFp,"}\n");
	return(0);
}

/*
����	
	���ݱ��壬����һ��ɾ�����м�¼�ĺ���������
�������
	tblName		����
	includeFunBody	�Ƿ���������壬1,������0������
	fp		�ļ����
�������
	funName		�����ĺ�������
����ֵ
	��
*/
void UnionGenerateDeleteRecFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp)
{
	int		ret;	

	UnionFormDeleteRecFunNameFromTBLName(tblName,funName);
	fprintf(outFp,"\n");
	fprintf(outFp,"\n/*\n���ܣ�\n");
	fprintf(outFp,"	ɾ��һ����¼\n");
	fprintf(outFp,"�������:\n");

	// ��������
	if ((ret = UnionGenerateSpecRecPrimaryKeyRemarkAsVarFunToFp(tblName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateDeleteRecFunDeclarationFromTBLDef:: UnionGenerateSpecRecPrimaryKeyRemarkAsVarFunToFp [%s]!\n",tblName);
		return;
	}
	fprintf(outFp,"�������:\n");
	fprintf(outFp,"	��\n");
	fprintf(outFp,"����ֵ:\n");
	fprintf(outFp,"	>=0 �ɹ�\n");
	fprintf(outFp,"	<0 ʧ��\n");
	fprintf(outFp,"*/\n");
	fprintf(outFp,"int %s(",funName);
	if ((ret = UnionGenerateSpecRecPrimaryKeyAsVarFunToFp(tblName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateDeleteRecFunDeclarationFromTBLDef:: UnionGenerateSpecRecPrimaryKeyAsVarFunToFp [%s]!\n",tblName);
		return;
	}
	if (!includeFunBody)
		fprintf(outFp,");\n");
	else
		fprintf(outFp,")\n");
	return;
}

/*
����	
	���ݱ��壬����һ������¼ɾ�����еĺ���
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateDeleteRecFunFromTBLDef(char *tblName,FILE *fp)
{
	FILE			*outFp = stdout;
	int			ret;
	char			funName[128+1];
	char			structName[128+1];
	char			tblNameConst[128+1];
	
	if (tblName == NULL)
		return(errCodeParameter);
		
	if (fp != NULL)
		outFp = fp;
	
	memset(structName,0,sizeof(structName));
	UnionFormStructNameFromTBLName(tblName,structName);
	// ��������	
	UnionGenerateDeleteRecFunDeclarationFromTBLDef(tblName,1,funName,outFp);
	// ������
	fprintf(outFp,"{\n");
	// �ֲ���������
	fprintf(outFp,"	int	ret;\n");
	fprintf(outFp,"	int	lenOfRecStr = 0;\n");
	fprintf(outFp,"	char	recStr[8192*4+1];\n");
	fprintf(outFp,"\n");
	// �����ж�
	if ((ret = UnionGenerateSpecRecPrimaryKeyIntoRecStrToFp(funName,tblName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateDeleteRecFunFromTBLDef:: UnionGenerateSpecRecPrimaryKeyIntoRecStrToFp [%s]!\n",tblName);
		return(UnionSetUserDefinedErrorCode(ret));
	}	
	fprintf(outFp,"	//ɾ����¼\n");
	UnionFormTBLNameConstNameFromTBLName(tblName,tblNameConst);
	fprintf(outFp,"	if ((ret = UnionDeleteUniqueObjectRecord(%s,recStr)) < 0)\n",tblNameConst);
	fprintf(outFp,"	{\n");
	fprintf(outFp,"		UnionUserErrLog(\"in %s:: UnionDeleteUniqueObjectRecord!\\n\");\n",funName);
	fprintf(outFp,"		return(UnionSetUserDefinedErrorCode(ret));\n");
	fprintf(outFp,"	}\n");
	fprintf(outFp,"\n");
	fprintf(outFp,"	return(ret);\n");
	fprintf(outFp,"}\n");
	return(0);
}

/*
����	
	���ݱ��壬����һ���޸ı��м�¼�ĺ���������
�������
	tblName		����
	includeFunBody	�Ƿ���������壬1,������0������
	fp		�ļ����
�������
	funName		�����ĺ�������
����ֵ
	��
*/
void UnionGenerateUpdateRecFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp)
{
	char	structPtrDefName[128+1];
	
	memset(structPtrDefName,0,sizeof(structPtrDefName));
	UnionFormStructDefNameFromTBLName(tblName,1,structPtrDefName);
	UnionFormUpdateRecFunNameFromTBLName(tblName,funName);
	fprintf(outFp,"\n");
	fprintf(outFp,"\n/*\n���ܣ�\n");
	fprintf(outFp,"	�޸�һ����¼\n");
	fprintf(outFp,"�������:\n");
	fprintf(outFp,"	prec	��¼ָ��\n");
	fprintf(outFp,"�������:\n");
	fprintf(outFp,"	prec	��¼ָ��\n");
	fprintf(outFp,"����ֵ\n");
	fprintf(outFp,"	>=0		�ɹ������ؼ�¼�Ĵ�С\n");
	fprintf(outFp,"	<0		ʧ��,������\n");
	fprintf(outFp,"*/\n");
	fprintf(outFp,"int %s(%s prec)",funName,structPtrDefName);
	if (!includeFunBody)
		fprintf(outFp,";\n");
	else
		fprintf(outFp,"\n");
	return;
}

/*
����	
	���ݱ��壬����һ������¼�޸ı��еĺ���
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateUpdateRecFunFromTBLDef(char *tblName,FILE *fp)
{
	FILE			*outFp = stdout;
	int			ret;
	char			funName[128+1];
	char			structName[128+1];
	char			tblNameConst[128+1];
	char			recStrAndStructFunName[128+1];
	
	if (tblName == NULL)
		return(errCodeParameter);
		
	if (fp != NULL)
		outFp = fp;
	
	memset(structName,0,sizeof(structName));
	UnionFormStructNameFromTBLName(tblName,structName);
	// ��������	
	UnionGenerateUpdateRecFunDeclarationFromTBLDef(tblName,1,funName,outFp);
	// ������
	fprintf(outFp,"{\n");
	// �ֲ���������
	fprintf(outFp,"	int	ret;\n");
	fprintf(outFp,"	int	lenOfRecStr;\n");
	fprintf(outFp,"	char	recStr[8192*4+1];\n");
	fprintf(outFp,"	char	primaryKey[1024+1];\n");
	fprintf(outFp,"\n");
	// �����ж�
	fprintf(outFp,"	//�Ӽ�¼ƴ�ؼ���\n");
	fprintf(outFp,"	memset(primaryKey,0,sizeof(primaryKey));\n");
	memset(recStrAndStructFunName,0,sizeof(recStrAndStructFunName));
	UnionFormConvertPrimaryKeyInStructIntoRecStrFunNameFromTBLName(tblName,recStrAndStructFunName);
	fprintf(outFp,"	if ((ret = %s(prec,primaryKey,sizeof(primaryKey))) < 0)\n",recStrAndStructFunName);
	fprintf(outFp,"	{\n");
	fprintf(outFp,"		UnionUserErrLog(\"in %s:: %s!\\n\");\n",funName,recStrAndStructFunName);
	fprintf(outFp,"		return(UnionSetUserDefinedErrorCode(ret));\n");
	fprintf(outFp,"	}\n");
	fprintf(outFp,"\n");
	
	fprintf(outFp,"	//�Ӽ�¼�Ӽ�¼��\n");
	fprintf(outFp,"	memset(recStr,0,sizeof(recStr));\n");
	memset(recStrAndStructFunName,0,sizeof(recStrAndStructFunName));
	UnionFormConvertNonPrimaryKeyInStructIntoRecStrFunNameFromTBLName(tblName,recStrAndStructFunName);
	fprintf(outFp,"	if ((lenOfRecStr = %s(prec,recStr,sizeof(recStr))) < 0)\n",recStrAndStructFunName);
	fprintf(outFp,"	{\n");
	fprintf(outFp,"		UnionUserErrLog(\"in %s:: %s!\\n\");\n",funName,recStrAndStructFunName);
	fprintf(outFp,"		return(UnionSetUserDefinedErrorCode(lenOfRecStr));\n");
	fprintf(outFp,"	}\n");
	fprintf(outFp,"\n");
	
	fprintf(outFp,"	//�����Զ��޸���\n");
	fprintf(outFp,"	if ((lenOfRecStr = UnionAutoAppendDBRecUpdateAttr(recStr,lenOfRecStr,sizeof(recStr))) < 0)\n");
	fprintf(outFp,"	{\n");
	fprintf(outFp,"		UnionUserErrLog(\"in %s:: UnionAutoAppendDBRecUpdateAttr!\\n\");\n",funName);
	fprintf(outFp,"		return(UnionSetUserDefinedErrorCode(lenOfRecStr));\n");
	fprintf(outFp,"	}\n");
	fprintf(outFp,"\n");
	
	fprintf(outFp,"	//�޸ļ�¼\n");
	UnionFormTBLNameConstNameFromTBLName(tblName,tblNameConst);
	fprintf(outFp,"	if ((ret = UnionUpdateUniqueObjectRecord(%s,primaryKey,recStr,lenOfRecStr)) < 0)\n",tblNameConst);
	fprintf(outFp,"	{\n");
	fprintf(outFp,"		UnionUserErrLog(\"in %s:: UnionUpdateUniqueObjectRecord!\\n\");\n",funName);
	fprintf(outFp,"		return(UnionSetUserDefinedErrorCode(ret));\n");
	fprintf(outFp,"	}\n");
	fprintf(outFp,"\n");
	fprintf(outFp,"	return(ret);\n");
	fprintf(outFp,"}\n");
	return(0);
}

/*
����	
	���ݱ��壬����һ����ȡ���м�¼�ĺ���������
�������
	tblName		����
	includeFunBody	�Ƿ���������壬1,������0������
	fp		�ļ����
�������
	funName		�����ĺ�������
����ֵ
	��
*/
void UnionGenerateSelectRecFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp)
{
	char	structPtrDefName[128+1];
	int	ret;
	
	memset(structPtrDefName,0,sizeof(structPtrDefName));
	UnionFormStructDefNameFromTBLName(tblName,1,structPtrDefName);
	UnionFormReadRecFunNameFromTBLName(tblName,funName);
	fprintf(outFp,"\n");
	fprintf(outFp,"\n/*\n���ܣ�\n");
	fprintf(outFp,"	��ȡһ����¼\n");
	fprintf(outFp,"�������:\n");
	//fprintf(outFp,"	prec	��¼ָ�룬����Թؼ�����ֵ\n");
	if ((ret = UnionGenerateSpecRecPrimaryKeyRemarkAsVarFunToFp(tblName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSelectRecFunDeclarationFromTBLDef:: UnionGenerateSpecRecPrimaryKeyRemarkAsVarFunToFp [%s]\n",tblName);
		return;
	}
	fprintf(outFp,"�������:\n");
	fprintf(outFp,"	prec	��¼ָ��\n");
	fprintf(outFp,"����ֵ\n");
	fprintf(outFp,"	>=0		�ɹ������ؼ�¼�Ĵ�С\n");
	fprintf(outFp,"	<0		ʧ��,������\n");
	fprintf(outFp,"*/\n");
	fprintf(outFp,"int %s(",funName);
	if ((ret = UnionGenerateSpecRecPrimaryKeyAsVarFunToFp(tblName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSelectRecFunDeclarationFromTBLDef:: UnionGenerateSpecRecPrimaryKeyAsVarFunToFp [%s]\n",tblName);
		return;
	}
	fprintf(outFp,",%s prec)",structPtrDefName);
	if (!includeFunBody)
		fprintf(outFp,";\n");
	else
		fprintf(outFp,"\n");
	return;
}

/*
����	
	���ݱ��壬����һ����ȡ���м�¼�ĺ���
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateSelectRecFunFromTBLDef(char *tblName,FILE *fp)
{
	FILE			*outFp = stdout;
	int			ret;
	char			funName[128+1];
	char			structName[128+1];
	char			tblNameConst[128+1];
	char			recStrAndStructFunName[128+1];
	
	if (tblName == NULL)
		return(errCodeParameter);
		
	if (fp != NULL)
		outFp = fp;
	
	memset(structName,0,sizeof(structName));
	UnionFormStructNameFromTBLName(tblName,structName);
	// ��������
	UnionGenerateSelectRecFunDeclarationFromTBLDef(tblName,1,funName,fp);
	// ������
	fprintf(outFp,"{\n");
	// �ֲ���������
	fprintf(outFp,"	int	ret;\n");
	fprintf(outFp,"	int	lenOfRecStr = 0;\n");
	fprintf(outFp,"	char	recStr[8192*4+1];\n");
	fprintf(outFp,"	char	condition[1024+1];\n");
	fprintf(outFp,"\n");
	// �����ж�
	fprintf(outFp,"	//�Ӽ�¼ƴװ��\n");
	/*
	memset(recStrAndStructFunName,0,sizeof(recStrAndStructFunName));
	UnionFormConvertStructIntoRecStrFunNameFromTBLName(tblName,recStrAndStructFunName);
	fprintf(outFp,"	if ((ret = %s(prec,condition,sizeof(condition))) < 0)\n",recStrAndStructFunName);
	fprintf(outFp,"	{\n");
	fprintf(outFp,"		UnionUserErrLog(\"in %s:: %s!\\n\");\n",funName,recStrAndStructFunName);
	fprintf(outFp,"		return(UnionSetUserDefinedErrorCode(ret));\n");
	fprintf(outFp,"	}\n");
	fprintf(outFp,"\n");
	*/
	if ((ret = UnionGenerateSpecRecPrimaryKeyIntoRecStrToFp(funName,tblName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSelectRecFunFromTBLDef:: UnionGenerateSpecRecPrimaryKeyIntoRecStrToFp!\n");
		return(ret);
	}
	fprintf(outFp,"	if (lenOfRecStr >= sizeof(condition))\n");
	fprintf(outFp,"	{\n");
	fprintf(outFp,"		UnionUserErrLog(\"in %s:: lenOfCondition = [%%d] too long!\\n\",lenOfRecStr);\n",funName);
	fprintf(outFp,"		return(ret);\n");
	fprintf(outFp,"	}\n");
	fprintf(outFp,"	memset(condition,0,sizeof(condition));\n");
	fprintf(outFp,"	memcpy(condition,recStr,lenOfRecStr);\n");
	fprintf(outFp,"\n");
	fprintf(outFp,"	//��ȡ��¼\n");
	UnionFormTBLNameConstNameFromTBLName(tblName,tblNameConst);
	fprintf(outFp,"	memset(recStr,0,sizeof(recStr));\n");
	fprintf(outFp,"	if ((lenOfRecStr = UnionSelectUniqueObjectRecordByPrimaryKey(%s,condition,recStr,sizeof(recStr))) < 0)\n",tblNameConst);
	fprintf(outFp,"	{\n");
	fprintf(outFp,"		UnionUserErrLog(\"in %s:: UnionSelectUniqueObjectRecordByPrimaryKey!\\n\");\n",funName);
	fprintf(outFp,"		return(UnionSetUserDefinedErrorCode(lenOfRecStr));\n");
	fprintf(outFp,"	}\n");
	fprintf(outFp,"\n");
	
	fprintf(outFp,"	//�Ӽ�¼��ƴװ�ṹ\n");
	memset(recStrAndStructFunName,0,sizeof(recStrAndStructFunName));
	UnionFormConvertRecStrIntoStructFunNameFromTBLName(tblName,recStrAndStructFunName);
	fprintf(outFp,"	if ((ret = %s(recStr,lenOfRecStr,prec)) < 0)\n",recStrAndStructFunName);
	fprintf(outFp,"	{\n");
	fprintf(outFp,"		UnionUserErrLog(\"in %s:: %s!\\n\");\n",funName,recStrAndStructFunName);
	fprintf(outFp,"		return(UnionSetUserDefinedErrorCode(ret));\n");
	fprintf(outFp,"	}\n");
	fprintf(outFp,"\n");
	
	fprintf(outFp,"	return(sizeof(*prec));\n");
	fprintf(outFp,"}\n");
	return(0);
}

/*
����	
	�����Զ����ɵĴ����ʶ
�������
	��
�������
	��
����ֵ
	��
*/
void UnionGenerateAutoCodesTagToFp(FILE *fp)
{
	char			fullSystemDateTime[14+1];
	
	FILE			*outFp = stdout;

	if (fp != NULL)
		outFp = fp;
	memset(fullSystemDateTime,0,sizeof(fullSystemDateTime));
	UnionGetFullSystemDateTime(fullSystemDateTime);
	fprintf(outFp,"// Automatically generated codes\n");
	fprintf(outFp,"// Generated Date and Time %s\n",fullSystemDateTime);
	fprintf(outFp,"// Generated by %s\n",getenv("LOGNAME"));
	fprintf(outFp,"\n");
	return;
}

/*
����	
	���ݱ��壬���������ͽṹ�����ͷ�ļ�
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/

int UnionGenerateVersionSpecConstDefAndStructDefFromTBLDef(char *tblName,FILE *fp)
{
	char	versionTag[128+1],currentVersionTag[128+1];
	char	versionGrp[128][64+1];
	int	versionNum;
	int	versionIndex;
	char	structDefFileName[512];
	FILE	*outFp = stdout;
	FILE	*structDefFp;
	int	ret;
			
	if (fp != NULL)
		outFp = fp;

	// ���ɰ汾��ʶ
	memset(currentVersionTag,0,sizeof(currentVersionTag));
	UnionFormStructTagNameOfCurrentVersionTBLDefFromTBLName(tblName,currentVersionTag);
	fprintf(outFp,"#ifdef _%s_\n",currentVersionTag);
	fprintf(outFp,"#include \"%s.h\"\n",currentVersionTag);
	// ��ȡ��ʷ�汾
	versionNum = UnionReadAllVersionNoOfSpecTBLDef(tblName,versionGrp,128);
	for (versionIndex = 0; versionIndex < versionNum; versionIndex++)
	{
		memset(versionTag,0,sizeof(versionTag));
		UnionFormStructTagNameOfSpecVersionTBLDefFromTBLName(tblName,versionGrp[versionIndex],versionTag);
		if (strcmp(versionTag,currentVersionTag) == 0)
			continue;
		fprintf(outFp,"#else\n");
		fprintf(outFp,"#ifdef _%s_\n",versionTag);
		fprintf(outFp,"#include \"%s.h\"\n",versionTag);
	}
	for (versionIndex = 0; versionIndex < versionNum; versionIndex++)
		fprintf(outFp,"#endif\n");
	fprintf(outFp,"#else\n");
	fprintf(outFp,"#include \"%s.h\"\n",currentVersionTag);
	fprintf(outFp,"#endif");

	// ��ýṹ�����ļ�������
	memset(structDefFileName,0,sizeof(structDefFileName));
	UnionFormFullTableDefStructHeadFileName(tblName,structDefFileName);
	if ((structDefFp = fopen(structDefFileName,"w")) == NULL)
	{
		UnionUserErrLog("in UnionGenerateVersionSpecConstDefAndStructDefFromTBLDef:: fopen [%s]!\n",structDefFileName);
		return(errCodeUseOSErrCode);
	}
	outFp = structDefFp;

	// �����Զ����ɱ�ʶ
	UnionGenerateAutoCodesTagToFp(outFp);
	
	// ���ɰ汾��ʶ
	memset(versionTag,0,sizeof(versionTag));
	UnionFormStructTagNameOfCurrentVersionTBLDefFromTBLName(tblName,versionTag);
	fprintf(outFp,"#ifndef _%s_\n",versionTag);
	fprintf(outFp,"#define _%s_\n",versionTag);
	fprintf(outFp,"#endif\n");
	fprintf(outFp,"\n");

	// ���峣��
	if ((ret = UnionGenerateConstDefFromTBL(tblName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateIncFileFromTBLDefToFp:: UnionGenerateConstDefFromTBL [%s] failure! ret = [%d]\n",tblName,ret);
		fclose(outFp);
		return(ret);
	}
	// �����ṹ
	if ((ret = UnionGenerateStructDefFromTBLDef(tblName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateIncFileFromTBLDefToFp:: UnionGenerateStructDefFromTBLDef [%s] failure! ret = [%d]\n",tblName,ret);
		fclose(outFp);
		return(ret);
	}
	fprintf(outFp,"\n");
	fclose(outFp);	
	return(0);
}

/*
����	
	���ݱ��壬����һ��ͷ�ļ�
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateIncFileFromTBLDefToFp(char *tblName,FILE *fp)
{
	FILE			*outFp = stdout;
	int			ret;
	char			programName[128+1];
	char			funName[128+1];
	
	if ((tblName == NULL) || (strcmp(tblName,"tableList") == 0) || (strcmp(tblName,"tableField") == 0))
		return(errCodeParameter);
	if (fp != NULL)
		outFp = fp;

	// �����Զ����ɱ�ʶ
	UnionGenerateAutoCodesTagToFp(outFp);
	// ����궨��
	UnionFormTBLProgramNameFromTBLName(tblName,programName);
	fprintf(outFp,"#ifndef _%s_\n",programName);
	fprintf(outFp,"#define _%s_\n",programName);
	fprintf(outFp,"\n");
	
	// ���ɽṹ����
	if ((ret = UnionGenerateVersionSpecConstDefAndStructDefFromTBLDef(tblName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateIncFileFromTBLDefToFp:: UnionGenerateVersionSpecConstDefAndStructDefFromTBLDef [%s] failure! ret = [%d]\n",tblName,ret);
		return(ret);
	}
		
	// ���庯��ԭ��
	fprintf(outFp,"\n//***** ���¶��庯������ *****\n");
	UnionGenerateStructDefaultValueSetFunDeclarationFromTBLDef(tblName,0,funName,outFp);
	UnionGenerateConvertStructIntoRecStrFunDeclarationFromTBLDef(tblName,0,funName,outFp);
	UnionGenerateConvertPrimaryKeyInStructIntoRecStrFunDeclarationFromTBLDef(tblName,0,funName,outFp);
	UnionGenerateConvertRecStrIntoStructPrimaryKeyFunDeclarationFromTBLDef(tblName,0,funName,outFp);
	UnionGenerateConvertNonPrimaryKeyInStructIntoRecStrFunDeclarationFromTBLDef(tblName,0,funName,outFp);
	UnionGenerateConvertRecStrIntoStructFunDeclarationFromTBLDef(tblName,0,funName,outFp);
	UnionGenerateInsertRecFunDeclarationFromTBLDef(tblName,0,funName,outFp);
	UnionGenerateDeleteRecFunDeclarationFromTBLDef(tblName,0,funName,outFp);
	UnionGenerateUpdateSpecFldFunDeclarationFromTBLDef(tblName,1,funName,outFp);
	UnionGenerateUpdateIntTypeSpecFldFunDeclarationFromTBLDef(tblName,1,funName,outFp);
	UnionGenerateUpdateRecFunDeclarationFromTBLDef(tblName,0,funName,outFp);
	UnionGenerateSelectRecFunDeclarationFromTBLDef(tblName,0,funName,outFp);
	UnionGenerateSelectRecFldFunDeclarationFromTBLDef(tblName,0,funName,outFp);
	UnionGenerateSelectRecIntTypeFldFunDeclarationFromTBLDef(tblName,0,funName,outFp);
	UnionGenerateBatchReadRecFunDeclarationFromTBLDef(tblName,0,funName,outFp);
	fprintf(outFp,"\n//***** ������������ *****\n");
	
	// �������
	fprintf(outFp,"\n#endif\n");
	return(0);
}

/*
����	
	���ݱ��壬����һ��ͷ�ļ�
�������
	tblName			����
	userSpecFileName	�û�ָ�����ļ���
	modeWhenFileExists	����ļ��Ѵ��ڵĲ�������
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateIncFileFromTBLDef(char *tblName,char *userSpecFileName,int modeWhenFileExists)
{
	char	fileName[512+1];
	FILE	*fp;
	int	ret;
	
	UnionLog("in UnionGenerateIncFileFromTBLDef:: userSpecFileName = [%s]\n",userSpecFileName);
	if ((userSpecFileName == NULL) || (strlen(userSpecFileName) == 0))
		UnionFormFullIncFileNameOfTBLProgramFromTBLName(tblName,fileName);
	else
		strcpy(fileName,userSpecFileName);
	if (!UnionExistsFile(fileName))
		goto writeNow;
	switch (modeWhenFileExists)
	{
		case	conOverwriteModeWhenFileExistsOverwrite:
			break;
		case	conOverwriteModeWhenFileExistsReturn:
			UnionUserErrLog("in UnionGenerateIncFileFromTBLDef:: %s already exists, the include file not generated!\n",fileName);
			return(errCodeFileAlreadyExists);
		default:
			if (!UnionConfirm("%s already exists, overwrite it",fileName))
				return(errCodeFileAlreadyExists);
			break;
	}
	UnionLog("in UnionGenerateIncFileFromTBLDef:: %s already exists, and overwrite it!\n",fileName);
writeNow:
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionUserErrLog("in UnionGenerateIncFileFromTBLDef:: fopen [%s] error!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	if ((ret = UnionGenerateIncFileFromTBLDefToFp(tblName,fp)) < 0)
		UnionUserErrLog("in UnionGenerateIncFileFromTBLDef:: Generate head file for table [%s] failure! ret = [%d]!\n",tblName,ret);
	else
		UnionLog("in UnionGenerateIncFileFromTBLDef:: Generate head file for table [%s] ok!\n",tblName);
	fclose(fp);
	return(ret);
}

/*
����	
	���ݱ��壬����һ��ͷ�ļ�
�������
	tblName			����
	userSpecFileName	�û�ָ�����ļ���
	modeWhenFileExists	����ļ��Ѵ��ڵĲ�������
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateVersionSpecConstDefAndStructDefFromTBLDefToSpecFile(char *tblName,char *userSpecFileName,int modeWhenFileExists)
{
	char	fileName[512+1];
	FILE	*fp;
	int	ret;
	
	UnionLog("in UnionGenerateVersionSpecConstDefAndStructDefFromTBLDefToSpecFile:: userSpecFileName = [%s]\n",userSpecFileName);
	if ((userSpecFileName == NULL) || (strlen(userSpecFileName) == 0))
		UnionFormFullIncFileNameOfTBLProgramFromTBLName(tblName,fileName);
	else
		strcpy(fileName,userSpecFileName);
	if (!UnionExistsFile(fileName))
		goto writeNow;
	switch (modeWhenFileExists)
	{
		case	conOverwriteModeWhenFileExistsOverwrite:
			break;
		case	conOverwriteModeWhenFileExistsReturn:
			UnionUserErrLog("in UnionGenerateVersionSpecConstDefAndStructDefFromTBLDefToSpecFile:: %s already exists, the include file not generated!\n",fileName);
			return(errCodeFileAlreadyExists);
		default:
			if (!UnionConfirm("%s already exists, overwrite it",fileName))
				return(errCodeFileAlreadyExists);
			break;
	}
	UnionLog("in UnionGenerateVersionSpecConstDefAndStructDefFromTBLDefToSpecFile:: %s already exists, and overwrite it!\n",fileName);
writeNow:
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionUserErrLog("in UnionGenerateVersionSpecConstDefAndStructDefFromTBLDefToSpecFile:: fopen [%s] error!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	if ((ret = UnionGenerateVersionSpecConstDefAndStructDefFromTBLDef(tblName,fp)) < 0)
		UnionUserErrLog("in UnionGenerateVersionSpecConstDefAndStructDefFromTBLDefToSpecFile:: Generate head file for table [%s] failure! ret = [%d]!\n",tblName,ret);
	else
		UnionLog("in UnionGenerateVersionSpecConstDefAndStructDefFromTBLDefToSpecFile:: Generate head file for table [%s] ok!\n",tblName);
	fclose(fp);
	return(ret);
}

/*
����	
	���ݱ��壬����һ��c�ļ�
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateCFileFromTBLDefToFp(char *tblName,FILE *fp)
{
	FILE			*outFp = stdout;
	int			ret;
	char			programName[128+1];
	char			funName[128+1];
	char			versionTag[128+1];
	
	if ((tblName == NULL) || (strcmp(tblName,"tableList") == 0) || (strcmp(tblName,"tableField") == 0))
		return(errCodeParameter);

	if (fp != NULL)
		outFp = fp;

	// �����Զ����ɱ�ʶ
	UnionGenerateAutoCodesTagToFp(outFp);
	
	// ���ɰ汾��ʶ
	memset(versionTag,0,sizeof(versionTag));
	UnionFormStructTagNameOfCurrentVersionTBLDefFromTBLName(tblName,versionTag);
	fprintf(outFp,"#ifndef _%s_\n",versionTag);
	fprintf(outFp,"#define _%s_\n",versionTag);
	fprintf(outFp,"#endif\n\n");
	
	// ������ͷ�ļ�
	fprintf(outFp,"#include <stdio.h>\n");
	fprintf(outFp,"#include <string.h>\n");
	fprintf(outFp,"#include <stdlib.h>\n");
	fprintf(outFp,"\n");
	fprintf(outFp,"#include \"UnionLog.h\"\n");
	fprintf(outFp,"#include \"UnionStr.h\"\n");
	fprintf(outFp,"#include \"unionErrCode.h\"\n");
	fprintf(outFp,"#include \"unionComplexDBRecord.h\"\n");
	fprintf(outFp,"#include \"%s.h\"\n",conDataTBLListFileName);
	fprintf(outFp,"\n");
	UnionFormIncFileNameOfTBLProgramFromTBLName(tblName,programName);
	fprintf(outFp,"#include \"%s\"\n",programName);
	
	if ((ret = UnionGenerateStructDefaultValueSetFunFromTBLDef(tblName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateCFileFromTBLDefToFp:: UnionGenerateStructDefaultValueSetFunFromTBLDef [%s] failure! ret = [%d]\n",tblName,ret);
		return(ret);
	}
	if ((ret = UnionGenerateConvertStructIntoRecStrFunFromTBLDef(tblName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateCFileFromTBLDefToFp:: UnionGenerateConvertStructIntoRecStrFunFromTBLDef [%s] failure! ret = [%d]\n",tblName,ret);
		return(ret);
	}
	if ((ret = UnionGenerateConvertPrimaryKeyInStructIntoRecStrFunFromTBLDef(tblName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateCFileFromTBLDefToFp:: UnionGenerateConvertPrimaryKeyInStructIntoRecStrFunFromTBLDef [%s] failure! ret = [%d]\n",tblName,ret);
		return(ret);
	}
	if ((ret = UnionGenerateConvertNonPrimaryKeyInStructIntoRecStrFunFromTBLDef(tblName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateCFileFromTBLDefToFp:: UnionGenerateConvertNonPrimaryKeyInStructIntoRecStrFunFromTBLDef [%s] failure! ret = [%d]\n",tblName,ret);
		return(ret);
	}
	if ((ret = UnionGenerateConvertRecStrIntoStructPrimaryKeyFunFromTBLDef(tblName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateCFileFromTBLDefToFp:: UnionGenerateConvertRecStrIntoStructPrimaryKeyFunFromTBLDef [%s] failure! ret = [%d]\n",tblName,ret);
		return(ret);
	}
	if ((ret = UnionGenerateConvertRecStrIntoStructFunFromTBLDef(tblName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateCFileFromTBLDefToFp:: UnionGenerateConvertRecStrIntoStructFunFromTBLDef [%s] failure! ret = [%d]\n",tblName,ret);
		return(ret);
	}
	if ((ret = UnionGenerateInsertRecFunFromTBLDef(tblName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateCFileFromTBLDefToFp:: UnionGenerateInsertRecFunFromTBLDef [%s] failure! ret = [%d]\n",tblName,ret);
		return(ret);
	}
	if ((ret = UnionGenerateDeleteRecFunFromTBLDef(tblName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateCFileFromTBLDefToFp:: UnionGenerateDeleteRecFunFromTBLDef [%s] failure! ret = [%d]\n",tblName,ret);
		return(ret);
	}
	if ((ret = UnionGenerateUpdateRecFunFromTBLDef(tblName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateCFileFromTBLDefToFp:: UnionGenerateUpdateRecFunFromTBLDef [%s] failure! ret = [%d]\n",tblName,ret);
		return(ret);
	}
	if ((ret = UnionGenerateUpdateSpecFldFunFromTBLDef(tblName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateCFileFromTBLDefToFp:: UnionGenerateUpdateSpecFldFunFromTBLDef [%s] failure! ret = [%d]\n",tblName,ret);
		return(ret);
	}
	if ((ret = UnionGenerateUpdateIntTypeSpecFldFunFromTBLDef(tblName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateCFileFromTBLDefToFp:: UnionGenerateUpdateIntTypeSpecFldFunFromTBLDef [%s] failure! ret = [%d]\n",tblName,ret);
		return(ret);
	}
	if ((ret = UnionGenerateSelectRecFunFromTBLDef(tblName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateCFileFromTBLDefToFp:: UnionGenerateSelectRecFunFromTBLDef [%s] failure! ret = [%d]\n",tblName,ret);
		return(ret);
	}
	if ((ret = UnionGenerateSelectRecIntTypeFldFunFromTBLDef(tblName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateCFileFromTBLDefToFp:: UnionGenerateSelectRecIntTypeFldFunFromTBLDef [%s] failure! ret = [%d]\n",tblName,ret);
		return(ret);
	}
	if ((ret = UnionGenerateSelectRecFldFunFromTBLDef(tblName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateCFileFromTBLDefToFp:: UnionGenerateSelectRecFldFunFromTBLDef [%s] failure! ret = [%d]\n",tblName,ret);
		return(ret);
	}
	if ((ret = UnionGenerateBatchReadRecFunFromTBLDef(tblName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateCFileFromTBLDefToFp:: UnionGenerateBatchReadRecFunFromTBLDef [%s] failure! ret = [%d]\n",tblName,ret);
		return(ret);
	}
	
	// �������
	return(0);
}

/*
����	
	���ݱ��壬����һ��C�ļ�
�������
	tblName			����
	userSpecFileName	�û�ָ�����ļ���
	modeWhenFileExists	����ļ��Ѵ��ڵĲ�������
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateCFileFromTBLDef(char *tblName,char *userSpecFileName,int modeWhenFileExists)
{
	char	fileName[512+1];
	FILE	*fp;
	int	ret;
	
	if ((userSpecFileName == NULL) || (strlen(userSpecFileName) == 0))
		UnionFormFullCFileNameOfTBLProgramFromTBLName(tblName,fileName);
	else
		strcpy(fileName,userSpecFileName);
	if (!UnionExistsFile(fileName))
		goto writeNow;
	switch (modeWhenFileExists)
	{
		case	conOverwriteModeWhenFileExistsOverwrite:
			break;
		case	conOverwriteModeWhenFileExistsReturn:
			UnionUserErrLog("in UnionGenerateCFileFromTBLDef:: %s already exists, the include file not generated!\n",fileName);
			return(errCodeFileAlreadyExists);
		default:
			if (!UnionConfirm("%s already exists, overwrite it",fileName))
				return(errCodeFileAlreadyExists);
			break;
	}
	UnionLog("in UnionGenerateCFileFromTBLDef:: %s already exists, and overwrite it!\n",fileName);
writeNow:
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionUserErrLog("in UnionGenerateCFileFromTBLDef:: fopen [%s] error!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	if ((ret = UnionGenerateCFileFromTBLDefToFp(tblName,fp)) < 0)
		UnionUserErrLog("in UnionGenerateCFileFromTBLDef:: Generate c file for table [%s] failure! ret = [%d]!\n",tblName,ret);
	else
		UnionLog("in UnionGenerateCFileFromTBLDef:: Generate c file for table [%s] ok!\n",tblName);
	fclose(fp);
	//if ((userSpecFileName == NULL) || (strlen(userSpecFileName) == 0))
	{
		if ((ret = UnionGenerateMakeFileFromTBLDefToSpecFile(tblName,"",conOverwriteModeWhenFileExistsOverwrite)) < 0)
			UnionUserErrLog("in UnionGenerateCFileFromTBLDef:: UnionGenerateMakeFileFromTBLDefToSpecFile for table [%s] failure! ret = [%d]!\n",tblName,ret);
		else
			UnionLog("in UnionGenerateCFileFromTBLDef:: UnionGenerateMakeFileFromTBLDefToSpecFile for table [%s] ok!\n",tblName);
		if ((ret = UnionGenerateMakeFileExcuteScriptFileFromTBLDefToSpecFile(tblName,"",conOverwriteModeWhenFileExistsOverwrite)) < 0)
			UnionUserErrLog("in UnionGenerateCFileFromTBLDef:: UnionGenerateMakeFileExcuteScriptFileFromTBLDefToSpecFile for table [%s] failure! ret = [%d]!\n",tblName,ret);
		else
			UnionLog("in UnionGenerateCFileFromTBLDef:: UnionGenerateMakeFileExcuteScriptFileFromTBLDefToSpecFile for table [%s] ok!\n",tblName);
		if ((ret = UnionComplyAutoGeneratedCFileFromTBLDef(tblName)) < 0)
			UnionUserErrLog("in UnionGenerateCFileFromTBLDef:: UnionComplyAutoGeneratedCFileFromTBLDef for table [%s] failure! ret = [%d]!\n",tblName,ret);
		else
			UnionLog("in UnionGenerateCFileFromTBLDef:: UnionComplyAutoGeneratedCFileFromTBLDef for table [%s] ok!\n",tblName);
	}
	return(ret);
}

/*
����	
	���ݱ��壬����һ��C�ļ���ͷ�ļ�
�������
	tblName			����
	modeWhenFileExists	����ļ��Ѵ��ڵĲ�������
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateIncFileAndCFileFromTBLDef(char *tblName,int modeWhenFileExists)
{
	int	ret;
	
	if ((ret = UnionGenerateIncFileFromTBLDef(tblName,"",modeWhenFileExists)) < 0)
		return(ret);
	return(UnionGenerateCFileFromTBLDef(tblName,"",modeWhenFileExists));
}

/*
����	
	���ݱ��嵥���еļ�¼���������嵥����ͷ�ļ�
�������
	fp		�ļ����
�������
	��
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionGenerateTBLListDefFromTableListTBLToFp(FILE *fp)
{
	FILE			*outFp = stdout;
	int			ret;
	char			fileName[512+1];
	int			recNum = 0;
	FILE			*recFileFp = NULL;
	char			recStr[8192*2+1];
	int			lenOfRecStr;
	char			remark[512+1];
	char			tableName[128+1];
	char			constName[128+1];
	int			resID;
	int			len;
	
	if (fp != NULL)
		outFp = fp;

	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/pid-%d.tmp",getenv("UNIONTEMP"),getpid());

	// �����Զ����ɱ�ʶ
	UnionGenerateAutoCodesTagToFp(outFp);
	// ����궨��
	fprintf(outFp,"#ifndef _%s_\n",conDataTBLListFileName);
	fprintf(outFp,"#define _%s_\n",conDataTBLListFileName);
	fprintf(outFp,"\n");
	
	// �����м�¼
	if ((ret = UnionBatchSelectObjectRecord(conDataTBLListTBLName,"",fileName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateTBLListDefFromTableListTBLToFp:: UnionBatchSelectObjectRecord [%s]!\n",conDataTBLListTBLName);
		return(ret);
	}
	
	// ���ļ�
	if ((recFileFp = fopen(fileName,"r")) == NULL)
	{
		UnionUserErrLog("in UnionGenerateTBLListDefFromTableListTBLToFp:: fopen [%s]\n",fileName);
		goto errExit;
	}
	while (!feof(recFileFp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(recFileFp,recStr,sizeof(recStr))) <= 0)
			continue;
		memset(remark,0,sizeof(remark));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"remark",remark,sizeof(remark))) < 0)
		{
			UnionUserErrLog("in UnionGenerateTBLListDefFromTableListTBLToFp:: UnionReadRecFldFromRecStr [%s] from [%s]\n","remark",recStr);
			goto errExit;
		}
		memset(tableName,0,sizeof(tableName));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"tableName",tableName,sizeof(tableName))) < 0)
		{
			UnionUserErrLog("in UnionGenerateTBLListDefFromTableListTBLToFp:: UnionReadRecFldFromRecStr [%s] from [%s]\n","tableName",recStr);
			goto errExit;
		}
		if (ret == 0)
		{
			UnionUserErrLog("in UnionGenerateTBLListDefFromTableListTBLToFp:: tableName is null in [%s]\n",recStr);
			ret = errCodeMarcoDefNameNotDefined;
			goto errExit;
		}
		if ((ret = UnionReadIntTypeRecFldFromRecStr(recStr,lenOfRecStr,"resID",&resID)) < 0)
		{
			UnionUserErrLog("in UnionGenerateTBLListDefFromTableListTBLToFp:: UnionReadIntTypeRecFldFromRecStr [%s] from [%s]\n","resID",recStr);
			goto errExit;
		}
		if (strlen(remark) != 0)
			fprintf(outFp,"// %s\n",remark);
		memset(constName,0,sizeof(constName));
		UnionFormTBLNameConstNameFromTBLName(tableName,constName);
		if ((len = strlen(constName)) < 64)
			memset(constName+len,' ',64-len);
		fprintf(outFp,"#define %s \"%s\"\n",constName,tableName);
		memset(constName,0,sizeof(constName));
		UnionFormTBLResIDConstNameFromTBLName(tableName,constName);
		if ((len = strlen(constName)) < 64)
			memset(constName+len,' ',64-len);
		fprintf(outFp,"#define %s %d\n",constName,resID);
		recNum++;
	}
	ret = recNum;
	fprintf(outFp,"#endif\n");
errExit:
	fclose(recFileFp);
	UnionDeleteFile(fileName);
	return(ret);
}

/*
����	
	��ñ��嵥����ͷ�ļ�������
�������
	��
�������
	fileName	�ļ���
����ֵ
	>=0	�ɹ�
	<0	������
*/
void UnionGetIncFileNameOfTBLListDef(char *fileName)
{
	sprintf(fileName,"%s/%s.h",getenv("UNIONINCDIR"),conDataTBLListFileName);
	return;
}	

/*
����	
	���ݱ��嵥���еļ�¼���������嵥����ͷ�ļ�
�������
	modeWhenFileExists	����ļ��Ѵ��ڵĲ�������
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateTBLListDefFromTableListTBL(int modeWhenFileExists)
{
	char	fileName[512+1];
	FILE	*fp;
	int	ret;
	
	UnionGetIncFileNameOfTBLListDef(fileName);
	if (!UnionExistsFile(fileName))
		goto writeNow;
	switch (modeWhenFileExists)
	{
		case	conOverwriteModeWhenFileExistsOverwrite:
			break;
		case	conOverwriteModeWhenFileExistsReturn:
			UnionUserErrLog("in UnionGenerateTBLListDefFromTableListTBL:: %s already exists, the include file not generated!\n",fileName);
			return(errCodeFileAlreadyExists);
		default:
			if (!UnionConfirm("%s already exists, overwrite it",fileName))
				return(errCodeFileAlreadyExists);
			break;
	}
	UnionLog("in UnionGenerateTBLListDefFromTableListTBL:: %s already exists, and overwrite it!\n",fileName);
writeNow:
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionUserErrLog("in UnionGenerateTBLListDefFromTableListTBL:: fopen [%s] error!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	if ((ret = UnionGenerateTBLListDefFromTableListTBLToFp(fp)) < 0)
		UnionUserErrLog("in UnionGenerateTBLListDefFromTableListTBL:: Generate table definition list head file failure! ret = [%d]!\n",ret);
	else
		UnionLog("in UnionGenerateTBLListDefFromTableListTBL:: Generate table definition list head file ok!\n");
	fclose(fp);
	return(ret);
}

/*
����	
	���ݱ��嵥���еļ�¼���������б�Ĳ����ļ�
�������
	modeWhenFileExists	����ļ��Ѵ��ڵĲ�������
�������
	��
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionGenerateSrcForAllTBLFromTableListTBL(int modeWhenFileExists)
{
	int			ret;
	char			fileName[512+1];
	int			recNum = 0;
	FILE			*recFileFp = NULL;
	char			recStr[8192*2+1];
	int			lenOfRecStr;
	char			remark[512+1];
	char			tableName[128+1];
	char			constName[128+1];

	// ���ɱ����嵥
	if ((ret = UnionGenerateTBLListDefFromTableListTBL(modeWhenFileExists)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSrcForAllTBLFromTableListTBL:: UnionGenerateTBLListDefFromTableListTBL!\n");
		return(ret);
	}
	
	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/pid-%d.tmp",getenv("UNIONTEMP"),getpid());

	// �����м�¼
	if ((ret = UnionBatchSelectObjectRecord(conDataTBLListTBLName,"",fileName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSrcForAllTBLFromTableListTBL:: UnionBatchSelectObjectRecord [%s]!\n",conDataTBLListTBLName);
		return(ret);
	}
	
	// ���ļ�
	if ((recFileFp = fopen(fileName,"r")) == NULL)
	{
		UnionUserErrLog("in UnionGenerateSrcForAllTBLFromTableListTBL:: fopen [%s]\n",fileName);
		goto errExit;
	}
	while (!feof(recFileFp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(recFileFp,recStr,sizeof(recStr))) <= 0)
			continue;
		memset(remark,0,sizeof(remark));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"remark",remark,sizeof(remark))) < 0)
		{
			UnionUserErrLog("in UnionGenerateSrcForAllTBLFromTableListTBL:: UnionReadRecFldFromRecStr [%s] from [%s]\n","remark",recStr);
			goto errExit;
		}
		memset(tableName,0,sizeof(tableName));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"tableName",tableName,sizeof(tableName))) < 0)
		{
			UnionUserErrLog("in UnionGenerateSrcForAllTBLFromTableListTBL:: UnionReadRecFldFromRecStr [%s] from [%s]\n","tableName",recStr);
			goto errExit;
		}
		if ((ret = UnionGenerateIncFileAndCFileFromTBLDef(tableName,modeWhenFileExists)) < 0)
		{
			UnionUserErrLog("in UnionGenerateSrcForAllTBLFromTableListTBL:: UnionGenerateIncFileAndCFileFromTBLDef!\n");
			continue;
		}
		recNum++;
	}
	ret = recNum;
errExit:
	fclose(recFileFp);
	UnionDeleteFile(fileName);
	return(ret);
}

/*
����	
	���ݱ��еļ�¼������enum��һ����¼��Ӧһ��enumֵ
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionGenerateEnumDefFromTBL(char *tblName,FILE *fp)
{
	FILE			*outFp = stdout;
	int			ret;
	char			fileName[512+1];
	int			recNum = 0;
	FILE			*recFileFp = NULL;
	char			recStr[8192*2+1];
	int			lenOfRecStr;
	char			remark[512+1];
	char			ID[128+1];
	char			macroValue[128+1];
	TUnionObject		objDef;
	int			fldNum;
	PUnionObjectFldDef	pfldDef;
	char			funName[128+1];

	if ((ret = UnionExistsMacroDefInTBL(tblName)) <= 0)
		return(ret);
		
	if (fp != NULL)
		outFp = fp;

	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/pid-%d.tmp",getenv("UNIONTEMP"),getpid());

	// �����м�¼
	if ((ret = UnionBatchSelectObjectRecord(tblName,"",fileName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateEnumDefFromTBL:: UnionBatchSelectObjectRecord [%s]!\n",tblName);
		return(ret);
	}
	
	// ���ļ�
	if ((recFileFp = fopen(fileName,"r")) == NULL)
	{
		UnionUserErrLog("in UnionGenerateEnumDefFromTBL:: fopen [%s]\n",fileName);
		goto errExit;
	}
	while (!feof(recFileFp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(recFileFp,recStr,sizeof(recStr))) <= 0)
			continue;
		memset(remark,0,sizeof(remark));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"remark",remark,sizeof(remark))) < 0)
		{
			UnionUserErrLog("in UnionGenerateEnumDefFromTBL:: UnionReadRecFldFromRecStr [%s] from [%s]\n","remark",recStr);
			goto errExit;
		}
		memset(ID,0,sizeof(ID));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"ID",ID,sizeof(ID))) < 0)
		{
			UnionUserErrLog("in UnionGenerateEnumDefFromTBL:: UnionReadRecFldFromRecStr [%s] from [%s]\n","ID",recStr);
			goto errExit;
		}
		if (ret == 0)
		{
			UnionUserErrLog("in UnionGenerateEnumDefFromTBL:: ID is null in [%s]\n",recStr);
			ret = errCodeMarcoDefNameNotDefined;
			goto errExit;
		}
		memset(macroValue,0,sizeof(macroValue));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"macroValue",macroValue,sizeof(macroValue))) < 0)
		{
			UnionUserErrLog("in UnionGenerateEnumDefFromTBL:: UnionReadRecFldFromRecStr [%s] from [%s]\n","macroValue",recStr);
			ret = errCodeMarcoDefNameNotDefined;
			goto errExit;
		}
		if (ret == 0)
		{
			UnionUserErrLog("in UnionGenerateEnumDefFromTBL:: ID is null in [%s]\n",recStr);
			ret = errCodeMarcoDefValueNotDefined;
			goto errExit;
		}
		fprintf(outFp,"remark=%s|value=%s\n",ID,macroValue);
		recNum++;
	}
	ret = recNum;
	UnionLog("in UnionGenerateEnumDefFromTBL:: recNum = [%d]\n",recNum);
errExit:
	fclose(recFileFp);
	UnionDeleteFile(fileName);
	return(ret);
}

/*
����	
	���ݱ��еļ�¼������enum��һ����¼��Ӧһ��enumֵ
�������
	tblName		����
	fileName	�ļ���
�������
	��
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionGenerateEnumDefFromTBLToSpecFile(char *tblName,char *fileName)
{
	FILE	*fp;
	int	ret;
	
	if ((ret = UnionExistsMacroDefInTBL(tblName)) <= 0)
		return(ret);
		
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionUserErrLog("in UnionGenerateEnumDefFromTBLToSpecFile:: fopen [%s] error!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	if ((ret = UnionGenerateEnumDefFromTBL(tblName,fp)) < 0)
		UnionUserErrLog("in UnionGenerateEnumDefFromTBLToSpecFile:: UnionGenerateEnumDefFromTBL!\n");
	fclose(fp);
	return(ret);
}

/*
����	
	���ݱ��壬����һ���޸�ָ����ĺ���������
�������
	tblName		����
	isDeclaration	1��ʾ��������0��ʾ�Ǻ�����
	funName		��������
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateUpdateSpecFldFunDeclarationFromTBLDef(char *tblName,int isDeclaration,char *funName,FILE *fp)
{
	FILE			*outFp = stdout;
	int			ret;
	
	if (tblName == NULL)
		return(errCodeParameter);
		
	if (fp != NULL)
		outFp = fp;
	
	// ��������
	UnionFormUpdateSpecFldRecFunNameFromTBLName(tblName,funName);
	fprintf(outFp,"/*\n");
	fprintf(outFp,"��������:\n");
	fprintf(outFp,"	�޸�ָ���ı���:\n");
	fprintf(outFp,"�������:\n");
	if ((ret = UnionGenerateSpecRecPrimaryKeyRemarkAsVarFunToFp(tblName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateUpdateSpecFldFunDeclarationFromTBLDef:: UnionGenerateSpecRecPrimaryKeyRemarkAsVarFunToFp [%s]!\n",tblName);
		return(ret);
	}
	fprintf(outFp,"	fldName ������\n");
	fprintf(outFp,"	fldValue ��ֵ\n");
	fprintf(outFp,"	fldValueLen ��ֵ����\n");
	fprintf(outFp,"�������:\n");
	fprintf(outFp,"	��\n");
	fprintf(outFp,"����ֵ:\n");
	fprintf(outFp,"	>=0 �ɹ�\n");
	fprintf(outFp,"	<0 ʧ��\n");
	fprintf(outFp,"*/\n");
	fprintf(outFp,"int %s(",funName);
	if ((ret = UnionGenerateSpecRecPrimaryKeyAsVarFunToFp(tblName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateUpdateSpecFldFunDeclarationFromTBLDef:: UnionGenerateSpecRecPrimaryKeyAsVarFunToFp [%s]!\n",tblName);
		return(ret);
	}
	fprintf(outFp,",char *fldName,char *fldValue,int lenOfFldValue)");
	if (isDeclaration)
		fprintf(outFp,";\n");
	else
		fprintf(outFp,"\n");
	return(0);
}

/*
����	
	���ݱ��壬����һ�����ݽṹȱʡ��ֵ����
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateUpdateSpecFldFunFromTBLDef(char *tblName,FILE *fp)
{
	FILE			*outFp = stdout;
	int			ret;
	char			funName[128+1];
	char			tblNameConst[128+1];
	
	if (tblName == NULL)
		return(errCodeParameter);
		
	if (fp != NULL)
		outFp = fp;
	
	// ��������
	if ((ret = UnionGenerateUpdateSpecFldFunDeclarationFromTBLDef(tblName,0,funName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateUpdateSpecFldFunFromTBLDef:: UnionGenerateUpdateSpecFldFunDeclarationFromTBLDef [%s]!\n",tblName);
		return(UnionSetUserDefinedErrorCode(ret));
	}
	// ������
	fprintf(outFp,"{\n");
	// �ֲ���������
	fprintf(outFp,"	int	ret;\n");
	fprintf(outFp,"	char	recStr[8192*2+1];\n");
	fprintf(outFp,"	char	primaryKey[1024+1];\n");
	fprintf(outFp,"	int	lenOfRecStr = 0;\n");
	fprintf(outFp,"\n");
	if ((ret = UnionGenerateSpecRecPrimaryKeyIntoRecStrToFp(funName,tblName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateUpdateSpecFldFunFromTBLDef:: UnionGenerateSpecRecPrimaryKeyIntoRecStrToFp [%s]!\n",tblName);
		return(UnionSetUserDefinedErrorCode(ret));
	}
	fprintf(outFp,"	strcpy(primaryKey,recStr);\n");
	fprintf(outFp,"\n");
	fprintf(outFp,"	// ���¼\n");
	fprintf(outFp,"	memset(recStr,0,sizeof(recStr));\n");
	fprintf(outFp,"	lenOfRecStr = 0;\n");
	fprintf(outFp,"	if ((ret = UnionPutRecFldIntoRecStr(fldName,fldValue,lenOfFldValue,recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr)) < 0)\n");
	fprintf(outFp,"	{\n");
	fprintf(outFp,"		UnionUserErrLog(\"in %s:: UnionPutRecFldIntoRecStr [%%s] failure!\\n\",fldName);\n",funName);
	fprintf(outFp,"		return(UnionSetUserDefinedErrorCode(ret));\n");
	fprintf(outFp,"	}\n");
	fprintf(outFp,"	lenOfRecStr += ret;\n");
	fprintf(outFp,"\n");
	fprintf(outFp,"	//�����Զ��޸���\n");
	fprintf(outFp,"	if ((lenOfRecStr = UnionAutoAppendDBRecUpdateAttr(recStr,lenOfRecStr,sizeof(recStr))) < 0)\n");
	fprintf(outFp,"	{\n");
	fprintf(outFp,"		UnionUserErrLog(\"in %s:: UnionAutoAppendDBRecUpdateAttr!\\n\");\n",funName);
	fprintf(outFp,"		return(UnionSetUserDefinedErrorCode(lenOfRecStr));\n");
	fprintf(outFp,"	}\n");
	fprintf(outFp,"\n");
	fprintf(outFp,"	// �޸ļ�¼\n");
	UnionFormTBLNameConstNameFromTBLName(tblName,tblNameConst);
	fprintf(outFp,"	if ((ret = UnionUpdateUniqueObjectRecord(%s,primaryKey,recStr,lenOfRecStr)) < 0)\n",tblNameConst);
	fprintf(outFp,"	{\n");
	fprintf(outFp,"		UnionUserErrLog(\"in %s:: UnionUpdateUniqueObjectRecord!\\n\");\n",funName);
	fprintf(outFp,"		return(UnionSetUserDefinedErrorCode(ret));\n");
	fprintf(outFp,"	}\n");
	fprintf(outFp,"\n");
	fprintf(outFp,"	return(ret);\n");
	fprintf(outFp,"}\n\n");
}

/*
����	
	���ݱ��壬����һ���޸�ָ����ĺ���������
�������
	tblName		����
	isDeclaration	1��ʾ��������0��ʾ�Ǻ�����
	funName		��������
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateUpdateIntTypeSpecFldFunDeclarationFromTBLDef(char *tblName,int isDeclaration,char *funName,FILE *fp)
{
	FILE			*outFp = stdout;
	int			ret;
	
	if (tblName == NULL)
		return(errCodeParameter);
		
	if (fp != NULL)
		outFp = fp;
	
	// ��������
	UnionFormUpdateIntTypeSpecFldRecFunNameFromTBLName(tblName,funName);
	fprintf(outFp,"/*\n");
	fprintf(outFp,"��������:\n");
	fprintf(outFp,"	�޸�ָ���ı���:\n");
	fprintf(outFp,"�������:\n");
	if ((ret = UnionGenerateSpecRecPrimaryKeyRemarkAsVarFunToFp(tblName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateUpdateIntTypeSpecFldFunDeclarationFromTBLDef:: UnionGenerateSpecRecPrimaryKeyRemarkAsVarFunToFp [%s]!\n",tblName);
		return(ret);
	}
	fprintf(outFp,"	fldName ������\n");
	fprintf(outFp,"	fldValue ��ֵ\n");
	fprintf(outFp,"�������:\n");
	fprintf(outFp,"	��\n");
	fprintf(outFp,"����ֵ:\n");
	fprintf(outFp,"	>=0 �ɹ�\n");
	fprintf(outFp,"	<0 ʧ��\n");
	fprintf(outFp,"*/\n");
	fprintf(outFp,"int %s(",funName);
	if ((ret = UnionGenerateSpecRecPrimaryKeyAsVarFunToFp(tblName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateUpdateIntTypeSpecFldFunDeclarationFromTBLDef:: UnionGenerateSpecRecPrimaryKeyAsVarFunToFp [%s]!\n",tblName);
		return(ret);
	}
	fprintf(outFp,",char *fldName,int fldValue)");
	if (isDeclaration)
		fprintf(outFp,";\n");
	else
		fprintf(outFp,"\n");
	return(0);
}

/*
����	
	���ݱ��壬����һ�����ݽṹȱʡ��ֵ����
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateUpdateIntTypeSpecFldFunFromTBLDef(char *tblName,FILE *fp)
{
	FILE			*outFp = stdout;
	int			ret;
	char			funName[128+1];
	char			tblNameConst[128+1];
	
	if (tblName == NULL)
		return(errCodeParameter);
		
	if (fp != NULL)
		outFp = fp;
	
	// ��������
	if ((ret = UnionGenerateUpdateIntTypeSpecFldFunDeclarationFromTBLDef(tblName,0,funName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateUpdateIntTypeSpecFldFunFromTBLDef:: UnionGenerateUpdateIntTypeSpecFldFunDeclarationFromTBLDef [%s]!\n",tblName);
		return(UnionSetUserDefinedErrorCode(ret));
	}
	// ������
	fprintf(outFp,"{\n");
	// �ֲ���������
	fprintf(outFp,"	int	ret;\n");
	fprintf(outFp,"	char	recStr[8192*2+1];\n");
	fprintf(outFp,"	char	primaryKey[1024+1];\n");
	fprintf(outFp,"	int	lenOfRecStr = 0;\n");
	fprintf(outFp,"\n");
	if ((ret = UnionGenerateSpecRecPrimaryKeyIntoRecStrToFp(funName,tblName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateUpdateIntTypeSpecFldFunFromTBLDef:: UnionGenerateSpecRecPrimaryKeyIntoRecStrToFp [%s]!\n",tblName);
		return(UnionSetUserDefinedErrorCode(ret));
	}
	fprintf(outFp,"	strcpy(primaryKey,recStr);\n");
	fprintf(outFp,"\n");
	fprintf(outFp,"	// ���¼\n");
	fprintf(outFp,"	memset(recStr,0,sizeof(recStr));\n");
	fprintf(outFp,"	lenOfRecStr = 0;\n");
	fprintf(outFp,"	if ((ret = UnionPutIntTypeRecFldIntoRecStr(fldName,fldValue,recStr+lenOfRecStr,sizeof(recStr)-lenOfRecStr)) < 0)\n");
	fprintf(outFp,"	{\n");
	fprintf(outFp,"		UnionUserErrLog(\"in %s:: UnionPutIntTypeRecFldIntoRecStr [%%s] failure!\\n\",fldName);\n",funName);
	fprintf(outFp,"		return(UnionSetUserDefinedErrorCode(ret));\n");
	fprintf(outFp,"	}\n");
	fprintf(outFp,"	lenOfRecStr += ret;\n");
	fprintf(outFp,"\n");
	fprintf(outFp,"	//�����Զ��޸���\n");
	fprintf(outFp,"	if ((lenOfRecStr = UnionAutoAppendDBRecUpdateAttr(recStr,lenOfRecStr,sizeof(recStr))) < 0)\n");
	fprintf(outFp,"	{\n");
	fprintf(outFp,"		UnionUserErrLog(\"in %s:: UnionAutoAppendDBRecUpdateAttr!\\n\");\n",funName);
	fprintf(outFp,"		return(UnionSetUserDefinedErrorCode(lenOfRecStr));\n");
	fprintf(outFp,"	}\n");
	fprintf(outFp,"\n");
	fprintf(outFp,"	// �޸ļ�¼\n");
	UnionFormTBLNameConstNameFromTBLName(tblName,tblNameConst);
	fprintf(outFp,"	if ((ret = UnionUpdateUniqueObjectRecord(%s,primaryKey,recStr,lenOfRecStr)) < 0)\n",tblNameConst);
	fprintf(outFp,"	{\n");
	fprintf(outFp,"		UnionUserErrLog(\"in %s:: UnionUpdateUniqueObjectRecord!\\n\");\n",funName);
	fprintf(outFp,"		return(UnionSetUserDefinedErrorCode(ret));\n");
	fprintf(outFp,"	}\n");
	fprintf(outFp,"\n");
	fprintf(outFp,"	return(ret);\n");
	fprintf(outFp,"}\n\n");
}

/*
����	
	���ݱ��壬����һ����ȡ���м�¼����ĺ���������
�������
	tblName		����
	includeFunBody	�Ƿ���������壬1,������0������
	fp		�ļ����
�������
	funName		�����ĺ�������
����ֵ
	��
*/
void UnionGenerateSelectRecFldFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp)
{
	int	ret;
	
	UnionFormReadRecFldFunNameFromTBLName(tblName,funName);
	fprintf(outFp,"\n");
	fprintf(outFp,"\n/*\n���ܣ�\n");
	fprintf(outFp,"	��ȡһ����¼\n");
	fprintf(outFp,"�������:\n");
	if ((ret = UnionGenerateSpecRecPrimaryKeyRemarkAsVarFunToFp(tblName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSelectRecFldFunDeclarationFromTBLDef:: UnionGenerateSpecRecPrimaryKeyRemarkAsVarFunToFp [%s]\n",tblName);
		return;
	}
	fprintf(outFp,"	fldName		����\n");
	fprintf(outFp,"	sizeOfBuf	��ֵ�����С\n");
	fprintf(outFp,"�������:\n");
	fprintf(outFp,"	fldValue	��ֵ\n");
	fprintf(outFp,"����ֵ\n");
	fprintf(outFp,"	>=0		�ɹ���������ֵ�ĳ���\n");
	fprintf(outFp,"	<0		ʧ��,������\n");
	fprintf(outFp,"*/\n");
	fprintf(outFp,"int %s(",funName);
	if ((ret = UnionGenerateSpecRecPrimaryKeyAsVarFunToFp(tblName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSelectRecFldFunDeclarationFromTBLDef:: UnionGenerateSpecRecPrimaryKeyAsVarFunToFp [%s]\n",tblName);
		return;
	}
	fprintf(outFp,",char *fldName,char *fldValue,int sizeOfBuf)");
	if (!includeFunBody)
		fprintf(outFp,";\n");
	else
		fprintf(outFp,"\n");
	return;
}

/*
����	
	���ݱ��壬����һ����ȡ���м�¼�ĺ���
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateSelectRecFldFunFromTBLDef(char *tblName,FILE *fp)
{
	FILE			*outFp = stdout;
	int			ret;
	char			funName[128+1];
	char			tblNameConst[128+1];
	
	if (tblName == NULL)
		return(errCodeParameter);
		
	if (fp != NULL)
		outFp = fp;
	
	// ��������
	UnionGenerateSelectRecFldFunDeclarationFromTBLDef(tblName,1,funName,fp);
	// ������
	fprintf(outFp,"{\n");
	// �ֲ���������
	fprintf(outFp,"	int	ret;\n");
	fprintf(outFp,"	int	lenOfRecStr = 0;\n");
	fprintf(outFp,"	char	recStr[8192*4+1];\n");
	fprintf(outFp,"	char	condition[1024+1];\n");
	fprintf(outFp,"\n");
	// �����ж�
	fprintf(outFp,"	//�Ӽ�¼ƴװ��\n");
	if ((ret = UnionGenerateSpecRecPrimaryKeyIntoRecStrToFp(funName,tblName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSelectRecFldFunFromTBLDef:: UnionGenerateSpecRecPrimaryKeyIntoRecStrToFp!\n");
		return(ret);
	}
	fprintf(outFp,"	if (lenOfRecStr >= sizeof(condition))\n");
	fprintf(outFp,"	{\n");
	fprintf(outFp,"		UnionUserErrLog(\"in %s:: lenOfCondition = [%%d] too long!\\n\",lenOfRecStr);\n",funName);
	fprintf(outFp,"		return(ret);\n");
	fprintf(outFp,"	}\n");
	fprintf(outFp,"	memset(condition,0,sizeof(condition));\n");
	fprintf(outFp,"	memcpy(condition,recStr,lenOfRecStr);\n");
	fprintf(outFp,"\n");
	fprintf(outFp,"	//��ȡ��¼\n");
	UnionFormTBLNameConstNameFromTBLName(tblName,tblNameConst);
	fprintf(outFp,"	memset(recStr,0,sizeof(recStr));\n");
	fprintf(outFp,"	if ((lenOfRecStr = UnionSelectUniqueObjectRecordByPrimaryKey(%s,condition,recStr,sizeof(recStr))) < 0)\n",tblNameConst);
	fprintf(outFp,"	{\n");
	fprintf(outFp,"		UnionUserErrLog(\"in %s:: UnionSelectUniqueObjectRecordByPrimaryKey!\\n\");\n",funName);
	fprintf(outFp,"		return(UnionSetUserDefinedErrorCode(lenOfRecStr));\n");
	fprintf(outFp,"	}\n");
	fprintf(outFp,"\n");
	
	fprintf(outFp,"	//��ָ����\n");
	fprintf(outFp,"	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,fldName,fldValue,sizeOfBuf)) < 0)\n");
	fprintf(outFp,"	{\n");
	fprintf(outFp,"		UnionUserErrLog(\"in %s:: UnionReadRecFldFromRecStr!\\n\");\n",funName);
	fprintf(outFp,"		return(UnionSetUserDefinedErrorCode(ret));\n");
	fprintf(outFp,"	}\n");
	fprintf(outFp,"\n");
	
	fprintf(outFp,"	return(ret);\n");
	fprintf(outFp,"}\n");
	return(0);
}

/*
����	
	���ݱ��壬����һ����ȡ���м�¼����ĺ���������
�������
	tblName		����
	includeFunBody	�Ƿ���������壬1,������0������
	fp		�ļ����
�������
	funName		�����ĺ�������
����ֵ
	��
*/
void UnionGenerateSelectRecIntTypeFldFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp)
{
	int	ret;
	
	UnionFormReadRecIntTypeFldFunNameFromTBLName(tblName,funName);
	fprintf(outFp,"\n");
	fprintf(outFp,"\n/*\n���ܣ�\n");
	fprintf(outFp,"	��ȡһ����¼\n");
	fprintf(outFp,"�������:\n");
	if ((ret = UnionGenerateSpecRecPrimaryKeyRemarkAsVarFunToFp(tblName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSelectRecIntTypeFldFunDeclarationFromTBLDef:: UnionGenerateSpecRecPrimaryKeyRemarkAsVarFunToFp [%s]\n",tblName);
		return;
	}
	fprintf(outFp,"	fldName		����\n");
	fprintf(outFp,"�������:\n");
	fprintf(outFp,"	fldValue	��ֵ\n");
	fprintf(outFp,"����ֵ\n");
	fprintf(outFp,"	>=0		�ɹ���������ֵ�ĳ���\n");
	fprintf(outFp,"	<0		ʧ��,������\n");
	fprintf(outFp,"*/\n");
	fprintf(outFp,"int %s(",funName);
	if ((ret = UnionGenerateSpecRecPrimaryKeyAsVarFunToFp(tblName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSelectRecIntTypeFldFunDeclarationFromTBLDef:: UnionGenerateSpecRecPrimaryKeyAsVarFunToFp [%s]\n",tblName);
		return;
	}
	fprintf(outFp,",char *fldName,int *fldValue)");
	if (!includeFunBody)
		fprintf(outFp,";\n");
	else
		fprintf(outFp,"\n");
	return;
}

/*
����	
	���ݱ��壬����һ����ȡ���м�¼�ĺ���
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateSelectRecIntTypeFldFunFromTBLDef(char *tblName,FILE *fp)
{
	FILE			*outFp = stdout;
	int			ret;
	char			funName[128+1];
	char			tblNameConst[128+1];
	
	if (tblName == NULL)
		return(errCodeParameter);
		
	if (fp != NULL)
		outFp = fp;
	
	// ��������
	UnionGenerateSelectRecIntTypeFldFunDeclarationFromTBLDef(tblName,1,funName,fp);
	// ������
	fprintf(outFp,"{\n");
	// �ֲ���������
	fprintf(outFp,"	int	ret;\n");
	fprintf(outFp,"	int	lenOfRecStr = 0;\n");
	fprintf(outFp,"	char	recStr[8192*4+1];\n");
	fprintf(outFp,"	char	condition[1024+1];\n");
	fprintf(outFp,"\n");
	// �����ж�
	fprintf(outFp,"	//�Ӽ�¼ƴװ��\n");
	if ((ret = UnionGenerateSpecRecPrimaryKeyIntoRecStrToFp(funName,tblName,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSelectRecIntTypeFldFunFromTBLDef:: UnionGenerateSpecRecPrimaryKeyIntoRecStrToFp!\n");
		return(ret);
	}
	fprintf(outFp,"	if (lenOfRecStr >= sizeof(condition))\n");
	fprintf(outFp,"	{\n");
	fprintf(outFp,"		UnionUserErrLog(\"in %s:: lenOfCondition = [%%d] too long!\\n\",lenOfRecStr);\n",funName);
	fprintf(outFp,"		return(ret);\n");
	fprintf(outFp,"	}\n");
	fprintf(outFp,"	memset(condition,0,sizeof(condition));\n");
	fprintf(outFp,"	memcpy(condition,recStr,lenOfRecStr);\n");
	fprintf(outFp,"\n");
	fprintf(outFp,"	//��ȡ��¼\n");
	UnionFormTBLNameConstNameFromTBLName(tblName,tblNameConst);
	fprintf(outFp,"	memset(recStr,0,sizeof(recStr));\n");
	fprintf(outFp,"	if ((lenOfRecStr = UnionSelectUniqueObjectRecordByPrimaryKey(%s,condition,recStr,sizeof(recStr))) < 0)\n",tblNameConst);
	fprintf(outFp,"	{\n");
	fprintf(outFp,"		UnionUserErrLog(\"in %s:: UnionSelectUniqueObjectRecordByPrimaryKey!\\n\");\n",funName);
	fprintf(outFp,"		return(UnionSetUserDefinedErrorCode(lenOfRecStr));\n");
	fprintf(outFp,"	}\n");
	fprintf(outFp,"\n");
	
	fprintf(outFp,"	//��ָ����\n");
	fprintf(outFp,"	if ((ret = UnionReadIntTypeRecFldFromRecStr(recStr,lenOfRecStr,fldName,fldValue)) < 0)\n");
	fprintf(outFp,"	{\n");
	fprintf(outFp,"		UnionUserErrLog(\"in %s:: UnionReadIntTypeRecFldFromRecStr!\\n\");\n",funName);
	fprintf(outFp,"		return(UnionSetUserDefinedErrorCode(ret));\n");
	fprintf(outFp,"	}\n");
	fprintf(outFp,"\n");
	
	fprintf(outFp,"	return(ret);\n");
	fprintf(outFp,"}\n");
	return(0);
}


/*
����	
	���ݲ����嵥���еļ�¼��������������ͷ�ļ�
�������
	fp		�ļ����
�������
	��
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionGenerateTBLCmdDefFromTBLDefToFp(FILE *fp)
{
	FILE			*outFp = stdout;
	int			ret;
	char			fileName[512+1];
	int			recNum = 0;
	FILE			*recFileFp = NULL;
	char			recStr[8192*2+1];
	int			lenOfRecStr;
	char			remark[512+1];
	char			macroName[128+1];
	char			macroValue[128+1];
	int			len;
	
	if (fp != NULL)
		outFp = fp;

	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/pid-%d.tmp",getenv("UNIONTEMP"),getpid());

	// �����Զ����ɱ�ʶ
	UnionGenerateAutoCodesTagToFp(outFp);
	// ����궨��
	fprintf(outFp,"#ifndef _%s_\n","operationList");
	fprintf(outFp,"#define _%s_\n","operationList");
	fprintf(outFp,"\n");
	
	// �����м�¼
	if ((ret = UnionBatchSelectObjectRecord("operationList","",fileName)) < 0)
	{
		UnionUserErrLog("in UnionGenerateTBLCmdDefFromTBLDefToFp:: UnionBatchSelectObjectRecord [%s]!\n","operationList");
		return(ret);
	}
	
	// ���ļ�
	if ((recFileFp = fopen(fileName,"r")) == NULL)
	{
		UnionUserErrLog("in UnionGenerateTBLCmdDefFromTBLDefToFp:: fopen [%s]\n",fileName);
		goto errExit;
	}
	while (!feof(recFileFp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(recFileFp,recStr,sizeof(recStr))) <= 0)
			continue;
		memset(remark,0,sizeof(remark));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"remark",remark,sizeof(remark))) < 0)
		{
			UnionUserErrLog("in UnionGenerateTBLCmdDefFromTBLDefToFp:: UnionReadRecFldFromRecStr [%s] from [%s]\n","remark",recStr);
			goto errExit;
		}
		memset(macroName,0,sizeof(macroName));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"macroName",macroName,sizeof(macroName))) < 0)
		{
			UnionUserErrLog("in UnionGenerateTBLCmdDefFromTBLDefToFp:: UnionReadRecFldFromRecStr [%s] from [%s]\n","macroName",recStr);
			goto errExit;
		}
		if (ret == 0)
		{
			UnionUserErrLog("in UnionGenerateTBLCmdDefFromTBLDefToFp:: macroName is null in [%s]\n",recStr);
			ret = errCodeMarcoDefNameNotDefined;
			goto errExit;
		}
		memset(macroValue,0,sizeof(macroValue));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"macroValue",macroValue,sizeof(macroValue))) < 0)
		{
			UnionUserErrLog("in UnionGenerateTBLCmdDefFromTBLDefToFp:: UnionReadRecFldFromRecStr [%s] from [%s]\n","macroValue",recStr);
			goto errExit;
		}
		if (ret == 0)
		{
			UnionUserErrLog("in UnionGenerateTBLCmdDefFromTBLDefToFp:: macroValue is null in [%s]\n",recStr);
			ret = errCodeMarcoDefNameNotDefined;
			goto errExit;
		}
		if (strlen(remark) != 0)
			fprintf(outFp,"// %s\n",remark);
		if ((len = strlen(macroName)) < 64)
			memset(macroName+len,' ',64-len);
		if (UnionIsDigitStr(macroValue))
			fprintf(outFp,"#define %s %d\n",macroName,atoi(macroValue));
		else
			fprintf(outFp,"#define %s (%s)\n",macroName,macroValue);
		recNum++;
	}
	ret = recNum;
	fprintf(outFp,"#endif\n");
errExit:
	fclose(recFileFp);
	UnionDeleteFile(fileName);
	return(ret);
}

/*
����	
	��ñ��������ͷ�ļ�������
�������
	��
�������
	fileName	�ļ���
����ֵ
	>=0	�ɹ�
	<0	������
*/
void UnionGetIncFileNameOfTBLCmdDef(char *fileName)
{
	sprintf(fileName,"%s/%s.h",getenv("UNIONINCDIR"),"operationList");
	return;
}	

/*
����	
	���ݲ���������еļ�¼��������������ͷ�ļ�
�������
	��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateTBLCmdDefFromTBLDef()
{
	char	fileName[512+1];
	FILE	*fp;
	int	ret;
	
	UnionGetIncFileNameOfTBLCmdDef(fileName);
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionUserErrLog("in UnionGenerateTBLCmdDefFromTBLDef:: fopen [%s] error!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	if ((ret = UnionGenerateTBLCmdDefFromTBLDefToFp(fp)) < 0)
		UnionUserErrLog("in UnionGenerateTBLCmdDefFromTBLDef:: UnionGenerateTBLCmdDefFromTBLDefToFp failure! ret = [%d]!\n",ret);
	else
		UnionLog("in UnionGenerateTBLCmdDefFromTBLDef:: UnionGenerateTBLCmdDefFromTBLDefToFp ok!\n");
	fclose(fp);
	return(ret);
}


/*
����	
	���ݱ��壬����һ������¼�ṹ�йؼ���ת��Ϊ��¼�ִ��ĺ���
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateConvertPrimaryKeyInStructIntoRecStrFunFromTBLDef(char *tblName,FILE *fp)
{
	FILE			*outFp = stdout;
	int			ret;
	TUnionObject		objDef;
	int			fldNum;
	PUnionObjectFldDef	pfldDef;
	char			fldDefName[128+1];
	char			funName[128+1];
	char			fldFunName[128+1];
	
	if (tblName == NULL)
		return(errCodeParameter);
		
	if (fp != NULL)
		outFp = fp;
	
	// ������
	memset(&objDef,0,sizeof(objDef));
	if ((ret = UnionReadObjectDefFromDesignDoc(tblName,&objDef)) < 0)
	{
		UnionUserErrLog("in UnionGenerateConvertPrimaryKeyInStructIntoRecStrFunFromTBLDef:: UnionReadObjectDefFromDesignDoc [%s]!\n",tblName);
		return(ret);
	}
	// ��������	
	UnionGenerateConvertPrimaryKeyInStructIntoRecStrFunDeclarationFromTBLDef(tblName,1,funName,outFp);
	// ������
	fprintf(outFp,"{\n");
	// �ֲ���������
	fprintf(outFp,"	int	ret;\n");
	fprintf(outFp,"	int	offset = 0;\n");
	fprintf(outFp,"\n");
	// �����ж�
	fprintf(outFp,"	if ((prec == NULL) || (recStr == NULL))\n");
	fprintf(outFp,"	{\n");
	fprintf(outFp,"		UnionUserErrLog(\"in %s:: prec or recStr is null!\\n\");\n",funName);
	fprintf(outFp,"		return(UnionSetUserDefinedErrorCode(errCodeNullPointer));\n");
	fprintf(outFp,"	}\n");

	for (fldNum = 0; fldNum < objDef.fldNum; fldNum++)
	{
		pfldDef = &(objDef.fldDefGrp[fldNum]);
		if (!UnionIsPrimaryKeyFldOfObject(&objDef,pfldDef->name))
			continue;
		UnionFormStructFldDefNameFromTBLName(objDef.name,pfldDef->name,fldDefName);
		// �������
		switch (pfldDef->type)
		{
			case	conObjectFldType_String:
				if (pfldDef->size > 1)
				{
					fprintf(outFp,"	ret = UnionPutRecFldIntoRecStr(%s,prec->%s,strlen(prec->%s),recStr+offset,sizeOfBuf-offset);\n",fldDefName,pfldDef->name,pfldDef->name);
					strcpy(fldFunName,"UnionPutRecFldIntoRecStr");
				}
				else
				{
					fprintf(outFp,"	ret = UnionPutCharTypeRecFldIntoRecStr(%s,prec->%s,recStr+offset,sizeOfBuf-offset);\n",fldDefName,pfldDef->name);
					strcpy(fldFunName,"UnionPutCharTypeRecFldIntoRecStr");
				}
				break;
			case	conObjectFldType_Int:
			case	conObjectFldType_Bool:
				fprintf(outFp,"	ret = UnionPutIntTypeRecFldIntoRecStr(%s,prec->%s,recStr+offset,sizeOfBuf-offset);\n",fldDefName,pfldDef->name);
				strcpy(fldFunName,"UnionPutIntTypeRecFldIntoRecStr");
				break;
			case	conObjectFldType_Double:
				fprintf(outFp,"	ret = UnionPutDoubleTypeRecFldIntoRecStr(%s,prec->%s,recStr+offset,sizeOfBuf-offset);\n",fldDefName,pfldDef->name);
				strcpy(fldFunName,"UnionPutDoubleTypeRecFldIntoRecStr");
				break;
			case	conObjectFldType_Bit:
				fprintf(outFp,"	ret = UnionPutBitsTypeRecFldIntoRecStr(%s,prec->%s,sizeof(prec->%s),recStr+offset,sizeOfBuf-offset);\n",fldDefName,pfldDef->name,pfldDef->name);
				strcpy(fldFunName,"UnionPutBitsTypeRecFldIntoRecStr");
				break;
			default:
				fprintf(outFp,"	ret = UnionPutBitsTypeRecFldIntoRecStr(%s,prec->%s,sizeof(prec->%s),recStr+offset,sizeOfBuf-offset);\n",fldDefName,pfldDef->name,pfldDef->name);
				strcpy(fldFunName,"UnionPutBitsTypeRecFldIntoRecStr");
				break;
		}
		fprintf(outFp,"	if (ret < 0)\n");
		fprintf(outFp,"	{\n");
		fprintf(outFp,"		UnionUserErrLog(\"in %s:: %s [%s] error!\\n\");\n",funName,fldFunName,pfldDef->name);
		fprintf(outFp,"		return(UnionSetUserDefinedErrorCode(ret));\n");
		fprintf(outFp,"	}\n");
		fprintf(outFp,"	offset += ret;\n");
	}
	fprintf(outFp,"	return(offset);\n");
	fprintf(outFp,"}\n");
	return(0);
}


/*
����	
	���ݱ��壬����һ������¼�ṹ�еķǹؼ���ת��Ϊ��¼�ִ��ĺ���
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateConvertNonPrimaryKeyInStructIntoRecStrFunFromTBLDef(char *tblName,FILE *fp)
{
	FILE			*outFp = stdout;
	int			ret;
	TUnionObject		objDef;
	int			fldNum;
	PUnionObjectFldDef	pfldDef;
	char			fldDefName[128+1];
	char			funName[128+1];
	char			fldFunName[128+1];
	
	if (tblName == NULL)
		return(errCodeParameter);
		
	if (fp != NULL)
		outFp = fp;
	
	// ������
	memset(&objDef,0,sizeof(objDef));
	if ((ret = UnionReadObjectDefFromDesignDoc(tblName,&objDef)) < 0)
	{
		UnionUserErrLog("in UnionGenerateConvertNonPrimaryKeyInStructIntoRecStrFunFromTBLDef:: UnionReadObjectDefFromDesignDoc [%s]!\n",tblName);
		return(ret);
	}
	// ��������	
	UnionGenerateConvertNonPrimaryKeyInStructIntoRecStrFunDeclarationFromTBLDef(tblName,1,funName,outFp);
	// ������
	fprintf(outFp,"{\n");
	// �ֲ���������
	fprintf(outFp,"	int	ret;\n");
	fprintf(outFp,"	int	offset = 0;\n");
	fprintf(outFp,"\n");
	// �����ж�
	fprintf(outFp,"	if ((prec == NULL) || (recStr == NULL))\n");
	fprintf(outFp,"	{\n");
	fprintf(outFp,"		UnionUserErrLog(\"in %s:: prec or recStr is null!\\n\");\n",funName);
	fprintf(outFp,"		return(UnionSetUserDefinedErrorCode(errCodeNullPointer));\n");
	fprintf(outFp,"	}\n");

	for (fldNum = 0; fldNum < objDef.fldNum; fldNum++)
	{
		pfldDef = &(objDef.fldDefGrp[fldNum]);
		if (UnionIsPrimaryKeyFldOfObject(&objDef,pfldDef->name))
			continue;
		UnionFormStructFldDefNameFromTBLName(objDef.name,pfldDef->name,fldDefName);
		// �������
		switch (pfldDef->type)
		{
			case	conObjectFldType_String:
				if (pfldDef->size > 1)
				{
					fprintf(outFp,"	ret = UnionPutRecFldIntoRecStr(%s,prec->%s,strlen(prec->%s),recStr+offset,sizeOfBuf-offset);\n",fldDefName,pfldDef->name,pfldDef->name);
					strcpy(fldFunName,"UnionPutRecFldIntoRecStr");
				}
				else
				{
					fprintf(outFp,"	ret = UnionPutCharTypeRecFldIntoRecStr(%s,prec->%s,recStr+offset,sizeOfBuf-offset);\n",fldDefName,pfldDef->name);
					strcpy(fldFunName,"UnionPutCharTypeRecFldIntoRecStr");
				}
				break;
			case	conObjectFldType_Int:
			case	conObjectFldType_Bool:
				fprintf(outFp,"	ret = UnionPutIntTypeRecFldIntoRecStr(%s,prec->%s,recStr+offset,sizeOfBuf-offset);\n",fldDefName,pfldDef->name);
				strcpy(fldFunName,"UnionPutIntTypeRecFldIntoRecStr");
				break;
			case	conObjectFldType_Double:
				fprintf(outFp,"	ret = UnionPutDoubleTypeRecFldIntoRecStr(%s,prec->%s,recStr+offset,sizeOfBuf-offset);\n",fldDefName,pfldDef->name);
				strcpy(fldFunName,"UnionPutDoubleTypeRecFldIntoRecStr");
				break;
			case	conObjectFldType_Bit:
				fprintf(outFp,"	ret = UnionPutBitsTypeRecFldIntoRecStr(%s,prec->%s,sizeof(prec->%s),recStr+offset,sizeOfBuf-offset);\n",fldDefName,pfldDef->name,pfldDef->name);
				strcpy(fldFunName,"UnionPutBitsTypeRecFldIntoRecStr");
				break;
			default:
				fprintf(outFp,"	ret = UnionPutBitsTypeRecFldIntoRecStr(%s,prec->%s,sizeof(prec->%s),recStr+offset,sizeOfBuf-offset);\n",fldDefName,pfldDef->name,pfldDef->name);
				strcpy(fldFunName,"UnionPutBitsTypeRecFldIntoRecStr");
				break;
		}
		fprintf(outFp,"	if (ret < 0)\n");
		fprintf(outFp,"	{\n");
		fprintf(outFp,"		UnionUserErrLog(\"in %s:: %s [%s] error!\\n\");\n",funName,fldFunName,pfldDef->name);
		fprintf(outFp,"		return(UnionSetUserDefinedErrorCode(ret));\n");
		fprintf(outFp,"	}\n");
		fprintf(outFp,"	offset += ret;\n");
	}
	fprintf(outFp,"	return(offset);\n");
	fprintf(outFp,"}\n");
	return(0);
}

/*
����	
	���ݱ��壬����һ������¼��ת��Ϊ��¼�ṹ�Ĺؼ��ֵĺ���������
�������
	tblName		����
	includeFunBody	�Ƿ���������壬1,������0������
	fp		�ļ����
�������
	funName		�����ĺ�������
����ֵ
	��
*/
void UnionGenerateConvertRecStrIntoStructPrimaryKeyFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp)
{
	char	structPtrDefName[128+1];
	
	memset(structPtrDefName,0,sizeof(structPtrDefName));
	UnionFormStructDefNameFromTBLName(tblName,1,structPtrDefName);
	UnionFormConvertRecStrIntoStructPrimaryKeyFunNameFromTBLName(tblName,funName);
	fprintf(outFp,"\n");
	fprintf(outFp,"\n/*\n���ܣ�\n");
	fprintf(outFp,"	����¼�ִ�ת��Ϊ��¼�ṹ�Ĺؼ��ֵĺ���\n");
	fprintf(outFp,"�������:\n");
	fprintf(outFp,"	recStr	��¼�ִ�\n");
	fprintf(outFp,"	lenOfRecStr	��¼������\n");
	fprintf(outFp,"�������:\n");
	fprintf(outFp,"	prec	��¼ָ��\n");
	fprintf(outFp,"����ֵ\n");
	fprintf(outFp,"	>=0		����������Ŀ\n");
	fprintf(outFp,"	<0		ʧ��,������\n");
	fprintf(outFp,"*/\n");
	fprintf(outFp,"int %s(char *recStr,int lenOfRecStr,%s prec)",funName,structPtrDefName);
	if (!includeFunBody)
		fprintf(outFp,";\n");
	else
		fprintf(outFp,"\n");
	return;
}

/*
����	
	���ݱ��壬����һ���Ӽ�¼���ж���¼�ؼ��ֵĺ���
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateConvertRecStrIntoStructPrimaryKeyFunFromTBLDef(char *tblName,FILE *fp)
{
	FILE			*outFp = stdout;
	int			ret;
	TUnionObject		objDef;
	int			fldNum;
	PUnionObjectFldDef	pfldDef;
	char			fldDefName[128+1];
	char			funName[128+1];
	char			defaultValueSetFunName[128+1];
	char			fldFunName[128+1];
	
	if (tblName == NULL)
		return(errCodeParameter);
		
	if (fp != NULL)
		outFp = fp;
	
	// ������
	memset(&objDef,0,sizeof(objDef));
	if ((ret = UnionReadObjectDefFromDesignDoc(tblName,&objDef)) < 0)
	{
		UnionUserErrLog("in UnionGenerateConvertRecStrIntoStructPrimaryKeyFunFromTBLDef:: UnionReadObjectDefFromDesignDoc [%s]!\n",tblName);
		return(ret);
	}
	// ��������
	UnionGenerateConvertRecStrIntoStructPrimaryKeyFunDeclarationFromTBLDef(tblName,1,funName,outFp);
	// ������
	fprintf(outFp,"{\n");
	// �ֲ���������
	fprintf(outFp,"	int	ret;\n");
	fprintf(outFp,"	int	offset = 0;\n");
	fprintf(outFp,"\n");
	// �����ж�
	fprintf(outFp,"	if ((prec == NULL) || (recStr == NULL))\n");
	fprintf(outFp,"	{\n");
	fprintf(outFp,"		UnionUserErrLog(\"in %s:: prec or recStr is null!\\n\");\n",funName);
	fprintf(outFp,"		return(UnionSetUserDefinedErrorCode(errCodeNullPointer));\n");
	fprintf(outFp,"	}\n");
	// ��ʼ������
	memset(defaultValueSetFunName,0,sizeof(defaultValueSetFunName));
	UnionFormStructDefaultValueSetFunNameFromTBLName(objDef.name,defaultValueSetFunName);
	fprintf(outFp,"	%s(prec);\n",defaultValueSetFunName);
	for (fldNum = 0; fldNum < objDef.fldNum; fldNum++)
	{
		pfldDef = &(objDef.fldDefGrp[fldNum]);
		UnionFormStructFldDefNameFromTBLName(objDef.name,pfldDef->name,fldDefName);
		if (!UnionIsPrimaryKeyFldOfObject(&objDef,pfldDef->name))	// �ж��Ƿ��ǹؼ���
			continue;
		// �������
		switch (pfldDef->type)
		{
			case	conObjectFldType_String:
				if (pfldDef->size > 1)
				{
					fprintf(outFp,"	ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,%s,prec->%s,sizeof(prec->%s));\n",fldDefName,pfldDef->name,pfldDef->name);
					strcpy(fldFunName,"UnionReadRecFldFromRecStr");
				}
				else
				{
					fprintf(outFp,"	ret = UnionReadCharTypeRecFldFromRecStr(recStr,lenOfRecStr,%s,&(prec->%s));\n",fldDefName,pfldDef->name);
					strcpy(fldFunName,"UnionReadCharTypeRecFldFromRecStr");
				}
				break;
			case	conObjectFldType_Int:
			case	conObjectFldType_Bool:
				fprintf(outFp,"	ret = UnionReadIntTypeRecFldFromRecStr(recStr,lenOfRecStr,%s,&(prec->%s));\n",fldDefName,pfldDef->name);
				strcpy(fldFunName,"UnionReadIntTypeRecFldFromRecStr");
				break;
			case	conObjectFldType_Double:
				fprintf(outFp,"	ret = UnionReadDoubleTypeRecFldFromRecStr(recStr,lenOfRecStr,%s,&(prec->%s));\n",fldDefName,pfldDef->name);
				strcpy(fldFunName,"UnionReadDoubleTypeRecFldFromRecStr");
				break;
			case	conObjectFldType_Bit:
				fprintf(outFp,"	ret = UnionReadBitsTypeRecFldFromRecStr(recStr,lenOfRecStr,%s,prec->%s,sizeof(prec->%s));\n",fldDefName,pfldDef->name,pfldDef->name);
				strcpy(fldFunName,"UnionReadBitsTypeRecFldFromRecStr");
				break;
			default:
				fprintf(outFp,"	ret = UnionReadBitsTypeRecFldFromRecStr(recStr,lenOfRecStr,%s,prec->%s,sizeof(prec->%s));\n",fldDefName,pfldDef->name,pfldDef->name);
				strcpy(fldFunName,"UnionReadBitsTypeRecFldFromRecStr");
				break;
		}
		fprintf(outFp,"	if (ret < 0)\n");
		fprintf(outFp,"	{\n");
		fprintf(outFp,"		if (!%d)\n",pfldDef->nullPermitted);
		fprintf(outFp,"		{\n");
		fprintf(outFp,"			UnionUserErrLog(\"in %s:: %s fld [%s] error!\\n\");\n",funName,fldFunName,pfldDef->name);
		fprintf(outFp,"			return(UnionSetUserDefinedErrorCode(ret));\n");
		fprintf(outFp,"		}\n");
		fprintf(outFp,"	}\n");
	}
	fprintf(outFp,"	return(0);\n");
	fprintf(outFp,"}\n");
	return(0);
}


/*
����	
	���ݱ�������ñ�ṹ����ͷ�ļ�·��
�������
	tblName		����
�������
	fullPath	ͷ�ļ�·��
����ֵ
	��
*/
void UnionFormFullPathOfTableDefStructHeadFileName(char *tblName,char *fullPath)
{
	sprintf(fullPath,"%s",getenv("UNIONDATAINCDIR"));
	return;
}

/*
����	
	���ݱ��������Ŀ���ļ���
�������
	tblName		����
�������
	fileName	�ļ���
����ֵ
	��
*/
void UnionFormObjFileNameOfTBLProgramFromTBLName(char *tblName,char *fileName)
{
	char	programName[128+1];
	char	dir[256];
	char	version[128];
	int	len;
	
	memset(version,0,sizeof(version));
	if ((len = UnionReadTableListRecVersionOfSpecTBLName(tblName,version,sizeof(version))) <= 0)
		strcpy(version,getenv("UNIONCURRENTSRCVERSION"));
	
	UnionFormTBLProgramNameFromTBLName(tblName,programName);
	sprintf(fileName,"%s.%s.o",programName,version);
	return;
}

/*
����	
	���ݱ��������Ŀ���ļ���(��ȫ·��)
�������
	tblName		����
�������
	funName		c�ļ���
����ֵ
	��
*/
void UnionFormFullObjFileNameOfTBLProgramFromTBLName(char *tblName,char *fileName)
{
	char	programName[128+1];
	char	dir[256];
	char	version[128];
	int	len;
	
	memset(version,0,sizeof(version));
	if ((len = UnionReadTableListRecVersionOfSpecTBLName(tblName,version,sizeof(version))) <= 0)
		strcpy(version,getenv("UNIONCURRENTSRCVERSION"));
	
	UnionFormTBLProgramNameFromTBLName(tblName,programName);
	sprintf(fileName,"%s/%s.%s.o",getenv("UNIONDATALIBDIR"),programName,version);
	return;
}

/*
����	
	���ݱ��壬����һ��makefile�ļ�
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateMakeFileFromTBLDefToFp(char *tblName,FILE *fp)
{
	FILE			*outFp = stdout;
	int			ret;
	char			tmpBuf[128+1];
	char			versionTag[128+1];
	
	if ((tblName == NULL) || (strcmp(tblName,"tableList") == 0) || (strcmp(tblName,"tableField") == 0))
		return(errCodeParameter);

	if (fp != NULL)
		outFp = fp;

	memset(tmpBuf,0,sizeof(tmpBuf));
	UnionGetFullSystemDateTime(tmpBuf);
	fprintf(outFp,"#auto-generated make file on %s by %s\n",tmpBuf,getenv("LOGNAME"));
	fprintf(outFp,"\n");
	//fprintf(outFp,"INCL = -I %s -I %s -I %s/include\n",getenv("UNIONDATAUSEHOMEINCDIR"),getenv("UNIONDATAINCDIR"),getenv("UNIONLIBDIR"));
	fprintf(outFp,"INCL = -I $(UNIONDATAUSEHOMEINCDIR) -I $(UNIONDATAINCDIR) -I $(UNIONLIBDIR)/include\n");
	fprintf(outFp,"\n");
	fprintf(outFp,"all:	makeall\\\n");
	fprintf(outFp,"	libProducts\n");
	fprintf(outFp,"\n");
	//fprintf(outFp,"DEFINES = %s\n",getenv("OSSPEC"));
	fprintf(outFp,"DEFINES = $(OSSPEC)\n");
	fprintf(outFp,"\n");
	fprintf(outFp,".SUFFIXES:.c.o\n");
	fprintf(outFp,".c.o:\n");
	fprintf(outFp,"	$(CC) $(DEFINES) $(INCL) -c $*.c\n");
	fprintf(outFp,"\n");
	fprintf(outFp,"libProducts:	$(null)\n");
	fprintf(outFp,"	mv *.o		%s\n",getenv("UNIONDATALIBDIR"));
	UnionFormIncFileNameOfTBLProgramFromTBLName(tblName,tmpBuf);
	fprintf(outFp,"	cp %s/%s	%s\n",getenv("UNIONDATAINCDIR"),tmpBuf,getenv("UNIONDATAPROINCDIR"));
	fprintf(outFp,"\n");	
	UnionFormObjFileNameOfTBLProgramFromTBLName(tblName,tmpBuf);
	fprintf(outFp,"objs=%s\n",tmpBuf);
	fprintf(outFp,"makeall:	$(objs)\n");
	return(0);
}

/*
����	
	���ݱ��������makefile�ļ���
�������
	tblName		����
�������
	funName		c�ļ���
����ֵ
	��
*/
void UnionFormMakeFileNameOfTBLProgramFromTBLName(char *tblName,char *cFileName)
{
	char	programName[128+1];
	char	dir[256];
	char	version[128];
	int	len;
	
	memset(version,0,sizeof(version));
	if ((len = UnionReadTableListRecVersionOfSpecTBLName(tblName,version,sizeof(version))) <= 0)
		strcpy(version,getenv("UNIONCURRENTSRCVERSION"));
	
	UnionFormTBLProgramNameFromTBLName(tblName,programName);
	sprintf(cFileName,"%s.%s.mk",programName,version);
	return;
}

/*
����	
	���ݱ��������makefile�ļ���
�������
	tblName		����
�������
	funName		c�ļ���
����ֵ
	��
*/
void UnionFormFullMakeFileNameOfTBLProgramFromTBLName(char *tblName,char *cFileName)
{
	char	programName[128+1];
	char	dir[256];
	char	version[128];
	int	len;
	
	memset(version,0,sizeof(version));
	if ((len = UnionReadTableListRecVersionOfSpecTBLName(tblName,version,sizeof(version))) <= 0)
		strcpy(version,getenv("UNIONCURRENTSRCVERSION"));
	
	UnionFormTBLProgramNameFromTBLName(tblName,programName);
	sprintf(cFileName,"%s/%s.%s.mk",getenv("UNIONDATASRCDIR"),programName,version);
	return;
}

/*
����	
	���ݱ��壬����һ��makefile�ļ�
�������
	tblName			����
	fileName		�ļ�����
	modeWhenFileExists	�ļ�����ʱ��д����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateMakeFileFromTBLDefToSpecFile(char *tblName,char *fileName,int modeWhenFileExists)
{
	char	thisFileName[512+1];
	FILE	*fp;
	int	ret;
	
	if ((fileName == NULL) || (strlen(fileName) == 0))
		UnionFormFullMakeFileNameOfTBLProgramFromTBLName(tblName,thisFileName);
	else
		strcpy(thisFileName,fileName);
	if (!UnionExistsFile(thisFileName))
		goto writeNow;
	switch (modeWhenFileExists)
	{
		case	conOverwriteModeWhenFileExistsOverwrite:
			break;
		case	conOverwriteModeWhenFileExistsReturn:
			UnionUserErrLog("in UnionGenerateMakeFileFromTBLDefToSpecFile:: %s already exists, the include file not generated!\n",thisFileName);
			return(errCodeFileAlreadyExists);
		default:
			if (!UnionConfirm("%s already exists, overwrite it",thisFileName))
				return(errCodeFileAlreadyExists);
			break;
	}
	UnionLog("in UnionGenerateMakeFileFromTBLDefToSpecFile:: %s already exists, and overwrite it!\n",thisFileName);
writeNow:
	if ((fp = fopen(thisFileName,"w")) == NULL)
	{
		UnionUserErrLog("in UnionGenerateMakeFileFromTBLDefToSpecFile:: fopen [%s] error!\n",thisFileName);
		return(errCodeUseOSErrCode);
	}
	if ((ret = UnionGenerateMakeFileFromTBLDefToFp(tblName,fp)) < 0)
		UnionUserErrLog("in UnionGenerateMakeFileFromTBLDefToSpecFile:: Generate c file for table [%s] failure! ret = [%d]!\n",tblName,ret);
	else
		UnionLog("in UnionGenerateMakeFileFromTBLDefToSpecFile:: Generate c file for table [%s] ok!\n",tblName);
	fclose(fp);
	return(ret);
}

/*
����	
	���ݱ��������makefile�ļ���ִ�нű��ļ�
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateMakeFileExcuteScriptFromTBLDefToFp(char *tblName,FILE *fp)
{
	FILE			*outFp = stdout;
	int			ret;
	char			tmpBuf[128+1];
	char			versionTag[128+1];
	
	if ((tblName == NULL) || (strcmp(tblName,"tableList") == 0) || (strcmp(tblName,"tableField") == 0))
		return(errCodeParameter);

	if (fp != NULL)
		outFp = fp;

	memset(tmpBuf,0,sizeof(tmpBuf));
	UnionGetFullSystemDateTime(tmpBuf);
	fprintf(outFp,"#auto-generated make scrpit on %s by %s\n",tmpBuf,getenv("LOGNAME"));
	fprintf(outFp,"\n");
	fprintf(outFp,"cd %s\n",getenv("UNIONDATASRCDIR"));
	fprintf(outFp,"\n");
	UnionFormMakeFileNameOfTBLProgramFromTBLName(tblName,tmpBuf);
	fprintf(outFp,"make -f %s\n",tmpBuf);
	return(0);
}

/*
����	
	���ݱ��������makefile�ļ���ִ�нű��ļ���
�������
	tblName		����
�������
	funName		c�ļ���
����ֵ
	��
*/
void UnionFormFullMakeFileExcuteScriptFileNameOfTBLProgramFromTBLName(char *tblName,char *cFileName)
{
	char	programName[128+1];
	char	dir[256];
	char	version[128];
	int	len;
	
	memset(version,0,sizeof(version));
	if ((len = UnionReadTableListRecVersionOfSpecTBLName(tblName,version,sizeof(version))) <= 0)
		strcpy(version,getenv("UNIONCURRENTSRCVERSION"));
	
	UnionFormTBLProgramNameFromTBLName(tblName,programName);
	sprintf(cFileName,"%s/mk%s.%s",getenv("UNIONDATASRCDIR"),programName,version);
	return;
}

/*
����	
	���ݱ��������makefile�ļ���ִ�нű��ļ�
�������
	tblName			����
	specFileName		�ļ�����
	modeWhenFileExists	�ļ�����ʱ��д����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateMakeFileExcuteScriptFileFromTBLDefToSpecFile(char *tblName,char *specFileName,int modeWhenFileExists)
{
	char	fileName[512+1];
	char	tmpBuf[512+1];
	FILE	*fp;
	int	ret;
	
	if ((specFileName == NULL) || (strlen(specFileName) == 0))
		UnionFormFullMakeFileExcuteScriptFileNameOfTBLProgramFromTBLName(tblName,fileName);
	else
		strcpy(fileName,specFileName);
	if (!UnionExistsFile(fileName))
		goto writeNow;
	switch (modeWhenFileExists)
	{
		case	conOverwriteModeWhenFileExistsOverwrite:
			break;
		case	conOverwriteModeWhenFileExistsReturn:
			UnionUserErrLog("in UnionGenerateMakeFileExcuteScriptFileFromTBLDefToSpecFile:: %s already exists, the include file not generated!\n",fileName);
			return(errCodeFileAlreadyExists);
		default:
			if (!UnionConfirm("%s already exists, overwrite it",fileName))
				return(errCodeFileAlreadyExists);
			break;
	}
	UnionLog("in UnionGenerateMakeFileExcuteScriptFileFromTBLDefToSpecFile:: %s already exists, and overwrite it!\n",fileName);
writeNow:
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionUserErrLog("in UnionGenerateMakeFileExcuteScriptFileFromTBLDefToSpecFile:: fopen [%s] error!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	if ((ret = UnionGenerateMakeFileExcuteScriptFromTBLDefToFp(tblName,fp)) < 0)
		UnionUserErrLog("in UnionGenerateMakeFileExcuteScriptFileFromTBLDefToSpecFile:: Generate c file for table [%s] failure! ret = [%d]!\n",tblName,ret);
	else
		UnionLog("in UnionGenerateMakeFileExcuteScriptFileFromTBLDefToSpecFile:: Generate c file for table [%s] ok!\n",tblName);
	fclose(fp);
	// �ýű��ļ�Ϊ��ִ��״̬
	sprintf(tmpBuf,"chmod a+x %s",fileName);
	system(tmpBuf);
	return(ret);
}

/*
����	
	���ݱ��������makefile�ļ���ִ�нű��ļ���
�������
	tblName		����
�������
	fileName	�ļ���
����ֵ
	��
*/
void UnionFormMakeFileExcuteScriptFileNameOfTBLProgramFromTBLName(char *tblName,char *fileName)
{
	char	programName[128+1];
	char	dir[256];
	char	version[128];
	int	len;
	
	memset(version,0,sizeof(version));
	if ((len = UnionReadTableListRecVersionOfSpecTBLName(tblName,version,sizeof(version))) <= 0)
		strcpy(version,getenv("UNIONCURRENTSRCVERSION"));
	
	UnionFormTBLProgramNameFromTBLName(tblName,programName);
	sprintf(fileName,"mk%s.%s",programName,version);
	return;
}

/*
����	
	�Զ�������ݱ��Զ����ɵ�Դ����
�������
	tblName			����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionComplyAutoGeneratedCFileFromTBLDef(char *tblName)
{
	char	tmpBuf[1024+1];
	int	offset;
	char	*fileName;
	int	ret;
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	UnionFormFullMakeFileExcuteScriptFileNameOfTBLProgramFromTBLName(tblName,tmpBuf);
	offset = strlen(tmpBuf);
	fileName = UnionGenerateMngSvrTempFile();
	sprintf(tmpBuf+offset," > %s 2>> %s",fileName,fileName);	
	if ((ret = system(tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionComplyAutoGeneratedCFileFromTBLDef:: system [%s] ret = [%d]!\n",tmpBuf,ret);
	}
	return(0);
}
