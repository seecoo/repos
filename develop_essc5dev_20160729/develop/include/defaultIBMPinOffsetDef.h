#ifndef _defaultIBMPinOffsetDef_
#define _defaultIBMPinOffsetDef_

int UnionGetIBMMinPINLength();
char *UnionGetIBMDecimalizationTable();
char *UnionGetIBMUserDefinedData();
char *UnionGetIBMUserDefinedDataByAccNo(char *accNo);
int UnionGetIBMPinCheckData(char *checkData);
int UnionGenerateIBMDecimalizationTable(char *decimalTable);

#endif
