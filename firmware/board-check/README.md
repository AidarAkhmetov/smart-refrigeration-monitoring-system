# Board check sketch

The first thing I uploaded to the new ESP32 board, before the temperature probes arrived. It needs nothing connected except the USB cable and the antenna.

1. Prints the chip model, number of cores, flash size and the MAC address once.
2. Every 10 seconds scans for Wi-Fi networks and prints each one with its signal strength (RSSI, in dBm).

Because of step 2 it also works as a small Wi-Fi meter. I'm going to use it with the laptop at the shop to measure the signal in the office and next to the cold room door (stage 2).

## Settings

- Board: Tools > Board > esp32 > **ESP32 Dev Module**
- Upload speed: **115200**. At the default 921600 one upload failed during the check after writing ("Serial data stream stopped"). At 115200 it works every time
- Serial monitor speed: **115200**

The 32U board has no antenna on the board itself, only a U.FL socket. Connect the antenna before running this sketch.

## How to read the RSSI

| RSSI | My label | Meaning |
|---|---|---|
| -60 dBm or better | good | Strong, stable |
| -60 to -70 dBm | ok | Fine for small messages |
| -70 to -80 dBm | weak | Works, but expect drops |
| below -80 dBm | very weak | Not reliable |

These limits are a common rule of thumb, not from a datasheet. The real test is whether messages actually arrive.

## First result (November 23, 2026, at home)

The board found 11 networks. Our home network was at -49 dBm with the external antenna on the desk. Chip reported by the upload tool: ESP32-D0WD, revision v1.0, 40 MHz crystal.
