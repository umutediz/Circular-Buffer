/*
 * CircularBuffer.h
 *
 *  Created on: 14 Ara 2019
 *      Author: Umut Ediz
 */

#ifndef CIRCULARBUFFER_H_
#define CIRCULARBUFFER_H_

#include "stdint.h"
#include "stdlib.h"


#define __GET_POS(CB, X) ((X) & (CB.size - 1))
typedef struct circularBuffer {
	uint8_t *Buffer;
	uint32_t readPos;
	uint32_t writePos;
	uint32_t size;
}tCircularBuffer;

typedef enum {
 BufferFalse,
 BufferTrue,
 BufferEmpty,
 BufferFull,
 BufferAvailable

}tBufferStatus;

tBufferStatus readCircularBuffer(tCircularBuffer* cb, uint8_t *data);
tBufferStatus writeCircularBuffer(tCircularBuffer* cb, uint8_t data);
uint32_t bufferLength(tCircularBuffer* cb);
tCircularBuffer initBuffer(uint32_t size);
void destroyBuffer(tCircularBuffer* cb);

#endif /* CIRCULARBUFFER_H_ */
