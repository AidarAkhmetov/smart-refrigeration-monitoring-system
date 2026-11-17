# Architecture

First idea only. Nothing here is final until stages 2 and 3 of the [roadmap](roadmap.md) are done.

```
               ONE COLD ROOM
  room: temperature in the main and second chamber
  outdoor unit: compressor, 2 condenser fans
  evaporator: fan, defrost, air temperature in and out
  power: is there mains power
                   |
       protection and isolation
                   |
     microcontroller (probably ESP32)
     stores data if Wi-Fi is down
                   |
          Wi-Fi (MQTT or HTTP)
                   |
             server (Python)
   checks data, logs anomalies, guesses the faulty part
                   |
                database
                   |
        app + phone notifications
```

The data is organized as one cold room with several parts, and each part has its own sensors and its own anomaly history.

## What I want to measure on each part

First ideas. Each sensor still needs to be checked for safety and price.

| Part | What to watch | Possible sensor |
|---|---|---|
| Room | Air temperature in both chambers | Temperature probes |
| Room | Door left open | Magnetic door sensor |
| Compressor | Is it running, starts per hour, how long it runs, current | Current clamp on the cable (installed by an electrician) |
| Compressor | Discharge pipe temperature | Temperature sensor on the outside of the pipe |
| Condenser fans | Is each fan running | Current clamp or vibration sensor on each fan |
| Condenser | Air temperature in and out | Two temperature sensors |
| Evaporator | Air temperature in and out, coil temperature | Temperature sensors |
| Defrost | When it starts and how long it takes | From the coil temperature curve |
| Power | Is there mains power | Isolated power detection |

## How faults could be found

The server compares the parts with each other. Some examples of what I want it to recognize:

| What the system sees | Most likely problem |
|---|---|
| Room getting warmer, compressor not running, power is on | Compressor, its contactor or the controller |
| Room getting warmer, nothing running, no power | Power cut |
| Compressor running, one condenser fan has no current, condenser getting hot | That condenser fan |
| Compressor running for a very long time, air in and out of the condenser almost the same temperature | Dirty condenser or low refrigerant |
| Defrost taking longer each time, coil stays cold | Ice building up, defrost heater or its sensor |
| Compressor turning on and off many times per hour | Short cycling: controller settings, pressure switch or refrigerant |
| Room warm but everything running normally | Door open or a lot of warm goods just loaded |

These are guesses based on how refrigeration units usually work. I'll adjust them after I collect real data and talk to the technician who services our unit.

## Example reading

```json
{
  "device_id": "node-01",
  "sensor_id": "compressor-current",
  "part": "compressor",
  "metric": "current",
  "value": 9.8,
  "unit": "A",
  "timestamp_utc": "2026-11-17T10:00:00Z",
  "quality": "good",
  "sequence": 1
}
```

`quality` is `good`, `suspect` or `bad` (for example when a sensor is disconnected or the value is impossible). `sequence` helps find lost or repeated messages. `part` links the reading to a part, so anomalies can be counted for each part separately.

## Open questions

- Which temperature sensors work at -20 °C and survive the moisture during defrost? DS18B20 probes are cheap, PT100/PT1000 is more accurate but needs extra parts.
- Current clamps or vibration sensors for the fans: which is cheaper and more reliable?
- One microcontroller or two (one inside, one at the outdoor unit)? Depends on cable length.
- MQTT or simple HTTP?
- Database: SQLite to start, maybe InfluxDB or TimescaleDB later.
- How to define "normal" for each part: fixed limits at first, later limits learned from the collected data.
