
#define BASE_ADDRESS      0x7A000000
#define OTP_SELECT_OFFSET 16

#define EVR230_CONTROL           0x00
#define EVR230_OTP_ENABLE        0x06
#define EVR230_PULSE_SELECT      0x1A
#define EVR230_FIRMWARE_VERSION  0x2E
#define EVR230_USEC_DIVIDER      0x4E
#define EVR230_PULSE_DELAY       0x6C
#define EVR230_PULSE_WIDTH       0x70
#define EVR230_FP_MAP(x)        (0x40 + x*2)

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

