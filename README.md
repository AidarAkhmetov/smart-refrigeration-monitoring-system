# Smart Refrigeration Monitoring System

A cheap monitoring system for small cold rooms. It keeps an eye on every main part of the refrigeration unit, warns the owner when something breaks or starts to go wrong, and shows roughly where the problem is.

Status: very early. Right now I'm studying the real installation and writing down requirements. No hardware bought and no code written yet. See [docs/roadmap.md](docs/roadmap.md).

## Why I'm doing this

My parents have a small fish shop. It's a family business and we don't have employees. My dad takes care of orders, deliveries and documents, my mom works at the till and takes in and lays out the goods, and I work there on weekends selling. The shop is 30-40 minutes from our house, next to some warehouses where there are also fish processing, meat and chicken workshops with their own refrigerators.

All our goods are kept in one cold room. A ready-made monitoring system costs around 500-800 euros, which is a lot for a shop like ours. So instead my dad drives there once or twice every weekend just to check that the cold room is working.

From November 2 to November 9, 2026 my parents were on a business trip and I had to check the cold room. I went there all week and everything was fine. On Sunday, November 8, one day before they came back, I thought it had been working fine all week anyway, so there was no point going, and I stayed home. On the evening of the 9th my dad went to check it and found out it had broken down on Sunday, the exact day I didn't go. We had to throw away about 2000 euros worth of goods.

It was my fault and I felt really bad about it. That's why I decided to build a monitoring system for our cold room myself. I want my parents to stop spending their weekends driving there to look at a thermometer, and I don't want something like this to happen again.

## What I want to build

Not just a thermometer that sends a message. I want the system to watch every important part of the unit: the compressor, the condenser fans, the evaporator, defrost, the room temperature and the power supply.

The idea:

- If one part starts behaving strangely, for example the compressor turns on and off too often or a fan draws more current than usual, the system logs it as an anomaly.
- If the anomalies on one part keep getting more frequent, that part is probably close to failing or needs service. Then you can call a technician before it breaks, not after.
- If something does break, the owner gets a notification right away with the time and the most likely place of the fault (for example "condenser fan 2 stopped" instead of just "temperature too high"). That way you can react quickly and save the goods.
- Everything is in one app, so you can check the cold room from home.

The first system goes into my parents' cold room. If it works well there, I want to offer it to the other shops and workshops near the warehouses, because they have the same problem and also can't pay for expensive systems.

## The installation

The first test site is the real cold room in our shop:

| Part | Model |
|---|---|
| Controller | Elitech ETC-974 (temperature and defrost controller) |
| Compressor | InvoTech hermetic scroll compressor, 3-phase 380-420 V |
| Condenser | FNH-49.9/180 air-cooled condenser with two fans (one was recently replaced with a YWF4E-400S) |
| Evaporator | DD-6.0/30 low-temperature unit cooler inside the room |
| Room | Main chamber and a smaller second chamber connected to it, around -18 to -20 °C |

More details in [docs/site-survey.md](docs/site-survey.md).

## Rules I follow

1. The system only watches. It never controls, reprograms or bypasses the existing controller. If my device fails, the cold room keeps working the same way as before.
2. The microcontroller is never connected directly to 230 V or 380 V. Anything on the mains side has to be isolated and installed by an electrician.
3. If a value isn't confirmed by a nameplate, a manual or a measurement, I mark it as unverified.

## Planned structure

```
sensors on each part of the unit + room temperature
                 |
        microcontroller (probably ESP32)
                 |   Wi-Fi, stores data locally if Wi-Fi is down
                 v
           server (Python)
   checks data, finds anomalies, guesses the fault
                 |
              database
                 |
     app: live status, history, notifications
```

Details and open questions are in [docs/architecture.md](docs/architecture.md). Every important decision and the reason for it goes into [docs/decisions.md](docs/decisions.md).

## Folders

| Folder | What's inside |
|---|---|
| `docs/` | Site survey, requirements, architecture, roadmap, decisions |
| `hardware/` | Sensors, datasheets, wiring notes (later) |
| `firmware/` | Microcontroller code (later) |
| `backend/` | Server code (later) |
| `dashboard/` | The app (later) |

## Author

Aidar Akhmetov
