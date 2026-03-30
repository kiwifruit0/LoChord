# Sequencer Library

This library will contain time-aware event scheduling and loop management.

## Planned Components

### Scheduler
- Manages a queue of pending `MidiEvent`s with timestamps
- `update()` called each loop to fire due events
- Enables non-blocking strum, arpeggio, and other timed patterns

### Arpeggiator  
- Takes a `Chord` and plays notes over time based on current BPM
- Reads `Clock::getBpm()` each update for real-time tempo changes
- Supports patterns: up, down, up-down, random

### LoopBuffer
- Fixed-size circular buffer of `MidiEvent`s
- Records events with timestamps relative to loop start
- Supports overdub by merging new events into existing loop

### Sequencer (State Machine)
- States: STOPPED, RECORDING, PLAYING, OVERDUB
- Coordinates Clock, LoopBuffer, and event routing
- Routes live input through record path when armed
