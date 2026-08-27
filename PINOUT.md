# PINOUT.md — LoChord signal map

Single source of truth for every net that crosses the firmware ↔ hardware boundary. `firmware/include/config.h` and the `lochord_hw` schematic must both match this file — if either disagrees with it, that side is out of date, not this file.

**Status:** this file is written for the **ESP32-P4** and supersedes the previous ESP32-S3-WROOM-1 assignment entirely — every GPIO number on the board changed. **The schematic and PCB now match this file.** `firmware/include/config.h` does not yet; see "Migration delta" at the bottom for what is left on the firmware side.

**Verification status.** Every pad number, GPIO number, and alternate function in this file has been checked line by line against **WT0132P4-A1 datasheet v2.3** (section 3.2 pin description, Figure 7 pin layout, Figure 9 dimensions) and against the **ESP32-P4 chip revision v1.3 datasheet** for the peripheral-level claims (strapping, ADC channels, SPI2 IO_MUX, USB PHYs). There are no `verify` rows left. Re-run the check if the module datasheet revs past v2.3.

One thing that check settled, because it is stated wrongly in several places online: **the module breaks out all 55 of the ESP32-P4's GPIOs.** Counting the pin table — GPIO0–1 (pads 71–72), GPIO2–15 (55–69), GPIO16–23 (74–81), GPIO24–27 (19, 20, 22, 23), GPIO28–43 (24–31, 33–40), GPIO44–54 (43–52, 54) — gives exactly 55. The "44 GPIO" figure that appears on vendor product pages is wrong; the datasheet's own feature list says "ESP32-P4 chip with full pinout". Nothing in the spares list below is contingent on a smaller set.

## Module

Target module is the **WT0132P4-A1-N16R32** — Wireless-Tag's castellated ESP32-P4 core board: 16MB flash, 32MB PSRAM stacked in-package, 25.00 × 20.00mm, 3.3mm tall (0.8mm carrier PCB + 2.5mm shield can), 82 pads across all four edges, no thermal pad.

Footprint numbers from Figure 9, for the KiCad footprint: **1.00mm pad pitch**, pad 0.56mm wide × 1.80mm long, and **1.22mm from the module corner to the near *edge* of the first pad — which puts the first pad *centre* 1.50mm in** (1.22 + 0.56/2). The 1.50mm figure is the one to build to, and it is the only one that closes: 1.5 + 22×1.0 + 1.5 = 25.00mm across the 23-pad long edges, and 1.5 + 17×1.0 + 1.5 = 20.00mm across the 18-pad short edges. An earlier revision of this file read 1.22mm as the pad centre; that was wrong and produced an asymmetric pad row. Supply is **4.8–5.5V on pad 41 with 1A typical draw** (section 4.3) — see "Power".

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
| 1 | Bottom row, 1st from left | 1u w × 2u h | 44 | 43 | SW3 |
| 2 | Bottom row, 2nd from left | 1u w × 2u h | 45 | 44 | SW4 |
| 3 | Bottom row, 3rd from left | 1u w × 2u h | 46 | 45 | SW5 |
| 4 | Bottom row, 4th from left | 1u w × 2u h | 47 | 46 | SW6 |
| 5 | Top row, left | 1.5u w × 1u h | 48 | 47 | SW7 |
| 6 | Top row, centre | 1u w × 1u h | 49 | 48 | SW8 |
| 7 | Top row, right | 1.5u w × 1u h | 50 | 49 | SW9 |

Reference designators were renumbered when the board was rebuilt for the P4 — SW1/SW2 are EN/BOOT, SW3–SW9 are the chord keys in physical order, SW10 is the loop key, and the encoders are RE1–RE3. The old scattered numbering (SW6, SW7, SW9, SW8, …) is gone.

Ascending GPIO order runs bottom-left → bottom-right, then top-left → top-right, so `buttonPins[]` index order and physical position match by construction. **Wire the switches to match this table** — it is a specification, not an assumption.

Whether the *musical* degree order should differ from this physical order is still open (the HiChord interleaves odd degrees on the bottom row and even on the top). That is a firmware mapping decision on top of this table, not a wiring one — record it here once decided.

GPIO49/50 consume **ADC2_CH0/CH1**. That is free of consequence on the P4: there is no radio to block ADC2, and the joystick uses ADC1 anyway.

Note for anyone who later wants ADC2: **the module datasheet's ADC2 channel numbers are wrong.** It labels GPIO49–54 as `ADC2_CHANNEL2`–`ADC2_CHANNEL7`; the ESP32-P4 chip datasheet (Table 2-7, Analog Functions) labels the same pins `ADC2_CHANNEL0`–`ADC2_CHANNEL5`. The chip datasheet is authoritative and is what ESP-IDF's channel enums follow. The pin *identities* are the same either way, so nothing on this board is affected — only the channel constant you would pass to the ADC driver.

## Loop control button

