#	2003/09/19	Wolfgang Wang

#	libs
#		01	unionMsgBuf
#		02	mngMsgBuf
#		03	msgBufMon

#	2003/09/23
#	libs
#		unionMsgBuf5.0.a
#		mngMsgBuf5.0.a
#		msgBufMon5.0.a

HOMEDIR = $(CMM2003DIR)/unionMsgBuf
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC)

all:	unionMsgBuf5.0.a	\
	mngMsgBuf5.0.a		\
	msgBufMon5.0.a		\
	finalProduct		\
	rmTmpFiles


.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs0150 = unionMsgBuf5.0.o
unionMsgBuf5.0.a:	$(objs0150)	
		ar $(OSSPECPACK) rv unionMsgBuf5.0.a $(objs0150)

objs0250 = mngMsgBuf5.0.o
mngMsgBuf5.0.a:	$(objs0250)	
		ar $(OSSPECPACK) rv mngMsgBuf5.0.a $(objs0250)

objs0350 = msgBufMon5.0.o
msgBufMon5.0.a:	$(objs0350)	
		ar $(OSSPECPACK) rv msgBufMon5.0.a $(objs0350)

finalProduct:	$(null)
	# includes
		cp $(INCDIR)unionMsgBuf.h		$(CMM2003INCDIR)
	# libs
		mv unionMsgBuf5.0.a			$(CMM2003LIBDIR)
		mv mngMsgBuf5.0.a			$(CMM2003LIBDIR)
		mv msgBufMon5.0.a			$(CMM2003LIBDIR)

rmTmpFiles:	$(null)
		rm *.o
