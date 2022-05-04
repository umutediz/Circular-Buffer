#include "CircularBuffer.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unity.h"

#define BUFFER_SIZE 64
tCircularBuffer* cb;

void setUp(void) {
  cb = malloc(sizeof(tCircularBuffer));
  cb->Buffer = (uint8_t*)malloc(BUFFER_SIZE);
  memset((uint8_t*)cb->Buffer, 0, BUFFER_SIZE);
  cb->size = BUFFER_SIZE;
  cb->readPos = 0;
  cb->writePos = 0;
}

void tearDown(void) {
  free((uint8_t*)cb->Buffer);
  free((tCircularBuffer*)cb);
}

void test_cb_single_write() {
  writeCircularBuffer(cb, 0x55);
  TEST_ASSERT_EQUAL_UINT8(0x55, cb->Buffer[0]);
  TEST_ASSERT_EQUAL_UINT16(1, cb->writePos);
}

void test_cb_single_read() {
  cb->Buffer[0] = 0x55;
  cb->writePos = 1;

  uint8_t data;
  TEST_ASSERT_EQUAL_UINT16(0, cb->readPos);
  TEST_ASSERT_EQUAL_UINT16(1, bufferLength(cb));
  TEST_ASSERT(readCircularBuffer(cb, &data) == BufferTrue);
  TEST_ASSERT_EQUAL_UINT8(0x55, data);
  TEST_ASSERT_EQUAL_UINT16(1, cb->readPos);
}

void test_buffer_length() {
#define COUNT 5
  for (uint8_t i = 0; i < COUNT; i++) {
    writeCircularBuffer(cb, 0x55);
  }
  TEST_ASSERT_EQUAL_UINT16(COUNT, bufferLength(cb));
}

void test_buffer_length_overflow() {
#define COUNT 5

  for (uint8_t i = 0; i < BUFFER_SIZE - 1; i++) {
    writeCircularBuffer(cb, 0x55);
  }
  TEST_ASSERT_EQUAL_UINT16(BUFFER_SIZE - 1, bufferLength(cb));

  uint8_t data;
  for (uint8_t i = 0; i < COUNT * 2; i++) {
    readCircularBuffer(cb, &data);
  }

  for (uint8_t i = 0; i < COUNT; i++) {
    writeCircularBuffer(cb, 0x55);
  }

  TEST_ASSERT_EQUAL_UINT16(58, bufferLength(cb));
}

void test_cb_write_sequence() {
  uint8_t sequence[] = "This is a test string";
  TEST_ASSERT_EQUAL_UINT8(22, sizeof(sequence));
  for (uint8_t i = 0; i < sizeof(sequence); i++) {
    writeCircularBuffer(cb, sequence[i]);
  }

  TEST_ASSERT_EQUAL_UINT8(sizeof(sequence), cb->writePos);
  TEST_ASSERT_EQUAL_UINT8_ARRAY(sequence, cb->Buffer, sizeof(sequence));
}

void test_cb_read_sequence() {
  uint8_t actual_data[] = "This is a test string";
  memcpy((uint8_t*)cb->Buffer, actual_data, sizeof(actual_data));
  cb->writePos = sizeof(actual_data);

  TEST_ASSERT_EQUAL_UINT8_ARRAY(actual_data, cb->Buffer, sizeof(actual_data));
  TEST_ASSERT_EQUAL_UINT16(0, cb->readPos);
  uint8_t sequence[22];
  for (uint8_t i = 0; i < sizeof(sequence); i++) {
    readCircularBuffer(cb, sequence + i);
  }
  TEST_ASSERT_EQUAL_UINT16(sizeof(actual_data), cb->readPos);
  TEST_ASSERT_EQUAL_UINT8_ARRAY(actual_data, sequence, sizeof(actual_data));
}

void test_cb_write_sequence_overflow() {
  uint8_t sequence[BUFFER_SIZE];

  for (uint32_t i = 0; i < sizeof(sequence) - 1; i++) {
    sequence[i] = i;
    tBufferStatus status = writeCircularBuffer(cb, sequence[i]);
    uint8_t message[256];
    if (status != BufferTrue) {
      sprintf(message,
              "Write Position of circular buffer is %d at %dth iteration",
              cb->writePos, i);
      TEST_ASSERT_MESSAGE(status == BufferTrue, message);
    }
  }

  TEST_ASSERT_EQUAL_UINT16(cb->size - 1, cb->writePos);
  TEST_ASSERT(writeCircularBuffer(cb, 0x55) == BufferFull);
  TEST_ASSERT_EQUAL_UINT16(sizeof(sequence) - 1, cb->writePos);
  TEST_ASSERT_EQUAL_UINT8_ARRAY(sequence, cb->Buffer, sizeof(sequence));
}

