
#include <cstdint>

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef struct
{
	u8  access;    /*Read/Write*/
	u8  status;    /*Filled by device*/
	u16 data;      /*Register data*/
	u32 address;   /*Register address*/
	u32 reference; /*Reserved*/
} message_t;

#define ACCESS_READ   1
#define ACCESS_WRITE  2
#define PACKET_SIZE   12
#define IO_TIMEOUT    2

#define RF_SOURCE_INTERNAL 0
#define RF_SOURCE_EXTERNAL 1