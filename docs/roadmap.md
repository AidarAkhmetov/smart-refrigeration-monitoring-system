# Roadmap

The project is split into two phases and 12 stages. Each stage has a clear "done when" rule, so I know when I can move on.

Status: ✅ done · 🔄 in progress · ⬜ not started

## Phase 1: monitoring prototype

The goal of Phase 1 is a working system in our cold room: temperature sensors, phone alerts, and an alert when the device goes quiet.

| # | Stage | What it is about | Done when | Status | Details |
|---|---|---|---|---|---|
| 1 | Site survey | Find out what equipment the cold room has and what the system needs to do | All equipment listed, requirements written | ✅ Nov 22, 2026 | [Site survey](site-survey.md), [Requirements](requirements.md) |
| 2 | Exact equipment data | Read every nameplate, mark where things are, check the conditions on site | Nameplates read, evaporator, door and office marked on the plan, controller setpoint read, Wi-Fi signal measured in the room | 🔄 Nameplates and plan done. Setpoint and Wi-Fi left | [Site survey](site-survey.md) |
| 3 | Sensor choice | Choose temperature sensors that work at -20 °C and fit the budget | Sensor chosen and tested on the desk (stage 4) | 🔄 First candidate: DS18B20 | [Decisions](decisions.md) |
| 4 | First build on the desk | ESP32 and temperature sensors on a breadboard, powered from the laptop's USB | Sensors compared with a reference thermometer in ice water and in a freezer | 🔄 Probe reads on the desk (temporary wiring). Proper 4.7 kOhm wiring and ice water / freezer test left | [Bench prototype](../hardware/bench-prototype.md) |
| 5 | Firmware | The program on the ESP32 | Reads sensors, keeps data when Wi-Fi is down, survives an unplugged sensor and a restart | ⬜ | |
| 6 | Server | Receives, checks and stores the data | Works with automated tests | 🔄 Receives status and temperature from the board, shows them on a page, 44 tests | [Server](../server/README.md) |
| 7 | App and alerts | What my parents see on their phones | Live values, history, phone alerts, "device offline" alert | 🔄 Telegram bot on the board: /status, alarms with reminders and a silence button. History and "device offline" left | [Bot](../firmware/cold-room-bot/README.md) |
| 8 | Test in the cold room | Battery sensors in the room for 2-3 weeks, receiver in the office | Runs 2-3 weeks, false alarms during defrost counted and fixed | ⬜ | |
| 9 | Final installation | One small sealed hole near the door, wired sensors instead of batteries | Wired sensors work, no ice around the cable after a month | ⬜ | |

Stages 2, 3 and 4 run at the same time: building on the desk is how I check if the sensor I chose is good enough. From stage 5 on, I finish one stage before starting the next.

## Phase 2: equipment health (research)

Starts only after Phase 1 works in the cold room. The goal is to watch each part of the refrigeration unit and notice problems before a breakdown.

| # | Stage | What it is about | Done when | Status |
|---|---|---|---|---|
| 10 | Equipment state | Measure safely if the compressor and the fans are running | Running state measured, sensors installed by an electrician | ⬜ |
| 11 | Normal data | Collect several weeks of data | I know what "normal" looks like for each part | ⬜ |
| 12 | Anomalies | Find unusual behavior of each part | Anomalies logged per part, fault ideas from the [architecture](architecture.md) tested on real data | ⬜ |
