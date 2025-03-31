// arduino due board, DRV8301, BLDC Motor

// 필요한 라이브러리
#include <SimpleFOC.h>
#include <DRV8301.h>
#include <SPI.h>

// ----- 핀 설정 (보드 구성에 맞게 수정) -----
// BLDCDriver3PWM용 PWM 출력 핀 (예: Due의 PWM 지원 핀)
#define PWM_A_PIN   6
#define PWM_B_PIN   7
#define PWM_C_PIN   8

// DRV8301용 SPI 핀 (Due의 기본 하드웨어 SPI 핀 사용)
// Arduino Due의 기본 SPI 핀은 아래와 같습니다.
//  - MOSI: ICSP-4 또는 보드에 따라 75번 핀
//  - MISO: ICSP-1 또는 보드에 따라 74번 핀
//  - SCK:  ICSP-3 또는 보드에 따라 76번 핀
// CS, EN_GATE, FAULT는 별도로 지정합니다.
#define DRV8301_CS_PIN      10   // Chip Select (CS)
#define DRV8301_EN_GATE_PIN 11   // 게이트 드라이브 활성화 핀
#define DRV8301_FAULT_PIN   12   // 장애 상태 출력 핀

// SPI 핀 (Due 하드웨어 SPI는 SPI object를 사용하므로 따로 지정할 필요는 없습니다)
// 하지만 DRV8301 라이브러리 생성자에는 하드웨어 SPI MOSI, MISO, SCK의 디지털 핀 번호가 필요합니다.
// 아래 값은 Arduino Due의 기본 하드웨어 SPI 핀 번호 (보드에 따라 달라질 수 있으므로 확인 필요)
#define DRV8301_MOSI_PIN 75
#define DRV8301_MISO_PIN 74
#define DRV8301_SCK_PIN  76

// ----- 모터 및 드라이버 객체 생성 -----
// 모터: 4극 쌍(폴 페어 수) 예제 (필요시 값 수정)
BLDCMotor motor = BLDCMotor(7);

// 3PWM 드라이버: PWM 핀 3개를 사용
BLDCDriver3PWM driver = BLDCDriver3PWM(PWM_A_PIN, PWM_B_PIN, PWM_C_PIN);

// DRV8301 객체 생성: 생성자 인자는 (MOSI, MISO, SCLK, CS, EN_GATE, FAULT)
DRV8301 gate_driver = DRV8301(DRV8301_MOSI_PIN, DRV8301_MISO_PIN, DRV8301_SCK_PIN,
                               DRV8301_CS_PIN, DRV8301_EN_GATE_PIN, DRV8301_FAULT_PIN);

float target_velocity = 0; // [rad/s]

void setup() {
  // 시리얼 통신 초기화
  Serial.begin(115200);
  while(!Serial); // Due의 경우 시리얼 포트 연결 대기

  Serial.println("Initializing Motor Control...");

  // PWM 드라이버 초기 설정
  driver.voltage_power_supply = 12;  // 전원 전압 [V]
  driver.init();
  
  // DRV8301 초기화 (3PWM 모드 사용)
  // DRV8301 라이브러리 내부에서는 SPI.begin() 등 하드웨어 SPI 설정을 할 수 있으나,
  // 필요시 Arduino Due의 SPI는 기본 SPI 객체를 사용합니다.
  gate_driver.begin(PWM_INPUT_MODE_3PWM);

  // 모터와 드라이버 연결
  motor.linkDriver(&driver);

  // 모터 움직임 제한 설정
  motor.voltage_limit = 6;   // [V]
  motor.velocity_limit = 20; // [rad/s]

  // 오픈 루프 제어 모드 설정
  motor.controller = MotionControlType::velocity_openloop;

  // 모터 초기화
  motor.init();

  Serial.println("Motor ready!");
  delay(1000);
}

void loop() {
  // 오픈 루프 제어: 목표 속도(target_velocity)를 기반으로 모터 구동
  motor.move(target_velocity);

  // 시리얼 명령 수신 (목표 속도 변경)
  serialReceiveUserCommand();
}

// 시리얼 통신을 통한 사용자 명령 수신 함수
void serialReceiveUserCommand() {
  static String received_chars = "";

  while (Serial.available()) {
    char inChar = (char)Serial.read();
    received_chars += inChar;
    if (inChar == '\n') {
      target_velocity = received_chars.toFloat();
      Serial.print("Target velocity set to: ");
      Serial.println(target_velocity);
      received_chars = "";
    }
  }
}
