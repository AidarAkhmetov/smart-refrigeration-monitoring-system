"""HTTP receiver for the local network.

Endpoints:
    POST /api/v1/messages  - an ESP32 node sends one message (JSON, schema v1)
    GET  /api/v1/devices   - status of every node seen so far, with the latest
                             reading of each of its probes (JSON)
    GET  /                 - simple status page with temperatures, refreshes every 10 s

Configuration (environment variables):
    RECEIVER_HOST   address to listen on (default 0.0.0.0, the whole local network)
    RECEIVER_PORT   TCP port (default 8000)
    RECEIVER_DB     SQLite file (default receiver.sqlite3)
    STALE_AFTER_S   a node counts as offline after this many seconds of silence (default 30)

Run from the server/ folder:  python -m receiver.app
"""

from __future__ import annotations

import html
import json
import logging
import os
from collections.abc import Sequence
from datetime import datetime, timezone
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from typing import Any

from .schema import ValidationError, parse_message
from .storage import DeviceStatus, LatestReading, Storage

log = logging.getLogger("receiver")

MAX_BODY_BYTES = 4096


def reading_to_dict(r: LatestReading, now: datetime) -> dict[str, Any]:
    return {
        "sensor_id": r.sensor_id,
        "value": r.value,
        "unit": r.unit,
        "status": r.status,
        "received_utc": r.received_at.isoformat(),
        "seconds_ago": round((now - r.received_at).total_seconds()),
    }


def device_to_dict(
    d: DeviceStatus,
    now: datetime,
    stale_after_s: int,
    readings: Sequence[LatestReading] = (),
) -> dict[str, Any]:
    age_s = (now - d.last_seen).total_seconds()
    return {
        "device_id": d.device_id,
        "online": age_s <= stale_after_s,
        "last_seen_utc": d.last_seen.isoformat(),
        "seconds_since_last_message": round(age_s),
        "fw_version": d.fw_version,
        "seq": d.seq,
        "uptime_s": d.uptime_s,
        "rssi_dbm": d.rssi_dbm,
        "message_count": d.message_count,
        "restarts": d.restarts,
        "readings": [reading_to_dict(r, now) for r in readings],
    }


STATUS_TEXT = {
    "disconnected": "probe not answering",
    "suspect_85": "suspect +85 °C (power-up value)",
    "out_of_range": "out of range",
}


def render_temperatures(d: dict[str, Any]) -> str:
    """One line per probe: latest value, or the fault, with its age."""
    if not d["readings"]:
        return "<span class='dim'>no probe</span>"
    lines = []
    for r in d["readings"]:
        sensor = html.escape(r["sensor_id"][-4:])
        age = f"<span class='dim'>{r['seconds_ago']} s ago, probe …{sensor}</span>"
        if r["status"] == "ok" and r["value"] is not None:
            css = "temp" if d["online"] else "temp dim"
            lines.append(f"<span class='{css}'>{r['value']:.1f} °C</span> {age}")
        else:
            text = STATUS_TEXT.get(r["status"], r["status"])
            lines.append(f"<span class='bad'>{html.escape(text)}</span> {age}")
    return "<br>".join(lines)


def render_page(devices: list[dict[str, Any]], now: datetime, stale_after_s: int) -> str:
    local_now = now.astimezone()
    tz_label = local_now.strftime("UTC%z")
    rows = []
    for d in devices:
        last_local = datetime.fromisoformat(d["last_seen_utc"]).astimezone()
        state, css = ("online", "ok") if d["online"] else ("OFFLINE", "bad")
        rows.append(
            f"<tr><td>{html.escape(d['device_id'])}</td>"
            f"<td class='{css}'>{state}</td>"
            f"<td>{render_temperatures(d)}</td>"
            f"<td>{last_local:%Y-%m-%d %H:%M:%S} ({d['seconds_since_last_message']} s ago)</td>"
            f"<td>{d['rssi_dbm']} dBm</td><td>{d['uptime_s']} s</td>"
            f"<td>{d['message_count']}</td><td>{d['restarts']}</td>"
            f"<td>{html.escape(d['fw_version'])}</td></tr>"
        )
    body = "".join(rows) or "<tr><td colspan='9'>No messages yet</td></tr>"
    return f"""<!doctype html>
<html><head><meta charset="utf-8"><meta http-equiv="refresh" content="10">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Cold room monitor</title>
<style>
body{{font-family:system-ui,sans-serif;margin:16px;background:#fff;color:#222}}
table{{border-collapse:collapse}}td,th{{border:1px solid #ccc;padding:6px 10px;text-align:left}}
.ok{{color:#0a7d2c;font-weight:600}}.bad{{color:#c00;font-weight:600}}
.temp{{font-size:1.4em;font-weight:700}}.dim{{color:#888;font-size:0.9em}}
.temp.dim{{font-size:1.4em}}
</style></head><body>
<h1>Cold room monitor: nodes</h1>
<p>Page time: {local_now:%Y-%m-%d %H:%M:%S} ({tz_label}). Refreshes every 10 s.
A node is OFFLINE after {stale_after_s} s without a message.</p>
<table><tr><th>Node</th><th>State</th><th>Temperature</th><th>Last message</th><th>Wi-Fi</th>
<th>Uptime</th><th>Messages</th><th>Restarts</th><th>Firmware</th></tr>
{body}</table></body></html>"""


