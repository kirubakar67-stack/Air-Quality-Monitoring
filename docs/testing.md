# 🧪 System Testing, Calibration & Troubleshooting Guide

This guide details the systematic hardware verification protocols, baseline calibration tests, functional validation procedures, and troubleshooting workflows for the **Air Quality Monitoring System**.

---

## 1. Pre-Power Safety & Continuity Verification

Before connecting the Arduino to a USB port or external power source, perform the following continuity checks using a digital multimeter in resistance/continuity mode:

- [ ] **VCC to GND Short-Circuit Check**:
  Measure resistance between the breadboard `+5V` rail and `GND` rail. The reading should be in the kilo-ohm range. If the multimeter beeps or reads near $0\ \Omega$, inspect wiring immediately to protect the USB host port.
- [ ] **Pin Mapping Check**:
  - MQ-135 `AO` $\to$ Arduino `A0`
  - LCD `SDA` $\to$ Arduino `A4`
  - LCD `SCL` $\to$ Arduino `A5`
  - Buzzer `(+)` $\to$ Arduino `D5`
- [ ] **Common Ground Continuity**:
  Verify that the MQ-135 GND, LCD GND, Buzzer GND, and Arduino GND are all bonded to the same reference rail.

---

## 2. Power-On & I2C Bus Address Discovery

1. Connect the Arduino UNO to your PC via USB.
2. Observe the hardware LEDs:
   - Arduino `ON` LED illuminates solid green.
   - MQ-135 power LED (red) illuminates.
   - LCD backlight illuminates.
3. Flash the I2C scanner sketch (provided in [arduino-uno/README.md](file:///c:/Users/kirub/OneDrive/Documents/air_quality_monitoring/arduino-uno/README.md)) to confirm device communication.
   - Expected output in Serial Monitor at 9600 baud:
     ```text
     Scanning...
     I2C device found at address 0x27 !
     done
     ```
   - If the scanner detects address `0x3F`, update the constructor line in `air_quality_monitor.ino`:
     ```cpp
     LiquidCrystal_I2C lcd(0x3F, 16, 2);
     ```

---

## 3. Sensor Pre-Heat & Baseline Stabilization Test

The MQ-135 heating element requires time to reach thermal equilibrium.

1. Upload `arduino-uno/air_quality_monitor.ino`.
2. Open the **Serial Monitor** at **9600 baud**.
3. Record raw ADC readings at 30-second intervals in a well-ventilated, clean ambient room:

| Elapsed Time | Typical Observed ADC | Sensor State | LCD Display Reading |
| :---: | :---: | :---: | :---: |
| **0 - 30 seconds** | 500 - 750 (Transient) | Initial cold-start heating spike | `Sensor: 680  Status: WARNING` (Initial transient) |
| **1 - 2 minutes** | 350 - 450 | Surface temperature stabilizing | `Sensor: 390  Status: NORMAL` |
| **3 - 5 minutes** | 200 - 320 (Stable Baseline) | Thermal equilibrium achieved | `Sensor: 245  Status: NORMAL` |

> [!NOTE]
> Setting your baseline: Record your clean-air value after 5 minutes of operation (e.g., $250$). Set your `AIR_THRESHOLD` in code roughly $100 - 150$ units above this ambient baseline (e.g., `const int AIR_THRESHOLD = 400;`).

---

## 4. Controlled Gas Challenge Test

To verify the threshold detection and alarm triggering logic without exposing personnel to toxic fumes:

### Test Procedure:
1. Ensure the system is running with the baseline reading displaying `Status: NORMAL` and the buzzer silent.
2. Moisten a cotton swab with a drop of **isopropyl alcohol (70%)** or bring an unlit lighter (releasing a brief puff of butane gas) or an extinguished incense stick within 2 to 3 cm of the sensor mesh.
3. **Observation**:
   - The raw ADC value will climb rapidly from the baseline (~250) to >600 within 2 to 4 seconds.
   - As soon as the reading passes `400`:
     - Line 2 of the LCD immediately switches to `Status: WARNING`.
     - The active buzzer energizes, producing a clear audible alert tone.
     - Serial Monitor outputs: `MQ-135 Raw ADC: 642 | Threshold: 400 | Status: WARNING (Contamination Detected)`.
4. Remove the test source and allow clean air to circulate.
   - Within 10 to 30 seconds, the ADC reading will decay below `400`.
   - The buzzer automatically silences, and the LCD returns to `Status: NORMAL`.

---

## 5. Troubleshooting Matrix

| Symptom | Root Cause | Solution |
| :--- | :--- | :--- |
| **LCD displays black boxes or blank screen** | Contrast trimpot misadjusted or wrong I2C address | 1. Rotate the blue potentiometer on the back of the I2C module with a screwdriver until text appears.<br>2. Run the I2C scanner sketch to confirm whether the address is `0x27` or `0x3F`. |
| **Buzzer buzzes continuously even in fresh air** | `AIR_THRESHOLD` configured below clean-air baseline, or sensor not pre-heated | 1. Allow sensor to warm up for 5 minutes.<br>2. Check Serial Monitor for current baseline ADC and increase `AIR_THRESHOLD` (e.g., from 400 to 500). |
| **Sensor values stay at 0 or 1023 continuously** | Faulty jumper or inverted A0/VCC wiring | Check connection from MQ-135 `AO` pin to Arduino `A0`. Ensure the analog output (`AO`) is connected, not digital output (`DO`). |
| **NodeMCU ESP8266 reports HTTP -301 or -401 on ThingSpeak** | Invalid API key or update interval too fast (<15s) | Ensure ThingSpeak Write API key is entered accurately in `config.h`. Ensure posting interval is $\ge 15$ seconds (default is 20s). |
| **Sensor reading fluctuates wildly** | Inadequate USB power current supply | Power Arduino directly from a powered USB port or dedicated 5V 1A adapter. Avoid unpowered USB multi-hubs. |
