#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = "ubicomp"; // Wi-Fi SSID
const char* password = "ubicomp407"; // Wi-Fi Password
const char* server = "203.253.128.177"; // 모비우스 서버 IP 주소
const int port = 7579; // 모비우스 서버 포트
const String cnt = "COMMAND";
const String ae = "KETIDGZ_earthquake"; // 모비우스에서 사용할 AE 이름

LiquidCrystal_I2C lcd(0x27, 20, 4); // LCD 주소와 크기에 따라 수정

unsigned long previousMillis = 0; // 이전 시간
const unsigned long interval = 1000; // 깜빡이는 간격 (1초)

void setup() {
  Serial.begin(115200);
  delay(100);

  lcd.begin(20, 4); // LCD 크기에 따라 수정
  lcd.init();
  lcd.backlight();

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
    http.begin(client, "http://" + String(server) + ":" + String(port) + "/Mobius/" + ae + "/" + cnt + "/" + "la");
    http.addHeader("Accept", "application/json");
    http.addHeader("X-M2M-RI", "12345");
    http.addHeader("X-M2M-Origin", ae);
    int httpCode = http.GET(); // HTTP GET 요청 전송
    String response = http.getString(); // 서버 응답 수신
    http.end(); // HTTP 연결 종료

    if (httpCode == 200) { // HTTP 응답 코드가 200인 경우
      Serial.print("Data received from Mobius:");
      //Serial.println(response);

      // JSON 파싱
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, response);

      // con 값 추출
      String con = doc["m2m:cin"]["con"].as<String>();
      Serial.println("con: " + con);

      // LCD에 출력
      lcd.clear();
      lcd.setCursor(2, 1);
      
      // 서버 값이 0인 경우
      if (con == "0") {
        lcd.print("Have a nice day");
      } else {
        // 서버 값이 0이 아닌 경우
        lcd.print(con);
      }

      // con 값이 0이 아닐 때만 LCD 화면 깜빡이기
      if (con != "0") {
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= interval) {
          previousMillis = currentMillis;
          lcd.noDisplay();
          delay(600);
          lcd.display();
        }
      }
    } else {
      Serial.print("HTTP GET failed: ");
      Serial.println(httpCode);
      Serial.println(response);
    }
  } else {
    Serial.println("Wi-Fi disconnected");
  }
}
