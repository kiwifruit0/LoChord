# CLAUDE.md — firmware/

The ESP32-P4 firmware half of LoChord (PlatformIO, Arduino framework). See the root `CLAUDE.md` for what the project is and how this relates to `lochord_hw/`. See `../PINOUT.md` for the canonical pin assignments — `include/config.h` should always match it.

**Migration status:** `PINOUT.md` is written against the ESP32-P4; `include/config.h` is **not yet**. Every GPIO number changes. Until that's done, treat any pin constant in this tree as stale.

## Platform

LoChord builds against **`pioarduino/platform-espressif32`**, not the official `platformio/espressif32` platform.

This matters and shouldn't be "corrected" back:

- The official PlatformIO Espressif32 platform stalled and never picked up Arduino-ESP32 3.x, which is where the TinyUSB-based USB stack (and therefore USB MIDI) lives. That's why the project moved to pioarduino in the first place, before the P4 was on the table.
- pioarduino is also the only PlatformIO platform with ESP32-P4 support. The toolchain decision made for USB MIDI happens to be the one that makes the P4 possible at all.

**A custom board JSON is required.** No stock board definition matches the WT0132P4-A1-N16R32 (16MB flash / 32MB PSRAM). Copy an existing `esp32p4` definition and adjust the flash size and PSRAM configuration. Getting the PSRAM size and speed flags wrong tends to surface as boot failures or corrupted PSRAM access rather than a clear error message, so verify against a known-good blink before building on top of it.

### Known-immature areas

Arduino-ESP32's P4 support is considerably younger than its S3 support. Nothing here is architecturally blocked, but budget integration time:

- **USB MIDI on P4 via the Arduino layer.** TinyUSB's P4 support was merged upstream and ESP-IDF's device stack lists MIDI as a supported class, but the Arduino-level `USBMIDI` path on this specific chip is newer than the equivalent S3 path. **Flash a minimal note-on sketch and confirm the OS sees a MIDI device before writing any chord logic against it.**
- **ES8388 driver.** Espressif's `esp_codec_dev` component is the maintained driver path and supports the ES8388, but it's an ESP-IDF component — using it from the Arduino framework may need it pulled in as a component dependency rather than an Arduino library. If that proves awkward, a direct I2C register-write init is a viable fallback; the ES8388 datasheet's recommended sequence is the reference.
- **ADC calibration** on P4 under Arduino.

Where the Arduino layer falls short, dropping to ESP-IDF APIs for that one peripheral is acceptable and preferable to fighting it. Say so in a comment when you do.

## Commands

All commands run from `firmware/`.

- Build: `pio run`
- Upload: `pio run -t upload`
- Serial monitor (115200): `pio device monitor` — quit with `Ctrl+A` then `Ctrl+X`
- Editor index / `compile_commands.json`: `pio run -t compiledb`
- Host tests (Unity): `pio test -e native`
  - Run a single suite: `pio test -e native -f <suite-name>`
  - Test files currently live directly in `firmware/test/`, so PlatformIO treats them as one suite named `*` — `-f` filtering only becomes useful once tests are split into one subdirectory per suite
- No lint/format tooling is configured yet

**PLACEHOLDER** — the P4 environment name isn't settled. The S3 environment was `esp32-s3-devkitc1-n8r2`, named after a devkit that no longer applies. Pick a name once the custom board JSON exists and update this section and `platformio.ini` together.

### Debugging

Unlike the S3 board, **a real hardware debugger is available**, and it runs concurrently with USB MIDI.

The P4 has physically separate USB PHYs: USB MIDI on the Full-Speed OTG (GPIO26/27), and USB-Serial-JTAG (GPIO24/25) broken out to a 4-pin debug header. Breakpoints, single-stepping, and memory inspection work with MIDI active — no probe hardware needed, just a USB cable to the header.

On the S3 this was impossible: TinyUSB claimed the only USB pins and `printf`-over-CDC was the sole channel. Any debugging advice in this repo predating the migration assumes that limitation and is obsolete.

Use it. For a real-time system, timing bugs are exactly the ones a print statement disturbs enough to hide.

## Key constraints (from the project spec)

- Must use native USB — the ESP32-P4's Full-Speed OTG port, not a USB-to-serial bridge
- Must call `USB.begin()` to initialise native USB
- TinyUSB must be enabled via PlatformIO build flags
- USB is **MIDI only**. USB Audio Class is not implemented and isn't planned for this board — recording is via the headphone jack into an audio interface
- This is a real-time performance instrument: the system must stay responsive and non-blocking. Avoid blocking delays anywhere in the input → MIDI path
- The headphone output is provisioned for a future onboard synthesiser and isn't driven by firmware yet — don't assume audio synthesis code exists. The **codec hardware does** exist now, which is a change from earlier revisions

## Architecture

