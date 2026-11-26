# Decisions

I only add new entries at the bottom and don't change old ones, so later I can see why something was decided. If a decision changes, I add a new entry that replaces the old one.

Status:

- Confirmed: decided, won't change without a strong reason.
- Replaced: no longer valid, a later entry explains why.
- Provisional: current choice, may change after testing.

---

## 2026-11-17: The system only watches

Status: Confirmed

Decision: The system only monitors. It never controls, reprograms or bypasses the Elitech controller or any safety circuit.

Why: The cold room holds all the goods of the shop. If my device has a bug or loses power, the unit has to keep working like before. It also keeps me away from mains wiring that I'm not qualified to work on.

Not chosen: Reading data directly from the controller. Maybe later, but only if the manual confirms it has a supported interface.

---

## 2026-11-17: One cold room, several measurement points

Status: Replaced by "The target is the new large cold room" below

Decision: The installation is treated as one cold room with several measurement points (main chamber, second chamber, evaporator outlet), not as separate rooms.

Why: The main and the second chamber are one refrigerated space served by one unit. Splitting them into separate "rooms" would make the data and the app more complicated for no reason.

---

## 2026-11-17: Only equipment from this cold room

Status: Replaced by "The target is the new large cold room" below

Decision: The survey only lists equipment that belongs to this cold room. Parts bought around the same time for a different refrigerator are left out.

Why: Mixing them in would give a wrong picture of the system, for example the wrong evaporator or refrigerant.

---

## 2026-11-17: Low price is a must

Status: Confirmed

Decision: Price is one of the main limits for the design. Every part I choose gets compared with the price of ready-made systems (500-800 euros).

Why: Because of that price our shop has no monitoring at all, and the shops around us are in the same situation. If my system is only a little cheaper, it doesn't help anyone.

But: Cheap can't mean unreliable. A monitor that silently stops working is worse than none, because people stop checking. So if the device goes quiet, that's an alarm too.

---

## 2026-11-17: Temperature and alerts first, equipment health later

Status: Confirmed

Decision: Phase 1 is reliable temperature monitoring, offline data storage, "device offline" detection and phone alerts. Watching each part of the unit and finding faults is Phase 2.

Why: The thing that cost us 2000 euros was simply that nobody knew the room was warming up. A reliable temperature alert would have been enough to save the goods. Equipment health needs extra sensors, some on the mains side, and weeks of normal data before it can work, so it makes sense to build it on top of a system that already works.

---

## 2026-11-17: ESP32 as the microcontroller

Status: Provisional

Decision: Plan around an ESP32 board.

Why: Built-in Wi-Fi, cheap, enough pins for several sensors, lots of documentation.

Will check: Wi-Fi signal strength at the cold room, and whether the board works reliably in the place where it will be mounted.

---

## 2026-11-17: Python for the server

Status: Provisional

Decision: Write the server in Python.

Why: Simple to start with, and it has good libraries for working with time series data, for web servers and for tests.

Not decided yet: MQTT or HTTP, which database, where the server runs.

---

## 2026-11-17: Own sensors, no connection to the controller

Status: Confirmed

Decision: All measurements come from the system's own sensors. Nothing is connected to the ETC-974 controller.

Why: The ETC-974 manual shows no data port. The only connector is for a copy card that copies settings between controllers. So there is no supported way to read data from it, and this confirms the "only watches" decision above.

---

## 2026-11-17: The target is the new large cold room

Status: Confirmed

Decision: The system is designed for the new large cold room (about 106 m2 floor area, 3.5 m ceiling), not for the small room that broke down. This replaces two earlier entries: "One cold room, several measurement points" (the main and second chamber belonged to the small room) and "Only equipment from this cold room" (the evaporator and refrigerant I had left out are exactly the ones bought for the new room).

Why: The small room is going to be taken down and the new room replaces it. The task stays the same: keep the goods safe and alert us in time.

What changes:

- The old compressor, condenser and evaporator are no longer part of the project.
- Known equipment for the new room: ETC-974 controller, HLD-450-36/28-6 evaporator, R404A refrigerant, a YWF4E-400S fan. The compressor and condenser are not known yet.
- The room is about 4-5 times larger, so it needs more measurement points spread across it, and the alert has to react to the warmest point.
- Phase 1 (temperature and alerts) can be designed now. Phase 2 sensors wait until the outdoor unit is identified.

---

## 2026-11-22: Equipment identified from the nameplates

Status: Confirmed

Decision: The site survey now uses nameplate data instead of receipts. The installation is one Holodom condensing unit (Frascold V25-103Y compressor, FNH-49.9/180 condenser with 4 fans, R404A) and one HLD.E-450-36/28-6 evaporator with 16.8 kW electric defrost, controlled by the ETC-974. This updates the equipment list in "The target is the new large cold room".