void test_cb_chasing_read() {
  uint8_t data;
  tBufferStatus status;

  for (uint32_t i = 0; i < BUFFER_SIZE / 4; i++) {
    status = writeCircularBuffer(cb, 0x55);
    TEST_ASSERT(status == BufferTrue);
  }
  TEST_ASSERT_EQUAL_UINT16(BUFFER_SIZE / 4, cb->writePos);

  for (uint32_t i = 0; i < BUFFER_SIZE / 4; i++) {
    status = readCircularBuffer(cb, &data);
    TEST_ASSERT(status == BufferTrue);
  }

  status = readCircularBuffer(cb, &data);
  TEST_ASSERT(status == BufferEmpty);
}

void test_cb_chasing_read_overflow() {
  uint8_t data;
  tBufferStatus status;

  for (uint32_t i = 0; i < BUFFER_SIZE / 4; i++) {
    status = writeCircularBuffer(cb, 0x55);
    TEST_ASSERT(status == BufferTrue);
  }
  TEST_ASSERT_EQUAL_UINT16(BUFFER_SIZE / 4, cb->writePos);

  for (uint32_t i = 0; i < BUFFER_SIZE / 4; i++) {
    status = readCircularBuffer(cb, &data);
    TEST_ASSERT(status == BufferTrue);
  }

  for (uint32_t i = 0; i < BUFFER_SIZE - 1; i++) {
    status = writeCircularBuffer(cb, 0x55);
    TEST_ASSERT(status == BufferTrue);
  }
  TEST_ASSERT(cb->writePos < cb->readPos);  // make sure overflow occured
  TEST_ASSERT(bufferLength(cb) > 0);

  uint32_t read_count = bufferLength(cb);
  for (uint32_t i = 0; i < read_count; i++) {
    status = readCircularBuffer(cb, &data);
    TEST_ASSERT(status == BufferTrue);
  }

  TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, bufferLength(cb),
                                   "Buffer still contains data");
  TEST_ASSERT(readCircularBuffer(cb, &data) == BufferEmpty);
}

void test_cb_peek() {
  writeCircularBuffer(cb, 0x55);
  writeCircularBuffer(cb, 0xAA);

  uint8_t data;
  TEST_ASSERT_TRUE(peekCircularBuffer(cb, &data, 0) == BufferTrue);
  TEST_ASSERT(data == 0x55);

  TEST_ASSERT_TRUE(peekCircularBuffer(cb, &data, 1) == BufferTrue);
  TEST_ASSERT(data == 0xAA);

  TEST_ASSERT(readCircularBuffer(cb, &data) == BufferTrue);
  TEST_ASSERT(data == 0x55);

  TEST_ASSERT(readCircularBuffer(cb, &data) == BufferTrue);
  TEST_ASSERT(data == 0xAA);
}

void test_cb_block_write() {
  uint8_t test_string[] = "Block Test String";
  uint8_t result[sizeof(test_string)];

  TEST_ASSERT(writeCircularBufferBlock(cb, test_string, sizeof(test_string)) ==
              BufferTrue);

  for (int i = 0; i < sizeof(test_string); i++) {
    TEST_ASSERT(readCircularBuffer(cb, result + i) == BufferTrue);
  }

  TEST_ASSERT_EQUAL_UINT8_ARRAY(test_string, result, sizeof(test_string));
}

void test_cb_block_read() {
  // Force block read to rollover
  for (int i = 0; i < cb->size - 4; i++) {
    TEST_ASSERT(writeCircularBuffer(cb, 0xFF));
  }

  while (bufferLength(cb) > 0) {
    uint8_t dummy;
    TEST_ASSERT(readCircularBuffer(cb, &dummy));
  }
  uint8_t expected[] = "Block Test String";
  uint8_t buff[sizeof(expected)];

  for (int i = 0; i < sizeof(expected); i++) {
    writeCircularBuffer(cb, expected[i]);
  }

  TEST_ASSERT(readCircularBufferBlock(cb, buff, sizeof(expected)) ==
              BufferTrue);
  TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, buff, sizeof(expected));
}