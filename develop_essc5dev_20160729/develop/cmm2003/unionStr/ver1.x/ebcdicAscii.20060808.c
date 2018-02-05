// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2004/7/29
// Version:	1.0

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ebcdicAscii.h"

#define	CBEGIN		0x0e
#define	CEND		0x0f
#define	CENTER		0x0a
#define	ELEMENTS_OF_42		64
#define	ELEMENTS_OF_43		160
#define	ELEMENTS_OF_44		184

#define	CBEGIN		0x0e
#define	CEND		0x0f
#define	CENTER		0x0a

#define		CHINESE_BEGIN	0x0e
#define		CHINESE_END		0x0f

#define		C_FALSE			0x00
#define		C_TRUE			0x01


static unsigned char table42[ELEMENTS_OF_42+1]=
{
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x8a, 0x4d, 0x5b, 0x47, 0x4a, 0x9c,

   0x45, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x40, 0x43, 0x49, 0x48, 0x5a, 0x50,

   0x4c, 0x4e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x51, 0x4b, 0x44, 0x7e, 0x5d, 0x5e,

   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x80, 0x59, 0x42, 0x5f, 0x52, 0x5c, 0x41
};

static unsigned char table43[ELEMENTS_OF_43+1]=
{
   0x00, 0x42, 0x57, 0x58, 0x41, 0x43, 0x92, 0x40,
   0x42, 0x44, 0x89, 0x00, 0x00, 0x00, 0x00, 0x00,

   0x00, 0x46, 0x48, 0x83, 0x85, 0x87, 0x62, 0x8e,
   0x60, 0x95, 0x96, 0x61, 0x00, 0x00, 0x00, 0x00,

   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

   0x00, 0x41, 0x43, 0x45, 0x47, 0x49, 0x4a, 0x4c,
   0x4e, 0x50, 0x52, 0x00, 0x54, 0x56, 0x58, 0x5a,

   0x5c, 0x5e, 0x60, 0x63, 0x65, 0x67, 0x69, 0x6a,
   0x6b, 0x6c, 0x6d, 0x00, 0x00, 0x6e, 0x71, 0x74,

   0x00, 0x4a, 0x77, 0x7a, 0x7d, 0x7e, 0x80, 0x81,
   0x82, 0x84, 0x86, 0x00, 0x88, 0x89, 0x8a, 0x8b,

   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x8c, 0x8d, 0x8f, 0x93, 0x62, 0x63,

   0x4b, 0x4d, 0x4f, 0x51, 0x53, 0x55, 0x57, 0x59,
   0x5b, 0x5d, 0x5f, 0x61, 0x64, 0x66, 0x68, 0x6f,

   0x72, 0x75, 0x78, 0x7b, 0x94, 0x70, 0x73, 0x76,
   0x79, 0x7c, 0x90, 0x91, 0x64, 0x65, 0x00, 0x00
};
   
static unsigned char table44[ELEMENTS_OF_44+1]=
{
   0x00, 0x00, 0x59, 0x5a, 0x7a, 0x7c, 0xf0, 0x9f,
   0xa1, 0xa3, 0x49, 0x5f, 0x78, 0x7d, 0x86, 0x00,

   0x46, 0xa5, 0xa7, 0xe1, 0xe3, 0xe5, 0xc1, 0xec,
   0x00, 0x00, 0x70, 0x47, 0x71, 0x48, 0x72, 0x73,

   0x46, 0x4d, 0x4f, 0x51, 0x53, 0x55, 0x5d, 0x7b,
   0x80, 0x81, 0x8c, 0x99, 0x74, 0x75, 0x8d, 0x76,

   0x7d, 0x4e, 0x50, 0x52, 0x54, 0x56, 0x5e, 0x7c,
   0x7e, 0x82, 0x60, 0x61, 0x4b, 0x9e, 0x77, 0x4c,

   0x00, 0xa0, 0xa2, 0xa4, 0xa6, 0xa8, 0xa9, 0xab,
   0xad, 0xaf, 0xb1, 0x00, 0xb3, 0xb5, 0xb7, 0xb9,

   0xbb, 0xbd, 0xbf, 0xc2, 0xc4, 0xc6, 0xc8, 0xc9,
   0xca, 0xcb, 0xcc, 0x00, 0x00, 0xcd, 0xd0, 0xd3,

   0x00, 0x00, 0xd6, 0xd9, 0xdc, 0xdd, 0xde, 0xdf,
   0xe0, 0xe2, 0xe4, 0x00, 0xe6, 0xe7, 0xe8, 0xe9,

   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0xea, 0xeb, 0xed, 0xf1, 0x00, 0x00,

   0xaa, 0xac, 0xae, 0xb0, 0xb2, 0xb4, 0xb6, 0xb8,
   0xba, 0xbc, 0xbe, 0xc0, 0xc3, 0xc5, 0xc7, 0xce,

   0xd1, 0xd4, 0xd7, 0xda, 0x00, 0xcf, 0xd2, 0xd5,
   0xd8, 0xdb, 0xee, 0xef, 0x78, 0x79, 0x00, 0x00,

   0x90, 0x91, 0x97, 0x98, 0x92, 0x8e, 0x8f, 0x93,
   0x94, 0x95, 0x96, 0x7a, 0x7b, 0x83, 0x84, 0x85,

   0x9a, 0x9b, 0x9c, 0x9d, 0x00, 0x00, 0x00, 0x00
};

static int etogb_41 (unsigned char *s, unsigned char *t)
{
	if (s[1]<0x41)
		return (-2);
	else if (s[1]<0x59)
	{
		t[0] = 0xa6;
		t[1] = s[1] + 0x80;
		return (0);
	}
	else if (s[1]<0x61)
		return (-2);
	else if (s[1]<0x79)
	{
		t[0] = 0xa6;
		t[1] = s[1] + 0x40;
		return (0);
	}
	else if (s[1]<0x80)
		return (-2);
	else if (s[1]<0xa1)
	{
		t[0] = 0xa7;
		t[1] = s[1] + 0x51;
		return (0);
	}
	//else if (s[1]<0xb1)
	//	return (-2);
	/* 41B1->41BA */
	else if (s[1]<0xbb)
	{
		//printf("REturned from 41B1->41BA !\n");
		t[0] = 0xa2;
		// 12/22/2002
		t[1] = s[1] - 0x10;
		return (0);
	}
	else if (s[1]<0xc0)
		return (-2);
	else if (s[1]<0xe1)
	{
		t[0] = 0xa7;
		t[1] = s[1] - 0x1f;
		return (0);
	}
	else if (s[1]<0xf1)
		return (-2);
	else if (s[1]<0xfd)
	{
		t[0] = 0xa2;
		t[1] = s[1];
		return (0);
	}
	else
		return (-2);
}

static int etogb_42 (unsigned char *s, unsigned char *t)
{
	if (s[1]<0x4a)
		return (-2);
	else if (s[1]==0x4a)
	{
		t[0] = 0xa1;
		t[1] = 0xea;
		return (0);
	}
	/* 425F - "￢" */
	else if ( s[1]==0x5f )
	{
		t[0] = 0xa9;
		t[1] = 0x56;
		return (0);
	}
	else if ( s[1]==0x7d )
		return (-40);
	else if (s[1]==0x79)
	{
		t[0] = 0xa3;
		t[1] = 0xe0;
		return (0);
	}
	else if (s[1]<0x51)
	{
		t[0] = 0xa3;
		t[1] = table42[s[1]-0x40] + 0x60;
		if (t[1]<0xe1)
			t[1] ++;
		if (t[1]==0x60)
			return (-2);
		else
			return (0);
	}
	else if (s[1]<0x5a)
		return (-2);
	else if (s[1]<0x62)
	{
		t[0] = 0xa3;
		t[1] = table42[s[1]-0x40] + 0x60;
		if (t[1]<0xe1)
			t[1] ++;
		if (t[1]==0x60)
			return (-2);
		else
			return (0);
	}
	/* 426A - "￤" */
	else if (s[1]==0x6a)
	{
		t[0] = 0xa9;
		t[1] = 0x57;
		return (0);
	}
	else if (s[1]<0x70)
	{
		t[0] = 0xa3;
		t[1] = table42[s[1]-0x40] + 0x60;
		if (t[1]<0xe1)
			t[1] ++;
		if (t[1]==0x60)
			return (-2);
		else
			return (0);
	}
	else if (s[1]<0x79)
		return (-2);
	else if (s[1]<0x80)
	{
		t[0] = 0xa3;
		t[1] = table42[s[1]-0x40] + 0x60;
		if (t[1]<0xe1)
			t[1] ++;
		if (t[1]==0x60)
			return (-2);
		else
			return (0);
	}
	else if (s[1]==0x80)
		return (-2);
	else if (s[1]<0x8a)
	{
		t[0] = 0xa3;
		t[1] = s[1] + 0x60;
		return (0);
	}
	else if (s[1]<0x91)
		return (-2);
	else if (s[1]<0x9a)
	{
		t[0] = 0xa3;
		t[1] = s[1] + 0x59;
		return (0);
	}
	else if (s[1]<0xa1)
		return (-2);
	else if (s[1]==0xa1)
	{
		t[0] = 0xa3;
		t[1] = 0xfe;
		return (0);
	}
	else if (s[1]<0xaa)
	{
		t[0] = 0xa3;
		t[1] = s[1] + 0x51;
		return (0);
	}
	else if (s[1]<0xc0)
		return (-2);
	else if (s[1]==0xc0)
	{
		t[0] = 0xa3;
		t[1] = 0xfb;
		return (0);
	}
	else if (s[1]<0xca)
	{
		t[0] = 0xa3;
		t[1] = s[1];
		return (0);
	}
	else if (s[1]<0xd0)
		return (-2);
	else if (s[1]==0xd0)
	{
		t[0] = 0xa3;
		t[1] = 0xfd;
		return (0);
	}
	else if (s[1]<0xda)
	{
		t[0] = 0xa3;
		t[1] = s[1] - 0x07;
		return (0);
	}
	else if (s[1]==0xe0)
	{
		t[0] = 0xa1;
		t[1] = 0xe7;
		return (0);
	}
	else if (s[1]<0xe2)
		return (-2);
	else if (s[1]<0xea)
	{
		t[0] = 0xa3;
		t[1] = s[1] - 0x0f;
		return (0);
	}
	else if (s[1]<0xf0)
		return (-2);
	else if (s[1]<0xfa)
	{
		t[0] = 0xa3;
		t[1] = s[1] - 0x40;
		return (0);
	}
	else 
		return (-2);
}

