
#ifndef AUDIO_H
#define AUDIO_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QAudioOutput>
#include <QDebug>

/*
#ifdef Q_OS_LINUX
#include <soxr.h>
#endif
*/

#include "audiobuffer.h"
#include "logging.h"

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
        virtual ~Audio();

        void start();
        void setFormat( QAudioFormat _afmt );

        AudioBuffer *abuf() const {
            return m_abuf;
        }

    signals:
        void formatChanged();

    public slots:
        void stateChanged( QAudio::State s );

        void runChanged( bool isRunning );
        void setVolume( qreal level );

    private slots:
        void threadStarted();
        void handleFormatChanged();
        void handlePeriodTimer();

    private:
        bool isRunning; // is core running
        QAudioFormat afmt_out;
        QAudioFormat afmt_in;
        QAudioOutput *aout;
        QIODevice *aio;
        AudioBuffer *m_abuf;
        QThread thread;
        QTimer timer;
        /*
        #ifdef Q_OS_LINUX
            qreal deviation;
            soxr_t soxr;
            double soxr_io_ratio;
        #endif
            */

};

#endif
