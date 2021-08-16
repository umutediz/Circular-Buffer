#ifndef __CIRCULAR_BUFFER_H_
#define __CIRCULAR_BUFFER_H_

#include "stdint.h"

#define STATIC_BUFFER_SIZE 384

typedef enum {
 BufferFalse,
 BufferTrue,
 BufferEmpty,
 BufferFull,
 BufferAvailable

}tBufferStatus;

class CircularBuffer {
   public:
    tBufferStatus write(uint8_t data);
    tBufferStatus read(uint8_t *data);
    uint32_t availableBytes();

   private:
    uint16_t readPos;
    uint16_t writePos;
    uint32_t size;
    uint8_t Buffer[STATIC_BUFFER_SIZE];
    uint16_t _get_pos(uint32_t index);
};

#endif