static int etogb_43 (unsigned char *s, unsigned char *t)
{
	if ((s[1]<0x41) || (s[1]>0xe0))
		return (-2);
	if ((s[1]>0x5b) && (s[1]<0x81))
		return (-2);
	else if ((s[1]<=0x45) || (s[1]==0xa1))
	{
		t[0] = 0xa1;
		t[1] = table43[s[1]-0x40] + 0x61;
		if (t[1]==0x61)
			return (-2);
		return (0);
	} 
	else if (s[1]==0x4a)
	{
		t[0] = 0xa1;
		t[1] = table43[s[1]-0x40] + 0x60;
		if (t[1]==0x60)
			return (-2);
		return (0);
	}
	else if (s[1]==0xe0)
	{
		t[0] = 0xa3;
		t[1] = 0xdc;
		return (0);
	}
	/* 4358 - "ー" */
	else if ( s[1]==0x58 ) 
	{
		t[0] = 0xa9;
		t[1] = 0x60;
		return 0;
	}
	else if ( s[1]==0x5b )
		return (-40);	    /* !!! cannot found these char in gb table !!! */
	/* 43BE - 43BF */
	else if ( s[1]==0xbe || s[1]==0xbf )
	{
		t[0] = 0xa9;
		t[1] = s[1] - 0x5D;
		return (0);
	}
	/* 43DC - 43DD */
	else if ( s[1]==0xdc || s[1]==0xdd )
	{
		t[0] = 0xa9;
		t[1] = s[1] - 0x79;
		return (0);
	}
	else
	{
		t[0] = 0xa5;
		t[1] = table43[s[1]-0x40] + 0x60;
		if (t[1]<0xe0)
			t[1] ++;
		if (t[1]==0x61)
			return (-4);
	}

	return (0);
}

static int etogb_44 (unsigned char *s, unsigned char *t)
{
	if ((s[1]<0x42) || (s[1]>0xf3))
		return (-2);
	else if ( s[1]==0x5c )
		return (-40);
	/* A966&309D 44DC GBK/5ゝ */
	else if (s[1]==0xDC)
	{
		t[0] = 0xa9;
		t[1] = 0x66;
		return (0);
	}
	/* A967&309E 44DD GBK/5ゞゝ */
	else if (s[1]==0xDD)
	{
		t[0] = 0xa9;
		t[1] = 0x67;
		return (0);
	}
	/* 44EB - "▽" */
	else if (s[1]==0xEB)
	{
		t[0] = 0xa8;
		t[1] = 0x8C;
		return (0);
	}
	/* 44EC - "▼" */
	else if (s[1]==0xEC)
	{
		t[0] = 0xa8;
		t[1] = 0x8B;
		return (0);
	}
	/* A95C&2010 445A GBK/5‐ */
	else if (s[1]==0x5A)
	{
		t[0] = 0xa9;
		t[1] = 0x5C;
		return (0);
	}
	/* A965&3006 445E GBK/5〆 */
	else if (s[1]==0x5E)
	{
		t[0] = 0xa9;
		t[1] = 0x65;
		return (0);
	}
	/* A996&3007 445F GBK/5〇 */
	else if (s[1]==0x5F)
	{
		t[0] = 0xa9;
		t[1] = 0x96;
		return (0);
	}
	/* 446C - "〒" */
	else if (s[1]==0x6C)
	{
		t[0] = 0xa8;
		t[1] = 0x93;
		return (0);
	}
	/* A959&2121 446F GBK/5℡ */
	else if (s[1]==0x6F)
	{
		t[0] = 0xa9;
		t[1] = 0x59;
		return (0);
	}
	/* A95A&3231 446D GBK/5㈱ */
	else if (s[1]==0x6D)
	{
		t[0] = 0xa9;
		t[1] = 0x5A;
		return (0);
	}
	else if (s[1]==0x70)
	{
		t[0] = 0xa3;
		t[1] = 0xde;
		return (0);
	}
	/* 447E - "‥" */
	else if (s[1]==0x7E)
	{
		t[0] = 0xa8;
		t[1] = 0x45;
		return (0);
	}
	else if ((s[1]<0x44) || (s[1]>0xdf))
	{
		t[0] = 0xa1;
		t[1] = table44[s[1]-0x40] + 0x60;
		if (t[1]<0xe0)
			t[1] ++;
		if (t[1]==0x61)
			return (-2);
		else
			return (0);
	}
	else if (s[1]<0x46)
	{
		t[0] = 0xa3;
		t[1] = table44[s[1]-0x40] + 0x60;
		if (t[1]<0xe0)
			t[1] ++;
		if (t[1]==0x61)
			return (-2);
		else
			return (0);
	}
	else if (s[1]==0xdf)
		return (-2);
	else if ((s[1]<0x4a) || (s[1]>0x80))
	{
		t[0] = 0xa4;
		t[1] = table44[s[1]-0x40] + 0x02;
		if (t[1]==0x02)
			return (-2);
		else
			return (0);
	}
	else if (s[1]==0x80)
		return (-2);
	else if ((s[1]<0x4f) || (s[1]>0x5a))
	{
		t[0] = 0xa1;
		t[1] = table44[s[1]-0x40] + 0x60;
		if (t[1]<0xe0)
			t[1] ++;
		if (t[1]==0x61)
			return (-2);
		return (0);
	}
	else if (s[1]==0x4f)
		return (-2);
	else if (s[1]==0x50)
	{
		t[0] = 0xa3;
		t[1] = 0xa7;
		return (0);
	}
	else if (s[1]<0x58)
	{
		t[0] = 0xa4;
		t[1] = table44[s[1]-0x40] + 0x02;
		if (t[1]==0x02)
			return (-2);
		return (0);
	}
	else
		return (-2);
}

static int etogb_45 (unsigned char *s, unsigned char *t)
{
	if (s[1]<0x45) 
		return (-2);
	if (s[1]<0x47)
	{
		t[0] = 0xa1;
		t[1] = s[1] + 0x60;
		return (0);
	}
	else if (s[1]<0x5b)
		return (-2);
	else if (s[1]<0x5d)
	{
		t[0] = 0xa1;
		t[1] = s[1] + 0x61;
		return (0);
	}
	else if ((s[1]<0x62) || (s[1]==0x78))
		return (-2);
	else if (s[1]<0x7b)
	{
		t[0] = 0xa1;
		t[1] = s[1] + 0x61;
		return (0);
	}
	else if ((s[1]==0x88) || (s[1]==0x8b))
	{
		t[0] = 0xa1;
		t[1] = s[1] + 0x60;
		return (0);
	}
	else if (s[1]<0xb1)
		return (-2);
	else if (s[1]<0xd9)
	{
		t[0] = 0xa2;
		t[1] = s[1];
		return (0);
	}
	else if (s[1]<0xe1)
		return (-2);
	else if (s[1]<0xeb)
	{
		t[0] = 0xa2;
		t[1] = s[1] - 0x08;
		return (0);
	}
	else if (s[1]<0xf1)
		return (-2);
	else if (s[1]<0xfb)
	{
		t[0] = 0xa2;
		t[1] = s[1] - 0x0c;
		return (0);
	}
	else 
		return (-2);
}

static int etogb_46 (unsigned char *s, unsigned char *t)
{
	if (s[1]<0x41)
		return (-2);
	else if (s[1]<0x61)
	{
		t[0] = 0xa8;
		t[1] = s[1] + 0x60;
		return (0);
	}
	else if (s[1]<0x65)
		return (-2);
	else if (s[1]<0x8a)
	{
		t[0] = 0xa8;
		t[1] = s[1] + 0x60;
		return (0);
	}
	else if (s[1]<0xa4)
		return (-2);
	else if (s[1]<0xf0)
	{
		t[0] = 0xa9;
		t[1] = s[1];
		return (0);
	}
	else 
		return (-2);
}

static int etogb_80 (unsigned char *s, unsigned char *t)
{
	/* 804F-807C, 807D-807E, 807F, 8081-80A5, 80A6-80B0, 80B1-80C9, 80CA, 80CB, 80CC-80CE, 80CF-80EF */
	if (s[1]<0x4F)
		return (-2);
	/* 804F-807C */
	else if (s[1]>=0x4f && s[1]<=0x7c)
	{
		if (s[1]<0x5b)
		{
			t[0] = 0xa2;
			if (s[1]<0x55)
				t[1] = s[1] + 0x5C;
			else if (s[1]<0x57)
				t[1] = s[1] + 0x8E;
			else if (s[1]<0x59)
				t[1] = s[1] + 0x98;
			else
				t[1] = s[1] + 0xA4;
		}
		else
		if (s[1]<0x66)
		{
			t[0] = 0xa4;
			t[1] = s[1] + 0x99;
		}
		else
		if (s[1]<0x6e)
		{
			t[0] = 0xa5;
			t[1] = s[1] + 0x91;
			//printf("NOW be processing !\n");
		}
		else if (s[1]<0x76)
		{
			t[0] = 0xa6;
			t[1] = s[1] + 0x4b;
		}
		else
		{
			t[0] = 0xa6;
			t[1] = s[1] + 0x63;
		}
	}
	/* 807D-807E */
	else if (s[1]>=0x7d && s[1]<=0x7e)
	{
		t[0] = 0xa6;
		t[1] = s[1] + 0x6f;
	}
	/* 807F */
	else if (s[1]==0x7f)
	{
		t[0] = 0xa6;
		t[1] = 0xf3;
	}
	/* 8081-80A5 */
	else if (s[1]>=0x81 && s[1]<=0xa5)
	{
		if (s[1]<0x8a)
		{
			t[0] = 0xa6;
			t[1] = s[1] + 0x75;
		}
		else if (s[1]<0x99)
		{
			t[0] = 0xa7;
			t[1] = s[1] + 0x38;
		}
		else
		{
			t[0] = 0xa7;
			t[1] = s[1] + 0x59;
		}
	}
	/* 80A6-80B0 */
	else if (s[1]>=0xa6 && s[1]<=0xb0)
	{
		t[0] = 0xa8;
		t[1] = s[1] - 0x10;
	}
	/* 80B1-80C9 */
	else if (s[1]>=0xb1 && s[1]<=0xc9)
	{
		t[0] = 0xa8;
		if (s[1]<0xb5)
			t[1] = s[1] + 0x10;
		else
			t[1] = s[1] + 0x35;
	}
	/* 80CA */
	else if (s[1]==0xca)
	{
		t[0] = 0xa9;
		t[1] = 0x58;
	}
	/* 80CB */
	else if (s[1]==0xcb)
	{
		t[0] = 0xa9;
		t[1] = 0x5b;
	}
	/* 80CC-80CE */
	else if (s[1]>=0xcc && s[1]<=0xce)
	{
		t[0] = 0xa9;
		t[1] = s[1] - 0x6f;
	}
	/* 80CF-80EF */
	else if (s[1]>=0xcf && s[1]<=0xef)
	{
		t[0] = 0xa9;

		if (s[1]<0xdc)
		{
			t[1] = s[1] - 0x38;
		}
		else if (s[1]<0xeb)
		{
			t[1] = s[1] + 0x14;
		}
		else
		{
			t[0] = 0xd7;
			t[1] = s[1] + 0x0F;
		}
	}
	else
	{
		return (-1);
	}
	return 0;
}

