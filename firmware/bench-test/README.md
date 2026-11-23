# Bench test sketch

First program for the stage 4 desk build. It reads every DS18B20 probe on one wire and prints the temperature to the laptop every 5 seconds. It does not send anything over Wi-Fi yet.

Wiring: [bench-wiring.svg](../../hardware/bench-wiring.svg)

## What it needs

- Arduino IDE with the ESP32 board package installed
- Libraries from the Library Manager: **OneWire** (Paul Stoffregen) and **DallasTemperature** (Miles Burton)
- Board setting: Tools > Board > esp32 > **ESP32 Dev Module**
- Serial monitor speed: **115200**

## Settings

| Setting | Value | Why |
|---|---|---|
| Data pin | GPIO4 | Not a strapping pin (GPIO0, 2, 5, 12, 15) and not used by the flash memory (GPIO6-11), so it can't stop the board from booting. Source: ESP32 datasheet v5.3, Espressif GPIO docs |
| Probe supply | 3.3 V from the board | DS18B20 works from 3.0 to 5.5 V. At 3.3 V the data line stays at the ESP32's own logic level |
| Pull-up | 4.7 kOhm, data to 3.3 V | As in the DS18B20 datasheet |
| Resolution | 12 bit (0.0625 °C steps) | Highest. One reading takes up to 750 ms |
| Read interval | 5 s | Enough for a bench test |
| Max probes | 4 | Two now, room for more later |

## Output

One CSV line per probe per reading, easy to paste into a spreadsheet:

```
uptime_ms,sensor,temp_c,status
10750,28FF641E8216034B,23.44,ok
```

`sensor` is the probe's unique 64-bit address, so I always know which probe a reading came from.

## Failure handling

| Status | Meaning |
|---|---|
| `ok` | Normal reading |
| `disconnected` | The probe didn't answer (loose wire or broken probe). The library returns -127 °C |
| `suspect_85` | Exactly +85 °C is the value the probe holds right after power-up. Usually means a power problem. In a freezer test it is never a real reading |
| `out_of_range` | Outside the -55...+125 °C range from the datasheet |

If no probe is found, or all of them fail, the sketch scans the wire again every 5 seconds, so a probe can be plugged in without restarting the board.

The sketch never waits for a reading to finish. It asks for one and checks back after 750 ms.

## Not in this version

- Wi-Fi, sending data to the office receiver
- Watchdog and deep sleep for battery power
- Calibration offsets. First I need to measure them (room temperature, ice water, freezer)
