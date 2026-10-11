# 📌 Complete Pin Connections & Wiring Reference

This document provides explicit point-to-point pin wiring tables, voltage domain classifications, and interface rationale for both the baseline **Arduino UNO** implementation and the optional **NodeMCU ESP8266** IoT version.

---

## 1. Arduino UNO Wiring Reference (Primary System)

The Arduino UNO operates at a standard **5V TTL logic level**, providing natural compatibility with the MQ-135 sensor heater, PCF8574 I2C adapter, and 5V active buzzer.

### Point-to-Point Connection Table

| Source Device | Source Pin | Arduino UNO Pin | Wire Color (Rec.) | Signal Description / Notes |
| :--- | :--- | :--- | :--- | :--- |
| **MQ-135 Sensor** | **VCC** | **5V** | Red | Primary 5V supply for sensor heater circuit |
| **MQ-135 Sensor** | **GND** | **GND** | Black | Ground reference |
| **MQ-135 Sensor** | **AOUT (AO)** | **A0** | Yellow | Analog voltage signal (0 - 5V dynamic range) |
| **MQ-135 Sensor** | **DOUT (DO)** | *NC (No Connect)* | — | On-board comparator output (unused; digital threshold handled in MCU) |
| **16x2 I2C LCD** | **VCC** | **5V** | Red | Power for HD44780 controller & LED backlight |
| **16x2 I2C LCD** | **GND** | **GND** | Black | Ground reference |
| **16x2 I2C LCD** | **SDA** | **A4** (or dedicated SDA) | Blue | I2C Serial Data line (4.7k $\Omega$ pull-up on backpack) |
| **16x2 I2C LCD** | **SCL** | **A5** (or dedicated SCL) | Green | I2C Serial Clock line (4.7k $\Omega$ pull-up on backpack) |
| **Active Buzzer** | **Positive (+)** | **D5** | Orange | Digital output line (HIGH = Active, LOW = Silent) |
| **Active Buzzer** | **Negative (-)** | **GND** | Black | Ground return path |

---

## 2. NodeMCU ESP8266 Wiring Reference (Optional IoT Version)

The ESP8266 operates natively at **3.3V logic**, while the MQ-135 heater requires a **5V supply**. When powered via USB, the `VIN` pin provides 5V directly from the USB bus.

### Point-to-Point Connection Table

| Source Device | Source Pin | NodeMCU Pin | GPIO Pin | Signal Description / Notes |
| :--- | :--- | :--- | :--- | :--- |
| **MQ-135 Sensor** | **VCC** | **VIN** | — | 5V USB bus voltage (essential for internal sensor heater) |
| **MQ-135 Sensor** | **GND** | **GND** | — | Common system ground |
| **MQ-135 Sensor** | **AOUT (AO)** | **A0** | ADC0 / TOUT | Analog voltage input (0 - 3.3V max input on NodeMCU board) |
| **16x2 I2C LCD** | **VCC** | **VIN** | — | 5V supply for LCD backpack logic and backlight |
| **16x2 I2C LCD** | **GND** | **GND** | — | Common ground |
| **16x2 I2C LCD** | **SDA** | **D2** | GPIO 4 | ESP8266 Software/Hardware I2C Data line |
| **16x2 I2C LCD** | **SCL** | **D1** | GPIO 5 | ESP8266 Software/Hardware I2C Clock line |
| **Active Buzzer** | **Positive (+)** | **D5** | GPIO 14 | Digital trigger pin |
| **Active Buzzer** | **Negative (-)** | **GND** | — | Common ground return |

---

## ⚠️ Electrical & Voltage Safety Guidelines

1. **Common Ground Integrity**:
   - All ground pins (`GND` from sensor, LCD, buzzer, and microcontroller) must terminate at a common ground node to prevent ground loop noise and analog ADC measurement distortion.
2. **Current Budget on 5V Rail**:
   - Arduino UNO on-board 5V regulator can supply up to 450–500mA when powered via USB.
   - MQ-135 heater: ~160 mA.
   - 16x2 LCD with backlight: ~25 mA.
   - Buzzer active: ~25 mA.
   - Total peak current is $\approx 210\text{ mA}$, comfortably within the 500mA USB limit.
3. **ESP8266 ADC Input Limit**:
   - The bare ESP8266 silicon has an ADC input range of 0 to 1.0V. The standard NodeMCU development board contains an internal resistor divider ($220\text{ k}\Omega / 100\text{ k}\Omega$) that scales the board's external `A0` pin to 0–3.3V.
   - Do **NOT** feed voltages above 3.3V into NodeMCU `A0`. If the MQ-135 output exceeds 3.3V in high gas concentrations, add an external resistor divider.
