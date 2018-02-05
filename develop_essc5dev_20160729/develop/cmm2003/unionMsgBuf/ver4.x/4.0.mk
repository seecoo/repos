#	2003/09/19	Wolfgang Wang

#	libs
#		01	unionMsgBuf
#		02	mngMsgBuf
#		03	msgBufMon

#	2003/09/23
#	libs
#		unionMsgBuf4.0.a
#		mngMsgBuf4.0.a
#		msgBufMon4.0.a
HOMEDIR = $(CMM2003DIR)/unionMsgBuf
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC)

all:	unionMsgBuf4.0.a	\
	mngMsgBuf4.0.a		\
	msgBufMon4.0.a		\
	finalProduct		\
	rmTmpFiles


.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs0140 = unionMsgBuf4.0.o
unionMsgBuf4.0.a:	$(objs0140)	
		ar $(OSSPECPACK) rv unionMsgBuf4.0.a $(objs0140)

objs0240 = mngMsgBuf4.0.o
mngMsgBuf4.0.a:	$(objs0240)	
		ar $(OSSPECPACK) rv mngMsgBuf4.0.a $(objs0240)

objs0340 = msgBufMon4.0.o
msgBufMon4.0.a:	$(objs0340)	
		ar $(OSSPECPACK) rv msgBufMon4.0.a $(objs0340)

finalProduct:	$(null)
	# includes
		cp $(INCDIR)unionMsgBuf.h		$(CMM2003INCDIR)
	# libs
		mv unionMsgBuf4.0.a			$(CMM2003LIBDIR)
		mv mngMsgBuf4.0.a			$(CMM2003LIBDIR)
		mv msgBufMon4.0.a			$(CMM2003LIBDIR)

rmTmpFiles:	$(null)
		rm *.o
