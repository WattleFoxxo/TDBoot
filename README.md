# TDBoot
 A bootloader and boot menu for the lilygo t-deck

## Flashing programs
### Using espflash

`espflash write-bin 0x{address} {binary}`

> example flashing the first partition

`espflash write-bin 0x70000 firmware.bin`
