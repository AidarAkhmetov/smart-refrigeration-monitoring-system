// cold-room-bot: the ESP32 reads the DS18B20 probe(s) and talks to Telegram
// directly over HTTPS (option A of decision 2026-11-24, no server needed).
//   - answers /status, /help and /id from ONE allowed chat
//   - sends an alert when the temperature stays outside the limits, when a
//     probe stops answering, and a message when things are back to normal
//   - repeats the alarm every 15 s until it is silenced with the inline
//     button or /ok; a new alarm state re-arms the reminders
//   - says hello after every power-up, so a reboot is visible in the chat
//   - optionally still posts schema v1 messages to the laptop receiver
// Known limit: the board cannot report its own power or internet loss.
//
// Settings live in secrets.h (local only, never in Git): Wi-Fi, bot token,
// allowed chat id. See secrets.example.h.
// Libraries: OneWire, DallasTemperature, ArduinoJson (v7). Serial: 115200.

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <esp_mac.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>
#include "secrets.h"
#include "telegram_ca.h"

static const char*    FW_VERSION       = "cold-room-bot-0.2";
static const uint32_t MEASURE_EVERY_MS = 10000;
static const uint32_t POLL_EVERY_MS    = 3000;   // how often to ask Telegram for new commands
static const uint32_t RETRY_SEND_MS    = 15000;  // retry an unsent alert this often
static const uint32_t WIFI_RETRY_MS    = 15000;
static const uint16_t HTTP_TIMEOUT_MS  = 6000;

// ---- Alert limits: BENCH values for testing on the desk -------------------
// Hold the probe in your hand to pass the upper limit. The room values
// (storage -18...-20 degC per owner) are NOT decided yet: they need the
// ETC-974 setpoint and defrost settings first, and must allow for defrost.
static const float    ALERT_HIGH_C     = 28.0f;
static const float    ALERT_LOW_C      = 10.0f;
static const float    HYSTERESIS_C     = 1.0f;     // back to normal only 1 degC inside the limit
static const uint32_t ALERT_DELAY_MS   = 30000;    // must last this long before an alert
static const uint32_t REPEAT_ALARM_MS  = 15000;    // remind this often until acknowledged

// ---- DS18B20 probe, TEMPORARY bench wiring of 2026-11-26 -------------------
// red next to GPIO25 (VDD from a GPIO output), yellow next to GPIO26 (data,
// internal ~45 kOhm pull-up), black next to GPIO27 (GND from a GPIO output).
// Final wiring: 3V3, GND and the external 4.7 kOhm pull-up (datasheet);
// then set PROBE_GPIO_POWER to false and PROBE_DATA_PIN to the real pin.
// arduino/wire-check finds where the three wires actually sit.
static const bool     PROBE_GPIO_POWER = true;
static const uint8_t  PROBE_DATA_PIN   = 26;
static const uint8_t  PROBE_VDD_PIN    = 25;
static const uint8_t  PROBE_GND_PIN    = 27;
static const uint8_t  MAX_PROBES       = 4;
static const uint32_t CONVERSION_MS    = 750;   // datasheet max at 12 bits
static const float    POWER_ON_VALUE_C = 85.0f;
static const float    MIN_VALID_C      = -55.0f;
static const float    MAX_VALID_C      = 125.0f;

// ---------------------------------------------------------------------------

struct Probe {
  DeviceAddress addr;
  char id[17];
  float value;
  const char* status;   // ok / disconnected / suspect_85 / out_of_range
};

enum class Alarm : uint8_t { Normal, High, Low, Fault };

static OneWire oneWire(PROBE_DATA_PIN);
static DallasTemperature dallas(&oneWire);
static Probe probes[MAX_PROBES];
static uint8_t probeCount = 0;
static bool haveReading = false;
static bool conversionPending = false;
static uint32_t conversionStartMs = 0;

static Alarm alarmState = Alarm::Normal;   // what the chat was last told
static Alarm candidate = Alarm::Normal;    // what the probe shows right now
static uint32_t candidateSinceMs = 0;

static WiFiClientSecure tls;
static int64_t nextUpdateId = 0;
static bool updatesSynced = false;
static String pendingText;                 // alert waiting to be delivered
static bool pendingButton = false;         // attach the "silence" button to it
static uint32_t lastSendTryMs = 0;
static bool acknowledged = false;          // owner silenced the current alarm
static uint32_t lastRepeatMs = 0;
static bool helloSent = false;

