#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "ubicomp"; // Wi-Fi SSID
const char* password = "ubicomp407"; // Wi-Fi Password
const char* server = "203.253.128.177"; // 모비우스 서버 IP 주소
const int port = 7579; // 모비우스 서버 포트
const String cnt2 = "command";
const String cnt1 = "led";
const String ae = "KETIDGZ_earthquake"; // 모비우스에서 사용할 AE 이름

const int ledPin = D8; // 발광 LED 핀
const int Buzzer2 = D3; 

void setup() {
  Serial.begin(115200);
  delay(100);
  
  pinMode(Buzzer2, OUTPUT);
  pinMode(ledPin, OUTPUT);

  WiFi.begin(ssid, password);

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected to Wi-Fi");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    // HTTP GET 요청 설정
    http.begin(client, "http://" + String(server) + ":" + String(port) + "/Mobius/" + ae + "/" + cnt1 + "/" + cnt2 + "/la");
    http.addHeader("Accept", "application/json");
    http.addHeader("X-M2M-RI", "12345");
    http.addHeader("X-M2M-Origin", ae);
    int httpCode = http.GET(); // HTTP GET 요청 전송
    String response = http.getString(); // 서버 응답 수신
    http.end(); // HTTP 연결 종료

    if (httpCode == 200) { // HTTP 응답 코드가 200인 경우
      Serial.print("Data received from Mobius:");
      Serial.println(response);

      // JSON 파싱
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, response);

      // con 값 추출
      String con = doc["m2m:cin"]["con"].as<String>();
      Serial.println("con: " + con);

      if (con != "0") {
        // 서버 값이 1일 때 LED 깜빡이기 10초 동안
        for (int i = 0; i < 50; i++) {
          digitalWrite(Buzzer2,HIGH); //(B4 note)
          digitalWrite(ledPin, HIGH);
          delay(200);
          digitalWrite(ledPin, LOW);
          digitalWrite(Buzzer2,LOW); //(B4 note)

          delay(200);
        }
        return; // HTTP GET 요청 종료
      }
    } else {
      Serial.print("HTTP GET failed: ");
      Serial.println(httpCode);
      Serial.println(response);
    }
  } else {
    Serial.println("Wi-Fi disconnected");
  }

  delay(1000); // 1초마다 요청을 보내도록 설정
}