"""
M.A.V.I.S. Flask backend.

Provides:
- Inventory inspection
- Robot dispatch
- Node verification
- Inventory synchronization
"""

from __future__ import annotations

import logging

from flask import Flask, jsonify, request

from config import (
    CAMERA_TIMEOUT,
    CAMERA_URL,
    ESP_API_TOKEN,
    ESP_IP,
    HTTP_TIMEOUT,
    INVENTORY_FILE,
)
from inventory import InventoryManager
from software.dispatch.robot_dispatch import (
    RobotDispatchError,
    dispatch_robot_to_node,
)
from software.vision.qr_verification import (
    QRVerificationError,
    verify_inventory_node,
)


logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s | %(levelname)s | %(name)s | %(message)s",
)

LOGGER = logging.getLogger("mavis.backend")

app = Flask(__name__)

inventory = InventoryManager(INVENTORY_FILE)


@app.get("/api/health")
def health() -> tuple:
    """Return backend health status."""
    return jsonify(
        {
            "service": "mavis-backend",
            "status": "ok",
        }
    )


@app.get("/api/inventory")
def get_inventory() -> tuple:
    """Return the current inventory state."""
    return jsonify(inventory.get_all())


@app.post("/api/dispatch")
def dispatch() -> tuple:
    """
    Dispatch the robot to a requested inventory node.

    Expected JSON:
        {
            "node_id": "store1_medA"
        }
    """

    payload = request.get_json(silent=True) or {}

    node_id = str(payload.get("node_id", "")).strip()

    if not node_id:
        return jsonify(
            {
                "success": False,
                "error": "node_id is required",
            }
        ), 400

    try:
        dispatch_robot_to_node(
            esp_ip=ESP_IP,
            api_token=ESP_API_TOKEN,
            node_id=node_id,
            timeout=HTTP_TIMEOUT,
        )

    except RobotDispatchError as exc:
        return jsonify(
            {
                "success": False,
                "error": str(exc),
            }
        ), 502

    return jsonify(
        {
            "success": True,
            "node_id": node_id,
            "command": f"GOTO,{node_id}",
        }
    )


@app.post("/api/verify")
def verify() -> tuple:
    """
    Verify the physical node using QR vision and update inventory.

    Expected JSON:
        {
            "expected_node_id": "store1_medA"
        }
    """

    payload = request.get_json(silent=True) or {}

    expected_node_id = str(
        payload.get("expected_node_id", "")
    ).strip()

    if not expected_node_id:
        return jsonify(
            {
                "success": False,
                "error": "expected_node_id is required",
            }
        ), 400

    try:
        detected_node_id = verify_inventory_node(
            webcam_url=CAMERA_URL,
            timeout=CAMERA_TIMEOUT,
        )

    except QRVerificationError as exc:
        return jsonify(
            {
                "success": False,
                "verified": False,
                "error": str(exc),
            }
        ), 502

    if detected_node_id != expected_node_id:
        return jsonify(
            {
                "success": False,
                "verified": False,
                "expected_node_id": expected_node_id,
                "detected_node_id": detected_node_id,
            }
        ), 409

    updated = inventory.decrement(
        expected_node_id
    )

    if not updated:
        return jsonify(
            {
                "success": False,
                "verified": True,
                "error": "Inventory item unavailable or out of stock.",
            }
        ), 409

    return jsonify(
        {
            "success": True,
            "verified": True,
            "node_id": expected_node_id,
            "inventory_updated": True,
        }
    )


if __name__ == "__main__":
    app.run(
        host="0.0.0.0",
        port=5000,
        debug=False,
    )
