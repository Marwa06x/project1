#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

constexpr int TFT_SCLK = 12;
constexpr int TFT_MOSI = 11;
constexpr int TFT_CS   = 4;
constexpr int TFT_DC   = 5;
constexpr int TFT_RST  = 6;

constexpr int HORZ_PIN = 17;
constexpr int VERT_PIN = 2;  
constexpr int SEL_PIN  = 7;  

constexpr bool UP_IS_LOW = true;
constexpr int MOVE_THRESHOLD = 450;
constexpr int CENTER_ZONE = 180;
constexpr int PAGE_COUNT = 3;

Adafruit_ST7789 screen(TFT_CS, TFT_DC, TFT_RST);

const char* const pageNames[PAGE_COUNT] = {
    "Pressure hPa", "Pressure mbar", "Temperature"
};

const char* const readings[PAGE_COUNT] = {
    "1013.2 hPa", "1013.2 mbar", "21.4 C"
};

int selectedPage = 0;
int joystickCenter = 0;
bool pageOpen = false;
bool readyToMove = true;

bool lastRawPress = false;
bool stablePress = false;
unsigned long buttonChangedAt = 0;

void drawMenu() {
    screen.fillScreen(ST77XX_BLACK);
    screen.setTextSize(2);
    screen.setTextColor(ST77XX_WHITE);
    screen.setCursor(8, 10);
    screen.print("Weather station");

    for (int i = 0; i < PAGE_COUNT; i++) {
        const bool highlighted = (i == selectedPage);
        screen.setCursor(8, 44 + i * 24);
        screen.setTextColor(highlighted ? ST77XX_YELLOW : ST77XX_WHITE);
        screen.print(highlighted ? "> " : "  ");
        screen.print(pageNames[i]);
    }
}

void drawPage() {
    screen.fillScreen(ST77XX_BLACK);
    screen.setTextColor(ST77XX_WHITE);
    screen.setTextSize(2);
    screen.setCursor(8, 10);
    screen.print(pageNames[selectedPage]);

    screen.setCursor(8, 46);
    screen.setTextSize(selectedPage == 1 ? 4 : 3);
    screen.print(readings[selectedPage]);

    screen.setCursor(8, 100);
    screen.setTextSize(1);
    screen.print("Press to go back");
}

bool buttonClicked() {
    const bool rawPress = digitalRead(SEL_PIN) == LOW;
    const unsigned long now = millis();

    if (rawPress != lastRawPress) {
        lastRawPress = rawPress;
        buttonChangedAt = now;
    }

    
    if (rawPress != stablePress && now - buttonChangedAt >= 30) {
        stablePress = rawPress;
        return stablePress;
    }

    return false;
}

void setup() {
    SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
    screen.init(240, 320);
    screen.setRotation(1);

    pinMode(SEL_PIN, INPUT_PULLUP);
    analogReadResolution(12);
    joystickCenter = analogRead(VERT_PIN);

    drawMenu();
}

void loop() {
    const int offset = analogRead(VERT_PIN) - joystickCenter;

    
    if (abs(offset) < CENTER_ZONE) {
        readyToMove = true;
    }

    if (!pageOpen && readyToMove && abs(offset) > MOVE_THRESHOLD) {
        const bool movingUp = UP_IS_LOW ? offset < 0 : offset > 0;
        const int step = movingUp ? -1 : 1;
        selectedPage = (selectedPage + step + PAGE_COUNT) % PAGE_COUNT;
        readyToMove = false;
        drawMenu();
    }

    if (buttonClicked()) {
        pageOpen = !pageOpen;

        if (pageOpen) {
            drawPage();
        } else {
            drawMenu();
        }
    }
}