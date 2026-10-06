# Software Architecture

## Centralized Control

The Python Flask server acts as the centralized control unit.

Its documented responsibilities include:

- Order processing
- System state tracking
- Robot communication
- Inventory management
- Computer vision coordination

## Inventory

Inventory state is maintained using a structured JSON file.

The report identifies inventory/node identifiers such as:

```text
store1_medA
store2_medB
