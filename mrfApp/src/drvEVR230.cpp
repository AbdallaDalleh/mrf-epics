
#include "drvEVR230.h"

EVR230::EVR230(const char* port_name, const char* asyn_name, int frequency)
	: asynPortDriver(
			port_name,
			256,
			asynInt32Mask | asynUInt32DigitalMask | asynFloat64Mask | asynDrvUserMask,
			asynInt32Mask | asynUInt32DigitalMask | asynFloat64Mask,
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
	this->frequency = frequency;

	createParam(EVR_Clock,            asynParamInt32,         &index_evr_clock);
	createParam(EVR_Firmware_Version, asynParamInt32,         &index_evr_firmware);
	createParam(EVR_RX_Violation,     asynParamUInt32Digital, &index_evr_rx_violation);
	createParam(EVR_Reset_RX,         asynParamInt32,         &index_evr_reset_rx);
	createParam(EVR_Enable,           asynParamUInt32Digital, &index_evr_enable);
	createParam(EVR_Pulser_Enable,    asynParamUInt32Digital, &index_evr_otp_enable);
	createParam(EVR_Pulser_Delay,     asynParamFloat64,       &index_evr_otp_delay);
	createParam(EVR_Pulser_Width,     asynParamFloat64,       &index_evr_otp_width);
	createParam(EVR_Connect,          asynParamInt32,         &index_evr_connect);
    createParam("EVR_TTL_Source",     asynParamInt32,         &index_evr_ttl_source);
    createParam(EVR_Universal_Source, asynParamInt32,         &index_evr_univ_source);
    createParam(EVR_PDP_Enable,       asynParamUInt32Digital, &index_evr_pdp_enable);
    createParam(EVR_PDP_Prescaler,    asynParamInt32,         &index_evr_pdp_prescaler);
    createParam(EVR_PDP_Delay,        asynParamFloat64,       &index_evr_pdp_delay);
    createParam(EVR_PDP_Width,        asynParamFloat64,       &index_evr_pdp_width);
    createParam(EVR_CML_Enable,       asynParamUInt32Digital, &index_evr_cml_enable);
    createParam(EVR_CML_Prescaler,    asynParamInt32,         &index_evr_cml_prescaler);
    createParam(EVR_Prescaler,        asynParamInt32,         &index_evr_prescaler);

    // enable event map, testing only ...
    evr230_write(device, 0x02,  1);
    evr230_write(device, 0x04, 0x3FFF);
    evr230_write(device, 0x42, 0x0F);

    u16 data;
    evr230_read(device, 0xb2, &data);
    printf("CML: 0x%X\n", data);
}

