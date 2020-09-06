/*
 * 결선참조 : http://blog.daum.net/rockjjy99/2723
 * DE/RE는 묶어서 아두이노8번과 연결, Data송신:High, Data수신:Low 
 * DI: TX->1, RO: RX<-0 
 * [Packet 설명] 
 * B0: 전등 Off 명령, B1: 전등 On 명령 
 * B2: 저등 Off 상태, B3: 전등 On 상태
 * 한시이클 소요 밀리초 : 170ms (읽기:70  쓰기:100, 시스템속도: 0)
 */

#include <SoftwareSerial.h>
#define SerialTxControl 8   //RS485 제어 접점 핀 arduino pin 8
#define RS485Transmit    HIGH
#define RS485Receive     LOW
#define DeviceID  "BB"  
char buff[3];            //수신된 문자 저장할 버퍼, 문자열 저장
char Statebuff[3];       //송신할 문자 저장할 버퍼, 문자열 저장
const int led = 9;
const int pushButton = 4;
int buttonState = 0;     //buttonState는 핀의 상태를 저장하는 데 사용됨
int old_buttonState = 0; //이 변수는 buttonState의 이전 값을 저장하는데 사용
int ledState = 0;        //0 == Off, 1 == On
int SendState = 0, SendState1 = 0; 

void setup(void) {
  Serial.begin(9600);
  pinMode(SerialTxControl, OUTPUT);  
  digitalWrite(SerialTxControl, RS485Transmit); 
  Serial.println("==== Arduino Uno Board ====");  
  Serial.println("==== Slave1 connected");  
  delay(100); 
  digitalWrite(SerialTxControl, RS485Receive);   
}

void Send_Statebuff(){
// Sending Packet for State-update -------------------------------------------
  digitalWrite(SerialTxControl, RS485Transmit);
  delay(5); 
  Serial.print(Statebuff); 
  delay(10);     //송신이 잘 안되면 Delay 값 조절
  digitalWrite(SerialTxControl, RS485Transmit);
  delay(5); 
  Serial.print(Statebuff); 
  delay(10);
  digitalWrite(SerialTxControl, RS485Receive);  
}

void Send_ACKPacket(byte i){
// Sending Packet to Network -------------------------------------------------
  //Serial.print(" : It is me, Slave1");
  digitalWrite(SerialTxControl, RS485Transmit); 
  delay(5);
  Serial.print(i, HEX);
  delay(50);     //짧으면 송신모드 비율이 길어져서 수신이 불안정함. 50 추천
  digitalWrite(SerialTxControl, RS485Receive);      
}

void Send_OnePacket(byte i){
// Sending only one Packet(off) to Network -----------------------------------
  SendState ++;
  if(SendState == 1){ 
    digitalWrite(SerialTxControl, RS485Transmit); 
    delay(5);
    Serial.print(i, HEX);
    delay(10);
    digitalWrite(SerialTxControl, RS485Receive);
  }    
}

void Send_OnePacket1(byte i){
// Sending only one Packet(on) to Network ------------------------------------
  SendState1 ++;
  if(SendState1 == 1){ 
    digitalWrite(SerialTxControl, RS485Transmit); 
    delay(5);
    Serial.print(i, HEX);
    delay(10);
    digitalWrite(SerialTxControl, RS485Receive);
  }    
}

void Save_Packet2buff(char i, char j){
// Saving Packet to buff -----------------------------------------------------
    Statebuff[0] = i;
    Statebuff[1] = j;
    Statebuff[2] = '\0'; 
}

void loop(void) {  
// Receive Packets ------------------------------------------------------------
  digitalWrite(SerialTxControl, RS485Receive);  
  int i = 0; 
  if(Serial.available()){
    delay(50);
    while( Serial.available() && i < sizeof(buff)-1){ 
      buff[i++] = Serial.read();
    } 
    buff[i++] = '\0';   //마지막에 null 문자 입력
  }
  if(i > 0){  
  Serial.print(buff);   //받은 패킷 출력
  
    // Analyse Packets --------------------------------------------------------- 
    if (!strcmp(buff, "B0")){ // 문자열 비교
      digitalWrite(led, LOW);
      ledState = 0;
      Send_ACKPacket(0xB0);         
    } else if (!strcmp(buff, "B1")){
      digitalWrite(led, HIGH);
      ledState = 1;
      Send_ACKPacket(0xB1);   
    } else if (!strcmp(buff, DeviceID)){
      Send_Statebuff();
    }
  }
      
// LED State 와 pushButton -----------------------------------------------------
  buttonState = digitalRead(pushButton);   //입력값을 읽고 저장

  //입력값이 HIGH인지 확인해서 state 값을 바꿈
  if((buttonState == HIGH) && (old_buttonState == LOW)){
    ledState = 1 - ledState;
    //Serial.println("\n Button Push");
    delay(10);    //debouncing 기능
  }

  //튕김방지(debouncing) 위한 추가 코드
  if((buttonState == LOW) && (old_buttonState == HIGH)){
    delay(10);
  }

  old_buttonState = buttonState;   //현재 loop에서의 buttonState 값을 저장 해 둔다

  if(ledState == 1){
    digitalWrite(led, HIGH);
    SendState = 0;
    Send_OnePacket1(0xB3); 
    Save_Packet2buff('B','3');     //State: On 저장  ---> B3 패킷으로
  }else{
    digitalWrite(led, LOW);
    SendState1 = 0;
    Send_OnePacket(0xB2); 
    Save_Packet2buff('B','2');     //State: Off 저장 ---> B2 패킷으로
  }
  
  //delay(50);  //System Speed
}  
  

  
