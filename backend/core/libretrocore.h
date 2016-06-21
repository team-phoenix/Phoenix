#pragma once

#include <QDir>
#include <QFile>
#include <QHash>
#include <QLibrary>
#include <QObject>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QRect>
#include <QSurface>

#include "core.h"
#include "gamepadstate.h"
#include "libretro.h"
#include "libretrosymbols.h"
#include "libretrovariable.h"
#include "logging.h"
#include "node.h"
#include "mousestate.h"

// Since each buffer holds one frame, depending on core, 30 frames = ~500ms
#define POOL_SIZE 30

/*
 * C++ wrapper around a Libretro core. Currently, only one LibretroCore instance may safely exist at any time due to the
 * lack of a context pointer for callbacks to use.
 *
 * The following keys are mandatory for source from Node::Command::SetSource:
 * "type": "libretro"
 * "core": Absolute path to the Libretro core
 * "game": Absolute path to a game the Libretro core accepts
 * "systemPath": Absolute path to the system directory (contents of which depend on the core)
 * "savePath": Absolute path to the save directory
 *
 * LibretroCore expects some kind of input producer (such as InputManager) to produce input which LibretroCore will then
 * consume. This input production also drives the production of frames (retro_run()), so time it such that it's as close
 * as possible to the console's native framerate!
 *
 * This also means that you can send updates from the input producer whenever you want, at any stage. retro_run() will
 * not be called unless you're in the PLAYING state.
 */

class LibretroCore : public Core {
        Q_OBJECT

    public:
        explicit LibretroCore( Core *parent = nullptr );
        ~LibretroCore();

    signals:
        void commandOut( Node::Command command, QVariant data, qint64 timeStamp );
        void dataOut( Node::DataType type, QMutex *mutex, void *data, size_t bytes, qint64 timeStamp );

    public:
        // Fire commandOut from a static context
        void fireCommandOut( Node::Command command, QVariant data, qint64 timeStamp );

        // Fire dataOut from a static context
        void fireDataOut( Node::DataType type, QMutex *mutex, void *data, size_t bytes, qint64 timeStamp );

        // Struct containing libretro methods
        LibretroSymbols symbols;

        // Used by environment callback. Provides info about the OpenGL context provided by the Phoenix frontend for
        // the core's internal use
        retro_hw_render_callback openGLContext;

        // Files and paths

        QLibrary coreFile;
        QFile gameFile;

        QDir contentPath;
        QDir systemPath;
        QDir savePath;

        // These must be members so their data stays valid throughout the lifetime of LibretroCore
        QFileInfo coreFileInfo;
        QFileInfo gameFileInfo;
        QFileInfo systemPathInfo;
        QFileInfo savePathInfo;
        QByteArray corePathByteArray;
        QByteArray gameFileByteArray;
        QByteArray gamePathByteArray;
        QByteArray systemPathByteArray;
        QByteArray savePathByteArray;
        const char *corePathCString{ nullptr };
        const char *gameFileCString{ nullptr };
        const char *gamePathCString{ nullptr };
        const char *systemPathCString{ nullptr };
        const char *savePathCString{ nullptr };

        // Raw ROM/ISO data, empty if (systemInfo->need_fullpath)
        QByteArray gameData;

        // SRAM

        void *saveDataBuf{ nullptr };

        // Core-specific constants

        // Information about the core (we store, Libretro core fills out with symbols.retro_get_system_info())
        retro_system_info *systemInfo;

        retro_perf_callback performanceCallback;

        // Node data

        Node::State currentState;

        // Producer data (for consumers like AudioOutput, VideoOutput...)

        // Get AV info from the core and fire signals to consumers
        void getAVInfo( retro_system_av_info *avInfo );

        // Video

        // Buffer pool (aka circular buffer of buffers), ensures thread safety (via the pipeline's unload/shutdown mechanism),
        // ensures atomic reads and writes (via a mutex) and prevents per-frame heap allocations
        QMutex videoMutex;
        uint8_t *videoBufferPool[ POOL_SIZE ] { nullptr };
        int videoPoolCurrentBuffer { 0 };
        size_t videoPoolIndividualBufferSize { 0 };

