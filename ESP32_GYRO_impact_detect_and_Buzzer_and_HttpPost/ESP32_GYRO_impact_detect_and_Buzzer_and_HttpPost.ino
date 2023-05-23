#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "ubicomp"; // Wi-Fi SSID
const char* password = "ubicomp407"; // Wi-Fi Password
const char* server = "203.253.128.177"; // 모비우스 서버 IP 주소
const int port = 7579; // 모비우스 서버 포트
const String cnt = "earthquake";
const String ae = "KETIDGZ"; // 모비우스에서 사용할 AE 이름
const String cin = "POSTING from ESP32"; // 전송할 데이터

Adafruit_MPU6050 mpu;

// 변수 선언
float prev_rotationX, prev_rotationY, prev_rotationZ;
float rotationThreshold = 1.0; // 회전 변화 임계값 (조정 가능)

const int beepPin = 15; // 부저를 제어하는 핀 번호



//부저 제어 함수
void controlBuzzer(){
  // 부저 제어
  tone(beepPin, 1000, 1000); // 1초 동안 1kHz 주파수로 소리 출력
  delay(2000); // 소리 재생 시간

  noTone(beepPin); // 부저 소리 중지
}

void setup(void) {
  Serial.begin(115200);

  // MPU6050 초기화
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  delay(1000); // Delay needed before calling the WiFi.begin
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) { // Check for the connection
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to the WiFi network");


  ledcSetup(0, 1E4, 10); // 톤 생성을 위한 LEDC 설정
  ledcAttachPin(beepPin, 0); // 부저 핀을 LEDC 채널에 연결
}

void sendPostRequest(const String& payload) {
  HTTPClient http;

  http.begin("http://" + String(server) + ":" + String(port) + "/Mobius/" + ae + "/" + cnt); // Specify destination for HTTP request
  http.addHeader("Content-Type", "application/vnd.onem2m-res+json; ty=4");
  http.addHeader("Accept", "application/json");
  http.addHeader("X-M2M-RI", "12345");
  http.addHeader("X-M2M-Origin", ae);
  int httpResponseCode = http.POST(payload); // Send the actual POST request

  if (httpResponseCode > 0) {
    String response = http.getString(); // Get the response to the request

    Serial.println(httpResponseCode); // Print return code
    Serial.println(response); // Print request answer
  } else {
    Serial.print("Error on sending POST: ");
    Serial.println(httpResponseCode);
  }

  http.end(); // Free resources

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
    controlBuzzer();

    if (WiFi.status() == WL_CONNECTED) { // Check WiFi connection status
      String payload = "{\"m2m:cin\": {\"con\": \"" + cin + "\"}}"; // 전송할 데이터
      sendPostRequest(payload);
    } else {
      Serial.println("Error in WiFi connection");
    }

    delay(1000); // Send a request every 10 seconds
  }

  // 1분 후에 "0" 값을 POST
  if (millis() >= 60000) {
    if (WiFi.status() == WL_CONNECTED) { // Check WiFi connection status
      String payload = "{\"m2m:cin\": {\"con\": \"0\"}}"; // 전송할 데이터
      sendPostRequest(payload);
    } else {
      Serial.println("Error in WiFi connection");
    }

    millis(); // Reset the timer
  }

  prev_rotationX = rotationX;
  prev_rotationY = rotationY;
  prev_rotationZ = rotationZ;

  delay(100); // 회전 변화를 측정하는 적절한 간격으로 조정 가능
}

