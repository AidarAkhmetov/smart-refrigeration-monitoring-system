# Decisions

I only add new entries at the bottom and don't change old ones, so later I can see why something was decided. If a decision changes, I add a new entry that replaces the old one.

Status:

- Confirmed: decided, won't change without a strong reason.
- Provisional: current choice, may change after testing.

---

## 2026-11-17: The system only watches

Status: Confirmed

Decision: The system only monitors. It never controls, reprograms or bypasses the Elitech controller or any safety circuit.

Why: The cold room holds all the goods of the shop. If my device has a bug or loses power, the unit has to keep working like before. It also keeps me away from mains wiring that I'm not qualified to work on.

Not chosen: Reading data directly from the controller. Maybe later, but only if the manual confirms it has a supported interface.

---

## 2026-11-17: One cold room, several measurement points

Status: Confirmed

Decision: The installation is treated as one cold room with several measurement points (main chamber, second chamber, evaporator outlet), not as separate rooms.

Why: The main and the second chamber are one refrigerated space served by one unit. Splitting them into separate "rooms" would make the data and the app more complicated for no reason.

---

## 2026-11-17: Only equipment from this cold room

Status: Confirmed

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
