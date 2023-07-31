#include <drvEVG230.h>

EVG230::EVG230(const char* port_name, const char* name, int frequency)
    : asynPortDriver(port_name,
                     NUMBER_OF_COUNTERS,
                     asynInt32Mask | asynFloat64Mask | asynUInt32DigitalMask | asynDrvUserMask,
                     asynInt32Mask | asynFloat64Mask | asynUInt32DigitalMask,
                     ASYN_CANBLOCK | ASYN_MULTIDEVICE,
                     1,
                     0, 0)
{
    this->board = new EVG230Board(name, frequency);
    if(this->board->error != asynSuccess) {
        // TODO: Error reporting.
        printf("X Could not connect to the device.\n");
        return;
    }

    createParam(EVG_Firmware_Version, asynParamInt32,         &index_evg_firmware);
    createParam(EVG_Enable,           asynParamUInt32Digital, &index_evg_enable);
    createParam(EVG_Enabled,          asynParamUInt32Digital, &index_evg_is_enabled);
    createParam(EVG_Clock,            asynParamInt32,         &index_evg_clock);
    createParam(EVG_RF_Source,        asynParamInt32,         &index_evg_rf_source);
    createParam(EVG_AC_Sync_Source,   asynParamInt32,         &index_evg_ac_sync_source);
    createParam(EVG_RF_Prescaler,     asynParamInt32,         &index_evg_rf_prescaler);
    createParam(EVG_AC_Prescaler,     asynParamInt32,         &index_evg_ac_prescaler);

    this->frequency = frequency;
}

asynStatus EVG230::readInt32(asynUser* pasynUser, epicsInt32* value)
{
    int function = pasynUser->reason;
    int status;
    int reg;
    u16 data;

    if(function == index_evg_firmware)
        status = board->readFirmware(&data);
    else if(function == index_evg_clock)
        status = board->readClock(&data);
    else if(function == index_evg_rf_source)
        status = board->readRFSource(&data);
    else if(function == index_evg_ac_sync_source)
        status = board->readACSyncSource(&data);
    else if(function == index_evg_rf_prescaler)
        status = board->readRFPrescaler(&data);
    else if(function == index_evg_ac_prescaler)
        status = board->readACPrescaler(&data);
    else {
        cout << "readInt32: Unknown function" << endl;
        return asynError;
    }

    if(status == 0)
        *value = data;

    return (asynStatus) status;
}

asynStatus EVG230::writeInt32(asynUser* pasynUser, epicsInt32 value)
{
    int status = asynSuccess;
    int function = pasynUser->reason;
    int reg;
    u16 data;

    if(function == index_evg_rf_source)
        status = board->setRFSource(value);
    else if(function == index_evg_ac_sync_source)
        status = board->setACSyncSource(value);
    else if(function == index_evg_rf_prescaler)
        status = board->setRFPrescaler(value);
    else if(function == index_evg_ac_prescaler)
        status = board->setACPrescaler(value);
    else {
        cout << "readInt32: Unknown function" << endl;
        return asynError;
    }

    return (asynStatus) status;
}

asynStatus EVG230::writeUInt32Digital(asynUser* asyn_user, epicsUInt32 value, epicsUInt32 mask)
{
    int status = asynSuccess;
    int function = asyn_user->reason;

    if(function == index_evg_enable)
        status = (value & mask) == 0x0 ? board->disable() : board->enable();
    else {
        cout << "writeUInt32Digital: Unknown function" << endl;
        return asynError;
    }

    return (asynStatus) status;
}

asynStatus EVG230::readUInt32Digital(asynUser* asyn_user, epicsUInt32* value, epicsUInt32 mask)
{
    u16 data;
    int function = asyn_user->reason;
    int status = asynSuccess;

    if(function == index_evg_enable) {
        status = board->isEnabled(&data);
    }
    else {
        cout << "readUInt32Digital: Unknown function" << endl;
        return asynError;
    }

    if(status == 0)
        *value = data;

    return (asynStatus) status;
}

extern "C" {

    asynStatus EVG230Configure(const char* port_name, const char* name, int frequency) {
        new EVG230(port_name, name, frequency);
        return asynSuccess;
    }

    static const iocshArg arg0 = { "Name", iocshArgString };
    static const iocshArg arg1 = { "Name", iocshArgString };
    static const iocshArg arg2 = { "Frequency", iocshArgInt };
    static const iocshArg* const configArgs[] = {&arg0, &arg1};

    static const iocshFuncDef configFuncDef = {"EVG230Configure", 2, configArgs};
    static void configCallFunc(const iocshArgBuf *args)
    {
        EVG230Configure(args[0].sval, args[1].sval, args[2].ival);
    }

    void drvEVG230Register(void)
    {
        iocshRegister(&configFuncDef, configCallFunc);
    }

    epicsExportRegistrar(drvEVG230Register);
}