
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
    int status = readRegister(REGISTER_RF_CONTROL, &raw_data);
    *data = (raw_data & (TCSEL | ECSEL | BRSEL)) != 0;
    return status;
}

int EVG230Board::setRFSource(u16 source)
{
    u16 data;
    int status;

    status = readRegister(REGISTER_RF_CONTROL, &data);
    if(status != 0)
        return status;

    data   = source == RF_SOURCE_INTERNAL ? data & ~(TCSEL | ECSEL | BRSEL) : data | (TCSEL | ECSEL | BRSEL);
    status = writeRegister(REGISTER_RF_CONTROL, data);
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
