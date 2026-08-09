# AGENTS.md

LoChord: ESP32-S3 MIDI chord controller. Two independent git repos live side by side — commit to the right one.

## Git layout (easy to get wrong)

- Root repo tracks `firmware/` only; `lochord_hw/*` is gitignored (`.gitignore:1`), so root `git status` hides hardware changes.
- Root `git add -A` still picks up a few pre-tracked lochord_hw files (`docs/`, `LICENSE`, `3d-models/`) — avoid staging them accidentally.
- `lochord_hw/` is its own git repo — a clone of atomic14/basic-esp32s3-dev-board with all LoChord HW work uncommitted and `origin` pointing upstream. Do not push; do not commit without asking. Use `git -C lochord_hw` for hw status/log.
- `lochord_hw/assets/kiswitch` is a git submodule: `git submodule update --init` in `lochord_hw` if missing.

## Firmware (PlatformIO, ESP32-S3, Arduino)

Run everything from `firmware/`. Default env: `esp32-s3-devkitc1-n8r2`.

- Build / upload / monitor: `pio run`, `pio run -t upload`, `pio device monitor` (115200; exit with Ctrl+A then Ctrl+X)
- `pio run -t compiledb` regenerates `compile_commands.json` (gitignored) for clangd — re-run after adding libs/includes
- Host tests: `pio test -e native` (Unity). Test files sit at `test/` root, so PlatformIO discovers one suite named `*` and `-f <name>` filtering won't work until each suite gets its own `test/<name>/` subdir
- No lint/format configured

See `firmware/.github/copilot-instructions.md` for architecture + conventions (note: it's firmware-only; the env name there is `esp32-s3-devkitc1-n8r2`).

## Firmware architecture

- `src/main.cpp` wires module libraries under `lib/<Module>/src/`: Input (Button/Joystick/RotaryEncoder), Core (Clock), Chords (ChordGenerator/Theory.h), Sequencer, Midi (MidiController/MidiOutput), Display (Display/UIController)
- Headers are included by name (`<ChordGenerator.h>`) because build flags add `-Ilib` — follow this in tests too
- `lib/Chords/src/Theory.h`: `Quality` enum values are direct indexes into static chord/extension tables (`static_cast<size_t>`); reordering enums silently changes behavior. Chords use fixed-capacity `std::array<int, MAX_CHORD_NOTES>` + size, not dynamic containers
- `MidiController` owns note lifecycle: `processNoteOn` stops active notes first; `processNoteOff` is gated by `activeRoot_`
- Strum/arp are mutually exclusive (enabling one disables the other)
- Pin config (buttons/encoders/joystick/SPI LCD) lives in `include/config.h`; LVGL config in `include/lv_conf.h` (`LV_CONF_INCLUDE_SIMPLE`)

## UI (EEZ Studio generated)

- `lib/UI/src/*` (screens.c, ui.c, styles.c, ...) is generated from `firmware/lochord.eez-project` (EEZ-FOR-LVGL, LVGL 9.5). Hand-edits get overwritten; edit the project in EEZ Studio and regenerate
- Hand-written glue lives in `lib/Display/src/UIController.*` / `Display.*`; generated `lib/UI/src/actions.h` hooks into it
- `lochord.eez-project` / `lochord.eez-project-ui-state` change constantly and are checked in — don't revert them

## Hardware (lochord_hw/, KiCad 10)

- Root sheet `lochord_hw.kicad_sch` + sub-sheets: `p_encoders.kicad_sch` (real work), `essentials.kicad_sch` and `p_buttons.kicad_sch` (empty 161-byte stubs — buttons sheet not built yet)
- `lochord_hw/.env` holds kicad-mcp server env (KICAD_APP_PATH=/usr/share/kicad, KICAD_VERSION=10.0, KICAD_SEARCH_PATHS=/home/kiwi/dev/micro/, MCP_TRANSPORT=streamable-http) — load it when running kicad-mcp
- `docs/` artifacts (schematic.pdf, .step renders) are CI-generated — don't hand-edit
- `lochord_hw-backups/` and `.history/` are KiCad backup dirs — leave alone
