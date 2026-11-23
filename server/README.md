# Receiver (server, first version)

A small Python program that runs on my laptop. ESP32 boards send it their messages over Wi-Fi. It checks every message, saves it in a database and shows a status page with every board and whether it is online.

It only uses the Python standard library (Python 3.11 or newer), so there is nothing to install to run it. `pytest` is only needed for the tests.

## Run it

From this `server/` folder:

```
python -m receiver.app
```

Then open `http://localhost:8000` in the browser. The page refreshes every 10 seconds.

The first time, Windows asks whether Python may use the network. Allow it for **private networks**, otherwise the board can't reach the laptop.

Settings, all optional (environment variables):

| Variable | Default | Meaning |
|---|---|---|
| `RECEIVER_HOST` | `0.0.0.0` | Listen on the whole local network |
| `RECEIVER_PORT` | `8000` | Port |
| `RECEIVER_DB` | `receiver.sqlite3` | Database file |
| `STALE_AFTER_S` | `60` | A board counts as offline after this many seconds without a message |

## What it does

| Address | What |
|---|---|
| `POST /api/v1/messages` | A board sends one message. Answers 201 when stored, 400 for broken JSON, 422 for a message that fails the checks, 413 if it is larger than 4 KB |
| `GET /api/v1/devices` | Every board seen so far as JSON: online or not, last message, signal, number of messages and restarts |
| `GET /` | The same as a page in the browser |

Message format and checks: [`receiver/schema.py`](receiver/schema.py). Every time is stored in UTC. The page shows local time with the time zone written next to it.

A temperature reading is only accepted as `ok` if it is inside the DS18B20 range from the datasheet (-55 to +125 °C). A probe that doesn't answer is sent as `disconnected` with no value, so a missing number is never stored as a real temperature.

## Tests

```
python -m venv .venv
.venv\Scripts\python -m pip install -r requirements-dev.txt
.venv\Scripts\python -m pytest
```

38 tests: valid and broken messages, values outside the sensor range, restart counting, the offline limit, and the HTTP answers. All pass.

## First run (November 23, 2026)

The board on my desk sent a message every 10 seconds and every one was stored. A board that stopped sending was shown as OFFLINE after 60 seconds.

## Not in this version

- No password: anyone on the same Wi-Fi could send messages. Fine on the desk, not for the shop
- No phone alerts yet (stage 7). For now "offline" is only visible on the page
- Runs on my laptop. Where it will run in the shop is not decided yet
