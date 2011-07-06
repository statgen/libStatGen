EXE = inputFileTest
TOOLBASE = InputFileTest

# Note: have to uncompress gz files to test since it contains the os.

TEST_COMMAND=	mkdir -p results && \
	./inputFileTest && \
	diff data/InputFileTest.bam results/bgzfFile.glf && \
	diff data/InputFileTest.txt results/defaultFile.glf && \
	gunzip -c results/defaultFile.gz | diff - data/InputFileTest.txt  && \
	gunzip -c results/gzipFile.glf | diff - data/InputFileTest.txt  && \
	diff data/InputFileTest.txt results/uncompressedFile.glf

include ../../../Makefiles/Makefile.test