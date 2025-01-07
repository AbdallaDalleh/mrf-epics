
#include "EVG230Board.h"

EVG230Board::EVG230Board(string asyn_name, int frequency)
{
    int status = pasynOctetSyncIO->connect(asyn_name.c_str(), 1, &this->device, NULL);
    if(status != asynSuccess) {
        // TODO: Error reporting.
        cout << device->errorMessage << endl;
        this->error = -1;
        return;
    }

    this->error = 0;
    this->frequency = frequency;
	enable();
	writeRegister(REGISTER_USEC_DIVIDER, this->frequency);
}

int EVG230Board::enable()
{
    int status;

    status = writeRegister(REGISTER_CONTROL, ~MSDIS & (FF | RSFIFO | DFIFO | RXVIO));
    return status;    
}

int EVG230Board::disable()
{
    int status = writeRegister(REGISTER_CONTROL, MSDIS | FF | RSFIFO | DFIFO | RXVIO);
    return status;    
}

int EVG230Board::isEnabled(u16* value)
{
    u16 raw_data;
    int status = readRegister(REGISTER_CONTROL, &raw_data);
    if(status == 0)
        *value = (raw_data & MSDIS) == 0;
    return status;
}

int EVG230Board::readFirmware(u16* data)
{
    return readRegister(REGISTER_FIRMWARE, data);
}

int EVG230Board::readClock(u16* data)
{
    return readRegister(REGISTER_USEC_DIVIDER, data);
}

int EVG230Board::readRFSource(u16* data)
{
    u16 raw_data;
    int status = readRegister(REGISTER_RF_CONTROL, &raw_data);
    *data = (raw_data & (TCSEL | ECSEL | BRSEL)) != 0;
    return status;
}

int EVG230Board::setRFSource(u16 source)
{
    u16 data;
    int status;

    status = readRegister(REGISTER_RF_CONTROL, &data);
    if(status == 0) {
        data   = source == RF_SOURCE_INTERNAL ? data & ~(TCSEL | ECSEL | BRSEL) : data | (TCSEL | ECSEL | BRSEL);
        status = writeRegister(REGISTER_RF_CONTROL, data);
    }
    return status;
}

int EVG230Board::readACSyncSource(u16* source)
{
    u16 raw_data;
    int status = readRegister(REGISTER_AC_ENABLE, &raw_data);
    *source = (raw_data & ACSYNC) != 0;
    return status;
}

int EVG230Board::setACSyncSource(u16 source)
{
    u16 raw_data;
    u16 data;
    int status = readRegister(REGISTER_AC_ENABLE, &raw_data);
    if(status == 0) {
        data = source == AC_SOURCE_EVENT ? data & ~ACSYNC : data | ACSYNC;
        status = writeRegister(REGISTER_AC_ENABLE, data);
    }

    return status;
}

int EVG230Board::readRFPrescaler(u16* data)
{
    int status = readRegister(REGISTER_RF_CONTROL, data);
    if(status == 0)
        *data = (*data & RFSELX) + 1;
    return status;
}

int EVG230Board::setRFPrescaler(u16 data)
{
    u16 raw_data;
    int status = readRegister(REGISTER_RF_CONTROL, &raw_data);
    if(status == 0)
        status = writeRegister(REGISTER_RF_CONTROL, (raw_data & ~RFSELX) | (data - 1));
    return status;
}

int EVG230Board::readACPrescaler(u16* data)
{
    u16 raw_data;
    int status = readRegister(REGISTER_AC_ENABLE, &raw_data);
    if(status == 0) {
        *data = (raw_data & AC_DIV);
    }
    return status;
}

int EVG230Board::setACPrescaler(u16 data)
{
    u16 raw_data;
    int status = readRegister(REGISTER_AC_ENABLE, &raw_data);
    if(status == 0) {
        raw_data &= ~AC_DIV;
        status = writeRegister(REGISTER_AC_ENABLE, raw_data | data);
    }
    return status;
}

