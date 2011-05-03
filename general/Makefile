UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
  # For Linux, add a couple extra warnings
  #
  # trying to strike the right balance on error/warning
  # handling is difficult, but here we make most everthing
  # an error, but allow unused results and variables for the
  # time being.
  #
  USER_WARNINGS ?= -Werror -Wno-unused-variable $(shell if [ X$(CCVERSION) \> X4.2.0 ] ; then echo " -Wno-unused-result" ; fi)
endif
ifeq ($(UNAME), Darwin)
  # Mac OS, nothing to add.
  # -Wall contain -Wunused-variable and -Wunused-result from 'man gcc'
endif

TOOLBASE=\
	BaseAsciiMap \
	BaseQualityHelper \
	BaseUtilities \
	BasicHash \
	BgzfFileType \
	CharBuffer \
	Chromosome \
	Cigar \
	CigarRoller \
	Error \
	FastQ \
	FileType \
	FortranFormat \
	GenomeSequence \
	GenotypeLists \
	glfHandler \
	GzipFileType \
	GzipHeader \
	Hash \
	Input \
	InputFile \
	IntArray \
	IntHash \
	LongHash \
	LongLongCounter \
	MapFunction \
	MathMatrix \
	MathVector \
	MemoryAllocators \
	MemoryInfo \
	MemoryMapArray \
	MemoryMap \
	MiniDeflate \
	Parameters \
	PedigreeAlleleFreq \
	Pedigree \
	PedigreeDescription \
	PedigreeFamily \
	PedigreeGlobals \
	PedigreePerson \
	QuickIndex \
	Random \
	SmithWaterman \
	Sort \
	STLUtilities \
	StringAlias \
	StringArray \
	StringBasics \
	StringHash \
	StringMap \
	TrimSequence \
	UncompressedFileType \
	WindowsHelper

SRCONLY=\
	PedigreeLoader.cpp \
	PedigreeTrim.cpp \
	PedigreeTwin.cpp 

HDRONLY= \
	Constant.h \
	Generic.h \
	GreedyTupleAligner.h \
	InplaceMerge.h \
	LongInt.h \
	MathConstant.h \
	PedigreeAlleles.h \
	SimpleStats.h

include ../Makefiles/Makefile.lib


CPP_TESTS := $(shell grep -m 1 'if defined(TEST)' *cpp |cut -f1 -d: )
selftest: 
	@for i in $(CPP_TESTS); do \
		if [ "XXX$$i" = XXX ] ;\
		then \
		    continue; \
		fi;\
		(echo "building in self test $$i"; g++ -DTEST -D__STDC_LIMIT_MACROS -o test_$$i $$i  -L. -lcsg -lm -lz -lssl ; ./test_$$i) ; \
		if [ $$? -ne 0 ] ; \
		then \
		    echo "make stopped because of errors." ; \
		    break ; \
		fi \
	done
