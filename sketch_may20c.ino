#include <Servo.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(10,11);

Servo servo1;  // 첫 번째 서보 모터 객체
Servo servo2;  // 두 번째 서보 모터 객체

int relayPin1 = 2; // D2 핀을 전자석 모듈 1을 제어하는 데 사용
int relayPin2 = 3; // D3 핀을 전자석 모듈 2를 제어하는 데 사용

void setup() {
  Serial.begin(9600);
  
  servo1.attach(A0);  // 첫 번째 서보 모터를 A0 핀에 연결
  servo2.attach(A1);  // 두 번째 서보 모터를 A1 핀에 연결
  
  pinMode(relayPin1, OUTPUT); // D2 핀을 출력 모드로 설정
  pinMode(relayPin2, OUTPUT); // D3 핀을 출력 모드로 설정

  mySerial.begin(9600);
}

void loop() {
  if(mySerial.available()>0) {
    int n=mySerial.read();
    Serial.println(n);
    
    for(int i=180;i>=0;i--) {
      servo1.write(i);
      servo2.write(180-i);
      delay(10);
    }
    digitalWrite(relayPin1, HIGH);
    digitalWrite(relayPin2, HIGH);
    delay(3000); // 5초 동안 대기

    for(int i=0;i<180;i++) {
      servo1.write(i);
      servo2.write(180-i);
      delay(10);
    }
    delay(1000);

    //리니어 레일 추가하면 왼쪽으로 옮기는 부분
    
    for(int i=180;i>=0;i--) {
      servo1.write(i);
      servo2.write(180-i);
      delay(10);
    }
    digitalWrite(relayPin1, LOW); // 전자석 모듈 1을 끄기
    digitalWrite(relayPin2, LOW);
    delay(3000); // 5초 동안 대기

    for(int i=0;i<180;i++) {
      servo1.write(i);
      servo2.write(180-i);
      delay(10);
    }
    delay(5000);
    //리니어 레일 추가하면 오른쪽으로 옮기는 부분
  }
  else {
    servo1.write(180);
    servo2.write(0);
  }  
}
