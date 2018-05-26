#ifndef ENCODE_H
#define ENCODE_H

#include <Arduino.h>

#define ESC ((uint8_t) 'E')
#define START ((uint8_t) 'S')

int8_t encodeData(uint8_t* plaindata, uint8_t size, uint8_t* endata); //endataの要素数はsize*2+1以上でなければならない 正常終了endataの要素数　異常終了-1
int8_t decodeData(uint8_t* endata, uint8_t size, uint8_t* dedata);    //正常終了dedataの要素数　異常終了　負数

void longToByte(int32_t input, uint8_t* output);  //longを分割(0:MSB,3:LSB)
void byteToLong(uint8_t* input, int32_t* output);  //Longにする(0:MSB,3:LSB)

#endif
