#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <DHT.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <MFRC522.h>
#include "time.h"

// ——— پین‌ها ———
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR    0x3C
#define I2C_SDA      5
#define I2C_SCL      4

#define DHTPIN       2
#define DHTTYPE      DHT22

#define RST_PIN      16  // RC522 Reset
#define SS_PIN       15  // RC522 Chip Select
#define MFRC522_MOSI 13
#define MFRC522_MISO 12
#define MFRC522_SCK  14

#define RELAY_PIN    25

// ——— وای‌فای ———
const char* WIFI_SSID     = ">><<>><<";
const char* WIFI_PASSWORD = "MEHRdAd1380";

// ——— اشیاء ———
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
DHT dht(DHTPIN, DHTTYPE);
MFRC522 mfrc522(SS_PIN, RST_PIN);
WebServer server(80);

// نگهداری آخرین تگ
struct TagEvent {
  String uid;
  String name;
  String timestamp;
} lastEvent;

// مپ تگ→نام
struct TagMap { const char* uid; const char* name; };
TagMap allowedTags[] = {
  {"62768D4C", "Mehrdad"},
  {"C28A834C", "Masoud"},
  {"828C8D4C", "Yas"},
  {"42E18C4C", "4"}
};
const int nTags = sizeof(allowedTags)/sizeof(allowedTags[0]);

// === تنظیم ساعت ایران ===
void initTime() {
  configTzTime("IRST-3:30", "pool.ntp.org", "time.nist.gov");
}
String nowStr() {
  struct tm t;
  if (!getLocalTime(&t)) return "--:--:--";
  char buf[32];
  strftime(buf, sizeof(buf), "%Y/%m/%d %H:%M:%S", &t);
  return String(buf);
}

// === نمایشگر اصلی ===
void showDefault() {
  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println("Sensor Data");
  display.drawFastHLine(0, 10, SCREEN_WIDTH, SSD1306_WHITE);

  display.setCursor(0, 12);
  display.printf("Temp: %s\n", isnan(temp) ? "Err" : String(temp,1).c_str());

  display.setCursor(0, 28);
  display.printf("Hum : %s\n", isnan(hum) ? "Err" : String(hum,1).c_str());

  display.drawFastHLine(0, 44, SCREEN_WIDTH, SSD1306_WHITE);
  display.setCursor(0, 46);
  display.println(nowStr());
  display.display();
}

// === نمایش بعد از شناسایی تگ ===
void showTagScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0,0);
  display.println("Access Granted");
  display.drawFastHLine(0,10,SCREEN_WIDTH,SSD1306_WHITE);

  display.setCursor(0,12);
  display.printf("Name: %s\n", lastEvent.name.c_str());

  display.setCursor(0,26);
  display.printf("UID : %s\n", lastEvent.uid.c_str());

  display.drawFastHLine(0,42,SCREEN_WIDTH,SSD1306_WHITE);

  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();
  display.setCursor(0,44);
  if (!isnan(temp) && !isnan(hum)) {
    display.printf("T:%.1fC H:%.1f%%", temp, hum);
  } else {
    display.println("DHT Err");
  }
  display.setCursor(0,54);
  display.println(nowStr());
  display.display();

  digitalWrite(RELAY_PIN, LOW);   // فعال کردن رله (اتصال به GND)
  delay(1000);                    // ۱ ثانیه روشن بماند
  digitalWrite(RELAY_PIN, HIGH);  // غیرفعال کردن رله (قطع اتصال به GND)

}

// === JSON API ===
// … (بقیه‌ی کد شما ثابت بماند)
void handleData() {
  DynamicJsonDocument doc(512);
  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();

  // اگر مقدار معتبر است، اضافه کن
  if (!isnan(temp)) doc["localTemperature"] = temp;
  if (!isnan(hum))  doc["localHumidity"]    = hum;

  doc["lastUID"]       = lastEvent.uid;
  doc["lastName"]      = lastEvent.name;
  doc["lastTimestamp"] = lastEvent.timestamp;
  doc["time"]          = nowStr();

  String out;
  serializeJson(doc, out);
  server.send(200, "application/json", out);
}
// … (بقیه‌ی کد شما ثابت بماند)


