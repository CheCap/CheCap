#include <WiFi.h>
#include <WiFiUdp.h>

#define CONDEBUG    //つながれたLEDでPCとの接続をデバッグする
#define LED 23




const char ssid[] = "ESP32_wifi"; // SSID
const char pass[] = "esp32pass";  // password
const int localPort = 10000;      // ポート番号(pythonのポート番号に合わせる)
const int remotePort = 10001;

const IPAddress ip(192, 168, 200, 232);       // IPアドレス(pythonのIPアドレスに合わせる)
const IPAddress remoteIP(192,168,200,233);  //上のと連番じゃないとdhcp環境で動かない
const IPAddress subnet(255, 255, 255, 0); // サブネットマスク

WiFiUDP udp;

uint8_t udpTx[255];
uint8_t uartBuff[18];//開始の0x00を除く終了の0x00までのデータを格納したい(4Byte*4+Checksum+0x00)
uint8_t pBuff=0;
uint16_t checksum=0;
long quat[4];

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

void checkUART(){
  while(Serial.available() > 0){
    uartBuff[pBuff]=Serial.read();
    checksum+=uartBuff[pBuff];
    if(pBuff>=17){  
      udp.beginPacket(remoteIP, remotePort);
      udpTx[0]=quat[0];
      udpTx[1]=quat[1];
      udpTx[2]=quat[2];
      udpTx[3]=quat[3];
      udp.write(uartBuff,18);//udp.print(Serial.available());
      udp.endPacket();
      pBuff=0;
      continue;
    }
    /*if(pBuff == 17){
      if(uartBuff[pBuff] == 0x00){      //終了バイト
        pBuff = 0;
        if(uartBuff[16] != (checksum&0xff)){   //誤り検出
          
          quat[0]='M';
          quat[1]='I';
          quat[2]='S';
          quat[3]='S';
          return;
        }else{
          for(uint8_t i=0; i<4; i++){
            quat[i] = (uartBuff[i*4]<<(8*3))|(uartBuff[i*4+1]<<(8*2))|(uartBuff[i*4+2]<<(8*1))|(uartBuff[i*4+3]);
          }
        }
      }else{                            //オーバーフロー
        pBuff = 0;
        checksum = 0;          
        quat[0]='O';
        quat[1]='V';
        quat[2]='E';
        quat[3]='R';
        return;
      }
    } else if(uartBuff[pBuff] == 0x00){ //開始バイト
      pBuff = 0;
      checksum = 0;
      uartBuff[pBuff] = 0x00;
      return;
    }*/
    pBuff++;
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
    udpTx[0]=0;
    udpTx[1]=0;
    udpTx[2]=0;
    udpTx[3]=0;
    udp.write(udpTx,4);
    udp.endPacket();
    
    while(WiFi.softAPgetStationNum() > 0){
      SerialToWiFi();
    }
  }
#ifdef CONDEBUG
    digitalWrite(LED,LOW);  
#endif 
  
}
