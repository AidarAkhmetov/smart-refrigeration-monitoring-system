// Bench test, stage 4: read DS18B20 probes on one 1-Wire bus and print
// the readings to the serial monitor as CSV lines.
//
// Hardware: ESP32-WROOM-32U board powered from the laptop USB port.
// Probes are powered from the board's 3V3 pin (DS18B20 supply range is
// 3.0-5.5 V), with a 4.7 kOhm pull-up from the data line to 3V3.
//
// Libraries (Arduino Library Manager):
//   OneWire by Paul Stoffregen
//   DallasTemperature by Miles Burton

#include <OneWire.h>
#include <DallasTemperature.h>

// GPIO4 is not a strapping pin (those are GPIO0, 2, 5, 12, 15) and not
// used by the module flash (GPIO6-11), so it is safe at boot.
constexpr uint8_t ONE_WIRE_PIN = 4;

constexpr uint32_t SERIAL_BAUD = 115200;
constexpr uint32_t READ_INTERVAL_MS = 5000;
constexpr uint8_t RESOLUTION_BITS = 12;        // 0.0625 C steps
constexpr uint32_t CONVERSION_TIME_MS = 750;   // datasheet max at 12 bits
constexpr uint8_t MAX_SENSORS = 4;

// The probe reports +85 C until the first conversion after power-up.
constexpr float POWER_ON_VALUE_C = 85.0f;
constexpr float MIN_VALID_C = -55.0f;
constexpr float MAX_VALID_C = 125.0f;

OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);

DeviceAddress addresses[MAX_SENSORS];
uint8_t sensorCount = 0;

enum class State { Idle, Converting };
State state = State::Idle;
uint32_t lastRequestMs = 0;

void addressToString(const DeviceAddress address, char* out) {
  for (uint8_t i = 0; i < 8; i++) {
    sprintf(out + i * 2, "%02X", address[i]);
  }
  out[16] = '\0';
}

const char* classify(float tempC) {
  if (tempC == DEVICE_DISCONNECTED_C) return "disconnected";
  if (tempC == POWER_ON_VALUE_C) return "suspect_85";
  if (tempC < MIN_VALID_C || tempC > MAX_VALID_C) return "out_of_range";
  return "ok";
}

void scanBus() {
  sensors.begin();
  sensorCount = 0;
  const uint8_t found = sensors.getDeviceCount();
  for (uint8_t i = 0; i < found && sensorCount < MAX_SENSORS; i++) {
    if (sensors.getAddress(addresses[sensorCount], i)) {
      sensors.setResolution(addresses[sensorCount], RESOLUTION_BITS);
      sensorCount++;
    }
  }

  Serial.printf("# found %u sensor(s) on GPIO%u\n", sensorCount, ONE_WIRE_PIN);
  char text[17];
  for (uint8_t i = 0; i < sensorCount; i++) {
    addressToString(addresses[i], text);
    Serial.printf("# sensor %u: %s\n", i, text);
  }
  if (sensorCount > 0 && sensors.isParasitePowerMode()) {
    Serial.println("# warning: parasite power detected, check the VDD wire");
  }
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(500);  // give the serial monitor time to attach
  Serial.println("# bench test: DS18B20 on ESP32");

  sensors.setWaitForConversion(false);  // do not block during conversion
  scanBus();
  Serial.println("uptime_ms,sensor,temp_c,status");
  lastRequestMs = millis();
}

void loop() {
  const uint32_t now = millis();

  if (state == State::Idle && now - lastRequestMs >= READ_INTERVAL_MS) {
    lastRequestMs = now;
    if (sensorCount == 0) {
      scanBus();  // nothing found yet: keep looking, a probe may be plugged in later
      return;
    }
    sensors.requestTemperatures();
    state = State::Converting;
  }

  if (state == State::Converting && now - lastRequestMs >= CONVERSION_TIME_MS) {
    char text[17];
    bool anyOk = false;
    for (uint8_t i = 0; i < sensorCount; i++) {
      const float tempC = sensors.getTempC(addresses[i]);
      const char* status = classify(tempC);
      if (strcmp(status, "ok") == 0) anyOk = true;
      addressToString(addresses[i], text);
      Serial.printf("%lu,%s,%.2f,%s\n", (unsigned long)now, text, tempC, status);
    }
    if (!anyOk) {
      sensorCount = 0;  // every probe failed: rescan the bus on the next round
    }
    state = State::Idle;
  }
}
