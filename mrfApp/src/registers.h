
#define BASE_ADDRESS 0x80000000

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

// Control/Status. 
// Address: 0x000
#define MSDIS  0x8000
#define FF     0x4000
#define RSFIFO 0x2000
#define DFIFO  0x1000
#define ERRLD  0x0800
#define VTRG1  0x0100
#define VTRG2  0x0080
#define RCYL1  0x0040
#define RCYL2  0x0020
#define SEQ1   0x0004
#define SEQ2   0x0002
#define RXVIO  0x0001

// Event Enable.
// Address: 0x002
#define DBUS   0x8000
#define SSEQ1  0x2000
#define SSEQ2  0x1000
#define CMODE  0x0800
#define ENEV7  0x0400
#define ENEV6  0x0200
#define ENEV5  0x0100
#define ENEV4  0x0080
#define ENEV3  0x0040
#define ENEV2  0x0020
#define ENEV1  0x0010
#define ENEV0  0x0008
#define ENSQ1  0x0004
#define ENSQ2  0x0002
#define ENVME  0x0001

// Multiplexed Counter Enable.
// Address: 0x01E
#define MXDB7  0x8000
#define MXDB6  0x4000
#define MXDB5  0x2000
#define MXDB4  0x1000
#define MXDB3  0x0800
#define MXDB2  0x0400
#define MXDB1  0x0200
#define MXDB0  0x0100
#define MXEV7  0x0080
#define MXEV6  0x0040
#define MXEV5  0x0020
#define MXEV4  0x0010
#define MXEV3  0x0008
#define MXEV2  0x0004
#define MXEV1  0x0002
#define MXEV0  0x0001

// AC Input Control.
// Address: 0x028
#define ACSQ2  0x8000
#define ACSQ1  0x4000
#define ACEV0  0x2000
#define ACSYNC 0x1000
#define ACBYP  0x0800
#define DLYSEL 0x0100
#define AC_DIV 0x00FF

// Multiplexed Counter Control.
// Address: 0x02A.
#define MXRS7  0x8000
#define MXRS6  0x4000
#define MXRS5  0x2000
#define MXRS4  0x1000
#define MXRS3  0x0800
#define MXRS2  0x0400
#define MXRS1  0x0200
#define MXRS0  0x0100
#define MXSQ2  0x0080
#define MXSQ1  0x0040
#define MXHSEL 0x0008
#define MXSEL2 0x0004
#define MXSEL1 0x0002
#define MXSEL0 0x0001

// RF Clock Select.
// Address: 0x040
#define TRRES  0x0800
#define TXPWD  0x0400
#define TXRES  0x0200
#define TCSEL  0x0100
#define ECSEL  0x0080
#define BRSEL  0x0040
#define RFSELX 0x003F
