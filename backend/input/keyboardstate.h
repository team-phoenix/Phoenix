#pragma once

/*
 * KeyboardState is a small struct that holds a heartbeat's worth of key states. This will be transformed into Gamepad
 * states inside of Remapper.
 */

struct KeyboardState {
    KeyboardState() = default;

    // Because key values run the entire possible range of ints, it's inefficient to store a 4GB * sizeof(int) array.
    // Storing the values as a hash map is also no good; you'll end up doing expensive heap allocations copying data to
    // this node's child as a signal and it's not thread-safe to simply send the hash map as a pointer.
    // These arrays act as a circular buffer. Keyboard pressed and released events are packed in as they come and the
    // index gets increased accordingly. To unpack, start "replaying" from 0 to tail and handle each (index,pressed)
    // pair you encounter accordingly.
    // If you manage to hit more than 128 keys in a single heartbeat, that's your problem!

    // First element of the buffer
    int head { 0 };

    // One beyond final entry in the buffer
    int tail { 0 };

    // Key code circular buffer
    int key[ 128 ] { 0 };

    // Key state circular buffer
    bool pressed[ 128 ] { false };
};
