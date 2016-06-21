#pragma once

#include "logging.h"

#include "node.h"

using QStringMap = QMap<QString, QString>;

/*
 * Superclass for all core plugins used by Phoenix. Core subclasses are not expected to have extensive
 * error checking or validation of the commands given to them. For example, they should not have to expect Command::Pause
 * when pausable is false.
 *
 * Core is a producer of both audio and video data. At regular intervals, Core will send out signals containing pointers
 * to buffers. These pointers will internally be part of a circular buffer that will remain valid for the lifetime of Core.
 * To safely copy their contents, obtain a lock using either audioMutex or videoMutex.
 *
 * Core is also a consumer of input data.
 */

class Core : public QObject {
        Q_OBJECT

    public:
        explicit Core( QObject *parent = nullptr );
        virtual ~Core() = default;

    public:
        // Properties

        // Is this Core instance pausable? NOTE: "pausable" means whether or not you can *enter* State::PAUSED, not leave.
        // Core will ALWAYS enter State::PAUSED after State::LOADING regardless of this setting
        // Read-only
        bool pausable{ false };

        // Multiplier of the system's native framerate, if any. If rewindable, it can be any real number. Otherwise, it must
        // be positive and nonzero
        // Read-write
        qreal playbackSpeed{ 1.0 };

        // Is this Core instance resettable? If true, this usually means you can "soft reset" instead of fully resetting
        // the state machine by cycling through the deinit then init states
        // Read-only
        bool resettable{ true };

        // Is this Core instance rewindable? If true, playbackSpeed may go to and below 0 to make the game move backwards
        // Read-only
        bool rewindable{ false };

        // Subclass-defined info specific to this session (ex. Libretro: core, game, system and save paths)
        // Read-write
        QStringMap source;

        // Current state
        // Read-write
        Node::State state{ Node::State::Stopped };

        // Range: [0.0, 1.0]
        // Read-write
        qreal volume{ 1.0 };
};
