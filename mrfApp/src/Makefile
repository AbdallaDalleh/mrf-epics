TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#==================================================
# build a support library

LIBRARY_IOC += mrf

# xxxRecord.h will be created from xxxRecord.dbd
#DBDINC += xxxRecord
# install mrf.dbd into <top>/dbd
DBD += mrf.dbd

# specify all source files to be compiled and added to the library
mrf_SRCS += drvEVG230.cpp
mrf_SRCS += drvEVR230.cpp
mrf_SRCS += EVG230Board.cpp
mrf_SRCS += evr230.cpp
mrf_INC  += registers.h
mrf_INC  += types.h
mrf_INC  += EVG230Board.h
mrf_INC  += evr230.h

mrf_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

