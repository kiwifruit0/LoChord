# CLAUDE.md — LoChord

Guidance for Claude when working anywhere in this repository. This file covers the project as a whole. Each subdirectory has its own `CLAUDE.md` with directory-specific detail — read this one first, then the relevant subdirectory file for whatever you're working on.

## What LoChord is

LoChord is a real-time MIDI chord controller, built on an **ESP32-P4** with PlatformIO/Arduino, that will later grow into a standalone synthesiser. It's directly inspired by the [HiChord](https://hichord.shop/) — a pocket chord instrument where buttons trigger chords within a scale and a joystick reshapes chord voicing on the fly — but LoChord is not a clone. Treat the HiChord as a reference for the *interaction model* (buttons = chords in a scale, joystick = chord quality/voicing, loop button = record/play/overdub), not as a spec to copy feature-for-feature.

### How LoChord differs from the HiChord

| Area | HiChord | LoChord |
|---|---|---|
| Sound source | Built-in synthesis engine (30+ instruments), speaker + headphone out | USB MIDI controller first; onboard **wavetable synth** is future work. Headphone jack and **ES8388 codec** are on the board now, output-only, provisioned for that future synth |
| Control surface | Buttons + joystick, no rotary encoders | Adds 1 clickable UI encoder (menu/parameter navigation) + 2 clickable control encoders (continuous parameters, e.g. BPM, strum) on top of the button + joystick layout |
| Settings / menus | Minimal on-device UI | **PLACEHOLDER** — the dedicated settings/menu system isn't detailed yet. Is this driven by the UI encoder click, a separate physical button, or something else? Tell me and I'll fill this in properly. |
| Display | Small onboard screen | 428×142 2.79" NV3007 TFT, 8-pin SPI/FPC, LVGL UI built in EEZ Studio |
| Looping | Up to 6 layered tracks, auto-synced | Single loop button driving a record/play/overdub/stop state machine (track count TBD) |
| Connectivity | USB-C, class-compliant MIDI *and* audio | Native USB (ESP32-P4 Full-Speed OTG), **MIDI only**. Recording is via the 3.5mm jack into an audio interface, not USB Audio Class |

Claude should default to LoChord's own spec (below, and `PINOUT.md`) rather than assuming HiChord behaviour, and should flag it if a request seems to conflict with this table.

## Why the ESP32-P4

LoChord moved from the ESP32-S3-WROOM-1 (N8R2) to the **WT0132P4-A1-N16R32** module during design. Anything in this repo, in git history, or in an older conversation that assumes an S3 is out of date. The reasons, in order of weight:

1. **The S3 board was cornered on pins.** Four spares against 32 assigned, two of them barely usable. Every new feature meant taking something else off the board. The P4 leaves 14 genuinely free GPIOs after everything is assigned.
2. **The S3 had no debug channel.** TinyUSB claimed GPIO19/20 for USB MIDI, which killed USB-Serial-JTAG *and* pin-JTAG simultaneously — leaving `printf`-over-CDC as the only way to see inside a running board. The P4 has physically separate USB PHYs, so a live hardware debugger runs concurrently with USB MIDI.
3. **The radio was pure cost.** LoChord uses no Wi-Fi or Bluetooth and never will. On the S3 that meant paying for silicon, accepting an antenna keep-out, and placing an RF source next to an analogue audio stage — all for nothing. The P4 has no radio at all.
4. **PSRAM stops being a pin problem.** The S3's quad-vs-octal PSRAM choice constrained which GPIOs the chord buttons could use. The P4 module stacks 32MB in-package; there is no external PSRAM bus and no conflict.

Raw speed was *not* a deciding factor. MIDI plus a 428×142 LVGL UI does not need 360MHz, and the P4 will likely be clocked down to help the USB current budget.

**The cost of the move**, so it isn't rediscovered as a surprise: a rebuilt power tree (the module takes 5V, not 3V3), a harder-to-solder module, a real USB current budget to size, and a less mature Arduino layer than the S3 enjoyed. See `firmware/CLAUDE.md` for the last of those.

## Repository layout

```
/
├── firmware/     ESP32-P4 PlatformIO project (Arduino framework, via pioarduino). See firmware/CLAUDE.md.
├── lochord_hw/   KiCad hardware project (schematic + PCB). See lochord_hw/CLAUDE.md.
└── PINOUT.md     Single source of truth for every net/pin that crosses the firmware↔hardware boundary.
```

The two halves are developed together but worked on through different tools: firmware via Claude Code, hardware via a KiCad MCP server. **Any change to a pin assignment, connector, or footprint has to be reflected on both sides** — that's what `PINOUT.md` is for.

## The firmware↔hardware boundary: PINOUT.md

