
#include <WiFi.h>
#include <time.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

// =====================================================
// BLARE CLOCK - XIAO ESP32-C3
// =====================================================

// Display GPIO pins
#define PIN_TFT_DC    6
#define PIN_TFT_CS    7
#define PIN_TFT_BL   21   // Active LOW
#define PIN_TFT_RST   8
#define PIN_TFT_SCLK  9
#define PIN_TFT_MOSI 10

#define PIN_BUZZER 20

// SWAPPED UP AND RIGHT BUTTONS
#define PIN_BTN_UP     5
#define PIN_BTN_DOWN   3
#define PIN_BTN_LEFT   4
#define PIN_BTN_RIGHT  2

#define SCREEN_W 284
#define SCREEN_H 76

// Use hardware SPI on the same wired pins for much faster drawing.
Adafruit_ST7789 display(PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST);

#define BLACK    0x0000
#define WHITE    0xFFFF
#define CYAN     0x07FF
#define LAVENDER 0xDDBF
#define GREY     0x8410
#define DARKGREY 0x2104

const char* WIFI_SSID = "JaiGuruji77";
const char* WIFI_PASSWORD = "Gunjan@7982";
const char* TIMEZONE = "EST5EDT,M3.2.0/2,M11.1.0/2";

bool alarmEnabled = true;
int alarmHour = 6;
int alarmMinute = 30;

bool alarmRinging = false;
int lastTriggeredDay = -1;

enum Screen { HOME, MENU, SET_HOUR, SET_MINUTE };
Screen currentScreen = HOME;

int menuSelection = 0;
const char* menuItems[] = {
  "Alarm ON/OFF",
  "Set alarm",
  "Back"
};
const int MENU_COUNT = 3;

bool forceRedraw = true;

unsigned long lastClockUpdate = 0;
int lastShownHour = -1;
int lastShownMinute = -1;
int lastShownDay = -1;
int lastShownMonth = -1;
int lastShownYear = -1;
unsigned long lastButtonTime = 0;
unsigned long lastAlarmBeep = 0;
unsigned long lastClickBeep = 0;

const unsigned long CLOCK_INTERVAL = 250; // Check often; draw only when displayed values change.
const unsigned long BUTTON_DELAY = 180;
const unsigned long ALARM_INTERVAL = 700;

bool clickBeepActive = false;
bool alarmToneActive = false;
unsigned long clickBeepEnd = 0;
unsigned long alarmToneEnd = 0;

// =====================================================
// TIME
// =====================================================

bool getClockTime(struct tm &t) {
  return getLocalTime(&t, 10);
}

void formatAlarm(char* out, size_t len) {
  int h = alarmHour % 12;
  if (h == 0) h = 12;
  snprintf(out, len, "%d:%02d %s",
           h, alarmMinute, alarmHour >= 12 ? "PM" : "AM");
}

void formatCurrentTime(char* out, size_t len, const struct tm &t) {
  int h = t.tm_hour % 12;
  if (h == 0) h = 12;
  snprintf(out, len, "%d:%02d", h, t.tm_min);
}

// =====================================================
// BUZZER
// Short click beeps and repeating alarm beeps
// =====================================================

void startClickBeep() {
  // Do not interrupt an active alarm sound.
  if (alarmRinging) return;

  tone(PIN_BUZZER, 2400);
  clickBeepActive = true;
  clickBeepEnd = millis() + 35;
}

void updateBuzzer() {
  unsigned long now = millis();

  if (alarmRinging) {
    clickBeepActive = false;

    if (now >= lastAlarmBeep) {
      if (!alarmToneActive) {
        tone(PIN_BUZZER, 2000);
        alarmToneActive = true;
        alarmToneEnd = now + 300;
      } else if (now >= alarmToneEnd) {
        noTone(PIN_BUZZER);
        alarmToneActive = false;
        lastAlarmBeep = now + 400;
      }
    }
    return;
  }

  if (clickBeepActive && now >= clickBeepEnd) {
    noTone(PIN_BUZZER);
    clickBeepActive = false;
  }
}

