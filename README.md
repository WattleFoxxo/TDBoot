# TDBoot
 A bootloader and boot menu for the lilygo t-deck

![A screenshot of the tdboot menu with the third entry (app2) selected.](https://github.com/WattleFoxxo/TDBoot/blob/main/docs/img/screenshot.png?raw=true)

## How it works

1. TDBoot has a [custom stage 2 bootloader](https://github.com/WattleFoxxo/TDBoot-Bootloader) to always boot into the factory partition unless the esp was software restarted.
2. The factory partition has a simple boot menu that allows you to select what app/ota partition you boot into.

You can customise the partition table to change the name of each application, change the size of each application and add or remove custom data partitions.

To install open TDBoot in vscode with platformio and upload it to your t-deck.

## Flashing programs
### Using espflash

`espflash write-bin 0x{address} {binary}`

> example flashing the first partition

`espflash write-bin 0x070000 yourapp.bin`

## Related
- TDBoot-Bootloader: [WattleFoxxo/TDBoot-Bootloader](https://github.com/WattleFoxxo/TDBoot-Bootloader)
