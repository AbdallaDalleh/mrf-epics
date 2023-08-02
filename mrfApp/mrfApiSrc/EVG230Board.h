
#include <cstdio>
#include <cstring>
#include <arpa/inet.h>
#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;

#include <asynPortDriver.h>
#include <asynOctetSyncIO.h>

#include "registers.h"
#include "types.h"

class EVG230Board
{
public:
    EVG230Board(string asyn_name, int frequency);

    int readFirmware(u16* data);
    int readClock(u16* data);
    int readRFSource(u16* source);
    int setRFSource(u16 source);
    int enable();
    int disable();
    int isEnabled(u16* value);
    int readACSyncSource(u16* source);
    int setACSyncSource(u16 source);
    int readRFPrescaler(u16* data);
    int setRFPrescaler(u16 data);
    int readACPrescaler(u16* data);
    int setACPrescaler(u16 data);
    int readMXCPrescaler(int counter, u32* data);
    int setMXCPrescaler(int counter, u32 data);
    int enableSequencer(int seq);
    int disableSequencer(int seq);
    int isSequencerEnabled(int seq, u16* data);
    int readSequencerPrescaler(int seq, u16* data);
    int setSequencerPrescaler(int seq, u16 data);
    int readSequencerTriggerSource(int seq, u16* data);
    int setSequencerTriggerSource(int seq, u16 data);
    int readSequencerEvent(int seq, int address, u16* data);
    int setSequencerEvent(int seq, int address, u16 data);
    int readSequencerEventTime(int seq, int address, u32* data);
    int setSequencerEventTime(int seq, int address, u32 data);

    int readRegister(int reg, u16* data);
    int writeRegister(int reg, u16 data);

    int error;
    string errorMessage;

private:
    asynUser* device;
    int frequency;
    u16 raw_data;
};