Why: I photographed every label of the installation. Nameplate data is more reliable than a receipt, and now I know the real ranges the sensors have to handle.

What changes:

- The ceiling is 3.0 m, not 3.5 m. I misread the plan the first time.
- The room is a freezer at -18 to -20 °C, so the sensors have to work well around -20 °C.
- The YWF4E-400S fan from the receipt is left out of the survey. It isn't needed for this project for now.

---

## 2026-11-22: Main device in the office, no hole in the wall

Status: Partly replaced. The device stays in the office, but the "no hole" part is replaced by "Battery sensors first, drill once at the end" below

Decision: The microcontroller goes into the office right behind the cold room wall and is powered from a normal socket. No hole is drilled between the office and the cold room.

Why: The office has a socket and stable internet only 2-3 m from the room, so I don't need any mains work. Drilling between the office and the room is not allowed.

Not chosen: A box outside next to the condensing unit. It would be in dust and weather and further from power and internet.

Will check: How the sensors inside reach the device (most likely wireless), the signal strength at each sensor point, and how long batteries last at -20 °C.

---

## 2026-11-22: Battery sensors first, drill once at the end

Status: Provisional

Decision: This replaces the "no hole" part of the entry above. We got permission to drill one small hole between the office and the cold room. But I only have one try, so I drill at the very end. First the sensors run on batteries inside the room for 2-3 weeks and send their data wirelessly to the device in the office. When the whole system works, I drill the hole near the door, run one cable through it, seal it and switch to wired sensors.

Why: Wired sensors are better for the long run: no batteries in the cold and nothing that can lose the radio signal. But a hole in a freezer wall can't be undone, so I want to be sure about the sensors, the cable and the places before I make it.

Will check: wall thickness and material, hidden cables near the office socket, and a sealing method that keeps ice from forming around the cable.

---

## 2026-11-22: Parts for the bench prototype

Status: Provisional

Decision: Start building with two ESP32-WROOM-32U boards (external antenna) and DS18B20 waterproof probes on a breadboard, powered from the laptop's USB. List: [hardware/bench-prototype.md](../hardware/bench-prototype.md).

Why: It's cheap, needs no soldering and no mains wiring, and lets me test the sensors in ice water and in a freezer before anything goes into the cold room. The external antenna version can later point its antenna at the cold room from the office.

Not chosen yet: PT1000 sensors. If the DS18B20 turns out too inaccurate at -20 °C even after correction, I'll compare it with a PT1000.

Will check: accuracy of each probe at -18 °C against a reference thermometer.

---

## 2026-11-23: First server: JSON over HTTP, Python standard library, SQLite

Status: Provisional

Decision: The board sends one small JSON message every 10 seconds with HTTP POST to a Python program on my laptop. The program checks the message, saves it in an SQLite file and shows a status page. It uses only the Python standard library. The message has a version number (`schema_version: 1`), so I can change the format later without confusing old and new boards. Details: [server/README.md](../server/README.md).

Why: I wanted to test the whole chain (board, Wi-Fi, laptop, database, "device offline") before the probes arrive, with as few moving parts as possible. HTTP is easy to test from the browser and from the tests. SQLite is a single file and needs no separate database program. The server writes the time itself, because the board has no clock yet.

Not chosen yet: MQTT. It is common for sensors, but it needs a separate broker program. I'll look at it again in stage 5, together with keeping messages when the connection is down.

Will check: where the server runs in the shop (laptop, Raspberry Pi or the second ESP32 as a receiver), and how to stop other devices on the shop Wi-Fi from sending fake messages.

---

## 2026-11-26: Phone alerts through a Telegram bot on the ESP32 itself

Status: Provisional

Decision: Alerts and the "phone app" are a Telegram bot. For now the ESP32 talks to the Telegram Bot API directly over HTTPS, with no server in between. It answers only my chat, checks Telegram's certificate, and keeps the bot token in `secrets.h`, which is never committed. Details: [firmware/cold-room-bot](../firmware/cold-room-bot/README.md).

Why: Nobody in the shop has a computer that runs day and night. Telegram is already on my parents' phones, sends push notifications for free and needs no app of my own. A bot on the board is the fastest way to get real alerts.

Not chosen yet: my own phone app, and the server-in-the-cloud version. The gap of this version is that the board can't report its own power or internet loss. For that, something outside the shop has to notice the silence: a small rented server (VPS) running the receiver and the bot. That comes later.

Will check: the alarm limits for the cold room, after reading the ETC-974 setpoint and defrost settings, so a normal defrost doesn't raise an alarm.