TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
USR_INCLUDES += -I$(TOP)/inc
USR_INCLUDES += -I$(TOP)/genlib
USR_INCLUDES += -I$(TOP)/src
USR_SYS_LIBS += gsl
USR_SYS_LIBS += m
USR_SYS_LIBS += gslcblas
VPATH += $(TOP)/src
USR_LDFLAGS += -rdynamic
#=============================
# Build the IOC support library

#LIBRARY_IOC += stabSupport

# xxxRecord.h will be created from xxxRecord.dbd
#DBDINC += xxxRecord
# Install devXxxSoft.dbd into <top>/dbd
#DBD += xxxSupport.dbd

# Compile and add the code to the support library
#stabSupport_SRCS += xxxRecord.c
#stabSupport_SRCS += devXxxSoft.c

# Link locally-provided code into the support library,
# rather than directly into the IOC application.
#stabSupport_SRCS += dbSubExample.c
#stabSupport_SRCS += stabHello.c
#stabSupport_SRCS += initTrace.c

#stabSupport_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================
# Build the IOC application

PROD_IOC = BCM
# BCM.dbd will be created and installed
DBD += BCM.dbd

# stab.dbd will be made up from these files:
BCM_DBD += base.dbd
#stab_DBD += xxxSupport.dbd
#stab_DBD += dbSubExample.dbd
#stab_DBD += stabHello.dbd
#stab_DBD += initTrace.dbd
BCM_DBD += BCMProc.dbd
BCM_DBD += asSupport.dbd

### DBD_ADD_PLACEHOLDER

# stab_registerRecordDeviceDriver.cpp derives from stab.dbd
BCM_SRCS += BCM_registerRecordDeviceDriver.cpp

# Build the main IOC entry point on workstation OSs.
BCM_SRCS_DEFAULT += iocMain.cpp
BCM_SRCS_vxWorks += -nil-

BCM_SRCS += BCMProc.c
BCM_SRCS += BCMCommunication.c
BCM_SRCS += BCMVariables.c
BCM_SRCS += BCMMath.c

# Add support from base/src/vxWorks if needed
#BCM_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Link in the code from the support library
#BCM_LIBS += stabSupport
BCM_LIBS += autosave

# NOTE: To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file

#ifneq ($(SNCSEQ),)
#    # Build sncExample into stabSupport
#    sncExample_SNCFLAGS += +r
#    stab_DBD += sncExample.dbd
#    stabSupport_SRCS += sncExample.stt
#    stabSupport_LIBS += seq pv
#    stab_LIBS += seq pv
#
#    # Build sncProgram as a standalone program
#    PROD_HOST += sncProgram
#    sncProgram_SNCFLAGS += +m
#    sncProgram_SRCS += sncProgram.st
#    sncProgram_LIBS += seq pv
#    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
#endif

# Finally link to the EPICS Base libraries
BCM_LIBS += $(EPICS_BASE_IOC_LIBS)

### LIBS_ADD_PLACEHOLDER

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

include $(TOP)/genlib/RULES_gen_dbd

BCMProc.dbd: BCMProc.c
-include BCMProc.dbd.dd

# v3.14
ifeq ($(BASE_3_14),YES)
$(COMMON_DIR)/BCMInclude.dbd: BCMProc.dbd
endif
$(COMMON_DIR)/BCM.dbd: BCMProc.dbd

