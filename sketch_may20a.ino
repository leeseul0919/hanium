#include <Wire.h>
#include <SoftwareSerial.h>
byte result_P[21];
int num=0;
int n1=0;
int car_direction=2;
// 설정한 I2C 주소
const byte i2cAddress = 8;
int st1=0;
#define echo 4  //초음파 에코 핀번호
#define trig 5  //초음파 트리거 핀번호

const int enaPin = 6;
const int in1Pin = 7;
const int in2Pin = 8;
const int enbPin = 11;
const int in3Pin = 9;
const int in4Pin = 10;
SoftwareSerial mySerial(2,3);

int st=0;
int start_node=2;
int end_node=8;

float cycletime;
float distance;

byte node_location[21][2]={{0,  0}, {1,  0}, {7,  0}, {8,  0}, {9,  0}, {15,  0}, {16,   0},
                               {0,  4}, {1,  4}, {7,  4}, {8,  4}, {9,  4}, {15,  4}, {16,   4},
                               {0,7}, {1,7}, {7,7}, {8,7}, {9,7}, {15,7}, {16, 7}};

void setup() {
  Wire.begin(i2cAddress); // I2C 통신을 설정하고 주소를 할당합니다.
  Wire.onReceive(receiveData); // 데이터를 수신할 때 호출할 함수를 등록합니다.
  Serial.begin(9600); // 시리얼 통신을 초기화합니다.
  pinMode(enaPin, OUTPUT);
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(enbPin, OUTPUT);
  pinMode(in3Pin, OUTPUT);
  pinMode(in4Pin, OUTPUT);

  pinMode(echo, INPUT);
  pinMode(trig, OUTPUT);

  // 모터 드라이버 초기 설정
  digitalWrite(enaPin, HIGH);
  digitalWrite(in1Pin, LOW);
  digitalWrite(in2Pin, HIGH);
  digitalWrite(enbPin, HIGH);
  digitalWrite(in3Pin, LOW);
  digitalWrite(in4Pin, HIGH);

  analogWrite(enaPin, 0); //초기 모터 세팅은 0으로
  analogWrite(enbPin, 0);

  mySerial.begin(9600);
}

void loop() {
  //for(int i=0;i<num;i++) Serial.print(result_P[i]);

  if(st==0) {
      //컨테이너가 올라와서 초음파 센서 값이 작아지면 차 start
      analogWrite(enaPin, 0);
      analogWrite(enbPin, 0);

      digitalWrite(trig, HIGH);
      delay(10);
      digitalWrite(trig, LOW);
  
      cycletime = pulseIn(echo, HIGH);
      distance = ((340 * cycletime) / 10000) / 2;

    if(distance > 5.0) {
      delay(1000);
    }
    else {
      st=1;
    }
  }
  
  else if(st==1) {
    if (Serial.available() > 0) {
      analogWrite(enaPin, 0);
      analogWrite(enbPin, 0);
      st1=0;
      String barcodeData = Serial.readString(); // 개행 문자까지 읽음
      mySerial.print(barcodeData);
      int kk = barcodeData.toInt(); // 문자열을 정수로 변환
      Serial.println(kk);
     
      int node=result_P[n1];
      if(kk==node) {
        int next_node=9;
        if(node!=end_node) next_node=result_P[n1+1];
        byte n_x=node_location[next_node][0];
        byte x=node_location[node][0];
        byte n_y=node_location[next_node][1];
        byte y=node_location[node][1];

        byte position_direction;
        if(x>n_x) position_direction=3;
        else if(n_x>x) position_direction=1;
        else if(y>n_y) position_direction=0;
        else if(n_y>y) position_direction=2;

        int dd=0;
        if ((car_direction == 0 && position_direction == 1) || (car_direction == 1 && position_direction == 2) || (car_direction == 2 && position_direction == 3) || (car_direction == 3 && position_direction == 0)) dd=1;
        else if ((car_direction == 0 && position_direction == 3) || (car_direction == 1 && position_direction == 0) || (car_direction == 2 && position_direction == 1) || (car_direction == 3 && position_direction == 2)) dd=2;

        if(dd==1) {
          analogWrite(enaPin, 240);
          delay(1000);
          analogWrite(enaPin, 0);
        }
        else if(dd==2) {
          analogWrite(enbPin, 240);
          delay(1000);
          analogWrite(enbPin, 0);
        }
        else {
          analogWrite(enaPin, 220);
          analogWrite(enbPin, 220);
          delay(500);
          analogWrite(enaPin, 0);
          analogWrite(enbPin, 0);
        }

        //Serial.println(dd);
       
        car_direction=position_direction;
        n1++;
        delay(500);
      }
      if(n1==num) {
        analogWrite(enaPin, 0);
        analogWrite(enbPin, 0);
        st1=0;
        st=2;
      }
    }
    else {
      if(st1==0) {
        analogWrite(enaPin, 220);
        analogWrite(enbPin, 220);
        delay(400);
        st1=1;
      }
      else {
        analogWrite(enaPin, 130);
        analogWrite(enbPin, 130);
      }
    }
  }

  else if(st==2) {  //st가 2일 때는 8번노드까지 온 상황
    if (Serial.available() > 0) {
      analogWrite(enaPin, 0);
      analogWrite(enbPin, 0);
      String qrcode=Serial.readString(); //이게 아마 9번
      int mm=qrcode.toInt();

      if(mm==9) {
         //while문 들어가서 초음파 센서 값 측정, 컨테이너가 들려서 초음파 센서 값이 커지면 while문 break하고 qr코드값 보내기
         while(1) {
           digitalWrite(trig, HIGH);
           delay(10);
           digitalWrite(trig, LOW);
  
           cycletime = pulseIn(echo, HIGH);
           distance = ((340 * cycletime) / 10000) / 2;
           if(distance > 5.0) break;
         }
  
        //컨테이너가 들린거니까 앱인벤터로 9번 보내고 st는 3으로
        mySerial.print("21");
        st=3;  
     }
   }
   else {
     if(st1==0) {
       analogWrite(enaPin, 220);
       analogWrite(enbPin, 220); 
       delay(300);
       st1=1;
     }
     else {
       analogWrite(enaPin, 130);
       analogWrite(enbPin, 130);
     }
   }
 }
  else {
    delay(5000);
    analogWrite(enaPin, 160);
    analogWrite(enbPin, 160);
  }
}

void receiveData(int byteCount) {
  while (Wire.available()) {
    byte receivedData = Wire.read(); // 수신된 데이터를 읽습니다.
    // 이제 receivedData 값을 처리하거나 출력합니다.
    // 여기에 원하는 처리 로직을 추가하세요.
    result_P[num]=receivedData;
    num++;
  }
}