`src/main.cpp` wires everything together: inputs (`Button`, `Joystick`, `RotaryEncoder`), timing (`Clock`), harmony (`ChordGenerator`), playback (`Sequencer` through `MidiController`), the display/UI, and USB MIDI output.

None of this is MCU-specific — the S3→P4 migration touches `config.h`, `platformio.ini`, and peripheral setup in `main.cpp`, and nothing below.

- `lib/Chords/src/Theory.h` — centralises all musical data: note names, chord qualities, scale intervals, joystick extension mappings, fixed chord templates. **Enum ordering here is behaviour-critical**: `Quality` and related enums are used as direct indexes into static tables. Don't reorder them without updating every table indexed by them.
- `lib/Chords/src/ChordGenerator.*` — converts a scale degree + joystick position into a concrete MIDI chord or note.
- `lib/Midi/src/MidiController.*` — owns note lifecycle and playback behaviour: active chord tracking, note-on/off, chord mode vs single-note mode, velocity calculation, sequencer updates.
  - `processNoteOn` always stops currently active notes before starting a new one.
  - `processNoteOff` ignores releases that don't match the active root.
  - Strum and arp modes are mutually exclusive: enabling one disables the other, and arp state resets when arp is turned on.
- `lib/Sequencer/src/Sequencer.*` — strum/arp timing, direction modes, note ordering, using `Clock` for beat-to-microsecond conversion.
- `lib/Midi/src/MidiOutput.*` — the USB MIDI boundary the controller writes to.
- `include/config.h` — all GPIO pin assignments. Keep in sync with `../PINOUT.md`; if they disagree, `PINOUT.md` is the source of truth and `config.h` is wrong.

## Physical control layout

Firmware doesn't lay out the hardware, but anything that maps a button index to a position on the device (UI hints, chord-to-key assignment, on-screen key highlighting) has to match the real pad. The full description lives in the root `CLAUDE.md`; the short version:

The 7 chord switches form one pad, 4u wide × 3u tall. **"2u" means two units _tall_, not wide** — the bottom keys are narrow and tall.

```
 ┌───────────┬───────┬───────────┐
 │   1.5u    │  1u   │   1.5u    │   top row: 3 keys, 1u tall
 ├───────┬───┴───┬───┴───┬───────┤
 │       │       │       │       │
 │  2u   │  2u   │  2u   │  2u   │   bottom row: 4 keys, 1u wide × 2u tall
 │       │       │       │       │
 └───────┴───────┴───────┴───────┘
```

`buttonPins[NUM_BUTTONS]` in `include/config.h` is a flat array of GPIOs, one per switch, read directly (not matrix-scanned).

**On the P4 map, array order and physical order match by construction.** Ascending GPIO (44→50) runs bottom-left → bottom-right, then top-left → top-right, and `PINOUT.md` specifies the wiring rather than assuming it. This is a change from the S3 revision, where the mapping was flagged as unconfirmed — UI and highlighting code can now rely on index order being physical order.

What is **not** settled is the mapping from physical key to **musical scale degree**. The HiChord interleaves odd degrees on the bottom row and even on the top; LoChord hasn't decided. Don't hardcode a degree order until it's recorded in `PINOUT.md`.

## Data types

- `Chord` is a fixed-capacity value type: `std::array<int, MAX_CHORD_NOTES>` + a size field. No dynamic containers (`std::vector`, etc.) for chord data.

## UI is generated code — do not hand-edit

- Source of truth is the EEZ Studio project `firmware/lochord.eez-project` (LVGL 9, 428×142, BGR, embedded fonts/bitmaps).
- `firmware/lib/UI/src/` (`screens.c`, `ui.c`, `styles.c`, `fonts.h`, `images.*`, etc.) is EEZ Studio *output*. Make UI changes in EEZ Studio and re-export — hand-editing generated files means losing those edits on the next export.
- `ui.c` loads screens and ticks the current screen.
- LVGL config lives in `include/lv_conf.h`, pulled in via `-DLV_CONF_INCLUDE_SIMPLE` and `-DEEZ_FOR_LVGL` (defined in `platformio.ini`, not in the file itself).

The panel and resolution are unchanged by the P4 migration — the NV3007 stays on SPI, so no EEZ Studio work needs redoing. Two P4 capabilities are worth using once the basics are stable: the **PPA (pixel processing accelerator)** and 2D-DMA can accelerate LVGL blits, and 32MB PSRAM makes full double-buffering trivial (428 × 142 × 2 = 121KB). Neither is required for correctness.

Known EEZ Studio gotchas, recorded so they aren't rediscovered:
- The **Button Matrix widget can't do mixed typography within a cell** — use a reusable User Widget with three individually styled labels instead.
- **Grid layout**: the property checkbox must be explicitly ticked alongside entering a value (unticked = ignored even with a value present); the container needs explicit pixel dimensions for `FR()` tracks to work; `LV_USE_GRID` must be enabled in `lv_conf.h`.
- Encoder navigation requires **Group assignment with sequential index values**.

