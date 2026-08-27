# PINOUT.md — LoChord signal map

Single source of truth for every net that crosses the firmware ↔ hardware boundary. `firmware/include/config.h` and the `lochord_hw` schematic must both match this file — if either disagrees with it, that side is out of date, not this file.

**Status:** this file has been rewritten for the **ESP32-P4** migration. It supersedes the previous ESP32-S3-WROOM-1 assignment entirely — every GPIO number on the board changes. Neither `firmware/include/config.h` nor the schematic matches it yet; see "Migration delta" at the bottom for what has to change on each side.

**Verification status.** Every pad number, GPIO number, and alternate function in this file has been checked line by line against **WT0132P4-A1 datasheet v2.3** (section 3.2 pin description, Figure 7 pin layout, Figure 9 dimensions) and against the **ESP32-P4 chip revision v1.3 datasheet** for the peripheral-level claims (strapping, ADC channels, SPI2 IO_MUX, USB PHYs). There are no `verify` rows left. Re-run the check if the module datasheet revs past v2.3.

One thing that check settled, because it is stated wrongly in several places online: **the module breaks out all 55 of the ESP32-P4's GPIOs.** Counting the pin table — GPIO0–1 (pads 71–72), GPIO2–15 (55–69), GPIO16–23 (74–81), GPIO24–27 (19, 20, 22, 23), GPIO28–43 (24–31, 33–40), GPIO44–54 (43–52, 54) — gives exactly 55. The "44 GPIO" figure that appears on vendor product pages is wrong; the datasheet's own feature list says "ESP32-P4 chip with full pinout". Nothing in the spares list below is contingent on a smaller set.

## Module

Target module is the **WT0132P4-A1-N16R32** — Wireless-Tag's castellated ESP32-P4 core board: 16MB flash, 32MB PSRAM stacked in-package, 25.00 × 20.00mm, 3.3mm tall (0.8mm carrier PCB + 2.5mm shield can), 82 pads across all four edges, no thermal pad.

Footprint numbers from Figure 9, for the KiCad footprint: **1.00mm pad pitch**, pad 0.56mm wide × 1.80mm long, first pad centre 1.22mm in from the corner on both the bottom edge and the left edge. Supply is **4.8–5.5V on pad 41 with 1A typical draw** (section 4.3) — see "Power".

Two consequences worth stating up front, because they remove constraints the S3 design was built around:

- **No radio.** The ESP32-P4 has no Wi-Fi or Bluetooth silicon at all. There is no antenna keep-out to plan around, no RF noise source adjacent to the analogue audio stage, and no ADC2-is-unusable-when-Wi-Fi-is-active caveat. This was the deciding reason for the migration.
- **No external PSRAM pin conflict.** PSRAM is stacked inside the module package, not wired over GPIO. The entire quad-vs-octal PSRAM problem that shaped the S3 pin map — and the "chord buttons 1–3 must avoid IO35/36/37" constraint that came with it — no longer exists. Nothing in this file is contingent on a PSRAM variant.

### Pad numbering

Pads run anticlockwise from the bottom-left corner (confirmed against Figure 7):

- **Bottom edge**, left→right: 1–23
- **Right edge**, bottom→top: 24–41
- **Top edge**, right→left: 42–64
- **Left edge**, top→bottom: 65–82

Assignments are grouped so each peripheral lands on a contiguous run of pads on the module edge facing it, given the device layout (module at left; joystick and loop key below it; encoder column to its right; display and chord pad far right).

## Reserved / not freely assignable

| Signal | GPIO | Pad | Why |
|---|---|---|---|
| USB D− (MIDI) | 26 | 22 | USB 2.0 Full-Speed OTG. Fixed by the package |
| USB D+ (MIDI) | 27 | 23 | As above |
| USB-Serial-JTAG D− | 24 | 19 | Console + hardware debugger. See "Debug" below |
| USB-Serial-JTAG D+ | 25 | 20 | As above |
| USB HS D−/D+ | — | 16 / 17 | Dedicated High-Speed OTG PHY pads. **Bring to test pads only, otherwise unconnected** — see "Debug" |
| BOOT | 35 | 31 | Strapping. Switch to GND. Do not use for anything else |
| Boot-mode select | 36 | 33 | Strapping. Permanent 10k pull-up to 3V3, nothing else on it |
| EN / reset | CHIP_PU | 70 | Module carries its own 10k pull-up |
| UART0 TXD | 37 | 34 | Strapping. Reserved — bootloader download port |
| UART0 RXD | 38 | 35 | Strapping. Reserved — bootloader download port |
| VCC | — | 41 | **5V in, 4.8–5.5V.** Not 3V3 — see "Power" |
| MIPI-DSI | — | 2–7 | Unconnected. Keeping the SPI NV3007 panel |
| MIPI-CSI | — | 9–14 | Unconnected. No camera |
| ESP_LDO_VO4 | — | 32 | **Do not use.** See "Power" |