`PINOUT.md` at the repo root lists every GPIO/net assignment — which ESP32-P4 pin drives which button, encoder, the LCD SPI bus, the joystick FPC, the ES8388 codec, and so on.

Rules for Claude:
- **Before** writing firmware code that references a pin, or **before** proposing a schematic/net change in KiCad, check `PINOUT.md` first.
- If a hardware change moves a signal to a different pin, update `PINOUT.md` as part of that same piece of work, and flag that `firmware/include/config.h` needs the matching update.
- If firmware work needs a pin that isn't in `PINOUT.md` yet, don't invent a number — add it as `TBD` and ask.
- **Rows in `PINOUT.md` marked `verify` are not confirmed against the module datasheet.** Don't treat them as settled, and don't quietly promote one to settled without actually checking.

## Hardware components at a glance

(Pin-level detail lives in `PINOUT.md`; layout/mechanical detail lives in `lochord_hw/CLAUDE.md`.)

- **WT0132P4-A1-N16R32** module — ESP32-P4, 16MB flash, 32MB in-package PSRAM, castellated, no radio
- 7× **Kailh Choc v1** switches, hand-soldered (not hotswap) — chord buttons. See "Chord button layout" below — the geometry is easy to get wrong, so read it before drawing, describing, or laying out the pad
- 1× Kailh Choc v1 switch (1u) — loop control button
- 428×142 2.79" NV3007 TFT LCD, 8-pin SPI, FPC — UI built with EEZ Studio + LVGL
- 3× ALPS EC11E15244G1 clickable rotary encoders — 1 for UI navigation, 2 for continuous parameter control
- 1× Nintendo Switch joystick module, 5-pin 0.5mm FPC — chord quality/voicing control
- **ES8388 stereo audio codec** with integrated headphone amplifier, I2C-controlled
- 1× headphone jack, output-only, driven directly by the ES8388
- EN + BOOT switches
- 4-pin debug header (USB-Serial-JTAG)
- 1× power-on LED

## Chord button layout

The 7 chord switches form a single rectangular pad, **4u wide × 3u tall**, made of two rows that share the same left and right edges.

**"2u" here means two units _tall_, not two units wide.** The bottom keys are tall and narrow — 1u wide × 2u tall, the orientation of a numpad's vertical Enter key — *not* wide horizontal 2u keys. Getting this backwards is the single most common way this layout gets rendered wrongly.

- **Bottom row** — 4 keys, each 1u wide × 2u tall, sitting directly side by side. Together they span the full 4u width.
- **Top row** — 3 keys, each 1u tall, widths **1.5u – 1u – 1.5u** left to right. Together they also span the full 4u width.

Because the top row's widths (1.5/1/1.5) don't divide the same way as the bottom row's (1/1/1/1), the vertical seams of the two rows **do not line up**. The seams in the top row fall at 1.5u and 2.5u from the left edge, which is the *middle* of the second and third bottom keys respectively. This offset is intentional and is what gives the pad its interlocking look — don't "tidy" it into aligned columns.

```
 ┌───────────┬───────┬───────────┐
 │   1.5u    │  1u   │   1.5u    │   top row: 3 keys, 1u tall
 ├───────┬───┴───┬───┴───┬───────┤
 │       │       │       │       │
 │  2u   │  2u   │  2u   │  2u   │   bottom row: 4 keys, 1u wide × 2u tall
 │       │       │       │       │
 └───────┴───────┴───────┴───────┘
   ← 1u →                              total pad: 4u wide × 3u tall
```

The two rows are adjacent — the bottom edge of the top row is the top edge of the bottom row. There's no gap band between them beyond the normal switch spacing.

The 1u loop button is a separate switch and is **not** part of this pad.

The switches are **Kailh Choc v1** (PG1350), not v2. Footprint and keycap implications are in `lochord_hw/CLAUDE.md`.

For which GPIO drives which physical key, see `PINOUT.md`. On the P4 map the array index order and physical position match by construction (ascending GPIO runs bottom-left → bottom-right, then top-left → top-right), so `buttonPins[]` order *is* physical order — but the mapping from physical key to **musical scale degree** is still open and shouldn't be assumed.

## Future: onboard wavetable synthesiser

Planned, not yet built — but the hardware for it is now on the board, which is a change from earlier revisions where the audio path was a placeholder.

**Why it touches both sides:** the ESP32-P4 has no built-in analogue DAC (nor does any comparable MCU — this is not a P4 shortcoming). Getting real audio out of the headphone jack means an external codec on the hardware side, plus a wavetable synthesis engine and an I2S driver on the firmware side.

