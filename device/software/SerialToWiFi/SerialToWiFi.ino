#include <WiFi.h>
#include <WiFiUdp.h>
#include "Encode.h"

#define CONDEBUG    //つながれたLEDでPCとの接続をデバッグする
#define LED 23
#define UBUFF 18



const char ssid[] = "ESP32_wifi"; // SSID
const char pass[] = "esp32pass";  // password
const int localPort = 10000;      // ポート番号(pythonのポート番号に合わせる)
const int remotePort = 10001;

const IPAddress ip(192, 168, 200, 232);       // IPアドレス(pythonのIPアドレスに合わせる)
const IPAddress remoteIP(192,168,200,233);  //上のと連番じゃないとdhcp環境で動かない
const IPAddress subnet(255, 255, 255, 0); // サブネットマスク

WiFiUDP udp;

long quat[4];//UARTで受信したデータをデコードしてlong型のクォータニオンにしたデータ
uint8_t uartBuff[UBUFF];//UARTで受信したデータ
uint8_t pBuff=0;
uint8_t pflag =0;
int8_t numofdata=0;

void beginUDP(){
#ifdef CONDEBUG
  pinMode(LED,OUTPUT);  
#endif

  WiFi.softAP(ssid, pass);           // SSIDとパスの設定
  delay(100);                        // 追記：このdelayを入れないと失敗する場合がある
  WiFi.softAPConfig(ip, ip, subnet); // IPアドレス、ゲートウェイ、サブネットマスクの設定

  IPAddress myIP = WiFi.softAPIP();
  
  udp.begin(localPort);  // UDP通信の開始(引数はポート番号)
  
}

void transmitUDP(long* txdata, uint8_t sizeofdata){
  udp.beginPacket(remoteIP, remotePort);
  if(sizeofdata>0){
    for(uint8_t i=0; i<sizeofdata; i++)
      udp.write(txdata[i]);
  }else{
    udp.print("ERR");
    udp.print(numofdata);
  }
  udp.endPacket();
}

void checkUART(){
  uint8_t dedata[UBUFF];        //受信したデータをデコードしたデータ
  while(Serial.available() > 0){
    switch(pflag){
    case 0:
      uartBuff[0]=Serial.read();
      if(uartBuff[0] == START){
        pflag=1;
        pBuff=1;
      }
      break;
    case 1:
      uartBuff[pBuff] = Serial.read();
      if(uartBuff[pBuff] == START && uartBuff[pBuff-1] != ESC){
        numofdata=decodeData(uartBuff,pBuff,dedata);
        for(uint8_t i=0; i<4; i++)
          byteToLong(&dedata[i*4],&quat[i]);
        
        transmitUDP(quat,4);
        uartBuff[0] = START;
        pflag = 1;
        pBuff = 0;
      }
      pBuff++;
      if(pBuff>=UBUFF){
        pBuff = 0;
        pflag = 0;
      }
      break;
    }
  }

}

void SerialToWiFi(){
  checkUART();
  //while(Serial.available()>0) Serial.read();
  /*while(Serial.available() > 0){
    udpTx[0] = Serial.read();
    Serial.write(udpTx[0]);
  }*/

}


void setup() {
  delay(100);
  Serial.begin(9600/*,SERIAL_8O1*/);
  beginUDP();

}

void loop() {
  if(WiFi.softAPgetStationNum() > 0){
#ifdef CONDEBUG
    digitalWrite(LED,HIGH);  
#endif
 
    udp.beginPacket(remoteIP, remotePort);
    quat[0]=0;
    quat[1]=0;
    quat[2]=0;
    quat[3]=0;
    transmitUDP(quat,4);
    udp.endPacket();
    
    while(WiFi.softAPgetStationNum() > 0){
      SerialToWiFi();
    }
  }
#ifdef CONDEBUG
    digitalWrite(LED,LOW);  
#endif 
  
}
