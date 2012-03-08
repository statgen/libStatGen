TOOLBASE = block consumer pbgzf queue reader util writer
SRCONLY = 
HDRONLY = 

USER_COMPILE_VARS = -D_USE_KNETFILE

include ../../Makefiles/Makefile.lib

$(INCLUDE_PATH)/%.h: %.h
	ln -s ../samtools/$(DIR_NAME)/$< $@
