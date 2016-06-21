#include "gamepadstate.h"

GamepadState::GamepadState() {
    memset( &hapticEffect, 0, sizeof( SDL_HapticEffect ) );
}
