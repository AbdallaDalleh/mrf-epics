
#include "evr230.h"

static u32 device_frequency;

int evr230_init(asynUser* device, const char* asyn_name, u32 frequency)
{
	int status;
		
	status = evr230_enable(device, 1);
	if(status != 0) {
		printf("Could not enable device %s\n", asyn_name);
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

	printf("\nDevice %s initialized successfully.\n\n", asyn_name);
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
        return asynError;
    }
    return asynSuccess;
}


int evr230_enable(asynUser* device, u16 enable)
{
	u16 state = enable ? (EVREN | MAPEN) : 0;
	int status = evr230_write(device, EVR230_CONTROL, state);
	return status;
}

int evr230_is_enabled(asynUser* device, u16* enabled)
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
	int status;
	u16 data;

	status = evr230_read(device, EVR230_CONTROL, &data);
	if(status != asynSuccess)
		return 1;

	status = evr230_write(device, EVR230_CONTROL, data | NFRAM);
	return status;
}

int evr230_enable_otp(asynUser* device, u16 output, u16 enable)
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

int evr230_set_otp_delay(asynUser* device, u16 output, double delay)
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

	status = evr230_write(device, EVR230_PULSE_DELAY + 2, (u16)(cycles & 0xFFFF));
	return status;
}

int evr230_get_otp_delay(asynUser* device, u16 output, double* delay)
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

	cycles = data << 16;
	status = evr230_read(device, EVR230_PULSE_DELAY + 2, &data);
	if(status != asynSuccess)
		return -1;

	cycles |= data;
	*delay = cycles / (double) device_frequency;
	return status;
}

int evr230_set_otp_width(asynUser* device, u16 output, double width)
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

int evr230_get_otp_width(asynUser* device, u16 output, double* width)
{
	int status;
	u16 data;

	status = evr230_write(device, EVR230_PULSE_SELECT, output + OTP_SELECT_OFFSET);
	if(status != asynSuccess)
		return -1;

	status = evr230_read(device, EVR230_PULSE_WIDTH + 2, &data);
	if(status != asynSuccess)
		return -1;

	*width = data / (double) device_frequency;	
	return status;
}

int evr230_reset_rx(asynUser* device)
{
	int status;
	u16 data;

	status = evr230_read(device, EVR230_CONTROL, &data);
	if(status != asynSuccess)
		return 1;

	status = evr230_write(device, EVR230_CONTROL, data | RXVIO);
	return status;
}

int evr230_is_rx_violation(asynUser* device, u16* value)
{
	int status;
	u16 data;

	status = evr230_read(device, EVR230_CONTROL, &data);
	if(status != asynSuccess)
		return -1;

	*value = (data & RXVIO) == RXVIO;
	return status;
}

int evr230_get_firmware_version(asynUser* device, u16* value)
{
	int status;
	u16 data;

	status = evr230_read(device, EVR230_FIRMWARE_VERSION, &data);
	if(status != asynSuccess)
		return -1;

	*value = data;
	return status;
}

int evr230_set_ttl_source(asynUser* device, u16 ttl, u16 source)
{
    return evr230_write(device, EVR230_FP_TTL(ttl), source);
}

int evr230_get_ttl_source(asynUser* device, u16 ttl, u16* source)
{
    int status;
    u16 data;

    status = evr230_read(device, EVR230_FP_TTL(ttl), &data);
    if (status != asynSuccess)
        return -1;

    *source = data;
    return status;
}

int evr230_set_universal_source(asynUser* device, u16 univ, u16 source)
{
    return evr230_write(device, EVR230_FP_UNIV(univ), source);
}

int evr230_get_universal_source(asynUser* device, u16 univ, u16* source)
{
    int status;
    u16 data;

    status = evr230_read(device, EVR230_FP_UNIV(univ), &data);
    if (status != asynSuccess)
        return -1;

    *source = data;
    return status;
}

int evr230_enable_pdp(asynUser* device, u16 output, u16 enable)
{
    u16 data;
    int status;

    status = evr230_read(device, EVR230_PDP_ENABLE, &data);
    if(status == asynSuccess) {
        if(enable) 
            status = evr230_write(device, EVR230_PDP_ENABLE, data |  PDP(output));
        else
            status = evr230_write(device, EVR230_PDP_ENABLE, data & ~PDP(output));
	}

	return status;
}

int evr230_is_pdp_enabled(asynUser* device, u16 output, u16* enabled)
{
	u16 data;
	int status;

	status = evr230_read(device, EVR230_PDP_ENABLE, &data);
	if(status == asynSuccess)
		*enabled = (data & PDP(output)) == PDP(output);

	return status;
}

