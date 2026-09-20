#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <WiFi.h>
#include "time.h"

const char* ssid     = "Jaiguruji77";
const char* password = "my password privacy";

const char* ntpServer = "pool.ntp.org";
const char* timeZone  = "EST5EDT,M3.2.0,M11.1.0";

#define VIV_SCLK 9
#define VIV_MOSI 10
#define VIV_RST 8
#define VIV_DC 4
#define VIV_CS 5
#define VIV_BL 6

class MyST7789 : public Adafruit_ST7789 {
  public:
  MyST7789(int8_t cs, int8_t dc, int8_t mosi, int8_t sclk, int8_t rst)
  : Adafruit_ST7789(cs, dc, mosi, sclk, rst) {}

  void setOffsets(uint8_t col, uint8_t row ) {
    _colstart = _colstart2 = col;
    _rowstart = _rowstart2 = row;
  }
};

MyST7789 viv(VIV_CS, VIV_DC, VIV_MOSI, VIV_SCLK, VIV_RST);

void setup() {
  Serial.begin(115200);

  pinMode(VIV_BL, OUTPUT);
  digitalWrite(VIV_BL, LOW);

  viv.init(76, 284);
  viv.setOffsets(82, 18);
  viv.invertDisplay(false);
  viv.setRotation(1);

  viv.fillScreen(ST77XX_BLACK);

  viv.setTextColor(ST77XX_WHITE);
  viv.setTextSize(2);
  viv.setCursor(10, 30);
  viv.print("Connected WiF.i.");

  WiFi.begin(ssid, password);
  while (WiFi.status()!= WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWifi connected!");
  Serial.println("TFT Initialized!");

  viv.fillScreen(ST77XX_BLACK);

}

void loop() {
  
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    delay(500);
    return;
  }

  char timeHour[3];
  char timeMinute[3];
  char timeSecond[3];
  char dateStr[11];

  strftime(timeHour, 3, "%H", &timeinfo);
  strftime(timeMinute, 3, "%M", &timeinfo);
  strftime(timeSecond, 3, "%S", &timeinfo);
  strftime(dateStr, 11, "%Y-%m-%d", &timeinfo);

  viv.fillScreen(ST77XX_BLACK);

  viv.setTextSize(4);
  viv.setTextColor(ST77XX_WHITE);
  viv.setCursor(15, 15);
  viv.print(timeHour);
  viv.print(":");
  viv.print(timeSecond);

  viv.setTextSize(1);
  viv.setTextColor(ST77XX_CYAN);
  viv.setCursor(15, 60);
  viv.print(dateStr);

  delay(1000);

}