int EVG230Board::readMXCPrescaler(int counter, u32* data)
{
    u16 raw_data;
    u32 value;
    int status = writeRegister(REGISTER_MXC_CONTROL, counter | MXHSEL);
    status    |= readRegister(REGISTER_MXC_PRESCALER, &raw_data);
    if(status == 0) {
        value   = raw_data;
        value <<= 16;
        status  = writeRegister(REGISTER_MXC_CONTROL, counter);
        status |= readRegister(REGISTER_MXC_PRESCALER, &raw_data);
        if(status == 0)
            value |= raw_data;
    }

    *data = value;
    return status;
}

int EVG230Board::setMXCPrescaler(int counter, u32 data)
{
    u16 raw_data;
    u32 value;
    int status = writeRegister(REGISTER_MXC_CONTROL, counter | MXHSEL);
    status    |= writeRegister(REGISTER_MXC_PRESCALER, (u16)(data >> 16));
    if(status == 0) {
        status  = writeRegister(REGISTER_MXC_CONTROL, counter);
        status |= writeRegister(REGISTER_MXC_PRESCALER, (u16)(data & 0xFFFF));
    }

    return status;
}

int EVG230Board::enableSequencer(int seq)
{
    u16 raw_data;
    int status = readRegister(REGISTER_EVENT_ENABLE, &raw_data);
    if(status == 0)
        status = writeRegister(REGISTER_EVENT_ENABLE, raw_data | (seq ? ENSQ2 : ENSQ1));
    return status;
}

int EVG230Board::disableSequencer(int seq)
{
    u16 raw_data;
    int status = readRegister(REGISTER_EVENT_ENABLE, &raw_data);
    if(status == 0)
        status = writeRegister(REGISTER_EVENT_ENABLE, raw_data & (seq ? ~ENSQ2 : ~ENSQ1));
    return status;
}

int EVG230Board::isSequencerEnabled(int seq, u16* data)
{
    u16 raw_data;
    int status = readRegister(REGISTER_EVENT_ENABLE, &raw_data);
    if(status == 0)
        *data = (raw_data & (seq ? ENSQ2 : ENSQ1)) != 0;
    return status;
}

int EVG230Board::readSequencerTriggerSource(int seq, u16* data)
{
    u16 raw_data;
    int status = readRegister(REGISTER_AC_ENABLE, &raw_data);
    if(status == 0)
        *data = (raw_data & (seq ? ACSQ2 : ACSQ1)) != 0;
    return status;
}

int EVG230Board::setSequencerTriggerSource(int seq, u16 data)
{
    u16 raw_data;
    u16 raw_data2;
    int status;
    status  = readRegister(REGISTER_AC_ENABLE, &raw_data);
    status |= readRegister(REGISTER_EVENT_ENABLE, &raw_data2);
    if(status == 0) {
        u16 s = seq ? ACSQ2 : ACSQ1;
        raw_data  = data ? raw_data | s : raw_data & ~s;
        raw_data2 = data ? raw_data2 & ~ENVME : raw_data2 | ENVME;
        status  = writeRegister(REGISTER_EVENT_ENABLE, raw_data2);
        status |= writeRegister(REGISTER_AC_ENABLE, raw_data);
    }
    return status;
}

int EVG230Board::readSequencerPrescaler(int seq, u16* data)
{
    return readRegister(seq ? REGISTER_SEQ_CLOCK_SEL2 : REGISTER_SEQ_CLOCK_SEL1, data);
}

int EVG230Board::setSequencerPrescaler(int seq, u16 data)
{
    return writeRegister(seq ? REGISTER_SEQ_CLOCK_SEL2 : REGISTER_SEQ_CLOCK_SEL1, data);
}

int EVG230Board::readSequencerEvent(int seq, int address, u16* data)
{
    u16 raw_data;
    int status = writeRegister(seq ? REGISTER_SEQ_ADDRESS1 : REGISTER_SEQ_ADDRESS0, address);
    if(status == 0) {
        status = readRegister(seq ? REGISTER_SEQ_CODE1 : REGISTER_SEQ_CODE0, &raw_data);
        *data = raw_data & 0x00FF;
    }

    return status;
}

