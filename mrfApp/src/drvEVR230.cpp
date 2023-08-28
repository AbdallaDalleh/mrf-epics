
#include "drvEVR230.h"

EVR230::EVR230(const char* port_name, const char* asyn_name, int frequency)
	: asynPortDriver(
			port_name,
			256,
			asynInt32Mask | asynDrvUserMask,
			asynInt32Mask,
			ASYN_MULTIDEVICE | ASYN_CANBLOCK,
			1, 0, 0)
{
	int status;

	status = pasynOctetSyncIO->connect(asyn_name, 1, &device, NULL);
	if(status != asynSuccess) {
		// TODO: Error reporting.
		printf("Could not connect to device %s\n", asyn_name);
		return;
	}

	status = evr230_init(device, asyn_name, frequency);
	if(status != 0)
		return;

	createParam(EVR_Clock,            asynParamInt32, &index_evr_clock);
	createParam(EVR_Firmware_Version, asynParamInt32, &index_evr_firmware);
}

asynStatus EVR230::readInt32(asynUser* asyn_user, epicsInt32* value)
{
	int status;
	int function;
	u16 data;

	function = asyn_user->reason;
	if(function == index_evr_clock)
		status = evr230_get_clock(device, &data);
	else if(function == index_evr_firmware)
		status = evr230_get_firmware_version(device, &data);
	else {
		printf("readInt32: Unknown function: %d\n", function);
		return asynError;
	}

	if(status != asynSuccess) {
		printf("readInt32: IO error\n");
		return asynError;
	}

	*value = data;
	return (asynStatus) status;
}

extern "C" {

    asynStatus EVR230Configure(const char* port_name, const char* name, int frequency) {
        new EVR230(port_name, name, frequency);
        return asynSuccess;
    }

    static const iocshArg arg0 = { "Name", iocshArgString };
    static const iocshArg arg1 = { "Name", iocshArgString };
    static const iocshArg arg2 = { "Frequency", iocshArgInt };
    static const iocshArg* const configArgs[] = {&arg0, &arg1, &arg2};

    static const iocshFuncDef configFuncDef = {"EVR230Configure", 3, configArgs};
    static void configCallFunc(const iocshArgBuf *args)
    {
        EVR230Configure(args[0].sval, args[1].sval, args[2].ival);
    }

    void drvEVR230Register(void)
    {
        iocshRegister(&configFuncDef, configCallFunc);
    }
	
	epicsExportRegistrar(drvEVR230Register);
}

