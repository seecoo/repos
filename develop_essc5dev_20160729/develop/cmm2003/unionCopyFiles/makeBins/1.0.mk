INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX

all:	unionCopyFiles1.0	\
	finalProducts

genProductTime:	$(null)
		genProductGenerateTime
# bins
# 01 mngTask
libs0130 =	$(CMM2003LIBDIR)unionCopyFiles1.0.a		\
		$(CMM2003LIBDIR)unionEnv3.0.a			\
		$(CMM2003LIBDIR)UnionLog1.2.a			\
		$(CMM2003LIBDIR)UnionStr1.2.a
unionCopyFiles1.0:	$(null)
		cc $(OSSPECLINK) -o unionCopyFiles1.0 $(libs0130) $(OSSPECLIBS)


finalProducts:	$(null)
		mv	unionCopyFiles1.0	$(CMM2003TOOLSDIR)unionCopyFiles