## Audio: ES8388 codec

The hardware is decided — an **ES8388 stereo I2S codec with integrated headphone amplifier**, on I2S plus I2C. The synth engine that will feed it isn't built, but the driver-side shape is now knowable rather than a placeholder.

- **I2S**: MCLK, BCLK, WS, DOUT. **MCLK is mandatory** for the ES8388 — this is a difference from a PCM5102A-class DAC, which runs off an internal PLL. Pins are in `../PINOUT.md`.
- **I2C**: SDA/SCL plus an active-low reset line. Needed at boot for codec init, and thereafter for volume.
- **No I2S DIN.** The ES8388's ADC is unused and should be explicitly powered down during init rather than left running.
- **Init is a real sequence, not a single register write.** Power-up ordering, sample rate and format configuration, enabling the DAC path, and unmuting in the right order — follow the datasheet's recommended sequence. Getting the order wrong is the usual cause of power-on pops even on a codec that handles muting internally.

### Volume control — decided

**Volume lives in the ES8388's DAC volume registers, written over I2C. Firmware must not apply a software gain stage.**

This supersedes an earlier revision of this file that recorded firmware-side gain as decided. That was never a decision the project made — it was an assumption that propagated through the docs. The codec was chosen partly *for* its hardware volume, so using it is the point.

Practical consequences for the synth engine:

- The mixing stage sums active voices and soft-clips/limits, and **stops there**. No master-gain multiply before the buffer goes to I2S.
- Because there's no software attenuation, the low-volume bit-depth loss and dithering problem that firmware gain would have introduced doesn't arise. Don't add dithering to solve a problem the design doesn't have.
- Volume changes are I2C writes, which are slow relative to the audio callback. **Never write codec registers from the audio task.** Queue the change from the UI task; the codec ramps internally.
- If a future feature genuinely needs per-voice or per-layer gain (e.g. mixing a loop against live playing), that's a different thing from master volume and can live in firmware — but master volume stays in the codec, and there should never be two places that attenuate the master bus.

**Still open:** the volume taper (linear vs logarithmic in dB terms) and which of the two control encoders drives it.

## Future: onboard synthesiser (wavetable engine)

Not built yet. This is the intended shape, so groundwork laid now doesn't have to be redone. See the root `CLAUDE.md` for why this is cross-cutting and `lochord_hw/CLAUDE.md` for the codec/analogue side.

- **Real-time architecture**: run the synthesis engine as its own FreeRTOS task, pinned to a dedicated core, fed by a lock-free queue of note-on/off events from `MidiController`. Synthesis must never block, or be blocked by, chord generation, MIDI I/O, or UI drawing — the existing non-blocking constraint applies just as much here. The P4 is dual-core, so a dedicated core is available.
- **Wavetable oscillator**: phase-accumulator per voice reading into a single-cycle wavetable, with at least linear interpolation between samples for pitch accuracy across the range.
- **Polyphony**: voice count should follow the same fixed-capacity philosophy as `Chord` (i.e. bounded by `MAX_CHORD_NOTES`, no dynamic voice allocation) — a chord shouldn't be able to demand more voices than the engine has reserved.
- **Envelope**: per-voice ADSR.
- **Mixing**: sum active voices, then soft-clip/limit, then hand the buffer to I2S. No master gain — see above.
- **Memory**: 32MB PSRAM is available in-package on the WT0132P4-A1-N16R32. Wavetables and the audio ring buffer can live there comfortably. Confirm PSRAM is enabled and correctly sized in the custom board JSON before any synth work assumes it.
- **Clock budget**: the P4 is likely to be clocked *down* from 360MHz to help the USB current budget (see `../PINOUT.md`). Don't design the engine assuming full clock — establish the actual operating frequency first, then size voice count to it.

**Open decisions:**
- Sample rate / bit depth (e.g. 44.1kHz vs 48kHz, 16-bit vs 24-bit). The ES8388 handles the common rates, so this is a firmware choice rather than a part constraint — but pick one canonical internal rate and resample nowhere else
- Wavetable source and format — hand-designed single-cycle waves, extracted from samples, or a mix; how many instrument tables to ship
- Target voice count (driven by max chord size plus any arp/strum overlap, and by the settled clock speed)
- Volume taper and which encoder drives it (see "Volume control" above — the *where* is decided, the *how it's driven* isn't)

## Settings / menu system

**PLACEHOLDER** — not described in the architecture yet. Once the settings/menu behaviour is defined in the root `CLAUDE.md`, this section should cover the corresponding firmware module(s) — e.g. whether there's a dedicated `Settings`/`Menu` module, or whether it lives inside the UI layer alongside `ui.c`.

The interaction pattern prototyped so far: rotary encoder navigation → scroll cells → click to open popup → scroll to adjust → click to save. Master volume will be one of the parameters this drives (writing to the codec over I2C, not to a firmware variable).
