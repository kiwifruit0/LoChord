# LoChord

A real-time MIDI chord controller for **ESP32-P4**, with chord generation, looping, and parameter control. Inspired by the [HiChord](https://hichord.shop/), but not a clone — see `CLAUDE.md` for how the two differ.

LoChord is both a firmware project and a hardware project developed together in one repo.

> **Mid-migration.** The project moved from the ESP32-S3-WROOM-1 to the WT0132P4-A1 (ESP32-P4) module during design. `PINOUT.md` is written against the P4; `firmware/include/config.h` and the KiCad schematic are not yet. See `PINOUT.md`'s "Migration delta" for the work list, and the root `CLAUDE.md` for why the move was made.

## Hardware at a glance

- **WT0132P4-A1-N16R32** module — ESP32-P4, dual-core RISC-V, 16MB flash, 32MB in-package PSRAM, no radio
- 7× Kailh Choc v1 chord switches + 1× loop button, hand-soldered, direct GPIO
- 3× ALPS EC11 clickable rotary encoders (1 UI, 2 control)
- Nintendo Switch joystick module on a 5-pin FPC
- 428×142 2.79" NV3007 TFT LCD, SPI, LVGL UI built in EEZ Studio
- **ES8388** stereo I2S codec with integrated headphone amp, driving a 3.5mm jack
- Native USB, **MIDI only** — recording is via the headphone jack into an audio interface
- 4-pin USB-Serial-JTAG debug header (works concurrently with USB MIDI)

## Repository layout

```
/
├── firmware/     ESP32-P4 PlatformIO project (Arduino framework, via pioarduino)
├── lochord_hw/   KiCad hardware project (schematic + PCB)
└── PINOUT.md     Single source of truth for every pin/net crossing the firmware↔hardware boundary
```

Each directory has its own `CLAUDE.md` with more detail; start with the root `CLAUDE.md` for the full picture.

## Firmware

Built against **`pioarduino/platform-espressif32`**, not the official `platformio/espressif32` platform. The official platform never picked up Arduino-ESP32 3.x (needed for USB MIDI) and has no ESP32-P4 support.

A **custom board JSON is required** — no stock definition matches the N16R32 (16MB flash / 32MB PSRAM) module.

### Development commands

Run from `firmware/`:

Generate `compile_commands.json`:
```
pio run -t compiledb
```

Build and upload:
```
pio run -t upload
```

Serial monitor:
```
pio device monitor
```
Exit with `Ctrl+A` then `Ctrl+X`

Host tests:
```
pio test -e native
```

## Hardware

The schematic (`lochord_hw.kicad_sch`) and PCB (`lochord_hw.kicad_pcb`) live in `lochord_hw/`. After a fresh clone, run:
```
git submodule update --init
```
to pull in the Kailh switch footprint library — without it, KiCad can't resolve the switch footprints.

Note that the switches are **Choc v1** (`PG1350`). Any Choc v2 footprint still present in the schematic is left over from an earlier revision and is wrong.

## Commit convention

Commits are prefixed by which side of the repo they touch:

- `fw:` — firmware-only changes
- `hw:` — hardware-only changes (including schematic/PCB edits)
- `chore:` — repo-wide changes that aren't feature work on either side (restructuring, docs, tooling)
- `fw+hw:` — changes that necessarily touch both sides together (e.g. reassigning a pin)

e.g. `fw: add strum direction toggle`, `hw: swap choc v2 footprints for v1`, `fw+hw: renumber gpio for esp32-p4`.
