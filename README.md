# flightpanel

Hardware and firmware for a USB input device for flight simulation computer games.

Project status: everything (hardware & software) works, and I've used it successfully. Not everything published yet; need to publish remaining files. Thinking about a version 2.


# System overview

```
                                                         up to 96x
┌──────────┐   ┌─────────────────┐   ┌──────────────────┐  ┌──────┐
│          │   │                 │   │                  ├──┤Button│
│          │USB│ Microcontroller │I2C│    Up to ~8      │  └──────┘
│ Computer ├───┤                 ├───┤ GPIO controllers │up to ~8x  up to 56x
│          │   │   Atmega32u4    │   │   PCA9575PW2     │  ┌──────┐  ┌──────┐
│          │   │                 │   │                  ├──┤Matrix├──┤Button│
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