static char deviceId[24];
static uint32_t lastPollMs = 0, lastWifiTryMs = 0, lastReceiverMs = 0, seq = 0;

// ---------------- probe ----------------

static void probePowerOn() {
  if (!PROBE_GPIO_POWER) return;
  pinMode(PROBE_GND_PIN, OUTPUT);
  digitalWrite(PROBE_GND_PIN, LOW);
  pinMode(PROBE_VDD_PIN, OUTPUT);
  digitalWrite(PROBE_VDD_PIN, HIGH);
}

static void dataPullup() {
  if (PROBE_GPIO_POWER) pinMode(PROBE_DATA_PIN, INPUT_PULLUP);
}

static void scanProbes() {
  dataPullup();
  dallas.begin();
  probeCount = 0;
  uint8_t n = dallas.getDeviceCount();
  for (uint8_t i = 0; i < n && probeCount < MAX_PROBES; i++) {
    Probe& p = probes[probeCount];
    if (!dallas.getAddress(p.addr, i)) continue;
    for (uint8_t b = 0; b < 8; b++) sprintf(p.id + b * 2, "%02X", p.addr[b]);
    p.id[16] = '\0';
    p.status = "disconnected";
    dallas.setResolution(p.addr, 12);
    probeCount++;
  }
  dallas.setWaitForConversion(false);
  Serial.printf("probes found: %u\n", probeCount);
}

static void startConversion() {
  conversionStartMs = millis();
  if (probeCount == 0) scanProbes();
  if (probeCount == 0) {
    haveReading = true;  // "no probe" is a result too (Fault)
    return;
  }
  dataPullup();
  dallas.requestTemperatures();
  conversionPending = true;
}

static void finishConversion() {
  conversionPending = false;
  uint8_t failed = 0;
  for (uint8_t i = 0; i < probeCount; i++) {
    Probe& p = probes[i];
    // getTempC re-reads the scratchpad and returns -127 when the CRC check
    // fails. A weak pull-up (bench wiring) causes occasional bad reads, so
    // try the same conversion result up to 3 times before calling it lost.
    float t = DEVICE_DISCONNECTED_C;
    uint8_t attempts = 0;
    while (attempts < 3 && t == DEVICE_DISCONNECTED_C) {
      dataPullup();
      t = dallas.getTempC(p.addr);
      attempts++;
    }
    if (attempts > 1) Serial.printf("probe %s: read needed %u attempts\n", p.id, attempts);
    p.value = t;
    if (t == DEVICE_DISCONNECTED_C) { p.status = "disconnected"; failed++; }
    else if (t == POWER_ON_VALUE_C) p.status = "suspect_85";
    else if (t < MIN_VALID_C || t > MAX_VALID_C) p.status = "out_of_range";
    else p.status = "ok";
    Serial.printf("probe %s: %.2f C (%s)\n", p.id, t, p.status);
  }
  haveReading = true;
  if (probeCount > 0 && failed == probeCount) probeCount = 0;  // rescan next time
}

// Worst state over all probes right now.
static Alarm currentCondition(float& worstValue) {
  bool anyOk = false, high = false, low = false, fault = (probeCount == 0);
  worstValue = NAN;
  for (uint8_t i = 0; i < probeCount; i++) {
    const Probe& p = probes[i];
    if (strcmp(p.status, "ok") != 0) { fault = true; continue; }
    anyOk = true;
    // While an alarm is active, require HYSTERESIS_C margin to call it normal.
    float hi = ALERT_HIGH_C - (alarmState == Alarm::High ? HYSTERESIS_C : 0);
    float lo = ALERT_LOW_C + (alarmState == Alarm::Low ? HYSTERESIS_C : 0);
    if (p.value > hi) { high = true; worstValue = p.value; }
    else if (p.value < lo) { low = true; worstValue = p.value; }
  }
  if (high) return Alarm::High;
  if (low) return Alarm::Low;
  if (fault || !anyOk) return Alarm::Fault;
  return Alarm::Normal;
}

// ---------------- text ----------------

static String temperaturesText() {
  if (probeCount == 0) return "датчик не найден";
  String s;
  for (uint8_t i = 0; i < probeCount; i++) {
    const Probe& p = probes[i];
    if (i) s += "\n";
    if (probeCount > 1) s += String("…") + (p.id + 12) + ": ";
    if (strcmp(p.status, "ok") == 0) s += String(p.value, 1) + " °C";
    else if (strcmp(p.status, "disconnected") == 0) s += "датчик не отвечает";
    else if (strcmp(p.status, "suspect_85") == 0) s += "подозрительные +85 °C (сбой питания датчика)";
    else s += "значение вне диапазона датчика";
  }
  return s;
}

