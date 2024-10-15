#ifndef HTML_H
#define HTML_H

String generateHTML(long distanceCm) {
  String html = "<html><head><meta charset='UTF-8'><meta http-equiv='refresh' content='2'><title>แสดงผลตรวจจับวัตถุ</title></head><body>";
  html += "<h1>ระยะห่างจากวัตถุ</h1>";
  html += "<p>ระยะห่างปัจจุบัน: " + String(distanceCm) + " cm</p>";
  html += "<form action='/start' method='POST'><button type='submit'>Start</button></form>";
  html += "<form action='/stop' method='POST'><button type='submit'>Stop</button></form>";
  html += "<form action='/history' method='GET'><button type='submit'>ดูประวัติ</button></form>";

  // ฟอร์มสำหรับใส่ token ใหม่
  html += "<h2>เปลี่ยน LINE Notify Token</h2>";
  html += "<form action='/updateToken' method='POST'>";
  html += "LINE Notify Token: <input type='text' name='token'><br>";
  html += "<button type='submit'>อัปเดต Token</button>";
  html += "</form>";

  html += "</body></html>";
  return html;
}

String generateHistoryHTML(String history) {
  String html = "<html><head><meta charset='UTF-8'><title>ประวัติการตรวจจับ</title></head><body>";
  html += "<h1>ประวัติการตรวจจับ</h1>";
  html += "<pre>" + history + "</pre>";  // แสดงประวัติการตรวจจับจาก SPIFFS
  html += "<a href='/'>กลับไปยังหน้าหลัก</a>";
  html += "</body></html>";
  return html;
}

String generateTokenUpdateHTML() {
  String html = "<html><head><meta charset='UTF-8'><title>อัปเดต Token สำเร็จ</title></head><body>";
  html += "<h1>อัปเดต Token เรียบร้อยแล้ว</h1>";
  html += "<a href='/'>กลับไปยังหน้าหลัก</a>";
  html += "</body></html>";
  return html;
}

#endif
