# 🚀 ESP32 RFID + DHT22 + OLED + Web Dashboard 🔐🌡️📊

[![Platform](https://img.shields.io/badge/platform-ESP32-blue.svg)](https://www.espressif.com/en/products/socs/esp32)
[![License](https://img.shields.io/github/license/mehrdadmb2/ESP32-RFID-DHT-OLED-WebDashboard)](LICENSE)
[![Language](https://img.shields.io/badge/language-C++-brightgreen.svg)](https://arduino.cc)
[![Web Dashboard](https://img.shields.io/badge/interface-Web%20Dashboard-orange)](https://)
[![Open Source](https://badgen.net/badge/open/source/green)]()

🎉 **A complete IoT security and monitoring system using ESP32**, featuring:
- 🔐 RFID-based access control (MFRC522)
- 🌡️ Real-time temperature and humidity via DHT22
- 🖥️ OLED status display
- 🌐 Live web dashboard with JSON API
- 📟 Real-time local time using NTP
- 💡 Relay control on successful RFID scan

---

## 📦 Features
- 🔄 Real-time sensor monitoring
- 🛡️ Secure tag-based access with logging
- 📺 OLED feedback for user and sensor status
- 📡 WiFi-based web dashboard & API (JSON)
- ⏰ Auto-sync time with NTP (Iran time zone)
- 💻 Fully responsive HTML interface (Farsi supported)
- ⚙️ Easy customization and tag management

---

## 🔧 Hardware Required

| Module            | Model            | Notes                          |
|------------------|------------------|--------------------------------|
| MCU              | ESP32 (Wemos D1 R32 or similar) | Main controller              |
| RFID Reader      | MFRC522          | SPI-based                      |
| Temperature/Humidity | DHT22         | GPIO-based                     |
| Display          | SSD1306 OLED 128x64 | I2C: SDA (GPIO 5), SCL (GPIO 4) |
| Relay Module     | 1-channel relay  | Controlled via GPIO 25         |
| Power            | 5V 2A adapter    | Or USB power                   |

---

## ⚙️ Wiring Diagram

| Component | ESP32 Pin  |
|-----------|------------|
| DHT22     | GPIO 2     |
| OLED SDA  | GPIO 5     |
| OLED SCL  | GPIO 4     |
| MFRC522 RST | GPIO 16  |
| MFRC522 SS  | GPIO 15  |
| MFRC522 MOSI | GPIO 13 |
| MFRC522 MISO | GPIO 12 |
| MFRC522 SCK  | GPIO 14 |
| Relay     | GPIO 25    |

---

## 🔌 Setup & Installation

1. 📥 **Clone the repo**:
   ```bash
   git clone https://github.com/mehrdadmb2/ESP32-RFID-DHT-OLED-WebDashboard.git
````

2. 📦 **Install required libraries in Arduino IDE**:

   * `Adafruit SSD1306`
   * `Adafruit GFX`
   * `DHT sensor library`
   * `MFRC522 by GithubCommunity`
   * `ArduinoJson`
   * `ESP32 board definitions`

3. ⚙️ **Configure your WiFi** in the code:

   ```cpp
   const char* WIFI_SSID     = "Your_SSID";
   const char* WIFI_PASSWORD = "Your_PASSWORD";
   ```

4. ✅ **Upload code** to ESP32 via USB.

5. 🌐 **Access web dashboard**:

   * Connect to the same WiFi as ESP32.
   * Find ESP32 IP address in serial monitor.
   * Open `http://<ESP_IP>` in your browser.

---

## 📊 Web Dashboard Preview

![Web Dashboard Screenshot](https://raw.githubusercontent.com/mehrdadmb2/ESP32-RFID-DHT-OLED-WebDashboard/main/assets/dashboard-preview.png)

* Updates every 5 seconds 🕓
* Shows sensor values, user data, uptime, IP, etc.
* Responsive on mobile and desktop 📱💻

---

## 🧩 Tag Management

Add new tags to `allowedTags` list:

```cpp
TagMap allowedTags[] = {
  {"62768D4C", "Mehrdad"},
  {"C28A834C", "Masoud"},
  {"828C8D4C", "Yas"},
  {"42E18C4C", "4"}  // Add your own
};
```

> 🛠 You can find the tag UID by scanning it and checking serial monitor.

---

## 📦 JSON API

Visit: `http://<ESP_IP>/data`

```json
{
  "localTemperature": 26.4,
  "localHumidity": 41.2,
  "lastUID": "62768D4C",
  "lastName": "Mehrdad",
  "lastTimestamp": "2025/05/24 12:34:56",
  "time": "2025/05/24 12:34:56"
}
```

---

## ✨ Customization Ideas

* Add more sensors (e.g., MQ-2 for smoke)
* Save logs to SD card or Firebase
* Enable buzzer or door unlock via relay
* Use Telegram Bot API to notify access

---

## 🛠 Troubleshooting

| Problem             | Fix                                         |
| ------------------- | ------------------------------------------- |
| OLED not displaying | Check I2C address (usually `0x3C`)          |
| WiFi not connecting | Check SSID/PASS, try rebooting router       |
| MFRC522 not working | Check SPI wiring + power from 3.3V          |
| DHT22 showing `NaN` | Check wiring; may need delay before reading |

---

## 🤝 Contribution

Pull requests welcome! Please follow best practices and include screenshots.

---

## 📜 License

MIT License © [Mehrdad](https://github.com/mehrdadmb2)

---

## ❤️ Acknowledgments

* [Adafruit Libraries](https://github.com/adafruit)
* [ArduinoJson](https://github.com/bblanchon/ArduinoJson)
* [ESP32 Core](https://github.com/espressif/arduino-esp32)

---

> ساخته شده با 💡 و عشق توسط مهراد برای علاقه‌مندان به IoT

```

---

### ⬇️ مراحل بعدی

1. فایل `README.md` را در پوشه ریپوی خود ذخیره کنید.
2. مطمئن شوید اسکرین‌شات از داشبورد وب در مسیر `assets/dashboard-preview.png` وجود دارد (یا لینک عکس آنلاین بگذارید).
3. `git add`, `git commit`, و `git push` کنید.

اگر بخوای می‌تونم یه آیکن حرفه‌ای برای پروژه‌ات طراحی کنم یا GIF نمایش لحظه‌ای داشبورد بسازم برای README.

آماده‌ای ادامه بدیم؟ 😎
```
