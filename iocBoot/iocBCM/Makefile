TOP = ../..
include $(TOP)/configure/CONFIG
ARCH = $(EPICS_HOST_ARCH)
TARGETS = envPaths BCM.cmd
SCRIPTS = BCM.cmd envPaths
INSTALL_LOCATION_SCRIPTS = $(INSTALL_LOCATION)/iocBoot/iocBCM
INSTALL_SCRIPTS = $(SCRIPTS:%= $(INSTALL_LOCATION_SCRIPTS)/%)
include $(TOP)/configure/RULES.ioc

all: BCM.cmd

BCM.cmd:
	sed "s/_EPICS_HOST_ARCH_/$(EPICS_HOST_ARCH)/" BCM.cmd.sample > $@
	chmod +x $@

$(INSTALL_LOCATION_SCRIPTS)/%.cmd: %.cmd
	mkdir -p $(INSTALL_LOCATION_SCRIPTS)
	test $< -ef $@ || cp $< $@

$(INSTALL_LOCATION_SCRIPTS)/envPaths: envPaths
	mkdir -p $(INSTALL_LOCATION_SCRIPTS)
	if ! test $< -ef $@; then \
		grep -v TOP < $< > $@; \
		echo "epicsEnvSet(\"TOP\",\"$(INSTALL_LOCATION)\")" >> $@; \
	fi

install: $(INSTALL_SCRIPTS)
