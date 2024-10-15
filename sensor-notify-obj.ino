#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include "FS.h"
#include "SPIFFS.h"
#include "html.h"  // รวมไฟล์ html.h

const int pingPin = 13; // Trig
const int inPin = 12;   // Echo

// กำหนด Wi-Fi Credentials
const char* ssid = "Dragon";
const char* password = "12345678";
String lineNotifyToken = "goMw6x6A4hlet9170QyOn4DAFFmfNnu59h83rax8F9z"; 

long lastNotified = 0;    
const int notificationInterval = 10000; // หน่วงการแจ้งเตือน 10 วินาที

WebServer server(80); 
long distanceCm = 0;   
bool detecting = false; // ควบคุมการตรวจจับ

void setup() {
  Serial.begin(9600);
 
  // เชื่อมต่อ WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // เริ่ม SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // กำหนดฟังก์ชันที่จะเรียกเมื่อเข้าเว็บเซิร์ฟเวอร์
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
    distanceCm = cm; // แสดงระยะทางใน Serial Monitor
    Serial.print(cm);
    Serial.print(" cm");
    Serial.println();
    
    // บันทึกประวัติลง SPIFFS
    saveHistory("ตรวจพบวัตถุในระยะ " + String(cm) + " cm");

    // ส่งการแจ้งเตือนหากระยะน้อยกว่า 100 cm และไม่มีการแจ้งเตือนซ้ำภายใน 10 วินาที
    if (cm < 100 && millis() - lastNotified > notificationInterval) {  
      sendLineNotify(cm);
      lastNotified = millis();
    }
  }

  server.handleClient();
  delay(1000);
}

// ฟังก์ชันคำนวณระยะทาง
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

// ฟังก์ชันที่ใช้จัดการเมื่อเปิดเว็บเพจ
void handleRoot() {
  String html = generateHTML(distanceCm);  // เรียกฟังก์ชันจาก html.h
  server.send(200, "text/html", html);  // ส่งเว็บเพจกลับไปยังคลายเอนต์
}

// ฟังก์ชันที่ใช้เมื่อกดปุ่ม Start
void handleStart() {
  detecting = true;  // เริ่มตรวจจับ
  server.sendHeader("Location", "/");   // เปลี่ยนเส้นทางกลับไปที่หน้าเว็บหลัก
  server.send(303);
}

// ฟังก์ชันที่ใช้เมื่อกดปุ่ม Stop
void handleStop() {
  detecting = false;  // หยุดตรวจจับ
  server.sendHeader("Location", "/");   // เปลี่ยนเส้นทางกลับไปที่หน้าเว็บหลัก
  server.send(303);
}

// ฟังก์ชันที่ใช้จัดการเมื่อเปิดหน้าเว็บประวัติ
void handleHistory() {
  String history = readHistory();  // อ่านประวัติการตรวจจับจาก SPIFFS
  String html = generateHistoryHTML(history);  // เรียกฟังก์ชันจาก html.h
  server.send(200, "text/html", html);  // ส่งเว็บเพจกลับไปยังคลายเอนต์
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
