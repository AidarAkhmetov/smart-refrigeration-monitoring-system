# Requirements

Draft. It will change once I know the real controller settings and what's possible on site.

Scope: one large freezer room (106.2 m2 floor area, 3.0 m ceiling, -18 to -20 °C) with one evaporator and several measurement points spread across it. All equipment is listed in the [site survey](site-survey.md).

## Phase 1: first prototype

### Temperature

| ID | Requirement | Priority | How I'll check it |
|---|---|---|---|
| R-01 | Measure air temperature at several points in the cold room without affecting the existing controller | High | Installed sensors are fully separate from the controller and its probe |
| R-02 | At least these points: near the evaporator air outlet, the far end of the room from the evaporator, near the door, and the middle of the room. Exact places chosen once the evaporator position is marked on the plan | High | One sensor per point, each with its own ID; positions marked on a room sketch |
| R-03 | Range at least -30 to +20 °C | High | Sensor datasheet and bench test |
| R-04 | Accuracy about +/-0.5 °C (to be confirmed) | High | Comparison with a reference thermometer |
| R-05 | Recognize defrost (the evaporator has 16.8 kW electric heaters) and don't alarm during a normal one, but alarm if the temperature doesn't come back down in time | High | Count false alarms over several weeks of real defrost cycles |

### Data

| ID | Requirement | Priority | How I'll check it |
|---|---|---|---|
| R-06 | Every reading has a time (UTC), a unit and a sensor ID | High | Server rejects readings without them |
| R-07 | Detect missing, old or impossible readings | High | Tests with unplugged sensor and fake values |
| R-08 | Keep data on the device when Wi-Fi is down and send it later | High | Turn off Wi-Fi for an hour, check that no data is lost |

### Alerts and app

| ID | Requirement | Priority | How I'll check it |
|---|---|---|---|
| R-09 | Send a notification to the owner's phone within a few minutes of a problem | High | Measure time from a simulated problem to the phone |
| R-10 | Alert when the device stops sending data (power cut, Wi-Fi down, broken device) | High | Unplug the device, check that the alert comes |
| R-11 | Simple app with current values and history | Medium | Used by my parents for a few weeks |
| R-12 | Alarm limits and delays can be changed | Medium | Change a limit, check it takes effect |

### Cost and installation

| ID | Requirement | Priority | How I'll check it |
|---|---|---|---|
| R-13 | Hardware cost far below a ready-made system (500-800 euros). Exact budget after choosing sensors | High | Parts list with real prices |
| R-14 | The low-voltage part can be installed without special skills. Anything on mains is done by an electrician | Medium | Written installation steps |
| R-15 | No new hole in the wall between the office and the cold room. The main device sits in the office and is powered from a normal socket with a plug-in adapter | High | Installation photos |
| R-16 | Sensors inside the room, including their power source, work for months at -20 °C | High | Freezer test before installation |

## Phase 2: research goals

These start only after Phase 1 works reliably and I have several weeks of normal data.

| ID | Goal |
|---|---|
| G-01 | Detect when the compressor and each condenser fan are running, only with isolated or non-contact sensors |
| G-02 | Track defrost start and duration |
| G-03 | Log anomalies for each part (too many starts per hour, running too long, unusual current, defrost taking longer than usual) |
| G-04 | Track how often anomalies happen on each part and warn when they become more frequent |
| G-05 | When something breaks, show the most likely faulty part and the time |

Whether G-03 to G-05 are possible with cheap sensors is an open research question. I'll report what works and what doesn't.

## Open questions

- The room is kept at -18 to -20 °C, but I haven't read the exact setpoint on the controller. The alarm limits depend on it, so I'll check it with my own sensors first.
- How do the sensors inside the room reach the device in the office without a hole in the wall? Wireless is the likely answer, but batteries lose capacity in the cold.
- How many measurement points does a room this size really need? Probably more than four, especially if goods are stacked high. To be decided after looking at how the air moves in the finished room.
- Is +/-0.5 °C really needed, or is +/-1 °C enough for a freezer?
- How often to take readings? Probably every 30-60 seconds.
- How should notifications come: Telegram, SMS, a call? It has to be something my parents will actually notice on a weekend.
- If the shop has no power at all, the device can't send anything. So the server has to notice the silence (R-10).
