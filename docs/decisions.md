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

Status: Provisional

Decision: The microcontroller goes into the office right behind the cold room wall and is powered from a normal socket. No hole is drilled between the office and the cold room.

Why: The office has a socket and stable internet only 2-3 m from the room, so I don't need any mains work. Drilling between the office and the room is not allowed.

Not chosen: A box outside next to the condensing unit. It would be in dust and weather and further from power and internet.

Will check: How the sensors inside reach the device (most likely wireless), the signal strength at each sensor point, and how long batteries last at -20 °C.