static String uptimeText() {
  uint32_t s = millis() / 1000;
  char buf[32];
  snprintf(buf, sizeof(buf), "%luч %02luм", (unsigned long)(s / 3600),
           (unsigned long)((s / 60) % 60));
  return buf;
}

static String statusText() {
  String s = "🌡 " + temperaturesText();
  s += "\nПределы (тест на столе): " + String(ALERT_LOW_C, 0) + "…" + String(ALERT_HIGH_C, 0) + " °C";
  if (alarmActive()) s += acknowledged ? "\nТревога: идёт, отключена (🔕), вернуть: /on" : "\nТревога: ИДЁТ";
  s += "\nWi-Fi: " + String(WiFi.RSSI()) + " dBm";
  s += "\nРаботает без перезагрузки: " + uptimeText();
  return s;
}

static String alarmText(Alarm a, float v) {
  switch (a) {
    case Alarm::High:  return "🔴 ТЕПЛО: " + String(v, 1) + " °C (выше " + String(ALERT_HIGH_C, 0) + " °C больше " + String(ALERT_DELAY_MS / 1000) + " с)";
    case Alarm::Low:   return "🔵 ХОЛОДНО: " + String(v, 1) + " °C (ниже " + String(ALERT_LOW_C, 0) + " °C больше " + String(ALERT_DELAY_MS / 1000) + " с)";
    case Alarm::Fault: return "⚠️ Проблема с датчиком: " + temperaturesText();
    default:           return "✅ Снова норма: " + temperaturesText();
  }
}

// ---------------- Telegram ----------------

// POST a JSON body to a Bot API method. Returns HTTP code, response in `out`.
static int tgCall(const char* method, const String& body, String* out = nullptr) {
  HTTPClient http;
  http.setTimeout(HTTP_TIMEOUT_MS);
  http.setConnectTimeout(HTTP_TIMEOUT_MS);
  http.setReuse(true);
  String url = String("https://api.telegram.org/bot") + BOT_TOKEN + "/" + method;
  if (!http.begin(tls, url)) return -1;
  http.addHeader("Content-Type", "application/json");
  int code = http.POST(body);
  if (out && code > 0) *out = http.getString();
  if (code != 200) {
    // Never print the URL: it contains the token.
    Serial.printf("telegram %s -> HTTP %d %s\n", method, code,
                  code < 0 ? http.errorToString(code).c_str() : "");
  }
  http.end();
  return code;
}

static const char* ACK_DATA = "ack";

static bool tgSend(int64_t chatId, const String& text, bool silenceButton = false) {
  JsonDocument doc;
  doc["chat_id"] = chatId;
  doc["text"] = text;
  if (silenceButton) {
    JsonObject b = doc["reply_markup"]["inline_keyboard"][0][0].to<JsonObject>();
    b["text"] = "🔕 Отключить тревогу";
    b["callback_data"] = ACK_DATA;
  }
  String body;
  serializeJson(doc, body);
  return tgCall("sendMessage", body) == 200;
}

static bool alarmActive() { return alarmState != Alarm::Normal; }

// Queue a message for the allowed chat; it is retried until delivered.
static void notify(const String& text, bool silenceButton = false) {
  Serial.printf("ALERT: %s\n", text.c_str());
  pendingText = pendingText.length() ? pendingText + "\n" + text : text;
  pendingButton = silenceButton;
  lastSendTryMs = 0;
}

static void flushPending() {
  if (!pendingText.length() || ALLOWED_CHAT_ID == 0) return;
  if (lastSendTryMs && millis() - lastSendTryMs < RETRY_SEND_MS) return;
  lastSendTryMs = millis();
  // The button only makes sense while the alarm is still on and not silenced.
  bool button = pendingButton && alarmActive() && !acknowledged;
  if (tgSend(ALLOWED_CHAT_ID, pendingText, button)) pendingText = "";
}

// Every REPEAT_ALARM_MS while an alarm is on and nobody silenced it.
static void repeatAlarm() {
  if (!alarmActive() || acknowledged || ALLOWED_CHAT_ID == 0) return;
  if (pendingText.length()) return;          // first alert not delivered yet
  if (millis() - lastRepeatMs < REPEAT_ALARM_MS) return;
  lastRepeatMs = millis();
  tgSend(ALLOWED_CHAT_ID, "⏰ Тревога продолжается: " + temperaturesText() +
                          "\nНажми кнопку или отправь /ok, чтобы отключить.", true);
}

