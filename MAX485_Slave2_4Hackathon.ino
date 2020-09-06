/*
 * 결선참조 : http://blog.daum.net/rockjjy99/2723
 * DE/RE는 묶어서 아두이노8번과 연결, Data송신:High, Data수신:Low 
 * DI: TX->1, RO: RX<-0 
 * [Packet 설명] 
 * C0: 가스 Close 명령 
 * C2: 가스 Close 상태, C3: 가스 Open 상태
 * 한시이클 소요 밀리초 : 160ms (읽기:60  쓰기:100, 시스템속도: 0)
 */

#include <SoftwareSerial.h>
#define SerialTxControl 8   //RS485 제어 접점 핀 arduino pin 8
#define RS485Transmit    HIGH
#define RS485Receive     LOW
#define DeviceID  "CC"  
char buff[3];            //수신된 문자 저장할 버퍼, 문자열 저장
char Statebuff[3];       //송신할 문자 저장할 버퍼, 문자열 저장
const int led_Close = 2, led_Open = 3;        //2: Close LED, 3: Open LED
const int pushButton[2] = {4, 5}; //4: Close 버튼,  5: Open 버튼
int buttonState[2] = {0,};     //buttonState는 핀의 상태를 저장하는 데 사용됨
int old_buttonState[2] = {0,};    //이 변수는 buttonState의 이전 값을 저장하는데 사용
int ledState[2] = {0,};           //0 == Close, 1 == Open
int SendState = 0, SendState1 = 0; 
int ledState_afterReboot = 0;    //Booting 후 가스밸브 잠근 상태로 시작 위한 변수

void setup(void) {
  Serial.begin(9600);
  pinMode(SerialTxControl, OUTPUT);  
  digitalWrite(SerialTxControl, RS485Transmit); 
  Serial.println("==== Arduino Uno Board ====");  
  Serial.println("==== Slave2 connected");  
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
  delay(10);     //송신이 잘 안되면 Delay 값 조절
  digitalWrite(SerialTxControl, RS485Receive);    
}

void Send_ACKPacket(byte i){
// Sending Packet to Network -------------------------------------------------
  //Serial.print(" : It is me, Slave2");
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
    delay(50);   //default 100
    while( Serial.available() && i < sizeof(buff)-1){ 
      buff[i++] = Serial.read();
    } 
    buff[i++] = '\0';   //마지막에 null 문자 입력
  }
  if(i > 0){  
  Serial.print(buff);   //받은 패킷 출력
  
    // Analyse Packets --------------------------------------------------------- 
    if (!strcmp(buff, "C0")){ // 가스밸브 Close
      digitalWrite(led_Close, HIGH);
      digitalWrite(led_Open, LOW);
      Save_Packet2buff('C','2');
      Send_ACKPacket(0xC0);         
    } else if (!strcmp(buff, DeviceID)){
      Send_Statebuff();
    }
  }
      
// LED State 와 pushButton -----------------------------------------------------

  for(int i = 0; i < 2; i++){
    buttonState[i] = digitalRead(pushButton[i]);   //입력값을 읽고 저장
    ledState[i] = buttonState[i];
  
    //튕김방지(debouncing) 위한 추가 코드
    if((buttonState[i] == LOW) && (old_buttonState[i] == HIGH)){
      delay(10);
    }
  
    old_buttonState[i] = buttonState[i];   //현재 loop에서의 buttonState 값을 저장 해 둔다
  }
    
  //Gas valve close after Device-reboot
  if(ledState_afterReboot == 0){
    ledState[0] = 1;
  }    
  ledState_afterReboot = 1;

  if(ledState[0] == 1){            //가스 Close
    digitalWrite(led_Close, HIGH);
    digitalWrite(led_Open, LOW);
    SendState = 0;
    Send_OnePacket1(0xC2); 
    Save_Packet2buff('C','2');     //State: Close 저장  ---> C2 패킷으로
  }else if(ledState[1] == 1){      //가스 Open
    digitalWrite(led_Close, LOW);
    digitalWrite(led_Open, HIGH);
    SendState1 = 0;
    Send_OnePacket(0xC3); 
    Save_Packet2buff('C','3');     //State: Open 저장 ---> C3 패킷으로
  }
    
  //delay(50);  //System Speed   
}  
  

  