asynStatus EVR230::readInt32(asynUser* asyn_user, epicsInt32* value)
{
	int status;
	int function;
	int address;
    u16 data;

	function = asyn_user->reason;
	getAddress(asyn_user, &address);
	if(function == index_evr_clock)
		status = evr230_get_clock(device, &data);
	else if(function == index_evr_firmware)
		status = evr230_get_firmware_version(device, &data);
    else if(function == index_evr_ttl_source)
        status = evr230_get_ttl_source(device, address, &data);
    else if(function == index_evr_univ_source)
        status = evr230_get_universal_source(device, address, &data);
    else if(function == index_evr_pdp_prescaler)
        status = evr230_get_pdp_prescaler(device, address, &data);
    else if(function == index_evr_cml_prescaler)
        status = evr230_get_cml_prescaler(device, address, &data);
    else if(function == index_evr_prescaler)
        status = evr230_get_prescaler(device, address, &data);
	else if(function == index_evr_reset_rx || function == index_evr_connect)
		return asynSuccess;
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

asynStatus EVR230::writeInt32(asynUser* asyn_user, epicsInt32 value)
{
	int status;
	int function;
    int address;

	function = asyn_user->reason;
	getAddress(asyn_user, &address);
	if(function == index_evr_reset_rx)
		status = evr230_reset_rx(device);
	else if(function == index_evr_connect) {
		printf("CONNECT EVR CLOCK: %d\n", this->frequency);
		status  = evr230_enable(device, 1);
		status |= evr230_set_clock(device, this->frequency);
		status |= evr230_flush(device);
		// evr230_reset_rx(device);
	}
    else if (function == index_evr_ttl_source)
        status = evr230_set_ttl_source(device, address, (u16) value);
    else if (function == index_evr_univ_source)
        status = evr230_set_universal_source(device, address, value);
    else if (function == index_evr_pdp_prescaler)
        status = evr230_set_pdp_prescaler(device, address, value);
    else if (function == index_evr_cml_prescaler)
        status = evr230_set_cml_prescaler(device, address, value);
    else if (function == index_evr_prescaler)
        status = evr230_set_prescaler(device, address, value);
	else {
		printf("writeInt32: Unknown function %d \n", function);
		return asynError;
	}

	return (asynStatus) status;
}

asynStatus EVR230::readUInt32Digital(asynUser* asyn_user, epicsUInt32* value, epicsUInt32 mask)
{
	int status;
	int function;
	int address;
	u16 data;

	function = asyn_user->reason;
	getAddress(asyn_user, &address);
	if(function == index_evr_rx_violation)
		status = evr230_is_rx_violation(device, &data);
	else if(function == index_evr_enable) {
		status = evr230_is_enabled(device, &data);
	}
	else if(function == index_evr_otp_enable)
		status = evr230_is_otp_enabled(device, address, &data);
    else if(function == index_evr_pdp_enable)
        status = evr230_is_pdp_enabled(device, address, &data);
    else if(function == index_evr_cml_enable)
        status = evr230_is_cml_enabled(device, address, &data);
	else {
		printf("readUInt32Digital: Unknown function: %d\n", function);
		return asynError;
	}

	if(status != asynSuccess) {
		printf("readUInt32Digital: I/O Error.\n");
		return asynError;
	}

	*value = (data);
	return (asynStatus) status;
}

asynStatus EVR230::writeUInt32Digital(asynUser* asyn_user, epicsUInt32 value, epicsUInt32 mask)
{
	int status;
	int function;
	int address;
 
	function = asyn_user->reason;
	getAddress(asyn_user, &address);
	if(function == index_evr_enable)
		status = evr230_enable(device, value);
	else if(function == index_evr_otp_enable)
		status = evr230_enable_otp(device, address, value);
    else if(function == index_evr_pdp_enable)
        status = evr230_enable_pdp(device, address, value);
    else if(function == index_evr_cml_enable)
        status = evr230_enable_cml(device, address, value);
	else {
		printf("writeUInt32Digital: Unknown function: %d\n", function);
		return asynError;
	}

	return (asynStatus) status;
}

asynStatus EVR230::readFloat64(asynUser* asyn_user, epicsFloat64* value)
{
	int status;
	int function;
	int address;
	double data;

	function = asyn_user->reason;
	getAddress(asyn_user, &address);
	if(function == index_evr_otp_delay)
		status = evr230_get_otp_delay(device, address, &data);
	else if(function == index_evr_otp_width)
		status = evr230_get_otp_width(device, address, &data);
    else if(function == index_evr_pdp_delay)
		status = evr230_get_pdp_delay(device, address, &data);
	else if(function == index_evr_pdp_width)
		status = evr230_get_pdp_width(device, address, &data);
	else {
		printf("readFloat64: Unknown function %d\n", function);
		return asynError;
	}

	if(status != asynSuccess) {
		printf("readFloat64: I/O error\n");
		return asynError;
	}

	*value = data;
	return asynSuccess;
}

asynStatus EVR230::writeFloat64(asynUser* asyn_user, epicsFloat64 value)
{
	int status;
	int function;
	int address;

	function = asyn_user->reason;
	getAddress(asyn_user, &address);
	if(function == index_evr_otp_delay)
		status = evr230_set_otp_delay(device, address, value);
	else if(function == index_evr_otp_width)
		status = evr230_set_otp_width(device, address, value);
    else if(function == index_evr_pdp_delay)
		status = evr230_set_pdp_delay(device, address, value);
	else if(function == index_evr_pdp_width)
		status = evr230_set_pdp_width(device, address, value);
	else {
		printf("writeFloat64: Unknown function %d\n", function);
		return asynError;
	}

	if(status != asynSuccess)
		printf("writeFloat64: I/O error\n");

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

