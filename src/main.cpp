#include <Arduino.h>

constexpr int HORZ_PIN = 17;
constexpr int VERT_PIN = 2;
constexpr int SEL_PIN  = 7;

void setup()
{
    Serial.begin(115200);
    pinMode(SEL_PIN, INPUT_PULLUP);
    analogReadResolution(12);
}

void loop()
{
    int horizontal = analogRead(HORZ_PIN);
    int vertical   = analogRead(VERT_PIN);
    bool pressed   = digitalRead(SEL_PIN) == LOW;

    Serial.print("Horizontal: ");
    Serial.print(horizontal);

    Serial.print("  Vertical: ");
    Serial.print(vertical);

    Serial.print("  Button: ");
    Serial.println(pressed ? "PRESSED" : "released");

    delay(200);
}