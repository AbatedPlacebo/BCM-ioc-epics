#Makefile at top of application tree
TOP = .
include $(TOP)/configure/CONFIG
DIRS := $(DIRS) $(filter-out $(DIRS), configure)
DIRS := $(DIRS) $(filter-out $(DIRS), $(wildcard *App))
DIRS := $(DIRS) $(filter-out $(DIRS), $(wildcard iocBoot))

define DIR_template
 $(1)_DEPEND_DIRS = configure
endef
$(foreach dir, $(filter-out configure,$(DIRS)),$(eval $(call DIR_template,$(dir))))

iocBoot_DEPEND_DIRS += $(filter %App,$(DIRS))
configure_DEPEND_DIRS += genlib

#someApp_DEPEND_DIRS += BCMApp # someApp будет собран после BCMApp

ifneq ($(wildcard /usr/bin/cscope),)
configure_DEPEND_DIRS += tags
endif

include $(TOP)/configure/RULES_TOP

genlib.install: genlib/gen_c.h
genlib.clean:
genlib.realclean:

genlib/gen_c.h:
	git submodule init
	git submodule update

tags: cscope.out

ifneq ($(filter tags,$(configure_DEPEND_DIRS)),)
tags.install: tags
tags.clean:
tags.realclean:
	rm -f cscope.out
endif

CSCOPE_FILES=$(wildcard  src/*.cpp inc/*.h *App/src/*.c *App/src/*pp genlib/*.h)

cscope.out: $(CSCOPE_FILES)
	cscope -b $^

ifneq ("$(INSTALL_LOCATION)",".")

#install: $(INSTALL_LOCATION)/vepp4.acf

#$(INSTALL_LOCATION)/vepp4.acf: vepp4.acf
#	cp $< $@

endif

