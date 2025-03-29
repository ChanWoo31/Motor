
#include <AccelStepper.h>

#define STEP_PIN 7
#define DIR_PIN 6
#define ENABLE_PIN 5

AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

int x;

void setup() {
  Serial.begin(9600);  // 시리얼 통신 시작
  while (!Serial) { 
    ; // 시리얼 포트 연결 대기 (필요한 경우)
  }

  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);

  digitalWrite(ENABLE_PIN, HIGH);

  stepper.setMaxSpeed(4000);
  stepper.setAcceleration(2000);

  //현재 위치를 0으로 초기화
  stepper.setCurrentPosition(0);
}

void loop() {

  

  //다음 명령 기다림
  while (Serial.available() == 0) {
    delay(10);
  }

  //수신한 명령에 따라 동작 수
  String command = Serial.readStringUntil('\n');
  command.trim();

  int converted_command = command.toInt(); 

  //모터 동작
  runMotor(converted_command);

  //모터 동작 완료 시 "done" 메시지 전송
  Serial.println("done");

}



void runMotor(int x){
  //모터 움직이기
  stepper.runToNewPosition(x);
  Serial.print(x);
  Serial.println("스텝");

  delay(1000);
}
