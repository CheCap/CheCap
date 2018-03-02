#include <WiFi.h>
#include <WiFiUdp.h>

const char ssid[] = "ESP32_wifi"; // SSID
const char pass[] = "esp32pass";  // password
const int localPort = 10000;      // ポート番号(pythonのポート番号に合わせる)
const int remotePort = 10001;

const IPAddress ip(192, 168, 200, 232);       // IPアドレス(pythonのIPアドレスに合わせる)
const IPAddress remoteIP(192,168,200,233);  //上のと連番じゃないとdhcp環境で動かない
const IPAddress subnet(255, 255, 255, 0); // サブネットマスク

uint8_t udpTx[255];

WiFiUDP udp;

uint16_t c=0;

void setup() {
  Serial.begin(115200);

  WiFi.softAP(ssid, pass);           // SSIDとパスの設定
  delay(100);                        // 追記：このdelayを入れないと失敗する場合がある
  WiFi.softAPConfig(ip, ip, subnet); // IPアドレス、ゲートウェイ、サブネットマスクの設定

  Serial.print("AP IP address: ");
  IPAddress myIP = WiFi.softAPIP();
  Serial.println(myIP);

  Serial.println("Starting UDP");
  udp.begin(localPort);  // UDP通信の開始(引数はポート番号)

  Serial.print("Local port: ");
  Serial.println(localPort);
}

void loop() {
  if (udp.parsePacket()) {
    char c = udp.read();
    Serial.println(c); // UDP通信で来た値を表示
  }
  if(c<36000){
    udp.beginPacket(remoteIP, remotePort);
    udpTx[0]=c/100;
    udpTx[1]=c/10;
    udpTx[2]=c/2;
    udp.write(udpTx,3);
    udp.endPacket();
  }else{
    c=0;
  }
  c++;
  delay(1);
}
