#ifndef HTML_H
#define HTML_H

String generateHTML(long distanceCm) {
  String html = "<html><head><meta charset='UTF-8'><meta http-equiv='refresh' content='2'><title>แสดงผลตรวจจับวัตถุ</title>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; background-color: #f4f4f4; color: #333; text-align: center; margin: 0; padding: 20px; }";
  html += "h1 { color: #2c3e50; font-size: 3em; }"; 
  html += "p { font-size: 5em; }";
  html += "button { background-color: #3498db; color: white; border: none; padding: 20px 40px; font-size: 1.5em; cursor: pointer; margin: 15px; }"; 
  html += "button:hover { background-color: #2980b9; }";
  html += "form { display: inline-block; margin: 20px; }";
  html += "input[type='text'] { padding: 10px; font-size: 1.5em; margin: 10px 0; width: 50%; }"; 
  html += "</style></head><body>";

  html += "<h1>ระยะห่างจากวัตถุ</h1>";
  html += "<p>ระยะห่างปัจจุบัน: " + String(distanceCm) + " cm</p>";
  html += "<form action='/start' method='POST'><button type='submit'>Start</button></form>";
  html += "<form action='/stop' method='POST'><button type='submit'>Stop</button></form>";
  html += "<form action='/history' method='GET'><button type='submit'>ดูประวัติ</button></form>"; 

  // ฟอร์มสำหรับใส่ token ใหม่
  html += "<h2 style='font-size: 2em;'>เปลี่ยน LINE Notify Token</h2>"; 
  html += "<form action='/updateToken' method='POST'>";
  html += "LINE Notify Token: <input type='text' name='token'><br>";
  html += "<button type='submit'>อัปเดต Token</button>";
  html += "</form>";

  html += "</body></html>";
  return html;
}

// ปรับ HTML เพื่อเพิ่มปุ่มลบประวัติและการแบ่งหน้า
String generateHistoryHTML(String history, int startLine) {
  String html = "<html><head><meta charset='UTF-8'><title>ประวัติการตรวจจับ</title>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; background-color: #f4f4f4; color: #333; text-align: center; margin: 0; padding: 20px; }";
  html += "h1 { color: #2c3e50; }";
  html += "pre { background-color: #ecf0f1; padding: 10px; border: 1px solid #ccc; text-align: left; display: inline-block; max-width: 90%; }";
  html += "a { text-decoration: none; color: #3498db; font-size: 1.2em; }";
  html += "a:hover { color: #2980b9; }";
  html += "button { background-color: #e74c3c; color: white; border: none; padding: 10px 20px; font-size: 1em; cursor: pointer; margin: 5px; }";
  html += "button:hover { background-color: #c0392b; }";
  html += ".blue-button { background-color: #3498db; color: white; padding: 10px 20px; font-size: 1em; border: none; cursor: pointer; margin-top: 10px; }";
  html += ".blue-button:hover { background-color: #2980b9; }";
  html += "</style></head><body>";
  
  html += "<h1>ประวัติการตรวจจับ</h1>";
  html += "<pre>" + history + "</pre>";
  html += "<div>";
  int previousPage = (startLine / 20) - 1;
  int nextPage = (startLine / 20) + 1;
  if (previousPage >= 0) {
    html += "<a href='/history?page=" + String(previousPage) + "'><button>&lt; ย้อนกลับ</button></a>";
  }
  html += "<a href='/history?page=" + String(nextPage) + "'><button>ถัดไป &gt;</button></a>";
  html += "</div>";
  html += "<a href='/'><button class='blue-button'>กลับไปยังหน้าหลัก</button></a><br>";
  html += "<form action='/clearHistory' method='POST'><button type='submit'>ลบประวัติทั้งหมด</button></form>";
  html += "</body></html>";
  return html;
}



String generateTokenUpdateHTML() {
  String html = "<html><head><meta charset='UTF-8'><title>อัปเดต Token สำเร็จ</title>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; background-color: #f4f4f4; color: #333; text-align: center; margin: 0; padding: 20px; }";
  html += "h1 { color: #27ae60; font-size: 3em; }"; 
  html += "a { text-decoration: none; color: #3498db; font-size: 1.5em; }"; 
  html += "a:hover { color: #2980b9; }";
  html += "</style></head><body>";

  html += "<h1>อัปเดต Token เรียบร้อยแล้ว</h1>";
  html += "<a href='/'>กลับไปยังหน้าหลัก</a>";
  html += "</body></html>";
  return html;
}

#endif
