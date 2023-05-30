TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#----------------------------------------------------
#  Optimization of db files using dbst (DEFAULT: NO)
#DB_OPT = YES

#----------------------------------------------------
# Create and install (or just install)
# databases, templates, substitutions like this
#DB += dbExample1.db
#DB += dbExample2.db
#DB += dbSubExample.db
#DB += user.substitutions
#DB += userHost.substitutions
DB += BCMProc.db
DB += BCM.db
DB += BCM.2.db
DB_INSTALLS += $(AUTOSAVE)/db/save_restoreStatus.db
### DB_INSTALLS_PLACEHOLDER

#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

USR_INCLUDES += -I$(TOP)/inc
USR_INCLUDES += -I$(TOP)/genlib
include $(TOP)/genlib/RULES_gen_db

VPATH += ../../src
VPATH += ..

$(COMMON_DIR)/BCMProc.db: BCMProc.c BCM.h ../Makefile $(GEN_DEPEND) 
BCMProc.db.d:

$(COMMON_DIR)/BCM.2.db: BCM.2.sh $(GEN_DEPEND)
BCM.2.db.d:



#$(COMMON_DIR)/BCMSH_.db: BCMSH_.sh
#BCMSH_.db.d:

