# Bench prototype (stage 4)

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

## Steps

1. Install the Arduino IDE and the ESP32 board support.
2. Connect one DS18B20 to the ESP32 on the breadboard (3 wires and the 4.7 kOhm resistor).
3. Upload a first program that prints the temperature every few seconds.
4. Test both probes: room temperature, ice water, home freezer. Write down the difference from the reference thermometer.

Results will go into this folder when I have them.
