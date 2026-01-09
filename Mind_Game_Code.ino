#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define NUM_LEDS 5
#define NUM_LEVELS 10

int ledPins[NUM_LEDS] = {2, 3, 4, 5, 6};
int buttonPins[NUM_LEDS] = {7, 8, 9, 10, 11};

// RGB LED (COMMON ANODE)
int redPin   = 12;   // RED = FAIL
int greenPin = 13;   // GREEN = SUCCESS
int bluePin  = A1;   // BLUE = CELEBRATION

int buzzer = A0;

// Unique tones for each LED
int ledTones[NUM_LEDS] = {400, 500, 600, 700, 800};

int sequence[NUM_LEVELS];

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WELCOME TO");
  delay(1500);
  lcd.clear();
  lcd.print("Mind Game");
  delay(2000);
  lcd.clear();
  lcd.print("ARE YOU READY?");
  delay(2500);
  lcd.clear();

  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(ledPins[i], OUTPUT);
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(buzzer, OUTPUT);

  rgbOff(); // Turn RGB OFF
  randomSeed(analogRead(0));
}

void loop() {
  for (int level = 1; level <= NUM_LEVELS; level++) {

    generateSequence();  // ✅ New random sequence every level

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Level: ");
    lcd.print(level);
    delay(800);

    showSequence(level);

    if (!checkPlayer(level)) {
      // ❌ FAILED
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Level ");
      lcd.print(level);
      lcd.setCursor(0, 1);
      lcd.print("FAILED!");

      rgbRed();        // 🔴 RGB RED
      lostSound();
      delay(1200);
      rgbOff();

      level = 0;  // Restart from Level 1
      continue;
    }

    // ✔ SUCCESS
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Level ");
    lcd.print(level);
    lcd.setCursor(0, 1);
    lcd.print("SUCCESS!");

    rgbGreen();       // 🟢 RGB GREEN
    successSound();
    delay(1200);
    rgbOff();
  }

  // 🎉 PLAYER COMPLETED ALL LEVELS
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("All Levels Clear");
  lcd.setCursor(0, 1);
  lcd.print("WINNER!");

  celebrationMode();
  while (1);
}

// --------------------------------------------
void generateSequence() {
  for (int i = 0; i < NUM_LEVELS; i++) {
    sequence[i] = random(NUM_LEDS);
  }
}

// --------------------------------------------
void showSequence(int level) {
  for (int i = 0; i < level; i++) {
    int led = sequence[i];

    digitalWrite(ledPins[led], HIGH);
    tone(buzzer, ledTones[led], 300);
    delay(350);

    digitalWrite(ledPins[led], LOW);
    delay(250);
  }
}

// --------------------------------------------
bool checkPlayer(int level) {
  for (int i = 0; i < level; i++) {
    int pressed = waitForButtonPress();
    if (pressed != sequence[i]) {
      return false;
    }
  }
  return true;
}

// --------------------------------------------
int waitForButtonPress() {
  while (true) {
    for (int i = 0; i < NUM_LEDS; i++) {
      if (digitalRead(buttonPins[i]) == LOW) {

        digitalWrite(ledPins[i], HIGH);
        tone(buzzer, ledTones[i], 250);
        delay(250);
        digitalWrite(ledPins[i], LOW);

        return i;
      }
    }
  }
}

// --------------------------------------------
void successSound() {
  tone(buzzer, 1000, 200);
  delay(150);
  tone(buzzer, 1500, 200);
}

void lostSound() {
  tone(buzzer, 300, 300);
  delay(300);
  tone(buzzer, 200, 300);
}

// --------------------------------------------
void celebrationMode() {
  for (int x = 0; x < 30; x++) {
    
    rgbRed();
    tone(buzzer, 800, 100);
    delay(120);

    rgbGreen();
    tone(buzzer, 1000, 100);
    delay(120);

    rgbBlue();
    tone(buzzer, 1200, 100);
    delay(120);

    rgbOff();
  }
}

// --------------------------------------------
// ✅ COMMON ANODE RGB CONTROL (LOW = ON, HIGH = OFF)

void rgbRed() {
  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, HIGH);
  digitalWrite(bluePin, HIGH);
}

void rgbGreen() {
  digitalWrite(redPin, HIGH);
  digitalWrite(greenPin, LOW);
  digitalWrite(bluePin, HIGH);
}

void rgbBlue() {
  digitalWrite(redPin, HIGH);
  digitalWrite(greenPin, HIGH);
  digitalWrite(bluePin, LOW);
}

void rgbOff() {
  digitalWrite(redPin, HIGH);
  digitalWrite(greenPin, HIGH);
  digitalWrite(bluePin, HIGH);
}