int EVG230Board::setSequencerEvent(int seq, int address, u16 data)
{
    int status = writeRegister(seq ? REGISTER_SEQ_ADDRESS1 : REGISTER_SEQ_ADDRESS0, address);
    if(status == 0) {
        status = writeRegister(seq ? REGISTER_SEQ_CODE1 : REGISTER_SEQ_CODE0, data & 0xFF);
    }

    return status;
}

int EVG230Board::readSequencerEventTime(int seq, int address, u32* data)
{
    u16 raw_data;
    int status = writeRegister(seq ? REGISTER_SEQ_ADDRESS1 : REGISTER_SEQ_ADDRESS0, address);
    if(status == 0) {
        status = readRegister(seq ? REGISTER_SEQ_TIME1 : REGISTER_SEQ_TIME0, &raw_data);
        *data = ((u32) raw_data) << 16;
        status |= readRegister(seq ? REGISTER_SEQ_TIME1 + 2: REGISTER_SEQ_TIME0 + 2, &raw_data);
        if(status == 0)
            *data |= raw_data;
    }

    return status;
}

int EVG230Board::setSequencerEventTime(int seq, int address, u32 data)
{
    u16 addr = seq ? REGISTER_SEQ_ADDRESS1 : REGISTER_SEQ_ADDRESS0;
    u16 time = seq ? REGISTER_SEQ_TIME1    : REGISTER_SEQ_TIME0;
    int status = writeRegister(addr, address);
    if(status == 0) {
        status  = writeRegister(time,     (u16)(data >> 16));
        status |= writeRegister(time + 2, (u16)(data & 0xFFFF));
    }

    return status;
}

int EVG230Board::triggerSequencer(int seq)
{
	u16 control;
	int status;

	status = readRegister(REGISTER_CONTROL, &control);
	if(status == 0) {
		status = writeRegister(REGISTER_CONTROL, control | (seq == 0 ? VTRG1 : VTRG2));
	}

	return status;
}

int EVG230Board::writeRegister(int reg, u16 data)
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
    status = pasynOctetSyncIO->writeRead(this->device, tx_buffer, PACKET_SIZE, rx_buffer, PACKET_SIZE, IO_TIMEOUT, &tx_bytes, &rx_bytes, &reason);
    if(status != asynSuccess || tx_bytes != PACKET_SIZE || rx_bytes != PACKET_SIZE) {
        // TODO: Error reporting.
        this->errorMessage = "Could not write to register";
        this->error = status;
        return status;
    }

    memcpy(&message, rx_buffer, sizeof(message_t));
    if(ntohs(message.status) != 0 /*|| ntohs(message.data) != data*/) {
        // TODO: Error reporting.
        this->errorMessage = "Device status error or data mismatch";
        this->error = status;
        return status;
    }

    this->error = 0;
    return asynSuccess;
}

int EVG230Board::readRegister(int reg, u16* data)
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
    message.address   = htonl(BASE_ADDRESS + reg);
    message.reference = 0x00000000;
    memcpy(tx_buffer, (void*) &message, sizeof(tx_buffer));
    status = pasynOctetSyncIO->writeRead(this->device, tx_buffer, PACKET_SIZE, rx_buffer, PACKET_SIZE, IO_TIMEOUT, &tx_bytes, &rx_bytes, &reason);
    if(status != asynSuccess || tx_bytes != PACKET_SIZE || rx_bytes != PACKET_SIZE) {
        // TODO: Error reporting.
        this->errorMessage = "Could not read register";
        this->error = status;
        return status;
    }

    memcpy(&message, rx_buffer, sizeof(message_t));
    if(ntohs(message.status) != 0) {
        // TODO: Error reporting.
        this->errorMessage = "Device status error";
        this->error = status;
        return status;
    }

    *data = ntohs(message.data);
    this->error = 0;
    return asynSuccess;
}
