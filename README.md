# LoChord

A real-time MIDI chord controller for ESP32-S3 with chord generation, looping, and parameter control.

Inspired by [HiChord](https://hichord.shop/).

## Architecture Overview

```
┌─────────────────────────────────────────────────────────────────┐
│                         main.cpp                                │
│   - Hardware initialization (USB, buttons, encoders)            │
│   - Main loop: polls inputs, calls controller methods           │
│   - Owns global instances: Clock, MidiOutput, MidiController    │
└──────────────────────────────┬──────────────────────────────────┘
                               │
        ┌──────────────────────┼──────────────────────┐
        ▼                      ▼                      ▼
┌───────────────┐    ┌─────────────────┐    ┌────────────────┐
│  lib/Input    │    │  lib/Midi       │    │  lib/Chords    │
│  Button       │    │  MidiOutput     │    │  ChordGenerator│
│  (Encoder)    │    │  MidiController │    │  Theory        │
│  (Joystick)   │    └────────┬────────┘    └────────────────┘
└───────────────┘             │
                              ▼
                    ┌─────────────────┐
                    │  lib/Core       │
                    │  Clock          │
                    │  MidiEvent      │
                    └─────────────────┘
                              │
                              ▼
                    ┌─────────────────┐
                    │  lib/Sequencer  │  ← Future: Scheduler,
                    │  (planned)      │    Arpeggiator, LoopBuffer
                    └─────────────────┘
```

## Library Descriptions

### lib/Core
Foundation types shared across the project.

| File | Purpose |
|------|---------|
| `Clock.h/cpp` | BPM tracking, time conversions (beats ↔ microseconds) |
| `MidiEvent.h` | Timestamped MIDI event struct for scheduling/recording |

### lib/Input
Hardware input abstraction.

| File | Purpose |
|------|---------|
| `Button.h/cpp` | Debounced button with press/release detection |
| *(future)* | Encoder, Joystick classes |

### lib/Chords
Music theory and chord generation (pure data, no timing).

| File | Purpose |
|------|---------|
| `Theory.h` | Scales, qualities, intervals, Chord struct |
| `ChordGenerator.h/cpp` | Generates MIDI note arrays from scale degree |

### lib/Midi
MIDI output and high-level control.

| File | Purpose |
|------|---------|
| `MidiOutput.h/cpp` | Thin wrapper around TinyUSB MIDI - sends events |
| `MidiController.h/cpp` | Coordinates chord generation → output, manages active notes |

### lib/Sequencer *(planned)*
Time-aware event scheduling and loop management.

| Component | Purpose |
|-----------|---------|
| `Scheduler` | Queue of pending MidiEvents, fires when due |
| `Arpeggiator` | Plays chord notes over time based on current BPM |
| `LoopBuffer` | Fixed-size event storage for loop record/playback |
| `Sequencer` | State machine (STOPPED/RECORDING/PLAYING/OVERDUB) |

## Data Flow

### Live Play (current)
```
Button press → MidiController.processNoteOn(root)
            → ChordGenerator.getMidiChord(root)
            → MidiOutput.noteOn() for each note
```

### Live Play with Scheduling (future)
```
Button press → MidiController.processNoteOn(root)
            → ChordGenerator.getMidiChord(root)
            → Scheduler.schedule(events)  // queued with timestamps
            
main loop   → Scheduler.update()
            → for each due event: MidiOutput.send(event)
```

### Loop Playback (future)
```
main loop → Sequencer.update()
         → if PLAYING/OVERDUB: check LoopBuffer for due events
         → MidiOutput.send(event)
         → if RECORDING/OVERDUB: also store live input in LoopBuffer
```

## Key Design Decisions

1. **Clock is external to MidiController** - Allows real-time BPM changes to affect arpeggios/strum mid-playback

2. **MidiOutput is a thin layer** - Just sends; doesn't decide what/when. Easy to mock for testing.

3. **MidiEvent has timestamps** - Enables recording, scheduling, and playback from a common format.

4. **Generation vs Scheduling separation** - ChordGenerator returns note data; Scheduler/Arpeggiator handles timing. Same chord can be played as block, strum, or arpeggio.

5. **No blocking delays** - Strum/arpeggio will use Scheduler instead of `delay()` to keep the system responsive.

## Memory Budget (ESP32-S3)

- ~320KB RAM available
- Loop buffer: ~10KB (625 events at 16 bytes each)
- Plenty of headroom for UI, buffers, etc.

---

## Development Commands

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
picocom -b 115200 /dev/ttyACM0
```
Exit with `Ctrl+A` then `Ctrl+X`
