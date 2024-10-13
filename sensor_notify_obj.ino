#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>

const int pingPin = 13; // Trig
const int inPin = 12;   // Echo

// กำหนด Wi-Fi Credentials
const char* ssid = "Dragon";
const char* password = "12345678";
String lineNotifyToken = "goMw6x6A4hlet9170QyOn4DAFFmfNnu59h83rax8F9z"; 

long lastNotified = 0;    
const int notificationInterval = 100; 

WebServer server(80); 
long distanceCm = 0;   
bool detecting = false; // ควบคุมการตรวจจับ

String historyLog = ""; // เก็บประวัติการตรวจจับ

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

  // กำหนดฟังก์ชันที่จะเรียกเมื่อเข้าเว็บเซิร์ฟเวอร์
  server.on("/", handleRoot);   // หน้าหลัก  
  server.on("/history", handleHistory); 
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
    historyLog += "ตรวจพบวัตถุในระยะ " + String(cm) + " cm\n";
    // ส่งการแจ้งเตือนหากระยะน้อยกว่า 100 cm และไม่มีการแจ้งเตือนซ้ำภายใน 10 วินาที
    if (cm < 100 && millis() - lastNotified > notificationInterval) {  
      sendLineNotify(cm);
      lastNotified = millis();
    }
  }

  server.handleClient();
  delay(1000);
}
// สูตรคำนวณระยะทาง
long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}
// ฟังก์ชันสำหรับแจ้งเตือนผ่าน LINE Notify
void sendLineNotify(long distance) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin("https://notify-api.line.me/api/notify&quot");
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
  String html = "<html><head><meta charset='UTF-8'><meta http-equiv='refresh' content='2'><title>แสดงผลตรวจจับวัตถุ</title></head><body>";
  html += "<h1>ระยะห่างจากวัตถุ</h1>";
  html += "<p>ระยะห่างปัจจุบัน: " + String(distanceCm) + " cm</p>";
  html += "<form action='/start' method='POST'><button type='submit'>Start</button></form>";
  html += "<form action='/stop' method='POST'><button type='submit'>Stop</button></form>";
  html += "<form action='/history' method='GET'><button type='submit'>ดูประวัติ</button></form>";
  html += "</body></html>";
 
  server.send(200, "text/html", html);  // ส่งเว็บเพจกลับไปยังคลายเอนต์
}

//หน้าเว็บประวัติ
void handleHistory() {
  String html = "<html><head><meta charset='UTF-8'><title>ประวัติการตรวจจับ</title></head><body>";
  html += "<h1>ประวัติการตรวจจับ</h1>";
  html += "<pre>" + historyLog + "</pre>"; 
  html += "<a href='/'>กลับไปยังหน้าหลัก</a>";
  html += "</body></html>";
 
  server.send(200, "text/html", html);
}
