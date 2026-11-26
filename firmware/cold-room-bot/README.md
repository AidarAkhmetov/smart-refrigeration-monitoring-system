# Cold room bot

The ESP32 reads the DS18B20 probe and talks to Telegram by itself over HTTPS. No computer has to run in the shop. This is "option A" from the [decisions](../../docs/decisions.md): quick to set up, but it has one known gap (see the end).

## What it does

| | |
|---|---|
| `/status` | Temperature now, the limits, Wi-Fi signal, time since the last restart |
| `/ok` or the 🔕 button | Silences the current alarm |
| `/on` | Turns the reminders for the current alarm back on |
| `/id` | Board name and firmware version |
| On its own | "Board switched on" after every power-up, so I can see when the power was gone |
| On its own | 🔴 too warm / 🔵 too cold / ⚠️ probe not answering, once the problem has lasted 30 s |
| On its own | ⏰ reminder every 15 s until somebody silences the alarm |
| On its own | ✅ back to normal |

The bot only answers one Telegram chat (mine). Anyone else who finds it gets no answer. Before the chat is set, it only tells a new chat its own chat id, nothing about the room.

A new alarm (or "back to normal" followed by a new problem) always starts the reminders again, even if the last one was silenced. Every state change has to last 30 s, so one bad reading doesn't send anything. "Back to normal" also needs 1 °C of margin inside the limit, so a temperature sitting right at the limit doesn't flip back and forth.

## Limits

The limits in the code are for the desk test: **10 to 28 °C**. I test them with my hand and warm water. The cold room limits are not decided. First I need the setpoint and the defrost settings from the ETC-974, otherwise every defrost would raise an alarm.

## Wiring (temporary)

Right now the probe hangs on three neighbouring pins: red at GPIO25, yellow (data) at GPIO26, black at GPIO27. The board powers the probe from two pins (the DS18B20 needs at most 1.5 mA) and the data line only has the ESP32's own weak pull-up, about 45 kOhm.

This works on the desk, but not well. With the probe in hot water about half of the reads failed the check and came back as `-127` ("probe not answering"). The code now reads each result up to 3 times, which hides single errors, but the real fix is the wiring from the datasheet: 3V3, GND and a 4.7 kOhm pull-up. Then `PROBE_GPIO_POWER` goes to `false`.

## Setup

1. Copy `secrets.example.h` to `secrets.h` and fill in Wi-Fi and the bot token from @BotFather. Leave `ALLOWED_CHAT_ID` at 0.
2. Upload, write anything to the bot. It answers with your chat id. Put it into `secrets.h` and upload again.
3. Board: **ESP32 Dev Module**, upload speed **115200**. Libraries: OneWire, DallasTemperature, ArduinoJson 7.

`secrets.h` is in `.gitignore`. The token never goes into the code, the log or this repository. The serial log never prints the request address, because it contains the token.

## HTTPS

The board checks Telegram's certificate. [`telegram_ca.h`](telegram_ca.h) has the two GoDaddy root certificates the Telegram chain ended in on 2026-11-26 (valid until 2037). If Telegram changes its certificate provider, the board will log a connection error and this file has to be updated.

## Optional: status page

If `RECEIVER_URL` is set, the board also sends the same schema v1 message as the [heartbeat](../heartbeat/README.md) to the [server](../../server/README.md) on my laptop every 10 s, so the page shows the temperature too.

## What it can't do

If the power or the internet goes out in the shop, the board can't tell anyone. A message can only say "I'm back" after the power returns. To get a "the shop went silent" alarm, something outside the shop has to notice that the board stopped talking. That is the server on a rented VPS, planned for later.
