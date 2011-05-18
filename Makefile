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
opt: TARGET = opt
debug: TARGET = debug
profile: TARGET = profile
test: TARGET = test
clean: TARGET = clean
install: TARGET = install

all test opt debug profile: $(SUBDIRS)

install: all

$(INSTALLDIR) :
	@echo " "
	@echo Creating directory $(INSTALLDIR)
	@echo " "
	@mkdir -p $(INSTALLDIR)

help : 
	@echo "Makefile help"
	@echo "-------------"
	@echo "Type...           To..."
	@echo "make              Compile everything "
	@echo "make help         Display this help screen"
	@echo "make all          Compile everything (opt, debug, & profile)"
	@echo "make opt          Compile optimized"
	@echo "make debug        Compile for debug"
	@echo "make profile      Compile for profile"
	@echo "make clean        Delete temporary files"
	@echo "make test         Execute tests (if there are any)"

clean: $(SUBDIRS)
	rm -f $(STAT_GEN_LIB)
	rm -f $(STAT_GEN_LIB_DEBUG)
	rm -f $(STAT_GEN_LIB_PROFILE)

# general depends on samtools
general: samtools

# other subdirectories depend on general
bam fastq glf: general

$(SUBDIRS): 
	@$(MAKE) OPTFLAG="$(OPTFLAG)" -C $@ $(TARGET)
