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

    createParam(EVG_Firmware_Version, asynParamInt32, &index_evg_firmware);

    this->frequency = frequency;
}

asynStatus EVG230::readInt32(asynUser* pasynUser, epicsInt32* value)
{
    int function = pasynUser->reason;
    int status;
    int reg;
    u16 data;

    if(function == index_evg_firmware) {
        reg = REGISTER_FIRMWARE;
    }

    status = readRegister(reg, &data);
    if(status != asynSuccess) {
        // TODO: Error reporting.
        printf("readInt32 error: %d\n", reg);
    }
    else
        *value = data;

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
        printf("Could not write to register: %d\n", reg);
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