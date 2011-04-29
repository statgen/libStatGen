PATH_TO_BASE=.
include $(PATH_TO_BASE)/Makefiles/Makefile.include

SUBDIRS=general bam fastq glf
CLEAN_SUBDIRS= $(patsubst %, %_clean, $(SUBDIRS))

# Build in all subdirectories.
#
# see http://www.gnu.org/software/make/manual/make.html#Phony-Targets
# for a way of improving the following:
#

# Can't build lib in parallel since multiple subdirs write to the library archive
.NOTPARALLEL:

.PHONY: $(SUBDIRS) all test clean $(CLEAN_SUBDIRS) samtools_install
all: TARGET = all
test: TARGET = test
clean: TARGET = clean
install: TARGET = install

all test: $(SUBDIRS)

install: all samtools_install

samtools_install: samtools $(INSTALLDIR)
	@echo " "
	@echo Installing to directory $(INSTALLDIR)
	@echo To select a different directory, run
	@echo " "
	@echo make install INSTALLDIR=your_preferred_dir
	@echo " "
	cp samtools/samtools-hybrid $(INSTALLDIR)

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

clean: samtools_clean $(CLEAN_SUBDIRS)
	rm -f libStatGen.a

# other subdirectories depend on general
bam fastq glf: general

$(CLEAN_SUBDIRS):  
	@$(MAKE) OPTFLAG="$(OPTFLAG)" -C $(patsubst %_clean,%,$@) $(TARGET)

$(SUBDIRS): samtools
	@$(MAKE) OPTFLAG="$(OPTFLAG)" -C $@ $(TARGET)


samtools: samtools-0.1.7a-hybrid
	ln -s samtools-0.1.7a-hybrid samtools

samtools-0.1.7a-hybrid: samtools-0.1.7a-hybrid.tar.bz2
	tar xvf samtools-0.1.7a-hybrid.tar.bz2
	@$(MAKE) OPTFLAG="$(OPTFLAG)" -C $@

samtools_clean:
	rm -rf samtools-0.1.7a-hybrid samtools
