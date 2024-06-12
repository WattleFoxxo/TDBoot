# TDBoot
 A bootloader and boot menu for the lilygo t-deck

![A screenshot of the tdboot menu with the third entry (app2) selected.](https://github.com/WattleFoxxo/TDBoot/blob/main/docs/img/screenshot.png?raw=true)

## Flashing programs
### Using espflash

`espflash write-bin 0x{address} {binary}`

> example flashing the first partition

`espflash write-bin 0x70000 firmware.bin`