// === HTML Dashboard ===
void handleRoot() {
  IPAddress ip = WiFi.localIP();
  int    rssi = WiFi.RSSI();
  unsigned long up = millis()/1000;
  unsigned long h = up/3600, m=(up%3600)/60, s=up%60;

  String html = R"rawliteral(
<!DOCTYPE html><html lang="fa"><head><meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1.0">
<title>ESP32 Pro Dashboard</title>
<style>
  body{background:#111;color:#eee;font-family:sans-serif;margin:0;padding:0;}
  header{bg:#222;padding:1rem;text-align:center;}
  header h1{margin:0;font-size:1.5rem;}
  .grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(150px,1fr));gap:8px;padding:8px;}
  .card{background:#222;padding:8px;border-radius:4px;text-align:center;}
  .card h2{margin:0.5rem 0; font-size:1rem;color:#6cf;}
  .card p {
  margin: 0;
  font-size: 1.2rem;
  background: linear-gradient(to right, #00e5ff, #7c4dff);
  -webkit-background-clip: text;
  -webkit-text-fill-color: transparent;
}

  footer{text-align:center;padding:4px;background:#222;font-size:0.8rem;}
</style>
</head><body>
<header><h1>ESP32 Dashboard</h1></header>
<div class="grid">
  <div class="card"><h2>Temp</h2><p id="temp">--</p></div>
  <div class="card"><h2>Humidity</h2><p id="humidity">--</p></div>
  <div class="card"><h2>Last UID</h2><p id="uid">--</p></div>
  <div class="card"><h2>Name</h2><p id="name">--</p></div>
  <div class="card"><h2>Timestamp</h2><p id="ts">--</p></div>
  <div class="card"><h2>Local Time</h2><p id="time">--</p></div>
  <div class="card"><h2>IP</h2><p>)rawliteral" + ip.toString() + R"rawliteral(</p></div>
  <div class="card"><h2>RSSI</h2><p>)rawliteral" + String(rssi)+" dBm"+ R"rawliteral(</p></div>
  <div class="card"><h2>Uptime</h2><p>)rawliteral" + String(h)+"h "+String(m)+"m "+String(s)+"s"+ R"rawliteral(</p></div>
</div>
<footer>بروز رسانی هر ۵ ثانیه</footer>
<script>
function fetchData(){
  fetch('/data').then(r=>r.json()).then(d=>{
    document.getElementById('temp').innerText = d.localTemperature!=null?d.localTemperature+' °C':'Err';
    document.getElementById('humidity').innerText = d.localHumidity!=null?d.localHumidity+' %':'Err';
    document.getElementById('uid').innerText = d.lastUID;
    document.getElementById('name').innerText = d.lastName;
    document.getElementById('ts').innerText = d.lastTimestamp;
    document.getElementById('time').innerText = d.time;
  }).catch(console.error);
}
setInterval(fetchData,5000);
window.onload = fetchData;
</script>
</body></html>
)rawliteral";

  server.send(200, "text/html; charset=utf-8", html);
}

void handleNotFound() {
  server.send(404, "text/plain", "Not Found");
}

void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SDA, I2C_SCL);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // پین 26 غیرفعال به‌طور پیش‌فرض


  // OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED failed"); while(1);
  }
  display.clearDisplay();

  // DHT
  dht.begin();

  // RFID: HSPI initialization
  SPI.begin(MFRC522_SCK, MFRC522_MISO, MFRC522_MOSI, SS_PIN);
  mfrc522.PCD_Init();

  // WiFi + Time
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("WiFi...");
  while (WiFi.status()!=WL_CONNECTED) {
    delay(200); Serial.print(".");
  }
  Serial.println("OK");
  initTime();

  // WebServer
  server.on("/",    HTTP_GET, handleRoot);
  server.on("/data",HTTP_GET, handleData);
  server.onNotFound(handleNotFound);
  server.begin();
}

void loop() {
  server.handleClient();

  static unsigned long lastDisplay=0;
  if (millis() - lastDisplay > 2000) {
    showDefault();
    lastDisplay = millis();
  }

  // RFID polling
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String uid="";
    for (byte i=0;i<mfrc522.uid.size;i++) {
      uid += String(mfrc522.uid.uidByte[i], HEX);
    }
    uid.toUpperCase();
    lastEvent.uid = uid;
    lastEvent.name = "Unknown";
    for (int i=0;i<nTags;i++) {
      if (uid == allowedTags[i].uid) {
        lastEvent.name = allowedTags[i].name;
      }
    }
    lastEvent.timestamp = nowStr();
    showTagScreen();
    mfrc522.PICC_HaltA();
  }
}
