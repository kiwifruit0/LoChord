# LoChord
A DIY version of the popular midi device: ![HiChord](https://hichord.shop/)

## General Commands
to generate `compile_commands.json`:
```pio run -t compiledb```

to run and upload:
```pio run -t upload```

viewing serial monitor:
```picocom -b 115200 /dev/ttyACM0```
and to exit: `Ctrl+A then Ctrl+X`
