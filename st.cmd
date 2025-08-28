#!../../bin/linux-x86_64/ioc

< iocBoot/ioc/envPaths
epicsEnvSet("EVR_MAPS", "maps/")

dbLoadDatabase "dbd/ioc.dbd"
ioc_registerRecordDeviceDriver pdbbase

dbLoadRecords("db/evg.db")
dbLoadRecords("db/evg-mxc.db")
dbLoadRecords("db/evg-seq.db")
dbLoadRecords("db/evg-events.db")

dbLoadRecords("db/evr.db")
dbLoadRecords("db/evr-pulsers.db")
dbLoadRecords("db/evr-ttl.db")
dbLoadRecords("db/evr-univ.db")
dbLoadRecords("db/evr-pdp.db")
dbLoadRecords("db/evr-cml.db")
dbLoadRecords("db/evr-prescaler.db")

dbLoadRecords("db/filling.db")

< ip.cmd

drvAsynIPPortConfigure("EVG230",   "$(EVG_IP):2000 UDP", 0, 0, 1)
drvAsynIPPortConfigure("EVR230_0", "$(EVR0_IP):2000 UDP", 0, 0, 1)
drvAsynIPPortConfigure("EVR230_1", "$(EVR1_IP):2000 UDP", 0, 0, 1)

EVG230Configure("EVG", "EVG230", 125)
EVR230Configure("EVR0", "EVR230_0", 125)
EVR230Configure("EVR1", "EVR230_1", 125)

save_restoreSet_Debug(0)
set_requestfile_path("$(TOP)/autosave/")
set_savefile_path("$(TOP)/autosave/")
set_pass0_restoreFile("timing.sav")
set_pass1_restoreFile("timing.sav")
save_restoreSet_DatedBackupFiles(0)

iocInit

create_monitor_set("timing.req", 5, "")

