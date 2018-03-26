#include "freeram.h"

#include "mpu.h"
#include "I2Cdev.h"

uint8_t led =13;



int ret;
void setup() {
    pinMode(led,OUTPUT);
//    Fastwire::setup(400,0);
    Wire.begin();
    Serial.begin(9600/*,SERIAL_8O1*/);
    ret = mympu_open(200);
    //Serial.print("MPU init: "); Serial.println(ret);
    //Serial.print("Free mem: "); Serial.println(freeRam());
	
}

unsigned int c = 0; //cumulative number of successful MPU/DMP reads
unsigned int np = 0; //cumulative number of MPU/DMP reads that brought no packet back
unsigned int err_c = 0; //cumulative number of MPU/DMP reads that brought corrupted packet
unsigned int err_o = 0; //cumulative number of MPU/DMP reads that had overflow bit set

void loop() {
  uint16_t checksum=0;
  
  //ret = mympu_update();
  digitalWrite(led,LOW);
  ret = lmympu_update();
  digitalWrite(led,HIGH);
  switch (ret) {
	case 0: c++; break;
	case 1: np++; //return;
	case 2: err_o++; //return;
	case 3: err_c++; //return; 
	//default: 
		//Serial.print("READ ERROR!  ");
		//Serial.println(ret);
    //digitalWrite(led,LOW);
		//return;
  }
  

  lmympu.lq[0]=0;
  lmympu.lq[1]=10;
  lmympu.lq[2]=100;
  lmympu.lq[3]=1000;  

  //Serial.write('A');
  Serial.write(0x00);
  for(uint8_t axis=0; axis<4; axis++){
    for(uint8_t digit=0; digit<4; digit++){
      uint8_t data = (uint8_t)(lmympu.lq[axis]&(0xff000000>>(8*digit)))>>(8*(3-digit));
      if(data==0x00)
        data = 4*axis+digit+1;
      Serial.write(data);
      checksum+=data;
    }
  }
  Serial.write((uint8_t)(checksum&0xff));
  Serial.write(0x00);
  /*if (!(c%25)) {
	  Serial.print(np); Serial.print("  "); Serial.print(err_c); Serial.print(" "); Serial.print(err_o);
	  Serial.print(" Y: "); Serial.print(mympu.ypr[0]);
	  Serial.print(" P: "); Serial.print(mympu.ypr[1]);
	  Serial.print(" R: "); Serial.print(mympu.ypr[2]);
	  Serial.print("\tgy: "); Serial.print(mympu.gyro[0]);
	  Serial.print(" gp: "); Serial.print(mympu.gyro[1]);
	  Serial.print(" gr: "); Serial.println(mympu.gyro[2]);
  }*/
  //
  delay(10);
}

