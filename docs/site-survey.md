# Site survey

My notes about the cold room and its refrigeration equipment. Each value is marked by where it comes from. I'll update this when I get more data.

First version: November 17, 2026. Updated: November 22, 2026, after I photographed every nameplate of the installation.

Where the information comes from:

- Nameplate: read from the label on the equipment.
- Packaging label / manual: from the controller's box and its manual.
- Receipt: from the purchase document.
- Building plan: from the floor plan of the building.
- Family: what my parents told me or what I know from working there.
- Unverified: I'm not sure yet.

## Change of target room

At first I surveyed the small cold room that broke down in November. It turned out that this small room is going to be taken down. A much larger cold room replaces it, and this is the room the monitoring system is built for. The goal stays the same: keep the goods safe and warn us early when something goes wrong.

What I wrote earlier about the small room no longer applies to this project. The old version is still in the Git history.

## The room

One large refrigerated room, already in use. On the building plan it is not a rectangle but a four-sided room that is narrower at one end.

| What | Value | Source |
|---|---|---|
| Wall lengths | 12.36 m, 7.11 m, 11.75 m, 10.73 m (going around the room: left, top, right, bottom on the plan) | Building plan |
| Floor area | 106.2 m2 | Building plan |
| Ceiling height | 3.0 m | Building plan |
| Volume | About 319 m3 (106.2 m2 x 3.0 m) | My calculation |
| Inside | Open room with two columns, pallets and shelves. No inner walls | Photos |
| Door | One large sliding insulated door to the yard, with a plastic strip curtain inside | Photos |
| Storage temperature | -18 to -20 °C (frozen fish) | Family. I'll confirm the real value with my own sensors |

Quick check of the plan: if the 7.11 m and 10.73 m walls are parallel, the other two walls put them about 11.75 m apart, and the area comes out at about 105 m2. That is close to the 106.2 m2 on the plan, so the numbers fit together.

The room is about 4-5 times larger than the old one, and it has only one evaporator. So one probe can't represent the whole room. I'll need several measurement points, including the far end from the evaporator and the area near the door.

## How the installation works

```
OUTSIDE, under a canopy                          INSIDE THE COLD ROOM

condensing unit (Holodom)                         evaporator HLD.E-450-36/28-6
  compressor Frascold V25-103Y                      4 fans, electric defrost heaters,
  condenser FNH-49.9/180, 4 fans  <-- pipes -->     expansion valve
  receiver, oil separator,           through        hangs on the wall under the ceiling
  pressure switches, gauges          the wall

controller ETC-974 (on the outside wall, next to the unit)
```

This is a simple picture of what I saw, not a wiring or piping diagram.

## Equipment

### Condensing unit: Holodom ASP-F-LV25103Y-1-1204

Made by Holodom, a refrigeration company in Almaty.

| What | Value | Source |
|---|---|---|
| Power supply | 380 V, 3 phases, 50 Hz | Nameplate |
| Installed power | 28.8 kW | Nameplate |
| Power consumption | 18 kW | Nameplate |
| Cooling capacity | 28.7 kW at -25 °C evaporating / +40 °C condensing | Nameplate |
| Refrigerant | R404A (the plate says "R404", the receipt for the refrigerant says R404A) | Nameplate, receipt |
| Size and weight | 1500 x 1200 x 1500 mm, 431 kg | Nameplate |
| Front panel | Main switch and indicator lamps, labelled "power", "high pressure switch" and "thermal protection" (two lamps have no readable label) | Photos |

### Compressor: Frascold V25-103Y

Semi-hermetic piston compressor, made in Italy.

| What | Value | Source |
|---|---|---|
| Power supply | 380-420 V, 3 phases, 50 Hz | Nameplate |
| Displacement | 102.9 m3/h at 1450 rpm (50 Hz) | Nameplate |
| Maximum operating current | 52.3 A | Nameplate |
| Locked rotor current (start) | 118.3 A or 202.7 A, depending on how the motor is connected | Nameplate |
| Oil | POE68 | Nameplate |
| Maximum discharge pressure | 30 bar | Nameplate |

The current values matter for Phase 2: if I measure the compressor current later, the sensor has to handle this range. The real running current will be lower than the maximum, and I'll only know it from a measurement.

### Condenser: FNH-49.9/180

| What | Value | Source |
|---|---|---|
| Heat rejection | 49.9 kW | Nameplate |
| Surface | 180 m2 | Nameplate |
| Fans | 4 axial fans | Photos |
| Fan model and voltage | Unknown | Unverified |

The plate on it says "Unit Cooler", but it is the outdoor air-cooled condenser of this unit.

### Other parts of the outdoor unit

| Part | What I know | Source |
|---|---|---|
| Liquid receiver (GVN, Turkey) | 25 liters, working pressure 33 bar | Nameplate |
| Oil separator (GVN) | 7/8" connections | Nameplate |
| A third small vessel (GVN) | 33 bar. Its type is on the part of the label I couldn't read | Nameplate, partly |
| Pressure switches (Ranco) | Two units with scales | Photos. Models and settings unknown |
| Pressure gauges | Three on the unit | Photos |

### Evaporator: Holodom HLD.E-450-36/28-6

One evaporator for the whole room.

| What | Value | Source |
|---|---|---|
| Cooling capacity | 36 kW at -8 °C, 28 kW at -25 °C | Nameplate |
| Surface | 160 m2 | Nameplate |
| Fans | 4 fans, 500 mm | Nameplate |
| Defrost | Electric heaters, 16.8 kW | Nameplate |
| Size and weight | 3510 x 670 x 660 mm, 148 kg | Nameplate |
| Mounting | On the wall under the ceiling, thermostatic expansion valve at the inlet, drain pipe through the wall | Photos |
| Position on the plan | Not marked yet | Unverified |

The 16.8 kW defrost heaters are important for the monitoring: during each defrost the air near the evaporator gets warmer for a while. That is normal, and the system must not send an alarm for it.

### Controller: Elitech ETC-974

It hangs on the outside wall next to the condensing unit, in a box with two switches. When I took the photo, it showed -18 °C.

| What | Value | Source |
|---|---|---|
| Power | 230 VAC +/-10%, 50/60 Hz | Packaging label |
| Working temperature | -5 to +55 °C | Packaging label |
| Probe | NTC, -50 to +120 °C, 2 m cable | Packaging label |
| Accuracy | +/-0.5 °C from -30 to +50 °C | Packaging label |
| Data interface | None. The manual only mentions a copy card for settings, no RS-485 or Modbus | Manual |
| Setpoint and defrost settings | Not read yet | Unverified |

Because the controller has no data port, the monitoring system uses its own sensors and doesn't connect to the controller at all.

## Where the monitoring device can go

- Right behind the cold room wall, next to the door, there is an office with a power socket and stable internet, 2-3 m from the room.
- We are not allowed to drill a hole between the office and the cold room.
- Wi-Fi and mobile signal work inside the cold room. I still have to measure how strong the signal is at the places where the sensors will be.

So the main box with the microcontroller can sit in the office and plug into the socket with a normal adapter. The sensors inside the room have to reach it without a new hole in the wall. They could be wireless, but their batteries then have to work at -20 °C. I'll choose this in the sensor stage.

## Not known yet

1. Where exactly the evaporator hangs on the plan, so I can choose the sensor points
2. The controller's setpoint and defrost settings (I'll only read them, not change them)
3. Fan models of the condenser and the evaporator
4. The settings of the pressure switches
5. Wi-Fi signal strength inside the room at the sensor points
