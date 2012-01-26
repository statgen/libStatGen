SUBDIRS=general bam fastq glf samtools asp

include Makefiles/Makefile.base


clean:$(SUBDIRS)
	rm -f $(STAT_GEN_LIB_OPT)
	rm -f $(STAT_GEN_LIB_DEBUG)
	rm -f $(STAT_GEN_LIB_PROFILE)

# general depends on samtools
general: samtools

# other subdirectories depend on general
bam fastq glf: general
