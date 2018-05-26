#include <OSCBoards.h>
#include <OSCBundle.h>
#include <OSCData.h>
#include <OSCMatch.h>
#include <OSCMessage.h>
#include <OSCTiming.h>
#include <SLIPEncodedSerial.h>
#include <SLIPEncodedUSBSerial.h>

#include <WiFi.h>
#include <WiFiUdp.h>
#include "Encode.h"


#define CONDEBUG    //つながれたLEDでPCとの接続をデバッグする
#define LED 23
#define UBUFF 18

HardwareSerial Serial2(2);


const char ssid[] = "ESP32_wifi"; // SSID
const char pass[] = "esp32pass";  // password
const int localPort = 10000;      // ポート番号(pythonのポート番号に合わせる)
const int remotePort = 10001;

const IPAddress ip(192, 168, 200, 232);       // IPアドレス(pythonのIPアドレスに合わせる)
const IPAddress remoteIP(192,168,200,233);  //上のと連番じゃないとdhcp環境で動かない
const IPAddress subnet(255, 255, 255, 0); // サブネットマスク

WiFiUDP udp;

int32_t quat[4];//UARTで受信したデータをデコードしてint32_t型のクォータニオンにしたデータ
float fquat[4];
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

void transmitUDP(/*int32_t*/uint8_t* txdata, uint8_t sizeofdata){
  udp.beginPacket(remoteIP, remotePort);
  if(sizeofdata>0){
    for(uint8_t i=0; i<sizeofdata; i++){
      udp.write(txdata[i]);
      Serial.print(txdata[i]);
      Serial.print(",");
    }
    Serial.println("");
  }else{
    udp.print("ERR");
    udp.print(numofdata);
  }
  udp.endPacket();
}

void SerialToWiFi(){
  OSCBundle HipBundle;
  
  uint8_t dedata[UBUFF];        //受信したデータをデコードしたデータ
  while(Serial2.available() > 0){
    switch(pflag){
    case 0:
      uartBuff[0]=Serial2.read();
      if(uartBuff[0] == START){
        pflag=1;
        pBuff=1;
      }
      break;
    case 1:
      uartBuff[pBuff] = Serial2.read();
      if(uartBuff[pBuff] == START && uartBuff[pBuff-1] != ESC){
        numofdata=decodeData(uartBuff,pBuff,dedata);
        for(uint8_t i=0; i<4; i++)
          byteToLong(&dedata[i*4],&quat[i]);
        for(uint8_t i=0; i<4; i++)
          fquat[i] = quat[i]/(1073741824.f);

        HipBundle.add("/hip").add(fquat[0]).add(fquat[1]).add(fquat[2]).add(fquat[3]);
        //transmitUDP(dedata,16);

        udp.beginPacket(remoteIP, remotePort);
        HipBundle.send(udp);
        udp.endPacket();
        HipBundle.empty();
        
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


void setup() {
  delay(100);
  Serial.begin(9600);
  Serial2.begin(115200/*,SERIAL_8O1*/);
  beginUDP();

}

void loop() {
  if(WiFi.softAPgetStationNum() > 0){
#ifdef CONDEBUG
    digitalWrite(LED,HIGH);  
#endif
 
    udp.beginPacket(remoteIP, remotePort);
    /*quat[0]=0;
    quat[1]=0;
    quat[2]=0;
    quat[3]=0;
    transmitUDP(quat,4);*/
    udp.endPacket();
    
    while(WiFi.softAPgetStationNum() > 0){
      SerialToWiFi();
    }
  }
#ifdef CONDEBUG
    digitalWrite(LED,LOW);  
#endif 
  
}
