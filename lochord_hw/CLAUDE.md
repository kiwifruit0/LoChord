# CLAUDE.md — lochord_hw/

The KiCad hardware half of LoChord — schematic `lochord_hw.kicad_sch`, PCB `lochord_hw.kicad_pcb`. See the root `CLAUDE.md` for what the project is and how this relates to `firmware/`. See `../PINOUT.md` for the canonical pin/net assignments — treat it as the spec for any net you add, rename, or move.

**Migration status: done.** The schematic and PCB have been rebuilt from scratch against the **WT0132P4-A1-N16R32** module, with the **ES8388 codec** and **Choc v1** switches. `PINOUT.md` and the board now agree; the remaining migration work is firmware-side only. The board is a **4-layer** design on a 130 × 90mm outline taken from the Figma industrial design.

## Where the board stands

Verified state as of the ESP32-P4 rebuild:

| Check | Result |
|---|---|
| Schematic ERC (all severities) | **0 violations** |
| Board DRC (errors + warnings) | **4 warnings**, all `silk_edge_clearance` |
| Schematic ↔ board parity | **0 issues** |
| Unrouted connections | **0** |
| Signal copper on `In1.Cu` / `In2.Cu` | **none** — both planes are single unbroken regions |

The four remaining warnings are the USB-C and 3.5mm jack silkscreen outlines running to the board edge. That is correct for edge-mounted connectors — the silk is trimmed at the routed edge during fabrication — and they should not be "fixed" by shrinking the outlines.

Routing totals: 2135mm on `F.Cu`, 2225mm on `B.Cu`, 383 through vias (0.30mm drill throughout, JLCPCB standard). Minimum track 0.15mm, minimum clearance 0.152mm.

Run `./export-production.sh` to regenerate `production/` — gerbers for all four copper layers, Excellon drill files, a JLCPCB-style position file, the BOM keyed on the `LCSC` field, and schematic + fab PDFs. Those outputs are gitignored; the script is the artefact.

## Working with the KiCad MCP server

Claude edits the schematic and PCB directly via the MCP server — no propose-and-wait step for routine changes (adding/moving footprints, wiring nets, adjusting values). Treat this the same as editing a firmware file: make the change, then say what was done.

