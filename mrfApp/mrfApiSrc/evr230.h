
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
#include "types.h"

#define BASE_ADDRESS      0x7A000000
#define OTP_SELECT_OFFSET 16

#define EVR230_CONTROL           0x00
#define EVR230_OTP_ENABLE        0x06
#define EVR230_PDP_ENABLE        0x18
#define EVR230_PULSE_SELECT      0x1A
#define EVR230_PDP_PRESCALER     0x28
#define EVR230_FIRMWARE_VERSION  0x2E
#define EVR230_USEC_DIVIDER      0x4E
#define EVR230_PULSE_DELAY       0x6C
#define EVR230_PULSE_WIDTH       0x70
#define EVR230_FP_TTL(x)        (0x40 + x*2)
#define EVR230_FP_UNIV(x)       (0x90 + x*2)

// Control/Status Register
// Address: 0x00
#define EVREN  0x8000
#define IRQEN  0x4000
#define RSTS   0x2000
#define HRTBT  0x1000
#define IRQFL  0x0800
#define LTS    0x0400
#define MAPEN  0x0200
#define MAPRS  0x0100
#define NFRAM  0x0080
#define VMRES  0x0040
#define AUTOI  0x0020
#define RSADR  0x0010
#define DIRQ   0x0010
#define RSFIF  0x0008
#define FF     0x0004
#define FNE    0x0002
#define RSDIRQ 0x0002
#define RXVIO  0x0001

// Output pulse enable.
// Address: 0x06.
#define OTP(x) (1U << x)
#define PDP(x) (1U << x)

int evr230_init(asynUser* device, const char* asyn_name, u32 frequency);
int evr230_get_firmware_version(asynUser* device, u16* value);
int evr230_enable(asynUser* device, u16 enable);
int evr230_is_enabled(asynUser* device, u16* enabled);
int evr230_is_rx_violation(asynUser* device, u16* value);
int evr230_reset_rx(asynUser* device);
int evr230_set_clock(asynUser* device, u16 clock);
int evr230_get_clock(asynUser* device, u16* clock);
int evr230_flush(asynUser* device);
int evr230_enable_otp(asynUser* device, u16 output, u16 enable);
int evr230_is_otp_enabled(asynUser* device, u16 output, u16* enabled);
int evr230_set_otp_delay(asynUser* device, u16 output, double  delay);
int evr230_get_otp_delay(asynUser* device, u16 output, double* delay);
int evr230_set_otp_width(asynUser* device, u16 output, double  width);
int evr230_get_otp_width(asynUser* device, u16 output, double* width);
int evr230_set_ttl_source(asynUser* device, u16 ttl, u16 source);
int evr230_get_ttl_source(asynUser* device, u16 ttl, u16* source);
int evr230_set_universal_source(asynUser* device, u16 univ, u16  source);
int evr230_get_universal_source(asynUser* device, u16 univ, u16* source);
int evr230_enable_pdp(asynUser* device, u16 output, u16 enable);
int evr230_is_pdp_enabled(asynUser* device, u16 output, u16* enabled);
int evr230_set_pdp_delay(asynUser* device, u16 output, double  delay);
int evr230_get_pdp_delay(asynUser* device, u16 output, double* delay);
int evr230_set_pdp_prescaler(asynUser* device, u16 output, u16  prescaler);
int evr230_get_pdp_prescaler(asynUser* device, u16 output, u16* prescaler);
int evr230_set_pdp_width(asynUser* device, u16 output, double  width);
int evr230_get_pdp_width(asynUser* device, u16 output, double* width);

int evr230_read(asynUser* device, int address, u16* data);
int evr230_write(asynUser* device, int address, u16 data);
