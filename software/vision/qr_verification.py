"""
M.A.V.I.S. QR-based spatial verification.

The camera is used to verify the robot's physical node rather than
performing continuous visual navigation.
"""

from __future__ import annotations

import logging
import urllib.request

import cv2
import numpy as np


LOGGER = logging.getLogger(__name__)


class QRVerificationError(RuntimeError):
    """Raised when the camera or QR verification pipeline fails."""


def verify_inventory_node(
    webcam_url: str,
    timeout: float = 2.0,
) -> str | None:
    """
    Capture one frame from the IP webcam and decode a QR code.

    Args:
        webcam_url: IP Webcam snapshot endpoint.
        timeout: Network timeout in seconds.

    Returns:
        Decoded QR identifier or None when no QR code is detected.
    """

    try:
        with urllib.request.urlopen(
            webcam_url,
            timeout=timeout,
        ) as response:
            image_bytes = response.read()

    except Exception as exc:
        LOGGER.error("Camera acquisition failed: %s", exc)
        raise QRVerificationError(
            "Unable to acquire camera frame."
        ) from exc

    image_array = np.frombuffer(
        image_bytes,
        dtype=np.uint8,
    )

    frame = cv2.imdecode(
        image_array,
        cv2.IMREAD_COLOR,
    )

    if frame is None:
        raise QRVerificationError(
            "Camera returned an invalid image."
        )

    detector = cv2.QRCodeDetector()

    data, _, _ = detector.detectAndDecode(frame)

    if not data:
        LOGGER.warning("No QR code detected.")
        return None

    decoded_id = data.strip()

    LOGGER.info(
        "QR node verified: %s",
        decoded_id,
    )

    return decoded_id
