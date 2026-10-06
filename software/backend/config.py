"""
M.A.V.I.S. Backend Configuration.

Environment variables are used so that network addresses and authentication
values are not hard-coded into source code.
"""

from __future__ import annotations

import os
from pathlib import Path


BASE_DIR = Path(__file__).resolve().parents[2]

DATA_DIR = BASE_DIR / "data"
INVENTORY_FILE = DATA_DIR / "inventory.json"

ESP_IP = os.getenv("MAVIS_ESP_IP", "192.168.1.100")
ESP_API_TOKEN = os.getenv("MAVIS_ESP_API_TOKEN", "")

CAMERA_URL = os.getenv(
    "MAVIS_CAMERA_URL",
    "http://192.168.1.118:8080/shot.jpg",
)

HTTP_TIMEOUT = float(os.getenv("MAVIS_HTTP_TIMEOUT", "3.0"))
CAMERA_TIMEOUT = float(os.getenv("MAVIS_CAMERA_TIMEOUT", "2.0"))
