# Memory Map Analysis

**Board:** Arduino Uno
**MCU:** ATmega328P

## Flash Memory

```text
.text = 2936 bytes
.data = 96 bytes
```

Flash used:

```text
2936 + 96 = 3032 bytes
```

Flash available to the sketch:

```text
32256 bytes
```

Flash remaining:

```text
32256 - 3032 = 29224 bytes
```

Flash usage:

```text
~9%
```

## SRAM Memory

```text
.data = 96 bytes
.bss  = 196 bytes
```

SRAM used:

```text
96 + 196 = 292 bytes
```

SRAM available:

```text
2048 bytes
```

SRAM remaining:

```text
2048 - 292 = 1756 bytes
```

SRAM usage:

```text
~14%
```

## Memory Summary

| Memory |   Used | Available | Remaining |
| ------ | -----: | --------: | --------: |
| Flash  | 3032 B |   32256 B |   29224 B |
| SRAM   |  292 B |    2048 B |    1756 B |

## Stack

The **1756 bytes** are the remaining SRAM budget for local variables and runtime stack use.

This is **not the actual peak stack usage**.

## Result

The firmware is within the Arduino Uno memory limits.
