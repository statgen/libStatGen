TOOLBASE = bgzf knetfile 
HDRONLY = khash.h bam.h

USER_COMPILE_VARS = -D_USE_KNETFILE

SUBDIRS=pbgzip

.PHONY: $(SUBDIRS)


include ../Makefiles/Makefile.lib

opt debug profile: $(SUBDIRS)

$(SUBDIRS): 
	@$(MAKE) -C $@ $(MAKECMDGOALS)

clean:$(SUBDIRS)
