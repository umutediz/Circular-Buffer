#include "src/CircularBuffer.h"
#include "/var/lib/gems/2.7.0/gems/ceedling-0.31.1/vendor/unity/src/unity.h"






tCircularBuffer* cb;



void setUp(void)

{

 cb = malloc(sizeof(tCircularBuffer));

 cb->Buffer = (uint8_t*) malloc(64);

 memset((uint8_t*)cb->Buffer, 0, 64);

 cb->size = 64;

 cb->readPos = 0;

 cb->writePos = 0;

}



void tearDown(void)

{

 free((uint8_t*)cb->Buffer);

 free((tCircularBuffer*)cb);

}



void test_cb_single_write(){

 writeCircularBuffer(cb, 0x55);

 UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((0x55)), (UNITY_INT)(UNITY_UINT8 )((cb->Buffer[0])), (

((void *)0)

), (UNITY_UINT)(28), UNITY_DISPLAY_STYLE_UINT8);

 UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((1)), (UNITY_INT)(UNITY_UINT16)((cb->writePos)), (

((void *)0)

), (UNITY_UINT)(29), UNITY_DISPLAY_STYLE_UINT16);

}



void test_cb_single_read(){

 cb->Buffer[0] = 0x55;

 cb->writePos=1;



 uint8_t data;

 UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((0)), (UNITY_INT)(UNITY_UINT16)((cb->readPos)), (

((void *)0)

), (UNITY_UINT)(37), UNITY_DISPLAY_STYLE_UINT16);

 UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((1)), (UNITY_INT)(UNITY_UINT16)((bufferLength(cb))), (

((void *)0)

), (UNITY_UINT)(38), UNITY_DISPLAY_STYLE_UINT16);

 do {if ((readCircularBuffer(cb, &data) == BufferTrue)) {} else {UnityFail( ((" Expression Evaluated To FALSE")), (UNITY_UINT)((UNITY_UINT)(39)));}} while(0);

 UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((0x55)), (UNITY_INT)(UNITY_UINT8 )((data)), (

((void *)0)

), (UNITY_UINT)(40), UNITY_DISPLAY_STYLE_UINT8);

 UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((1)), (UNITY_INT)(UNITY_UINT16)((cb->readPos)), (

((void *)0)

), (UNITY_UINT)(41), UNITY_DISPLAY_STYLE_UINT16);

}



void test_buffer_length(){



 for (uint8_t i = 0; i<5; i++){

  writeCircularBuffer(cb, 0x55);

 }

 UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((5)), (UNITY_INT)(UNITY_UINT16)((bufferLength(cb))), (

((void *)0)

), (UNITY_UINT)(49), UNITY_DISPLAY_STYLE_UINT16);

}



void test_buffer_length_overflow(){





 for (uint8_t i = 0; i<64 -1; i++){

  writeCircularBuffer(cb, 0x55);

 }

 UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((64 -1)), (UNITY_INT)(UNITY_UINT16)((bufferLength(cb))), (

((void *)0)

), (UNITY_UINT)(58), UNITY_DISPLAY_STYLE_UINT16);



 uint8_t data;

 for (uint8_t i = 0; i<5*2; i++){

  readCircularBuffer(cb, &data);

 }



 for (uint8_t i = 0; i<5; i++){

  writeCircularBuffer(cb, 0x55);

 }



 UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((58)), (UNITY_INT)(UNITY_UINT16)((bufferLength(cb))), (

((void *)0)

), (UNITY_UINT)(69), UNITY_DISPLAY_STYLE_UINT16);

}



void test_cb_write_sequence(){

 uint8_t sequence[] = "This is a test string";

 UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((22)), (UNITY_INT)(UNITY_UINT8 )((sizeof(sequence))), (

((void *)0)

), (UNITY_UINT)(74), UNITY_DISPLAY_STYLE_UINT8);

 for(uint8_t i=0; i<sizeof(sequence); i++){

  writeCircularBuffer(cb, sequence[i]);

 }



 UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT8 )((sizeof(sequence))), (UNITY_INT)(UNITY_UINT8 )((cb->writePos)), (

((void *)0)

), (UNITY_UINT)(79), UNITY_DISPLAY_STYLE_UINT8);

 UnityAssertEqualIntArray(( const void*)((sequence)), ( const void*)((cb->Buffer)), (UNITY_UINT32)((sizeof(sequence))), (

((void *)0)

), (UNITY_UINT)(80), UNITY_DISPLAY_STYLE_UINT8, UNITY_ARRAY_TO_ARRAY);

}



void test_cb_read_sequence(){

 uint8_t actual_data[] = "This is a test string";

 memcpy((uint8_t*)cb->Buffer, actual_data, sizeof(actual_data));

 cb->writePos=sizeof(actual_data);



 UnityAssertEqualIntArray(( const void*)((actual_data)), ( const void*)((cb->Buffer)), (UNITY_UINT32)((sizeof(actual_data))), (

((void *)0)

), (UNITY_UINT)(88), UNITY_DISPLAY_STYLE_UINT8, UNITY_ARRAY_TO_ARRAY);

 UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((0)), (UNITY_INT)(UNITY_UINT16)((cb->readPos)), (

((void *)0)

), (UNITY_UINT)(89), UNITY_DISPLAY_STYLE_UINT16);

 uint8_t sequence[22];

 for(uint8_t i=0; i<sizeof(sequence); i++){

  readCircularBuffer(cb, sequence+i);

 }

 UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((sizeof(actual_data))), (UNITY_INT)(UNITY_UINT16)((cb->readPos)), (

((void *)0)

), (UNITY_UINT)(94), UNITY_DISPLAY_STYLE_UINT16);

 UnityAssertEqualIntArray(( const void*)((actual_data)), ( const void*)((sequence)), (UNITY_UINT32)((sizeof(actual_data))), (

((void *)0)

), (UNITY_UINT)(95), UNITY_DISPLAY_STYLE_UINT8, UNITY_ARRAY_TO_ARRAY);

}



