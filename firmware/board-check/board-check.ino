// board-check: first power-up test for the ESP32 bench board (no sensors needed).
// 1) Prints chip info once (model, revision, cores, flash, MAC).
// 2) Every 10 s scans Wi-Fi and prints each network with its RSSI (dBm).
// Serial Monitor: 115200 baud.
// NOTE: ESP32-WROOM-32U has NO built-in antenna - attach the external
// antenna (U.FL connector) BEFORE uploading this sketch.

#include <WiFi.h>

static const uint32_t SCAN_INTERVAL_MS = 10000;
static uint32_t lastScanMs = 0;
static bool scanRunning = false;

static const char* quality(int32_t rssi) {
  if (rssi >= -60) return "good";
  if (rssi >= -70) return "ok";
  if (rssi >= -80) return "weak";
  return "very weak";
}

void setup() {
  Serial.begin(115200);
  delay(1000);  // give the Serial Monitor time to attach
  Serial.println();
  Serial.println("=== board-check ===");
  Serial.printf("Chip model:    %s rev %d\n", ESP.getChipModel(), ESP.getChipRevision());
  Serial.printf("CPU cores:     %d @ %lu MHz\n", ESP.getChipCores(), (unsigned long)ESP.getCpuFreqMHz());
  Serial.printf("Flash size:    %lu MB\n", (unsigned long)(ESP.getFlashChipSize() / (1024 * 1024)));
  Serial.printf("Free heap:     %lu bytes\n", (unsigned long)ESP.getFreeHeap());

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  Serial.printf("MAC address:   %s\n", WiFi.macAddress().c_str());
  Serial.println("Board OK. Starting Wi-Fi scans every 10 s...");
}

void loop() {
  uint32_t now = millis();

  if (!scanRunning && (lastScanMs == 0 || now - lastScanMs >= SCAN_INTERVAL_MS)) {
    WiFi.scanNetworks(true);  // async: does not block loop()
    scanRunning = true;
    lastScanMs = now;
  }

  if (scanRunning) {
    int16_t n = WiFi.scanComplete();
    if (n == WIFI_SCAN_RUNNING) return;
    scanRunning = false;
    if (n < 0) {
      Serial.println("Scan failed, will retry.");
      return;
    }
    Serial.printf("\n--- %d networks found (uptime %lu s) ---\n", n, (unsigned long)(now / 1000));
    for (int16_t i = 0; i < n; i++) {
      Serial.printf("%2d  %4ld dBm  %-9s  ch %2ld  %s\n", i + 1, (long)WiFi.RSSI(i),
                    quality(WiFi.RSSI(i)), (long)WiFi.channel(i), WiFi.SSID(i).c_str());
    }
    WiFi.scanDelete();
  }
}
