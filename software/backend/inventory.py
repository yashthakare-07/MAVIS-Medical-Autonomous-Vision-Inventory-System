"""
Thread-safe JSON inventory management for M.A.V.I.S.
"""

from __future__ import annotations

import json
import threading
from pathlib import Path
from typing import Any


class InventoryManager:
    """Manage inventory data stored in a JSON file."""

    def __init__(self, inventory_file: Path) -> None:
        self.inventory_file = inventory_file
        self._lock = threading.Lock()

        self.inventory_file.parent.mkdir(
            parents=True,
            exist_ok=True,
        )

        if not self.inventory_file.exists():
            self._write({})

    def _read(self) -> dict[str, Any]:
        with self.inventory_file.open("r", encoding="utf-8") as file:
            return json.load(file)

    def _write(self, data: dict[str, Any]) -> None:
        with self.inventory_file.open(
            "w",
            encoding="utf-8",
        ) as file:
            json.dump(
                data,
                file,
                indent=2,
            )

    def get_all(self) -> dict[str, Any]:
        """Return the complete inventory."""
        with self._lock:
            return self._read()

    def get_item(self, item_id: str) -> dict[str, Any] | None:
        """Return one inventory item."""
        with self._lock:
            data = self._read()
            return data.get(item_id)

    def decrement(self, item_id: str) -> bool:
        """
        Decrement inventory after successful physical verification.

        Returns:
            True if stock was successfully decremented.
            False if the item does not exist or is out of stock.
        """
        with self._lock:
            data = self._read()

            if item_id not in data:
                return False

            stock = int(data[item_id].get("stock", 0))

            if stock <= 0:
                return False

            data[item_id]["stock"] = stock - 1
            self._write(data)

            return True