static int etogb_cd (unsigned char *s, unsigned char *t)
{
	if (s[1]<0x41)
		return (-2);
	/* 804F-807C */
	else if (s[1]>=0x41 && s[1]<=0x4c)
	{
		t[0] = 0xa6;
		t[1] = s[1] + 0x9f;
	}
	else if (s[1]>=0x4d && s[1]<=0x51)
	{
		t[0] = 0xa6;
		t[1] = s[1] + 0xa1;
	}
	else if (s[1]>=0x52 && s[1]<=0x53)
	{
		t[0] = 0xa6;
		t[1] = s[1] + 0xa2;
	}
	else if (s[1]>=0x54 && s[1]<=0x58)
	{
		t[0] = 0xa8;
		t[1] = s[1] - 0x14;
	}
	else if (s[1]>=0x59 && s[1]<=0xab)
	{
		t[0] = 0xa8;
		if(s[1]<=0x7f)
			t[1] = s[1] - 0x13;
		else if(s[1]==0x80)
			return -2;
		else if(s[1]<=0x92)
			t[1] = s[1] - 0x14;
		else if(s[1]<=0x9d)
			t[1] = s[1] - 0x13;
		else if(s[1]<=0xa3)
			t[1] = s[1] - 0x11;
		else if(s[1]<=0xa5)
			t[1] = s[1] - 0x10;
		else/* if(s[1]<=0xab)*/
			t[1] = s[1] + 0x15;
	}
	else if (s[1]>=0xac && s[1]<=0xee)
	{
		t[0] = 0xa9;
		if(s[1]<=0xc1)
			t[1] = s[1] - 0x6c;
		else if(s[1]<=0xd8)
			t[1] = s[1] - 0x5a;
		else
			t[1] = s[1] - 0x59;
	}
	else
	{
		return (-1);
	}
	return 0;
}

/*
   CharNo  Name       GBKRange    UCSRange    HostRange   Zone     CharNo
-  ------  ---------  ---------   ---------   ---------   -------  ------
%  14139   NewGBK     8140-A0FE   4E00-72DB:  8141-CC67?  GBK/3     6080
                      AA40-FD9B   72DC-9FA5:              GBK/4     8059

     302   GBKSymbol/ A2A1-A996:  0144-33D5:  CD41-CE97?  GBK/1,5    274
&          CompatCJK  FD9C-FEA0:  E7C7-FFE4:           ??             28

@   1222   UDC-1&2    AAA1-AFFE   E000-E233   7641-78FD?  GBK/U      564
                      F8A1-FEFE   E234-E4C5   7941-7C9F?  GBK/U      658

!    672   UDC-3      A140-A792   E4C6-E791   7CA0 7FFD?  GBK/U      658
                      A793-A7A0   E792-E79F   8041-804E?  GBK/U       14

#    160   NoUse      A2AB-A9FE:  E586-E80F:  804F-80EA?  GBK/1,5    155
                      D7FA-D7FE   E810-E814   80EB-80EF?  GBK/2        5

*   7445   OldGB      A1A1-A9FE:  0000-33FF:  4040-46EF:  GBK/1      682
                                  FE00-FFFF:

                      B0A1-D7F9   4E00-9FA5:  48A0-5C9A   GBK/2     3755

                      D8A1-F7FE   4E00-9FA5:  5CA0-6C9F   GBK/2     3008
---------------------------------------------------------------------------
   23940              :  discontinuity code points area            23940
                      ?  temporary code points assigned by C.C.
                      ?? 28 map to Host code among 31 IBM select symbols
                         Host code: 427D,435B,445C no corresponded GBK code
*/
static int etogbGBK125 (unsigned char *s, unsigned char *t)
{
/*
   CharNo  Name       GBKRange    UCSRange    HostRange   Zone     CharNo
-  ------  ---------  ---------   ---------   ---------   -------  ------
*   7445   OldGB      A1A1-A9FE:  0000-33FF:  4040-46EF:  GBK/1      682
                                  FE00-FFFF:

                      B0A1-D7F9   4E00-9FA5:  48A0-5C9A   GBK/2     3755

                      D8A1-F7FE   4E00-9FA5:  5CA0-6C9F   GBK/2     3008
---------------------------------------------------------------------------
*/
	/* IBM chinese characters Not in GBK1/2 range */
	/*
	if (!(memcmp(s,"\x40\x40",2)>=0&&memcmp(s,"\x46\xef",2)<=0||
		  memcmp(s,"\x48\xa0",2)>=0&&memcmp(s,"\x5c\x9a",2)<=0||
		  memcmp(s,"\x5c\xa0",2)>=0&&memcmp(s,"\x6c\x9f",2)<=0))
		return (-2);
	*/
	
	if ((s[0] == 0x40) && (s[1] == 0x40))
	{
		t[0] = 0xa1;
		t[1] = 0xa1;
		return (0);
	}

	if (s[0] == 0x41)
		return (etogb_41 (s, t));

	if (s[0] == 0x42)
		return (etogb_42 (s, t));

	if (s[0] == 0x43)
		return (etogb_43 (s, t));

	if (s[0] == 0x44)
		return (etogb_44 (s, t));

	if (s[0] == 0x45)
		return (etogb_45 (s, t));

	if (s[0] == 0x46)
		return (etogb_46 (s, t));

	/* 804F-807C, 807D-807E, 807F, 8081-80A5, 80A6-80B0, 80B1-80C9, 80CA, 80CB, 80CC-80CE, 80CF-EF */
	if (s[0] == 0x80)
		return (etogb_80 (s, t));

	if (s[0] == 0xcd)
		return (etogb_cd (s, t));

	if ((s[0]>0x48) && (s[0]<=0x6c))
	{
		if ((s[1]>=0x40) && (s[1]<0xa0))
		{
			t[0] = 0xb0 + (s[0] - 0x48) * 2 - 1;
			if (s[1]<0x80)
				t[1] = s[1] + 0x60;
			else if (s[1]>0x80)
				t[1] = s[1] + 0x5f;
			else
				return (-2);
		}
		else if ((s[1]>=0xa0) && (s[1]<0xfe))
		{
			t[0] = 0xb0 + (s[0] - 0x48) * 2;
			t[1] = s[1] + 0x01;
		}
		else
			return (-2);
		return (0);
	}
	else if ((s[0]==0x48) && (s[1]>=0xa0))
	{
		t[0] = 0xb0 + (s[0] - 0x48) * 2;
		t[1] = s[1] + 0x01;
		return (0);    
	}
	return (-1);
}

int etogb (unsigned char *s, unsigned char *t)
{
	int  rc;

	if ( !(rc = etogbGBK125 (s, t)) )
	{
		;
	}
	else if ( !(rc = ibm2cnGBK34(s, t)) )
	{
		;
	}
	else if ( !(rc = ibm2cnGBKU(s, t)) )
	{
		;
	}
	else
		rc = ibm2cnGBK4ext(s, t);

	return rc;

}


/********************************************************************/

static unsigned char TableATOE[256] = 
	   { 0x00, 0x01, 0x02, 0x03, 0x37, 0x2D, 0x2E, 0x2F,
	     0x16, 0x05, 0x25, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	     0x10, 0x11, 0x12, 0x13, 0x3C, 0x3D, 0x32, 0x26,
	     0x18, 0x19, 0x3F, 0x27, 0x1C, 0x1D, 0x1E, 0x1F,
	     0x40, 0x5A, 0x7F, 0x7B, 0x5B, 0x6C, 0x50, 0x7D,
	     0x4D, 0x5D, 0x5C, 0x4E, 0x6B, 0x60, 0x4B, 0x61,
	     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
	     0xF8, 0xF9, 0x7A, 0x5E, 0x4C, 0x7E, 0x6E, 0x6F,
	     0x7C, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
	     0xC8, 0xC9, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6,
	     0xD7, 0xD8, 0xD9, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6,
	     0xE7, 0xE8, 0xE9, 0x4A, 0xE0, 0x4F, 0x5F, 0x6D,
	     0x79, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
	     0x88, 0x89, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96,
	     0x97, 0x98, 0x99, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
	     0xA7, 0xA8, 0xA9, 0xC0, 0x6A, 0xD0, 0xA1, 0x07,
	     0x20, 0x21, 0x22, 0x23, 0x24, 0x15, 0x06, 0x17,
	     0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x09, 0x0A, 0x1B,
	     0x30, 0x31, 0x1A, 0x33, 0x34, 0x35, 0x36, 0x08,
	     0x38, 0x39, 0x3A, 0x3B, 0x04, 0x14, 0x3E, 0xE1,
	     0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
	     0x49, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
	     0x58, 0x59, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
	     0x68, 0x69, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75,
	     0x76, 0x77, 0x78, 0x80, 0x8A, 0x8B, 0x8C, 0x8D,
	     0x8E, 0x8F, 0x90, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E,
	     0x9F, 0xA0, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
	     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
	     0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
	     0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xDA, 0xDB,
	     0xDC, 0xDD, 0xDE, 0xDF, 0xEA, 0xEB, 0xEC, 0xED,
	     0xEE, 0xEF, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
	     };
	     
static unsigned char TableETOA[256] = 
	   { 0x00, 0x01, 0x02, 0x03, 0x9C, 0x09, 0x86, 0x7F,
	     0x97, 0x8D, 0x8E, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	     0x10, 0x11, 0x12, 0x13, 0x9D, 0x85, 0x08, 0x87,
	     0x18, 0x19, 0x92, 0x8F, 0x1C, 0x1D, 0x1E, 0x1F,
	     0x80, 0x81, 0x82, 0x83, 0x84, 0x0A, 0x17, 0x1B,
	     0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x05, 0x06, 0x07,
	     0x90, 0x91, 0x16, 0x93, 0x94, 0x95, 0x96, 0x04,
	     0x98, 0x99, 0x9A, 0x9B, 0x14, 0x15, 0x9E, 0x1A,
	     0x20, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
	     0xA7, 0xA8, 0x5B, 0x2E, 0x3C, 0x28, 0x2B, 0x5D,
	     0x26, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
	     0xB0, 0xB1, 0x21, 0x24, 0x2A, 0x29, 0x3B, 0x5E,
	     0x2D, 0x2F, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
	     0xB8, 0xB9, 0x7C, 0x2C, 0x25, 0x5F, 0x3E, 0x3F,
	     0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0, 0xC1,
	     0xC2, 0x60, 0x3A, 0x23, 0x40, 0x27, 0x3D, 0x22,
	     0xC3, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
	     0x68, 0x69, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9,
	     0xCA, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70,
	     0x71, 0x72, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0,
	     0xD1, 0x7E, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
	     0x79, 0x7A, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,
	     0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
	     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
	     0x7B, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
	     0x48, 0x49, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED,
	     0x7D, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,
	     0x51, 0x52, 0xEE, 0xEF, 0xF0, 0xF1, 0xF2, 0xF3,
	     0x5C, 0x9F, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
	     0x59, 0x5A, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9,
	     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
	     0x38, 0x39, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
	     };