**What's decided:**
- **Codec: ES8388**, a stereo I2S codec with integrated headphone amplifier, controlled over I2C. It drives the 3.5mm jack directly — there is no separate amplifier IC.
- **Volume lives in the codec**, not in firmware. The ES8388's DAC volume registers are the single control point, written over I2C from the UI. Firmware must **not** also apply a software gain stage — one volume, one owner. This is a change from an earlier revision of these docs, which recorded a firmware-gain approach; that was never a decision the project made, and it has been superseded.
- **The ES8388's ADC half is unused.** No mic, no line-in, no `ADCDAT`. It's powered down at init. This is the known cost of choosing a codec over a discrete DAC.
- **No USB Audio Class.** Recording is via the headphone jack into an audio interface. USB stays MIDI-only.

**Still open:** sample rate and bit depth, wavetable source and format, target voice count, and the volume taper (which encoder drives it and with what curve). Detail and remaining decisions are split between `firmware/CLAUDE.md` and `lochord_hw/CLAUDE.md`.

**Sequencing:** MIDI output, chord generation, and looping are the current priority. The synth engine is deliberately being designed for *later* — but unlike earlier revisions, the hardware it will drive is being designed now, so firmware groundwork (task structure, PSRAM assumptions) should assume the ES8388 exists.

## Workflow conventions

- **Single repo, single history.** Firmware and hardware live in one repo and are committed together — there's no separate versioning for `lochord_hw/`. A change that touches both sides (e.g. moving a pin) goes in as normal commits in the same repo, not a submodule bump or cross-repo reference. (The `assets/kiswitch` footprint library is the one exception — that's an actual git submodule; see `lochord_hw/CLAUDE.md`.)
- **Claude edits the schematic and PCB directly via the KiCad MCP server.** No propose-then-wait-for-approval step for routine schematic/PCB edits — treat it the same as editing a firmware file. See `lochord_hw/CLAUDE.md` for the MCP-specific detail.
- **Commit message convention** — prefix every commit with one of:
  - `fw:` — firmware-only changes (anything under `firmware/`)
  - `hw:` — hardware-only changes (anything under `lochord_hw/`, including schematic/PCB edits made via MCP)
  - `chore:` — repo-wide changes that aren't feature work on either side: restructuring, `PINOUT.md`-only edits, tooling, docs, `CLAUDE.md` updates, `.gitignore`, etc.
  - For a commit that necessarily changes both sides together (e.g. reassigning a pin: `PINOUT.md` + `config.h` + the schematic net in one commit), use `fw+hw:` rather than picking one arbitrarily.
  - Keep the rest of the message lowercase and imperative, e.g. `fw: add strum direction toggle`, `hw: swap choc v2 footprints for v1`, `fw+hw: move codec i2c to spare pins`.

## Project status

Actively in development, **mid-migration from ESP32-S3 to ESP32-P4**. `PINOUT.md` is written against the P4; `firmware/include/config.h` and the schematic are not yet. Until a P4 board boots, keep the S3 assignment in version control as a fallback reference.

Chord generation, MIDI output, and looping remain the functional focus. The onboard synthesiser is future work, but its hardware is being designed in now.

### Open decisions

Tracked here so they don't get rediscovered. Each is recorded in more detail in the relevant file.

- **Chord key → scale degree mapping.** Physical order is settled; musical order isn't. (`PINOUT.md`)
- **Encoder debouncing.** RC in hardware, firmware-only, or both — this changes the schematic. (`PINOUT.md`)
- **Settings / menu system.** Interaction model not defined. (this file, `firmware/CLAUDE.md`)
- **Sample rate, bit depth, voice count, wavetable format.** (`firmware/CLAUDE.md`)
- **Volume taper and which encoder drives it.** The *where* is decided (codec I2C); the *how* isn't. (`firmware/CLAUDE.md`)
- **Both LDO part numbers**, subject to JLCPCB Basic Parts availability. (`lochord_hw/CLAUDE.md`)
- **Mechanical / form factor.** Nothing captured. (`lochord_hw/CLAUDE.md`)
- **Battery.** Presence undecided. (`PINOUT.md`, `lochord_hw/CLAUDE.md`)

### Closed decisions

Don't reopen these without a reason, and don't let stale docs or old conversations reintroduce them:

- **MCU** — ESP32-P4 (WT0132P4-A1-N16R32 module). Not the S3, not a bare P4 chip.
- **I2C GPIO expander (TCA9555/PCF8575)** — not needed. It only ever existed to work around S3 pin starvation.
- **Encoder-switch resistor ladder** — same; not needed.
- **Quad vs octal PSRAM** — moot. PSRAM is in-package on the P4 module.
- **Audio codec** — ES8388. Not a discrete DAC + amp, not a PCM5102A.
- **Volume control location** — ES8388 registers over I2C, not firmware gain, not a physical pot.
- **Switch type** — Kailh Choc v1, hand-soldered.
- **USB Audio Class** — not implemented. Aux cable into an audio interface instead.
