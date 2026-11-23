"""Message format (schema version 1) sent by an ESP32 node, and its validation.

Example message::

    {
      "schema_version": 1,
      "device_id": "esp32-a8032a5d0424",
      "fw_version": "heartbeat-0.1",
      "seq": 17,
      "uptime_s": 170,
      "rssi_dbm": -47,
      "readings": [
        {"sensor_id": "28FF641E8216034B", "quantity": "temperature",
         "value": -18.5, "unit": "degC", "status": "ok"}
      ]
    }

The node has no real-time clock yet, so the server stamps every message with
its own UTC receive time. ``readings`` may be empty (heartbeat only).
"""

from __future__ import annotations

import math
import re
from dataclasses import dataclass, field
from typing import Any

SCHEMA_VERSION = 1

DEVICE_ID_RE = re.compile(r"^[a-z0-9-]{3,40}$")
SENSOR_ID_RE = re.compile(r"^[0-9A-F]{16}$")  # DS18B20 64-bit ROM code, hex
FW_VERSION_RE = re.compile(r"^[A-Za-z0-9._-]{1,32}$")

RSSI_MIN_DBM = -120
RSSI_MAX_DBM = 0
MAX_READINGS = 16
UINT32_MAX = 2**32 - 1

# DS18B20 measuring range from the datasheet.
TEMP_MIN_C = -55.0
TEMP_MAX_C = 125.0

READING_STATUSES = frozenset({"ok", "disconnected", "suspect_85", "out_of_range"})


class ValidationError(ValueError):
    """The message does not match schema version 1."""


@dataclass(frozen=True)
class Reading:
    sensor_id: str
    quantity: str
    value: float | None
    unit: str
    status: str


@dataclass(frozen=True)
class Message:
    device_id: str
    fw_version: str
    seq: int
    uptime_s: int
    rssi_dbm: int
    readings: tuple[Reading, ...] = field(default_factory=tuple)


def _require_int(data: dict[str, Any], key: str, lo: int, hi: int) -> int:
    value = data.get(key)
    # bool is a subclass of int in Python, so reject it explicitly.
    if not isinstance(value, int) or isinstance(value, bool):
        raise ValidationError(f"'{key}' must be an integer")
    if not lo <= value <= hi:
        raise ValidationError(f"'{key}' must be between {lo} and {hi}, got {value}")
    return value


def _require_str(data: dict[str, Any], key: str, pattern: re.Pattern[str]) -> str:
    value = data.get(key)
    if not isinstance(value, str) or not pattern.match(value):
        raise ValidationError(f"'{key}' is missing or has an invalid format")
    return value


def _parse_reading(raw: Any, index: int) -> Reading:
    where = f"readings[{index}]"
    if not isinstance(raw, dict):
        raise ValidationError(f"{where} must be an object")
    sensor_id = _require_str(raw, "sensor_id", SENSOR_ID_RE)
    if raw.get("quantity") != "temperature":
        raise ValidationError(f"{where}.quantity must be 'temperature'")
    if raw.get("unit") != "degC":
        raise ValidationError(f"{where}.unit must be 'degC'")
    status = raw.get("status")
    if status not in READING_STATUSES:
        raise ValidationError(f"{where}.status must be one of {sorted(READING_STATUSES)}")

    value = raw.get("value")
    if value is not None:
        if not isinstance(value, (int, float)) or isinstance(value, bool):
            raise ValidationError(f"{where}.value must be a number or null")
        value = float(value)
        if not math.isfinite(value):
            raise ValidationError(f"{where}.value must be finite")
    if status == "ok":
        if value is None:
            raise ValidationError(f"{where}: status 'ok' needs a value")
        if not TEMP_MIN_C <= value <= TEMP_MAX_C:
            raise ValidationError(
                f"{where}: {value} degC is outside {TEMP_MIN_C}..{TEMP_MAX_C} degC"
            )
    return Reading(sensor_id, "temperature", value, "degC", status)


def parse_message(data: Any) -> Message:
    """Validate a decoded JSON object and return a Message, or raise ValidationError."""
    if not isinstance(data, dict):
        raise ValidationError("message must be a JSON object")
    if data.get("schema_version") != SCHEMA_VERSION or isinstance(
        data.get("schema_version"), bool
    ):
        raise ValidationError(f"'schema_version' must be {SCHEMA_VERSION}")

    readings_raw = data.get("readings", [])
    if not isinstance(readings_raw, list):
        raise ValidationError("'readings' must be a list")
    if len(readings_raw) > MAX_READINGS:
        raise ValidationError(f"too many readings (max {MAX_READINGS})")

    return Message(
        device_id=_require_str(data, "device_id", DEVICE_ID_RE),
        fw_version=_require_str(data, "fw_version", FW_VERSION_RE),
        seq=_require_int(data, "seq", 0, UINT32_MAX),
        uptime_s=_require_int(data, "uptime_s", 0, UINT32_MAX),
        rssi_dbm=_require_int(data, "rssi_dbm", RSSI_MIN_DBM, RSSI_MAX_DBM),
        readings=tuple(_parse_reading(r, i) for i, r in enumerate(readings_raw)),
    )
