include Makefiles/Makefile.include

SUBDIRS=general bam fastq glf samtools

# Build in all subdirectories.
#
# see http://www.gnu.org/software/make/manual/make.html#Phony-Targets
# for a way of improving the following:
#

# Can't build lib in parallel since multiple subdirs write to the library archive
.NOTPARALLEL:

.PHONY: $(SUBDIRS) all test clean debug install 
all: TARGET = all
debug: TARGET = debug
test: TARGET = test
clean: TARGET = clean
install: TARGET = install

all test debug: $(SUBDIRS)

install: all

$(INSTALLDIR) :
	@echo " "
	@echo Creating directory $(INSTALLDIR)
	@echo " "
	@mkdir -p $(INSTALLDIR)

help : 
	@echo "Generic Source Distribution"
	@echo " "
	@echo "This Makefile will compile and install" $(TOOL) "on your system"
	@echo " "
	@echo "Type...           To..."
	@echo "make              Compile everything "
	@echo "make help         Display this help screen"
	@echo "make all          Compile everything "
	@echo "make install      Install binaries in $(INSTALLDIR)"
	@echo "make install INSTALLDIR=directory_for_binaries"
	@echo "                  Install binaries in directory_for_binaries"
	@echo "make clean        Delete temporary files"
	@echo "make test         Execute tests (if there are any)"

clean: $(SUBDIRS)
	rm -f libStatGen.a

# general depends on samtools
general: samtools

# other subdirectories depend on general
bam fastq glf: general

$(SUBDIRS): 
	@$(MAKE) OPTFLAG="$(OPTFLAG)" -C $@ $(TARGET)
