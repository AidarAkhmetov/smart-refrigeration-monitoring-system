# Site survey

My notes from going through the installation and taking photos of every nameplate I could reach. Each value is marked by where it comes from. I'll update this as I get better photos and the manuals.

Date: November 17, 2026

Where the information comes from:

- Nameplate: read from the label on the equipment.
- Receipt: from the purchase document.
- Family: what my parents told me or what I know from working there. Needs to be checked on site.
- Unverified: I'm not sure yet.

## The room

The cold room in my parents' fish shop is used as a freezer for all the goods. It's one refrigerated space with:

- the main chamber, about half of the room
- a smaller second chamber connected to the main one, about a quarter
- the entrance, the last quarter

| What | Value | Source |
|---|---|---|
| Ceiling height | 3.5 m | Building plan |
| Normal temperature | Around -18 to -20 °C | Family |
| Temperature during defrost | Up to about +10 °C | Family, unverified. Could be the evaporator coil temperature, not the room air |
| How cold air gets into the second chamber | Unknown | Unverified |

I'll check the real setpoint and defrost settings in the controller menu (only looking, not changing anything).

## Photos

All the outdoor close-up photos show parts of one and the same outdoor refrigeration unit. I took them close so the labels could be read. The wide outdoor photos show that same unit as a whole. The indoor photos show the evaporator. The photos are not in this repository.

## Equipment

### Controller: Elitech ETC-974

| What | Value | Source |
|---|---|---|
| Power | 230 VAC +/-10%, 50/60 Hz | Packaging label |
| Working temperature | -5 to +55 °C | Packaging label |
| Probe | NTC, about +/-0.5 °C, 2 m cable | Packaging label |
| Exact version and manual | Unknown | Unverified |
| Data interface (RS-485, Modbus, etc.) | Unknown | Unverified. Not assuming any until the manual confirms it |

My family says there is one controller for this cold room. I have two photos of ETC-974 panels that look a bit different, so I still need to check on site which panel belongs to this unit.

### Compressor: InvoTech hermetic scroll

| What | Value | Source |
|---|---|---|
| Power | 380-420 V, 3-phase, 50 Hz (or 460 V / 60 Hz) | Nameplate |
| MRC (max running current) | 12.2 A | Nameplate |
| LRC (locked rotor current) | 60 A | Nameplate |
| Max pressure | H 3.2 / L 2.0 MPa | Nameplate, partly readable |
| Oil | POE | Nameplate |
| Full model number | Unknown | A pipe clamp covers the start of it |

LRC is the current at start-up when the motor isn't turning yet, not the normal running current. I won't pick a current sensor based on these numbers until I have a clean nameplate and the datasheet.

### Condenser: FNH-49.9/180

| What | Value | Source |
|---|---|---|
| Capacity on the label | 49.9 kW | Nameplate |
| Surface | 180 m2 | Nameplate |
| Location | Outdoors, part of the outdoor unit | Photos and family |

Funny thing: the nameplate says "Unit Cooler", but in supplier catalogs the FNH series is listed as air-cooled condensers, and it's obviously mounted outside as part of the outdoor unit.

### Condenser fan: YWF4E-400S

| What | Value | Source |
|---|---|---|
| Power | 220 V, 50 Hz, 180 W, 4 poles | Receipt and motor label |
| Location | The black fan on the outdoor unit | Family and photos |

It was bought and installed recently. My family says the condenser has two fans. I don't know the model of the other one yet, and I haven't counted them myself.

### Evaporator: DD-6.0/30

| What | Value | Source |
|---|---|---|
| Capacity on the label | 6.0 kW | Nameplate |
| Surface | 30 m2 | Nameplate |
| Type | DD series, low-temperature unit cooler, fits a freezer | Nameplate and catalogs |
| Exact position in the room | Unknown | Unverified |

It's the only evaporator I found in the cold room. I still have to check whether there is anything else cooling the second chamber.

### Other parts of the outdoor unit

There is a liquid receiver and an electrical box. Both nameplates came out blurry. I don't open the electrical box, that's a job for an electrician.

## Not verified yet

I'm not assuming exact numbers of controllers, evaporators, fans or refrigerant circuits until I check them myself on site or find them in the documentation.

1. Exact ETC-974 version and its manual
2. Which ETC-974 panel belongs to this cold room
3. Full compressor model (need a clean photo of the nameplate)
4. Number of condenser fans and the model of the second one
5. Refrigerant type
6. Number of refrigerant circuits
7. Real setpoint and defrost settings from the controller
8. Where the evaporator is and how air gets to the second chamber
9. Where there's Wi-Fi and where I can get low-voltage power

Equipment bought around the same time for a different refrigerator is not part of this project and is not listed here.
