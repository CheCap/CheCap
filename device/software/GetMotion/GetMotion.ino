#include "freeram.h"

#include "mpu.h"
#include "I2Cdev.h"

#include "Encode.h"

uint8_t led =13;



int ret;
void setup() {
    pinMode(led,OUTPUT);
    Wire.begin();
    Serial.begin(9600/*,SERIAL_8O1*/);
    ret = mympu_open(200);
	
}

unsigned int c = 0; //cumulative number of successful MPU/DMP reads
unsigned int np = 0; //cumulative number of MPU/DMP reads that brought no packet back
unsigned int err_c = 0; //cumulative number of MPU/DMP reads that brought corrupted packet
unsigned int err_o = 0; //cumulative number of MPU/DMP reads that had overflow bit set

void loop() {
  //uint16_t checksum=0;
  uint8_t plaindata[16];  //long型のクォータニオンを1バイトに分割したデータ
  uint8_t udata[33];       //送りたいデータ(↑)をエンコードした、最終的にUARTで送るデータ
  uint8_t numofdata;       //送信バイト数
  
  //ret = mympu_update();
  
  digitalWrite(led,HIGH);
  ret = lmympu_update();
  switch (ret) {
	case 0: c++; break;
	case 1: np++; //return;
	case 2: err_o++; //return;
	case 3: err_c++; //return; 
	//default: 
		//Serial.print("READ ERROR!  ");
		//Serial.println(ret);
    digitalWrite(led,LOW);
		//return;
  }
  
 for(uint8_t i=0; i<4; i++){
  longToByte(lmympu.lq[i],&plaindata[i*4]);
 }

  numofdata=encodeData(plaindata,16,udata);
  Serial.write(udata,numofdata);
  //delay(10);
}