void UnionAsciiToEbcdic (unsigned char *AsciiBuffer,
		    unsigned char *EbcdicBuffer,
		    int TranLen)
{
	int  i;
	for (i=0; i<TranLen; i++)
    {
		EbcdicBuffer[i] = TableATOE[AsciiBuffer[i]];
    }
	return;
}

void UnionEbcdicToAscii (unsigned char *EbcdicBuffer,
		    unsigned char *AsciiBuffer,
		    int TranLen)
{
	int  i;
	for (i=0; i<TranLen; i++)
    {
		AsciiBuffer[i] = TableETOA[EbcdicBuffer[i]];
    }
	return;
}

/********************************************************************/







static unsigned char table81[200] =
{
   0x40, 0x44, 0x41, 0x45, 0x45, 0x46, 0x60, 0x5b,
   0x5d, 0x4a, 0xa1, 0x7c, 0x7f, 0x61, 0x71, 0x62,

   0x72, 0x63, 0x73, 0x64, 0x74, 0x65, 0x75, 0x42,
   0x43, 0x42, 0x43, 0x5b, 0x5c, 0x66, 0x76, 0x4b,

   0x7a, 0x7b, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
   0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,

   0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
   0x4c, 0x79, 0x7a, 0x67, 0x77, 0x4d, 0x78, 0x00,

   0x68, 0x69, 0x79, 0xed, 0xee, 0xef, 0x4e, 0xe0,
   0x88, 0x4a, 0x4a, 0x8b, 0x6a, 0x6e, 0xe5, 0xe6,

   0xe0, 0xe1, 0xe4, 0xe7, 0xe8, 0xe9, 0xea, 0xe2,
   0xe3, 0x6b, 0xf0, 0xf1, 0xf2, 0xf3, 0x7d, 0x00,
};

