# CLAUDE.md — lochord_hw/

The KiCad hardware half of LoChord — schematic `lochord_hw.kicad_sch`, PCB `lochord_hw.kicad_pcb`. See the root `CLAUDE.md` for what the project is and how this relates to `firmware/`. See `../PINOUT.md` for the canonical pin/net assignments — treat it as the spec for any net you add, rename, or move.

**Migration status:** the board is being reworked from the ESP32-S3-WROOM-1 to the **WT0132P4-A1-N16R32** module, with an **ES8388 codec** and **Choc v1** switches. `PINOUT.md` reflects the target; the schematic and PCB do not yet. `PINOUT.md`'s "Migration delta" section is the work list.

## Working with the KiCad MCP server

Claude edits the schematic and PCB directly via the MCP server — no propose-and-wait step for routine changes (adding/moving footprints, wiring nets, adjusting values). Treat this the same as editing a firmware file: make the change, then say what was done.

Still open (tell me if you have a preference, otherwise I'll use judgement and flag anything that looks destructive or hard to undo):
- Whether any specific operations should always pause for explicit confirmation first (e.g. deleting a net, editing the board outline)
- Whether ERC/DRC should run after every change or only when a change is considered finished

Given the scale of the migration, one exception worth observing regardless: **the module footprint swap is not a routine change.** It replaces every net on the board. Flag before, not after.

## Project structure

- `lochord_hw.kicad_sch` / `lochord_hw.kicad_pcb` — schematic and PCB
- `fp-lib-table` points at the Kailh switch footprint library via the git submodule at `assets/kiswitch`. After a fresh clone, run `git submodule update --init` — without it, KiCad can't resolve the switch footprints
- `3d-models/` — STEP models for parts referenced by the PCB
- Gitignored, never commit: `*.bak`, `lochord_hw-backups/`, `*.lck`

## Components (see `../PINOUT.md` for pin-level detail)

- **WT0132P4-A1-N16R32** module — ESP32-P4, 16MB flash, 32MB in-package PSRAM. Castellated stamp-hole core board, 25 × 20 × 3.2mm, 82 pads across all four edges, **no thermal pad**
- 7× **Kailh Choc v1** switches, hand-soldered (not hotswap sockets) — chord buttons. See "Chord button layout" below; footprints must match it exactly
- 1× Kailh Choc v1 switch (1u) — loop control button
- 428×142 2.79" NV3007 TFT LCD — FPC connector, 8-pin SPI
- 3× ALPS EC11E15244G1 clickable rotary encoders (1 UI, 2 control)
- 1× Nintendo Switch joystick module — 5-pin, 0.5mm-pitch FPC connector
- **1× ES8388 stereo audio codec** with integrated headphone amplifier
- 1× headphone jack — output only, driven directly from the codec's `LOUT1`/`ROUT1`
- 2× LDO regulators — one 3V3 digital, one low-noise 3V3 analogue (**part numbers TBD**)
- EN + BOOT switches
- 4-pin debug header (USB-Serial-JTAG)
- 1× power-on LED

## The module

### Why it changed

The full rationale is in the root `CLAUDE.md`. The short version for hardware purposes: the S3 board was out of pins, had no debug channel once USB MIDI claimed GPIO19/20, and carried a radio the project never uses — which meant an antenna keep-out and an RF noise source next to an analogue audio stage, for nothing.

### Mechanical differences from the WROOM-1

| | ESP32-S3-WROOM-1 | WT0132P4-A1 |
|---|---|---|
| Size | 18 × 25.5mm | 25 × 20mm |
| Pads | 40 + EPAD, 3 edges | 82, all 4 edges, no thermal pad |
| Pitch | 1.27mm | ~1.0mm (`verify` against the datasheet drawing) |
| Hand-solderable | comfortably | yes, but harder |
| Supply | 3V3 | **5V** |

**No bottom thermal pad means no reflow is required** — this is hand-solderable with a fine tip and flux, which matters for a prototype. Solder all four corners first to fix position, then work along each edge. Only ~38 of the 82 pads are connected, so routing out of the footprint is far less congested than the pad count suggests.

**Leave unconnected:** MIPI-DSI (pads 2–7) and MIPI-CSI (pads 9–14). The NV3007 SPI panel is staying. MIPI-DSI would mean a much larger tablet-style panel and a full EEZ Studio rebuild at a new resolution — a product decision, not a technical requirement of the P4, and not one that's been taken.

**Bring to test pads only:** USB_DM/USB_DP (pads 16/17), the dedicated High-Speed OTG PHY. Nothing uses them — USB MIDI needs nothing beyond Full-Speed. Two test pads keep a future USB Audio Class experiment from being blocked by the PCB.

### Sourcing

The module is available on JLCPCB's assembly parts catalogue, so it can go through standard SMT placement alongside the rest of the board rather than needing separate sourcing. **Check the SKU carefully** — there are N16R16 and N16R32 variants and multiple hardware revisions (v1.3, v3.2 / N16R32X). LoChord targets **N16R32**. Confirm the revision you can actually buy matches the pad map you laid out against, since revisions can shift pin details.

## Chord button layout

The 7 chord switches form one rectangular pad, **4u wide × 3u tall**, in two rows sharing the same left and right edges. (Root `CLAUDE.md` carries the same description — keep them in step if either changes.)

**"2u" means two units _tall_, not two units wide.** The bottom keys are narrow and tall (1u × 2u, like a numpad's vertical Enter key), not wide horizontal ones. This is the detail that keeps getting rendered backwards.

- **Bottom row** — 4× keys, each 1u wide × 2u tall, directly side by side, spanning the full 4u width.
- **Top row** — 3× keys, each 1u tall, widths **1.5u – 1u – 1.5u** left to right, also spanning the full 4u width.

The two rows' vertical seams deliberately **do not align**. The top row's seams sit at 1.5u and 2.5u from the left edge — i.e. centred over the second and third bottom keys. Don't normalise this into aligned columns.

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

### Choc v1, not v2

The switches are **Kailh Choc v1** (PG1350 family). Earlier revisions of this repo specified Choc v2 (PG1353); that is superseded and any v2 footprint still in the schematic is wrong.

What this changes:

- **Different footprint entirely.** v1 and v2 have different pin positions and different plastic alignment posts — they are not drop-in compatible. Swap to the `PG1350` footprints in `assets/kiswitch`, don't try to adapt the v2 ones.
- **Lower profile.** Choc v1 is the low-profile switch; v2 is taller with an MX-compatible stem. For a pocketable instrument, v1 is the better fit and reduces total device height.
- **Much better keycap availability.** The low-profile Choc keycap ecosystem (including 1.5u and 2u sizes) is built around v1. This matters here specifically because the layout needs non-1u caps in both 1.5u and vertical 2u — a v2 board would have been fighting for caps that mostly don't exist.
- **Still hand-soldered, not hotswap.** No socket keepouts to plan around, but leave room on the reverse for solder joints and the direct per-switch GPIO routing (no matrix).

Footprint/layout implications:

- Choc spacing is nominally 18.0mm × 17.0mm (x × y) per unit, so at nominal pitch the pad is **72.0mm wide × 51.0mm tall** — **check this against whatever spacing the `assets/kiswitch` Choc v1 footprints actually use** before committing to an enclosure size.
- The 1.5u top keys need the 1.5u Choc v1 footprint variant, and the 2u bottom keys need the **vertically-oriented** 2u variant (or the horizontal one rotated 90°) — using a horizontal 2u footprint here is wrong.
- The 1u loop button is a **separate** switch outside this pad — its position relative to the pad isn't fixed yet (**placeholder**).

## Audio output stage — ES8388

**Decided and no longer a placeholder.** The audio path is an **ES8388 stereo I2S codec with integrated headphone amplifier**, controlled over I2C, driving the 3.5mm jack directly.

The ESP32-P4 has no built-in analogue DAC, so an external chip was always required — this is true of every comparable MCU, not a P4 shortcoming.

### What was chosen and why

The alternative was a discrete I2S DAC (PCM5102A-class) feeding a separate headphone amp (TPA6132A2-class). The ES8388 wins on:

- **Part count and board area** — one IC plus passives instead of two ICs plus two sets of passives, and no separate amplifier stage to lay out.
- **Hardware volume.** The ES8388's DAC volume registers are the control point for master volume. This is the deciding factor: the codec was chosen partly *for* this, and firmware does no software attenuation. See "Volume control" below.
- **Pop suppression** is largely handled internally, provided the I2C power-up/down sequence is correct.
- **Stereo.** Worth stating explicitly because the ES8311 — the ESP32-ecosystem codec with the most first-party reference material — is **mono**, and is therefore wrong for this project despite being the more commonly-seen part. Don't substitute it.

### What it costs

- **An I2C bus and a reset line** the board wouldn't otherwise need. Cheap on the P4, which has spare pins; it would have been genuinely painful on the S3.
- **A real I2C init sequence** in firmware rather than none at all — a hardware-strapped DAC needs no driver beyond feeding I2S.
- **A dead ADC.** The ES8388 brings a stereo ADC, mic preamps, and an input mixer. LoChord has no audio input, so `ADCDAT`, `LIN*`/`RIN*`, and the mic bias pin are all left unconnected, and the ADC is powered down at init. This is the honest cost of a codec over a discrete DAC, and it was accepted knowingly. **Do not wire a line-in jack "just in case"** — if input is ever wanted, it comes back as a real decision with its own pins and its own analogue layout.
- **MCLK is mandatory.** The ES8388 needs a master clock; a PCM5102A wouldn't have. That's a fourth I2S line.

### Connections

- **I2S**: MCLK, BCLK, WS/LRCLK, DOUT (P4 → `DACDAT`). `ADCDAT` unconnected.
- **I2C**: SDA/SCL with 4.7k pull-ups to the 3V3 digital rail, plus an active-low reset line.
- **`CE` pin**: strapped, sets the I2C address (0x10 or 0x11). **Placeholder — record which is chosen** so firmware isn't guessing.
- **Outputs**: `LOUT1`/`ROUT1` → AC coupling caps → 3.5mm jack. `LOUT2`/`ROUT2` (line level) unconnected.
- **Supplies**: `DVDD` from the digital 3V3 rail; `AVDD` and `PVDD` from the separate analogue rail.

### Layout requirements

- **AVDD/PVDD get their own low-noise LDO**, not a tap off the digital rail. Ferrite plus local bulk capacitance at the codec.
- **Keep the analogue section physically away** from the LCD's SPI bus and the USB connector. There's no radio to avoid any more, which removes the worst offender, but SPI at 40MHz and USB are still real noise sources.
- **Ground and plane separation** between the analogue section and the digital side, with a single deliberate tie point.
- Keep the I2S clock lines away from the joystick's analogue traces — `PINOUT.md` already separates them on the module edge; preserve that on the board.
- Lay the whole codec + coupling + jack section out as a **liftable sub-circuit**. It has already survived one MCU change; treat it as portable.

### Still open

- **Placeholder** — `CE` strapping and therefore the I2C address
- **Placeholder** — package, exact BOM line, and JLCPCB/LCSC availability (see "Design conventions")
- **Placeholder** — whether any additional mute/pop sequencing is needed beyond what the ES8388 does internally. Determine from the datasheet's recommended power-up sequence, not by guessing
- **Placeholder** — the headphone jack is wired output-only with no insertion detection. Only worth adding if a speaker gets added later that should auto-mute on insert; not needed for headphone-only output

### Volume control — decided

**Volume lives in the ES8388's DAC volume registers, driven over I2C.** Not in firmware as a software gain stage, and not via a physical potentiometer.

An earlier revision of this file recorded firmware-side gain as decided. That was never a project decision — it was an assumption that propagated through the docs, and it's superseded. Firmware must not apply its own master attenuation on top; one volume, one owner. See `firmware/CLAUDE.md`.

## Power tree

The module takes **5V** and regulates internally. This inverts the previous design, where the board made 3V3 first and fed the module a 3V3 rail. Three rails now:

| Rail | Source | Feeds |
|---|---|---|
| 5V | USB-C VBUS | Module VCC, both LDOs below |
| 3V3 digital | LDO from 5V | LCD, backlight driver, joystick, ES8388 `DVDD`, I2C pull-ups, power LED |
| 3V3 analogue | Separate low-noise LDO from 5V | ES8388 `AVDD` and `PVDD` only |

### Three things that aren't optional

1. **`LD1117S33` comes out.** It was already unsuitable for the S3's current draw and dropout, and the MCU no longer needs a board-supplied 3V3 rail at all. Both LDOs are new part selections. **Placeholder** — pick both, subject to JLCPCB Basic Parts availability. The analogue one should be an LP5907/TLV75533-class low-noise part.
2. **`ESP_LDO_VO4` (pad 32) is a trap.** The module exposes what looks like a free 3V3 source, but it's the P4's own internal LDO: 0.2A maximum, carrying digital core switching noise. Don't hang the backlight on it. Don't hang anything analogue on it. Use it for nothing.
3. **Do the USB current budget before layout, not after.** The module datasheet specifies the external supply should source ~1A. A plain USB 2.0 host port guarantees 500mA. A P4 at 360MHz with 32MB PSRAM, the LCD backlight, and the headphone amp can plausibly exceed that on transients — and this is a device whose entire purpose is being plugged into a laptop. Browning out on a USB-A port is fatal to it.

   Mitigations, cheapest first: generous bulk capacitance (≥100µF plus ceramics), clock the P4 down (MIDI plus a 428×142 LVGL UI does not need 360MHz), and read the USB-C CC resistors to detect whether the source advertises 1.5A/3.0A before unlocking full clocks and backlight brightness.

`R1`/`R2` remain the USB-C CC pull-downs. `R3` (the old EN pull-up) is **removed** — the module carries its own. `SW1` + `C5` stay.

**Placeholder** — battery presence and any additional rails aren't decided.

## Debug header

A 4-pin header carrying USB-Serial-JTAG (GPIO24/25) plus 3V3 and GND.

This is the fix for the S3 board's worst problem and shouldn't be value-engineered out. On the S3, TinyUSB claimed the only USB pins for MIDI and killed both USB-Serial-JTAG and pin-JTAG, leaving `printf`-over-CDC as the only debug channel. The P4's separate PHYs mean a live hardware debugger — breakpoints, single-stepping, memory inspection — runs concurrently with USB MIDI.

0.1" or JST-SH is fine. This is a bring-up and development connector, not a user-facing port, so it doesn't need a USB-C receptacle of its own — but it does need to be reachable with the board assembled.

## Mechanical / form factor

**PLACEHOLDER** — no constraints captured yet. Worth adding: target board/enclosure dimensions, whether you're aiming for something HiChord-sized (pocketable), connector placement constraints (USB-C position, headphone jack position, debug header accessibility), battery presence/placement if any.

Two inputs that now exist and should inform it when it's written: the chord pad is nominally 72.0 × 51.0mm at Choc spacing, and Choc v1 is lower-profile than the v2 previously specified, so total device height comes down.

## Design conventions

- **Part sourcing**: when picking any new part for assembly (ICs, passives, connectors — anything beyond the hand-soldered Choc switches, encoders, joystick, and LCD, which are placed manually), default to **JLCPCB's Basic Parts library**, not Extended. Basic parts go through standard SMT assembly; Extended parts add per-part setup fees and longer lead times. Check a candidate part's JLCPCB/LCSC library status *before* settling on it, not after.

  This applies most urgently to the **two LDOs**, which are both unselected. It applies less to the ES8388 and the module itself, which are already chosen for other reasons — but confirm both are assemblable before committing a board.

**PLACEHOLDER** — footprint/library naming, layer stack, trace/via rules, silkscreen conventions, anything else you want followed consistently. Fill in as you settle on them, or tell me now and I'll write it up.
