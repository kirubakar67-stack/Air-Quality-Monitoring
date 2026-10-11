#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// Wi-Fi Network Credentials (ESP8266 requires 2.4 GHz network)
// ============================================================================
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";

// ============================================================================
// ThingSpeak Cloud Configuration
// ============================================================================
// Instructions:
//   1. Log in to your account at https://thingspeak.com
//   2. Navigate to "Channels" -> "My Channels" -> "API Keys"
//   3. Copy your Channel ID (numeric) and Write API Key (16 characters) below:
//
// Field Mapping:
//   - Field 1: Raw MQ-135 Sensor ADC Value
//   - Field 2: Alert Status (0 = Normal, 1 = Warning)
// ============================================================================
const unsigned long THINGSPEAK_CHANNEL_ID = 0000000;
const char* THINGSPEAK_WRITE_API_KEY      = "YOUR_WRITE_API_KEY";

#endif // CONFIG_H
