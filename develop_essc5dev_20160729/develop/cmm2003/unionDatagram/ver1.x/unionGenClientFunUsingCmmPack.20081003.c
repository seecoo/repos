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
#include "unionPackAndFunRelation.h"
#include "unionGenClientFunUsingCmmPack.h"
#include "UnionLog.h"

/*
����	
	��ȡһ�����������������ֵ����
�������
	funName		��������
	varValueTag	ֵ����
	inputOrOutput	�Ƕ����뻹�������1�����������2�����������3�Ƿ���ֵ
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadTypeOfSpecVarValueTag(char *funName,char *varValueTag,int inputOrOutputOrReturn)
{
	int			ret;
	char			fileName[256+1];
	FILE			*indexFp;
	char			lineStr[512+1];
	int			lineLen;
	TUnionVarValueTagDef	def;
	
	if (inputOrOutputOrReturn == 1)
		UnionGetFunVarInputValueListIndexFileName(funName,fileName);
	else if (inputOrOutputOrReturn == 2)
		UnionGetFunVarOutputValueListIndexFileName(funName,fileName);
	else if (inputOrOutputOrReturn == 3)
		UnionGetFunVarReturnValueListIndexFileName(funName,fileName);
	else
	{
		UnionUserErrLog("in UnionReadTypeOfSpecVarValueTag:: invalid inputOrOutputOrReturn [%d]\n",inputOrOutputOrReturn);
		return(errCodeParameter);
	}
	if ((indexFp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadTypeOfSpecVarValueTag:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	while (!feof(indexFp))
	{
		// ��һ��
		memset(lineStr,0,sizeof(lineStr));
		if (((lineLen = UnionReadOneDataLineFromTxtFile(indexFp,lineStr,sizeof(lineStr))) < 0) && (lineLen != errCodeFileEnd))
		{
			UnionLog("in UnionReadTypeOfSpecVarValueTag:: UnionReadOneDataLineFromTxtFile!\n");
			continue;
		}
		if (lineLen == errCodeFileEnd)
			continue;
		// ���ļ��ж�ȡȡֵ��ʶ
		memset(&def,0,sizeof(def));
		if ((ret = UnionReadVarValueTagDefFromStr(lineStr,lineLen,&def)) < 0)
		{
			UnionUserErrLog("in UnionReadTypeOfSpecVarValueTag:: UnionReadVarValueTagDefFromStr [%s]!\n",lineStr);
			continue;
		}
		if (strcmp(def.varValueTag,varValueTag) == 0)
		{
			fclose(indexFp);
			return(def.baseTypeTag);
		}
	}
	fclose(indexFp);
	UnionUserErrLog("in UnionReadTypeOfSpecVarValueTag:: [%s] not defined in [%s]\n",varValueTag,fileName);
	return(errCodeCDPMDL_VarValueTagNotDefined);
}

/*
����	
	����һ�ζԱ�������и�ֵ�Ĵ���
�������
	isClientFun	��ʶ�Ƿ��ǿͻ��˺���
	funName		��������
	errExit		�����˳��Ķϵ�
	ppackAndFunDef	��ֵ��
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateCmmPackSetCodes(int isClientFun,char *funName,char *errExit,PUnionPackAndFunRelation ppackAndFunDef,FILE *fp)
{
	int				ret;
	FILE				*outFp=stdout;
	int				index;
	int				baseType;
	char				varName[128+1];
	char				fldFunName[128+1];
	char				lenOfVar[256+1];
	char				putFldIntVarName[48+1],reaFldFromVarName[48+1];
	int				blankNum = 0;
	
	UnionSetCmmPackReqAndResVarName(isClientFun,putFldIntVarName,reaFldFromVarName);

	if (ppackAndFunDef == NULL)
	{
		UnionUserErrLog("in UnionGenerateCmmPackSetCodes:: ppackAndFunDef is NULL!\n");
		return(errCodeParameter);
	}
	if (fp != NULL)
		outFp = fp;
	fprintf(outFp,"\n        // ���¶������Ľ��и�ֵ\n");
	if (isClientFun)
	{
		blankNum = 8;
		fprintf(outFp,"        // ��ʼ��������\n");
		fprintf(outFp,"        UnionInitCmmPackFldDataList(%s);\n",putFldIntVarName);
	}
	else
	{
		fprintf(outFp,"        // ��ʼ����Ӧ����\n");
		fprintf(outFp,"        UnionInitCmmPackFldDataList(%s);\n",putFldIntVarName);
	}
	
	for (index = 0; index < ppackAndFunDef->relationNum; index++)
	{
		if (ppackAndFunDef->relationGrp[index].isPackToFun  == isClientFun)	// �ǽ�������ֵΪ����
			continue;
		if (strlen(ppackAndFunDef->relationGrp[index].remark) != 0)
		{
			fprintf(outFp,"        // ����%s\n",ppackAndFunDef->relationGrp[index].remark);
		}
		strcpy(varName,ppackAndFunDef->relationGrp[index].varValueTag);
		switch (baseType = UnionConvertCVarDefKeyWordIntoTag(ppackAndFunDef->relationGrp[index].fldType))
		{
			case	conVarTypeTagChar:
				strcpy(fldFunName,"UnionPutCharTypeCmmPackFldIntoFldDataList");				
				break;
			case	conVarTypeTagUnsignedChar:
				strcpy(fldFunName,"UnionPutUnsignedCharTypeCmmPackFldIntoFldDataList");				
				break;
			case	conVarTypeTagInt:
			case	conVarTypeTagEnum:
				strcpy(fldFunName,"UnionPutIntTypeCmmPackFldIntoFldDataList");				
				break;
			case	conVarTypeTagUnsignedInt:
				strcpy(fldFunName,"UnionPutUnsignedIntTypeCmmPackFldIntoFldDataList");				
				break;
			case	conVarTypeTagLong:
				strcpy(fldFunName,"UnionPutLongTypeCmmPackFldIntoFldDataList");				
				break;
			case	conVarTypeTagUnsignedLong:
				strcpy(fldFunName,"UnionPutUnsignedLongTypeCmmPackFldIntoFldDataList");				
				break;
			case	conVarTypeTagDouble:
				strcpy(fldFunName,"UnionPutDoubleTypeCmmPackFldIntoFldDataList");				
				break;
			default:
				goto complexVar;
		}
		fprintf(outFp,"        if ((ret = %s(%s,%s,%s)) < 0)\n",fldFunName,ppackAndFunDef->relationGrp[index].packFldID,varName,putFldIntVarName);
		fprintf(outFp,"        {\n");
		fprintf(outFp,"                UnionUserErrLog(\"in %s:: %s %s! ret = [%%d]\\n\",ret);\n",funName,fldFunName,ppackAndFunDef->relationGrp[index].packFldID);
		if ((errExit != NULL) && (strlen(errExit) != 0))
			fprintf(outFp,"                goto %s;\n",errExit);
		else
			fprintf(outFp,"                return(UnionTranslateClientErrCode(ret));\n");
		fprintf(outFp,"        }\n");
		continue;
complexVar:
		memset(lenOfVar,0,sizeof(lenOfVar));
		if (strlen(ppackAndFunDef->relationGrp[index].varNameOfRealLen) != 0)
			strcpy(lenOfVar,ppackAndFunDef->relationGrp[index].varNameOfRealLen);
		else
		{
			switch (baseType)
			{
				case	conVarTypeTagString:
					sprintf(lenOfVar,"strlen(%s)",ppackAndFunDef->relationGrp[index].varValueTag);
					break;
				case	conVarTypeTagVoid:
					UnionUserErrLog("in UnionGenerateCmmPackSetCodes:: [%s] of type [%s] must specified a len!\n",ppackAndFunDef->relationGrp[index].varValueTag,
							ppackAndFunDef->relationGrp[index].fldType);
					return(errCodeCDPMDL_NotBaseVarType);
				case	conVarTypeTagPointer:
					sprintf(lenOfVar,"sizeof(*(%s))",ppackAndFunDef->relationGrp[index].varValueTag);
					break;
				case	conVarTypeTagArray:
					sprintf(lenOfVar,"sizeof(%s)",ppackAndFunDef->relationGrp[index].varValueTag);
					break;
				default:
					goto otherVar;
			}
		}
		if (isClientFun)
		{
			fprintf(outFp,"        if (%s != NULL)\n",ppackAndFunDef->relationGrp[index].varValueTag);
			fprintf(outFp,"        {\n");
		}
		UnionPrintBlankOfNumToFp(blankNum,outFp);
		fprintf(outFp,"        if ((ret = UnionPutCmmPackFldIntoFldDataList(%s,%s,%s,%s)) < 0)\n",
					ppackAndFunDef->relationGrp[index].packFldID,
					lenOfVar,
					ppackAndFunDef->relationGrp[index].varValueTag,
					putFldIntVarName);
		UnionPrintBlankOfNumToFp(blankNum,outFp);
		fprintf(outFp,"        {\n");
		UnionPrintBlankOfNumToFp(blankNum,outFp);
		fprintf(outFp,"                UnionUserErrLog(\"in %s:: %s %s! ret = [%%d]\\n\",ret);\n",funName,"UnionPutCmmPackFldIntoFldDataList",ppackAndFunDef->relationGrp[index].packFldID);
		UnionPrintBlankOfNumToFp(blankNum,outFp);
		if ((errExit != NULL) && (strlen(errExit) != 0))
			fprintf(outFp,"                goto %s;\n",errExit);
		else
			fprintf(outFp,"                return(UnionTranslateClientErrCode(ret));\n");
		UnionPrintBlankOfNumToFp(blankNum,outFp);
		fprintf(outFp,"        }\n");
		if (isClientFun)
			fprintf(outFp,"        }\n");
		continue;
otherVar:
		switch (baseType)
		{
			case	conVarTypeTagStruct:
			case	conVarTypeTagUnion:
				if (strlen(lenOfVar) == 0)
					sprintf(lenOfVar,"sizeof(%s)",ppackAndFunDef->relationGrp[index].varValueTag);
				break;
			default:
				UnionUserErrLog("in UnionGenerateCmmPackSetCodes:: [%s] of type [%s] not invalid!\n",ppackAndFunDef->relationGrp[index].varValueTag,
							ppackAndFunDef->relationGrp[index].fldType);
				return(errCodeCDPMDL_NotBaseVarType);
		}
		fprintf(outFp,"        if ((ret = UnionPutCmmPackFldIntoFldDataList(%s,&(%s),%s,%s)) < 0)\n",
				ppackAndFunDef->relationGrp[index].packFldID,
				lenOfVar,
				ppackAndFunDef->relationGrp[index].varValueTag,
				putFldIntVarName);
		fprintf(outFp,"        {\n");
		fprintf(outFp,"                UnionUserErrLog(\"in %s:: %s %s! ret = [%%d]\\n\",ret);\n",funName,"UnionPutCmmPackFldIntoFldDataList",ppackAndFunDef->relationGrp[index].packFldID);
		if ((errExit != NULL) && (strlen(errExit) != 0))
			fprintf(outFp,"                goto %s;\n",errExit);
		else
			fprintf(outFp,"                return(UnionTranslateClientErrCode(ret));\n");
		fprintf(outFp,"        }\n");
	}
	return(0);
}

/*
����	
	����һ���Ա�������и�ֵ�ĺ���
�������
	isClientFun	��ʶ�Ƿ��ǿͻ��˺���
	funName		��������
	errExit		�����˳��Ķϵ�
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateCmmPackFldSetCodesForFun(int isClientFun,char *funName,char *errExit,FILE *fp)
{
	int				ret;
	TUnionPackAndFunRelation	packAndFunDef;
	
	memset(&packAndFunDef,0,sizeof(packAndFunDef));
	if ((ret = UnionReadSpecPackAndFunRelationFromFile(funName,&packAndFunDef)) < 0)
	{
		UnionUserErrLog("in UnionGenerateCmmPackFldSetCodesForFun:: UnionReadSpecPackAndFunRelationFromFile!\n");
		return(ret);
	}
	return(UnionGenerateCmmPackSetCodes(isClientFun,funName,errExit,&packAndFunDef,fp));
}

/*
����	
	���������ĺ���Ӧ���ı�������
�������
	isClientFun	��ʶ�Ƿ��ǿͻ��˺���
�������
	putFldIntVarName	����������
	reaFldFromVarName	��Ӧ��������
����ֵ
	>=0		�ɹ�
	<0		�������
*/
void UnionSetCmmPackReqAndResVarName(int isClientFun,char *putFldIntVarName,char *reaFldFromVarName)
{
	if (isClientFun)
	{
		strcpy(putFldIntVarName,"&packReq");
		strcpy(reaFldFromVarName,"&packRes");
	}
	else
	{
		strcpy(putFldIntVarName,"ppackRes");
		strcpy(reaFldFromVarName,"ppackReq");
	}
}
		