def make_handler(storage: Storage, stale_after_s: int) -> type[BaseHTTPRequestHandler]:
    class Handler(BaseHTTPRequestHandler):
        server_version = "ColdRoomReceiver/0.1"

        def _send(self, status: int, body: bytes, content_type: str) -> None:
            self.send_response(status)
            self.send_header("Content-Type", content_type)
            self.send_header("Content-Length", str(len(body)))
            self.end_headers()
            self.wfile.write(body)

        def _send_json(self, status: int, obj: Any) -> None:
            self._send(status, json.dumps(obj).encode("utf-8"), "application/json")

        def do_POST(self) -> None:  # noqa: N802 (name required by BaseHTTPRequestHandler)
            if self.path != "/api/v1/messages":
                self._send_json(404, {"error": "not found"})
                return
            try:
                length = int(self.headers.get("Content-Length", "0"))
            except ValueError:
                self._send_json(400, {"error": "bad Content-Length"})
                return
            if length <= 0:
                self._send_json(400, {"error": "empty body"})
                return
            if length > MAX_BODY_BYTES:
                self._send_json(413, {"error": f"body larger than {MAX_BODY_BYTES} bytes"})
                return
            raw = self.rfile.read(length)
            try:
                msg = parse_message(json.loads(raw))
            except (json.JSONDecodeError, UnicodeDecodeError):
                log.warning("rejected from %s: invalid JSON", self.client_address[0])
                self._send_json(400, {"error": "invalid JSON"})
                return
            except ValidationError as exc:
                log.warning("rejected from %s: %s", self.client_address[0], exc)
                self._send_json(422, {"error": str(exc)})
                return
            message_id = storage.save(msg, source_ip=self.client_address[0])
            log.info(
                "stored #%d from %s seq=%d rssi=%d dBm readings=%d",
                message_id, msg.device_id, msg.seq, msg.rssi_dbm, len(msg.readings),
            )
            self._send_json(201, {"stored": message_id})

        def do_GET(self) -> None:  # noqa: N802
            now = datetime.now(timezone.utc)
            by_device: dict[str, list[LatestReading]] = {}
            for r in storage.latest_readings():
                by_device.setdefault(r.device_id, []).append(r)
            devices = [
                device_to_dict(d, now, stale_after_s, by_device.get(d.device_id, []))
                for d in storage.devices()
            ]
            if self.path == "/api/v1/devices":
                self._send_json(
                    200, {"now_utc": now.isoformat(timespec="seconds"), "devices": devices}
                )
            elif self.path == "/":
                self._send(
                    200, render_page(devices, now, stale_after_s).encode("utf-8"), "text/html; charset=utf-8"
                )
            else:
                self._send_json(404, {"error": "not found"})

        def log_message(self, format: str, *args: Any) -> None:  # noqa: A002
            log.debug("%s - %s", self.client_address[0], format % args)

    return Handler


def build_server(
    host: str, port: int, storage: Storage, stale_after_s: int
) -> ThreadingHTTPServer:
    return ThreadingHTTPServer((host, port), make_handler(storage, stale_after_s))


def main() -> None:
    logging.basicConfig(
        level=logging.INFO, format="%(asctime)s %(levelname)s %(name)s: %(message)s"
    )
    host = os.environ.get("RECEIVER_HOST", "0.0.0.0")
    port = int(os.environ.get("RECEIVER_PORT", "8000"))
    db_path = os.environ.get("RECEIVER_DB", "receiver.sqlite3")
    stale_after_s = int(os.environ.get("STALE_AFTER_S", "30"))

    storage = Storage(db_path)
    server = build_server(host, port, storage, stale_after_s)
    log.info("listening on %s:%d, database %s", host, port, db_path)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        log.info("stopping")
    finally:
        server.server_close()
        storage.close()


if __name__ == "__main__":
    main()
