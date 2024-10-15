#ifndef HTML_H
#define HTML_H

String generateHTML(long distanceCm) {
  String html = "<html><head><meta charset='UTF-8'><meta http-equiv='refresh' content='2'><title>แสดงผลตรวจจับวัตถุ</title>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; background-color: #f4f4f4; color: #333; text-align: center; margin: 0; padding: 20px; }";
  html += "h1 { color: #2c3e50; }";
  html += "p { font-size: 1.2em; }";
  html += "button { background-color: #3498db; color: white; border: none; padding: 10px 20px; font-size: 1em; cursor: pointer; margin: 5px; }";
  html += "button:hover { background-color: #2980b9; }";
  html += "form { display: inline-block; margin: 10px; }";
  html += "input[type='text'] { padding: 5px; font-size: 1em; }";
  html += "</style></head><body>";

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
  String html = "<html><head><meta charset='UTF-8'><title>ประวัติการตรวจจับ</title>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; background-color: #f4f4f4; color: #333; text-align: center; margin: 0; padding: 20px; }";
  html += "h1 { color: #2c3e50; }";
  html += "pre { background-color: #ecf0f1; padding: 10px; border: 1px solid #ccc; text-align: left; display: inline-block; max-width: 90%; }";
  html += "a { text-decoration: none; color: #3498db; font-size: 1.2em; }";
  html += "a:hover { color: #2980b9; }";
  html += "</style></head><body>";

  html += "<h1>ประวัติการตรวจจับ</h1>";
  html += "<pre>" + history + "</pre>";  // แสดงประวัติการตรวจจับจาก SPIFFS
  html += "<a href='/'>กลับไปยังหน้าหลัก</a>";
  html += "</body></html>";
  return html;
}

String generateTokenUpdateHTML() {
  String html = "<html><head><meta charset='UTF-8'><title>อัปเดต Token สำเร็จ</title>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; background-color: #f4f4f4; color: #333; text-align: center; margin: 0; padding: 20px; }";
  html += "h1 { color: #27ae60; }";
  html += "a { text-decoration: none; color: #3498db; font-size: 1.2em; }";
  html += "a:hover { color: #2980b9; }";
  html += "</style></head><body>";

  html += "<h1>อัปเดต Token เรียบร้อยแล้ว</h1>";
  html += "<a href='/'>กลับไปยังหน้าหลัก</a>";
  html += "</body></html>";
  return html;
}

#endif
