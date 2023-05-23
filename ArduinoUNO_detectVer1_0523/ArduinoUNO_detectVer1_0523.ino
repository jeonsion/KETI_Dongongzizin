#include "I2Cdev.h"
#include "MPU6050.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

MPU6050 accelgyro; // MPU6050 객체 선언

int16_t ax, ay, az; // 가속도계 데이터 변수
int16_t gx, gy, gz; // 자이로스코프 데이터 변수

#define LED_PIN 13 // LED 핀 번호
bool blinkState = false; // LED 상태 변수

int16_t prev_gx = 0; // 이전 자이로스코프 X축 값
int16_t prev_gy = 0; // 이전 자이로스코프 Y축 값
int16_t prev_gz = 0; // 이전 자이로스코프 Z축 값

int16_t impactThreshold = 1000; // 충격 감지 임계값

void setup() {
    // I2C 통신 설정
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    // 시리얼 통신 설정
    Serial.begin(9600);

    // I2C 장치 초기화
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();

    // 장치 연결 확인
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

    // LED 핀을 출력으로 설정
    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    // 가속도계와 자이로스코프 데이터 읽기
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // 현재 값과 이전 값의 차이 계산
    int16_t delta_gx = abs(gx - prev_gx);
    int16_t delta_gy = abs(gy - prev_gy);
    int16_t delta_gz = abs(gz - prev_gz);

    // 이전 값 업데이트
    prev_gx = gx;
    prev_gy = gy;
    prev_gz = gz;

    // 충격 감지 여부 확인
    if (delta_gx > impactThreshold || delta_gy > impactThreshold || delta_gz > impactThreshold) {
        Serial.println("Impact detected!"); // 충격 감지 메시지 출력
        // 여기에 추가적인 동작을 추가할 수 있습니다.
    }

    // LED 상태 변경
    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);

    delay(100); // 필요에 따라 딜레이 조정
}
