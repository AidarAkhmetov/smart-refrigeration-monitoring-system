# Roadmap

Scope: one large cold room (about 106 m2) that replaces the old small one.

## Phase 1: monitoring prototype

| # | Stage | Finished when | Status |
|---|---|---|---|
| 1 | Site survey and requirements | Equipment identified, requirements written | Done (Nov 22, 2026) |
| 2 | Exact equipment data | All nameplates read; setpoint and defrost settings read from the controller; evaporator and door positions marked on a room sketch; Wi-Fi signal measured inside the room | In progress (nameplates done) |
| 3 | Sensor choice and safety check | Temperature sensors chosen; anything on mains checked by an electrician | Not started |
| 4 | Bench prototype | Sensors compared with a reference thermometer on my desk, low voltage only | In progress: parts chosen ([hardware/bench-prototype.md](../hardware/bench-prototype.md)) |
| 5 | Firmware | Reads sensors, stores data offline, survives unplugged sensor, Wi-Fi loss and restart | Not started |
| 6 | Server | Receives, checks and stores data, has automated tests | Not started |
| 7 | App and notifications | Live values, history, phone alerts, "device offline" alert | Not started |
| 8 | Pilot in the new cold room | Battery sensors run for 2-3 weeks; false alarms during defrost counted and fixed | Not started |
| 9 | Final installation | One small sealed hole near the door, wired sensors, no ice around the cable after a month | Not started |

## Phase 2: equipment health (research)

| # | Stage | Finished when | Status |
|---|---|---|---|
| 10 | Equipment state sensing | Compressor and fan running state measured safely | Not started |
| 11 | Normal data | Several weeks of data, so I know what "normal" looks like for each part | Not started |
| 12 | Anomalies and fault hypotheses | Anomalies logged per part; fault hypotheses from the architecture tested on real data | Not started |

I don't start the next stage until the current one is really done. Phase 2 starts only after the pilot in stage 8 works.
