/*
 * 결선참조 : http://blog.daum.net/rockjjy99/2723
 * DE/RE는 묶어서 아두이노8번과 연결, Data송신:High, Data수신:Low
 * DI: TX->1, RO: RX<-0 
 * Arduino Mega Board 
 * 한시이클 소요 밀리초 : 225ms (읽기:70  쓰기:155, 시스템속도: 0)
 * Slave 사이클밀리초 : Slave1 226ms,  Slave2 216ms
 */

#include <SoftwareSerial.h>
#define SerialTxControl 8   //RS485 제어 접점 핀 arduino pin 8
#define RS485Transmit    HIGH
#define RS485Receive     LOW
//#define DeviceID  "AA"  
byte DeviceID[4] = {0xAA, 0xBB, 0xCC};   //AA: Master(거실), BB: Slave(Room No.1), CC: Slave(Dining room)
char buff[3];            //수신된 문자 저장할 버퍼, 문자열 저장
char Statebuff[3];       //송신할 문자 저장할 버퍼, 문자열 저장
const int led_AA = 9;    //9:거실등
int IoTState[6] = {0,}; //0 == Off, 1 == On
const int pushButton[6] = {2, 3, 4, 5, 6, 7};   //2:거실등(토글), 3:방등(토글), 4:주방가스, 5:환기팬OnOff(토글), 6:환기팬속도(토글)
int buttonState[6] = {0,};    //buttonState는 핀의 상태를 저장하는 데 사용됨
int old_buttonState[6] = {0,}; //이 변수는 buttonState의 이전 값을 저장하는데 사용
int couNt = 1;    //Device를 순서메 맞게 송신모드로 정하기 위한 변수
int SendState[6] = {0,};
int SendState1[6] = {0,}; 
int Tizen_B1 = A0;    //아두이노 핀번호 거실등
int Tizen_B1_State, Tizen_B1_On = 0 ;
int Tizen_B2 = A1;    //아두이노 핀번호 안방등
int Tizen_B2_State, Tizen_B2_On = 0 ;
int Tizen_B3 = A2;    //아두이노 핀번호 주방가스밸브
int Tizen_B3_State, Tizen_B3_On = 0 ;
int Tizen_B4 = A3;    //아두이노 핀번호 Spare
int Tizen_B4_State, Tizen_B4_On = 0 ;


void setup(void) {
  Serial.begin(9600);
  pinMode(SerialTxControl, OUTPUT);  
  digitalWrite(SerialTxControl, RS485Transmit);
  Serial.println("==== Arduino Mega Board ====");  
  Serial.println("==== Slave0 connected");  
  delay(100); 
  digitalWrite(SerialTxControl, RS485Receive);  //Serial Receive Mode 

  pinMode(Tizen_B1, INPUT); 
  pinMode(Tizen_B2, INPUT); 
  pinMode(Tizen_B3, INPUT); 
  pinMode(Tizen_B4, INPUT); 
}

void PushBTN2IoT(int i){
// Operating IoT and Sending Packet to Network -------------------------------------------------
    buttonState[i] = digitalRead(pushButton[i]);   //입력값을 읽고 저장

  //입력값이 HIGH인지 확인해서 state 값을 바꿈
  if((buttonState[i] == HIGH) && (old_buttonState[i] == LOW)){
    IoTState[i] = 1 - IoTState[i];
    delay(10);    //debouncing 기능
  }

  //튕김방지(debouncing) 위한 추가 코드
  if((buttonState[i] == LOW) && (old_buttonState[i] == HIGH)){
    delay(10);
  }

  old_buttonState[i] = buttonState[i];   //현재 loop에서의 buttonState 값을 저장 해 둔다

  /*
  if(IoTState[i] == 1){
    digitalWrite(IoT[i], HIGH);
  }else{
    digitalWrite(IoT[i], LOW);
  }*/  
}

void Send_Statebuff(){
// Sending Packet for State-update -------------------------------------------
  digitalWrite(SerialTxControl, RS485Transmit); 
  delay(5);
  Serial.print(Statebuff); 
  delay(10);
  Serial.print(Statebuff); 
  delay(5);     //송신이 잘 안되면 Delay 값 조절
  digitalWrite(SerialTxControl, RS485Receive);  
}

void Send_Packet(byte i){
  digitalWrite(SerialTxControl, RS485Transmit); 
  delay(5);
  Serial.print(i, HEX);
  delay(10);     //짧으면 송신모드 비율이 길어져서 수신이 불안정함. 50 추천
  digitalWrite(SerialTxControl, RS485Receive);      
}

void Send_OnePacket(byte i, int j){   //i: Packet(HEX), j: 버튼리셋
// Sending only one Packet(off) to Network -----------------------------------
  SendState[j] ++;
  if(SendState[j] == 1){ 
    digitalWrite(SerialTxControl, RS485Transmit); 
    delay(5);    
    for(int z=0; z < 3; z++){
      Serial.print(i, HEX);
      delay(150);    //송신 패킷 간격
    }
  }
  digitalWrite(SerialTxControl, RS485Receive);    
}

void Send_OnePacket1(byte i, int j){   //i: Packet(HEX), j: 버튼번호
// Sending only one Packet(on) to Network ------------------------------------
  SendState1[j] ++;
  if(SendState1[j] == 1){ 
    digitalWrite(SerialTxControl, RS485Transmit); 
    delay(5);
    for(int z=0; z < 3; z++){
      Serial.print(i, HEX);
      delay(150);   //송신 패킷 간격
    }
  }
  digitalWrite(SerialTxControl, RS485Receive);    
}

