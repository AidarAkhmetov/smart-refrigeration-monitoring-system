# Site survey

My notes from going through the installation and taking photos of every nameplate I could reach. If I couldn't read a value clearly, I wrote that it's unverified. I'll update this as I get better photos and the manuals.

Date: November 17, 2026

## The room

The cold room in my parents' fish shop is used as a freezer for all the goods. It's split into three parts:

- the main chamber, about half of the room
- a smaller second chamber connected to the main one, about a quarter
- the entrance, the last quarter

Ceiling height is 3.5 m. Normally the temperature is around -18 to -20 °C. During defrost it goes up, I've heard numbers up to about +10 °C, but I'm not sure if that's the air in the room or the temperature of the evaporator coil. I'll check the real setpoint and defrost settings in the controller menu (only looking, not changing anything).

All the equipment below belongs to this one cold room. Outside there is one condensing unit, and my close-up photos are all parts of that same unit.

## Equipment

### Controller: Elitech ETC-974

- Power: 230 VAC +/-10%, 50/60 Hz
- Working temperature: -5 to +55 °C
- NTC probe, about +/-0.5 °C, 2 m cable

There's one controller. I still need the exact version and the official manual. Until I have the manual I'm not assuming it has RS-485, Modbus or any other way to read data from it.

### Compressor: InvoTech hermetic scroll

- 380-420 V, 3-phase, 50 Hz (or 460 V / 60 Hz)
- MRC 12.2 A, LRC 60 A
- Max pressure H 3.2 / L 2.0 MPa
- POE oil

The nameplate is dirty and a pipe clamp covers the beginning of the model number, so I don't know the full model yet. LRC is the locked rotor current (the current at start-up when the motor isn't turning yet), not the normal running current, so I won't pick a current sensor based on these numbers until I have a clean nameplate and the datasheet.

### Condenser: FNH-49.9/180

- Capacity on the label: 49.9 kW, surface 180 m2
- Air-cooled, installed outside
- Two fans. One was recently replaced with a YWF4E-400S (220 V, 50 Hz, 4 poles, 180 W). I don't know the model of the second one yet.

Funny thing: the nameplate says "Unit Cooler", but in supplier catalogs the FNH series is listed as air-cooled condensers, and it's obviously mounted outside as part of the condensing unit.

The outdoor unit also has a liquid receiver and its own electrical box. Both nameplates came out blurry in my photos. I don't open the electrical box, that's a job for an electrician.

### Evaporator: DD-6.0/30

- Capacity on the label: 6.0 kW, surface 30 m2
- DD series means low-temperature unit coolers, which fits a freezer

This is the only evaporator in the room. I still have to check exactly where it hangs and how the cold air gets into the second chamber.

## Still need to find out

1. Exact ETC-974 version and its manual
2. Full compressor model (need a clean photo of the nameplate)
3. Model of the second condenser fan
4. Refrigerant type
5. Real setpoint and defrost settings from the controller
6. Where the evaporator is and how air gets to the second chamber
7. Where there's Wi-Fi and where I can get low-voltage power