Still open (tell me if you have a preference, otherwise I'll use judgement and flag anything that looks destructive or hard to undo):
- Whether any specific operations should always pause for explicit confirmation first (e.g. deleting a net, editing the board outline)
- Whether ERC/DRC should run after every change or only when a change is considered finished

Given the scale of the migration, one exception worth observing regardless: **the module footprint swap is not a routine change.** It replaces every net on the board. Flag before, not after. (This was done once, for the S3 → P4 swap.)

### Traps found the hard way

- **Kailh `_90deg` footprints rotate the drawn keycap, not the switch.** `SW_Kailh_Choc_V1_2.00u_90deg` differs from `SW_Kailh_Choc_V1_2.00u` in exactly one respect: the `Dwgs.User` keycap outline is drawn 17 × 36mm instead of 36 × 17mm. The pads, the plate cutout and the two alignment posts are byte-identical, so the switch body still faces the standard direction. That convention comes from MX, whose cross stem is four-fold symmetric — a numpad Enter key really is an unrotated switch under a vertical cap. **A Choc v1 stem is a pair of parallel bars and is not symmetric under 90°**, so a vertical 2u Choc cap needs the *switch* turned as well. The correct construction is the plain `2.00u` footprint placed at 90°, which gives a vertical stem and a vertical keycap outline together. Using `_90deg` at 0° draws the right picture on a board whose switches would take horizontal caps — and nothing in ERC or DRC will tell you.

- **`pcbnew`'s Python bindings drop items when a session both removes and adds board objects.** Adding is reliable on its own if you set `item.thisown = False` after `board.Add(item)`; mixing `Remove()` and `Add()` in one session silently loses roughly half of what you added, and a second `LoadBoard()` in the same process returns a broken object. Do removals by editing the `.kicad_pcb` s-expressions directly, and keep additions in their own process.
- **Freerouting will route signals straight through your ground plane unless the DSN says the layer is a plane.** Exporting the board gives every copper layer `(type signal)`. Freerouting's log then cheerfully says *"Layer 'GND' has been automatically configured as a dedicated power plane"* — and routes on it anyway. The first routed result had **1.4m of signal trace cutting through `In1.Cu` and another 1.4m through `In2.Cu`**, which throws away the entire reason for going to four layers. The fix is to edit the exported `.dsn` before running the router and change those two layers to `(type power)`. Check `GetLayerName` counts per layer after every SES import; it is not obvious from a render.
- **Pour the outer-layer copper *after* routing, not before.** A board-wide `F.Cu`/`B.Cu` GND pour exports as a `(plane …)` covering the whole layer, which leaves the router nowhere to go — the first attempt sat at zero progress for eight minutes because all four layers were planes. Route on bare outer layers, then add the pours and the stitching-via grid into whatever space is left.
- **Fan the ES8388 out by hand before autorouting, not after.** At 0.45mm pitch with 0.25mm pads, the geometry is unforgiving: a trace can only leave a pad if `trace/2 + clearance ≤ 0.45 − 0.125 = 0.325mm`. A 0.25mm trace at 0.22mm clearance needs 0.345mm and **cannot escape at all** — the neighbouring pads' clearance rings meet over the pad's own centre line. Escapes are therefore 0.20mm wide, and the `Audio` netclass clearance had to come down from 0.30mm to 0.20mm for the same reason (the track stays 0.35mm once it is clear of the package). Freerouting's own fan-out stage gets most pins out but leaves two or three walled in by their neighbours' escapes, and once that has happened no amount of extra routing passes recovers it. The fix that works: pre-route a straight perpendicular stub off every signal pad before exporting the DSN, so the router inherits a fan-out it cannot destroy.
- **The MCP's schematic writer does not escape double quotes inside property values.** KiCad's `power:*` symbols have descriptions containing literal quotes (`... with name "GND", ground`), which produced an unparseable `.kicad_sch` — the file balanced fine but eeschema and `kicad-cli` both refused it with a bare "Failed to load schematic". If a schematic suddenly won't load after placing power symbols, look there first.

## Project structure

- `lochord_hw.kicad_sch` / `lochord_hw.kicad_pcb` — schematic and PCB
- `assets/lochord.kicad_sym` + `assets/lochord.pretty/` — **project-local symbols and footprints**, registered in `sym-lib-table` / `fp-lib-table` under the nickname `lochord`. Everything the stock KiCad libraries don't cover lives here: the ESP32-P4 module, the ES8388 symbol, the display and joystick FPC connectors and their mechanical outlines, the tactile switch, and a QFN-28 variant with JLCPCB-legal thermal vias
- `fp-lib-table` also points at the Kailh switch footprint library via the git submodule at `assets/kiswitch`. After a fresh clone, run `git submodule update --init` — without it, KiCad can't resolve the switch footprints
- `3d-models/` — STEP models for parts referenced by the PCB
- Gitignored, never commit: `*.bak`, `lochord_hw-backups/`, `*.lck`

### What's in `assets/lochord.pretty`

| Footprint | Why it's custom |
|---|---|
| `WT0132P4-A1` | 82-pad castellated module, no stock equivalent |
| `FPC_1x08_P0.50mm_Horizontal_ZIF` | display FPC; land pattern is the Hirose FH12-8S-0.5SH family |
| `FPC_1x05_P0.50mm_Horizontal_ZIF` | joystick FPC; same family, scaled to 5 ways |
| `Display_NV3007_2.79in_428x142` | mechanical outline only, so the panel reserves its area |
| `Joystick_NintendoSwitch_Module` | mechanical mount with the two diagonal solder tabs |
| `SW_SMD_TS-1088-AR02016_3.9x3.0mm` | tactile switch, carried over from the S3 revision |
| `QFN-28-1EP_4x4mm_P0.45mm_EP2.6x2.6mm_ThermalVias` | stock KiCad's EP is 2.4mm with 0.2mm via drills; this widens the pad to the ES8388 datasheet nominal and opens the drills to JLCPCB's standard 0.3mm |

## Components (see `../PINOUT.md` for pin-level detail)

- **WT0132P4-A1-N16R32** module (`U1`) — ESP32-P4, 16MB flash, 32MB in-package PSRAM. Castellated stamp-hole core board, 25 × 20 × 3.2mm, 82 pads across all four edges, **no thermal pad**
- 7× **Kailh Choc v1** switches, hand-soldered (not hotswap sockets) — chord buttons. See "Chord button layout" below; footprints must match it exactly
- 1× Kailh Choc v1 switch (1u) — loop control button
- 428×142 2.79" NV3007 TFT LCD — FPC connector, 8-pin SPI
- 3× ALPS EC11E15244G1 clickable rotary encoders (1 UI, 2 control)
- 1× Nintendo Switch joystick module — 5-pin, 0.5mm-pitch FPC connector
- **1× ES8388 stereo audio codec** with integrated headphone amplifier
- 1× headphone jack — output only, driven directly from the codec's `LOUT1`/`ROUT1`
- 2× LDO regulators — `U2` AMS1117-3.3 (digital) and `U3` LP5907MFX-3.3 (low-noise analogue)
- 1× **USBLC6-2SC6** (`U4`) ESD protection on the USB-C data pair — new for this revision
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
| Pitch | 1.27mm | **1.00mm** (confirmed, datasheet v2.1 Figure 9) |
| Hand-solderable | comfortably | yes, but harder |
| Supply | 3V3 | **5V** |

**No bottom thermal pad means no reflow is required** — this is hand-solderable with a fine tip and flux, which matters for a prototype. Solder all four corners first to fix position, then work along each edge. Only ~38 of the 82 pads are connected, so routing out of the footprint is far less congested than the pad count suggests.

**Leave unconnected:** MIPI-DSI (pads 2–7) and MIPI-CSI (pads 9–14). The NV3007 SPI panel is staying. MIPI-DSI would mean a much larger tablet-style panel and a full EEZ Studio rebuild at a new resolution — a product decision, not a technical requirement of the P4, and not one that's been taken.

**Bring to test pads only:** USB_DM/USB_DP (pads 16/17), the dedicated High-Speed OTG PHY. Nothing uses them — USB MIDI needs nothing beyond Full-Speed. Two test pads keep a future USB Audio Class experiment from being blocked by the PCB.

### Sourcing

**This was previously recorded as "available on JLCPCB's assembly parts catalogue" — that could not be confirmed.** The WT0132P4-A1 does not appear in the JLCPCB parts catalogue searched while building this board, so its `LCSC` field is `TBD` and it is currently a hand-solder part. Verify before assuming SMT placement. **Check the SKU carefully** — there are N16R16 and N16R32 variants and multiple hardware revisions (v1.3, v3.2 / N16R32X). LoChord targets **N16R32**. Confirm the revision you can actually buy matches the pad map you laid out against, since revisions can shift pin details.

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
- The 1.5u top keys need the 1.5u Choc v1 footprint variant. The 2u bottom keys use `SW_Kailh_Choc_V1_2.00u` **placed at 90° in the layout** — the switch body has to turn with the cap, so rotating the placement is the fix, not picking a differently-named footprint. See the `_90deg` trap below.
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
- **`CE` pin**: strapped **low through `R24` (10k to GND) → 7-bit I2C address `0x10`**. This matches ESP-IDF / ESP-ADF reference code for the LyraT, so firmware ported from there needs no address change. Tying `CE` permanently low also guarantees 2-wire mode: the datasheet selects SPI mode on a *high-to-low transition* of `CE`, which can never happen if it never goes high. Swap `R24` for a pull-up to move to `0x11`.
- **Outputs**: `LOUT1`/`ROUT1` → AC coupling caps → 3.5mm jack. `LOUT2`/`ROUT2` (line level) unconnected.
- **Supplies**: `DVDD` **and `PVDD`** from the digital 3V3 rail; **`AVDD` and `HPVDD`** from the separate analogue rail. Note the correction — `PVDD` is the *digital I/O* supply and belongs with the host's I/O rail, not on the analogue one. An earlier revision of this file had that wrong.
- **No reset line.** The ES8388 has no hardware reset pin; reset is register `0x00` bit 7 (`SCPReset`). The GPIO that an earlier revision reserved for it (GPIO19) is now a spare.

### Layout requirements

- **`AVDD`/`HPVDD` get their own low-noise LDO**, not a tap off the digital rail. Ferrite plus local bulk capacitance at the codec. *(This bullet used to say `AVDD`/`PVDD`; `PVDD` is the digital I/O supply and belongs on the digital rail — see "Connections" above.)*  — **done:** `U3` → `FB1` → `+3.3VA`, with 10µF + 100nF at each of `AVDD` and `HPVDD`.
- **Keep the analogue section physically away** from the LCD's SPI bus and the USB connector. There's no radio to avoid any more, which removes the worst offender, but SPI at 40MHz and USB are still real noise sources. — **done:** the codec block sits in the band between the two chord-key rows, ~57mm from USB-C and ~39mm from the LCD FPC.
- ~~**Ground and plane separation** between the analogue section and the digital side, with a single deliberate tie point.~~ **Superseded.** The board is 4-layer with a single unbroken ground plane; a split plane under the I2S bus would break the return path of the very signals it is meant to protect. See "Layer stack" under Design conventions for the reasoning and what replaced it.
- Keep the I2S clock lines away from the joystick's analogue traces — `PINOUT.md` already separates them on the module edge; preserve that on the board.
- Lay the whole codec + coupling + jack section out as a **liftable sub-circuit**. It has already survived one MCU change; treat it as portable.

### Package and BOM — settled

**QFN-28, 4.0 × 4.0mm, 0.45mm pitch, 2.6 × 2.6mm exposed pad** (datasheet section 9, `QFNWB4×4-28L-A`). Not LQFP — several secondary sources say LQFP-28 and they are wrong. LCSC **`C365736`** (Extended, standard assembly). Footprint is `lochord:QFN-28-1EP_4x4mm_P0.45mm_EP2.6x2.6mm_ThermalVias`.

**Output coupling is 220µF, not the 47µF the datasheet suggests.** The datasheet's typical application circuit specifies 47µF for headphone use, but into a 32Ω headphone that puts the high-pass corner at 1/(2π·32·47µ) ≈ **106Hz** — it would audibly gut the bass, and worse into 16Ω cans. 220µF puts the corner at ≈23Hz. This is also what the ESP32-LyraT and AI-Thinker A1S reference designs use, so it is the well-trodden value. The cost is two 6.3 × 5.4mm aluminium electrolytics (`C24`/`C25`, LCSC `C32514`) instead of two ceramics.

### Still open

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
| 3V3 digital | `U2` **AMS1117-3.3**, SOT-223 (LCSC `C6186`, Basic) | LCD, backlight, joystick, ES8388 `DVDD` **and `PVDD`**, I2C pull-ups, power LED |
| 3V3 analogue | `U3` **LP5907MFX-3.3**, SOT-23-5 (LCSC `C5370990`, Extended) → ferrite `FB1` (600Ω @100MHz, LCSC `C1002`) | ES8388 **`AVDD` and `HPVDD`** only |

### Three things that aren't optional

1. **`LD1117S33` came out; both LDOs are chosen.**
   - **Digital: AMS1117-3.3 in SOT-223 (`C6186`, Basic).** The old objection to the LD1117 class was dropout, and that objection dies with the 5V input: 1.7V of headroom against ~1.1V of dropout at 800mA, and the digital rail only draws ~120mA. SOT-223 also spreads the ~0.2W far better than a SOT-23-5 would, which matters because the LCD backlight is on this rail.
   - **Analogue: LP5907MFX-3.3 in SOT-23-5 (`C5370990`, Extended).** This is the LP5907/TLV75533-class low-noise part the design called for — 51µVrms output noise, 75dB PSRR at 1kHz, 300mA. It feeds only `AVDD` + `HPVDD` through `FB1`, so ~80mA peak and ~0.14W. If you want the best noise figure available, genuine TI **`C80670`** (10µVrms, 82dB) is pin-identical but thinly stocked; the design does not depend on it.
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

Now driven by the **Figma industrial design** (`hardware_design`, node `30:2`), which is the top-down view of the device drawn at **10px = 1mm**. Every user-facing part is placed at its Figma coordinate; the board origin is the outline's top-left corner.

**Board: 130.0 × 90.0mm, 5mm corner radius, 4 layers, 1.6mm.**

| Part | Centre (mm from board top-left) | Side |
|---|---|---|
| `U1` ESP32-P4 module | 16.40, 19.60 — rotated 270° | top |
| `RE1` UI encoder | 38.80, 13.20 | top |
| `RE2` control encoder 1 | 38.80, 30.45 | top |
| `RE3` control encoder 2 | 38.80, 45.95 | top |
| `M1` joystick | 16.15, 50.15 | top |
| `M2` display panel | 87.60, 16.60 | top |
| `SW3`–`SW6` chord bottom row | 59.55 / 78.25 / 96.95 / 115.65, all at y 67.95 — **each rotated 90°** | top |
| `SW7`–`SW9` chord top row | 64.30 / 87.55 / 110.80, all at y 39.95 | top |
| `SW10` loop key | 18.05, 77.45 | top |
| `J2` debug header | 46.70, 68.31 — vertical, pin 1 at y 64.50 | top |
| `J3` joystick FPC | 17.0, 64.5 | top |
| `J1` USB-C | left edge, y ≈ 66 | bottom |
| `J5` 3.5mm jack | bottom edge, x ≈ 38 | bottom |
| `J4` LCD FPC | 62.0, 15.0 — under the panel | bottom |

The **band between the two chord-key rows** — x 52…120, y 48…60 on the bottom side — is the single biggest clear area on the board, and it is where the whole codec section lives. It looks like an odd place for analogue until you notice what surrounds it: chord keys are DC switch closures, so the only things above and below the codec are quiet. The USB-C connector is ~57mm away and the LCD FPC ~39mm.

Layout inside the band, left to right: `U5` at (63, 54) with its four decoupling caps hugging the supply pins it feeds (`C13`/`C14` left for `DVDD`/`PVDD`, `C16`/`C18` right for `AVDD`/`HPVDD`), then the reference caps (`C19`–`C23`), the bulk caps, the four I2S series resistors and the I2C pull-ups spreading rightwards on a 3–4mm pitch. **Do not pack this tighter.** An earlier revision squeezed the same parts into x 52…92 at 0.7mm spacing and the result was unroutable — the codec's supply pins ended up walled in behind their own decoupling.

Everything else — power tree, codec, decoupling, pull-ups — is on the **bottom** side, in the four large areas the top side leaves free: under the module, under the joystick, under the display, and the band between the two chord-key rows.

### Two things the industrial design didn't settle, and how they were resolved

1. **There is no room on any board edge for connectors.** The Figma layout leaves only 4–6mm of margin all round, and both a USB-C receptacle (9.0 × 7.4mm) and a 3.5mm jack (13.9 × 5.9mm, insertion axis perpendicular to the edge) need more than that. The two places they *do* fit are the gap on the **left edge between the joystick and the loop key** (USB-C) and the **bottom edge in the open area left of the chord pad** (jack). Both are on the bottom side so the top face stays as drawn. **If the industrial design wants these ports somewhere specific, that's a layout change, and it will need margin made for it.**

2. **The display's FPC tail has nowhere to go on the top side.** The channel between the encoder column and the panel is 5.7mm wide and the connector needs 6.05mm. The panel is therefore fed from **underneath**: `J4` sits on the bottom side under the panel and a **16 × 2.5mm slot is cut in the board at x 54–70, y 19.75–22.25** for the tail to fold through. **The slot position is a guess at where the tail exits the panel and must be matched to the panel actually purchased.**

### Still missing: mounting

**There are no mounting holes on this board, and that is a deliberate gap, not an oversight.** The Figma industrial design doesn't show any, and there's no enclosure spec to place them against — inventing four holes would just be a guess that later has to be undone.

Three of the four corners are clear and would take an M2 hole today: **(126, 4)**, **(126, 86)** and **(4, 86)** in board coordinates. The top-left corner is not — the module's courtyard reaches to (4.7, 5.4), so a fourth hole there needs the module nudged or a smaller screw. Decide the enclosure first, then place them.

### Why the module is rotated 270°

`PINOUT.md` groups each peripheral onto a contiguous run of pads on one module edge. Rotating the module 270° (clockwise 90°) points those runs at the right parts of the board:

| Module edge | Faces | Carries |
|---|---|---|
| top (pads 42–64) | board **right** | 7 chord keys, loop key, all 6 LCD signals — 14 nets, the biggest group, and the one where SPI length actually matters |
| bottom (pads 1–23) | board **left** | USB D±, USB-Serial-JTAG D±, UART0 — the USB-C is on the left edge, so this is a short, clean run |
| left (pads 65–82) | board **top** | I2S, I2C, joystick |
| right (pads 24–41) | board **bottom** | the three encoders, plus VCC |

**The known cost:** the joystick sits *below* the module but its three signals leave from the module's board-top edge, and the codec sits at the bottom-middle while I2S also leaves from the top. Both groups take the long way round. That was accepted deliberately — there is only one rotation that puts the 14-net chord + LCD group on the edge facing them, and short SPI matters more than short joystick wiring. Both long runs are over an unbroken ground plane, and the I2S lines carry 33Ω series damping.

### Deliberate deviations from the Figma geometry

- The two control encoders are drawn 15.0mm apart; the EC11 courtyard is 15.49mm tall. They were opened to **15.5mm** (0.25mm each) so the courtyards clear. Invisible in use.
- **Encoder knobs must be ≤15mm across.** The two control encoders sit on a 15.5mm pitch, so anything wider fouls its neighbour. The Figma draws them as 10mm circles (and the UI encoder as 15mm), which is comfortably inside that; just don't fit 16mm knobs later without revisiting the spacing.
- Key *pitch* follows the Figma (18.7mm horizontally, 28.0mm between rows) rather than Choc's nominal 18.0 × 17.0mm. The design deliberately shows gaps between keys; keycaps (17 × 36mm for the vertical 2u, 27 × 17mm for the 1.5u) clear each other by 0.75–1.7mm everywhere.

## Before you order a board

Three things in this design are **assumptions, not verified facts**, and all three are cheap to check and expensive to get wrong. Check them against the parts you actually buy.

### 1. The display's 8-way FPC pin order

`J4` is wired `1 GND, 2 VCC, 3 SCL, 4 SDA, 5 RES, 6 DC, 7 CS, 8 BLK` — the near-universal order printed on 8-pin SPI TFT breakouts. **It could not be confirmed for this specific 2.79in 428×142 NV3007 panel**; no vendor publishes a pin table, and the panel has no settled part number in this repo. If the panel you buy differs, the board is wrong and needs a respin. Get the panel's own drawing first.

Related and equally unverified: **where the tail exits the panel**, which is what the 16 × 2.5mm slot at x 54–70, y 19.75–22.25 assumes.

### 2. The display backlight drive

`BLK` on these modules is usually a *logic* input feeding an on-panel transistor, but on some it is the LED cathode. The board supports both and you populate one:

| Option | Fit | Leave off |
|---|---|---|
| `BLK` is a logic/PWM input **(default)** | `R15` (0Ω) | `Q1`, `R17` |
| `BLK` is the LED cathode | `Q1` (SI2302), `R17` (100Ω) | `R15` |

`R16` (100k to GND) is fitted either way so the backlight is dark until firmware drives it. **Never populate `R15` and `Q1` together** — that shorts GPIO6 to ground through the MOSFET.

### 3. The joystick's 5-way FPC pin order

`J3` is wired `1 GND, 2 VCC, 3 X, 4 Y, 5 SW`. The Nintendo Switch stick module's pinout is not publicly documented and this could not be confirmed. The risk is contained by design: the three signal pins land on GPIO16/17/18, which are **all ADC1 channels and all usable as digital inputs**, so a wrong signal order is a firmware constant change, not a respin. Only a VCC/GND swap would need rework, and even that cannot damage anything — the axes are a resistive divider either way.

### And one sourcing question

**The ESP32-P4 module has no LCSC part number in this design.** This file previously claimed the WT0132P4-A1 is in JLCPCB's assembly catalogue; it does not appear in the JLCPCB parts catalogue searched during this work. Its `LCSC` field is set to `TBD`. Either confirm a JLCPCB SKU (checking the **N16R32** variant and hardware revision, per "Sourcing" above) or plan to hand-solder it — which the castellated, no-thermal-pad package supports.

Everything else on the board has a checked LCSC number in its `LCSC` field, Basic where one exists.

## Design conventions

- **Part sourcing**: when picking any new part for assembly (ICs, passives, connectors — anything beyond the hand-soldered Choc switches, encoders, joystick, and LCD, which are placed manually), default to **JLCPCB's Basic Parts library**, not Extended. Basic parts go through standard SMT assembly; Extended parts add per-part setup fees and longer lead times. Check a candidate part's JLCPCB/LCSC library status *before* settling on it, not after.

  This applies most urgently to the **two LDOs**, which are both unselected. It applies less to the ES8388 and the module itself, which are already chosen for other reasons — but confirm both are assemblable before committing a board.

### Layer stack — 4 layers

| Layer | Use |
|---|---|
| `F.Cu` | signal, plus a GND pour in the leftover copper |
| `In1.Cu` (named **GND**) | **solid ground plane**, unbroken except for the FPC tail slot |
| `In2.Cu` (named **PWR**) | **solid +3V3 plane** |
| `B.Cu` | signal, plus a GND pour in the leftover copper |

**All signal routing lives on `F.Cu` and `B.Cu`. The two inner layers carry no signals at all** — that is the whole point, and it is worth re-checking after any autoroute (see "Traps found the hard way").

The S3 revision was 2-layer. Four layers is what makes the analogue section defensible on a board that also carries a 40MHz SPI bus and USB: every signal has an unbroken return path directly beneath it, and the GND/+3V3 plane pair adds useful interplane capacitance.

**Deliberate deviation from earlier guidance in this file.** The "Layout requirements" section above asks for *ground and plane separation between the analogue section and the digital side, with a single deliberate tie point*. That is not what was built, and it should not be: a split plane under the I2S bus breaks the return path of exactly the signals it is supposed to protect, and moat-and-bridge grounding is a 2-layer-era technique. What replaces it is a **single unbroken ground plane plus physical separation** — the codec sits at board (63, 54) in the clear band between the two chord-key rows, ~57mm from the USB-C connector and ~39mm from the LCD FPC, on its own LDO behind a ferrite. Its decoupling is split left and right so each supply pin escapes towards its own capacitor rather than fighting the I2S fan-out for the same corridor. If a future revision measures a real noise problem, the fix is placement or supply filtering, not cutting the plane.

### Trace, via and clearance rules

Sized for JLCPCB's standard 4-layer process with margin, not for its limits.

| Netclass | Track | Clearance | Via |
|---|---|---|---|
| `Default` | 0.20mm | 0.15mm | 0.60 / 0.30mm |
| `Power` — `GND`, `+5V`, `+3V3`, `+3.3VA`, `VA_RAW` | 0.60mm | 0.18mm | 0.80 / 0.40mm |
| `USB` — USB and USB-Serial-JTAG pairs | 0.25mm | 0.25mm | 0.60 / 0.30mm |
| `Audio` — `HP_*`, codec references | 0.35mm | 0.30mm | 0.60 / 0.30mm |

| Board constraint | Value |
|---|---|
| Minimum track | 0.127mm (JLCPCB allows 0.09mm) |
| Minimum via | 0.45mm pad / 0.25mm drill |
| Hole to hole | 0.50mm |
| Copper to board edge | 0.30mm |

`Default` clearance is 0.15mm rather than 0.20mm. JLCPCB's floor is 0.09mm, so 0.15mm still has a comfortable margin, and the extra 0.05mm is what lets the autorouter get through the congested band around the codec and the chord-key field. Don't take it lower without a reason.

**Never specify a via drill below 0.30mm** unless there is a real reason — JLCPCB surcharges 0.20mm drills, which is why the stock KiCad QFN-28 thermal-via footprint had to be replaced.

### Netclasses

`Default`, `Power`, `USB`, `Audio`, assigned by pattern in `lochord_hw.kicad_pro`. Adding a net that should be wide or well-separated means adding it to a pattern, not hand-tuning the trace.

### Silkscreen

- Reference designators only; **values are hidden on the board**. The BOM and the position file carry values, and 90 parts' worth of value text makes the silk unreadable.
- Reference text 0.8mm high, 0.12mm thick.
- Silk must not cross a pad. The project footprints break their outlines where pads intrude rather than drawing through them.

### Fields on every part

- **`LCSC`** — the JLCPCB/LCSC part number. This is the field the BOM is generated from; keep using it.
- Parts that are **hand-soldered** (`SW3`–`SW10`, `RE1`–`RE3`, `M1`, `M2`, `J2`) are marked *exclude from position files* so JLCPCB assembly doesn't try to place them.
- Parts that are **fitted but not populated** (`Q1`, `R17`, `C27`–`C32`) carry `dnp` + *exclude from BOM*.
