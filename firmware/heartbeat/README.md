# Heartbeat sketch

The ESP32 connects to Wi-Fi and every 10 seconds sends a short status message to the [receiver](../../server/README.md) on my laptop. There are no sensors in it yet, so `readings` is an empty list. The point is to test the whole chain first: board, Wi-Fi, laptop, database, "device offline" detection. The temperature will go into the same message later.

## Message

JSON, sent with HTTP POST to `/api/v1/messages`. The full format and its checks are in [`server/receiver/schema.py`](../../server/receiver/schema.py).

```json
{"schema_version":1,"device_id":"esp32-a8032a5d0424","fw_version":"heartbeat-0.1",
 "seq":17,"uptime_s":170,"rssi_dbm":-47,"readings":[]}
```

| Field | Meaning |
|---|---|
| `device_id` | `esp32-` plus the board's factory Wi-Fi MAC address, so every board has its own name |
| `seq` | Message number. Starts at 0 after every restart, so the server can count restarts |
| `uptime_s` | Seconds since the board started |
| `rssi_dbm` | Wi-Fi signal at the moment of sending |

The board has no clock, so it sends no time. The server writes down when each message arrived (in UTC).

## Setup

1. Copy `secrets.example.h` to `secrets.h` in the same folder and fill in the Wi-Fi name, password and the laptop's address. `secrets.h` is in `.gitignore`.
2. Start the receiver on the laptop first.
3. Board: **ESP32 Dev Module**, upload speed **115200**, serial monitor **115200**.

Serial output when it works:

```
=== heartbeat ===
Device id: esp32-a8032a5d0424
Receiver:  http://192.168.0.11:8000/api/v1/messages
sent #0  rssi -53 dBm  -> stored (ok 1, failed 0)
```

## What goes wrong and what happens

| Problem | Behaviour |
|---|---|
| Wi-Fi lost | Stops sending, tries to reconnect every 15 s |
| Laptop off or receiver not running | Prints `FAILED`, tries again in 10 s. Waits at most 3 s per try |
| Receiver rejects the message | Prints the error the receiver sent back |

## Bug I found

In the first version every board was called `esp32-000000000000`. I asked for the MAC address before the Wi-Fi driver was started, and then it returns zeros. Now it reads the MAC straight from the chip's eFuse memory.

## Not in this version

- Messages are not saved when the laptop is off. They are simply lost. Keeping them until the connection is back is part of stage 5
- No sensors, no watchdog, no battery saving
- Plain HTTP without a password, so it is only for our own local network
