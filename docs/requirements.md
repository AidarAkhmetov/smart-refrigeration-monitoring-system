# Requirements

Draft. It will change once I know the real controller settings and what's possible on site.

Scope: one cold room (main chamber with a smaller second chamber connected to it) with several measurement points inside it.

## Phase 1: first prototype

### Temperature

| ID | Requirement | Priority | How I'll check it |
|---|---|---|---|
| R-01 | Measure air temperature at several points in the cold room without affecting the existing controller | High | Installed sensors are fully separate from the controller and its probe |
| R-02 | At least these points: main chamber, second chamber, near the evaporator air outlet | High | One sensor per point, each with its own ID |
| R-03 | Range at least -30 to +20 °C | High | Sensor datasheet and bench test |
| R-04 | Accuracy about +/-0.5 °C (to be confirmed) | High | Comparison with a reference thermometer |
| R-05 | Recognize defrost and don't alarm during a normal one, but alarm if the temperature doesn't come back down in time | High | Count false alarms over several weeks of real defrost cycles |

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

- Is +/-0.5 °C really needed, or is +/-1 °C enough for a freezer?
- How often to take readings? Probably every 30-60 seconds.
- How should notifications come: Telegram, SMS, a call? It has to be something my parents will actually notice on a weekend.
- If the shop has no power at all, the device can't send anything. So the server has to notice the silence (R-10).
