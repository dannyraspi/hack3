/*
 * 결선참조 : http://blog.daum.net/rockjjy99/2723
 * DE/RE는 묶어서 아두이노8번과 연결, Data송신:High, Data수신:Low
 * DI: TX->1, RO: RX<-0 
 * Arduino Nano Board 
 * Slave 사이클밀리초 : Slave0 255ms, Slave1 170ms, Slave2 160ms
 */

#include <SoftwareSerial.h>
#define SerialTxControl 8   //RS485 제어 접점 핀 arduino pin 8
#define RS485Transmit    HIGH
#define RS485Receive     LOW

int state = 0;
char buff[3];            //수신된 문자 저장할 버퍼, 문자열 저장
boolean j[3] = {true, true, true};
unsigned long previousMillis = 0;

void setup(void) {
  Serial.begin(9600);
  pinMode(SerialTxControl, OUTPUT);  
  digitalWrite(SerialTxControl, RS485Transmit);
  Serial.println("==== Arduino Nano Board ====");    
  Serial.println("==== Master connected");    
  delay(100); 
  digitalWrite(SerialTxControl, RS485Receive);  //Serial Receive Mode 
}

void sendPacket(byte i){
    digitalWrite(SerialTxControl, RS485Transmit);  //Serial Send Mode
    delay(5);
    Serial.println(i, HEX);
    delay(5);  //System Speed, Packet Display Speed
    digitalWrite(SerialTxControl, RS485Receive);  //Serial Receive Mode    
}

void loop(void) {  
  digitalWrite(SerialTxControl, RS485Receive);  //Serial Receive Mode  

  int i = 0; 
  if(Serial.available()){
    delay(50);
    while( Serial.available() && i < (sizeof(buff)-1)){ 
      buff[i++] = Serial.read();
    } 
    buff[i++] = '\0';
  }
  if(i>0){  
  Serial.print(buff); // 패킷 출력
  }

//타이머 설정 ------------------------------------------------------------------
  unsigned long currentMillis = millis();
  
  if(currentMillis - previousMillis >= 300){
    previousMillis = currentMillis;
    state ++;    
  }

// Slave State Update ---------------------------------------------------------
  if ((state == 0) && (j[0] == true)){                  //AA   
    sendPacket(0xAA);
    j[0] = false;
    j[1] = true;
    j[2] = true;

  }else if((state == 1) && (j[1] == true)){             //BB
    sendPacket(0xBB);
    j[0] = true;
    j[1] = false;
    j[2] = true; 

  }else if((state == 2) && (j[2] == true)){             //CC
    sendPacket(0xCC);
    j[0] = true;
    j[1] = true;
    j[2] = false; 

  }else if(state >= 3){
    state = 0;
    j[0] = true;
    j[1] = true;
    j[2] = true; 
  }

  //delay(300);  //System Speed, Packet Display Speed - Slave로 부터 패킷 받는 시간 (최소 260: Slave0 255ms, Slave1 170ms, Slave2 160ms)    
}  
