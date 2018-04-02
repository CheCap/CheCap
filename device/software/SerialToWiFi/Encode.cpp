#include "Encode.h"

int8_t encodeData(uint8_t* plaindata, uint8_t size, uint8_t* endata){
    uint8_t pointer =0;

    endata[pointer] = START;
    pointer++;

    for(uint8_t i=0; i<size; i++){
      switch(plaindata[i]){
      case START:
        endata[pointer] = ESC;
        pointer++;
        endata[pointer] = START;
        break;
      case ESC:
        endata[pointer] = ESC;
        pointer++;
        endata[pointer] = ESC;
        break;
      default:
        endata[pointer] = plaindata[i];
      }
      pointer++;
    }

    return pointer;
}

int8_t decodeData(uint8_t* endata, uint8_t size, uint8_t* dedata){
  uint8_t epointer = 0;
  uint8_t dpointer = 0;

  if(endata[epointer]!=START)
    return -1;

  epointer++;

  while(epointer < size){
    switch (endata[epointer]) {
    case ESC:
      epointer++;
      if(endata[epointer] == ESC || endata[epointer] == START){
        dedata[dpointer] = endata[epointer];
      }else
        return -2;
      break;
    case START:
      return -3;
    default:
      dedata[dpointer] = endata[epointer];
    }
    epointer++;
    dpointer++;
  }

  return dpointer;
}


void longToByte(long input, uint8_t* output){
  output[0] = (uint8_t)((input&0xff000000)>>24);
  output[1] = (uint8_t)((input&0x00ff0000)>>16);
  output[2] = (uint8_t)((input&0x0000ff00)>>8);
  output[3] = (uint8_t)(input&0x000000ff);
}

void byteToLong(uint8_t* input, long* output){
  *output = (((long)input[0])<<24) | (((long)input[1])<<16) | (((long)input[2])<<8) | (((long)input[3]));
}
