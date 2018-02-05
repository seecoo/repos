// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2006/8/10
// Version:	1.0

#ifndef _ebcdicAscii_
#define _ebcdicAscii_

void UnionAsciiToEbcdic (unsigned char *AsciiBuffer,
		    unsigned char *EbcdicBuffer,
		    int TranLen);

void UnionEbcdicToAscii (unsigned char *EbcdicBuffer,
		    unsigned char *AsciiBuffer,
		    int TranLen);

#endif
