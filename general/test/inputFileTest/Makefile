EXE = inputFileTest
TOOLBASE = InputFileTest

TEST_COMMAND=	mkdir -p results && \
	./inputFileTest > results/results.log && \
	diff data/InputFileTest.bam results/bgzfFile.glf && \
	diff data/InputFileTest.txt results/defaultFile.glf && \
	diff data/InputFileTest.gz results/defaultFile.gz && \
	diff data/InputFileTest.gz results/gzipFile.glf && \
	diff data/InputFileTest.txt results/InputFileTest.glf && \
	diff data/InputFileTest.txt results/uncompressedFile.glf && \
	diff data/textFile.gz results/textFile.gz && \
	diff data/textFile.gz results/textFile1.gz && \
	diff results/results.log expected/results.log

include ../../../Makefiles/Makefile.test