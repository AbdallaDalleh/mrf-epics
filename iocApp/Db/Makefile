# SPDX-FileCopyrightText: 1998 Argonne National Laboratory
#
# SPDX-License-Identifier: EPICS

TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#----------------------------------------------------
# Create and install (or just install) into <top>/db
# databases, templates, substitutions like this
#DB += xxx.db
DB += evg.db
DB += evg-mxc.db
DB += evg-seq.db
DB += evg-events.db

DB += evr.db
DB += evr-pulsers.db
DB += evr-ttl.db
DB += evr-univ.db
DB += evr-pdp.db
DB += evr-cml.db
DB += evr-prescaler.db

DB += filling.db
#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_template = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

