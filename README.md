# Arduino Uno Embedded Firmware

## Overview

This project is developed using **Arduino Uno (ATmega328P)**.

It contains:

* UART Ring Buffer
* Non-blocking State Machine
* LCD display
* Memory analysis
* Test results

## Hardware

**Board:** Arduino Uno
**MCU:** ATmega328P
**Baud Rate:** 9600

## Pin Configuration

| Device        | Pin |
| ------------- | --- |
| LCD RS        | D8  |
| LCD EN        | D9  |
| LCD D4        | D4  |
| LCD D5        | D5  |
| LCD D6        | D6  |
| LCD D7        | D7  |
| SELECT Button | A0  |

## Module 1 - UART Ring Buffer

File: `module1_uart.ino`

* RX and TX ring buffers are used.
* Buffer size is 16 bytes.
* FIFO order is maintained.
* New data is dropped when the buffer is full.
* Overflow is counted.
* UART baud rate is 9600.

## Module 2 - State Machine

File: `module2_state_machine.ino`

States:

```text
IDLE → RUNNING → DONE → IDLE
```

* SELECT button starts RUNNING.
* RUNNING lasts 5 seconds.
* DONE lasts 2 seconds.
* `millis()` is used, so the code is non-blocking.

## Memory Usage

```text
Flash Used: 3032 bytes
Flash Available: 32256 bytes
Flash Usage: ~9%

SRAM Used: 292 bytes
SRAM Available: 2048 bytes
SRAM Remaining: 1756 bytes
SRAM Usage: ~14%
```

## Files

```text
README.md
module1_uart.ino
module2_state_machine.ino
memory_map_analysis.md
test_logs.txt
```

## Result

UART tests: **PASS**

State machine tests: **PASS**

Memory test: **PASS**

Overall Result: **PASS**
