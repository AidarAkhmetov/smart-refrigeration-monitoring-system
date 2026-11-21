# Architecture

Status: PROPOSED. Nothing here is final. It will change after the equipment is fully identified and the sensors are tested (stages 2-4 of the [roadmap](roadmap.md)).

## 1. Overview

```
        ONE COLD ROOM (about 106 m2)
  evaporator outlet, far end, door, middle of the room
  (Phase 2: compressor, condenser fans, defrost, power)
                   |
       protection and isolation
                   |
     microcontroller (probably ESP32)
     stores data if Wi-Fi is down
                   |
          Wi-Fi (MQTT or HTTP)
                   |
             server (Python)
     checks data, stores it, sends alerts
                   |
                database
                   |
        app + phone notifications
```

The data is organized as one cold room with several measurement points. In Phase 2 each part of the unit also gets its own channels.

## 2. Verified equipment

What I actually know about the installation is in the [site survey](site-survey.md). Short version: one freezer room of 106.2 m2 at -18 to -20 °C, one outdoor Holodom condensing unit (Frascold V25-103Y compressor, FNH-49.9/180 condenser with 4 fans, R404A), one HLD.E-450-36/28-6 evaporator with electric defrost, and an Elitech ETC-974 controller without a data port.

Phase 1 only needs sensors inside the room. For Phase 2 the nameplates now give the ranges the sensors must handle, for example up to 52.3 A for the compressor.

Where things go: the main device sits in the office right behind the cold room wall, where there is a socket and internet. For the first 2-3 weeks the sensors inside run on batteries and send their data wirelessly. After that I drill one small sealed hole near the door and switch to wired sensors.

## 3. Candidate sensors

None of these are chosen. Each one still needs to be checked against the datasheet, tested and priced.

| Measurement | Candidates | Why it might fit | Concerns |
|---|---|---|---|
| Room air temperature | DS18B20 waterproof probe | Cheap, digital, several on one wire | Need to check accuracy at -20 °C and moisture during defrost |
| Room air temperature | PT1000 + converter (for example MAX31865) | More accurate and stable | More expensive, more parts |
| Room air temperature, test period | Wireless battery sensor inside the room, receiver in the office | No hole needed while I'm still testing | Batteries at -20 °C, radio range through the insulated wall and door |
| Compressor / fan running (Phase 2) | Split-core current transformer | Doesn't touch the wiring, clamps around one wire | Has to be installed by an electrician in the panel |
| Fan running (Phase 2) | Vibration sensor on the fan housing | No mains work at all | Less reliable, may pick up vibration from the compressor |
| Door open (Phase 2) | Magnetic reed switch | Very cheap | Need a safe way to mount it on a freezer door |
| Mains power present | Isolated power detection module | Tells a power cut apart from a broken unit | Mains side, electrician |

## 4. Proposed measurement methods

Phase 1:

- Temperature probes near the evaporator air outlet, at the far end of the room, near the door and in the middle of the room. In a room this size the temperature can differ a lot between these places, so the alert should look at the warmest point, not at an average. Exact places chosen on site.
- Wired probes would need a cable route into the room without drilling between the office and the room. If there is no such route, the sensors inside have to be wireless.
- Readings every 30-60 seconds.
- The device sends a short "I'm alive" message regularly. If the server doesn't hear from it for a few minutes, that's an alert.
- Defrost detection from the temperature curve near the evaporator (temperature rises fast and comes back on a regular schedule). To be tested on real data.

Phase 2 (research):

- Compressor and fan state from current, with the sensors installed by an electrician.
- Count starts per hour, run time and defrost duration for each part, and compare them with normal values collected over several weeks.

## 5. Fault hypotheses (not validated)

These are ideas for Phase 2, based on how refrigeration units usually work. None of them are implemented or tested. I'll check them against real data and with the technician who services our unit.

| What the system might see | Possible cause |
|---|---|
| Room getting warmer, compressor not running, power is on | Compressor, its contactor or the controller |
| Room getting warmer, nothing running, no power | Power cut |
| Compressor running, one condenser fan has no current, condenser getting hot | That condenser fan |
| Compressor running for a very long time, room still not cold enough | Dirty condenser, low refrigerant or a fan problem |
| Defrost taking longer each time | Ice building up, defrost heater or its sensor |
| Compressor turning on and off many times per hour | Short cycling: controller settings, pressure switch or refrigerant |
| Room warm but everything running normally | Door open or a lot of warm goods just loaded |

## 6. Example reading

```json
{
  "device_id": "node-01",
  "sensor_id": "room-far-end-air",
  "metric": "temperature",
  "value": -18.6,
  "unit": "degC",
  "timestamp_utc": "2026-11-17T10:00:00Z",
  "quality": "good",
  "sequence": 1
}
```

`quality` is `good`, `suspect` or `bad` (for example when a sensor is disconnected or the value is impossible). `sequence` helps find lost or repeated messages. The value above is an example, not a measurement.

## 7. Open questions

- How much of the test version (wireless) can be reused in the final version (wired)? Ideally the same sensors and the same firmware structure, only the way the data travels changes.
- DS18B20 or PT1000 (if wired)? Depends on the bench test at freezer temperature.
- MQTT or simple HTTP?
- Database: SQLite to start, maybe InfluxDB or TimescaleDB later.
- Where does the server run: a cheap cloud server, or a Raspberry Pi at home?
- How to send notifications: Telegram bot is the easiest to start with.
