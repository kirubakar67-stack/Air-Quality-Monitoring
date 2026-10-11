/*
 * ============================================================================
 * Project: Air Quality Monitoring System - IoT Extension
 * Target Board: NodeMCU ESP8266 (ESP-12E Module)
 * Author: Kirubakar CB
 * GitHub Repository: https://github.com/kirubakar67-stack/Air-Quality-Monitoring
 * ============================================================================
 *
 * DESCRIPTION:
 *   Extends the baseline air quality monitor with cloud IoT capabilities.
 *   Reads gas concentration data from an MQ-135 sensor, refreshes a 16x2 I2C 
 *   LCD, controls an audible buzzer alarm upon threshold violation, and transmits
 *   telemetry periodically to ThingSpeak IoT Cloud via 2.4 GHz Wi-Fi.
 *
 * ESP8266 NODEMCU PIN CONNECTIONS:
 *   -----------------------------------------------------------------
 *   Component           Component Pin       NodeMCU ESP8266 Pin
 *   -----------------------------------------------------------------
 *   MQ-135 Sensor       VCC                 VIN (5V Rail, sensor heater needs 5V)
 *   MQ-135 Sensor       GND                 GND
 *   MQ-135 Sensor       AOUT (Analog Out)   A0 (ADC0, Max 3.3V input on NodeMCU)
 *   16x2 I2C LCD        VCC                 VIN (5V Rail for standard 5V backpack)
 *   16x2 I2C LCD        GND                 GND
 *   16x2 I2C LCD        SDA                 D2 (GPIO 4 - Software/Hardware I2C SDA)
 *   16x2 I2C LCD        SCL                 D1 (GPIO 5 - Software/Hardware I2C SCL)
 *   Buzzer              Positive (+)        D5 (GPIO 14)
 *   Buzzer              Negative (-)        GND
 *   -----------------------------------------------------------------
 *
 * IMPORTANT VOLTAGE CONSIDERATION:
 *   NodeMCU boards have an onboard resistor divider (220k/100k) stepping down 
 *   0-3.3V at the board's A0 pin to 0-1.0V for the raw ESP8266 chip. 
 *   Because MQ-135 is powered by 5V, if high gas concentrations produce >3.3V, 
 *   an external voltage divider is recommended between MQ-135 AOUT and NodeMCU A0.
 *
 * CREDENTIAL SETUP:
 *   Copy 'config.example.h' to 'config.h' in the same folder and enter your
 *   Wi-Fi credentials and ThingSpeak API keys. 'config.h' is git-ignored.
 * ============================================================================
 */

#include <ESP8266WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ThingSpeak.h>

// Include credentials. If config.h is not yet created, fall back to config.example.h.
#if __has_include("config.h")
  #include "config.h"
#else
  #include "config.example.h"
  #warning "Using config.example.h! Copy config.example.h to config.h and add your private credentials."
#endif

// ============================================================================
// HARDWARE DEFINITIONS & CONSTANTS
// ============================================================================

// I2C Pin Assignment for NodeMCU ESP8266
const int I2C_SDA_PIN = D2;  // GPIO 4
const int I2C_SCL_PIN = D1;  // GPIO 5

// Sensor and Actuator Pins
const int MQ135_PIN   = A0;  // NodeMCU ADC0 (0 - 3.3V input range)
const int BUZZER_PIN  = D5;  // GPIO 14

// Configurable Warning Threshold (0 - 1023 ADC range)
const int AIR_THRESHOLD = 400;

// Timing intervals (Milliseconds)
const unsigned long SENSOR_READ_INTERVAL_MS   = 500;     // Local UI/Buzzer loop
const unsigned long THINGSPEAK_POST_INTERVAL_MS = 20000; // Cloud post interval (ThingSpeak min is 15s)

// ============================================================================
// OBJECT INSTANTIATIONS
// ============================================================================

// Standard I2C address is 0x27 (or 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);
WiFiClient client;

// State Variables
unsigned long lastSensorReadTime = 0;
unsigned long lastCloudPostTime  = 0;
int currentSensorValue           = 0;
bool isWarningActive             = false;

// ============================================================================
// FUNCTION DECLARATIONS
// ============================================================================
void connectWiFi();
void readSensorAndUpdateLocalUI();
void transmitToThingSpeak();

