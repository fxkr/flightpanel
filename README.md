# flightpanel

Hardware and firmware for a USB input device for flight simulation computer games.

Project status: everything (hardware & software) works, and I've used it successfully. Not everything published yet; need to publish remaining files. Thinking about a version 2.


# System overview

```
                                    up to 15x (3x per PCB)   up to 16x each
┌──────────┐   ┌─────────────────┐   ┌──────────────────┐  ┌────────────────┐
│          │   │                 │   │                  ├──┤Button / Encoder│
│          │USB│ Microcontroller │I2C│  GPIO expanders  │  └────────────────┘
│ Computer ├───┤                 ├───┤                  │  up to 2x  up to 56x
│          │   │   Atmega32u4    │   │  NXP PCA9575PW2  │  ┌──────┐  ┌──────┐
│          │   │                 │   │   (3 per PCB)    ├──┤Matrix├──┤Button│
└──────────┘   └─────────────────┘   └──────────────────┘  └──────┘  └──────┘
 standard       standard PCB,         custom PCB           custom PCB
 drivers        custom firmware
```

Microcontroller:

* Firmware: [src](https://github.com/fxkr/flightpanel/tree/main/src)

GPIO controller:

* Schematic: [controller-rev1-final-sch.pdf](https://github.com/fxkr/flightpanel/blob/main/hardware/controller-rev1/controller-rev1-final-sch.pdf)

Button panel:

* Schematic: [button-panel-rev2-final-sch.pdf](https://github.com/fxkr/flightpanel/blob/main/hardware/button-panel-rev2/button-panel-rev2-final-sch.pdf)
