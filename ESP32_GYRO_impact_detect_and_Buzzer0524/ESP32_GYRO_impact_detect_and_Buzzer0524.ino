#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

// 변수 선언
float prev_rotationX, prev_rotationY, prev_rotationZ;
float rotationThreshold = 1.0; // 회전 변화 임계값 (조정 가능)

const int beepPin = 15; // 부저를 제어하는 핀 번호

void setup(void) {
  Serial.begin(115200);

  ledcSetup(0, 1E5, 12); // 톤 생성을 위한 LEDC 설정
  ledcAttachPin(beepPin, 0); // 부저 핀을 LEDC 채널에 연결

  while (!Serial)
    delay(10); // 시리얼 통신 초기화

  Serial.println("Adafruit MPU6050 test!");

  // MPU6050 초기화
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");
}

void loop() {
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // 회전 변화 계산
  float rotationX = g.gyro.x;
  float rotationY = g.gyro.y;
  float rotationZ = g.gyro.z;

  float delta_rotationX = abs(rotationX - prev_rotationX);
  float delta_rotationY = abs(rotationY - prev_rotationY);
  float delta_rotationZ = abs(rotationZ - prev_rotationZ);

  // 충격 판단
  if (delta_rotationX > rotationThreshold || delta_rotationY > rotationThreshold || delta_rotationZ > rotationThreshold) {
    Serial.println("Impact detected!"); // 충격 감지 메시지 출력

    // 부저 제어
    tone(beepPin, 1000, 1000); // 1초 동안 1kHz 주파수로 소리 출력
    delay(1000); // 소리 재생 시간

    noTone(beepPin); // 부저 소리 중지
  }

  prev_rotationX = rotationX;
  prev_rotationY = rotationY;
  prev_rotationZ = rotationZ;

  delay(100); // 회전 변화를 측정하는 적절한 간격으로 조정 가능
}
