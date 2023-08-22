
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

#include "evr230_registers.h"
#include "types.h"

int evr230_init(asynUser* device, const char* asyn_name, u32 frequency);
int evr230_enable(asynUser* device, int enable);
int evr230_is_enabled(asynUser* device, int* enabled);
int evr230_set_clock(asynUser* device, u16 clock);
int evr230_get_clock(asynUser* device, u16* clock);
int evr230_flush(asynUser* device);

int evr230_read(asynUser* device, int address, u16* data);
int evr230_write(asynUser* device, int address, u16 data);
