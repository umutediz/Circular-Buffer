/*
 * CircularBuffer.c
 *
 *  Created on: 14 Ara 2019
 *      Author: Umut
 */

#include "CircularBuffer.h"

//#warning "Buffer size must be a power of two"

tBufferStatus readCircularBuffer(tCircularBuffer* cb, uint8_t *data){
	if (bufferLength(cb) == 0) { return BufferEmpty;}
	*data = cb->Buffer[cb->readPos];
	cb->readPos = ((cb->readPos + 1) & ( cb->size - 1));
	return BufferTrue;
}

tBufferStatus writeCircularBuffer(tCircularBuffer* cb, uint8_t data){
	if (bufferLength(cb) == (cb->size-1)) { return BufferFull;}
	cb->Buffer[cb->writePos] = data;
	cb->writePos = (cb->writePos + 1) & (cb->size - 1); // must be atomic
	return BufferTrue;
}

uint32_t bufferLength(tCircularBuffer* cb){
	return ((cb->writePos - cb->readPos)&(cb->size-1));
}

tCircularBuffer initBuffer(uint32_t size){
	tCircularBuffer cb;
	cb.size = size;
	cb.Buffer = (uint8_t*)malloc(sizeof(uint8_t) * cb.size);
	return cb;
}

void destroyBuffer(tCircularBuffer* cb){
	free(cb->Buffer);
}