// ============================================================================
// SETUP FUNCTION
// ============================================================================
void setup() {
  Serial.begin(115200);
  delay(200);

  Serial.println(F("\n=============================================="));
  Serial.println(F("  Air Quality Monitor - NodeMCU IoT Version   "));
  Serial.println(F("=============================================="));

  // Initialize hardware pins
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // Initialize I2C bus with ESP8266 explicit pins
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Air Monitor IoT");
  lcd.setCursor(0, 1);
  lcd.print("Connecting WiFi");

  // Establish Wi-Fi connection
  connectWiFi();

  // Initialize ThingSpeak client
  ThingSpeak.begin(client);

  // Splash screen transition
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected!");
  lcd.setCursor(0, 1);
  lcd.print("IP: ");
  lcd.print(WiFi.localIP().toString().substring(0, 12));
  delay(2000);
  lcd.clear();

  Serial.println(F("[INFO] System operational. Commencing monitoring loop."));
}

// ============================================================================
// MAIN LOOP
// ============================================================================
void loop() {
  unsigned long currentMillis = millis();

  // Maintain Wi-Fi connectivity
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  // Task 1: High-frequency local sensor acquisition & UI refresh (Every 500ms)
  if (currentMillis - lastSensorReadTime >= SENSOR_READ_INTERVAL_MS) {
    lastSensorReadTime = currentMillis;
    readSensorAndUpdateLocalUI();
  }

  // Task 2: Cloud Telemetry Posting to ThingSpeak (Every 20s)
  if (currentMillis - lastCloudPostTime >= THINGSPEAK_POST_INTERVAL_MS) {
    lastCloudPostTime = currentMillis;
    transmitToThingSpeak();
  }
}

// ============================================================================
// LOCAL UI & SENSOR HANDLING
// ============================================================================
void readSensorAndUpdateLocalUI() {
  currentSensorValue = analogRead(MQ135_PIN);
  isWarningActive    = (currentSensorValue > AIR_THRESHOLD);

  // Update Buzzer Output
  digitalWrite(BUZZER_PIN, isWarningActive ? HIGH : LOW);

  // Output to Serial Monitor
  Serial.print(F("[TELEMETRY] Sensor ADC: "));
  Serial.print(currentSensorValue);
  Serial.print(F(" | Threshold: "));
  Serial.print(AIR_THRESHOLD);
  Serial.print(F(" | Status: "));
  Serial.println(isWarningActive ? F("WARNING") : F("NORMAL"));

  // Update LCD Row 1: Sensor Reading (trailing spaces prevent ghosting)
  lcd.setCursor(0, 0);
  lcd.print("Sensor: ");
  lcd.print(currentSensorValue);
  lcd.print("    ");

  // Update LCD Row 2: Status & Wi-Fi indicator
  lcd.setCursor(0, 1);
  if (isWarningActive) {
    lcd.print("WARNG  [WiFi:OK]");
  } else {
    lcd.print("NORML  [WiFi:OK]");
  }
}

// ============================================================================
// CLOUD TELEMETRY TRANSMISSION (THINGSPEAK)
// ============================================================================
void transmitToThingSpeak() {
  // Guard check against unconfigured credentials
  if (THINGSPEAK_CHANNEL_ID == 0 || strcmp(THINGSPEAK_WRITE_API_KEY, "YOUR_WRITE_API_KEY") == 0) {
    Serial.println(F("[WARNING] ThingSpeak not configured. Skipping upload."));
    return;
  }

  // Populate ThingSpeak fields
  // Field 1: Raw MQ-135 Reading
  // Field 2: Status flag (0 = Safe, 1 = Warning)
  ThingSpeak.setField(1, currentSensorValue);
  ThingSpeak.setField(2, isWarningActive ? 1 : 0);

  Serial.println(F("[CLOUD] Transmitting telemetry to ThingSpeak..."));
  int httpCode = ThingSpeak.writeFields(THINGSPEAK_CHANNEL_ID, THINGSPEAK_WRITE_API_KEY);

  if (httpCode == 200) {
    Serial.println(F("[CLOUD] Telemetry successfully published to ThingSpeak (HTTP 200)."));
  } else {
    Serial.print(F("[CLOUD ERROR] ThingSpeak update failed. HTTP Error Code: "));
    Serial.println(httpCode);
  }
}

// ============================================================================
// WI-FI CONNECTION HELPER
// ============================================================================
void connectWiFi() {
  if (strcmp(WIFI_SSID, "YOUR_WIFI_SSID") == 0) {
    Serial.println(F("[WARNING] Default Wi-Fi credentials detected. Please edit config.h."));
    return;
  }

  Serial.print(F("[WIFI] Connecting to SSID: "));
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 25) {
    delay(500);
    Serial.print(F("."));
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(F("\n[WIFI] Connected successfully!"));
    Serial.print(F("[WIFI] IP Address: "));
    Serial.println(WiFi.localIP());
  } else {
    Serial.println(F("\n[WIFI] Connection failed. Retrying in background."));
  }
}
