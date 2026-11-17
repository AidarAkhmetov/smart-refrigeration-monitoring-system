# Testing plan

How I plan to check each part of the system before trusting it with the real cold room. No tests have been done yet. Results will go into a separate document when I have them.

## 1. Sensor accuracy (bench)

- Put all temperature sensors together with a reference thermometer in ice water (0 °C) and record the difference.
- Repeat in a home freezer (around -18 °C), because this is the range that matters.
- Record the offset of each sensor and decide if a correction is needed.

Pass: every sensor within the accuracy from requirement R-04 after correction.

## 2. Sensor faults (bench)

- Unplug a sensor while the device is running.
- Short the data line.
- Send obviously wrong values from a test script.

Pass: the device marks the reading as `bad`, keeps reading the other sensors and doesn't restart in a loop.

## 3. Network and power (bench)

- Turn off Wi-Fi for 10 minutes, then 1 hour. Check that all readings arrive afterwards with correct times.
- Unplug the device. Check that the "device offline" alert reaches the phone and measure how long it takes.
- Restart the device many times in a row. Check that it comes back on its own.

Pass: no data lost during Wi-Fi outages within the buffer size; offline alert within the time agreed in the requirements.

## 4. Server (automated tests)

- Valid reading is stored.
- Reading without time, unit or sensor ID is rejected.
- Duplicate reading (same `sequence`) is stored only once.
- Old or impossible values are flagged.
- No data from a device for longer than the limit creates an alert.

These will run automatically every time I change the server code.

## 5. Alerts end to end

- Simulate a temperature rise above the limit and measure the time until the phone gets the notification.
- Check that the alert doesn't repeat every minute, but does repeat if nobody reacts.

## 6. Pilot in the real cold room

- Run the system next to the existing controller for several weeks.
- Compare my readings with the controller display a few times a day.
- Count false alarms during defrost and adjust the defrost detection.
- Keep notes about everything that went wrong.

Pass: no missed real problem and almost no false alarms over the pilot period.
