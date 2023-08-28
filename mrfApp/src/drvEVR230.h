#ifndef EVR230_H
#define EVR230_H

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
using std::string;
using std::cout;
using std::endl;

#include <epicsExport.h>
#include <asynPortDriver.h>
#include <iocsh.h>
#include <asynOctetSyncIO.h>

#include "EVR230Board.h"

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

#define EVR_CML_Enable          "EVR_CML_Enable"
#define EVR_CML_Prescaler       "EVR_CML_Prescaler"
#define EVR_Event_Map           "EVR_Event_Map"
#define EVR_PDP_Enable          "EVR_PDP_Enable"
#define EVR_PDP_Prescaler       "EVR_PDP_Prescaler"
#define EVR_PDP_Delay           "EVR_PDP_Delay"
#define EVR_PDP_Width           "EVR_PDP_Width"
#define EVR_Prescaler           "EVR_Prescaler"
#define EVR_Pulser_Enable       "EVR_Pulser_Enable"
#define EVR_Pulser_Delay        "EVR_Pulser_Delay"
#define EVR_Pulser_Width        "EVR_Pulser_Width"
#define EVR_Enable              "EVR_Enable"
#define EVR_RX_Violation        "EVR_RX_Violation"
#define EVR_Reset_RX            "EVR_Reset_RX"
#define EVR_Clock               "EVR_Clock"
#define EVR_Firmware_Version    "EVR_Firmware_Version"
#define EVR_TTL_Source          "EVR_TTL_Source"
#define EVR_Universal_Source    "EVR_Universal_Source"

class EVR230 : public asynPortDriver
{
public:
	EVR230(const char* port_name, const char* asyn_name, int frequency);
	// ~EVR230();
	virtual asynStatus readInt32(asynUser* asyn_user, epicsInt32* value);
	virtual asynStatus writeInt32(asynUser* asyn_user, epicsInt32 value);
	virtual asynStatus readUInt32Digital(asynUser* asyn_user, epicsUInt32* value, epicsUInt32 mask);
    virtual asynStatus writeUInt32Digital(asynUser* asyn_user, epicsUInt32 value, epicsUInt32 mask);

protected:
    int index_evr_clock;
    int index_evr_firmware;
	int index_evr_rx_violation;
	int index_evr_reset_rx;
	int index_evr_enable;

private:
	asynUser* device;
};

#endif

