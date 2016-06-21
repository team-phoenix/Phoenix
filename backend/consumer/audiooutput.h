#pragma once

#include "node.h"
#include "audiobuffer.h"
#include "samplerate.h"

#include <QAudio>
#include <QAudioFormat>

/*
 * The AudioOutput class writes data to the default output device. Its internal buffers must be set by invoking
 * commandIn() with the proper arguments before any data can be passed to it via dataIn(). Set the volume (from 0 to 1
 * inclusive) with slotSetVolume().
 *
 * Comments in this class use the words "frames" and "samples". For clarity, assuming 16-bit stereo audio:
 * 1 frame = 4 bytes (L, L, R, R)
 * 1 sample = 2 bytes (L, L) or (R, R)
 */

class QAudioOutput;

class AudioOutput : public Node {
        Q_OBJECT

    public:
        explicit AudioOutput( Node *parent = nullptr );
        ~AudioOutput();

    public slots:
        void commandIn( Command command, QVariant data, qint64 timeStamp ) override;
        void dataIn( DataType type, QMutex *mutex, void *data, size_t bytes, qint64 timeStamp ) override;

    private slots:
        void outputStateChanged( QAudio::State outputState );
        void outputUnderflow();

    private:
        bool vsync{ true };
        State state{ State::Stopped };

        // Respond to the core running or not by keeping audio output active or not
        // AKA we'll pause if core is paused
        void pipelineStateChanged();

        // Free memory, clean up
        void shutdown();

        // Completely init/re-init audio output and resampler
        void resetAudio();

        // Allocate memory for conversion
        void allocateMemory();

        // Opaque pointer for libsamplerate
        SRC_STATE *resamplerState{ nullptr };

        // Audio and video timing provided by Core via the controller
        int sampleRate{ 0 };
        double hostFPS{ 60.0 };
        double coreFPS{ 60.0 };
        double sampleRateRatio{ 1.0 };

        // Internal buffers used for resampling
        short *inputDataShort{ nullptr };
        float *inputDataFloat{ nullptr };
        float *outputDataFloat{ nullptr };
        short *outputDataShort{ nullptr };

        // Set to true if the core is currently running
        bool coreIsRunning{ false };

        // Input and output audio formats being used
        QAudioFormat outputAudioFormat;
        QAudioFormat inputAudioFormat;

        // An interface to the output device
        QAudioOutput *outputAudioInterface{ nullptr };

        // Size of outputBuffer's unconsumed data
        int outputCurrentByte{ 0 };

        // A buffer that removes data from itself once it's read
        AudioBuffer outputBuffer;

        //
        // TODO: Make these configurable
        //

        // Max size of the outputBuffer. Equivalent to "audio buffering" setting in other programs
        int outputLengthMs{ 200 };

        // Ideal amount of data in the output buffer. Make this large enough to ensure no underruns
        int outputTargetMs{ 40 };

        // Max amount of stretching performed to compensate for output buffer position being off target
        double maxDeviation{ 0.005 };

        //
        // ---
        //

};