void stopAlarm() {
  alarmRinging = false;
  alarmToneActive = false;
  clickBeepActive = false;
  noTone(PIN_BUZZER);
}

// =====================================================
// BASIC DRAWING
// =====================================================

void drawBorder() {
  display.drawRoundRect(1, 1, 282, 74, 7, CYAN);
}

void drawMenuIcon() {
  for (int i = 0; i < 3; i++) {
    display.fillRoundRect(9, 8 + i * 5, 12, 2, 1, LAVENDER);
  }
}

void drawDate() {
  display.fillRect(25, 5, 65, 21, BLACK);
  display.setFont(NULL);
  display.setTextSize(1);
  display.setTextColor(LAVENDER);

  struct tm t;
  if (getClockTime(t)) {
    char date[16], day[8];
    strftime(date, sizeof(date), "%b %d", &t);
    strftime(day, sizeof(day), "%a", &t);
    lastShownDay = t.tm_mday;
    lastShownMonth = t.tm_mon;
    lastShownYear = t.tm_year;

    display.setCursor(26, 6);
    display.print(date);
    display.setCursor(26, 16);
    display.print(day);
  } else {
    display.setCursor(26, 6);
    display.print("BLARE");
    display.setCursor(26, 16);
    display.print("CLOCK");
  }
}

void drawAlarmStatus() {
  display.fillRoundRect(232, 6, 42, 17, 5, BLACK);
  display.drawRoundRect(232, 6, 42, 17, 5, CYAN);
  display.setFont(NULL);
  display.setTextSize(1);
  display.setTextColor(alarmEnabled ? CYAN : GREY);
  display.setCursor(238, 11);
  display.print(alarmEnabled ? "ON" : "OFF");
}

// Only redraw the clock's own region.
void drawCurrentTime() {
  display.fillRect(86, 23, 120, 30, BLACK);

  struct tm t;
  display.setFont(NULL);

  if (!getClockTime(t)) {
    display.setTextSize(2);
    display.setTextColor(LAVENDER);
    display.setCursor(111, 30);
    display.print("--:--");
    return;
  }

  char text[12];
  formatCurrentTime(text, sizeof(text), t);
  lastShownHour = t.tm_hour;
  lastShownMinute = t.tm_min;

  display.setTextSize(3);
  display.setTextColor(LAVENDER);

  int width = strlen(text) * 18;
  int x = (SCREEN_W - width - 20) / 2;

  display.setCursor(x, 27);
  display.print(text);

  display.setTextSize(1);
  display.setCursor(x + width + 3, 39);
  display.print(t.tm_hour >= 12 ? "PM" : "AM");
}

void drawNextAlarm() {
  display.fillRect(93, 56, 105, 12, BLACK);
  display.setFont(NULL);
  display.setTextSize(1);
  display.setTextColor(CYAN);
  display.setCursor(95, 58);
  display.print("NEXT:");

  display.setTextColor(LAVENDER);
  display.setCursor(132, 58);

  if (alarmEnabled) {
    char text[16];
    formatAlarm(text, sizeof(text));
    display.print(text);
  } else {
    display.print("OFF");
  }
}

void drawHome() {
  display.fillScreen(BLACK);
  drawBorder();
  drawMenuIcon();
  drawDate();
  drawAlarmStatus();
  drawCurrentTime();
  drawNextAlarm();
}

// =====================================================
// MENU: REDRAW ONLY MENU ROWS WHEN SELECTION CHANGES
// =====================================================

