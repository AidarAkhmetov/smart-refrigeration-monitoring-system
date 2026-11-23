// heartbeat: the ESP32 joins Wi-Fi and every 10 s sends a status message
// (schema v1, JSON over HTTP POST) to the receiver on the laptop.
// No sensors yet: "readings" is an empty list. Temperature comes later.
// Credentials and the receiver address live in secrets.h (local only, never in Git).
// Serial Monitor: 115200 baud.

#include <WiFi.h>
#include <HTTPClient.h>
#include <esp_mac.h>
#include "secrets.h"

static const char*    FW_VERSION         = "heartbeat-0.1";
static const uint32_t SEND_INTERVAL_MS   = 10000;
static const uint32_t WIFI_RETRY_MS      = 15000;
static const uint16_t HTTP_TIMEOUT_MS    = 3000;  // a send never blocks longer than this

static char     deviceId[24];      // "esp32-" + 12 hex digits of the MAC
static uint32_t seq = 0;           // message counter, starts at 0 after every boot
static uint32_t lastSendMs = 0;
static uint32_t lastWifiAttemptMs = 0;
static uint32_t sentOk = 0;
static uint32_t sentFailed = 0;

static void makeDeviceId() {
  // Read the factory Wi-Fi MAC from eFuse. WiFi.macAddress() returns zeros
  // if it is called before the Wi-Fi driver has started.
  uint8_t mac[6] = {0};
  if (esp_read_mac(mac, ESP_MAC_WIFI_STA) != ESP_OK) {
    Serial.println("WARNING: could not read MAC, device id will be zeros");
  }
  snprintf(deviceId, sizeof(deviceId), "esp32-%02x%02x%02x%02x%02x%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

static void sendMessage() {
  char body[256];
  int len = snprintf(body, sizeof(body),
      "{\"schema_version\":1,\"device_id\":\"%s\",\"fw_version\":\"%s\","
      "\"seq\":%lu,\"uptime_s\":%lu,\"rssi_dbm\":%ld,\"readings\":[]}",
      deviceId, FW_VERSION, (unsigned long)seq, (unsigned long)(millis() / 1000),
      (long)WiFi.RSSI());
  if (len <= 0 || len >= (int)sizeof(body)) {
    Serial.println("ERROR: message too long, not sent");
    return;
  }

  HTTPClient http;
  http.setTimeout(HTTP_TIMEOUT_MS);
  http.setConnectTimeout(HTTP_TIMEOUT_MS);
  if (!http.begin(RECEIVER_URL)) {
    Serial.println("ERROR: bad RECEIVER_URL in secrets.h");
    sentFailed++;
    return;
  }
  http.addHeader("Content-Type", "application/json");
  int code = http.POST((uint8_t*)body, len);
  if (code == 201) {
    sentOk++;
    Serial.printf("sent #%lu  rssi %ld dBm  -> stored (ok %lu, failed %lu)\n",
                  (unsigned long)seq, (long)WiFi.RSSI(), (unsigned long)sentOk,
                  (unsigned long)sentFailed);
  } else {
    sentFailed++;
    if (code < 0) {
      Serial.printf("sent #%lu  -> FAILED: %s (is the receiver running?)\n",
                    (unsigned long)seq, http.errorToString(code).c_str());
    } else {
      Serial.printf("sent #%lu  -> receiver answered %d: %s\n",
                    (unsigned long)seq, code, http.getString().c_str());
    }
  }
  http.end();
  seq++;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("=== heartbeat ===");
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  makeDeviceId();
  Serial.printf("Device id: %s\nReceiver:  %s\n", deviceId, RECEIVER_URL);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  lastWifiAttemptMs = millis();
}

void loop() {
  uint32_t now = millis();

  if (WiFi.status() != WL_CONNECTED) {
    if (now - lastWifiAttemptMs >= WIFI_RETRY_MS) {
      Serial.println("Wi-Fi not connected, retrying...");
      WiFi.disconnect();
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
      lastWifiAttemptMs = now;
    }
    return;  // nothing to send without Wi-Fi (no buffering in this version)
  }

  if (lastSendMs == 0 || now - lastSendMs >= SEND_INTERVAL_MS) {
    lastSendMs = now;
    sendMessage();
  }
}
