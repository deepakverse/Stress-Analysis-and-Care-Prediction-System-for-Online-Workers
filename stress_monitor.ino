/*
 * Stress Analysis and Care Prediction System for Online Workers
 * --------------------------------------------------------------
 * Controller : Arduino UNO
 * IDE        : Arduino IDE
 *
 * Sensors:
 *   GSR / Skin Response : A0
 *   Heart Rate          : A1
 *   Temperature         : A2
 *   SpO2                : A3
 *
 * Outputs:
 *   LCD 16x2            : RS=7, EN=8, D4=9, D5=10, D6=11, D7=12
 *   Alarm/Buzzer        : D6
 *
 * NOTE:
 * This is a GitHub-ready reference implementation based on the
 * hardware and functional description in the project report.
 * Sensor threshold values are demonstration values and should be
 * calibrated for the actual sensors before real-world use.
 */

#include <LiquidCrystal.h>

// LCD pins: RS, EN, D4, D5, D6, D7
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// Analog sensor pins
const byte GSR_PIN       = A0;
const byte HEART_RATE_PIN = A1;
const byte TEMP_PIN      = A2;
const byte SPO2_PIN      = A3;

// Alarm
const byte BUZZER_PIN = 6;

// Demonstration thresholds
const int GSR_THRESHOLD   = 600;
const int PULSE_THRESHOLD = 600;
const float TEMP_THRESHOLD = 37.5;
const int SPO2_THRESHOLD  = 500;

void setup() {
  Serial.begin(9600);

  lcd.begin(16, 2);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  lcd.setCursor(0, 0);
  lcd.print("Stress Monitor");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Read sensor values
  int gsrValue   = analogRead(GSR_PIN);
  int heartValue = analogRead(HEART_RATE_PIN);
  int tempRaw    = analogRead(TEMP_PIN);
  int spo2Value  = analogRead(SPO2_PIN);

  // Example LM35 temperature conversion
  float voltage = tempRaw * (5.0 / 1023.0);
  float temperatureC = voltage * 100.0;

  // Calculate a simple stress score
  int stressScore = calculateStress(
    gsrValue,
    heartValue,
    temperatureC,
    spo2Value
  );

  // Determine stress status
  const char* status;

  if (stressScore >= 60) {
    status = "HIGH";
    digitalWrite(BUZZER_PIN, HIGH);
  }
  else if (stressScore >= 30) {
    status = "MODERATE";
    digitalWrite(BUZZER_PIN, LOW);
  }
  else {
    status = "NORMAL";
    digitalWrite(BUZZER_PIN, LOW);
  }

  // Display result
  displayResult(stressScore, status);

  // Send data to Serial Monitor for IoT/cloud integration
  printSensorData(
    gsrValue,
    heartValue,
    temperatureC,
    spo2Value,
    stressScore,
    status
  );

  delay(1000);
}

// --------------------------------------------------------------
// Stress score calculation
// --------------------------------------------------------------
int calculateStress(int gsr, int heart, float temperature, int spo2) {
  int score = 0;

  if (gsr > GSR_THRESHOLD)
    score += 30;

  if (heart > PULSE_THRESHOLD)
    score += 30;

  if (temperature > TEMP_THRESHOLD)
    score += 20;

  if (spo2 < SPO2_THRESHOLD)
    score += 20;

  return constrain(score, 0, 100);
}

// --------------------------------------------------------------
// LCD display
// --------------------------------------------------------------
void displayResult(int score, const char* status) {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Stress: ");
  lcd.print(score);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("Status: ");
  lcd.print(status);
}

// --------------------------------------------------------------
// Serial output
// --------------------------------------------------------------
void printSensorData(
  int gsr,
  int heart,
  float temperature,
  int spo2,
  int stress,
  const char* status
) {
  Serial.print("GSR=");
  Serial.print(gsr);

  Serial.print(", HeartRateSensor=");
  Serial.print(heart);

  Serial.print(", Temperature=");
  Serial.print(temperature, 1);
  Serial.print("C");

  Serial.print(", SpO2Sensor=");
  Serial.print(spo2);

  Serial.print(", Stress=");
  Serial.print(stress);
  Serial.print("%");

  Serial.print(", Status=");
  Serial.println(status);
}
