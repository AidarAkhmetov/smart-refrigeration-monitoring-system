# Smart Refrigeration Monitoring System

A low-cost monitoring system for small cold rooms. The first goal is simple and reliable: measure the temperature, notice when the system goes quiet, and alert the owner's phone. The longer-term goal is to watch each main part of the refrigeration unit and catch problems before they turn into a breakdown.

## Current status

Research and requirements phase. I have surveyed the cold room, read the nameplates of all its refrigeration equipment, and written the requirements and a proposed architecture. No hardware has been bought, no code has been written and nothing has been tested yet. Progress is tracked in [docs/roadmap.md](docs/roadmap.md).

## Why I'm doing this

My parents have a small fish shop. It's a family business and we don't have employees. My dad takes care of orders, deliveries and documents, my mom works at the till and takes in and lays out the goods, and I work there on weekends selling. The shop is 30-40 minutes from our house, next to some warehouses where there are also fish processing, meat and chicken workshops with their own refrigerators.

All our goods are kept in one cold room. A ready-made monitoring system costs around 500-800 euros, which is a lot for a shop like ours. So instead my dad drives there once or twice every weekend just to check that the cold room is working.

From November 2 to November 9, 2026 my parents were on a business trip and I had to check the cold room. I went there all week and everything was fine. On Sunday, November 8, one day before they came back, I thought it had been working fine all week anyway, so there was no point going, and I stayed home. On the evening of the 9th my dad went to check it and found out it had broken down on Sunday, the exact day I didn't go. We had to throw away about 2000 euros worth of goods.

It was my fault and I felt really bad about it. That's why I decided to build a monitoring system for our cold room myself. I want my parents to stop spending their weekends driving there to look at a thermometer, and I don't want something like this to happen again.

## The problem

Small shops and workshops keep all their stock in one or two cold rooms, but they usually have no monitoring at all, because commercial systems cost more than they can justify. A failure is noticed only when someone comes by and looks. If that happens a day later, the stock is already lost.

## Objectives

Phase 1, first prototype:

1. Measure the air temperature reliably at several points inside the cold room.
2. Recognize normal defrost cycles, so the system doesn't send false alarms every few hours.
3. Send an alert to the owner's phone when the temperature stays out of range.
4. Send an alert when the device stops reporting (power cut, Wi-Fi down, dead sensor).
5. Keep a history of all readings and show it in a simple app.
6. Keep the hardware cost far below a commercial system.

Phase 2, research goal:

7. Watch the state of each main part (compressor, condenser fans, evaporator, defrost, power supply).
8. Log anomalies for each part and track how often they happen. If they become more frequent, the part probably needs service.
9. When something breaks, point to the most likely faulty part and the time it happened.

Phase 2 only starts after Phase 1 works reliably and I have collected enough normal operating data to know what "abnormal" looks like.

Expected outcome of Phase 1: a working, tested prototype installed in our new cold room, with documented hardware, firmware, server code, test results and real cost.

## Scope

One cold room in my parents' shop. The small cold room that broke down is going to be taken down and replaced by a larger one (about 106 m2 of floor area), so the system is designed for the new room. It is one refrigerated space, treated as one cold room with several measurement points spread across it.

If the system works well there, I want to offer it to the other shops and workshops near the warehouses later. That is outside the scope of this project for now.

## Equipment

| Part | Model and main data | How I know |
|---|---|---|
| Room | One room, 106.2 m2 floor area, 3.0 m ceiling, one sliding door | Building plan, photos |
| Room temperature | Freezer, -18 to -20 °C | Family (to be confirmed with my sensors) |
| Condensing unit | Holodom ASP-F-LV25103Y-1-1204, 380 V 3-phase, 28.7 kW cooling at -25 °C | Nameplate |
| Compressor | Frascold V25-103Y, semi-hermetic, max. 52.3 A | Nameplate |
| Condenser | FNH-49.9/180, 4 fans | Nameplate, photos |
| Evaporator | Holodom HLD.E-450-36/28-6, 4 fans, 16.8 kW electric defrost | Nameplate |
| Refrigerant | R404A | Nameplate, purchase receipt |
| Controller | Elitech ETC-974 (temperature and defrost controller), no data port | Packaging label and manual |

Details, and everything that is still unverified, are in [docs/site-survey.md](docs/site-survey.md).

## Ground rules

1. The system only watches. It never controls, reprograms or bypasses the existing controller. If my device fails, the cold room keeps working the same way as before.
2. The microcontroller is never connected directly to 230 V or 380 V. Anything on the mains side has to be isolated and installed by an electrician.
3. If a value isn't confirmed by a nameplate, a manual or a measurement, I mark it as unverified.

## Proposed architecture

```
temperature sensors in the cold room
(later: sensors on each part of the unit)
                 |   no new hole in the wall, so probably wireless
                 v
   microcontroller (probably ESP32) in the office
   behind the cold room wall, plugged into a socket
                 |   internet, stores data locally if it is down
                 v
           server (Python)
     checks data, decides when to alert
                 |
              database
                 |
     app: live values, history, notifications
```

This is a proposal, not a final design. Details and open questions: [docs/architecture.md](docs/architecture.md).

## Development plan

Hardware: choose temperature sensors that work at -30 °C and survive defrost moisture, test them on my desk against a reference thermometer, then design a small low-voltage box with the microcontroller and power supply. For Phase 2, choose safe ways to detect if the compressor and fans are running (for example current clamps installed by an electrician).

Software: microcontroller firmware that reads sensors, keeps data when Wi-Fi is down and sends it later. A Python server that checks and stores the readings and sends alerts. A simple app to see current values and history.

The full step-by-step plan is in [docs/roadmap.md](docs/roadmap.md), and the reasons behind design choices are in [docs/decisions.md](docs/decisions.md).

## Testing

Every part gets tested before it goes into the real cold room: sensor accuracy against a reference thermometer, behavior when a sensor is unplugged, when Wi-Fi drops and when power is cut, and false alarms during real defrost cycles. The plan is in [docs/testing-plan.md](docs/testing-plan.md). No tests have been done yet.

## Documentation

Done so far:

- [Site survey](docs/site-survey.md)
- [Requirements](docs/requirements.md)
- [Proposed architecture](docs/architecture.md)
- [Roadmap](docs/roadmap.md)
- [Decision log](docs/decisions.md)
- [Testing plan](docs/testing-plan.md)

Will be added when the work gets there: hardware design and parts list with real prices, firmware, server, experimental results, calibration and validation, and known limitations.

## Author

Aidar Akhmetov