| Signal | GPIO | Pad | Notes |
|---|---|---|---|
| Loop button (1u, Choc v1) | 51 | 50 | Sch. ref SW10. Drives record/play/overdub/stop |

Continues the chord-button run, so all 8 keys sit on 8 adjacent top-edge pads. Not a strapping pin — a plain switch-to-GND with an internal pull-up, no special handling. (The S3 design had this on IO46 with a long warning attached about its reset-time pull-down; that warning does not carry over and should not be copied forward.)

## Rotary encoders (3×, ALPS EC11E15244G1, clickable)

All three on the module's **right edge** (pads 24–41 in module-local terms). Note that the module is fitted rotated 270°, so that edge physically faces the *bottom* of the board while the encoders themselves sit in a column to the module's right — the pad grouping is still contiguous, the traces just take a short dog-leg. See `lochord_hw/CLAUDE.md`, "Why the module is rotated 270°", for why that rotation won. Each of the 9 signals carries an external 10k pull-up (R5–R13); the A/B pins additionally want RC debouncing if contact bounce proves troublesome.

| Encoder | Role | Pin A | Pin B | Switch | Pads | Sch. ref |
|---|---|---|---|---|---|---|
| 1 | **UI navigation** (menu/parameter select) | 39 | 40 | 41 | 36, 37, 38 | RE1 |
| 2 | **Control 1** (e.g. BPM) | 28 | 29 | 30 | 24, 25, 26 | RE2 |
| 3 | **Control 2** (e.g. strum amount) | 31 | 32 | 33 | 27, 28, 29 | RE3 |

None of these are strapping pins, so unlike the S3 map there is no "do not put an encoder here" exclusion to observe. The external pull-ups are safe on all nine.

The encoders occupy two runs on the right edge, not one: pads 24–29 (encoders 2 and 3) and pads 36–38 (encoder 1). **Pads 30–35 sit between them and are all spoken for** — GPIO34, the BOOT strap (35), `ESP_LDO_VO4` (32), the boot-mode strap (36), and the UART0 download pair (37/38). So the BOOT switch, the boot-select pull-up, and the UART download pads all land in the middle of the encoder column. That is the best available arrangement — those six pads cannot move — but it is worth knowing before placing parts, because those support components have to be tucked around the encoder wiring rather than grouped elsewhere.

GPIO28–33 are also the P4's *alternate* SPI2 mapping (`SPI2 CS/D/CK/Q/HOLD/WP`). Nothing on this board uses that mapping — the display is on the IO_MUX set, GPIO7–11 — so there is no conflict, but don't let the datasheet's pin-function text suggest these pads are reserved.

**Encoder debouncing — decided.** Firmware-first, with the hardware option kept open at zero cost: the six A/B lines each carry a **10nF capacitor footprint to GND (C27–C32), fitted but marked DNP**. Debounce in firmware; if contact bounce proves troublesome on real hardware, populate the caps and you have a ~100µs RC with the 10k pull-ups, without a board respin. This is the reason the decision no longer blocks the schematic.

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
| Codec CE (I2C address select) | — | — | **Strapped low through R24 (10k to GND) → 7-bit I2C address `0x10`.** Not a GPIO |

**The ES8388 has no hardware reset pin.** An earlier revision of this file assigned GPIO19 (pad 77) as an active-low codec RESET. That was wrong — the 28-pin QFN has no such pin, and reset is a *register* operation (register 0x00 bit 7, `SCPReset`). **GPIO19 / pad 77 is therefore free and has moved to the spares list.** Firmware resets the codec over I2C as the first step of init; there is nothing to toggle in hardware.

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

A 4-pin 0.1" (or JST-SH) header is enough — this is a bring-up and development connector, not a user-facing port, so it does not need a USB-C receptacle of its own. Fitted as `J2`, a 1×4 2.54mm header on the top face, standing vertically in the clear corridor between the headphone-jack column and the left edge of the chord pad (board-relative 46.70, 64.50 for pin 1). It is outside every keycap footprint, so it stays reachable with the board assembled and with caps fitted. Pin order top to bottom is **3V3, GND, D−, D+**, and those four names are on the front silkscreen beside the pins.

**UART0 (GPIO37/38, pads 34/35) is brought out to test pads `TP3`/`TP4`.** The bootloader download port is reserved either way, and two pads make it probeable without adding a second connector.

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
| 3V3 digital | `U2` AMS1117-3.3, SOT-223 | LCD, backlight driver, joystick, ES8388 **DVDD and PVDD**, I2C pull-ups, power LED |
| 3V3 analogue | `U3` LP5907MFX-3.3, SOT-23-5, through ferrite `FB1` | ES8388 **AVDD and HPVDD** only |

Three things that are not optional:

