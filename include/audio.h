
#ifndef AUDIO_H
#define AUDIO_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QAudioOutput>
#include <QDebug>

#include <memory>

#include "audiobuffer.h"
#include "logging.h"

#include "samplerate.h"

/* The Audio class writes audio data to connected audio device.
 * All of the audio functionality lives in side of this class.
 * Any errors starting with "[phoenix.audio]" correspond to this class.
 *
 * The audio class is instantiated inside of the videoitem.cpp class.
 * The Audio class uses the AudioBuffer class, which lives in the audiobuffer.cpp class, as a temporary audio buffer
 * that can be written has a whole chunk to the audio output.
 */

class Audio : public QObject {
        Q_OBJECT
    public:
        Audio( QObject * = 0 );
        ~Audio();

        void startAudioThread();

        void setInFormat( QAudioFormat newInFormat );

        AudioBuffer *getAudioBuf() const;

    signals:
        void signalFormatChanged();

    public slots:
        void slotStateChanged( QAudio::State state );

        void slotRunChanged( bool _isCoreRunning );
        void slotSetVolume( qreal level );

    private slots:
        void slotThreadStarted();
        void slotHandleFormatChanged();
        void slotHandlePeriodTimer();

    private:
        // libsamplerate callback to fetch audio data for the library to convert
        // Arguments: callbackData - User-defined pointer to a struct containing info it might need
        //            outBuf - Pointer to array of floats where the data will go. Maximum number of
        //                     frames it holds were provided to src_callback_read()
        // Returns: Number of frames provided to converter
        static long getAudioData( void *callbackData, float **outBuf );

        // Opaque pointer for libsamplerate
        SRC_STATE *resamplerState;

        qreal sampleRateRatio;
        int audioInBytesNeeded;
        float tempDataFloat[4096 * 2];
        float convertedDataFloat[4096 * 2];
        short convertedDataShort[4096 * 2];

        bool isCoreRunning;
        QAudioFormat audioFormatOut;
        QAudioFormat audioFormatIn;

        // We delete aout; Use a normal pointer.
        QAudioOutput *audioOut;

        // aio doesn't own the pointer; Use a normal pointer.
        QIODevice *audioOutIODev;

        QThread audioThread;
        QTimer audioTimer;

        std::unique_ptr<AudioBuffer>audioBuf;

};

#endif