        // An OpenGL context for 3D cores to draw with
        QOpenGLContext *context { nullptr };

        // An FBO that serves as the target for the 3D core
        QOpenGLFramebufferObject *fbo { nullptr };

        // FIXME: Race condition with the render thread when making this current?
        QOffscreenSurface *surface { nullptr };

        // Video geometry info for use by video consumers
        LibretroVideoFormat videoFormat;

        // Audio

        qreal audioSampleRate { 44100 };

        // Buffer pool (aka circular buffer of buffers), ensures thread safety (via the pipeline's unload/shutdown mechanism),
        // ensures atomic reads and writes (via a mutex) and prevents per-frame heap allocations
        QMutex audioMutex;
        int16_t *audioBufferPool[ POOL_SIZE ] { nullptr };
        int audioPoolCurrentBuffer { 0 };
        size_t audioPoolIndividualBufferSize { 0 };

        // Amount audioBufferPool[ audioBufferPoolIndex ] has been filled
        // Each frame, exactly ( sampleRate * 4 ) bytes should be copied to
        // audioBufferPool[ audioBufferPoolIndex ][ audioBufferCurrentByte ] in total
        // FIXME: In practice, that's not always the case? Some cores only hit that *on average*
        int audioBufferCurrentByte { 0 };

        // Input

        LibretroVideoFormat consumerFmt;

        // Gamepad states
        // Everything is indexed by instanceID here as we need to see the states of every connected controller individually
        // Note that gamepads is overwritten with data from its parent at the beginning of each frame.
        // Nothing you set will be preserved.
        QHash<int, GamepadState> gamepads;

        // For fallback rumble, which requires us to explicitly stop and set a rumble effect each time we want to change
        // the strength, we must make sure not to waste time resetting it unless the value's actually changed. Store the value of the
        // current/active strength here. Indexed by instanceID, given a default of 0.0 on controller connect.
        QHash<int, qreal> fallbackRumbleCurrentStrength;

        MouseState mouse;
        QRect windowGeometry;
        int aspectMode { 0 };

        // Information about the controllers and buttons used by the core
        // FIXME: Where's the max number of controllers defined?
        // Key format: "port,device,index,id" (all 4 unsigned integers are represented as strings)
        //     ex. "0,0,0,0"
        // Value is a human-readable description
        QMap<QString, QString> inputDescriptors;

        // Misc

        // Core-specific variables
        QMap<QByteArray, LibretroVariable> variables;

        // True if variables are dirty and the core needs to reload them from us
        bool variablesAreDirty{ false };
};

// Libretro is a C API. This limits us to one LibretroCore per process.
extern LibretroCore libretroCore;

// SRAM
void LibretroCoreLoadSaveData();
void LibretroCoreStoreSaveData();

// Should only be called on load time (consumers expect buffers to be valid while Core is active)
void LibretroCoreGrowBufferPool( retro_system_av_info *avInfo );
void LibretroCoreFreeBufferPool();

// Callbacks
void LibretroCoreAudioSampleCallback( int16_t left, int16_t right );
size_t LibretroCoreAudioSampleBatchCallback( const int16_t *data, size_t frames );
bool LibretroCoreEnvironmentCallback( unsigned cmd, void *data );
void LibretroCoreInputPollCallback( void );
void LibretroCoreLogCallback( enum retro_log_level level, const char *fmt, ... );
int16_t LibretroCoreInputStateCallback( unsigned port, unsigned device, unsigned index, unsigned id );
void LibretroCoreVideoRefreshCallback( const void *data, unsigned width, unsigned height, size_t pitch );

// Extra callbacks
uintptr_t LibretroCoreGetFramebufferCallback( void );
retro_proc_address_t LibretroCoreOpenGLProcAddressCallback( const char *sym );
bool LibretroCoreRumbleCallback( unsigned port, enum retro_rumble_effect effect, uint16_t strength );

// Helper that generates key for looking up the inputDescriptors
QString LibretroCoreInputTupleToString( unsigned port, unsigned device, unsigned index, unsigned id );
