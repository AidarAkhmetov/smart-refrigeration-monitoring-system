# Requirements

First version. It will change once I know the real controller settings and what's possible on site.

## Room

| ID | Requirement | Priority |
|---|---|---|
| R-01 | Measure air temperature in the room without affecting the existing controller | High |
| R-02 | Range at least -30 to +20 °C so both normal work and defrost are covered | High |
| R-03 | Separate temperature sensors for the main chamber and the second chamber | Medium |
| R-04 | Recognize defrost and don't send a high temperature alarm during normal defrost, but do send it if the temperature doesn't come back down in time | High |

## Equipment health

| ID | Requirement | Priority |
|---|---|---|
| R-05 | Track the state of each main part separately: compressor, each condenser fan, evaporator fan, defrost, power supply | High |
| R-06 | Detect when each part is running, only with isolated or non-contact sensors | High |
| R-07 | Log anomalies for each part (too many starts per hour, running too long, current higher or lower than normal, defrost taking longer than usual, etc.) | High |
| R-08 | Count how often anomalies happen on each part and warn when they get more frequent, meaning the part may need service soon | Medium |
| R-09 | When something breaks, say which part is most likely at fault and when it happened | High |

## Notifications and app

| ID | Requirement | Priority |
|---|---|---|
| R-10 | Send a notification to the owner's phone within a few minutes, even when nobody is at the shop | High |
| R-11 | Send an alarm when the device stops sending data (power cut, Wi-Fi down, broken sensor) | High |
| R-12 | One app with the current state of each part, history and the list of anomalies | High |
| R-13 | Alarm limits and delays can be changed in the app | Medium |

## Data

| ID | Requirement | Priority |
|---|---|---|
| R-14 | Every reading has a time (UTC), a unit and a sensor ID | High |
| R-15 | Detect missing, old or impossible readings | High |
| R-16 | Keep data on the device when Wi-Fi is down and send it later | Medium |

## Cost and installation

| ID | Requirement | Priority |
|---|---|---|
| R-17 | Hardware cost much lower than a ready-made system (500-800 euros). Exact budget after I choose the sensors | High |
| R-18 | The low-voltage part can be installed by a normal person, anything on mains is done by an electrician | Medium |

## Open questions

- What accuracy is really needed? +/-0.5 °C seems fine for a freezer, need to confirm.
- How often to take readings? Probably every 30-60 seconds, but defrost and compressor starts may need faster.
- How should notifications come: Telegram, SMS, a call? It has to be something my parents will actually notice on a weekend.
- If the shop has no power at all, the device can't send anything. So the server has to notice that the device went quiet (R-11).
- What counts as "normal" for each part? I'll need a few weeks of data from the working unit first.
