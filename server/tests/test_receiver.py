"""Tests for message validation, storage and the HTTP receiver."""

from __future__ import annotations

import json
import threading
import urllib.error
import urllib.request
from collections.abc import Iterator
from datetime import datetime, timedelta, timezone
from pathlib import Path
from typing import Any

import pytest

from receiver.app import build_server, device_to_dict
from receiver.schema import ValidationError, parse_message
from receiver.storage import Storage


def make_msg(**overrides: Any) -> dict[str, Any]:
    msg: dict[str, Any] = {
        "schema_version": 1,
        "device_id": "esp32-a8032a5d0424",
        "fw_version": "heartbeat-0.1",
        "seq": 1,
        "uptime_s": 10,
        "rssi_dbm": -47,
        "readings": [],
    }
    msg.update(overrides)
    return msg


def reading(**overrides: Any) -> dict[str, Any]:
    r: dict[str, Any] = {
        "sensor_id": "28FF641E8216034B",
        "quantity": "temperature",
        "value": -18.5,
        "unit": "degC",
        "status": "ok",
    }
    r.update(overrides)
    return r


# ---------- validation ----------


def test_valid_heartbeat_parses() -> None:
    msg = parse_message(make_msg())
    assert msg.device_id == "esp32-a8032a5d0424"
    assert msg.rssi_dbm == -47
    assert msg.readings == ()


def test_readings_are_optional() -> None:
    data = make_msg()
    del data["readings"]
    assert parse_message(data).readings == ()


def test_valid_reading_parses() -> None:
    msg = parse_message(make_msg(readings=[reading()]))
    assert msg.readings[0].value == -18.5
    assert msg.readings[0].unit == "degC"


def test_disconnected_reading_may_have_null_value() -> None:
    msg = parse_message(make_msg(readings=[reading(value=None, status="disconnected")]))
    assert msg.readings[0].value is None


@pytest.mark.parametrize(
    "overrides",
    [
        {"schema_version": 2},
        {"schema_version": True},
        {"device_id": "ESP32 with spaces"},
        {"device_id": ""},
        {"seq": -1},
        {"seq": "5"},
        {"seq": True},
        {"uptime_s": 1.5},
        {"rssi_dbm": 10},
        {"rssi_dbm": -200},
        {"fw_version": "x" * 33},
        {"readings": "none"},
        {"readings": [reading()] * 17},
    ],
)
def test_invalid_message_rejected(overrides: dict[str, Any]) -> None:
    with pytest.raises(ValidationError):
        parse_message(make_msg(**overrides))


@pytest.mark.parametrize(
    "bad",
    [
        {"sensor_id": "28ff641e8216034b"},  # lower case
        {"sensor_id": "1234"},
        {"quantity": "humidity"},
        {"unit": "degF"},
        {"status": "great"},
        {"value": None},  # status ok needs a value
        {"value": 126.0},  # above DS18B20 range
        {"value": -56.0},  # below DS18B20 range
        {"value": "cold"},
        {"value": float("nan")},
    ],
)
def test_invalid_reading_rejected(bad: dict[str, Any]) -> None:
    with pytest.raises(ValidationError):
        parse_message(make_msg(readings=[reading(**bad)]))


def test_non_object_rejected() -> None:
    with pytest.raises(ValidationError):
        parse_message([1, 2, 3])


# ---------- storage ----------


@pytest.fixture
def storage(tmp_path: Path) -> Iterator[Storage]:
    s = Storage(tmp_path / "test.sqlite3")
    yield s
    s.close()


def test_storage_keeps_latest_state_and_counts(storage: Storage) -> None:
    t0 = datetime(2026, 11, 23, 12, 0, tzinfo=timezone.utc)
    storage.save(parse_message(make_msg(seq=1, rssi_dbm=-50)), "192.168.0.14", t0)
    storage.save(
        parse_message(make_msg(seq=2, rssi_dbm=-45, readings=[reading()])),
        "192.168.0.14",
        t0 + timedelta(seconds=10),
    )
    [dev] = storage.devices()
    assert dev.seq == 2
    assert dev.rssi_dbm == -45
    assert dev.message_count == 2
    assert dev.restarts == 0
    assert dev.last_seen == t0 + timedelta(seconds=10)


def test_storage_detects_restart(storage: Storage) -> None:
    for seq in (5, 6, 0, 1):
        storage.save(parse_message(make_msg(seq=seq)), "192.168.0.14")
    [dev] = storage.devices()
    assert dev.restarts == 1


def test_storage_rejects_naive_time(storage: Storage) -> None:
    with pytest.raises(ValueError):
        storage.save(parse_message(make_msg()), "1.2.3.4", datetime(2026, 11, 23, 12, 0))


def test_offline_after_stale_limit(storage: Storage) -> None:
    t0 = datetime(2026, 11, 23, 12, 0, tzinfo=timezone.utc)
    storage.save(parse_message(make_msg()), "192.168.0.14", t0)
    [dev] = storage.devices()
    assert device_to_dict(dev, t0 + timedelta(seconds=60), 60)["online"] is True
    assert device_to_dict(dev, t0 + timedelta(seconds=61), 60)["online"] is False


# ---------- HTTP ----------


@pytest.fixture
def base_url(storage: Storage) -> Iterator[str]:
    server = build_server("127.0.0.1", 0, storage, stale_after_s=60)
    thread = threading.Thread(target=server.serve_forever, daemon=True)
    thread.start()
    yield f"http://127.0.0.1:{server.server_address[1]}"
    server.shutdown()
    server.server_close()


def post(url: str, body: bytes) -> tuple[int, dict[str, Any]]:
    req = urllib.request.Request(
        url, data=body, headers={"Content-Type": "application/json"}, method="POST"
    )
    try:
        with urllib.request.urlopen(req, timeout=5) as resp:
            return resp.status, json.loads(resp.read())
    except urllib.error.HTTPError as err:
        return err.code, json.loads(err.read())


def test_http_accepts_valid_message(base_url: str) -> None:
    status, body = post(f"{base_url}/api/v1/messages", json.dumps(make_msg()).encode())
    assert status == 201
    assert body["stored"] == 1
    with urllib.request.urlopen(f"{base_url}/api/v1/devices", timeout=5) as resp:
        devices = json.loads(resp.read())["devices"]
    assert devices[0]["device_id"] == "esp32-a8032a5d0424"
    assert devices[0]["online"] is True


def test_http_rejects_bad_json(base_url: str) -> None:
    status, _ = post(f"{base_url}/api/v1/messages", b"{not json")
    assert status == 400


def test_http_rejects_invalid_message(base_url: str) -> None:
    status, body = post(
        f"{base_url}/api/v1/messages", json.dumps(make_msg(rssi_dbm=5)).encode()
    )
    assert status == 422
    assert "rssi_dbm" in body["error"]


def test_http_rejects_too_large_body(base_url: str) -> None:
    status, _ = post(f"{base_url}/api/v1/messages", b"x" * 5000)
    assert status == 413


def test_http_unknown_path(base_url: str) -> None:
    status, _ = post(f"{base_url}/api/v1/other", b"{}")
    assert status == 404


def test_status_page_renders(base_url: str) -> None:
    post(f"{base_url}/api/v1/messages", json.dumps(make_msg()).encode())
    with urllib.request.urlopen(f"{base_url}/", timeout=5) as resp:
        page = resp.read().decode()
    assert "esp32-a8032a5d0424" in page
    assert "online" in page
