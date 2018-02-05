// Author:	Wolfgang Wang
// Date:	2003/10/08

// Version:	1.0

#ifndef _SJL06LMK_
#define _SJL06LMK_

typedef enum
{
	conLMK0001 = 0,
	conLMK0203,
	conLMK0405,
	conLMK0607,
	conLMK0809,
	conLMK1011,
	conLMK1213,
	conLMK1415,
	conLMK1617,
	conLMK1819,
	conLMK2021,
	conLMK2223,
	conLMK2425,
	conLMK2627,
	conLMK2829,
	conLMK3031,
	conLMK3233,
	conLMK3435,
	conLMK3637,
	conLMK3839,
	conLMK4041,
	conLMK4243,
	conLMK4445,
	conLMK4647,
	conLMK4849,
	conLMK5051,
	conLMK5253,
	conLMK5455,
	conLMK5657,
	conLMK5859,
	conLMK6061,
	conLMK6263,
	conLMK6465,
	conLMK6667,
	conLMK6869,
	conLMK7071,
	conLMK7273,
	conLMK7475,
	conLMK7677,
	conLMK7879,
	conLMK8081,
	conLMK8283,
	conLMK8485,
	conLMK8687,
	conLMK8889,
	conLMK9091,
	conLMK9293,
	conLMK9495,
	conLMK9697,
	conLMK9899,
	conLMKAll
} TUnionSJL06LMKPairIndex;

int UnionConnectSJL06LMKTBL(char *hsmGrpID);
int UnionDisconnectSJL06LMKTBL(char *hsmGrpID);
char *UnionGetSJL06LMKPair(TUnionSJL06LMKPairIndex lmkPairIndex);
TUnionSJL06LMKPairIndex UnionConvertSJL06PairIndex(char *lmkPairIndex);
char *UnionGetNameOfLMKPair(TUnionSJL06LMKPairIndex lmkPair);
char *UnionGetSJL06LMKPairOfSpecifiedHsmGrp(char *hsmGrpID,TUnionSJL06LMKPairIndex lmkPairIndex);
int UnionGetNameOfLMKDefFileOfHsmGrp(char *hsmGrpID,char *fileName);

#endif
