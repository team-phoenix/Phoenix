
#ifndef LIBRETRO_TYPES
#define LIBRETRO_TYPES

// Friendly typedefs for types used in libretro which are mostly enum-like
// macro constants.
// This is just to allow us to deal with data using more meaningful types
// names than just "int" or "unsigned".


// used to represent a device type
// NONE/JOYPAD/MOUSE/KEYBOARD/LIGHTGUN/ANALOG/POINTER or subclass
typedef unsigned retro_device_type;

// id of button
typedef unsigned retro_device_id;

#endif