1. **`LD1117S33` comes out.** It was already the wrong part for the S3's current draw and dropout, and the MCU no longer needs a 3V3 rail from the board at all. Both replacement LDOs are new part selections — see `lochord_hw/CLAUDE.md`.
2. **`ESP_LDO_VO4` (pad 32) is a trap.** The module exposes it and it looks like free 3V3 sitting right there, but it is the P4's own internal LDO: 0.2A maximum, and it carries digital core switching noise. Do not hang the backlight on it. Do not hang anything analogue on it. Use it for nothing.
3. **The USB current budget must be done before layout, not after.** The module datasheet specifies the external supply should source ~1A. A plain USB 2.0 host port guarantees 500mA. A P4 at 360MHz with 32MB PSRAM, the LCD backlight, and the headphone amp can plausibly exceed that on transients — and this is a device whose whole purpose is being plugged into a laptop. Mitigations, cheapest first: generous bulk capacitance (≥100µF plus ceramics), clock the P4 down (MIDI plus a 428×142 LVGL UI does not need 360MHz), and read the USB-C CC resistors to detect whether the source advertises 1.5A/3.0A before unlocking full clocks and backlight brightness.

**`PVDD` is the ES8388's digital I/O supply, not an analogue rail.** An earlier revision of this file put `AVDD` and `PVDD` on the analogue LDO. That is wrong: per the datasheet pin table, `DVDD` is the digital core supply and `PVDD` is the digital *I/O* supply — both must sit on the same 3V3 rail as the host's I/O. The analogue rail feeds **`AVDD` (analogue supply) and `HPVDD` (headphone driver supply)**.

`R2`/`R3` are the USB-C CC pull-downs (5.1k). Battery presence is still undecided.

## Spares

Everything not assigned above. The pin squeeze that drove this migration is gone: the S3 map had 4 spares against 32 assigned pins, two of them barely usable.

| GPIO | Pad | Suitability |
|---|---|---|
| 2, 3, 4, 5 | 55–58 | Unrestricted. These four are the complete pin-JTAG set (MTCK/MTDI/MTMS/MTDO) — kept free so that option survives |
| 19, 20, 21, 22, 23 | 77–81 | Unrestricted. All five are ADC1 channels (CH3–CH7), so this is where a second analogue control would go. GPIO19 landed here when the phantom codec RESET was removed |
| 34 | 30 | JTAG source select, floats at reset. Fine as an input or output — **prefer not to fit a permanent pull-up** |
| 42, 43 | 39, 40 | Unrestricted |
| 52, 53, 54 | 51, 52, 54 | Unrestricted. Confirmed broken out; also ADC2_CH3/CH4/CH5 and the analogue comparator inputs |

That is **15** genuinely free GPIOs after everything is assigned, against 4 on the S3. Consequences:

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

**Hardware (`lochord_hw.kicad_sch` / `.kicad_pcb`) — done**

The schematic and PCB were rebuilt against this file. For the record, what changed:

- **Module footprint replaced.** ESP32-S3-WROOM-1 → `lochord:WT0132P4-A1` (25.00 × 20.00mm, 82 pads, 1.00mm pitch, four edges, no thermal pad). Land pattern 0.60 × 2.00mm, 0.60mm under the module body and 1.40mm proud of it so a hand-solder fillet is visible from the side.
- **All 7 chord buttons, the loop button and all 3 encoders rewired** to the GPIOs above. No net survived from the S3 revision.
- **Switch footprints are Choc v1** — `SW_Kailh_Choc_V1_2.00u` **placed at 90°** for the four vertical 2u bottom keys, `..._1.50u` for the two wide top keys, `..._1.00u` for the centre top key and the loop key. Do not use the `..._2.00u_90deg` variant here; see "Choc `_90deg` footprints rotate the keycap, not the switch" in `lochord_hw/CLAUDE.md`.
- **ES8388 block added**: codec, 4.7k I2C pull-ups, 33Ω series damping on all four I2S lines, `CE` strapped low for address `0x10`, VREF/VMID/ADCVREF decoupling, 220µF output coupling into the jack.
- **Power tree rebuilt**: `LD1117S33` gone; 5V straight from USB-C VBUS to module pad 41; `U2` AMS1117-3.3 for the digital rail and `U3` LP5907MFX-3.3 (through ferrite `FB1`) for the analogue rail.
- **Debug header `J2`** on GPIO24/25 + 3V3 + GND, plus `TP1`/`TP2` for the High-Speed USB PHY pads and `TP3`/`TP4` for UART0.
- **LCD FPC `J4`** (8-way 0.5mm) and **joystick FPC `J3`** (5-way 0.5mm) added on the new pins.
- **EN keeps `SW1` + `C1`** and drops the old external pull-up — the module provides it.
- **USB-C gained ESD protection** (`U4`, USBLC6-2SC6), which the S3 revision did not have.

**Firmware is the only side still outstanding.**

**Keep the S3 map in version control alongside this one until a P4 board actually boots.** It is the only working reference if the migration stalls — and it is now only reachable through git history, since the schematic and PCB have been rebuilt.
