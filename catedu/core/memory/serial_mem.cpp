#include "serial_mem.hpp"
#include <algorithm>
#include <assert.h>
#include <string.h>

static size_t remaining_memory(SerialMem *serial)
{
    return serial->buffer.size - serial->at;
}


size_t SerialMem::write_buffer(Buffer buf)
{
#ifdef _DEBUG
    assert(this->filter_ == Any || this->filter_ == Write);
#endif

    const size_t bytes_to_write = std::min(buf.size, remaining_memory(this));

    memcpy((uint8_t*)this->buffer.data + this->at, buf.data, bytes_to_write);

    this->at += bytes_to_write;

    return buf.size - bytes_to_write;
}


size_t SerialMem::read_buffer(Buffer buf)
{
#ifdef _DEBUG
    assert(this->filter_ == Any || this->filter_ == Read);
#endif

    const size_t bytes_to_read = std::min(buf.size, remaining_memory(this));

    memcpy(buf.data, (uint8_t*)this->buffer.data + this->at, bytes_to_read);

    this->at += bytes_to_read;

    return buf.size - bytes_to_read;
}

void *SerialMem::pointer()
{
    return ((uint8_t*)this->buffer.data + this->at);
}
