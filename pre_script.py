Import("env")

board_config = env.BoardConfig()

board_config.update("build.hwids", [
    # First VID/PID will be used by firmware, and for PlatformIO's device discovery
    # Picked any from Linux kernel's drivers/hid/hid-quirks.c where HID_QUIRK_MULTI_INPUT is set.
    # Upside: allows multiple joysticks on same endpoint in Linux.
    # Downside: under Linux, ModemManager will grab the ttyACM device, so flashing won't work.
    #   Possible workarounds:
    #     1. Briefly connect RST to GND twice, then flash within 8 seconds (ie with bootloader active), or
    #     2. Create a udev rule for this VID/PID; search for ID_MM_TTY_BLACKLIST to find how, or
    #     3. `systemctl stop ModemManager`
    [ "0x04f2", "0xb19d" ],

    # Other VIDs/PIDs only used for device discovery
    [ "0x1B4F", "0x9206" ], # Default Sparkfun Arduino Pro Micro VIDs/Pids
    [ "0x1B4F", "0x9205" ],
    [ "0x2341", "0x8037" ],
    [ "0x2341", "0x0037" ],
    [ "0x2341", "0x8036" ], # Default Arduino Leonardo VIDs/PIDs
    [ "0x2341", "0x0036" ],
    [ "0x2A03", "0x0036" ],
    [ "0x2A03", "0x8036" ],
])

board_config.update("build.usb_product", "F/A-18C")
board_config.update("vendor", "Felix")