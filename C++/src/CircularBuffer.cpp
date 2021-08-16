#include "CircularBuffer.h"

uint32_t CircularBuffer::availableBytes() {
    return ((this->writePos - this->readPos) & (this->size - 1));
}

tBufferStatus CircularBuffer::write(uint8_t data) {
    if (this->availableBytes() == (this->size - 1)) {
        return BufferFull;
    }
    this->Buffer[this->writePos] = data;
    this->writePos = (this->writePos + 1) & (this->size - 1);  // must be atomic
    return BufferTrue;
};

tBufferStatus CircularBuffer::read(uint8_t *data) {
    if (this->availableBytes() == 0) {
        return BufferEmpty;
    }
    *data = this->Buffer[this->readPos];
    this->readPos = ((this->readPos + 1) & (this->size - 1));
    return BufferTrue;
};

uint16_t CircularBuffer::_get_pos(uint32_t index){
    return ((index) & (this->size - 1));
}