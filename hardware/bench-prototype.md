# Stage 4: First build on the desk

**Status:** 🔄 board tested, waiting for the temperature probes · Previous: [stage 3, sensor choice](../docs/roadmap.md) · Next: stage 5, firmware · All stages: [roadmap](../docs/roadmap.md)

The first thing I build: one microcontroller and a temperature sensor on my desk at home. Everything runs from the laptop's USB port (5 V), so there is no mains wiring and nothing touches the cold room yet.

Goal of this stage: read the temperature, show it on the laptop, and check how accurate the sensors are at room temperature, in ice water (0 °C) and in a home freezer (around -18 °C).

## Parts

Chosen on November 22, 2026. Prices will be added from the receipts after ordering.

| Part | Qty | Why this one | Where | Price |
|---|---:|---|---|---:|
| ESP32-WROOM-32U board, USB Type-C | 2 | One will become the sensor node in the room, the other the receiver in the office. The "U" version has a connector for an external antenna instead of an antenna on the board. Later I can point the receiver's antenna at the cold room | [Alash Electronics](https://alash-electronics.kz/product/esp32-wroom-32u-s-type-c-razyomom) | - |
| Wi-Fi antenna 2.4 GHz with U.FL to SMA cable (set of 2) | 1 set | The 32U board needs it. One for each board | Ozon | - |
| DS18B20 waterproof temperature probe, 1 m cable | 2 | Digital, cheap, several can share one wire later. One spare, because cheap ones are sometimes faulty | [Ampermarket](https://ampermarket.kz/sensors/climate/ds18b20/) | - |
| Resistor 4.7 kOhm, 0.25 W | 5-10 | Pull-up resistor the DS18B20 data line needs | Any electronics shop | - |
| Breadboard, 830 points | 1 | Build without soldering | [Alash Electronics](https://alash-electronics.kz/product/bespaechnaya-maketnaya-plata-830-kontaktov) | - |
| Jumper wires, male-male, 15 cm | 1 pack | Short wires keep the breadboard tidy | [Alash Electronics](https://alash-electronics.kz/product/15-sm-40-kontaktnyy-peremychka-liniya-provod-p-p) | - |
| USB-A to Type-C cable, 1 m, with data | 1 | To upload programs. A charge-only cable won't work | Any shop | - |
| **Total** | | | | - |

No batteries yet. Which batteries survive -20 °C, and how to power the sensor node from them, is the next step after this one.

## Things I checked before choosing

- The seller's page for the DS18B20 probe gives a range of -55 to +125 °C, but the +/-0.5 °C accuracy only from -10 to +85 °C. Our room is at -18 to -20 °C, so I can't trust the factory accuracy there. That's why every sensor gets compared with a reference thermometer in a freezer.
- The cable material of the probe isn't stated. If it's PVC, it gets stiff in the cold. That doesn't matter on the desk, but it will matter in the room.
- The U.FL connector on the board is small and not made for many connections. The antenna has to be fixed to something so its weight doesn't hang on the connector.

## What arrived (November 23, 2026)

Everything except the DS18B20 probes and the resistors, which are still on the way.

What I found on the board itself:

| What | Found | How I know |
|---|---|---|
| Module | Marked "ESP-32U", U.FL antenna socket | Label on the module |
| Chip | ESP32-D0WD, revision v1.0, 40 MHz crystal | Reported by the upload tool |
| USB chip | Silicon Labs CP2102 | Marking on the chip. Windows didn't install the driver by itself, I installed the CP210x driver from the Silicon Labs website |
| 3.3 V regulator | marked "1117 3.3" | Marking on the part |
| Pins | 38, all labelled. 3V3, GPIO4 and GND are there, so the [wiring](bench-wiring.svg) fits this board | Labels on the board |
| Buttons | BOOT and EN (restart) | Labels on the board |

Two things to be careful about with this board:

- The pins marked D0, D1, D2, D3, CMD and CLK go to the board's flash memory. Nothing may be connected to them.
- GPIO4 sits between GPIO16 and GPIO0. GPIO0 decides how the board starts, so the probe's data wire must not end up one pin off.

One problem I noticed: the board is wide. On one 830-point breadboard it covers all the holes on one side, and 3V3 and GPIO4 are on opposite sides. My jumper wires are male-male and the board has male pins too. So I need female-male jumper wires to connect the board to the breadboard.

## Steps

1. ✅ Install the Arduino IDE and the ESP32 board support.
2. ✅ Check the board without sensors: [board-check](../firmware/board-check/README.md). It starts, takes programs, and finds our Wi-Fi at -49 dBm.
3. Connect one DS18B20 to the ESP32 on the breadboard (3 wires and the 4.7 kOhm resistor): [wiring](bench-wiring.svg).
4. Upload [bench-test](../firmware/bench-test/README.md), which prints the temperature every 5 seconds. Without a probe it already runs and correctly reports `found 0 sensor(s) on GPIO4`.
5. Test both probes: room temperature, ice water, home freezer. Write down the difference from the reference thermometer.

Results will go into this folder when I have them.
