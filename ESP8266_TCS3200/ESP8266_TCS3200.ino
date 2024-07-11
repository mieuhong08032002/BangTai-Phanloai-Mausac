//blynk
#define BLYNK_TEMPLATE_ID "TMPL6V5y1FxUM"
#define BLYNK_TEMPLATE_NAME "ESP8266TCS3200"
#define BLYNK_FIRMWARE_VERSION        "0.2.0"

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG
#define APP_DEBUG
#define USE_NODE_MCU_BOARD
#include "BlynkEdgent.h"
//servo
#include <Servo.h>
Servo myservo;
//gogole sheet
//#include <WiFiClientSecure.h>
//#include <ESP8266HTTPClient.h>
//#include <WiFiManager.h>
//#include <WiFiClientSecureBearSSL.h>     
//#include <Wire.h>                                  
//std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
//String color_ar[3]={"RED","GREEN","BLUE"};

//Định nghĩa chân kết nối
const int s0 = D3;
const int s1 = D4;                                         
const int s2 = D6;
const int s3 = D7;
const int out = D5;
const int sensorH1 = D2;
//khai báo biến
const int ag_sv[3] = {20, 90, 160};
int red = 0;
int green = 0;
int blue = 0;
int countRed = 0;
int countGreen = 0;
int countBlue = 0;
int color = 0;
int array_color[10];
int stt_h1 = 0;
int index_sv = 0;
int index_rec = 0;
unsigned long tim_start = 0;
int tim_end = 3500;
int stt_s = 0;
void setup() {
  Serial.begin(115200);
  myservo.attach(D8);  // attaches the servo on GIO2 to the servo object
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  pinMode(out, INPUT);
  pinMode(sensorH1, INPUT);
  digitalWrite(s0, HIGH);//set TCS3200
  digitalWrite(s1, HIGH);
  WiFi.mode(WIFI_STA);
  BlynkEdgent.begin();
  // client->setInsecure();

}

void loop() {
  index_sv = index_sv % 10;
  index_rec = index_rec % 10;
  if (millis() - tim_start >= tim_end) {
    readColor();
  }
  //write_google_sheet("jsagdf");
  readSensor();
  updateDataToBlink();
}

void updateDataToBlink() {
  BlynkEdgent.run();
  Blynk.virtualWrite(V11, countRed);
  Blynk.virtualWrite(V12, countBlue);
  Blynk.virtualWrite(V13, countGreen);
  Blynk.virtualWrite(V14, countRed + countBlue + countGreen);
}
BLYNK_CONNECTED() {
  // Request Blynk server to re-send latest values for all pins
  Blynk.syncAll();
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
  if (red < 30 && blue < 30 && green < 30) return;
  // So sánh để phân loại ra 3 màu Red - Blue - Green
  if (red < blue && red < green && blue < green && green > 40 && blue > 30) { // Nhận biết màu ĐỎ
    delay(250);
    countRed++;
    array_color[index_rec] = 1;
    index_rec++;
    tim_start = millis();
    Serial.println("Red: " + (String)(countRed));
    return;
  }
  if (blue < red && blue < green && red > green && red > 25 && blue < 15) { //Nhận biết màu Xanh lam
    delay(250);
    countBlue++;
    array_color[index_rec] = 2;
    index_rec++;
    tim_start = millis();
    Serial.println("Blue: " + (String)(countBlue));
    return;
  }
  if (green < red && green < blue && blue > red && red < 30 && green < 25) {// Nhận biết màu Xanh Lục
    delay(250);
    countGreen++;
    array_color[index_rec] = 3;
    index_rec++;
    tim_start = millis();
    Serial.println("Green: " + (String)(countGreen));
    return;
  }
}
void controlServo(int n) {
  if (n == 0) return;
  myservo.write(ag_sv[n - 1]);
}
void readSensor() {
  delay(100);
  stt_s = digitalRead(D2);
  if (stt_s == 0) {
    stt_h1 = 1;
  }
  else {
    if (stt_h1 == 1) {
      stt_h1 = 0;
      controlServo(array_color[index_sv]);
      index_sv++;
      Serial.println(index_sv);
    }
  }
}
//void write_google_sheet(String params) {
//  
//  
//   HTTPClient https;
//   String url="https://script.google.com/macros/s/AKfycbwTHG5ZhAyJ4Kzwb1DwU7mtuNsaw17-oskeVqOloJA_HjPJNIyM0dYG17t9UCE3BqVGiw/exec?loaihang=123";
//   Serial.println("Reading Data From Google Sheet.....");
//   https.begin(*client, url.c_str());
//   //-----------------------------------------------------------------------------------
//   //Removes the error "302 Moved Temporarily Error"
//   https.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
//   //-----------------------------------------------------------------------------------
//   //Get the returning HTTP status code
//   int httpCode = https.GET();
//   Serial.print("HTTP Status Code: ");
//   Serial.println(httpCode);
//   
//   https.end();
//}
