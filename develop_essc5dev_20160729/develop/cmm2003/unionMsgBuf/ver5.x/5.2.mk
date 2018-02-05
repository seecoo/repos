#	2003/09/19	Wolfgang Wang

#	libs
#		01	unionMsgBuf
#		02	mngMsgBuf
#		03	msgBufMon

#	2003/09/23
#	libs
#		unionMsgBuf5.2.a

HOMEDIR = $(CMM2003DIR)/unionMsgBuf
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC)

all:	unionMsgBuf5.2.a	\
	finalProduct		\
	rmTmpFiles


.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs0150 = unionMsgBuf5.2.o
unionMsgBuf5.2.a:	$(objs0150)	
		ar $(OSSPECPACK) rv unionMsgBuf5.2.a $(objs0150)

finalProduct:	$(null)
	# includes
		cp $(INCDIR)unionMsgBuf.h		$(CMM2003INCDIR)
	# libs
		mv unionMsgBuf5.2.a			$(CMM2003LIBDIR)

rmTmpFiles:	$(null)
		rm *.o
