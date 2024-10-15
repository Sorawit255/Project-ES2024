#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include "FS.h"
#include "SPIFFS.h"
#include "html.h"

const int pingPin = 13; // Trig
const int inPin = 12;   // Echo

const char* ssid = "Dragon";
const char* password = "12345678";
String lineNotifyToken = "goMw6x6A4hlet9170QyOn4DAFFmfNnu59h83rax8F9z"; 

long lastNotified = 0;    
const int notificationInterval = 100; 

WebServer server(80); 
long distanceCm = 0;   
bool detecting = false;

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // ฟังก์ชันที่จะเรียกเมื่อเข้าเว็บเซิร์ฟเวอร์
  server.on("/", handleRoot);   // หน้าหลัก
  server.on("/start", handleStart);  
  server.on("/stop", handleStop);    
  server.on("/history", handleHistory); 
  server.on("/updateToken", HTTP_POST, handleUpdateToken); 
  server.begin();     
  Serial.println("HTTP server started");
}

void loop() {
  long duration, cm;

  if (detecting) { 
    pinMode(pingPin, OUTPUT);
    digitalWrite(pingPin, LOW);
    delayMicroseconds(2);
    digitalWrite(pingPin, HIGH);
    delayMicroseconds(5);
    digitalWrite(pingPin, LOW);

    // รับคลื่นสะท้อนกลับ
    pinMode(inPin, INPUT);
    duration = pulseIn(inPin, HIGH);
    cm = microsecondsToCentimeters(duration);
    distanceCm = cm;
    Serial.print(cm);
    Serial.print(" cm");
    Serial.println();
    
    // บันทึกประวัติลง SPIFFS
    saveHistory("ตรวจพบวัตถุในระยะ " + String(cm) + " cm");

    if (cm < 100 && millis() - lastNotified > notificationInterval) {  
      sendLineNotify(cm);
      lastNotified = millis();
    }
  }

  server.handleClient();
  delay(1000);
}

long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}

// ฟังก์ชันสำหรับแจ้งเตือนผ่าน LINE Notify
void sendLineNotify(long distance) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin("https://notify-api.line.me/api/notify");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.addHeader("Authorization", "Bearer " + String(lineNotifyToken));

    String payload = "message=ตรวจจับวัตถุในระยะ " + String(distance) + " cm!";
    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error on sending: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}


void handleRoot() {
  String html = generateHTML(distanceCm);  // เรียกฟังก์ชันจาก html.h
  server.send(200, "text/html", html);  // ส่งเว็บเพจกลับไปยังคลายเอนต์
}

// ปุ่ม Start
void handleStart() {
  detecting = true;  
  server.sendHeader("Location", "/");  
  server.send(303);
}

// ปุ่ม Stop
void handleStop() {
  detecting = false;  
  server.sendHeader("Location", "/");   
  server.send(303);
}

// เปิดหน้าเว็บประวัติ
void handleHistory() {
  String history = readHistory();  // อ่านประวัติจาก SPIFFS
  String html = generateHistoryHTML(history); 
  server.send(200, "text/html", html);  
}

// ฟังก์ชันบันทึกประวัติลงใน SPIFFS
void saveHistory(String data) {
  File file = SPIFFS.open("/history.txt", FILE_APPEND);
  if(!file){
    Serial.println("Failed to open file for appending");
    return;
  }
  file.println(data);
  file.close();
}

// ฟังก์ชันอ่านประวัติจาก SPIFFS
String readHistory() {
  File file = SPIFFS.open("/history.txt");
  if(!file){
    Serial.println("Failed to open file for reading");
    return "";
  }
  String history = "";
  while(file.available()){
    history += file.readString();
  }
  file.close();
  return history;
}

// ฟังก์ชันอัปเดต LINE Notify Token
void handleUpdateToken() {
  if (server.hasArg("token")) {  // ตรวจสอบว่ามีการส่ง token ใหม่มาจากฟอร์มหรือไม่
    lineNotifyToken = server.arg("token");  // อัปเดตตัวแปร lineNotifyToken
    Serial.println("LINE Notify Token อัปเดตเรียบร้อย: " + lineNotifyToken);
   
    // ส่งข้อความแจ้งว่าการอัปเดตสำเร็จ
    String html = generateTokenUpdateHTML();  // เรียกฟังก์ชันจาก html.h
    server.send(200, "text/html", html);
  } else {
    server.send(400, "text/plain", "Bad Request: Token is missing");
  }
}