void drawMenuRow(int i) {
  int y = 23 + i * 14;

  display.fillRect(8, y - 2, 268, 14, BLACK);

  if (i == menuSelection) {
    display.fillRoundRect(9, y - 2, 266, 13, 3, DARKGREY);
    display.drawRoundRect(9, y - 2, 266, 13, 3, CYAN);
    display.setTextColor(CYAN);
    display.setCursor(15, y);
    display.print(">");
  } else {
    display.setTextColor(LAVENDER);
    display.setCursor(15, y);
    display.print(" ");
  }

  display.setFont(NULL);
  display.setTextSize(1);
  display.setCursor(28, y);
  display.print(menuItems[i]);

  if (i == 0) {
    display.fillRect(220, y, 40, 9, BLACK);
    display.setCursor(224, y);
    display.setTextColor(alarmEnabled ? CYAN : GREY);
    display.print(alarmEnabled ? "ON" : "OFF");
  }
}

void drawMenu() {
  display.fillScreen(BLACK);
  drawBorder();

  display.setFont(NULL);
  display.setTextSize(1);
  display.setTextColor(CYAN);
  display.setCursor(12, 8);
  display.print("SETTINGS");

  for (int i = 0; i < MENU_COUNT; i++) {
    drawMenuRow(i);
  }
}

// =====================================================
// ALARM EDITING: UPDATE ONLY VALUE
// =====================================================

void drawEditValue() {
  display.fillRect(90, 25, 105, 32, BLACK);
  display.setFont(NULL);
  display.setTextSize(3);
  display.setTextColor(LAVENDER);

  int value = (currentScreen == SET_HOUR)
              ? alarmHour % 12 : alarmMinute;

  if (currentScreen == SET_HOUR && value == 0) value = 12;

  display.setCursor(115, 30);
  if (value < 10) display.print("0");
  display.print(value);
}

void drawEditScreen() {
  display.fillScreen(BLACK);
  drawBorder();

  display.setFont(NULL);
  display.setTextSize(1);
  display.setTextColor(CYAN);
  display.setCursor(12, 8);
  display.print(currentScreen == SET_HOUR
                 ? "SET ALARM - HOUR"
                 : "SET ALARM - MIN");

  drawEditValue();

  display.setTextSize(1);
  display.setTextColor(CYAN);
  display.setCursor(12, 59);
  display.print("UP/DOWN: ADJUST");
  display.setCursor(177, 59);
  display.print("RIGHT: OK");
}

void redrawScreen() {
  switch (currentScreen) {
    case HOME: drawHome(); break;
    case MENU: drawMenu(); break;
    case SET_HOUR:
    case SET_MINUTE: drawEditScreen(); break;
  }
  forceRedraw = false;
}

// =====================================================
// BUTTONS
// =====================================================

bool buttonPressed(int pin) {
  if (digitalRead(pin) != LOW) return false;
  if (millis() - lastButtonTime < BUTTON_DELAY) return false;

  delay(15);
  if (digitalRead(pin) != LOW) return false;

  lastButtonTime = millis();
  return true;
}

void handleButtons() {
  // Read all buttons; swapped UP and RIGHT wiring mapping above.
  bool up = buttonPressed(PIN_BTN_UP);
  bool down = buttonPressed(PIN_BTN_DOWN);
  bool left = buttonPressed(PIN_BTN_LEFT);
  bool right = buttonPressed(PIN_BTN_RIGHT);

  if (!(up || down || left || right)) return;

  if (alarmRinging) {
    stopAlarm();
    forceRedraw = true;
    return;
  }

  startClickBeep();

  switch (currentScreen) {
    case HOME:
      if (left || right) {
        currentScreen = MENU;
        menuSelection = 0;
        forceRedraw = true;
      }
      break;

    case MENU: {
      int oldSelection = menuSelection;

      if (up) {
        menuSelection = (menuSelection + MENU_COUNT - 1) % MENU_COUNT;
      }
      if (down) {
        menuSelection = (menuSelection + 1) % MENU_COUNT;
      }

      if (menuSelection != oldSelection) {
        drawMenuRow(oldSelection);
        drawMenuRow(menuSelection);
      }

      if (left) {
        currentScreen = HOME;
        forceRedraw = true;
      }

      if (right) {
        if (menuSelection == 0) {
          alarmEnabled = !alarmEnabled;
          drawMenuRow(menuSelection);
        } else if (menuSelection == 1) {
          currentScreen = SET_HOUR;
          forceRedraw = true;
        } else {
          currentScreen = HOME;
          forceRedraw = true;
        }
      }
      break;
    }

    case SET_HOUR:
      if (up) {
        alarmHour = (alarmHour + 1) % 24;
        drawEditValue();
      }
      if (down) {
        alarmHour = (alarmHour + 23) % 24;
        drawEditValue();
      }
      if (left) {
        currentScreen = MENU;
        forceRedraw = true;
      }
      if (right) {
        currentScreen = SET_MINUTE;
        forceRedraw = true;
      }
      break;

    case SET_MINUTE:
      if (up) {
        alarmMinute = (alarmMinute + 1) % 60;
        drawEditValue();
      }
      if (down) {
        alarmMinute = (alarmMinute + 59) % 60;
        drawEditValue();
      }
      if (left) {
        currentScreen = SET_HOUR;
        forceRedraw = true;
      }
      if (right) {
        currentScreen = HOME;
        forceRedraw = true;
      }
      break;
  }
}

