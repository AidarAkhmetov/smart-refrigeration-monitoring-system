"""SQLite storage for received messages. All timestamps are stored in UTC (ISO 8601)."""

from __future__ import annotations

import sqlite3
import threading
from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path

from .schema import Message

_SCHEMA_SQL = """
CREATE TABLE IF NOT EXISTS messages (
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    received_at TEXT    NOT NULL,   -- server receive time, UTC ISO 8601
    device_id   TEXT    NOT NULL,
    fw_version  TEXT    NOT NULL,
    seq         INTEGER NOT NULL,
    uptime_s    INTEGER NOT NULL,
    rssi_dbm    INTEGER NOT NULL,
    source_ip   TEXT    NOT NULL
);
CREATE INDEX IF NOT EXISTS idx_messages_device ON messages(device_id, id);

CREATE TABLE IF NOT EXISTS readings (
    message_id INTEGER NOT NULL REFERENCES messages(id),
    sensor_id  TEXT    NOT NULL,
    quantity   TEXT    NOT NULL,
    value      REAL,               -- NULL when the sensor gave no value
    unit       TEXT    NOT NULL,
    status     TEXT    NOT NULL
);
CREATE INDEX IF NOT EXISTS idx_readings_message ON readings(message_id);
"""


@dataclass(frozen=True)
class DeviceStatus:
    device_id: str
    last_seen: datetime  # UTC
    fw_version: str
    seq: int
    uptime_s: int
    rssi_dbm: int
    message_count: int
    restarts: int  # times seq went backwards, i.e. the node rebooted


@dataclass(frozen=True)
class LatestReading:
    device_id: str
    sensor_id: str
    value: float | None  # degC; None when the sensor gave no value
    unit: str
    status: str
    received_at: datetime  # UTC, server receive time of the message


class Storage:
    """Thread-safe wrapper around one SQLite connection."""

    def __init__(self, path: str | Path) -> None:
        self._lock = threading.Lock()
        self._conn = sqlite3.connect(str(path), check_same_thread=False)
        self._conn.row_factory = sqlite3.Row
        with self._lock, self._conn:
            self._conn.executescript(_SCHEMA_SQL)

    def close(self) -> None:
        with self._lock:
            self._conn.close()

    def save(self, msg: Message, source_ip: str, received_at: datetime | None = None) -> int:
        when = received_at or datetime.now(timezone.utc)
        if when.tzinfo is None:
            raise ValueError("received_at must be timezone-aware")
        with self._lock, self._conn:
            cur = self._conn.execute(
                "INSERT INTO messages (received_at, device_id, fw_version, seq, uptime_s,"
                " rssi_dbm, source_ip) VALUES (?, ?, ?, ?, ?, ?, ?)",
                (
                    when.astimezone(timezone.utc).isoformat(timespec="seconds"),
                    msg.device_id,
                    msg.fw_version,
                    msg.seq,
                    msg.uptime_s,
                    msg.rssi_dbm,
                    source_ip,
                ),
            )
            message_id = int(cur.lastrowid or 0)
            self._conn.executemany(
                "INSERT INTO readings (message_id, sensor_id, quantity, value, unit, status)"
                " VALUES (?, ?, ?, ?, ?, ?)",
                [
                    (message_id, r.sensor_id, r.quantity, r.value, r.unit, r.status)
                    for r in msg.readings
                ],
            )
        return message_id

    def latest_readings(self) -> list[LatestReading]:
        """Most recent reading of every (node, sensor) pair, ordered by node and sensor."""
        with self._lock:
            rows = self._conn.execute(
                "SELECT m.device_id, r.sensor_id, r.value, r.unit, r.status, m.received_at"
                " FROM readings r JOIN messages m ON m.id = r.message_id"
                " WHERE r.rowid IN ("
                "   SELECT MAX(r2.rowid) FROM readings r2"
                "   JOIN messages m2 ON m2.id = r2.message_id"
                "   GROUP BY m2.device_id, r2.sensor_id)"
                " ORDER BY m.device_id, r.sensor_id"
            ).fetchall()
        return [
            LatestReading(
                device_id=row["device_id"],
                sensor_id=row["sensor_id"],
                value=row["value"],
                unit=row["unit"],
                status=row["status"],
                received_at=datetime.fromisoformat(row["received_at"]),
            )
            for row in rows
        ]

    def devices(self) -> list[DeviceStatus]:
        """Latest state of every node, with message and restart counts."""
        with self._lock:
            rows = self._conn.execute(
                "SELECT device_id, received_at, fw_version, seq, uptime_s, rssi_dbm"
                " FROM messages ORDER BY device_id, id"
            ).fetchall()
        result: dict[str, DeviceStatus] = {}
        for row in rows:
            prev = result.get(row["device_id"])
            restarts = prev.restarts if prev else 0
            if prev is not None and row["seq"] < prev.seq:
                restarts += 1
            result[row["device_id"]] = DeviceStatus(
                device_id=row["device_id"],
                last_seen=datetime.fromisoformat(row["received_at"]),
                fw_version=row["fw_version"],
                seq=row["seq"],
                uptime_s=row["uptime_s"],
                rssi_dbm=row["rssi_dbm"],
                message_count=(prev.message_count if prev else 0) + 1,
                restarts=restarts,
            )
        return list(result.values())