static String acknowledge() {
  if (!alarmActive()) return "Сейчас тревоги нет. " + temperaturesText();
  if (acknowledged) return "Тревога уже отключена.";
  acknowledged = true;
  Serial.println("alarm acknowledged");
  return "🔕 Тревога отключена, напоминаний больше не будет.\n"
         "Напишу, когда станет норма или появится новая проблема.\n"
         "Вернуть напоминания: /on\nСейчас: " +
         temperaturesText();
}

static void handleCommand(int64_t chatId, const char* text) {
  Serial.printf("message from chat %lld: %s\n", (long long)chatId, text);
  if (ALLOWED_CHAT_ID == 0) {
    // Setup mode: reveal only the chat id, nothing about the room.
    tgSend(chatId, "Твой chat_id: " + String((long long)chatId) +
                   "\nЕго нужно вписать в secrets.h (ALLOWED_CHAT_ID), после этого бот заработает.");
    return;
  }
  if (chatId != ALLOWED_CHAT_ID) {
    Serial.println("  ignored: chat not allowed");
    return;
  }
  String cmd = text;
  int at = cmd.indexOf('@');           // "/status@my_bot" in groups
  if (at > 0) cmd = cmd.substring(0, at);
  cmd.trim();
  if (cmd == "/status" || cmd == "/start") {
    tgSend(chatId, statusText(), alarmActive() && !acknowledged);
  } else if (cmd == "/ok") {
    tgSend(chatId, acknowledge());
  } else if (cmd == "/on") {
    if (!alarmActive()) {
      tgSend(chatId, "Сейчас тревоги нет, напоминать не о чем. Если что-то случится, напишу сам.");
    } else {
      acknowledged = false;
      lastRepeatMs = 0;          // remind right away
      Serial.println("alarm re-armed");
      tgSend(chatId, "🔔 Напоминания снова включены.");
    }
  } else if (cmd == "/id") {
    tgSend(chatId, "chat_id: " + String((long long)chatId) + "\nплата: " + deviceId +
                   "\nпрошивка: " + FW_VERSION);
  } else {
    tgSend(chatId, "Команды:\n/status — температура сейчас\n"
                   "/ok — отключить текущую тревогу\n"
                   "/on — снова включить напоминания о текущей тревоге\n/id — данные платы\n"
                   "Тревога приходит сама и повторяется каждые " +
                   String(REPEAT_ALARM_MS / 1000) + " с, пока её не отключить.");
  }
}

// Inline "silence" button pressed under an alarm message.
static void handleButton(const char* callbackId, int64_t chatId, const char* data) {
  Serial.printf("button from chat %lld: %s\n", (long long)chatId, data);
  String reply = "Нет доступа";
  bool allowed = ALLOWED_CHAT_ID != 0 && chatId == ALLOWED_CHAT_ID;
  if (allowed && strcmp(data, ACK_DATA) == 0) reply = acknowledge();
  // Telegram shows a spinner on the button until the press is answered.
  JsonDocument doc;
  doc["callback_query_id"] = callbackId;
  doc["text"] = allowed ? "Готово" : reply;
  String body;
  serializeJson(doc, body);
  tgCall("answerCallbackQuery", body);
  if (allowed) tgSend(chatId, reply);
}

static void pollTelegram() {
  JsonDocument req;
  req["offset"] = nextUpdateId;
  req["timeout"] = 0;
  req["limit"] = updatesSynced ? 5 : 1;
  if (!updatesSynced) req["offset"] = -1;   // after boot: skip old commands
  req["allowed_updates"][0] = "message";
  req["allowed_updates"][1] = "callback_query";   // the "silence" button
  String body, resp;
  serializeJson(req, body);
  if (tgCall("getUpdates", body, &resp) != 200) return;

  JsonDocument filter;
  filter["ok"] = true;
  filter["result"][0]["update_id"] = true;
  filter["result"][0]["message"]["chat"]["id"] = true;
  filter["result"][0]["message"]["text"] = true;
  filter["result"][0]["callback_query"]["id"] = true;
  filter["result"][0]["callback_query"]["data"] = true;
  filter["result"][0]["callback_query"]["message"]["chat"]["id"] = true;
  JsonDocument doc;
  if (deserializeJson(doc, resp, DeserializationOption::Filter(filter))) {
    Serial.println("telegram: bad JSON");
    return;
  }
  for (JsonObject u : doc["result"].as<JsonArray>()) {
    int64_t id = u["update_id"].as<int64_t>();
    nextUpdateId = id + 1;
    if (!updatesSynced) continue;           // old message from before boot
    if (u["callback_query"].is<JsonObject>()) {
      handleButton(u["callback_query"]["id"] | "",
                   u["callback_query"]["message"]["chat"]["id"].as<int64_t>(),
                   u["callback_query"]["data"] | "");
      continue;
    }
    int64_t chat = u["message"]["chat"]["id"].as<int64_t>();
    const char* text = u["message"]["text"] | "";
    if (chat != 0 && text[0]) handleCommand(chat, text);
  }
  updatesSynced = true;
}

