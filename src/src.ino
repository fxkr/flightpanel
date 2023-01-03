// Arduino project file.
//
// setup/loop are in main.cpp.
//
// Arduino IDE can edit/build this; however there's no (easy) way
// to add compilation options with it. So for a final build, it is
// recommended to use PlatformIO instead.
//
// (We need to add compilation options,
// because we need to change the USB VID/PID,
// because we need to hit the right quirks in the Linux kernel,
// because otherwise Linux won't let us emulate Joysticks on the same USB endpoint.
// Using multiple endpoints isn't supported by ArduinoJoystickLibrary.)