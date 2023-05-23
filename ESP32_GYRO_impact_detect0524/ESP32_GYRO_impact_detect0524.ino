// decription
// 자이로 센서는 충격을 감지하는 센서로 작동합니다.
// 각 속도를 계산하여 일정 임계값 이상 센서가 동작할 시 시리얼 모니터에 데이터를 출력합니다.

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

// 변수 선언
float prev_rotationX, prev_rotationY, prev_rotationZ;
float rotationThreshold = 1.0; // 회전 변화 임계값 (조정 가능)

void setup(void) {
  Serial.begin(115200);
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

  }

  prev_rotationX = rotationX;
  prev_rotationY = rotationY;
  prev_rotationZ = rotationZ;

  delay(100); // 회전 변화를 측정하는 적절한 간격으로 조정 가능
}
