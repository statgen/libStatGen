
MAKEFILES_PATH := $(dir $(lastword $(MAKEFILE_LIST)))
include $(MAKEFILES_PATH)Makefile.base

RELEASE_FILE?=$(DIR_NAME).$(VERSION).tgz

ADDITIONAL_HELP= @echo "make install      Install binaries in $(INSTALLDIR)";\
	echo "make install INSTALLDIR=directory_for_binaries";\
	echo "                  Install binaries in directory_for_binaries"


.PHONY: package package2

package : 
# the touch gets rid of a tar warning
#	echo directory name = $(DIR_NAME)
	touch $(RELEASE_FILE)
	tar cvz --exclude="*~" --exclude=$(RELEASE_FILE) --exclude='obj/*' --exclude='*.a'  --exclude='include/*' --exclude='bin/*' --exclude='test/results/*' --exclude-vcs --exclude-backups -f $(RELEASE_FILE) --transform 's,^,$(DIR_NAME)_$(VERSION)/,' * --show-transformed-names 

PACKAGE2_MAKE = $(MAKEFILES_PATH)/Makefile.include

package2 : 
# the touch gets rid of a tar warning
#	echo directory name = $(DIR_NAME)
	touch $(RELEASE_FILE)
	tar cvz --exclude="*~" --exclude=$(RELEASE_FILE) --exclude='obj/*' --exclude='*.a'  --exclude='include/*' --exclude='bin/*' --exclude='test/results/*' --exclude-vcs --exclude-backups -f $(RELEASE_FILE) --transform 's,^,$(DIR_NAME)_$(VERSION)/$(DIR_NAME)/,' --transform 's,$(DIR_NAME)/$(BASE_LIB_DIRNAME),$(BASE_LIB_DIRNAME),'  --transform 's,$(DIR_NAME)/$(PACKAGE2_MAKE),$(PACKAGE2_MAKE),' * $(BASE_LIB_PATH) $(PACKAGE2_MAKE) --show-transformed-names