### Strapping pins — the porting trap

The P4's strapping set is **GPIO34, 35, 36, 37, 38** — completely different from the S3's 0/3/45/46. Nothing carried over.

- **GPIO35 = 0 and GPIO36 = 1 together select Joint Download Boot.** So: BOOT switch to GND on 35, permanent pull-up on 36. Neither is available for anything else.
- **The pull-up on GPIO36 is mandatory, not belt-and-braces.** The datasheet's default-strapping table gives GPIO35 a weak *internal* pull-up (reads 1, hence SPI Boot by default) but leaves **GPIO36, 37 and 38 floating**. With GPIO36 floating there is no guaranteed way into download mode, so the external 10k is what makes the BOOT switch work at all.
- **The GPIO36 pull-up is safe.** GPIO36 doubles as the ROM-message-printing strap, but that only takes effect once `EFUSE_UART_PRINT_CONTROL` is burnt; at its default value of 0 the pin is ignored for print control and ROM output goes to both UART0 and USB-Serial-JTAG regardless.
- **GPIO34 is JTAG source select and floats at reset.** Its strapping value is likewise only consulted once `EFUSE_JTAG_SEL_ENABLE` is burnt, which it is not by default — so a pull-up on it is harmless today. Treat it as a plain GPIO but avoid a permanent pull-up anyway, so that burning that eFuse later isn't a board respin.
- **GPIO0 is free on the P4** (it's XTAL_32K_N, not a strapping pin). This is the opposite of the S3, where GPIO0 was BOOT and had to be kept clear. It is used below as the codec I2C data line.

## Chord buttons (7×, Kailh Choc v1, hand-soldered)

One GPIO per button, direct (not matrix-scanned). Switch to GND, **internal** pull-up (`INPUT_PULLUP`) — no external resistors.

Placed on pads 43–49: seven consecutive top-edge pads, the module edge that faces the chord pad.

```
 ┌───────────┬───────┬───────────┐
 │   1.5u    │  1u   │   1.5u    │   top row: 3 keys, 1u tall
 ├───────┬───┴───┬───┴───┬───────┤
 │       │       │       │       │
 │  2u   │  2u   │  2u   │  2u   │   bottom row: 4 keys, 1u wide × 2u tall
 │       │       │       │       │
 └───────┴───────┴───────┴───────┘
```

| Button | Position | Key size | GPIO | Pad | Sch. ref |
|---|---|---|---|---|---|
| 1 | Bottom row, 1st from left | 1u w × 2u h | 44 | 43 | SW6 |
| 2 | Bottom row, 2nd from left | 1u w × 2u h | 45 | 44 | SW7 |
| 3 | Bottom row, 3rd from left | 1u w × 2u h | 46 | 45 | SW9 |
| 4 | Bottom row, 4th from left | 1u w × 2u h | 47 | 46 | SW8 |
| 5 | Top row, left | 1.5u w × 1u h | 48 | 47 | SW10 |
| 6 | Top row, centre | 1u w × 1u h | 49 | 48 | SW11 |
| 7 | Top row, right | 1.5u w × 1u h | 50 | 49 | SW12 |

Ascending GPIO order runs bottom-left → bottom-right, then top-left → top-right, so `buttonPins[]` index order and physical position match by construction. **Wire the switches to match this table** — it is a specification, not an assumption.

Whether the *musical* degree order should differ from this physical order is still open (the HiChord interleaves odd degrees on the bottom row and even on the top). That is a firmware mapping decision on top of this table, not a wiring one — record it here once decided.

GPIO49/50 consume **ADC2_CH0/CH1**. That is free of consequence on the P4: there is no radio to block ADC2, and the joystick uses ADC1 anyway.

Note for anyone who later wants ADC2: **the module datasheet's ADC2 channel numbers are wrong.** It labels GPIO49–54 as `ADC2_CHANNEL2`–`ADC2_CHANNEL7`; the ESP32-P4 chip datasheet (Table 2-7, Analog Functions) labels the same pins `ADC2_CHANNEL0`–`ADC2_CHANNEL5`. The chip datasheet is authoritative and is what ESP-IDF's channel enums follow. The pin *identities* are the same either way, so nothing on this board is affected — only the channel constant you would pass to the ADC driver.

## Loop control button

| Signal | GPIO | Pad | Notes |
|---|---|---|---|
| Loop button (1u, Choc v1) | 51 | 50 | Sch. ref SW14. Drives record/play/overdub/stop |

Continues the chord-button run, so all 8 keys sit on 8 adjacent top-edge pads. Not a strapping pin — a plain switch-to-GND with an internal pull-up, no special handling. (The S3 design had this on IO46 with a long warning attached about its reset-time pull-down; that warning does not carry over and should not be copied forward.)

## Rotary encoders (3×, ALPS EC11E15244G1, clickable)

All three on the right edge, matching the mockup's encoder column. Each of the 9 signals carries an external pull-up (R6–R14); the A/B pins additionally want RC debouncing if contact bounce proves troublesome.

| Encoder | Role | Pin A | Pin B | Switch | Pads | Sch. ref |
|---|---|---|---|---|---|---|
| 1 | **UI navigation** (menu/parameter select) | 39 | 40 | 41 | 36, 37, 38 | SW15 |
| 2 | **Control 1** (e.g. BPM) | 28 | 29 | 30 | 24, 25, 26 | SW3 |
| 3 | **Control 2** (e.g. strum amount) | 31 | 32 | 33 | 27, 28, 29 | SW13 |

None of these are strapping pins, so unlike the S3 map there is no "do not put an encoder here" exclusion to observe. The external pull-ups are safe on all nine.

The encoders occupy two runs on the right edge, not one: pads 24–29 (encoders 2 and 3) and pads 36–38 (encoder 1). **Pads 30–35 sit between them and are all spoken for** — GPIO34, the BOOT strap (35), `ESP_LDO_VO4` (32), the boot-mode strap (36), and the UART0 download pair (37/38). So the BOOT switch, the boot-select pull-up, and the UART download pads all land in the middle of the encoder column. That is the best available arrangement — those six pads cannot move — but it is worth knowing before placing parts, because those support components have to be tucked around the encoder wiring rather than grouped elsewhere.

GPIO28–33 are also the P4's *alternate* SPI2 mapping (`SPI2 CS/D/CK/Q/HOLD/WP`). Nothing on this board uses that mapping — the display is on the IO_MUX set, GPIO7–11 — so there is no conflict, but don't let the datasheet's pin-function text suggest these pads are reserved.

Encoder debouncing strategy (RC vs firmware-only vs both) is still open — record the decision here once made, since fitting RC changes the schematic.

## Joystick (Nintendo Switch module, 5-pin 0.5mm FPC)

| Signal | GPIO | Pad | Notes |
|---|---|---|---|
| VCC | — | — | 3V3 digital rail |
| GND | — | — | |
| X axis (LR) | 16 | 74 | **ADC1_CH0** |
| Y axis (UD) | 17 | 75 | **ADC1_CH1** |
| Click / SW | 18 | 76 | Switch to GND, internal pull-up |

Both axes on ADC1. On the S3 this was a hard requirement (ADC2 is unusable while the radio is active); on the P4 it is simply good practice — ADC1 is the better-characterised block and keeping both axes on one ADC keeps calibration consistent.

The three signals sit on consecutive left-edge pads (74, 75, 76), so the 5-wire FPC leaves as one short bundle. Pad 73 is GND, which separates them from the codec I2C pair immediately above — and the I2S clock lines are two pads further up still, with the slow I2C pair sitting between them and the analogue joystick traces. That ordering is deliberate: the fastest audio signals end up furthest from the ADC inputs.

## LCD (428×142 2.79" NV3007, 8-pin SPI FPC)

| Signal | GPIO | Pad | Notes |
|---|---|---|---|
| BL (backlight) | 6 | 59 | LEDC PWM, keep >25kHz. **Not** a direct LED drive |
| CS | 7 | 60 | SPI2 `CS_PAD` |
| MOSI | 8 | 61 | SPI2 `D_PAD` |
| SCLK | 9 | 62 | SPI2 `CK_PAD` |
| DC | 10 | 63 | SPI2 `Q_PAD` — that is the MISO pad, unused here, so it is free for DC |
| RST | 11 | 65 | SPI2 `WP_PAD`, unused in 1-bit mode. Pad 64 is GND — good SPI return path mid-bus |
| MISO | — | — | `PIN_LCD_MISO = -1` — panel needs no readback |
| VCC / GND | — | — | 3V3 digital rail |

**Confirmed:** GPIO7/8/9 really are SPI2's IO_MUX `CS_PAD`/`D_PAD`/`CK_PAD`, so the bus bypasses the GPIO matrix and keeps its clock headroom. The module datasheet lists a second SPI2 mapping on GPIO28–35 (`SPI2 CS/D/CK/Q/HOLD/WP`), which is the set the encoders occupy — do not confuse the two; the encoders are on the *alternate* SPI2 mapping and the display is on the IO_MUX one, and only the display's placement matters for speed.

The whole display block now sits on one contiguous run, pads 59–63 plus 65, with only the GND at pad 64 breaking it.

Layout notes unchanged from the S3 design: keep SCLK and MOSI short and referenced to solid ground, and fit a series resistor (≈22Ω) on SCLK at the module end to damp ringing. The backlight is an LED string needing more current and voltage headroom than a GPIO can supply — GPIO12 drives a transistor or a constant-current/boost driver's enable-or-PWM input, never the LEDs directly.

The P4's PPA (pixel processing accelerator) and 2D-DMA can accelerate LVGL blits, and 32MB PSRAM makes full double-buffering trivial (428 × 142 × 2 = 121KB). Neither changes the pin map; both are noted so the firmware side knows they are available.

## Audio output — ES8388 codec

**Decided.** The audio path is an **ES8388 stereo I2S codec with integrated headphone amplifier**, controlled over I2C. This is no longer a placeholder section: the chip is chosen, volume lives in the codec's own registers, and the ADC half is deliberately left unconnected.

| Signal | GPIO | Pad | Notes |
|---|---|---|---|
| I2S MCLK | 12 | 66 | **Required.** The ES8388 needs a master clock — this is not optional as it would have been for a PCM5102A-class DAC |
| I2S DOUT | 13 | 67 | P4 → ES8388 `DACDAT` |
| I2S BCLK | 14 | 68 | Bit clock |
| I2S WS / LRCLK | 15 | 69 | Word select / left-right clock |
| I2S DIN | — | — | **Not fitted.** ES8388 `ADCDAT` left unconnected — see below |
| Codec I2C SDA | 0 | 71 | 4.7k pull-up to 3V3 |
| Codec I2C SCL | 1 | 72 | 4.7k pull-up to 3V3 |
| Codec RESET | 19 | 77 | Active-low. Static line — placement is not critical |
| Codec CE (I2C address select) | — | — | Strapped, not a GPIO. Tie to set address 0x10 or 0x11 — record which once chosen |

**The ADC is deliberately unused.** The ES8388 is a full codec and brings a stereo ADC, mic preamps, and an input mixer. LoChord has no audio input path, so `ADCDAT`, `LIN*`/`RIN*`, and the mic bias pin are all left unconnected, and the ADC is powered down during I2C init rather than left running. This is a known cost of choosing a codec over a discrete DAC — it buys the integrated headphone amp and hardware volume, and the input half is dead weight. Do not wire a line-in jack "just in case"; if that changes, it comes back here as a real decision with its own pins and its own analogue layout.

**Volume lives in the codec, not in firmware.** The ES8388's DAC volume registers are the single control point for master volume, driven over I2C from the UI. Firmware must not also apply a software gain stage during mixing — one volume, one owner. See `firmware/CLAUDE.md`.

**None of the I2S signals have an IO_MUX pad on the P4** — the module's pin table lists no I2S function on any pad, so every I2S line including MCLK is routed through the GPIO matrix and can sit on any GPIO. MCLK on GPIO12 is therefore no worse electrically than MCLK on GPIO0, and it buys a fully contiguous four-wire I2S bundle: **pads 66, 67, 68, 69, in that order**, with no other signal interleaved.

The I2C pair is on pads 71 and 72 — physically adjacent, on the far side of CHIP_PU (pad 70) from the I2S bundle. Keeping SDA and SCL adjacent matters more than keeping them next to the I2S lines: they share a pull-up pair and want to run as a couple.

Analogue supply and layout requirements are in `lochord_hw/CLAUDE.md`.

## Headphone jack (output only)

| Signal | Notes |
|---|---|
| L out | ES8388 `LOUT1`, AC-coupled |
| R out | ES8388 `ROUT1`, AC-coupled |
| GND | |
| Detect | Not fitted. Only worth adding if a speaker is added later that should auto-mute on insert |

`LOUT1`/`ROUT1` are the ES8388's headphone-amp outputs and drive the jack directly — there is no separate amplifier IC on this board. `LOUT2`/`ROUT2` (line level) are unconnected.

Recording into a computer is over an aux cable into an audio interface, not USB. USB Audio Class is **not** implemented and is not planned for the first board — the analogue chain above is identical whether or not it is added later as a firmware-only change.

## Debug

**Decided:** a dedicated debug header, not just test pads.

| Signal | GPIO | Pad | Notes |
|---|---|---|---|
| USB-Serial-JTAG D− | 24 | 19 | |
| USB-Serial-JTAG D+ | 25 | 20 | |
| 3V3 | — | — | Header pin |
| GND | — | — | Header pin |

This is the single biggest practical win of the P4 migration and the fix for the S3 board's worst problem. On the S3, TinyUSB claimed GPIO19/20 for USB MIDI and you lost USB-Serial-JTAG and pin-JTAG simultaneously, leaving `printf`-over-CDC as the only debug channel. On the P4 these are **physically separate PHYs**: USB MIDI runs on GPIO26/27 while a live JTAG debugger and serial console run concurrently on GPIO24/25. Breakpoints, single-stepping, and memory inspection are available with MIDI active.

A 4-pin 0.1" (or JST-SH) header is enough — this is a bring-up and development connector, not a user-facing port, so it does not need a USB-C receptacle of its own.

The dedicated High-Speed OTG pads (16/17) go to **test pads only**. Nothing uses them: USB MIDI needs nothing beyond Full-Speed. They exist so that a future USB Audio Class experiment is not blocked by the PCB, at the cost of two test pads.

Pin-JTAG (MTCK/MTDI/MTMS/MTDO) on GPIO2–5 is *not* used — USB-Serial-JTAG makes an external probe unnecessary. All four are nonetheless left **completely free** (pads 55–58) rather than assigned to something else, so the option stays open at zero cost. An earlier draft of this file put the codec I2C and reset on GPIO2/3 and closed it off; that was a side effect of a pad assignment that has since been corrected, not a decision.

## System

| Signal | GPIO | Pad | Notes |
|---|---|---|---|
| EN / reset | CHIP_PU | 70 | `SW1` + `C5`. Module has its own 10k pull-up — no external `R3` needed |
| BOOT | 35 | 31 | `SW2`, switch to GND |
| Boot-mode select | 36 | 33 | `R4` 10k to 3V3, permanent |
| Power LED | — | — | `D3` + `R5`, wired to the 3V3 rail. No GPIO |
| USB D+/D− (MIDI) | 27 / 26 | 23 / 22 | Fixed by the package |

## Power

The module takes **5V** and regulates internally. This inverts the S3 design, where the board regulated to 3V3 first and fed the module a 3V3 rail.

| Rail | Source | Feeds |
|---|---|---|
| 5V | USB-C VBUS | Module VCC (pad 41), both LDOs below |
| 3V3 digital | LDO from 5V | LCD, backlight driver, joystick, ES8388 DVDD, I2C pull-ups, power LED |
| 3V3 analogue | **Separate low-noise LDO** from 5V | ES8388 AVDD and PVDD only |

Three things that are not optional:

1. **`LD1117S33` comes out.** It was already the wrong part for the S3's current draw and dropout, and the MCU no longer needs a 3V3 rail from the board at all. Both replacement LDOs are new part selections — see `lochord_hw/CLAUDE.md`.
2. **`ESP_LDO_VO4` (pad 32) is a trap.** The module exposes it and it looks like free 3V3 sitting right there, but it is the P4's own internal LDO: 0.2A maximum, and it carries digital core switching noise. Do not hang the backlight on it. Do not hang anything analogue on it. Use it for nothing.
3. **The USB current budget must be done before layout, not after.** The module datasheet specifies the external supply should source ~1A. A plain USB 2.0 host port guarantees 500mA. A P4 at 360MHz with 32MB PSRAM, the LCD backlight, and the headphone amp can plausibly exceed that on transients — and this is a device whose whole purpose is being plugged into a laptop. Mitigations, cheapest first: generous bulk capacitance (≥100µF plus ceramics), clock the P4 down (MIDI plus a 428×142 LVGL UI does not need 360MHz), and read the USB-C CC resistors to detect whether the source advertises 1.5A/3.0A before unlocking full clocks and backlight brightness.

`R1`/`R2` remain the USB-C CC pull-downs. Battery presence is still undecided.

## Spares

Everything not assigned above. The pin squeeze that drove this migration is gone: the S3 map had 4 spares against 32 assigned pins, two of them barely usable.

| GPIO | Pad | Suitability |
|---|---|---|
| 2, 3, 4, 5 | 55–58 | Unrestricted. These four are the complete pin-JTAG set (MTCK/MTDI/MTMS/MTDO) — kept free so that option survives |
| 20, 21, 22, 23 | 78–81 | Unrestricted. All four are ADC1 channels (CH4–CH7), so this is where a second analogue control would go |
| 34 | 30 | JTAG source select, floats at reset. Fine as an input or output — **prefer not to fit a permanent pull-up** |
| 42, 43 | 39, 40 | Unrestricted |
| 52, 53, 54 | 51, 52, 54 | Unrestricted. Confirmed broken out; also ADC2_CH3/CH4/CH5 and the analogue comparator inputs |

That is **14** genuinely free GPIOs after everything is assigned, against 4 on the S3. Consequences:

- **The I2C expander evaluation is closed.** TCA9555/PCF8575 was only ever a workaround for S3 pin starvation. All 8 keys are on direct GPIO with internal pull-ups and stay there.
- **The encoder-switch resistor-ladder workaround is closed.** Same reason. Delete it from any plan that still carries it.
- Adding a second loop button, jack-insertion detect, or a status LED no longer requires taking something else off the board.

## Migration delta

Everything below changes together. Per the commit convention this is a `fw+hw:` change — `PINOUT.md`, `config.h`, and the schematic move in one commit, or they diverge.

**Firmware (`firmware/include/config.h`)**
- Full renumber against the table above. No GPIO number survives from the S3 map.
- `platformio.ini`: new environment targeting the P4 via `pioarduino/platform-espressif32`. A custom board JSON is needed — no stock definition matches N16R32 (16MB flash / 32MB PSRAM).
- Joystick moves to `analogRead` on ADC1 (GPIO16/17); backlight stays `ledcAttach`/`ledcWrite`. Same Arduino APIs either way.
- The `native` host-test environment is unaffected.

**Hardware (`lochord_hw.kicad_sch` / `.kicad_pcb`)**
- **Replace the module footprint.** ESP32-S3-WROOM-1 (18 × 25.5mm, 40 pads + EPAD, 3 edges, 1.27mm pitch) → WT0132P4-A1 (25.00 × 20.00mm, 82 pads, 4 edges, no thermal pad, 1.00mm pitch, 0.56 × 1.80mm pads, first pad 1.22mm in from the corner). Different outline, different pad map, nothing survives.
- **Rewire all 7 chord buttons and the loop button** to the GPIOs above.
- **Rewire all 3 encoders.** Unlike the S3 revision, these do *not* keep their existing nets — every encoder pin moves.
- **Change the switch footprints from Choc v2 to Choc v1** — see `lochord_hw/CLAUDE.md`.
- **Add the ES8388 block**: codec, I2C pull-ups, reset line, AC coupling caps to the jack, and its own analogue LDO.
- **Rebuild the power tree**: `LD1117S33` out, 5V straight to module VCC, two new LDOs (digital + analogue).
- **Add the 4-pin debug header** on GPIO24/25 + 3V3 + GND.
- **Add test pads** for USB_DM/USB_DP (pads 16/17).
- **Add the LCD FPC connector** (8-pin) and backlight driver on the new pins.
- **Add the joystick FPC connector** (5-pin, 0.5mm pitch) on the new pins.
- EN keeps `SW1` + `C5` but drops `R3` — the module provides the pull-up.

**Keep the S3 map in version control alongside this one until a P4 board actually boots.** It is the only working reference if the migration stalls.