int evr230_set_pdp_delay(asynUser* device, u16 output, double delay)
{
	int status;
	u32 cycles;
    u16 prescaler;

    status = evr230_write(device, EVR230_PULSE_SELECT, output);
	if(status != asynSuccess)
		return -1;

    status = evr230_read(device, EVR230_PDP_PRESCALER, &prescaler);
    if (status != asynSuccess)
        return -1;

    cycles = delay * device_frequency / prescaler;
    status = evr230_write(device, EVR230_PULSE_DELAY, cycles >> 16);
    if(status != asynSuccess)
        return -1;

    status = evr230_write(device, EVR230_PULSE_DELAY + 2, (u16)(cycles & 0xFFFF));
    return status;
}

int evr230_get_pdp_delay(asynUser* device, u16 output, double* delay)
{
	int status;
	u16 data;
    u16 prescaler;
	u32 cycles;

	status = evr230_write(device, EVR230_PULSE_SELECT, output);
	if(status != asynSuccess)
		return -1;

    status = evr230_read(device, EVR230_PDP_PRESCALER, &prescaler);
    if (status != asynSuccess)
        return -1;

    status = evr230_read(device, EVR230_PULSE_DELAY, &data);
	if(status != asynSuccess)
		return -1;

	cycles = data << 16;
	status = evr230_read(device, EVR230_PULSE_DELAY + 2, &data);
	if(status != asynSuccess)
		return -1;

	cycles |= data;
	*delay = cycles * prescaler / (double) device_frequency;
	return status;
}

int evr230_set_pdp_prescaler(asynUser* device, u16 output, u16 prescaler)
{
    int status;

    status = evr230_write(device, EVR230_PULSE_SELECT, output);
    if(status != asynSuccess)
        return -1;

    status = evr230_write(device, EVR230_PDP_PRESCALER, prescaler);
    if (status != asynSuccess)
        return -1;

    return status;
}

int evr230_get_pdp_prescaler(asynUser* device, u16 output, u16* prescaler)
{
    int status;
    u16 data;

    status = evr230_write(device, EVR230_PULSE_SELECT, output);
    if(status != asynSuccess)
        return -1;

    status = evr230_read(device, EVR230_PDP_PRESCALER, &data);
    if (status != asynSuccess)
        return -1;

    *prescaler = data;
    return status;
}

int evr230_set_pdp_width(asynUser* device, u16 output, double width)
{
    int status;
    u32 cycles;
    u16 prescaler;

    status = evr230_write(device, EVR230_PULSE_SELECT, output);
    if(status != asynSuccess)
        return -1;

    status = evr230_read(device, EVR230_PDP_PRESCALER, &prescaler);
    if (status != asynSuccess)
        return -1;

    cycles = width * device_frequency / prescaler;
    status = evr230_write(device, EVR230_PULSE_WIDTH, cycles >> 16);
    if(status != asynSuccess)
        return -1;

    status = evr230_write(device, EVR230_PULSE_WIDTH + 2, (u16)(cycles & 0xFFFF));
    return status;
}

int evr230_get_pdp_width(asynUser* device, u16 output, double* width)
{
    int status;
    u16 data;
    u16 prescaler;
    u32 cycles;

    status = evr230_write(device, EVR230_PULSE_SELECT, output);
    if(status != asynSuccess)
        return -1;

    status = evr230_read(device, EVR230_PDP_PRESCALER, &prescaler);
    if (status != asynSuccess)
        return -1;

    status = evr230_read(device, EVR230_PULSE_WIDTH, &data);
    if(status != asynSuccess)
        return -1;

    cycles = data << 16;
    status = evr230_read(device, EVR230_PULSE_WIDTH + 2, &data);
    if(status != asynSuccess)
        return -1;

    cycles |= data;
    *width = cycles * prescaler / (double) device_frequency;
    return status;
}

int evr230_enable_cml(asynUser* device, u16 output, u16 enable)
{
    u16 value;

    value = CML_FREQUENCY_MODE + (enable ? CML_ENABLE : CML_DISABLE);
    return evr230_write(device, EVR230_CML_ENABLE(output), value);
}

int evr230_is_cml_enabled(asynUser* device, u16 output, u16* enabled)
{
    int status;
    u16 data;

    status = evr230_read(device, EVR230_CML_ENABLE(output), &data);
    if (status == asynSuccess)
        *enabled = data & CML_ENABLE;

    return status;
}

int evr230_set_cml_prescaler(asynUser* device, u16 output, u16 prescaler)
{
    int status;

    status = evr230_write(device, EVR230_CML_HP(output), prescaler / 2);
    if (status != asynSuccess)
        return -1;

    return evr230_write(device, EVR230_CML_HP(output), prescaler - (prescaler / 2));
}

int evr230_get_cml_prescaler(asynUser* device, u16 output, u16* prescaler)
{
    int status;
    u16 data;

    status = evr230_read(device, EVR230_CML_HP(output), &data);
    if (status != asynSuccess)
        return status;

    *prescaler = data;
    status = evr230_read(device, EVR230_CML_HP(output), &data);
    if (status != asynSuccess)
        return status;

    *prescaler += data;
    return status;
}

