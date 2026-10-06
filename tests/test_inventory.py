from pathlib import Path

from software.backend.inventory import InventoryManager


def test_inventory_decrement(tmp_path: Path) -> None:
    inventory_file = tmp_path / "inventory.json"

    manager = InventoryManager(inventory_file)

    manager._write(
        {
            "store1_medA": {
                "stock": 2
            }
        }
    )

    assert manager.decrement("store1_medA") is True

    item = manager.get_item("store1_medA")

    assert item is not None
    assert item["stock"] == 1


def test_inventory_rejects_empty_stock(tmp_path: Path) -> None:
    inventory_file = tmp_path / "inventory.json"

    manager = InventoryManager(inventory_file)

    manager._write(
        {
            "store1_medA": {
                "stock": 0
            }
        }
    )

    assert manager.decrement("store1_medA") is False
