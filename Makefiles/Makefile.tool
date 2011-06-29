
MAKEFILES_PATH := $(dir $(lastword $(MAKEFILE_LIST)))
include $(MAKEFILES_PATH)Makefile.base

RELEASE_FILE?=$(DIR_NAME).$(VERSION).tgz

ADDITIONAL_HELP= @echo "make install      Install binaries in $(INSTALLDIR)";\
	echo "make install INSTALLDIR=directory_for_binaries";\
	echo "                  Install binaries in directory_for_binaries"


.PHONY: package wholepackage

package : 
# the touch gets rid of a tar warning
#	echo directory name = $(DIR_NAME)
	touch $(RELEASE_FILE)
	tar cvz --exclude="*~" --exclude=$(RELEASE_FILE) --exclude='obj/*' --exclude='*.a'  --exclude='include/*' --exclude='bin/*' --exclude='test/results/*' --exclude-vcs --exclude-backups -f $(RELEASE_FILE) --transform 's,^,$(DIR_NAME)_$(VERSION)/,' * --show-transformed-names 

#WHOLEPACKAGE_MAKE = $(MAKEFILES_PATH)/Makefile.base
#BASE_LIB_PARTS := $(subst /, , $(BASE_LIB_PATH))
#BASE_LIB_DIRNAME := $(word $(words $(BASE_LIB_PARTS)), $(BASE_LIB_PARTS))

#wholepackage: 
## the touch gets rid of a tar warning
#	touch $(RELEASE_FILE)
#	tar cvz --exclude="*~" --exclude=$(RELEASE_FILE) --exclude='obj/*' --exclude='*.a'  --exclude='include/*' --exclude='bin/*' --exclude='test/results/*' --exclude-vcs --exclude-backups -f $(RELEASE_FILE) --transform 's,^,$(DIR_NAME)_$(VERSION)/$(DIR_NAME)/,' --transform 's,$(DIR_NAME)/$(BASE_LIB_DIRNAME),$(BASE_LIB_DIRNAME),'  --transform 's,$(DIR_NAME)/$(WHOLEPACKAGE_MAKE),$(WHOLEPACKAGE_MAKE),' * $(BASE_LIB_PATH) $(WHOLEPACKAGE_MAKE) --show-transformed-names