static unsigned char table82[200] =
{
   0x5a, 0x7f, 0x7b, 0x5b, 0x6c, 0x50, 0x50, 0x4d,
   0x5d, 0x5c, 0x4e, 0x6b, 0x60, 0x4b, 0x61, 0xf0,

   0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
   0xf9, 0x7a, 0x5e, 0x4c, 0x7e, 0x6e, 0x6f, 0x7c,

   0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8,
   0xc9, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,

   0xd8, 0xd9, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
   0xe8, 0xe9, 0x44, 0xe0, 0x45, 0x70, 0x6d, 0x00,

   0x79, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
   0x88, 0x89, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96,

   0x97, 0x98, 0x99, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6,
   0xa7, 0xa8, 0xa9, 0xc0, 0x4f, 0xd0, 0xa1, 0x47,

   0x81, 0x48, 0x82, 0x49, 0x83, 0x51, 0x84, 0x52,
   0x85, 0x86, 0xc0, 0x87, 0xc1, 0x88, 0xc2, 0x89,

   0xc3, 0x8a, 0xc4, 0x8c, 0xc5, 0x8d, 0xc6, 0x8e,
   0xc7, 0x8f, 0xc8, 0x90, 0xc9, 0x91, 0xca, 0x92,

   0xcb, 0x56, 0x93, 0xcc, 0x94, 0xcd, 0x95, 0xce,
   0x96, 0x97, 0x98, 0x99, 0x9a, 0x9d, 0xcf, 0xd5,

   0x9e, 0xd0, 0xd6, 0x9f, 0xd1, 0xd7, 0xa2, 0xd2,
   0xd8, 0xa3, 0xd3, 0xd9, 0xa4, 0xa5, 0xa6, 0xa7,

   0xa8, 0x53, 0xa9, 0x54, 0xaa, 0x55, 0xac, 0xad,
   0xae, 0xaf, 0xba, 0xbb, 0x57, 0xbc, 0xda, 0xdb,

   0x46, 0xbd, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static unsigned char table83[200] = 
{
   0x47, 0x81, 0x48, 0x82, 0x49, 0x83, 0x51, 0x84,
   0x52, 0x85, 0x86, 0xc0, 0x87, 0xc1, 0x88, 0xc2,
   
   0x89, 0xc3, 0x8a, 0xc4, 0x8c, 0xc5, 0x8d, 0xc6,
   0x8e, 0xc7, 0x8f, 0xc8, 0x90, 0xc9, 0x91, 0xca,
   
   0x92, 0xcb, 0x56, 0x93, 0xcc, 0x94, 0xcd, 0x95,
   0xce, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9d, 0xcf,
   
   0xd5, 0x9e, 0xd0, 0xd6, 0x9f, 0xd1, 0xd7, 0xa2,
   0xd2, 0xd8, 0xa3, 0xd3, 0xd9, 0xa4, 0xa5, 0xa6,
   
   0xa6, 0xa7, 0xa8, 0x53, 0xa9, 0x54, 0xaa, 0x55,
   0xac, 0xad, 0xae, 0xaf, 0xba, 0xbb, 0x57, 0xbc,
   
   0xda, 0xdb, 0x46, 0xbd, 0xd4, 0x59, 0x5a, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x61,
};

static int gbtoe_a1 (unsigned char *ss, unsigned char *t)
{
	unsigned char s[3];
  
	if (ss[1]<0xa1)
		return (-2);
	else if (ss[1]==0xa1)
	{
		t[0] = t[1] = 0x40;
		return (0);
	}
	else if (ss[1]<0xe0)
		s[1] = ss[1] - 0x61;
	else
		s[1] = ss[1] - 0x60;

	if ((s[1]==0x41) || (s[1]==0x42) || (s[1]==0x43)
		|| (s[1]==0x4a) || (s[1]==0x57) || (s[1]==0x58)
		|| (s[1]==0x89))
		t[0] = 0x43;
	else if ((s[1]==0x87) || (s[1]==0x8a))
		t[0] = 0x42;
	else if (s[1]==0x78)
		t[0] = 0x44;
	else if ((s[1]==0x44) || (s[1]==0x45) || (s[1]==0x5b)
		|| (s[1]==0x5c) || (s[1]==0x88) || (s[1]==0x8b)
		|| ((s[1]>0x61) && (s[1]<0x7b))
		|| ((s[1]>0xae) && (s[1]<0xed)))
		t[0] = 0x45;
	else if (((s[1]>0x45) && (s[1]<0x62)) || ((s[1]>0x7a) && (s[1]<0x9f)))
		t[0] = 0x44;
	else if ((s[1]>0xee) && (s[1]<0xfb))
		t[0] = 0x41;
	else
		return (-2);

	t[1] = table81[s[1]-0x40];
	if (t[1] == 0x00)
		return (-2);

	return (0);
}

static int gbtoe_a2 (unsigned char *s, unsigned char *t)
{
	if (s[1]<0xa1)
	{
		/* A2A1-A2FE */
		//printf("Returned from here A2A1-A2FE !\n");
		return (-2);
	}
	else if (s[1]<=0xaa)
	{
		/* A2A1-A2AA */
		t[0] = 0x41;
		t[1] = s[1] + 0x10;
		return (0);
	}
	else if (s[1]<=0xb0)
	{
		/* A2AB-A2B0 */
		t[0] = 0x80;
		t[1] = s[1] - 0x5c;
		return (0);
	}
	else if (s[1]>=0xe3 && s[1]<=0xe4)
	{
		/* A2E3-A2E4 */
		t[0] = 0x80;
		t[1] = s[1] - 0x8e;
		return (0);
	}
	else if (s[1]<0xd9)
	{
		t[0] = 0x45;
		t[1] = s[1];
		return (0);
	}
	/* A2E2*2469 45EA GBK/1⑩ */
	else if (s[1]<=0xe2)
	{
		t[0] = 0x45;
		t[1] = s[1] + 0x08;
		return (0);
	}
	else if (s[1]<0xe5)
		return (-2);
	else if (s[1]<0xef)
	{
		t[0] = 0x45;
		t[1] = s[1] + 0x0c;
		return (0);
	}
	else if (s[1]<=0xf0)
	{
		t[0] = 0x80;
		t[1] = s[1] - 0x98;
		return (0);
	}
	else if (s[1]<0xfd)
	{
		t[0] = 0x41;
		t[1] = s[1];
		return (0);
	}
	else if (s[1]<=0xfe)
	{
		t[0] = 0x80;
		t[1] = s[1] - 0xa4;
		return (0);
	}
	else
		return (-2);
}

static int gbtoe_a3 (unsigned char *ss, unsigned char *t)
{
	unsigned char s[3];

	if ((ss[1]>0xa0) && (ss[1]<0xe0))
		s[1] = ss[1] - 0x61;
	else if ((ss[1]>=0xe0) && (ss[1]<0xff))
		s[1] = ss[1] - 0x60;
	else 
		return (-2);

	if (s[1]==0x7b)
	{
		t[0] = 0x43;
		t[1] = 0xe0;
		return (0);
	}
	else if ((s[1]==0x46) || ((s[1]>0x79) && (s[1]<0x7e)))
		t[0] = 0x44;
	else if ((s[1]>=0x40) && (s[1]<0x9f))
		t[0] = 0x42;
	else
		return (-2);

	t[1] = table82[s[1]-0x40];
	if (t[1]==0x00)
		return (-2);

	return (0);
}

static int gbtoe_a4 (unsigned char *ss, unsigned char *t)
{
	unsigned char s[3];

	if ( ss[1]<0xa1 )
		return (-2);
	/* A4F4->A4FE */
	else if ( ss[1]>0xf3 )
	{
		t[0] = 0x80;
		t[1] = ss[1] - 0x99;
		return (0);
	}
	else
		s[1] = ss[1] - 0x02;

	t[0] = 0x44;
	t[1] = table82[s[1]-0x40];
	if (t[1]==0x00)
		return (-2);

	return (0);
}

static int gbtoe_a5 (unsigned char *ss, unsigned char *t)
{
	unsigned char s[3];

	if (ss[1]<0xa1)
		return (-2);
	else if (ss[1]<0xe0)
		s[1] = ss[1] - 0x61;
	else if (ss[1]<0xf7)
		s[1] = ss[1] - 0x60;
	/* A5F7->A5FE */
	else if (ss[1]<=0xfe)
	{
		t[0] = 0x80;
		t[1] = ss[1] - 0x91;
		return 0;
	}
	else
		return (-2);

	t[0] = 0x43;
	t[1] = table83 [s[1]-0x40];

	if (t[1]==0x00)
		return (-2);

	return (0);
}

static int gbtoe_a6 (unsigned char *s, unsigned char *t)
{
	if (s[1]<0xa1)
		return (-4);
	else if (s[1]<0xb9)
	{
		t[0] = 0x41;
		t[1] = s[1] - 0x40;
		return (0);
	}
	/* A6B9->A6C0 */
	else if (s[1]<0xc1)
	{
		t[0] = 0x80;
		t[1] = s[1] - 0x4b;
		return (0);
	}
	/* A6B9->A6DF, A6E0->A6FE */
	else if (s[1]<0xd9)
	{
		t[0] = 0x41;
		t[1] = s[1] - 0x80;
		return (0);
	}
	/* A6D9->A6DF */
	else if (s[1]<=0xdf)
	{
		t[0] = 0x80;
		t[1] = s[1] - 0x63;
		return (0);
	}
	/* A6E0->A6EB */
	else if (s[1]<=0xeb)
	{
		t[0] = 0xcd;
		t[1] = s[1] - 0x9f;
		return (0);
	}
	/* A6EC - A6ED */
	else if (s[1]<=0xed)
	{
		t[0] = 0x80;
		t[1] = s[1] - 0x6f;
		return (0);
	}
	/* A6EE - A6F2 */
	else if (s[1]<0xf3)
	{
		t[0] = 0xcd;
		t[1] = s[1] - 0xa1;
		return (0);
	}
	/* A6F3 */
	else if (s[1]==0xf3)
	{
		t[0] = 0x80;
		t[1] = 0x7f;
		return (0);
	}
	/* A6E4 - A6F5 */
	else if (s[1]<0xf6)
	{
		t[0] = 0xcd;
		t[1] = s[1] - 0xa2;
		return (0);
	}
	/* A6F6 - A6FE */
	else if (s[1]<0xff)
	{
		t[0] = 0x80;
		t[1] = s[1] - 0x75;
		return (0);
	}
	else
		return (-3);
}

static int gbtoe_a7 (unsigned char *s, unsigned char *t)
{
	if (s[1]<0xa1)
		return (-2);
	else if (s[1]<0xc2)
	{
		t[0] = 0x41;
		t[1] = s[1] + 0x1f;
		return (0);
	}
	/* A7C2->A7D0 */
	else if (s[1]<0xd1)
	{
		t[0] = 0x80;
		t[1] = s[1] - 0x38;
		return (0);
	}
	else if (s[1]<0xd1)
		return (-2);
	else if (s[1]<0xf2)
	{
		t[0] = 0x41;
		t[1] = s[1] - 0x51;
		return (0);
	}
	/* A7F2 - A7FE */
	else if (s[1]<0xff)
	{
		t[0] = 0x80;
		t[1] = s[1] - 0x59;
		return (0);
	}
	else
		return (-2);
}

static int gbtoe_a8 (unsigned char *s, unsigned char *t)
{
	if (s[1]<0x40)
		return (-2);
	/* A840->A844 */
	else if (s[1]<0x45)
	{
		t[0] = 0xcd;
		t[1] = s[1] + 0x14;
		return (0);
	}
	/* A845 */
	else if (s[1]==0x45)
	{
		t[0] = 0x44;
		t[1] = 0x7e;
		return (0);
	}
	/* A846->A86C */
	else if (s[1]<0x6d)
	{
		t[0] = 0xcd;
		t[1] = s[1] + 0x13;
		return (0);
	}
	/* A86D->A87E */
	else if (s[1]<0x7f)
	{
		t[0] = 0xcd;
		t[1] = s[1] + 0x14;
		return (0);
	}
	/* A880->A88A */
	else if (s[1]<0x8b)
	{
		t[0] = 0xcd;
		t[1] = s[1] + 0x13;
		return (0);
	}
	/* A88B */
	else if (s[1]==0x8b)
	{
		t[0] = 0x44;
		t[1] = 0xec;
		return (0);
	}
	/* A88C */
	else if (s[1]==0x8c)
	{
		t[0] = 0x44;
		t[1] = 0xeb;
		return (0);
	}
	/* A88D->A892 */
	else if (s[1]<0x93)
	{
		t[0] = 0xcd;
		t[1] = s[1] + 0x11;
		return (0);
	}
	/* A893 */
	else if (s[1]==0x93)
	{
		t[0] = 0x44;
		t[1] = 0x6c;
		return (0);
	}
	/* A894->A895 */
	else if (s[1]<0x96)
	{
		t[0] = 0xcd;
		t[1] = s[1] + 0x10;
		return (0);
	}
	/* A896->A8A0 */
	else if (s[1]<0xa1)
	{
		t[0] = 0x80;
		t[1] = s[1] + 0x10;
		return (0);
	}
	/* A8A0->A8C4 */
	else if (s[1]>=0xc1 && s[1]<0xc5)
	{
		t[0] = 0x80;
		t[1] = s[1] - 0x10;
		return (0);
	}
	/* A8E9*3129 4689 GBK/1 */
	else if (s[1]==0xe9)
	{
		t[0] = 0x46;
		t[1] = 0x89;
		return (0);
	}
	else if (s[1]<0xbb)
	{
		t[0] = 0x46;
		t[1] = s[1] - 0x60;
		return (0);
	}
	// 12/22/2002
	else if (s[1]<0xc1)
	{
		t[0] = 0xcd;
		t[1] = s[1] - 0x15;
		return (0);
	}
	else if (s[1]<0xc5)
		return (-2);
	else if (s[1]<0xe9)
	{
		t[0] = 0x46;
		t[1] = s[1] - 0x60;
		return (0);
	}
	/* A8EA#E7CD 80B5 GBK/1, A8FE#E7E1 80C9 GBK/1 */
	else if (s[1]>0xe9)
	{
		t[0] = 0x80;
		t[1] = s[1] - 0x35;
		return (0);
	}
	else
		return (-2);
}

static int gbtoe_a9 (unsigned char *s, unsigned char *t)
{
	/* A940->A9FE */
	if ((s[1]>0xa3) && (s[1]<0xf0))
	{
		t[0] = 0x46;
		t[1] = s[1];
		return (0);
	}
	/* A9A1#E7FE 80D9 GBK/1, A9A3#E800 80DB GBK/1 */
	else if (s[1]>=0xa1 && s[1]<=0xa3)
	{
		t[0] = 0x80;
		t[1] = s[1] + 0x38;
		return (0);
	}
	/* A9F0#E801 80DC GBK/1, A9FE#E80F 80EA GBK/1 */
	else if (s[1]>=0xf0 && s[1]<=0xfe)
	{
		t[0] = 0x80;
		t[1] = s[1] - 0x14;
		return (0);
	}
	/* A940 3021 CDAC GBK/5, A955 FE30 CDC1 GBK/5 */
	else if (s[1]>=0x40 && s[1]<=0x55)
	{
		t[0] = 0xcd;
		t[1] = s[1] + 0x6c;
		return (0);
	}
	/* A956&FFE2 425F GBK/5 */
	else if ( s[1]==0x56 )
	{
		t[0] = 0x42;
		t[1] = 0x5f;
		return (0);
	}
	/* A957&FFE4 426A GBK/5 */
	else if ( s[1]==0x57 )
	{
		t[0] = 0x42;
		t[1] = 0x6a;
		return (0);
	}
	/* A958#E7E2 80CA GBK/5 */
	else if ( s[1]==0x58 )
	{
		t[0] = 0x80;
		t[1] = 0xca;
		return (0);
	}
	/* A959&2121 446F GBK/5 */
	else if ( s[1]==0x59 )
	{
		t[0] = 0x44;
		t[1] = 0x6f;
		return (0);
	}
	/* A95A&3231 446D GBK/5 */
	else if ( s[1]==0x5a )
	{
		t[0] = 0x44;
		t[1] = 0x6d;
		return (0);
	}
	/* A95B#E7E3 80CB GBK/5 */
	else if ( s[1]==0x5b )
	{
		t[0] = 0x80;
		t[1] = 0xcb;
		return (0);
	}
	/* A95C&2010 445A GBK/5 */
	else if ( s[1]==0x5c )
	{
		t[0] = 0x44;
		t[1] = 0x5a;
		return (0);
	}
	/* A95D#E7E4 80CC GBK/5, A95F#E7E6 80CE GBK/5 */
	else if (s[1]>=0x5d && s[1]<=0x5f)
	{
		t[0] = 0x80;
		t[1] = s[1] + 0x6f;
		return (0);
	}
	/* A960&30FC 4358 GBK/5 */
	else if ( s[1]==0x60 )
	{
		t[0] = 0x43;
		t[1] = 0x58;
		return (0);
	}
	/* A961&309B 43BE GBK/5, A962&309C 43BF GBK/5 */
	else if ( s[1]==0x61 || s[1]==0x62)
	{
		t[0] = 0x43;
		t[1] = s[1] + 0x5d;
		return (0);
	}
	/* A963&30FD 43DC GBK/5, A964&30FE 43DD GBK/5 */
	else if ( s[1]==0x63 || s[1]==0x64)
	{
		t[0] = 0x43;
		t[1] = s[1] + 0x79;
		return (0);
	}
	/* A965&3006 445E GBK/5 */
	else if ( s[1]==0x65 )
	{
		t[0] = 0x44;
		t[1] = 0x5e;
		return (0);
	}
	/* A966&309D 44DC GBK/5, A967&309E 44DD GBK/5 */
	else if ( s[1]==0x66 || s[1]==0x67)
	{
		t[0] = 0x44;
		t[1] = s[1] + 0x76;
		return (0);
	}
	/* A968 FE49 CDC2 GBK/5, A995 E7F3 CDEE GBK/5 */
	else if (s[1]>=0x68 && s[1]<=0x95)
	{
		t[0] = 0xcd;
		if(s[1]<0x7f)
			t[1] = s[1] + 0x5a;
		else if(s[1]>0x7f)
			t[1] = s[1] + 0x59;
		else return (-1);
		return (0);
	}
	/* A996&3007 445F GBK/5 */
	else if ( s[1]==0x96 )
	{
		t[0] = 0x44;
		t[1] = 0x5f;
		return (0);
	}
	/* A997#E7F4 80CF GBK/5, A9A0#E7FD 80D8 GBK/5 */
	else if (s[1]>=0x97 && s[1]<=0xa0)
	{
		t[0] = 0x80;
		t[1] = s[1] + 0x38;
		return (0);
	}
	else
		return (-2);
}

static int gbtoeGBK125 (unsigned char *s, unsigned char *t)
{
	/*
	   CharNo  Name       GBKRange    UCSRange    HostRange   Zone     CharNo
	-  ------  ---------  ---------   ---------   ---------   -------  ------
	*   7445   OldGB      A1A1-A9FE:  0000-33FF:  4040-46EF:  GBK/1      682
	                                  FE00-FFFF:
	
	                      B0A1-D7F9   4E00-9FA5:  48A0-5C9A   GBK/2     3755
	
	                      D8A1-F7FE   4E00-9FA5:  5CA0-6C9F   GBK/2     3008
	---------------------------------------------------------------------------
*/
	/* GB chinese characters Not in GBK1/2 range */
	/*
	if (!(memcmp(s,"\xa1\xa1",2)>=0&&memcmp(s,"\xa9\xfe",2)<=0||
		  memcmp(s,"\xb0\xa1",2)>=0&&memcmp(s,"\xd7\xf9",2)<=0||
		  memcmp(s,"\xd8\xa1",2)>=0&&memcmp(s,"\xf7\xfe",2)<=0))
		return (-2);
	*/

	/* put the GBK/5 2 exceptional chars here !!! */
	/* A8A0#E7C6 80B0 GBK/5 */
	if ( s[0]==0xa8 && s[1]==0xa0 )
	{
		t[0] = 0x80;
		t[1] = 0xb0;
		return (0);
	}
	/* A9A0#E7FD 80D8 GBK/5 */
	if ( s[0]==0xa9 && s[1]==0xa0 )
	{
		t[0] = 0x80;
		t[1] = 0xd8;
		return (0);
	}

	if ( s[1]==0xa0 || s[1]==0xff )
		return (-1);
  
	if (s[0]==0xa1)
		return (gbtoe_a1 (s, t));
  
	if (s[0]==0xa2)
		return (gbtoe_a2 (s, t));
  
	if (s[0]==0xa3)
		return (gbtoe_a3 (s, t));
  
	if (s[0]==0xa4)
		return (gbtoe_a4 (s, t));
  
	if (s[0]==0xa5)
		return (gbtoe_a5 (s, t));

	if (s[0]==0xa6)
		return (gbtoe_a6 (s, t));

	if (s[0]==0xa7)
		return (gbtoe_a7 (s, t));

	if (s[0]==0xa8)
		return (gbtoe_a8 (s, t));

	if (s[0]==0xa9)
		return (gbtoe_a9 (s, t));

	if ((s[0]>=0xb0) && (s[0]<=0xf7))
	{
		if (s[1]<0xa0)
			return (-2);

		t[0] = (s[0] - 0xb0)/0x02 + (s[0] - 0xb0)%0x02+ 0x48;
		if (s[0]%0x02)
		{
			if ((s[1]>0xa0) && (s[1]<=0xdf))
			{
				t[1] = s[1] -0x60;
			}
			else
			{
				// 12/22/2002Ro
				if(s[0]==0xd7&&s[1]>0xf9)
				{
					t[0] = 0x80;
					t[1] = s[1] - 0xf;
				}
				else
					t[1] = s[1] -0x5f;
				
			}
		}
		else
		{
			t[1] = s[1] -0x01;
		}
		return (0);
	}
  
	return (-1);
}

int gbtoe (unsigned char *s, unsigned char *t)
{
	int rc;

	if ( !(rc = gbtoeGBK125 (s, t) ))
	{
		;
	}
	else if ( !(rc = cn2ibmGBK34 (s, t) ))
	{
		;
	}
	else if ( !(rc = cn2ibmGBK4ext (s, t) ))
	{
		;
	}
	else
		rc = cn2ibmGBKU (s, t);

	return rc;
}


/********************************************************************/
/*
   CharNo  Name       GBKRange    UCSRange    HostRange   Zone     CharNo
-  ------  ---------  ---------   ---------   ---------   -------  ------
%  14139   NewGBK     8140-A0FE   4E00-72DB:  8141-CC67?  GBK/3     6080
                      AA40-FD9B   72DC-9FA5:              GBK/4     8059

     302   GBKSymbol/ A2A1-A996:  0144-33D5:  CD41-CE97?  GBK/1,5    274
&          CompatCJK  FD9C-FEA0:  E7C7-FFE4:           ??             28

@   1222   UDC-1&2    AAA1-AFFE   E000-E233   7641-78FD?  GBK/U      564
                      F8A1-FEFE   E234-E4C5   7941-7C9F?  GBK/U      658

!    672   UDC-3      A140-A792   E4C6-E791   7CA0 7FFD?  GBK/U      658
                      A793-A7A0   E792-E79F   8041-804E?  GBK/U       14

#    160   NoUse      A2AB-A9FE:  E586-E80F:  804F-80EA?  GBK/1,5    155
                      D7FA-D7FE   E810-E814   80EB-80EF?  GBK/2        5

*   7445   OldGB      A1A1-A9FE:  0000-33FF:  4040-46EF:  GBK/1      682
                                  FE00-FFFF:

                      B0A1-D7F9   4E00-9FA5:  48A0-5C9A   GBK/2     3755

                      D8A1-F7FE   4E00-9FA5:  5CA0-6C9F   GBK/2     3008
---------------------------------------------------------------------------
   23940              :  discontinuity code points area            23940
                      ?  temporary code points assigned by C.C.
                      ?? 28 map to Host code among 31 IBM select symbols
                         Host code: 427D,435B,445C no corresponded GBK code
*/

/*
GBK汉字一个区有96个汉字,编码从起始字符0x40~0x7F=0x3F(63), 0x81~0xA0=0x21(31)
*/

static unsigned char cnGbk34TableX[]=
{
0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
0xA0,	/* GBK/3 Total 6080 characters */
0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 
0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 
0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 
0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 
0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, 
0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE,
0x00 
};

static unsigned char ibmGbk34TableX[]=
{
0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
0xA0,	/* GBK/3 Total 6080 characters */
0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 
0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 
0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 
0x00 
};

static unsigned char cnGbk4TableY[]= /* 96 */
{
0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 
0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 
0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 
0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 
0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 
0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 
0xA0,
0x00
};

static unsigned char cnGbkTableY[]= /* 190 */
{
0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 
0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 
0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 
0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 
0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 
0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 
0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 
0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 
0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 
0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 
0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, 
0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE,
0x00
};

/*
IBM字符编码一个区有188个字符,编码从起始字符0x41~0x7E=0x3E(62), 
0x80~0xFD=0x7E(126)
*/
static unsigned char ibmGbkTableY[]= /* 188 */
{
0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 
0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 
0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 
0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 
0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 
0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 
0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 
0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 
0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 
0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, 
0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 
0x00 
};

int cn2ibmGBK4ext(unsigned char *s, unsigned char *t)
{
	/* GB chinese characters Not in GBK/4 extension range */
	if (!(memcmp(s,"\xfd\x9c",2)>=0&&memcmp(s,"\xfe\xa0",2)<=0))
		return (-2);
	if( memcmp(s,"\xfd\x9c",2)>=0 && memcmp(s,"\xfd\xa0",2)<=0 )
	{	/* 5 chars */
		t[0] = s[0] - 0x30; t[1] = s[1] + 0x53;
	}
	else
	if( memcmp(s,"\xfe\x40",2)>=0 && memcmp(s,"\xfe\x49",2)<=0 )
	{	/* 10 chars */
		t[0] = s[0] - 0x31; t[1] = s[1] + 0xB4;
	}
	else
	if( (memcmp(s,"\xfe\x4a",2)>=0 && memcmp(s,"\xfe\x80",2)<0 )||
		(memcmp(s,"\xfe\x8a",2)>=0 && memcmp(s,"\xfe\xa0",2)<=0) )
	{	/* 76 chars */
		t[0] = s[0] - 0x30; t[1] = s[1] - 0x09;
	}
	else
	if( memcmp(s,"\xfe\x80",2)>=0 && memcmp(s,"\xfe\x89",2)<=0 )
	{	/* 10 chars */
		t[0] = s[0] - 0x30; t[1] = s[1] - 0x0A;
	}
	else
	{
		return (-1);
	}
	return 0;
}

//设计以下2个函数
//从中文字符内码表区GBK/3/3转换至IBM中文字符内码表区:
int cn2ibmGBK34 (unsigned char *s, unsigned char *t)
{
	register int offset = 0, i, j;
	/* GB chinese characters Not in GBK/4 range */
	if (!((memcmp(s,"\x81\x40",2)>=0&&memcmp(s,"\xa0\xfe",2)<=0)||
		  (memcmp(s,"\xaa\x40",2)>=0&&memcmp(s,"\xfd\x9b",2)<=0)))
		return (-2);
	/*
	   CharNo  Name       GBKRange    UCSRange    HostRange   Zone     CharNo
	-  ------  ---------  ---------   ---------   ---------   -------  ------
	%  14139   NewGBK     8140-A0FE   4E00-72DB:  8141-CC67?  GBK/3     6080
	                      AA40-FD9B   72DC-9FA5:              GBK/4     8059
	*/
	for(i=0;cnGbk34TableX[i];i++)
	{
		if(s[0]==cnGbk34TableX[i])
		{
			break;
		}
	}
	//
	// If not found in X table
	if(!cnGbk34TableX[i])	return (-1);
	//
	// GBK/3
	if(s[0]<0xa1)
	{
		for(j=0;cnGbkTableY[j];j++)
		{
			if(s[1]==cnGbkTableY[j])
			{
				break;
			}
		}
		//
		// If not found in Y table
		if(!cnGbkTableY[j])	return (-1);
	}
	//
	// GBK/4
	else
	{
		for(j=0;cnGbk4TableY[j];j++)
		{
			if(s[1]==cnGbk4TableY[j])
			{
				break;
			}
		}
		//
		// If not found in Y table
		if(!cnGbk4TableY[j])	return (-1);
	}
	//
	// GBK/3
	if(s[0]<0xa1)
	{
		//
		// Calculate offset
		offset = i * 190 + j; // 190
	}
	else
	{
		//
		// Calculate offset
		offset = 32*190 + (i - 32) * 96 + j; // 190
	}
	//
	// Compute i in IBM X table
	i = offset / 188;
	//
	// Compute j in IBM Y table
	j = offset % 188;

	t[0] = ibmGbk34TableX[i];
	t[1] = ibmGbkTableY[j];

	return 0;
}

int ibm2cnGBK4ext(unsigned char *s, unsigned char *t)
{
	/* IBM chinese characters Not in GBK/4 extension range */
	if (!(memcmp(s,"\xcd\xef",2)>=0&&memcmp(s,"\xce\x97",2)<=0))
		return (-2);
	if( memcmp(s,"\xcd\xef",2)>=0 && memcmp(s,"\xcd\xf3",2)<=0 )
	{	/* 5 chars */
		t[0] = s[0] + 0x30; t[1] = s[1] - 0x53;
	}
	else
	if( memcmp(s,"\xcd\xf4",2)>=0 && memcmp(s,"\xcd\xfd",2)<=0 )
	{	/* 10 chars */
		t[0] = s[0] + 0x31; t[1] = s[1] - 0xB4;
	}
	else
	if( (memcmp(s,"\xce\x41",2)>=0 && memcmp(s,"\xce\x76",2)<0 )||
		(memcmp(s,"\xce\x81",2)>=0 && memcmp(s,"\xce\x97",2)<=0 ))
	{	/* 76 chars */
		t[0] = s[0] + 0x30; t[1] = s[1] + 0x09;
	}
	else
	if( memcmp(s,"\xce\x76",2)>=0 && memcmp(s,"\xce\x7f",2)<=0 )
	{	/* 10 chars */
		t[0] = s[0] + 0x30; t[1] = s[1] + 0x0a;
	}
	else
	{
		return (-1);
	}
	return 0;
}

int ibm2cnGBK34 (unsigned char *s, unsigned char *t)
{
	register int offset = 0, i, j;
	/* IBM chinese characters Not in GBK/4 range */
	if (!(memcmp(s,"\x81\x41",2)>=0&&memcmp(s,"\xcc\x67",2)<=0))
		return (-2);
	/*
	   CharNo  Name       GBKRange    UCSRange    HostRange   Zone     CharNo
	-  ------  ---------  ---------   ---------   ---------   -------  ------
	%  14139   NewGBK     8140-A0FE   4E00-72DB:  8141-CC67?  GBK/3     6080
	                      AA40-FD9B   72DC-9FA5:              GBK/4     8059
	*/
	for(i=0;ibmGbk34TableX[i];i++)
	{
		if(s[0]==ibmGbk34TableX[i])
		{
			break;
		}
	}
	//
	// If not found in X table
	if(!ibmGbk34TableX[i])	return (-1);

	for(j=0;ibmGbkTableY[j];j++)
	{
		if(s[1]==ibmGbkTableY[j])
		{
			break;
		}
	}
	//
	// If not found in Y table
	if(!ibmGbkTableY[j])	return (-1);
	//
	// Calculate offset
	offset = i * 188 + j;
	//printf("IBM offset: %d\n", offset);
	// GBK/3
	if(memcmp(s,"\xa1\x82",2)<0)
	{
		//
		// Compute i in CN X table
		i = offset / 190;
		//
		// Compute j in CN Y table
		j = offset % 190;

		t[1] = cnGbkTableY[j];
	}
	// GBK/4
	else
	{
		//
		// Compute i in CN X table
		i = (offset - 32*190) / 96 + 32;
		//
		// Compute j in CN Y table
		j = (offset - 32*190) % 96;

		t[1] = cnGbk4TableY[j];
	}

	t[0] = cnGbk34TableX[i];

	return 0;
}

/* GBK/U  Range: CN: A140 - A7A0, IBM: 7CA0 - 804E = */
/* GBK/U  Range: CN: AAA1 - AFFE, IBM: 7641 - 78FD = */
/* GBK/U  Range: CN: F8A1 - FEFE, IBM: 7941 - 7C9F = */

static unsigned char cnGbkuTableX[]=
{
0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 
0x00 
};

static unsigned char cnGbku1TableX[]=
{
0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE,
0x00 
};


static unsigned char ibmGbkuTableX[]=
{
0x7C, 0x7D, 0x7E, 0x7F, 0x80, 
0x76, 0x77, 0x78, 
0x79, 0x7A, 0x7B, 0x7C, 
0x00 
};

static unsigned char ibmGbku1TableX[]=
{
0x76, 0x77, 0x78, 
0x79, 0x7A, 0x7B, 0x7C, 
0x00 
};

/* 190 *//*
static unsigned char cnGbkuTableY[]= 
{
0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 
0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 
0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 
0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 
0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 
0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 
0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 
0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 
0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 
0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 
0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, 
0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE,
0x00
};
*/

static unsigned char cnGbku1TableY[]= /* 94 */
{
0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 
0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 
0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 
0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 
0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, 
0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE,
0x00
};

/*
IBM字符编码一个区有188个字符,编码从起始字符0x41~0x7E=0x3E(62), 
0x80~0xFD=0x7E(126)
*/
static unsigned char ibmGbkuTableY_7C[]= /* 188 */
{
0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 
0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 
0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 
0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 
0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, 
0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 
0x00 
};

/* GBK/U  Range: CN: A140 - A7A0, IBM: 7CA0 - 804E = */
/* GBK/U  Range: CN: AAA1 - AFFE, IBM: 7641 - 78FD = */
/* GBK/U  Range: CN: F8A1 - FEFE, IBM: 7941 - 7C9F = */
//从中文字符内码表区GBK/U转换至IBM中文字符内码表区:
int cn2ibmGBKU (unsigned char *s, unsigned char *t)
{
	register int offset = 0, i, j;
	/* GB chinese characters Not in GBK/4 range */
	if (!((memcmp(s,"\xa1\x40",2)>=0&&memcmp(s,"\xa7\x92",2)<=0)||
		  (memcmp(s,"\xa7\x93",2)>=0&&memcmp(s,"\xa7\xa0",2)<=0)||
		  (memcmp(s,"\xaa\xa1",2)>=0&&memcmp(s,"\xaf\xfe",2)<=0)||
		  (memcmp(s,"\xf8\xa1",2)>=0&&memcmp(s,"\xfe\xfe",2)<=0)))
		return (-2);
	/*
	   CharNo  Name       GBKRange    UCSRange    HostRange   Zone     CharNo
	-  ------  ---------  ---------   ---------   ---------   -------  ------
@   1222   UDC-1&2    AAA1-AFFE   E000-E233   7641-78FD?  GBK/U      564
                      F8A1-FEFE   E234-E4C5   7941-7C9F?  GBK/U      658

!    672   UDC-3      A140-A792   E4C6-E791   7CA0 7FFD?  GBK/U      658
                      A793-A7A0   E792-E79F   8041-804E?  GBK/U       14
	*/
	if ((memcmp(s,"\xa1\x40",2)>=0&&memcmp(s,"\xa7\x92",2)<=0)||
		(memcmp(s,"\xa7\x93",2)>=0&&memcmp(s,"\xa7\xa0",2)<=0))
	{

		for(i=0;cnGbkuTableX[i];i++)
		{
			if(s[0]==cnGbkuTableX[i])
			{
				break;
			}
		}
		//
		// If not found in X table
		if(!cnGbkuTableX[i])	return (-1);
		//
		// Use GBK/4 table
		for(j=0;cnGbk4TableY[j];j++)
		{
			if(s[1]==cnGbk4TableY[j])
			{
				break;
			}
		}
		//
		// If not found in Y table
		if(!cnGbk4TableY[j])	return (-1);
		//
		// CN: A19E, IBM: 7CFD
		if(memcmp(s,"\xa1\x9e",2)>0)
		{
			//
			// Calculate offset
			offset = i * 96 + j; // 190
			//
			// Compute i in IBM X table
			i = (offset - 94 /* GBK: A140 - A19E Host: 7CA0 - 7CFD */) / 188 + 1;
			//
			// Compute j in IBM Y table
			j = (offset - 94) % 188;
			t[1] = ibmGbkTableY[j];
		}
		else
		{
			//
			// Calculate offset
			//offset = j; // 190
			t[1] = ibmGbkuTableY_7C[j];
		}

		t[0] = ibmGbkuTableX[i];
	}
	else
	if (  (memcmp(s,"\xaa\xa1",2)>=0&&memcmp(s,"\xaf\xfe",2)<=0)||
		  (memcmp(s,"\xf8\xa1",2)>=0&&memcmp(s,"\xfe\xfe",2)<=0 ))
	{

		for(i=0;cnGbku1TableX[i];i++)
		{
			if(s[0]==cnGbku1TableX[i])
			{
				break;
			}
		}
		//
		// If not found in X table
		if(!cnGbku1TableX[i])	return (-1);
		//
		// Use GBK table
		for(j=0;cnGbku1TableY[j];j++)
		{
			if(s[1]==cnGbku1TableY[j])
			{
				break;
			}
		}
		//
		// If not found in Y table
		if(!cnGbku1TableY[j])	return (-1);
		//
		// Calculate offset
		offset = i * 94 + j; // 94
		//
		// Compute i in IBM X table
		i = offset / 188;
		//
		// Compute j in IBM Y table
		j = offset % 188;

		t[0] = ibmGbku1TableX[i];
		t[1] = ibmGbkTableY[j];
	}
	else
	{
		return (-1);
	}

	return 0;
}

int ibm2cnGBKU (unsigned char *s, unsigned char *t)
{
	register int offset = 0, i, j;
	/* IBM chinese characters Not in GBK/U range */
	if (!((memcmp(s,"\x76\x41",2)>=0&&memcmp(s,"\x78\xfd",2)<=0)||
		  (memcmp(s,"\x79\x41",2)>=0&&memcmp(s,"\x7c\x9f",2)<=0)||
		  (memcmp(s,"\x7c\xa0",2)>=0&&memcmp(s,"\x7f\xfd",2)<=0)||
		  (memcmp(s,"\x80\x41",2)>=0&&memcmp(s,"\x80\x4e",2)<=0)))
		return (-2);
	/*
	   CharNo  Name       GBKRange    UCSRange    HostRange   Zone     CharNo
	-  ------  ---------  ---------   ---------   ---------   -------  ------
	@   1222   UDC-1&2    AAA1-AFFE   E000-E233   7641-78FD?  GBK/U      564
	                      F8A1-FEFE   E234-E4C5   7941-7C9F?  GBK/U      658

	!    672   UDC-3      A140-A792   E4C6-E791   7CA0 7FFD?  GBK/U      658
	                      A793-A7A0   E792-E79F   8041-804E?  GBK/U       14
	*/

	if (  (memcmp(s,"\x7c\xa0",2)>=0&&memcmp(s,"\x7f\xfd",2)<=0)||
		  (memcmp(s,"\x80\x41",2)>=0&&memcmp(s,"\x80\x4e",2)<=0 ))
	{

		for(i=0;ibmGbkuTableX[i];i++)
		{
			if(s[0]==ibmGbkuTableX[i])
			{
				break;
			}
		}
		//
		// If not found in X table
		if(!ibmGbkuTableX[i])	return (-1);
		//
		// CN: A19E, IBM: 7CFD
		if(memcmp(s,"\x7c\xfd",2)>0)
		{

			for(j=0;ibmGbkTableY[j];j++)
			{
				if(s[1]==ibmGbkTableY[j])
				{
					break;
				}
			}

			//
			// If not found in Y table
			if(!ibmGbkTableY[j])	return (-1);
			//
			// Calculate offset
			offset = (i - 1) * 188 + j + 94;
		}
		else
		{/* GBK: A140 - A19E Host: 7CA0 - 7CFD */
			for(j=0;ibmGbkuTableY_7C[j];j++)
			{
				if(s[1]==ibmGbkuTableY_7C[j])
				{
					break;
				}
			}
			//
			// If not found in Y table
			if(!ibmGbkuTableY_7C[j])	return (-1);

			offset = j;
		}
		//printf("IBM offset: %d\n", offset);
		//
		// Compute i in CN X table
		i = offset / 96;
		//
		// Compute j in CN Y table
		j = offset % 96;

		t[0] = cnGbkuTableX[i];
		t[1] = cnGbk4TableY[j];
	}
	else
	if ((memcmp(s,"\x76\x41",2)>=0&&memcmp(s,"\x78\xfd",2)<=0)||
		(memcmp(s,"\x79\x41",2)>=0&&memcmp(s,"\x7c\x9f",2)<=0 ))
	{
		for(i=0;ibmGbku1TableX[i];i++)
		{
			if(s[0]==ibmGbku1TableX[i])
			{
				break;
			}
		}
		//
		// If not found in X table
		if(!ibmGbku1TableX[i])	return (-1);
		//
		// CN: A19E, IBM: 7CFD
		for(j=0;ibmGbkTableY[j];j++)
		{
			if(s[1]==ibmGbkTableY[j])
			{
				break;
			}
		}

		//
		// If not found in Y table
		if(!ibmGbkTableY[j])	return (-1);
		//
		// Calculate offset
		offset = i * 188 + j;
		//printf("IBM offset: %d\n", offset);
		//
		// Compute i in CN X table
		i = offset / 94;
		//
		// Compute j in CN Y table
		j = offset % 94;

		t[0] = cnGbku1TableX[i];
		t[1] = cnGbku1TableY[j];
	}
	else
	{
		return (-1);
	}

	return 0;
}



/*********************************************************************
 *     Filename : TranslateCodeUp.c
 *-------------------------------------------------------------------*
 *  Description : Translate a string with host encode into local
 *		  encode :
 *		  alphabetical and numberic will be exchanged from
 *		  EBCDIC to ASCII, and Chinese will be translated
 *		  from IBM1386, IBM1388 to GBK Chinese.
 *-------------------------------------------------------------------*
 *    Create by : Robert Shen
 *-------------------------------------------------------------------*
 *   Corpration : BrightWay
 *-------------------------------------------------------------------*
 *  Create date : 1998.5.6
 *-------------------------------------------------------------------*
 *  Last modify : 12/23/2002
 *********************************************************************/




/***** zhangxi, add, 1998/09/26 *****/
int				Reserve0E0FByte = 0;
unsigned char	Reserve0EChar = ' ', Reserve0FChar = ' ';

/*********************************************************************
 *     Function : Set0E0FReserveChar
 *-------------------------------------------------------------------*
 *  Description : Normally, 0E/0F that denote begin and end of chinese
 *                in IBM5250 code will be deleted when text is 
 *                translated to GB code. It bring on the length of
 *                target string is different from the source. It is
 *		  not acceptable when transaction message is unpacket.
 *		  So 0E and 0F must be replaced by another character
 *		  to guarantee the consistent of length.
 *-------------------------------------------------------------------*
 *  Last modify : 1998.9.26
 *-------------------------------------------------------------------*
 *        Input : 1. unsigned char Reserve0EChar
 *                   The character will be used to replace 0E
 *		  2. unsigned char Reserve0FChar
 *		     The CHaracter will be used to replace 0F
 *-------------------------------------------------------------------*
 *       Output : none
 *-------------------------------------------------------------------*
 * Return Value : none
 *********************************************************************/

void Set0E0FReserveChar (unsigned char Reserve0ECh, unsigned char Reserve0FCh)
{
	Reserve0EChar = Reserve0ECh;
	Reserve0FChar = Reserve0FCh;
}

/***** end of add *****/

/*********************************************************************
 *     Function : TransDown
 *-------------------------------------------------------------------*
 *  Description : Translate string known by host to RS/6000 format
 *-------------------------------------------------------------------*
 *  Last modify : 1998.5.6
 *-------------------------------------------------------------------*
 *        Input : 1. unsigned char *SourceStr
 *		     Source string with host encode
 *-------------------------------------------------------------------*
 *       Output : 1. unsigned char *TargetStr
 *		     Target string, chars be ASCII encode and chinese
 *		     is follow GB protocol.
 *-------------------------------------------------------------------*
 * Return Value : 1. 0 is ok
 *		  2. non-zero meaans error
 *********************************************************************/

int IBM5250Decode (unsigned char *SourcePtr, unsigned char *TargetPtr, int *DataLen)
{
	int	SourceLen=0, TargetLen=0;
	int	ObjLen;
	int rc;		/* Internal return value - Ro20020420 */
	int	CodeFlag = C_FALSE;

	unsigned char	*TempBuffer;

	/* Ro20020424 */
	TempBuffer = (unsigned char *) malloc (*DataLen + 1);
	memset (TempBuffer, 0x00, *DataLen + 1);

	ObjLen = *DataLen;
	while (SourceLen < ObjLen)
	{
		/* Chinese begin */
		if (SourcePtr[SourceLen] == CHINESE_BEGIN)
		{
			CodeFlag = C_TRUE;

			if (Reserve0E0FByte)
			{
				TempBuffer[TargetLen ++] = Reserve0EChar;
			}
			SourceLen ++;
		}
		/* Chinese end */
		else if (SourcePtr[SourceLen] == CHINESE_END)
		{
			CodeFlag = C_FALSE;

			if (Reserve0E0FByte)
			{
				TempBuffer[TargetLen ++] = Reserve0FChar;
			}
			SourceLen ++;
		}
		/* In Chinese string, every char 2 bytes */
		else if ( CodeFlag )		/* common area */
		{
			rc = etogb (SourcePtr+SourceLen, TempBuffer+TargetLen);
			if(rc<0)
			{
				free (TempBuffer);	/* 20020505Ro */
				return rc;	/* Ro20020420 */
			}
			SourceLen += 2;
			TargetLen += 2;
		}
		/* Not Chinese char */
		else
		{
			UnionEbcdicToAscii (SourcePtr + SourceLen, TempBuffer + TargetLen, 1);
			SourceLen ++;
			TargetLen ++;
		}

	}

	/* Fixed incorrect target length - 20020419Ro */
	*DataLen = TargetLen;
  
	memcpy (TargetPtr, TempBuffer, *DataLen);
	free (TempBuffer);

	return (0);
}



/*********************************************************************
 *     Filename : TranslateCodeUp.c
 *-------------------------------------------------------------------*
 *  Description : Translate a string with host encode into local
 *		  encode :
 *		  alphabetical and numberic will be exchanged from
 *		  ASCII to EBCDIC which are known by host, and Chinese
 *		  will be translated from GBK to IBM1386, IBM1388.
 *-------------------------------------------------------------------*
 *    Create by : Robert Shen
 *-------------------------------------------------------------------*
 *   Corpration : BrightWay
 *-------------------------------------------------------------------*
 *  Create date : 1998.5.6
 *-------------------------------------------------------------------*
 *  Last modify : 12/23/2002
 *********************************************************************/






/*********************************************************************
 *     Function : TransDown
 *-------------------------------------------------------------------
 *  Description : Translate string known by host to RS/6000 format
 *-------------------------------------------------------------------
 *  Last modify : 12/23/2002
 *-------------------------------------------------------------------
 *        Input : 1. unsigned char *SourcePtr
 *		     Source string, chars be ASCII encode and chinese
 *		     is follow GB protocol.
 *-------------------------------------------------------------------
 *       Output : 1. unsigned char *TargetPtr
 *		     Target string with host encode
 *-------------------------------------------------------------------
 * Return Value : 1. 0 is ok
 *		  2. non-zero meaans error
 *********************************************************************/

int IBM5250Encode (unsigned char *SourcePtr, unsigned char *TargetPtr, int *DataLen)
{
	int	SourceLen = 0, TargetLen = 0;
	int	ObjLen;
	int rc;	/* Internal return value - Ro20020420 */
	int	CodeFlag = C_FALSE;			/* Assume not chinese */

	unsigned char *TempBuffer;
	/* Chinese char : 0xA1FE ~ 0xA1FE */
	/* 0x0E HH 0x0F A 0x0E HH 0x0F A 0x0E HH 0x0F A - Ro20020424 */
	TempBuffer = (unsigned char *) malloc (*DataLen + ((*DataLen + 1) /3) * 2 + 1);
	memset (TempBuffer, 0x00, *DataLen + ((*DataLen + 1) /3) * 2 + 1);

	ObjLen = *DataLen;
	while (SourceLen < ObjLen)
	{
		/* Case 1: Now is  the First Chinese char - Revised 24/12/2002Ro */
		if ( !CodeFlag && (SourcePtr[SourceLen] & 0x80) && (SourcePtr[SourceLen+1] >= 0x40))
		{
			/* Insert Chinese Begin char */
			CodeFlag = C_TRUE;
			TempBuffer[TargetLen ++] = CHINESE_BEGIN;
	    
			rc = gbtoe (SourcePtr+SourceLen, TempBuffer+TargetLen);
			if(rc<0)
			{
				free (TempBuffer);	/* 20020505Ro */
				return rc;	 /* Ro20020420 */
			}

			SourceLen += 2;
			TargetLen += 2;
		}
		/* Case 2: Now it's in Chinese string - Revised 24/12/2002Ro */
		else if ( CodeFlag && (SourcePtr[SourceLen] & 0x80) && (SourcePtr[SourceLen+1] >= 0x40))
		{
			CodeFlag = C_TRUE;
			rc = gbtoe (SourcePtr+SourceLen, TempBuffer+TargetLen);
			if(rc<0)
			{
				free (TempBuffer);	/* 20020505Ro */
				return rc;	 /* Ro20020420 */
			}

			SourceLen += 2;
			TargetLen += 2;
		}
		/* Case 3: If previous char is chinese, but current char is not Chinese - Chinese END */
		else if ( CodeFlag && !(SourcePtr[SourceLen] & 0x80) )
		{
			/* Insert Chese End Char Here */
			CodeFlag = C_FALSE;
			TempBuffer[TargetLen ++] = CHINESE_END;
			/* Ro20020424 */
			/* Bug here !!! */

			UnionAsciiToEbcdic (SourcePtr + SourceLen, TempBuffer + TargetLen, 1);
			SourceLen ++;
			TargetLen ++;
		}
		/* If not Chinese, Fixed Bug here !!! - Ro20020424 */
		else	
		{
			CodeFlag = C_FALSE;
			UnionAsciiToEbcdic (SourcePtr + SourceLen, TempBuffer + TargetLen, 1);
			SourceLen ++;
			TargetLen ++;
		}
	}

	/* If previous char is Chinese, insert end here ! */
	if ( CodeFlag )
	{
		TempBuffer[TargetLen ++] = CHINESE_END;
    }

	*DataLen = TargetLen;

	memcpy (TargetPtr, TempBuffer, *DataLen);
	free (TempBuffer);

	return (0);
}

// this function added by Wolfgang Wang
int ConvertAsciiToEbcdic(unsigned char *ascii,unsigned char *ebcdic,int len)
{
	int	i;
	unsigned char	tmpBuf[2048];
	
	if (len > sizeof(tmpBuf))
		return(-1);
		
	memcpy(tmpBuf,ascii,len);
	i = 0;
	while (i < len)
	{
		if (ascii[i] > 128)
		{
			//printf("??%x %x::\n",ascii[i],ascii[i+1]);
			gbtoe(tmpBuf+i,ebcdic+i);
			//printf("!!%x %x::\n",tmpBuf[i],tmpBuf[i+1]);
			//printf("::%x %x::\n",ebcdic[i],ebcdic[i+1]);
			i += 2;
		}
		else
		{
			UnionAsciiToEbcdic(tmpBuf+i,ebcdic+i,1);
			i++;
		}
	}
	return(0);
}