/*
����	
	����һ���ӱ������ֵ����Ĵ���
�������
	isClientFun	��ʶ�Ƿ��ǿͻ��˺���
	funName		��������
	errExit		�����˳��Ķϵ�
	ppackAndFunDef	��ֵ��
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateCmmPackReadCodes(int isClientFun,char *funName,char *errExit,PUnionPackAndFunRelation ppackAndFunDef,FILE *fp)
{
	int				ret;
	FILE				*outFp=stdout;
	int				index;
	int				baseType;
	char				varName[128+1];
	char				fldFunName[128+1];
	int				returnValueSet = 0;
	char				lenOfVar[128+1];
	char				putFldIntVarName[48+1],reaFldFromVarName[48+1];
	int				blankNum = 0;
	
	if (isClientFun)
		blankNum = 8;
	UnionSetCmmPackReqAndResVarName(isClientFun,putFldIntVarName,reaFldFromVarName);
	
	if (ppackAndFunDef == NULL)
	{
		UnionUserErrLog("in UnionGenerateCmmPackSetCodes:: ppackAndFunDef is NULL!\n");
		return(errCodeParameter);
	}
	if (fp != NULL)
		outFp = fp;
	fprintf(outFp,"\n        // ���´ӱ��������ֵ\n");
	for (index = 0; index < ppackAndFunDef->relationNum; index++)
	{
		if (ppackAndFunDef->relationGrp[index].isPackToFun != isClientFun)	// �ǽ�������ֵΪ�����Ķ���
			continue;
		if (isClientFun)
		{
			returnValueSet = 0;
			if (strcmp(ppackAndFunDef->relationGrp[index].varValueTag,"ret") == 0)	// ������ֵ
			{
				fprintf(outFp,"        returnValueSet = 1;\n");
				returnValueSet = 1;
			}
		}
		if (strlen(ppackAndFunDef->relationGrp[index].remark) != 0)
		{
			fprintf(outFp,"        // ��ȡ%s\n",ppackAndFunDef->relationGrp[index].remark);
		}
		switch (baseType = UnionConvertCVarDefKeyWordIntoTag(ppackAndFunDef->relationGrp[index].fldType))
		{
			case	conVarTypeTagChar:
				strcpy(fldFunName,"UnionReadCharTypeCmmPackFldFromFldDataList");				
				break;
			case	conVarTypeTagUnsignedChar:
				strcpy(fldFunName,"UnionReadUnsignedCharTypeCmmPackFldFromFldDataList");				
				break;
			case	conVarTypeTagInt:
			case	conVarTypeTagEnum:
				strcpy(fldFunName,"UnionReadIntTypeCmmPackFldFromFldDataList");				
				break;
			case	conVarTypeTagUnsignedInt:
				strcpy(fldFunName,"UnionReadUnsignedIntTypeCmmPackFldFromFldDataList");				
				break;
			case	conVarTypeTagLong:
				strcpy(fldFunName,"UnionReadLongTypeCmmPackFldFromFldDataList");				
				break;
			case	conVarTypeTagUnsignedLong:
				strcpy(fldFunName,"UnionReadUnsignedLongTypeCmmPackFldFromFldDataList");				
				break;
			case	conVarTypeTagDouble:
				strcpy(fldFunName,"UnionReadDoubleTypeCmmPackFldFromFldDataList");				
				break;
			default:
				goto complexVar;
		}
		fprintf(outFp,"        if ((ret = %s(%s,%s,&(%s))) < 0)\n",fldFunName,reaFldFromVarName,ppackAndFunDef->relationGrp[index].packFldID,ppackAndFunDef->relationGrp[index].varValueTag);
		fprintf(outFp,"        {\n");
		fprintf(outFp,"                if (!%d)\n",ppackAndFunDef->relationGrp[index].isOptional);
		fprintf(outFp,"                {\n");
		fprintf(outFp,"                        UnionUserErrLog(\"in %s:: %s %s! ret = [%%d]\\n\",ret);\n",funName,fldFunName,ppackAndFunDef->relationGrp[index].packFldID);
		if ((errExit != NULL) && (strlen(errExit) != 0))
			fprintf(outFp,"                        goto %s;\n",errExit);
		else
			fprintf(outFp,"                        return(UnionTranslateClientErrCode(ret));\n");
		fprintf(outFp,"                }\n");
		fprintf(outFp,"        }\n");
		if (isClientFun)
		{
			if (returnValueSet)
			{
				fprintf(outFp,"        if (returnValueSet)\n");
				fprintf(outFp,"                returnValue = ret;\n");
			}
		}
		continue;
complexVar:
		memset(lenOfVar,0,sizeof(lenOfVar));
		if (strlen(ppackAndFunDef->relationGrp[index].varNameOfRealLen) != 0)
			strcpy(lenOfVar,ppackAndFunDef->relationGrp[index].varNameOfRealLen);
		else
		{
			switch (baseType)
			{
				case	conVarTypeTagString:
				case	conVarTypeTagVoid:
					UnionUserErrLog("in UnionGenerateCmmPackReadCodes:: [%s] of type [%s] must specified a len!\n",ppackAndFunDef->relationGrp[index].varValueTag,
							ppackAndFunDef->relationGrp[index].fldType);
					return(errCodeCDPMDL_NotBaseVarType);
				case	conVarTypeTagPointer:
					sprintf(lenOfVar,"sizeof(*(%s))",ppackAndFunDef->relationGrp[index].varValueTag);
					break;
				case	conVarTypeTagArray:
					sprintf(lenOfVar,"sizeof(%s)",ppackAndFunDef->relationGrp[index].varValueTag);
					break;
				default:
					goto otherVar;
			}
		}
		if (isClientFun)
		{
			fprintf(outFp,"        if (%s != NULL)\n",ppackAndFunDef->relationGrp[index].varValueTag);
			fprintf(outFp,"        {\n");
		}
		UnionPrintBlankOfNumToFp(blankNum,outFp);
		fprintf(outFp,"        if ((ret = UnionReadCmmPackFldFromFldDataList(%s,%s,%s,%s)) < 0)\n",
					reaFldFromVarName,
					ppackAndFunDef->relationGrp[index].packFldID,
					lenOfVar,
					ppackAndFunDef->relationGrp[index].varValueTag);
		UnionPrintBlankOfNumToFp(blankNum,outFp);
		fprintf(outFp,"        {\n");
		UnionPrintBlankOfNumToFp(blankNum,outFp);
		fprintf(outFp,"                if (!%d)\n",ppackAndFunDef->relationGrp[index].isOptional);
		UnionPrintBlankOfNumToFp(blankNum,outFp);
		fprintf(outFp,"                {\n");
		UnionPrintBlankOfNumToFp(blankNum,outFp);
		fprintf(outFp,"                        UnionUserErrLog(\"in %s:: %s %s! ret = [%%d]\\n\",ret);\n",funName,"UnionReadCmmPackFldFromFldDataList",ppackAndFunDef->relationGrp[index].packFldID);
		if ((errExit != NULL) && (strlen(errExit) != 0))
			fprintf(outFp,"                        goto %s;\n",errExit);
		else
			fprintf(outFp,"                        return(UnionTranslateClientErrCode(ret));\n");
		UnionPrintBlankOfNumToFp(blankNum,outFp);
		fprintf(outFp,"                }\n");
		UnionPrintBlankOfNumToFp(blankNum,outFp);
		fprintf(outFp,"        }\n");
		if (isClientFun)
			fprintf(outFp,"        }\n");
		continue;
otherVar:
		switch (baseType)
		{
			case	conVarTypeTagStruct:
			case	conVarTypeTagUnion:
				if (strlen(lenOfVar) == 0)
					sprintf(lenOfVar,"sizeof(%s)",ppackAndFunDef->relationGrp[index].varValueTag);
				break;
			default:
				UnionUserErrLog("in UnionGenerateCmmPackReadCodes:: [%s] of type [%s] not invalid!\n",ppackAndFunDef->relationGrp[index].varValueTag,
							ppackAndFunDef->relationGrp[index].fldType);
				return(errCodeCDPMDL_NotBaseVarType);
		}
		fprintf(outFp,"        if ((ret = UnionReadCmmPackFldFromFldDataList(%s,%s,%s,&(%s))) < 0)\n",
				reaFldFromVarName,
				ppackAndFunDef->relationGrp[index].packFldID,
				lenOfVar,
				ppackAndFunDef->relationGrp[index].varValueTag);
		fprintf(outFp,"        {\n");
		fprintf(outFp,"                if (!%d)\n",ppackAndFunDef->relationGrp[index].isOptional);
		fprintf(outFp,"                {\n");
		fprintf(outFp,"                        UnionUserErrLog(\"in %s:: %s %s! ret = [%%d]\\n\",ret);\n",funName,"UnionReadCmmPackFldFromFldDataList",ppackAndFunDef->relationGrp[index].packFldID);
		if ((errExit != NULL) && (strlen(errExit) != 0))
			fprintf(outFp,"                        goto %s;\n",errExit);
		else
			fprintf(outFp,"                        return(UnionTranslateClientErrCode(ret));\n");
		fprintf(outFp,"                }\n");
		fprintf(outFp,"        }\n");
	}
	return(0);
}

/*
����	
	����һ���ӱ������ֵ����Ĵ���
�������
	isClientFun	��ʶ�Ƿ��ǿͻ��˺���
	funName		��������
	errExit		�����˳��Ķϵ�
	fp		���Գ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateCmmPackFldReadCodesForFun(int isClientFun,char *funName,char *errExit,FILE *fp)
{
	int				ret;
	TUnionPackAndFunRelation	packAndFunDef;
	
	memset(&packAndFunDef,0,sizeof(packAndFunDef));
	if ((ret = UnionReadSpecPackAndFunRelationFromFile(funName,&packAndFunDef)) < 0)
	{
		UnionUserErrLog("in UnionGenerateCmmPackFldReadCodesForFun:: UnionReadSpecPackAndFunRelationFromFile!\n");
		return(ret);
	}
	return(UnionGenerateCmmPackReadCodes(isClientFun,funName,errExit,&packAndFunDef,fp));
}

/*
����	
	������������
�������
	fp		�ļ����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateCmmPackAndFunVarDeclareCodes(FILE *fp)
{
	FILE	*outFp = stdout;
	
	if (fp != NULL)
		outFp = fp;
	
	fprintf(outFp,"{\n");
	fprintf(outFp,"        int               ret;\n");
	fprintf(outFp,"        int               returnValueSet = 0;\n");
	fprintf(outFp,"        int               returnValue;\n");
	fprintf(outFp,"        TUnionCmmPackData packReq;  // ������\n");
	fprintf(outFp,"        TUnionCmmPackData packRes;  // ��Ӧ����\n");
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
int UnionGenerateCommWithCmmPackSvrCodes(char *funName,FILE *fp)
{
	FILE	*outFp = stdout;
	char	serviceID[100];
	int	ret;
	
	if (fp != NULL)
		outFp = fp;

	memset(serviceID,0,sizeof(serviceID));
	if ((ret = UnionGetServiceIDFromPackAndFunRelationDef(funName,serviceID)) < 0)
	{
		UnionUserErrLog("in UnionGenerateCommWithCmmPackSvrCodes:: UnionGetServiceIDFromPackAndFunRelationDef [%s]!\n",funName);
		return(ret);
	}
	fprintf(outFp,"\n        // �������ͨѶ\n");
	fprintf(outFp,"        // ��ʼ����Ӧ����\n");
	fprintf(outFp,"        UnionInitCmmPackFldDataList(&packRes);\n");
	fprintf(outFp,"        if ((ret = UnionCommWithCmmPackSvr(%d,&packReq,&packRes)) < 0)\n",atoi(serviceID));
	fprintf(outFp,"        {\n");
	fprintf(outFp,"                UnionUserErrLog(\"in %s:: UnionCommWithCmmPackSvr! ret = [%%d]!\\n\",ret);\n",funName);
	fprintf(outFp,"                return(UnionTranslateClientErrCode(ret));\n");
	fprintf(outFp,"        }\n");
	return(0);
}	

/*
����	
	����һ���ͻ��˺���
�������
	funName		��������
	incConfFileName	���ɵĺ���Ҫʹ�õ�ͷ�ļ�
	fp		�ļ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateClientFunUsingCmmPackToFp(char *funName,char *incConfFileName,FILE *fp)
{
	int		ret;
	FILE		*outFp = stdout;
	TUnionFunDef	funDef;
	int		index;
	
	// ����������
	memset(&funDef,0,sizeof(&funDef));
	if ((ret = UnionReadFunDefFromDefaultDefFile(funName,&funDef)) < 0)
	{
		UnionUserErrLog("in UnionGenerateClientFunUsingCmmPackToFp:: UnionReadFunDefFromDefaultDefFile [%s]!\n",funName);
		return(ret);
	}

	if (fp != NULL)
		outFp = fp;

	if ((incConfFileName != NULL) && (strlen(incConfFileName) > 0))
	{
		// д������ͷ�ļ���Ϣ
		if ((ret = UnionAddIncludeFileToFile(incConfFileName,outFp)) < 0)
		{
			UnionUserErrLog("in UnionGenerateClientFunUsingCmmPackToFp:: UnionAddIncludeFileToFile!\n");
			goto errExit;
		}
		// ��������ļ�
		fprintf(outFp,"#ifndef _unionCmmPackData_\n");
		fprintf(outFp,"#include \"unionCmmPackData.h\"\n");
		fprintf(outFp,"#endif\n");
		fprintf(outFp,"\n");
	}
	
	// ����������д�뵽������
	if ((ret = UnionPrintFunDefToFp(&funDef,1,0,outFp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateClientFunUsingCmmPackToFp:: UnionPrintFunDefToFp [%s]!\n",funName);
		goto errExit;
	}
	/*
	// дһ���������ɲ���������������ļ�
	if ((ret = UnionGenerateFunVarValueTagListIndexFile(&funDef)) < 0)
	{
		UnionUserErrLog("in UnionGenerateClientFunUsingCmmPackToFp:: UnionGenerateFunVarValueTagListIndexFile [%s]!\n",funName);
		goto errExit;
	}
	*/
	// ����������������
	if ((ret = UnionGenerateCmmPackAndFunVarDeclareCodes(fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateClientFunUsingCmmPackToFp:: UnionGenerateCmmPackAndFunVarDeclareCodes [%s]!\n",funName);
		goto errExit;
	}

	// �������ĸ�ֵ����
	if ((ret = UnionGenerateCmmPackFldSetCodesForFun(1,funName,NULL,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateClientFunUsingCmmPackToFp:: UnionGenerateCmmPackFldSetCodesForFun [%s]!\n",funName);
		goto errExit;
	}

	// �����������ͨѶ�Ĵ���
	if ((ret = UnionGenerateCommWithCmmPackSvrCodes(funName,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateClientFunUsingCmmPackToFp:: UnionGenerateCommWithCmmPackSvrCodes [%s]!\n",funName);
		goto errExit;
	}

	// �����ӱ��Ķ�ֵ�Ĵ���
	if ((ret = UnionGenerateCmmPackFldReadCodesForFun(1,funName,NULL,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateClientFunUsingCmmPackToFp:: UnionGenerateCmmPackFldReadCodesForFun [%s]!\n",funName);
		goto errExit;
	}
	
	// ������β����
	fprintf(outFp,"\n");
	fprintf(outFp,"        // �����˷���ֵ\n");
	fprintf(outFp,"        if (returnValueSet)\n");
	fprintf(outFp,"                 return(UnionTranslateClientErrCode(returnValue));\n");
	fprintf(outFp,"        return(UnionTranslateClientErrCode(ret));\n");
	fprintf(outFp,"}\n\n");
errExit:
	if (fp != stdout)
		fclose(fp);
	return(ret);
}

/*
����	
	����һ���ͻ��˺���
�������
	funName		��������
	toNewFile	�Ƿ�д�����ļ���
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateClientFunUsingCmmPack(char *funName,int toNewFile)
{
	char	fileName[256+1];
	char	incFileConf[256+1];
	int	ret;
	FILE	*fp;
		
	memset(fileName,0,sizeof(fileName));
	memset(incFileConf,0,sizeof(incFileConf));
	if ((ret = UnionGetFullProgramFileNameOfFun(funName,fileName,incFileConf)) < 0)
	{
		UnionUserErrLog("in UnionGenerateClientFunUsingCmmPack:: UnionGetFullProgramFileNameOfFun [%s]!\n",funName);
		return(ret);
	}
	if (!toNewFile)
	{
		fp = fopen(fileName,"a");
		memset(incFileConf,0,sizeof(incFileConf));
	}	
	else
		fp = fopen(fileName,"w");
	if (fp == NULL)
	{
		UnionSystemErrLog("in UnionGenerateClientFunUsingCmmPack:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	if ((ret = UnionGenerateClientFunUsingCmmPackToFp(funName,incFileConf,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateClientFunUsingCmmPack:: UnionGenerateClientFunUsingCmmPackToFp [%s]!\n",funName);
		fclose(fp);
		return(ret);
	}
	fclose(fp);
	return(0);
}

/*
����	
	Ϊָ���ĳ����������пͻ���API
�������
	nameOfProgram	��������
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateAllCmmPackClientAPIInSpecProgram(char *nameOfProgram)
{
	char	lineStr[1024+1];
	int	lineLen;
	int	ret;
	FILE	*fp;
	char	fileName[512+1];
	int	funNum = 0,failNum = 0;
	TUnionVarTypeDef	def;
	
	if (nameOfProgram == NULL)
		return(errCodeParameter);
	// ����ļ�����
	UnionGetDefaultFileNameOfVarTypeTagDef("",fileName);
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionGenerateAllCmmPackClientAPIInSpecProgram:: fopen [%s] error!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	memset(&def,0,sizeof(def));
	while (!feof(fp))
	{
		// ���ļ��ж�ȡһ��
		memset(lineStr,0,sizeof(lineStr));
		if ((lineLen = UnionReadOneDataLineFromTxtFile(fp,lineStr,sizeof(lineStr))) < 0)
		{
			if ((lineLen == errCodeEnviMDL_NullLine) || (lineLen == errCodeFileEnd))	// ����
				continue;
			UnionSystemErrLog("in UnionGenerateAllCmmPackClientAPIInSpecProgram:: UnionReadOneDataLineFromTxtFile error in [%s]\n",fileName);
			fclose(fp);
			return(lineLen);
		}
		if (lineLen == 0)	// ����
			continue;
		memset(&def,0,sizeof(def));
		if ((ret = UnionReadVarTypeDefFromStr(lineStr,lineLen,&def)) < 0)
			continue;
		if (def.typeTag != conDesginKeyWordTagFun)	// ���Ǻ���
			continue;
		if (strcmp(nameOfProgram,def.nameOfProgram) != 0)	// ���������������ĺ���
			continue;
		if ((ret = UnionGenerateClientFunUsingCmmPack(def.nameOfType,!funNum)) < 0)
		{
			UnionUserErrLog("in UnionGenerateAllCmmPackClientAPIInSpecProgram:: UnionGenerateClientFunUsingCmmPack [%s]!\n",def.nameOfType);
			printf("��������ʧ��::%s!\n",def.nameOfType);
			failNum++;
		}
		else
		{
			printf("�������ɳɹ�::%s!\n",def.nameOfType);
			funNum++;
		}
	}
	fclose(fp);
	UnionLog("in UnionGenerateAllCmmPackClientAPIInSpecProgram:: [%d] functions generated!\n",funNum);
	printf("���ɳɹ��ĺ�����Ŀ::%04d\n",funNum);
	printf("����ʧ�ܵĺ�����Ŀ::%04d\n",failNum);
	return(funNum);
}

/*
����	
	����һ������˺���
�������
	funName		��������
	incConfFileName	���ɵĺ���Ҫʹ�õ�ͷ�ļ�
	fp		�ļ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateServerFunUsingCmmPackToFp(char *funName,char *incConfFileName,FILE *fp)
{
	int		ret;
	FILE		*outFp = stdout;
	TUnionFunDef	funDef;
	int		index;
	
	// ����������
	memset(&funDef,0,sizeof(&funDef));
	if ((ret = UnionReadFunDefFromDefaultDefFile(funName,&funDef)) < 0)
	{
		UnionUserErrLog("in UnionGenerateServerFunUsingCmmPackToFp:: UnionReadFunDefFromDefaultDefFile [%s]!\n",funName);
		return(ret);
	}

	if (fp != NULL)
		outFp = fp;

	if ((incConfFileName != NULL) && (strlen(incConfFileName) > 0))
	{
		// д������ͷ�ļ���Ϣ
		if ((ret = UnionAddIncludeFileToFile(incConfFileName,outFp)) < 0)
		{
			UnionUserErrLog("in UnionGenerateServerFunUsingCmmPackToFp:: UnionAddIncludeFileToFile!\n");
			goto errExit;
		}
		// ��������ļ�
		fprintf(outFp,"#ifndef _unionCmmPackData_\n");
		fprintf(outFp,"#include \"unionCmmPackData.h\"\n");
		fprintf(outFp,"#endif\n");
		fprintf(outFp,"\n");
	}
	
	// ����������д�뵽������
	fprintf(outFp,"int %sAtSvr(PUnionCmmPackData ppackReq,PUnionCmmPackData ppackRes)\n",funDef.funName);
	fprintf(outFp,"{\n");
	// ���������ȱʡ��С
	UnionSetDefaultArraySizeWhenNoSizeSet(8192);
	// ���庯������
	if ((ret = UnionPrintFunVarDefToFpOfCPragramWithPrefixBlank(8,&funDef,"ret",8192,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateServerFunUsingCmmPackToFp:: UnionPrintFunVarDefToFpOfCPragramWithPrefixBlank [%s]!\n",funName);
		return(ret);
	}
	fprintf(outFp,"        int returnValue;\n");
	fprintf(outFp,"        int finishOK = 0;\n");
	// ������ָ���������ռ�Ĵ���
	fprintf(outFp,"\n");
	//UnionSetDefaultArraySizeWhenNoSizeSet(8192);
	if ((ret = UnionGenerateMallocCodesForPointerFunVarToFp(&funDef,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateServerFunUsingCmmPackToFp:: UnionGenerateMallocCodesForPointerFunVarToFp [%s]!\n",funName);
		return(ret);
	}
	// �����ӱ��Ķ�ֵ�Ĵ���
	if ((ret = UnionGenerateCmmPackFldReadCodesForFun(0,funName,"freePointer",fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateServerFunUsingCmmPackToFp:: UnionGenerateCmmPackFldReadCodesForFun [%s]!\n",funName);
		goto errExit;
	}
	// �������ú����Ĵ���
	if ((ret = UnionGenerateCallCodesOfFun(8,"returnValue",&funDef,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateServerFunUsingCmmPackToFp:: UnionGenerateCallCodesOfFun [%s]!\n",funName);
		goto errExit;
	}
	fprintf(outFp,"        if (returnValue != 0)\n");
	fprintf(outFp,"        {\n");
	fprintf(outFp,"                 UnionUserErrLog(\"in %sAtSvr:: %s error! ret = %%d\\n\",returnValue);\n",funDef.funName,funDef.funName);
	fprintf(outFp,"                 goto freePointer;\n");
	fprintf(outFp,"        }\n");
	// �������ĸ�ֵ����
	if ((ret = UnionGenerateCmmPackFldSetCodesForFun(0,funName,"freePointer",fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateServerFunUsingCmmPackToFp:: UnionGenerateCmmPackFldSetCodesForFun [%s]!\n",funName);
		goto errExit;
	}
	fprintf(outFp,"        finishOK = 1;\n");
	// �����ͷ�ָ��Ĵ���
	fprintf(outFp,"freePointer:\n");
	if ((ret = UnionGenerateFreeCodesForPointerFunVarToFp(&funDef,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateServerFunUsingCmmPackToFp:: UnionGenerateFreeCodesForPointerFunVarToFp [%s]!\n",funName);
		return(ret);
	}
	fprintf(outFp,"        if (finishOK)\n");
	fprintf(outFp,"                return(0);\n");
	fprintf(outFp,"        else\n");
	fprintf(outFp,"                return(ret);\n");
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
	funName		��������
	toNewFile	�Ƿ�д�����ļ���
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateServerFunUsingCmmPack(char *funName,int toNewFile)
{
	char	fileName[256+1];
	char	incFileConf[256+1];
	int	ret;
	FILE	*fp;
	int	len;
		
	memset(fileName,0,sizeof(fileName));
	memset(incFileConf,0,sizeof(incFileConf));
	if ((ret = UnionGetFullProgramFileNameOfFun(funName,fileName,incFileConf)) < 0)
	{
		UnionUserErrLog("in UnionGenerateServerFunUsingCmmPack:: UnionGetFullProgramFileNameOfFun [%s]!\n",funName);
		return(ret);
	}
	if (((len = strlen(fileName)) < 2) || (strcmp(fileName+len-2,".c") != 0))
	{
		UnionUserErrLog("in UnionGenerateServerFunUsingCmmPack:: fileName [%s] too short or not a c file!\n",fileName);
		return(errCodeParameter);
	}
	sprintf(fileName+len-2,".AtSvr.c");
	if (!toNewFile)
	{
		fp = fopen(fileName,"a");
		memset(incFileConf,0,sizeof(incFileConf));
	}	
	else
		fp = fopen(fileName,"w");
	if (fp == NULL)
	{
		UnionSystemErrLog("in UnionGenerateServerFunUsingCmmPack:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	if ((ret = UnionGenerateServerFunUsingCmmPackToFp(funName,incFileConf,fp)) < 0)
	{
		UnionUserErrLog("in UnionGenerateServerFunUsingCmmPack:: UnionGenerateServerFunUsingCmmPackToFp [%s]!\n",funName);
		fclose(fp);
		return(ret);
	}
	fclose(fp);
	return(0);
}

/*
����	
	Ϊָ���ĳ����������з����API
�������
	nameOfProgram	��������
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateAllCmmPackServerAPIInSpecProgram(char *nameOfProgram)
{
	char	lineStr[1024+1];
	int	lineLen;
	int	ret;
	FILE	*fp;
	char	fileName[512+1];
	int	funNum = 0,failNum = 0;
	TUnionVarTypeDef	def;
	
	if (nameOfProgram == NULL)
		return(errCodeParameter);
	// ����ļ�����
	UnionGetDefaultFileNameOfVarTypeTagDef("",fileName);
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionGenerateAllCmmPackServerAPIInSpecProgram:: fopen [%s] error!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	memset(&def,0,sizeof(def));
	while (!feof(fp))
	{
		// ���ļ��ж�ȡһ��
		memset(lineStr,0,sizeof(lineStr));
		if ((lineLen = UnionReadOneDataLineFromTxtFile(fp,lineStr,sizeof(lineStr))) < 0)
		{
			if ((lineLen == errCodeEnviMDL_NullLine) || (lineLen == errCodeFileEnd))	// ����
				continue;
			UnionSystemErrLog("in UnionGenerateAllCmmPackServerAPIInSpecProgram:: UnionReadOneDataLineFromTxtFile error in [%s]\n",fileName);
			fclose(fp);
			return(lineLen);
		}
		if (lineLen == 0)	// ����
			continue;
		memset(&def,0,sizeof(def));
		if ((ret = UnionReadVarTypeDefFromStr(lineStr,lineLen,&def)) < 0)
			continue;
		if (def.typeTag != conDesginKeyWordTagFun)	// ���Ǻ���
			continue;
		if (strcmp(nameOfProgram,def.nameOfProgram) != 0)	// ���������������ĺ���
			continue;
		if ((ret = UnionGenerateServerFunUsingCmmPack(def.nameOfType,!funNum)) < 0)
		{
			UnionUserErrLog("in UnionGenerateAllCmmPackServerAPIInSpecProgram:: UnionGenerateServerFunUsingCmmPack [%s]!\n",def.nameOfType);
			printf("��������ʧ��::%s!\n",def.nameOfType);
			failNum++;
		}
		else
		{
			printf("�������ɳɹ�::%s!\n",def.nameOfType);
			funNum++;
		}
	}
	fclose(fp);
	UnionLog("in UnionGenerateAllCmmPackServerAPIInSpecProgram:: [%d] functions generated!\n",funNum);
	printf("���ɳɹ��ĺ�����Ŀ::%04d\n",funNum);
	printf("����ʧ�ܵĺ�����Ŀ::%04d\n",failNum);
	return(funNum);
}

