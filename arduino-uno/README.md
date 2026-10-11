# 📟 Arduino UNO Firmware: Air Quality Monitoring System

This directory contains the primary firmware for the **Arduino UNO** implementation of the Air Quality Monitoring System.

---

## 📋 Features

- **Continuous Analog Acquisition**: Reads the raw ADC signal from the MQ-135 gas sensor via analog pin `A0`.
- **Flicker-Free I2C LCD Display**: Refreshes readings on a 16x2 character display using space-padded string formatting instead of screen clearing.
- **Dynamic Threshold Alert**: Compares real-time values against a configurable threshold (`AIR_THRESHOLD = 400`) and engages an audible buzzer when pollution or hazardous gases are detected.
- **Serial Telemetry**: Emits formatted timestamped data streams over USB Serial at 9600 baud for debugging and logging.

---

## 🛠️ Hardware Requirements

| Component | Specification | Connection to UNO |
| :--- | :--- | :--- |
| **Microcontroller** | Arduino UNO (ATmega328P) | Main Controller |
| **Gas Sensor** | MQ-135 Air Quality Sensor Module | VCC $\to$ 5V, GND $\to$ GND, AOUT $\to$ A0 |
| **Alphanumeric Display** | 16x2 LCD with PCF8574 I2C Backpack | VCC $\to$ 5V, GND $\to$ GND, SDA $\to$ A4, SCL $\to$ A5 |
| **Audible Alarm** | 5V Active Buzzer | Positive (+) $\to$ D5, Negative (-) $\to$ GND |
| **Power Supply** | 5V USB (min 500mA) or 9V DC Adapter | Power source for MCU and sensor heater |

> [!IMPORTANT]
> The MQ-135 internal heating element draws between **150mA and 180mA** at 5V. Always supply power from a reliable USB port or regulated external power supply. Powering through low-current unpowered USB hubs may cause sensor voltage drops.

---

## 🔌 Pin Mapping

| Module Pin | Arduino UNO Pin | Function |
| :--- | :--- | :--- |
| **MQ-135 VCC** | 5V | 5V Power Supply |
| **MQ-135 GND** | GND | Ground Reference |
| **MQ-135 AOUT** | A0 | Analog Voltage Input (0 - 5V) |
| **LCD VCC** | 5V | Display Power |
| **LCD GND** | GND | Ground Reference |
| **LCD SDA** | A4 | I2C Data Line |
| **LCD SCL** | A5 | I2C Clock Line |
| **Buzzer (+)** | D5 | Digital Control Output |
| **Buzzer (-)** | GND | Ground Reference |

---

## 📦 Required Arduino Libraries

Before uploading the sketch, install the following library via the Arduino IDE Library Manager:

1. **`LiquidCrystal_I2C`**
   - Open **Arduino IDE** $\to$ **Sketch** $\to$ **Include Library** $\to$ **Manage Libraries...**
   - Search for: `LiquidCrystal_I2C` (by Frank de Brabander or Marco Schwartz)
   - Click **Install**.

---

## 🔍 Determining Your LCD I2C Address

Most 16x2 I2C modules use either `0x27` or `0x3F`. If your LCD backlight illuminates but no characters appear, verify the address using this scanner sketch:

```cpp
#include <Wire.h>

void setup() {
  Wire.begin();
  Serial.begin(9600);
  while (!Serial);
  Serial.println("\nI2C Scanner");
}

void loop() {
  byte error, address;
  int nDevices = 0;
  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println(" !");
      nDevices++;
    }
  }
  if (nDevices == 0) Serial.println("No I2C devices found\n");
  delay(5000);
}
```

If your address is `0x3F`, modify line 49 of `air_quality_monitor.ino`:
```cpp
LiquidCrystal_I2C lcd(0x3F, 16, 2);
```

---

## 🚀 Upload Instructions

1. Connect your **Arduino UNO** to your computer via USB.
2. Open `arduino-uno/air_quality_monitor.ino` in the **Arduino IDE**.
3. Under **Tools** $\to$ **Board**, select **Arduino Uno**.
4. Under **Tools** $\to$ **Port**, select the active COM port (e.g., `COM3`, `COM4`).
5. Click **Verify** (`Ctrl + R`) to ensure compilation succeeds.
6. Click **Upload** (`Ctrl + U`) to flash the firmware.
7. Open **Serial Monitor** (`Ctrl + Shift + M`) at **9600 baud** to view real-time data.

---

## 🌡️ Sensor Stabilization & Calibration Note

- **Initial Burn-In**: A brand-new MQ-135 requires **24 to 48 hours** of continuous preheating to eliminate manufacturing moisture and stabilize the SnO2 layer.
- **Power-On Stabilization**: Upon power-up, wait **2 to 3 minutes** before relying on threshold warnings.
- **Relative Measurement Notice**: The raw ADC values (0–1023) reflect relative electrical resistance changes. They indicate relative air quality degradation and must not be cited as certified ppm or scientific AQI without controlled gas chamber calibration.
