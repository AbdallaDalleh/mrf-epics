
#include "EVR230Board.h"

static u32 device_frequency;

int evr230_init(asynUser* device, const char* asyn_name, u32 frequency)
{
    int status = pasynOctetSyncIO->connect(asyn_name, 1, &device, NULL);
	if(status != asynSuccess) {
		// TODO: Error reporting.
		printf("Could not connect to device %s\n", asyn_name);
		return -1;
	}

	status = evr230_set_clock(device, frequency);
	if(status != 0) {
		printf("Could not set device device_frequency.\n");
		return -1;
	}
	device_frequency = frequency;

	status = evr230_flush(device);
	if(status != 0) {
		printf("Could not flush device RAM.\n");
		return -1;
	}

	printf("Device initialized successfully.\n");
	return 0;
}

int evr230_read(asynUser* device, int address, u16* data)
{
    message_t message;
    char tx_buffer[PACKET_SIZE];
    char rx_buffer[PACKET_SIZE];
    int status;
    size_t tx_bytes;
    size_t rx_bytes;
    int reason;

    message.access    = ACCESS_READ;
    message.status    = 0;
    message.data      = 0x0000;
    message.address   = htonl(BASE_ADDRESS + address);
    message.reference = 0x00000000;
    memcpy(tx_buffer, (void*) &message, sizeof(tx_buffer));
    status = pasynOctetSyncIO->writeRead(device, tx_buffer, PACKET_SIZE, rx_buffer, PACKET_SIZE, IO_TIMEOUT, &tx_bytes, &rx_bytes, &reason);
    if(status != asynSuccess || tx_bytes != PACKET_SIZE || rx_bytes != PACKET_SIZE) {
        // TODO: Error reporting.
        return status;
    }

    memcpy(&message, rx_buffer, sizeof(message_t));
    if(ntohs(message.status) != 0) {
        // TODO: Error reporting.
        return status;
    }

    *data = ntohs(message.data);
    return asynSuccess;
}

int evr230_write(asynUser* device, int reg, u16 data)
{
    message_t message;
    char tx_buffer[PACKET_SIZE];
    char rx_buffer[PACKET_SIZE];
    int status;
    size_t tx_bytes;
    size_t rx_bytes;
    int reason;

    message.access    = ACCESS_WRITE;
    message.status    = 0;
    message.data      = htons(data);
    message.address   = htonl(BASE_ADDRESS + reg);
    message.reference = 0x00000000;

    memcpy(tx_buffer, (void*) &message, sizeof(tx_buffer));
    status = pasynOctetSyncIO->writeRead(device, tx_buffer, PACKET_SIZE, rx_buffer, PACKET_SIZE, IO_TIMEOUT, &tx_bytes, &rx_bytes, &reason);
    if(status != asynSuccess || tx_bytes != PACKET_SIZE || rx_bytes != PACKET_SIZE) {
        return status;
    }

    memcpy(&message, rx_buffer, sizeof(message_t));
    if(ntohs(message.status) != 0 /*|| ntohs(message.data) != data*/) {
        return status;
    }
    return asynSuccess;
}


int evr230_enable(asynUser* device, int enable)
{
	int state = enable ? (EVREN | MAPEN) : 0;
	int status = evr230_write(device, EVR230_CONTROL, state);
	return status;
}

int evr230_is_enabled(asynUser* device, int* enabled)
{
	u16 data;
	int status = evr230_read(device, EVR230_CONTROL, &data);
	if(status == asynSuccess)
		*enabled = (data & EVREN) == EVREN;

	return status;
}

int evr230_set_clock(asynUser* device, u16 clock)
{
	int status = evr230_write(device, EVR230_USEC_DIVIDER, clock);
	return status;
}

int evr230_get_clock(asynUser* device, u16* clock)
{
	u16 data;
	int status;

	status = evr230_read(device, EVR230_USEC_DIVIDER, &data);
	if(status == asynSuccess)
		*clock = data;

	return status;
}

int evr230_flush(asynUser* device)
{
	int status = evr230_write(device, EVR230_CONTROL, NFRAM);
	return status;
}

int evr230_enable_otp(asynUser* device, int output, int enable)
{
	u16 data;
	int status;
	
	status = evr230_read(device, EVR230_OTP_ENABLE, &data);
	if(status == asynSuccess) {
		if(enable) 
			status = evr230_write(device, EVR230_OTP_ENABLE, data |  OTP(output));
		else
			status = evr230_write(device, EVR230_OTP_ENABLE, data & ~OTP(output));
	}

	return status;
}


int evr230_is_otp_enabled(asynUser* device, u16 output, u16* enabled)
{
	u16 data;
	int status;

	status = evr230_read(device, EVR230_OTP_ENABLE, &data);
	if(status == asynSuccess)
		*enabled = (data & OTP(output)) == OTP(output);

	return status;
}

int evr230_set_otp_delay(asynUser* device, u16 output, float delay)
{
	int status;
	u32 cycles;

	cycles = delay * device_frequency;
	status = evr230_write(device, EVR230_PULSE_SELECT, output + OTP_SELECT_OFFSET);
	if(status != asynSuccess)
		return -1;

	status = evr230_write(device, EVR230_PULSE_DELAY, cycles >> 16);
	if(status != asynSuccess)
		return -1;

	status = evr230_write(device, EVR230_PULSE_DELAY + 2, (u16)(cycles & 0xFF));
	return status;
}

int evr230_get_otp_delay(asynUser* device, u16 output, float* delay)
{
	int status;
	u16 data;
	u32 cycles;

	status = evr230_write(device, EVR230_PULSE_SELECT, output + OTP_SELECT_OFFSET);
	if(status != asynSuccess)
		return -1;

	status = evr230_read(device, EVR230_PULSE_DELAY, &data);
	if(status != asynSuccess)
		return -1;

	cycles = (u32)(data) << 16;
	status = evr230_read(device, EVR230_PULSE_DELAY + 2, &data);
	if(status != asynSuccess)
		return -1;

	cycles |= data;
	*delay = cycles / (double) device_frequency;
	return status;
}

int evr230_set_otp_width(asynUser* device, u16 output, float width)
{
	int status;
	u16 cycles;

	cycles = width * device_frequency;
	status = evr230_write(device, EVR230_PULSE_SELECT, output + OTP_SELECT_OFFSET);
	if(status != asynSuccess)
		return -1;

	status = evr230_write(device, EVR230_PULSE_WIDTH + 2, cycles);
	return status;
}

int evr230_get_otp_width(asynUser* device, u16 output, float* width)
{
	int status;
	u16 data;

	status = evr230_write(device, EVR230_PULSE_SELECT, output + OTP_SELECT_OFFSET);
	if(status != asynSuccess)
		return -1;

	status = evr230_read(device, EVR230_PULSE_WIDTH, &data);
	if(status != asynSuccess)
		return -1;

	*width = data / (double) device_frequency;
	return status;
}

