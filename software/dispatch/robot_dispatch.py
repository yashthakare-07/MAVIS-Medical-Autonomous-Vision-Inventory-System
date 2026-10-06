"""
M.A.V.I.S. robot dispatch client.

The dispatch protocol follows the documented project interface:

    GOTO,<node_id>
"""

from __future__ import annotations

import logging

import requests


LOGGER = logging.getLogger(__name__)


class RobotDispatchError(RuntimeError):
    """Raised when the robot cannot be reached or rejects a command."""


def dispatch_robot_to_node(
    esp_ip: str,
    api_token: str,
    node_id: str,
    timeout: float = 3.0,
) -> bool:
    """
    Send a navigation command to the ESP32.

    Args:
        esp_ip: ESP32 network address.
        api_token: Authentication token.
        node_id: Destination node identifier.
        timeout: HTTP timeout in seconds.

    Returns:
        True when the ESP32 accepts the command.

    Raises:
        RobotDispatchError:
            When communication fails or the ESP32 rejects the command.
    """

    if not node_id.strip():
        raise ValueError("node_id must not be empty")

    url = f"http://{esp_ip}/command"

    headers = {
        "Content-Type": "text/plain",
    }

    if api_token:
        headers["Authorization"] = f"Bearer {api_token}"

    payload = f"GOTO,{node_id}"

    LOGGER.info("Dispatching robot: %s", payload)

    try:
        response = requests.post(
            url,
            headers=headers,
            data=payload,
            timeout=timeout,
        )

    except requests.RequestException as exc:
        LOGGER.error("ESP32 communication failure: %s", exc)
        raise RobotDispatchError(
            "Unable to communicate with ESP32."
        ) from exc

    if response.status_code != 200:
        LOGGER.error(
            "ESP32 rejected command with status %s",
            response.status_code,
        )

        raise RobotDispatchError(
            f"ESP32 rejected command: HTTP {response.status_code}"
        )

    LOGGER.info("Dispatch command accepted by ESP32.")
    return True
