#	2003/09/19	Wolfgang Wang
#	Version		3.0

#	bins
#		01	mngMsgBuf
#		02	msgBufMon

#	2003/09/23
#	bins
#		mngMsgBuf2.0
#		msgBufMon5.0

BINLIBS = $(UNIONLIBDIR)/binLibs

HOMEDIR = $(HOME)/unionMsgBuf
BINDIR = $(HOMEDIR)/bin/
LIBDIR = $(HOMEDIR)/lib/
INCDIR = $(HOMEDIR)/include/
OBJDIR = $(HOMEDIR)/obj/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX

all:	genProductTime		\
	mngMsgBuf5.0		\
	msgBufMon5.0		\
	binProduct		\
	finalProduct

genProductTime:	$(null)
		genProductGenerateTime

libs0140 =	$(CMM2003LIBDIR)mngMsgBuf5.0.a			\
		$(CMM2003LIBDIR)unionMsgBuf5.0.a			\
		$(CMM2003LIBDIR)unionIndexTBL1.0.a			\
		$(CMM2003LIBDIR)unionTask3.2.a			\
		$(CMM2003LIBDIR)unionEnv3.0.a			\
		$(CMM2003LIBDIR)UnionLog3.0.a			\
		$(CMM2003LIBDIR)unionModule1.0.a			\
		$(CMM2003LIBDIR)unionEnv3.0.a			\
		$(CMM2003LIBDIR)unionInput1.0.a			\
		$(CMM2003LIBDIR)unionPreDefinedCmd1.0.a		\
		$(CMM2003LIBDIR)unionVersion1.0.a			\
		$(CMM2003LIBDIR)unionLibName3.0.a			\
		unionProductGenerateTime.a			\
		$(CMM2003VERLIB)unionPredefinedVersion5.0.a	\
		$(CMM2003LIBDIR)UnionStr1.2.a
mngMsgBuf5.0:	$(null)
		cc -o mngMsgBuf5.0 $(libs0140)

libs0240 =	$(CMM2003LIBDIR)msgBufMon5.0.a			\
		$(CMM2003LIBDIR)unionMsgBuf5.0.a			\
		$(CMM2003LIBDIR)unionIndexTBL1.0.a			\
		$(CMM2003LIBDIR)unionTask3.2.a			\
		$(CMM2003LIBDIR)unionEnv3.0.a			\
		$(CMM2003LIBDIR)UnionLog3.0.a			\
		$(CMM2003LIBDIR)unionModule1.0.a			\
		$(CMM2003LIBDIR)unionEnv3.0.a			\
		$(CMM2003LIBDIR)unionInput1.0.a			\
		$(CMM2003LIBDIR)unionPreDefinedCmd1.0.a		\
		$(CMM2003LIBDIR)unionVersion1.0.a			\
		$(CMM2003LIBDIR)unionMonitor1.0.a			\
		$(CMM2003LIBDIR)unionLibName3.0.a			\
		unionProductGenerateTime.a			\
		$(CMM2003VERLIB)unionPredefinedVersion5.0.a	\
		$(CMM2003LIBDIR)UnionStr1.2.a
msgBufMon5.0:	$(null)
		cc -o msgBufMon5.0 $(libs0240) -lcurses

binProduct:	$(null)
		cp mngMsgBuf5.0				$(BINLIBS)
		mv mngMsgBuf5.0				$(BINDIR)
		cp msgBufMon5.0				$(BINLIBS)
		mv msgBufMon5.0				$(BINDIR)

finalProduct:	$(null)
# bins
		cp $(BINDIR)mngMsgBuf5.0		$(CMM2003BINDIR)mngMsgBuf
		cp $(BINDIR)msgBufMon5.0		$(CMM2003BINDIR)msgBufMon
	# rmTmpFiles
		rm *.a