void test_cb_write_sequence_overflow(){

 uint8_t sequence[64];



 for(uint32_t i=0; i<sizeof(sequence)-1; i++){

  sequence[i] = i;

  tBufferStatus status = writeCircularBuffer(cb, sequence[i]);

  uint8_t message[256];

  if (status != BufferTrue){

   sprintf(message, "Write Position of circular buffer is %d at %dth iteration", cb->writePos, i);

   do {if ((status == BufferTrue)) {} else {UnityFail( (((message))), (UNITY_UINT)((UNITY_UINT)(107)));}} while(0);

  }

 }



 UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((cb->size-1)), (UNITY_INT)(UNITY_UINT16)((cb->writePos)), (

((void *)0)

), (UNITY_UINT)(111), UNITY_DISPLAY_STYLE_UINT16);

 do {if ((writeCircularBuffer(cb, 0x55) == BufferFull)) {} else {UnityFail( ((" Expression Evaluated To FALSE")), (UNITY_UINT)((UNITY_UINT)(112)));}} while(0);

 UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((sizeof(sequence)-1)), (UNITY_INT)(UNITY_UINT16)((cb->writePos)), (

((void *)0)

), (UNITY_UINT)(113), UNITY_DISPLAY_STYLE_UINT16);

 UnityAssertEqualIntArray(( const void*)((sequence)), ( const void*)((cb->Buffer)), (UNITY_UINT32)((sizeof(sequence))), (

((void *)0)

), (UNITY_UINT)(114), UNITY_DISPLAY_STYLE_UINT8, UNITY_ARRAY_TO_ARRAY);

}



void test_cb_chasing_read(){

 uint8_t data;

 tBufferStatus status;



 for(uint32_t i=0; i<64/4; i++){

  status = writeCircularBuffer(cb, 0x55);

  do {if ((status == BufferTrue)) {} else {UnityFail( ((" Expression Evaluated To FALSE")), (UNITY_UINT)((UNITY_UINT)(123)));}} while(0);

 }

 UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((64/4)), (UNITY_INT)(UNITY_UINT16)((cb->writePos)), (

((void *)0)

), (UNITY_UINT)(125), UNITY_DISPLAY_STYLE_UINT16);



 for(uint32_t i=0; i<64/4; i++){

  status = readCircularBuffer(cb, &data);

  do {if ((status == BufferTrue)) {} else {UnityFail( ((" Expression Evaluated To FALSE")), (UNITY_UINT)((UNITY_UINT)(129)));}} while(0);

 }



 status = readCircularBuffer(cb, &data);

 do {if ((status == BufferEmpty)) {} else {UnityFail( ((" Expression Evaluated To FALSE")), (UNITY_UINT)((UNITY_UINT)(133)));}} while(0);

}



void test_cb_chasing_read_overflow(){

 uint8_t data;

 tBufferStatus status;



 for(uint32_t i=0; i<64/4; i++){

  status = writeCircularBuffer(cb, 0x55);

  do {if ((status == BufferTrue)) {} else {UnityFail( ((" Expression Evaluated To FALSE")), (UNITY_UINT)((UNITY_UINT)(142)));}} while(0);

 }

 UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT16)((64/4)), (UNITY_INT)(UNITY_UINT16)((cb->writePos)), (

((void *)0)

), (UNITY_UINT)(144), UNITY_DISPLAY_STYLE_UINT16);



 for(uint32_t i=0; i<64/4; i++){

  status = readCircularBuffer(cb, &data);

  do {if ((status == BufferTrue)) {} else {UnityFail( ((" Expression Evaluated To FALSE")), (UNITY_UINT)((UNITY_UINT)(148)));}} while(0);

 }



 for(uint32_t i=0; i<64 -1; i++){

  status = writeCircularBuffer(cb, 0x55);

  do {if ((status == BufferTrue)) {} else {UnityFail( ((" Expression Evaluated To FALSE")), (UNITY_UINT)((UNITY_UINT)(153)));}} while(0);

 }

 do {if ((cb->writePos < cb->readPos)) {} else {UnityFail( ((" Expression Evaluated To FALSE")), (UNITY_UINT)((UNITY_UINT)(155)));}} while(0);

 do {if ((bufferLength(cb) > 0)) {} else {UnityFail( ((" Expression Evaluated To FALSE")), (UNITY_UINT)((UNITY_UINT)(156)));}} while(0);



 uint32_t read_count = bufferLength(cb);

 for (uint32_t i = 0; i < read_count; i++){

  status = readCircularBuffer(cb, &data);

  do {if ((status == BufferTrue)) {} else {UnityFail( ((" Expression Evaluated To FALSE")), (UNITY_UINT)((UNITY_UINT)(161)));}} while(0);

 }



 UnityAssertEqualNumber((UNITY_INT)(UNITY_UINT32)((0)), (UNITY_INT)(UNITY_UINT32)((bufferLength(cb))), (("Buffer still contains data")), (UNITY_UINT)(164), UNITY_DISPLAY_STYLE_UINT32);

 do {if ((readCircularBuffer(cb, &data) == BufferEmpty)) {} else {UnityFail( ((" Expression Evaluated To FALSE")), (UNITY_UINT)((UNITY_UINT)(165)));}} while(0);

}