void Save_Packet2buff(char i, char j){
// Saving Packet to buff -----------------------------------------------------
    Statebuff[0] = i;
    Statebuff[1] = j;
    Statebuff[2] = '\0'; 
}

void loop(void) {  
// Input from Tizen ------------------------------------------------------- 
//거실등
  Tizen_B1_State = analogRead(Tizen_B1);
  if((Tizen_B1_State > 500) and (Tizen_B1_On == 0)){
    IoTState[0] = 1;
    Tizen_B1_On = 1;   
  }else if((Tizen_B1_State < 500) and (Tizen_B1_On == 1)){
    IoTState[0] = 0;
    Tizen_B1_On = 0;     
  }

//안방등
  Tizen_B2_State = analogRead(Tizen_B2);
  if((Tizen_B2_State > 500) and (Tizen_B2_On == 0)){
    Send_Packet(0xB1);
    Tizen_B2_On = 1;       
  }else if((Tizen_B2_State < 500) and (Tizen_B2_On == 1)){
    Send_Packet(0xB0);
    Tizen_B2_On = 0; 
  }

//주방가스밸브
  Tizen_B3_State = analogRead(Tizen_B3);
  if((Tizen_B3_State > 500) and (Tizen_B3_On == 0)){
    Tizen_B3_On = 1;   //가스밸브 상태 Open 신호 
    Serial.println("========== 가스밸브 Open ==========");         
  }else if((Tizen_B3_State < 500) and (Tizen_B3_On == 1)){
    Tizen_B3_On = 0;
    
    digitalWrite(SerialTxControl, RS485Transmit); 
    delay(5);    
    for(int z=0; z < 5; z++){
      Serial.print(0xC0, HEX);
      delay(150); 
    }
    digitalWrite(SerialTxControl, RS485Receive); 

    Serial.println("========== 가스밸브 Close ==========");  
  }

//Spare
  Tizen_B4_State = analogRead(Tizen_B4);
  if((Tizen_B4_State > 500) and (Tizen_B4_On == 0)){
   // IoTState[3] = 1;
    Tizen_B4_On = 1;
    Serial.println("========== Test ON ==========");         
  }else if((Tizen_B4_State < 500) and (Tizen_B4_On == 1)){
   // IoTState[3] = 0;
    Tizen_B4_On = 0;
    Serial.println("========== Test OFF ==========");  
  }  
  
// RS485 Reading ----------------------------------------------------------  
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

    // Analyse Packets --------------------------------------------------------- 
    digitalWrite(SerialTxControl, RS485Receive);
    if (!strcmp(buff, "A0")){ // 문자열 비교
      digitalWrite(led_AA, LOW);
      IoTState[0] = 0;
      Send_Packet(0xA0); 
              
    } else if (!strcmp(buff, "A1")){
      digitalWrite(led_AA, HIGH);
      IoTState[0] = 1;
      Send_Packet(0xA1);  
       
    } else if (!strcmp(buff, "AA")){
      Send_Statebuff(); 
    }
  }
 

// Device AA (Slave 거실) -----------------------------------------------
  PushBTN2IoT(0);   //거실등
  if(IoTState[0] == 1){
  //if((IoTState[0] == 1)&& (buttonState[0] == HIGH)){
    digitalWrite(led_AA, HIGH);
    SendState[0] = 0;
    Send_OnePacket1(0xA3, 0);      //Send_OnePacket1(byte i, j){   //i: Packet(HEX), j: 버튼번호
    Save_Packet2buff('A','3');     //State: On 저장  ---> A3 패킷으로
  }else if(IoTState[0] == 0){
  //}else if((IoTState[0] == 0) && (buttonState[0] == HIGH)){  
    digitalWrite(led_AA, LOW);
    SendState1[0] = 0;
    Send_OnePacket(0xA2, 0);       //Send_OnePacket1(byte i, j){   //i: Packet(HEX), j: 버튼번호
    Save_Packet2buff('A','2');     //State: Off저장  ---> A2 패킷으로
  } 
  //Send_Statebuff();                //buff에 저장된 패킷을 네트워크 전송


// Device BB (Slave 방1) -------------------------------------------------
  PushBTN2IoT(1);   //방등
  if((IoTState[1] == 1)&& (buttonState[1] == HIGH)){
    SendState[1] = 0;
    Send_OnePacket1(0xB1, 1);      //Send_OnePacket1(byte i, j){   //i: Packet(HEX), j: 버튼번호
  }else if((IoTState[1] == 0) && (buttonState[1] == HIGH)) {
    SendState1[1] = 0;
    Send_OnePacket(0xB0, 1);       //Send_OnePacket1(byte i, j){   //i: Packet(HEX), j: 버튼번호   
  } 


// Device CC (Slave 주방) -------------------------------------------------
  buttonState[2] = digitalRead(pushButton[2]);   //입력값을 읽고 저장

  //튕김방지(debouncing) 위한 추가 코드
  if((buttonState[2] == LOW) && (old_buttonState[2] == HIGH)){
    delay(10);
  }
  old_buttonState[2] = buttonState[2];   //현재 loop에서의 buttonState 값을 저장 해 둔다
  
  if(buttonState[2] == 1){
    digitalWrite(SerialTxControl, RS485Transmit); 
    delay(5);    
    for(int z=0; z < 5; z++){
      Serial.print(0xC0, HEX);
      delay(150); 
    }
    digitalWrite(SerialTxControl, RS485Receive);  
  } 

  //delay(50);  //System Speed, Packet Display Speed - Slave로 부터 패킷 받는 시간 (최소 300: Slave0 236ms, Slave1 226ms, Slave2 216ms)    

}  

  