// ---------------- optional laptop receiver ----------------

static void postToReceiver() {
  if (strlen(RECEIVER_URL) == 0) return;
  JsonDocument doc;
  doc["schema_version"] = 1;
  doc["device_id"] = deviceId;
  doc["fw_version"] = FW_VERSION;
  doc["seq"] = seq++;
  doc["uptime_s"] = millis() / 1000;
  doc["rssi_dbm"] = WiFi.RSSI();
  JsonArray rs = doc["readings"].to<JsonArray>();
  for (uint8_t i = 0; i < probeCount; i++) {
    JsonObject r = rs.add<JsonObject>();
    r["sensor_id"] = probes[i].id;
    r["quantity"] = "temperature";
    if (strcmp(probes[i].status, "ok") == 0 || strcmp(probes[i].status, "suspect_85") == 0)
      r["value"] = serialized(String(probes[i].value, 2));
    else
      r["value"] = nullptr;
    r["unit"] = "degC";
    r["status"] = probes[i].status;
  }
  String body;
  serializeJson(doc, body);
  HTTPClient http;
  http.setTimeout(2000);
  http.setConnectTimeout(2000);
  if (!http.begin(RECEIVER_URL)) return;
  http.addHeader("Content-Type", "application/json");
  int code = http.POST(body);
  if (code != 201) Serial.printf("receiver -> %d\n", code);
  http.end();
}

// ---------------- main ----------------

static void updateAlarm() {
  float v;
  Alarm now = currentCondition(v);
  if (now != candidate) {
    candidate = now;
    candidateSinceMs = millis();
  }
  // Faults and returns to normal are also debounced, so one bad read or
  // one good read in the middle of a problem does not spam the chat.
  if (candidate != alarmState && millis() - candidateSinceMs >= ALERT_DELAY_MS) {
    alarmState = candidate;
    acknowledged = false;       // a new state always needs fresh attention
    lastRepeatMs = millis();
    notify(alarmText(alarmState, v), alarmActive());
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.printf("\n=== %s ===\n", FW_VERSION);
  uint8_t mac[6] = {0};
  esp_read_mac(mac, ESP_MAC_WIFI_STA);
  snprintf(deviceId, sizeof(deviceId), "esp32-%02x%02x%02x%02x%02x%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  if (strcmp(BOT_TOKEN, "PASTE_TOKEN_HERE") == 0) {
    Serial.println("ERROR: BOT_TOKEN is not set in secrets.h");
  }
  probePowerOn();
  delay(50);
  scanProbes();
  startConversion();
  tls.setCACert(TELEGRAM_CA);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  lastWifiTryMs = millis();
}

void loop() {
  uint32_t now = millis();

  if (conversionPending && now - conversionStartMs >= CONVERSION_MS) {
    finishConversion();
    updateAlarm();
  }
  if (!conversionPending && now - conversionStartMs >= MEASURE_EVERY_MS) {
    startConversion();
  }

  if (WiFi.status() != WL_CONNECTED) {
    if (now - lastWifiTryMs >= WIFI_RETRY_MS) {
      Serial.println("Wi-Fi not connected, retrying...");
      WiFi.disconnect();
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
      lastWifiTryMs = now;
    }
    return;
  }
  if (strcmp(BOT_TOKEN, "PASTE_TOKEN_HERE") == 0) return;

  if (!helloSent && haveReading && ALLOWED_CHAT_ID != 0) {
    helloSent = true;
    notify("🟢 Плата включилась.\n" + statusText());
  }
  if (now - lastPollMs >= POLL_EVERY_MS) {
    lastPollMs = now;
    pollTelegram();
  }
  flushPending();
  repeatAlarm();
  if (now - lastReceiverMs >= MEASURE_EVERY_MS) {
    lastReceiverMs = now;
    postToReceiver();
  }
}
