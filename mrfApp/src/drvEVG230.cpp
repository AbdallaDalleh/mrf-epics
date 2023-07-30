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
    int status = pasynOctetSyncIO->connect(name, 1, &asyn_user, NULL);
    if(status != asynSuccess) {
        // TODO: Error reporting.
        printf("Could not connect to the device.\n");
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
        reg = REGISTER_FIRMWARE;
    else if(function == index_evg_clock)
        reg = REGISTER_USEC_DIVIDER;
    else if(function == index_evg_rf_source || function == index_evg_rf_prescaler)
        reg = REGISTER_RF_CONTROL;
    else if(function == index_evg_ac_sync_source || function == index_evg_ac_prescaler)
        reg = REGISTER_AC_ENABLE;
    else {
        return asynError;
    }

    status = readRegister(reg, &data);
    if(status != asynSuccess) {
        // TODO: Error reporting.
        printf("readInt32 error: %d\n", reg);
    }
    else {
        if(function == index_evg_rf_source)
            *value = (data & RF_CONTROL_EXTERNAL) != 0;
        else if(function == index_evg_ac_sync_source)
            *value = (data & AC_ENABLE_SYNC) != 0;
        else if(function == index_evg_rf_prescaler)
            *value = (data & RF_CONTROL_DIVIDER_MASK) + 1;
        else if(function == index_evg_ac_prescaler)
            *value = (data & AC_ENABLE_DIVIDER_MASK) + 1;
        else
            *value = data;
    }

    return asynSuccess;
}

asynStatus EVG230::writeInt32(asynUser* pasynUser, epicsInt32 value)
{
    int status = asynSuccess;
    int function = pasynUser->reason;
    int reg;
    u16 data;

    if(function == index_evg_rf_source) {
        reg    = REGISTER_RF_CONTROL;
        status = readRegister(reg, &data);
        data   = value == RF_SOURCE_INTERNAL ? data & (~RF_CONTROL_EXTERNAL) : (data | RF_CONTROL_EXTERNAL);
    }
    else if(function == index_evg_ac_sync_source) {
        reg    = REGISTER_AC_ENABLE;
        status = readRegister(reg, &data);
        data   = value == AC_SOURCE_EVENT ? data & (~AC_ENABLE_SYNC) : (data | AC_ENABLE_SYNC);
    }
    else if(function == index_evg_rf_prescaler) {
        reg    = REGISTER_RF_CONTROL;
        status = readRegister(reg, &data);
        data   = (data & ~RF_CONTROL_DIVIDER_MASK) | ((value - 1));
    }
    else if(function == index_evg_ac_prescaler) {
        reg    = REGISTER_AC_ENABLE;
        status = readRegister(reg, &data);
        data   = (data & ~AC_ENABLE_DIVIDER_MASK) | ((value - 1));
    }
    else {
        // TODO: Error reporting.
        printf("writeInt32 unknown function error.\n");
        return asynError;
    }

    if(status != asynSuccess) {
        printf("writeInt32 IO error: %d | %d\n", reg, function);
        return asynError;
    }

    status = writeRegister(reg, data);
    return (asynStatus) status;
}

asynStatus EVG230::writeUInt32Digital(asynUser* asyn_user, epicsUInt32 value, epicsUInt32 mask)
{
    int status = asynSuccess;
    int function = asyn_user->reason;

    if(function == index_evg_enable)
        status = writeRegister(REGISTER_CONTROL, (value & mask) == 0x1 ? CONTROL_ENABLE: CONTROL_DISABLE);

    return (asynStatus) status;
}

asynStatus EVG230::readUInt32Digital(asynUser* asyn_user, epicsUInt32* value, epicsUInt32 mask)
{
    u16 data;
    int function = asyn_user->reason;
    int status = asynSuccess;

    if(function == index_evg_is_enabled) {
        status = readRegister(REGISTER_CONTROL, &data);
        if(status == asynSuccess)
            *value = !((data & CONTROL_DISABLE_BIT) == 0x8000);
    }
    return (asynStatus) status;
}

int EVG230::writeRegister(int reg, u16 data)
{
    message_t message;
    char tx_buffer[PACKET_SIZE];
    char rx_buffer[PACKET_SIZE];
    int status;
    size_t tx_bytes;
    size_t rx_bytes;
    int reason;

    message.access    = ACCESS_WRITE;
    message.status    = 0;
    message.data      = htons(data);
    message.address   = htonl(REGISTER_BASE_ADDRESS + reg);
    message.reference = 0x00000000;

    memcpy(tx_buffer, (void*) &message, sizeof(tx_buffer));
    status = pasynOctetSyncIO->writeRead(this->asyn_user, tx_buffer, PACKET_SIZE, rx_buffer, PACKET_SIZE, 2, &tx_bytes, &rx_bytes, &reason);
    if(status != asynSuccess || tx_bytes != PACKET_SIZE || rx_bytes != PACKET_SIZE) {
        // TODO: Error reporting.
        printf("Could not write to register: %d\n", reg);
        return status;
    }

    memcpy(&message, rx_buffer, sizeof(message_t));
    if(ntohs(message.status) != 0 /*|| ntohs(message.data) != data*/) {
        // TODO: Error reporting.
        printf("Device status error or data mismatch\n.");
        return asynError;
    }

    return asynSuccess;
}

int EVG230::readRegister(int reg, u16* data)
{
    message_t message;
    char tx_buffer[PACKET_SIZE];
    char rx_buffer[PACKET_SIZE];
    int status;
    size_t tx_bytes;
    size_t rx_bytes;
    int reason;

    message.access    = ACCESS_READ;
    message.status    = 0;
    message.data      = 0x0000;
    message.address   = htonl(REGISTER_BASE_ADDRESS + reg);
    message.reference = 0x00000000;

    memcpy(tx_buffer, (void*) &message, sizeof(tx_buffer));
    status = pasynOctetSyncIO->writeRead(this->asyn_user, tx_buffer, PACKET_SIZE, rx_buffer, PACKET_SIZE, 2, &tx_bytes, &rx_bytes, &reason);
    if(status != asynSuccess || tx_bytes != PACKET_SIZE || rx_bytes != PACKET_SIZE) {
        // TODO: Error reporting.
        printf("Could not read register: %d\n", reg);
        return status;
    }

    memcpy(&message, rx_buffer, sizeof(message_t));
    if(ntohs(message.status) != 0) {
        // TODO: Error reporting.
        printf("Device status error\n.");
        return asynError;
    }

    *data = ntohs(message.data);
    return asynSuccess;
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