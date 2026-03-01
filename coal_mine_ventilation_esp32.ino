#include <Wire.h>
#include <U8g2lib.h>
U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
#define MQ7_PIN 34
#define MQ4_PIN 35
#define FAN_PIN 27
#define BUZZER_PIN 26
#define HEATER_MQ7 25
#define HEATER_MQ4 33
#define PWM_FREQ 5000
#define PWM_RESOLUTION 8
unsigned long lastSwitch = 0;
bool highHeat = true;
unsigned long startTime = 0;

void setup() {
  Serial.begin(115200);
  
  pinMode(MQ7_PIN, INPUT);
  pinMode(MQ4_PIN, INPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(HEATER_MQ4, OUTPUT);
  
  digitalWrite(FAN_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  
  ledcAttach(HEATER_MQ7, PWM_FREQ, PWM_RESOLUTION);
  
  digitalWrite(HEATER_MQ4, HIGH);
  ledcWrite(HEATER_MQ7, 255);  // Start HIGH heat
  
  display.begin();
  display.clearBuffer();
  display.setFont(u8g2_font_6x10_tr);
  display.drawStr(10, 30, "Gas Detection");
  display.drawStr(15, 45, "Reading Mode");
  display.sendBuffer();
  
  startTime = millis();
  lastSwitch = startTime;
  
  Serial.println(F("\n╔════════════════════════════════════╗"));
  Serial.println(F("║  GAS SENSOR - READING MODE ✓       ║"));
  Serial.println(F("╚════════════════════════════════════╝"));
  Serial.println(F("\nSensors detected and working!"));
  Serial.println(F("Monitoring values...\n"));
  
  delay(2000);
}

void loop() {
  unsigned long now = millis();
  
    if (highHeat && (now - lastSwitch >= 60000)) {
    ledcWrite(HEATER_MQ7, 72);  // LOW heat (1.4V)
    highHeat = false;
    lastSwitch = now;
    Serial.println(F("\n→ MQ7: Switched to LOW heat (1.4V) - Reading mode\n"));
  } 
  else if (!highHeat && (now - lastSwitch >= 90000)) {
    ledcWrite(HEATER_MQ7, 255);  // HIGH heat (5V)
    highHeat = true;
    lastSwitch = now;
    Serial.println(F("\n→ MQ7: Switched to HIGH heat (5V) - Cleaning mode\n"));
  }
  
    int mq7_value = analogRead(MQ7_PIN);
  int mq4_value = analogRead(MQ4_PIN);
  
    unsigned long timeInPhase = now - lastSwitch;
  unsigned long phaseTime = highHeat ? 60000 : 90000;
  int secondsLeft = (phaseTime - timeInPhase) / 1000;
  
 
  Serial.print(F("MQ7: "));
  Serial.print(mq7_value);
  Serial.print(F(" | MQ4: "));
  Serial.print(mq4_value);
  Serial.print(F(" | Heat: "));
  Serial.print(highHeat ? F("HIGH (5V)") : F("LOW (1.4V)"));
  Serial.print(F(" | "));
  Serial.print(secondsLeft);
  Serial.println(F("s"));
  
    updateDisplay(mq7_value, mq4_value, secondsLeft);
  
  delay(500);
}

void updateDisplay(int mq7, int mq4, int secondsLeft) {
  display.clearBuffer();
  
   display.setFont(u8g2_font_6x10_tr);
  display.drawStr(5, 10, "Gas Detection");
  display.drawHLine(0, 12, 128);
 
  char line1[25];
  char line2[25];
  sprintf(line1, "CO  MQ7: %d", mq7);
  sprintf(line2, "CH4 MQ4: %d", mq4);
  
  display.setFont(u8g2_font_5x7_tr);
  display.drawStr(2, 24, line1);
  display.drawStr(2, 34, line2);
  
  display.drawHLine(0, 38, 128);
  
    display.setFont(u8g2_font_6x10_tr);
  display.drawStr(20, 50, "READING MODE");
  
  char heaterStr[25];
  sprintf(heaterStr, "Heat: %s (%ds)", highHeat ? "5V" : "1.4V", secondsLeft);
  display.drawStr(10, 62, heaterStr);
  
  display.sendBuffer();
}