// =====================================================
// ALARM: REPEAT UNTIL ANY BUTTON PRESSED
// =====================================================

void checkAlarm() {
  struct tm t;

  if (!alarmEnabled || alarmRinging) return;
  if (!getClockTime(t)) return;

  if (t.tm_hour == alarmHour &&
      t.tm_min == alarmMinute &&
      t.tm_sec == 0 &&
      lastTriggeredDay != t.tm_yday) {

    alarmRinging = true;
    alarmToneActive = false;
    lastTriggeredDay = t.tm_yday;
    lastAlarmBeep = 0;
  }
}

// =====================================================
// SETUP
// =====================================================

void setup() {
  pinMode(PIN_BTN_UP, INPUT_PULLUP);
  pinMode(PIN_BTN_DOWN, INPUT_PULLUP);
  pinMode(PIN_BTN_LEFT, INPUT_PULLUP);
  pinMode(PIN_BTN_RIGHT, INPUT_PULLUP);

  pinMode(PIN_BUZZER, OUTPUT);
  noTone(PIN_BUZZER);

  pinMode(PIN_TFT_BL, OUTPUT);
  digitalWrite(PIN_TFT_BL, LOW); // Active-low backlight ON

  // Keep the existing SCLK/MOSI wiring, but use hardware SPI instead of
  // bit-banged software SPI to speed up full-screen/menu drawing.
  SPI.begin(PIN_TFT_SCLK, -1, PIN_TFT_MOSI, PIN_TFT_CS);
  display.init(76, 284);
  display.setRotation(1);
  display.invertDisplay(false);
  display.fillScreen(BLACK);

  configTzTime(TIMEZONE, "pool.ntp.org", "time.nist.gov");

  if (strcmp(WIFI_SSID, "YOUR_WIFI_NAME") != 0) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  }

  forceRedraw = true;
}

// =====================================================
// LOOP
// =====================================================

void loop() {
  if (forceRedraw) redrawScreen();

  handleButtons();
  checkAlarm();
  updateBuzzer();

  // Check the clock frequently, but redraw only values that actually changed.
  // The displayed time has minute precision, so it will not be redrawn every
  // check or every second.
  if (currentScreen == HOME &&
      millis() - lastClockUpdate >= CLOCK_INTERVAL) {
    lastClockUpdate = millis();

    struct tm nowTime;
    if (getClockTime(nowTime)) {
      if (nowTime.tm_hour != lastShownHour ||
          nowTime.tm_min != lastShownMinute) {
        drawCurrentTime();
      }

      if (nowTime.tm_mday != lastShownDay ||
          nowTime.tm_mon != lastShownMonth ||
          nowTime.tm_year != lastShownYear) {
        drawDate();
      }
    }
  }

  delay(2);
}
