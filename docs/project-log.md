# Project log

Short notes on what I did and what I learned, newest at the top. The full details are in the other documents, and every change is in the Git history.

---

## November 23, 2026

- The parts arrived two days early, except the temperature probes and the resistors.
- The laptop saw the board but there was no COM port. The USB chip is a CP2102 and Windows had no driver for it. I installed it from the Silicon Labs website.
- Uploaded my first programs. One just prints the chip data and scans Wi-Fi: our home network is at -49 dBm. The temperature program also runs and correctly says it found 0 sensors.
- One upload failed at the default speed. At 115200 it works every time.
- Found out the board is too wide for one breadboard and I only have male-male wires. I need female-male wires.

---

## November 21-22, 2026

- Went to the cold room and photographed every nameplate: condensing unit, compressor, condenser, evaporator, receiver, controller. Now the [site survey](site-survey.md) is based on nameplates instead of receipts.
- Found a mistake of mine: the ceiling is 3.0 m, not 3.5 m. I had misread the plan.
- Marked the evaporator, the door and the office on the building plan. The evaporator hangs on the far wall, about 12 m from the door.
- We got permission to drill one hole between the office and the cold room for the sensor cable. I only get one try, so I decided to test with battery sensors first for 2-3 weeks and drill only at the very end.
- Chose the parts for the first build on my desk: two ESP32 boards with external antennas and DS18B20 temperature sensors. Checked every shop page before ordering. One sensor was out of stock, one board needed an extra antenna, and one resistor listing was in the car parts section.
- Stage 1 (site survey) is done.
- Added the nameplate photos as [evidence](evidence/README.md) and drew a [sketch of the room](images/room-sketch.svg) with the planned sensor points. I blacked out the serial numbers and didn't upload the building plan, because it has personal data on it.
- Started working with branches and pull requests, and tagged the end of stage 1.

## November 17, 2026

- Started the project and this repository.
- Surveyed the small cold room that broke down: controller, compressor, condenser, evaporator.
- Wrote the requirements, a first architecture, the roadmap and a testing plan.
- In the evening I found out the small room is going to be taken down, and a much larger one (106 m2) replaces it. Changed the target of the project to the new room.

## November 9, 2026

- My dad found the cold room broken. It had stopped the day before, on the one day I didn't go to check it. We lost about 2000 euros of goods. This is why I started the project.
