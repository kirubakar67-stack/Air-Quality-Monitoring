/*
 * ============================================================================
 * Project: Air Quality Monitoring System
 * Target Board: Arduino UNO (ATmega328P)
 * Author: Kirubakar CB
 * GitHub Repository: https://github.com/kirubakar67-stack/Air-Quality-Monitoring
 * ============================================================================
 *
 * DESCRIPTION:
 *   Reads analog gas concentrations using an MQ-135 semiconductor sensor module,
 *   displays continuous real-time values and qualitative safety status on a 16x2 
 *   I2C Liquid Crystal Display (LCD), and triggers an audible buzzer alarm when
 *   air contamination exceeds a user-defined threshold.
 *
 * HARDWARE CONNECTIONS:
 *   -----------------------------------------------------------------
 *   Component           Component Pin       Arduino UNO Pin
 *   -----------------------------------------------------------------
 *   MQ-135 Sensor       VCC                 5V (Requires ~150mA heater current)
 *   MQ-135 Sensor       GND                 GND
 *   MQ-135 Sensor       AOUT (Analog Out)   A0
 *   16x2 I2C LCD        VCC                 5V
 *   16x2 I2C LCD        GND                 GND
 *   16x2 I2C LCD        SDA                 A4 (Hardware I2C Data)
 *   16x2 I2C LCD        SCL                 A5 (Hardware I2C Clock)
 *   Buzzer              VCC / Positive (+)  D5 (Digital PWM Pin)
 *   Buzzer              GND / Negative (-)  GND
 *   -----------------------------------------------------------------
 *
 * SENSOR CALIBRATION & WARM-UP NOTE:
 *   The MQ-135 sensor uses an internal heating element and a Tin Dioxide (SnO2) 
 *   sensing layer. For stable and reliable baseline measurements:
 *     1. Initial burn-in: 24 to 48 hours of continuous power before first use.
 *     2. Warm-up time: At least 2-3 minutes after power-on before relying on alerts.
 *   Raw analog values (0 - 1023) represent relative gas concentration levels and
 *   must not be treated as certified PPM or standardized AQI without laboratory
 *   curve fitting and controlled baseline zeroing.
 * ============================================================================
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ============================================================================
// CONFIGURATION & CONSTANTS
// ============================================================================

// I2C LCD Address: Typically 0x27 or 0x3F for PCF8574 backpacks.
// If your display does not show text, run an I2C scanner sketch to verify.
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin Definitions
const int MQ135_PIN  = A0;  // Analog input from MQ-135 sensor
const int BUZZER_PIN = 5;   // Digital output driving the buzzer

// Configurable Alert Threshold (ADC range: 0 - 1023)
// Typical clean room ambient baseline is between 150 - 350.
// Adjust this threshold based on your specific ambient baseline and testing.
const int AIR_THRESHOLD = 400;

// Loop sampling interval in milliseconds
const unsigned long SAMPLE_INTERVAL_MS = 500;

// Global Variables
int sensorValue = 0;

// ============================================================================
// SETUP FUNCTION
// ============================================================================
void setup() {
  // Initialize Serial Monitor communication for logging and debugging
  Serial.begin(9600);
  Serial.println(F("========================================"));
  Serial.println(F("  Air Quality Monitoring System - UNO   "));
  Serial.println(F("========================================"));

  // Configure hardware pin modes
  pinMode(MQ135_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Ensure buzzer is silenced at startup
  digitalWrite(BUZZER_PIN, LOW);

  // Initialize 16x2 I2C LCD
  lcd.init();
  lcd.backlight();

  // Display initial splash screen
  lcd.setCursor(0, 0);
  lcd.print("Air Quality");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");

  Serial.println(F("[INFO] System initializing. Warming up MQ-135 sensor..."));

  // Stabilization pause (allows basic heater stabilization on power-up)
  delay(2000);

  // Clear splash text and prepare operational screen
  lcd.clear();
  Serial.println(F("[INFO] System operational. Monitoring started."));
  Serial.print(F("[INFO] Configured Warning Threshold: "));
  Serial.println(AIR_THRESHOLD);
}

// ============================================================================
// MAIN LOOP
// ============================================================================
void loop() {
  // Read analog voltage from MQ-135 sensor (10-bit ADC: 0 to 1023)
  sensorValue = analogRead(MQ135_PIN);

  // Evaluate air quality against configured threshold
  bool warning = (sensorValue > AIR_THRESHOLD);

  // Output real-time metrics to Serial Monitor
  Serial.print(F("MQ-135 Raw ADC: "));
  Serial.print(sensorValue);
  Serial.print(F(" | Threshold: "));
  Serial.print(AIR_THRESHOLD);
  Serial.print(F(" | Status: "));
  Serial.println(warning ? F("WARNING (Contamination Detected)") : F("NORMAL (Safe Ambient)"));

  // Update LCD Line 1: Real-time sensor reading
  // Trailing spaces prevent character ghosting without requiring lcd.clear()
  lcd.setCursor(0, 0);
  lcd.print("Sensor: ");
  lcd.print(sensorValue);
  lcd.print("    ");

  // Update LCD Line 2: Status indicator & buzzer alarm control
  lcd.setCursor(0, 1);
  if (warning) {
    lcd.print("Status: WARNING ");
    digitalWrite(BUZZER_PIN, HIGH);  // Activate audible buzzer alert
  } else {
    lcd.print("Status: NORMAL  ");
    digitalWrite(BUZZER_PIN, LOW);   // Deactivate buzzer
  }

  // Delay between measurement samples
  delay(SAMPLE_INTERVAL_MS);
}
