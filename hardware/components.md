# 🔩 Hardware Components & Specifications

This document outlines the complete Bill of Materials (BOM), electrical ratings, and functional descriptions for all components used in the **Air Quality Monitoring System**.

---

## 📋 Bill of Materials (BOM)

| Item # | Component | Model / Type | Quantity | Operating Voltage | Current Consumption | Primary Purpose |
| :---: | :--- | :--- | :---: | :---: | :---: | :--- |
| **1** | Main Microcontroller | **Arduino UNO R3** (ATmega328P) | 1 | 5V DC (USB/Barrel) | ~45 - 50 mA | Sensor sampling, threshold evaluation, display drive |
| **2** | Gas Sensor Module | **MQ-135** (SnO2 semiconductor) | 1 | 5V DC $\pm$ 0.1V | ~150 - 180 mA (Heater) | Detection of NH3, NOx, Alcohol, Benzene, Smoke, CO2 |
| **3** | Display Module | **16x2 Character LCD** | 1 | 5V DC | ~20 - 30 mA | Visual output of real-time readings and status |
| **4** | I2C Backpack | **PCF8574 I2C Adapter** | 1 | 2.5V - 6V DC | ~2.5 mA | Reduces LCD pin requirement from 16 to 2 (SDA/SCL) |
| **5** | Audible Alarm | **Active Buzzer** (5V DC) | 1 | 3.5V - 5.5V DC | ~25 - 30 mA | Acoustic warning on threshold violation |
| **6** | Prototyping Platform | **830-Point Breadboard** | 1 | N/A | N/A | Solderless component interconnection |
| **7** | Interconnect Wires | **Dupont Jumper Wires** (M-M, M-F) | ~15 | N/A | N/A | Circuit wiring |
| **8** | Power Cable | **USB Type-B Cable** | 1 | 5V DC (USB) | Min. 500 mA rated | Host PC communication and main power |
| **9** | *(Optional IoT)* | **NodeMCU ESP8266 (ESP-12E)** | 1 | 3.3V (5V via USB/VIN) | ~80 - 170 mA (Wi-Fi TX) | Wi-Fi telemetry uplink to ThingSpeak cloud |

---

## 🔬 Component Details & Operational Profiles

### 1. MQ-135 Air Quality Sensor Module
- **Sensing Material**: Tin Dioxide ($\text{SnO}_2$), an n-type semiconductor whose surface electrical conductivity increases as reducing gas concentrations rise.
- **Target Gases**: Ammonia ($\text{NH}_3$), Nitrogen Oxides ($\text{NO}_x$), Alcohol, Benzene, Smoke, Carbon Dioxide ($\text{CO}_2$).
- **Heater Resistance ($R_H$)**: $31\ \Omega \pm 3\ \Omega$ at room temperature.
- **Heater Consumption ($P_H$)**: $\le 900\text{ mW}$ ($5\text{V} \times 160\text{mA}$).
- **Load Resistance ($R_L$)**: Adjustable on-board potentiometer ($10\text{ k}\Omega - 47\text{ k}\Omega$).
- **Sensitivity ($S$)**: $\frac{R_s(\text{in clean air})}{R_s(\text{in 100ppm NH}_3)} \ge 5$.
- **Preheating Time**:
  - Initial burn-in for pristine units: **24 to 48 hours**.
  - Operational stabilization: **120 to 180 seconds** per power cycle.

> [!CAUTION]
> The internal heating element becomes warm to the touch during normal operation. This is standard thermal behavior required for the catalytic gas reaction on the tin dioxide surface. Ensure adequate air ventilation around the sensor casing.

---

### 2. 16x2 Character LCD with PCF8574 I2C Adapter
- **Display Matrix**: 16 characters $\times$ 2 rows with $5\times 8$ dot-matrix font.
- **Controller**: HD44780 or equivalent.
- **I2C Expander**: NXP PCF8574 / PCF8574A 8-bit quasi-bidirectional I/O expander.
- **Interface Protocol**: Standard 100 kHz I2C (Inter-Integrated Circuit).
- **Default Addresses**:
  - PCF8574: `0x27` (A0, A1, A2 unbridged).
  - PCF8574A: `0x3F` (A0, A1, A2 unbridged).
- **Contrast Adjustment**: Integrated 10 k$\Omega$ single-turn trimpot on the rear backpack.

---

### 3. Active 5V Buzzer
- **Type**: Electromagnetic active piezo buzzer with integrated internal oscillator.
- **Operating Voltage**: 3.5V to 5.5V DC.
- **Resonant Frequency**: ~2300 Hz $\pm$ 300 Hz.
- **Sound Pressure Level (SPL)**: $\ge 85\text{ dB}$ at 10 cm.
- **Control Interface**: Driven directly via digital pin output (`HIGH` = tone active, `LOW` = silent).
- **Current Draw**: Approximately 25mA when energized. Driven safely within the ATmega328P per-pin limit (40mA absolute max, 20mA recommended continuous).

---

### 4. Arduino UNO R3
- **Microcontroller**: Microchip ATmega328P 8-bit AVR RISC processor.
- **Clock Speed**: 16 MHz external crystal oscillator.
- **Flash Memory**: 32 KB (0.5 KB utilized by bootloader).
- **SRAM**: 2 KB.
- **EEPROM**: 1 KB.
- **Analog Inputs**: 6 channels (A0 - A5) with 10-bit Successive Approximation Register (SAR) ADC ($1024$ discrete steps, $4.88\text{ mV}$ resolution at $5.0\text{V}$ reference).
- **Digital I/O**: 14 pins (of which 6 provide 8-bit PWM).
