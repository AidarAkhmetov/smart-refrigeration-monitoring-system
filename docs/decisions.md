# Decisions

I only add new entries at the bottom and don't change old ones, so later I can see why something was decided.

---

## 2026-11-17: The system only watches

Decision: The system only monitors. It never controls, reprograms or bypasses the Elitech controller or any safety circuit.

Why: The cold room holds all the goods of the shop. If my device has a bug or loses power, the unit has to keep working like before. It also keeps me away from mains wiring that I'm not qualified to work on.

Not chosen: Reading data directly from the controller. Maybe later, but only if the manual confirms it has a supported interface.

---

## 2026-11-17: One room, several parts

Decision: The app and the data treat the installation as one cold room made of several parts (compressor, fans, evaporator, etc.), each with its own sensors.

Why: There's one outdoor unit, one controller and one evaporator for one room. And to find which part is broken, each part needs its own data and its own anomaly history.

---

## 2026-11-17: Only equipment from this cold room

Decision: The survey only lists equipment that belongs to this cold room. Parts bought around the same time for a different refrigerator are left out.

Why: Mixing them in would give a wrong picture of the system, for example the wrong evaporator or refrigerant.

---

## 2026-11-17: Low price is a must

Decision: Price is one of the main limits for the design. Every part I choose gets compared with the price of ready-made systems (500-800 euros).

Why: Because of that price our shop has no monitoring at all, and the shops around us are in the same situation. If my system is only a little cheaper, it doesn't help anyone.

But: Cheap can't mean unreliable. A monitor that silently stops working is worse than none, because people stop checking. So if the device goes quiet, that's an alarm too.

---

## 2026-11-17: Watch each part, not only temperature

Decision: The system tracks the health of each main part separately and counts how often anomalies happen on each one.

Why: Temperature only tells you something is wrong when it's already happening and the goods are already warming up. If the compressor or a fan starts acting strange more and more often, that's a sign it will break soon, and you can call a technician earlier. And when something does break, knowing which part it is saves time.
