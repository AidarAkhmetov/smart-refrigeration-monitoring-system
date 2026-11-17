# Site survey

My notes about the cold room and the equipment I have access to. Each value is marked by where it comes from. I'll update this as I get nameplate photos and manuals.

Date: November 17, 2026

Where the information comes from:

- Nameplate: read from the label on the equipment.
- Packaging label / manual: from the controller's box and its manual.
- Receipt: from the purchase document.
- Building plan: from the floor plan of the building.
- Family: what my parents told me or what I know from working there. Needs to be checked on site.
- Unverified: I'm not sure yet.

## Change of target room

At first I surveyed the small cold room that broke down in November. It turned out that this small room is going to be taken down. A much larger cold room replaces it, and this is the room the monitoring system is built for. The goal stays the same: keep the goods safe and warn us early when something goes wrong.

What I wrote earlier about the small room (its compressor, condenser and evaporator) no longer applies to this project, so it was removed from this survey. The old version is still in the Git history.

## The room

One large refrigerated room. On the building plan it is not a rectangle but a four-sided room that is narrower at one end.

| What | Value | Source |
|---|---|---|
| Wall lengths | 12.36 m, 7.11 m, 11.75 m, 10.73 m (going around the room: left, top, right, bottom on the plan) | Building plan |
| Floor area | 106.2 m2 | Building plan |
| Ceiling height | 3.5 m | Building plan, given for the whole floor. To check in this room |
| Volume | About 370 m3 (106.2 m2 x 3.5 m) | My calculation, depends on the ceiling height |
| Inner partitions, door position | Unknown | Unverified |
| Planned temperature | Not decided yet. We keep frozen fish, so probably a freezer like the old room | Family, unverified |

Quick check of the plan: if the 7.11 m and 10.73 m walls are parallel, the other two walls put them about 11.75 m apart, and the area comes out at about 105 m2. That is close to the 106.2 m2 on the plan, so the numbers fit together.

The room is about 4-5 times larger than the old one. That matters for the sensors: one probe can't represent the whole room, so I'll need several measurement points spread across it, including the far end from the evaporator and the area near the door.

## Equipment I have access to

### Controller: Elitech ETC-974

| What | Value | Source |
|---|---|---|
| Power | 230 VAC +/-10%, 50/60 Hz | Packaging label |
| Working temperature | -5 to +55 °C | Packaging label |
| Probe | NTC, -50 to +120 °C, 2 m cable | Packaging label |
| Accuracy | +/-0.5 °C from -30 to +50 °C | Packaging label |
| Display resolution | 1 °C or 0.1 °C | Packaging label |
| Data interface | None. The manual only mentions a copy card for settings, no RS-485 or Modbus | Manual |
| Which room it will control | Not confirmed for the new room yet | Unverified |

Because the controller has no data port, the monitoring system uses its own sensors and doesn't connect to the controller at all.

### Evaporator: HLD-450-36/28-6

| What | Value | Source |
|---|---|---|
| Model | HLD-450-36/28-6 | Receipt |
| Quantity | 1 | Receipt |
| Capacity, fans, voltage, defrost type | Unknown | Need the nameplate or the datasheet |
| Position in the room | Unknown | Unverified |

It was bought in November 2026 for the new room. I'm not guessing its capacity or fan data from the model name. I'll read them from the nameplate once it's installed, or find the manufacturer's datasheet.

### Refrigerant: R404A

| What | Value | Source |
|---|---|---|
| Type | R404A | Receipt |
| Amount bought | 2 cylinders, 10.9 kg each | Receipt |

Bought together with the evaporator. I'll confirm with the technician that this is the refrigerant the system is charged with.

### Fan: YWF4E-400S

| What | Value | Source |
|---|---|---|
| Power | 220 V, 50 Hz, 180 W, 4 poles, insulation class F | Receipt |
| Type | Axial fan, suction type | Receipt |
| Where it will be installed | Unknown | Unverified |

## Not known yet

The new room is not finished, so a lot of the installation can't be checked yet. I'm not assuming anything about these until I see them on site or in the documentation:

1. Compressor or condensing unit: model, power supply, current
2. Condenser, and the number and models of its fans
3. Where the YWF4E-400S fan goes
4. Number of evaporators and where they hang
5. Defrost type (electric heaters or another method) and defrost settings
6. Planned setpoint
7. Whether the ETC-974 will control the new room, and its exact version
8. Number of refrigerant circuits
9. Where there's Wi-Fi and where I can get low-voltage power near the room
