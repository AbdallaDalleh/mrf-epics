#ifndef EVG230_H
#define EVG230_H

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

#include "EVG230Board.h"

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

// Register Addresses
#define REGISTER_CONTROL        0x00
#define REGISTER_EVENT_ENABLE   0x02
#define REGISTER_SW_EVENT       0x04
#define REGISTER_SEQ_CLOCK_SEL1 0x24
#define REGISTER_SEQ_CLOCK_SEL2 0x26
#define REGISTER_AC_ENABLE      0x28
#define REGISTER_MXC_CONTROL    0x2A
#define REGISTER_MXC_PRESCALER  0x2C
#define REGISTER_FIRMWARE       0x2E
#define REGISTER_RF_CONTROL     0x40
#define REGISTER_SEQ_ADDRESS0   0x44
#define REGISTER_SEQ_CODE0      0x46
#define REGISTER_SEQ_TIME0      0x48
#define REGISTER_SEQ_ADDRESS1   0x50
#define REGISTER_SEQ_CODE1      0x52
#define REGISTER_SEQ_TIME1      0x54
#define REGISTER_USEC_DIVIDER   0x68

// Register definitions
#define CONTROL_DISABLE         0xF001
#define CONTROL_DISABLE_BIT     0x8000
#define CONTROL_ENABLE          0x7001
#define CONTROL_VTRG1           0x0100
#define CONTROL_VTRG2           0x0080
#define EVENT_ENABLE_VME        0x0001
#define EVENT_ENABLE_SEQUENCER1 0x0002
#define EVENT_ENABLE_SEQUENCER0 0x0004
#define AC_ENABLE_SEQ1          0x8000
#define AC_ENABLE_SEQ0          0x4000
#define AC_ENABLE_SYNC          0x1000
#define AC_ENABLE_DIVIDER_MASK  0x00FF
#define MXC_CONTROL_HIGH_WORD   0x0008
#define RF_CONTROL_EXTERNAL     0x01C0
#define RF_CONTROL_DIVIDER_MASK 0x003F
#define USEC_DIVIDER            125

#define EVENT_END_SEQUENCE      0x7f

/*Device name maximum length*/
#define NAME_LENGTH            30
/*evg register base address*/
#define REGISTER_BASE_ADDRESS  0x80000000

#define NUMBER_OF_EVENTS     100
#define NUMBER_OF_SEQUENCERS 2
#define NUMBER_OF_ADDRESSES  2048
#define NUMBER_OF_COUNTERS   8
#define MAX_EVENT_FREQUENCY  125
#define MAX_SEQUENCERS       2
#define NUMBER_OF_RETRIES	 3 /*Maximum number of retransmissions*/
#define PACKET_SIZE          12

// Asyn Parameters
// mrf-vmeevg230.template
#define EVG_Enabled             "EVG_is_enabled"
#define EVG_Enable              "EVG_Enable"
#define EVG_Clock               "EVG_Clock"
#define EVG_RF_Source           "EVG_RF_Source"
#define EVG_RF_Prescaler        "EVG_RF_Prescaler"
#define EVG_AC_Sync_Source      "EVG_AC_Sync_Source"
#define EVG_AC_Prescaler        "EVG_AC_Prescaler"
#define EVG_Software_Event      "EVG_Software_Event"
#define EVG_Firmware_Version    "EVG_Firmware_Version"

// mrf-vmeevg230-sequencer.template
#define EVG_SEQ_Enabled        "EVG_SEQ_Enabled"
#define EVG_SEQ_Trigger_Source "EVG_SEQ_Trigger_Source"
#define EVG_SEQ_Prescaler      "EVG_SEQ_Prescaler"
#define EVG_SEQ_Trigger        "EVG_SEQ_Trigger"

// mrf-vmeevg230-event.template
#define EVG_SEQ0_Event          "EVG_SEQ0_Event"
#define EVG_SEQ1_Event          "EVG_SEQ1_Event"
#define EVG_SEQ0_Event_Time     "EVG_SEQ0_Event_Time"
#define EVG_SEQ1_Event_Time     "EVG_SEQ1_Event_Time"

// mrf-vmeevg230-counter.template
#define EVG_MXC_Prescaler   "EVG_MXC_Prescaler"

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define ERROR(x, y) \
    if((x)) \
    { \
        printf("%s | %s | %d : %s\n", __FILENAME__, __func__, __LINE__, y); \
        return asynError; \
    } \

typedef enum
{
	TRIGGER_SOFT,
	TRIGGER_AC
} triggersource_t;

class EVG230 : public asynPortDriver
{
public:
    EVG230(const char* port_name, const char* name, int frequency);
    // ~EVG230();
    virtual asynStatus readInt32(asynUser* asyn_user, epicsInt32* value);
    virtual asynStatus writeInt32(asynUser* asyn_user, epicsInt32 value);
    virtual asynStatus readUInt32Digital(asynUser* asyn_user, epicsUInt32* value, epicsUInt32 mask);
    virtual asynStatus writeUInt32Digital(asynUser* asyn_user, epicsUInt32 value, epicsUInt32 mask);

protected:
    int index_evg_is_enabled;
    int index_evg_enable;
    int index_evg_clock;
    int index_evg_rf_source;
    int index_evg_rf_prescaler;
    int index_evg_ac_sync_source;
    int index_evg_ac_prescaler;
    int index_evg_software_event;
    int index_evg_firmware;

    int index_evg_seq_enabled;
    int index_evg_seq_trigger_source;
    int index_evg_seq_prescaler;
    int index_evg_seq_trigger;

    int index_seq0_event;
    int index_seq0_event_time;
    int index_seq1_event;
    int index_seq1_event_time;

    int index_counter_prescaler;

private:
    asynUser* asyn_user;
    EVG230Board* board;
    int frequency;
};

#endif
