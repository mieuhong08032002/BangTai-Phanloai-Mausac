#include "HX711.h"
#include <ESP32Servo.h>
#include <HardwareSerial.h>
#include <WiFiManager.h>
#include <HTTPClient.h>
#include <Wire.h>

// load cell
const int LOADCELL_DOUT_PIN = 4;
const int LOADCELL_SCK_PIN = 2;
HX711 scale;
//tsc3200
const int s0 = 19;
const int s1 = 18;
const int out = 14;
const int s2 = 27;
const int s3 = 26;

int red = 0;
int green = 0;
int blue = 0;
//servo
Servo myservo;
//uart
HardwareSerial mySerial(2);
String stt_send_uart;
//google sheet
String Web_App_URL = "https://script.google.com/macros/s/AKfycbzzRFjArZ5VYtkewoF3DoAa9T5XT054b5_2i8nI0BkFN6R9LcE1Nz_vFMY5oo9avbDC7A/exec";
String key;
int loai;
int trong_luong = 0;
int a = 0;

void setup() {
  Serial.begin(115200);
  mySerial.begin(115200, SERIAL_8N1, 16, 17);

  myservo.attach(13);

  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  pinMode(out, INPUT);

  digitalWrite(s0, HIGH);
  digitalWrite(s1, LOW);

  WiFiManager wifiManager;
  // reset và erase cài đặt trước đó
  // wifiManager.resetSettings();
  // tên thiết bị, để tạo SSID mặc định
  wifiManager.autoConnect("DATN2024");
  // Sau khi kết nối thành công
  Serial.println("Kết nối WiFi thành công!");
  controlServo(135);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(1000);    // this value is obtained by calibrating the scale with known weights as in previous step
  scale.tare();
  delay(1000);
  controlServo(30);
  //sendKeyToUart("rst=0\0");
  // reset the scale to 0
  Serial.println("DATN 2024- Phan Loai Nong San");
  mySerial.println("DATN 2024- Phan Loai Nong San");
}
void loop() {
  trong_luong = read_load_cell();
  delay(1000);
  for (int i = 30; i <= 135; i += 5) {
    controlServo(i);
    delay(25);
  }
  phanLoai();
  if (loai != 0) {
    sendKeyToUart("k" + (String)(loai) + "=0\0");
    while (true)
    {
      delay(100);
      if (readKeyValue() == "ok") {
        break;
      }
      else {
        sendKeyToUart("k" + (String)(loai) + "=0\0");
      }
    }
    writeSheet();
    loai = 0;
  }
}
void writeSheet() {
  String Send_Data_URL = Web_App_URL + "?sts=write";
  Send_Data_URL += "&loaihang=" + String(loai);
  Send_Data_URL += "&can=" + String(trong_luong);
  Serial.println();
  Serial.println("-------------");
  Serial.println("Send data to Google Spreadsheet...");
  Serial.print("URL : ");
  Serial.println(Send_Data_URL);
  // Initialize HTTPClient as "http".
  HTTPClient http;

  // HTTP GET Request.
  http.begin(Send_Data_URL.c_str());
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

  // Gets the HTTP status code.
  int httpCode = http.GET();
  Serial.print("HTTP Status Code : ");
  Serial.println(httpCode);

  // Getting response from google sheets.
  String payload;
  if (httpCode > 0) {
    payload = http.getString();
    Serial.println("Payload : " + payload);
  }

  http.end();
}
void phanLoai() {
  int timeStart = millis();
  while ( millis() - timeStart   < 5000) {
    readColor();
    if (green > blue & blue > red & red<1800 & blue>2300 & green > 2900 & trong_luong > 45) {
      loai = 1;
      Serial.println("Loai: " + String(loai));
      break;
    }
    else if (green > blue & blue > red & red<1800 & blue>2300 & green > 2900 & trong_luong <= 45) {
      loai = 2;
      Serial.println("Loai: " + String(loai));
      break;
    }
    else if (blue > green & green > red & red > 1400 & blue > 2500 & green > 2200 ) {
      loai = 3;
      Serial.println("Loai: " + String(loai));
      break;
    }
    else if (blue > red & green > blue & red > 2000 & blue > 2480 & green > 2500) {
      loai = 4;
      Serial.println("Loai: " + String(loai));
      break;
    }
    else  loai = 0;
  }
  delay(1000);
  controlServo(30);// tro ve vi tri ban dau
}
void sendKeyToUart(String key) {
  mySerial.print(key);
}
String readKeyValue() {
  if (mySerial.available()) {
    String data = mySerial.readStringUntil('\n');
    // Tách key
    return data;
  }
  return "\0";
}
float roundTo(float value, int decimalPlaces) {
  float scale = pow(10, decimalPlaces);
  return round(value * scale) / scale;
}
float read_load_cell() {
  float h = scale.get_units(10);
  scale.power_down();              // put the ADC in sleep mode
  delay(500);
  scale.power_up();
  if (h > 15) {
    Serial.print("Weight: ");
    Serial.println(h, 1);
    return roundTo(h, 2);
  }
  else return read_load_cell();
}
void readColor() {
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);
  //count OUT, pRed, RED
  red = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);
  digitalWrite(s3, HIGH);
  //count OUT, pBLUE, BLUE
  blue = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);
  digitalWrite(s2, HIGH);
  //count OUT, pGreen, GREEN
  green = pulseIn(out, digitalRead(out) == HIGH ? LOW : HIGH);
  //Giá trị R_G_B của màu sắc thu được
  Serial.print("R Intensity:");
  Serial.print(red, DEC);
  Serial.print(" G Intensity: ");
  Serial.print(green, DEC);
  Serial.print(" B Intensity : ");
  Serial.print(blue, DEC);
  Serial.println();

}
void controlServo(int n) {
  myservo.write(n);
}
