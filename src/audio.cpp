

#include "audio.h"


Audio::Audio( QObject *parent )
    : QObject( parent ),
      isRunning( false ),
      aout( nullptr ),
      aio( nullptr ),
      timer( this ) {

    this->moveToThread( &thread );
    connect( &thread, SIGNAL( started() ), SLOT( threadStarted() ) );
    thread.setObjectName( "phoenix-audio" );
    /*
    #ifdef Q_OS_LINUX
            soxr = nullptr;
            deviation = 0.005;
    #endif
            */

    m_abuf = new AudioBuffer();
    Q_CHECK_PTR( m_abuf );
    //connect(m_abuf, SIGNAL(hasPeriodSize()), this, SLOT(handleHasPeriodSize()));

    timer.moveToThread( &thread );
    connect( &timer, &QTimer::timeout, this, &Audio::handlePeriodTimer );

    // we need send this signal to ourselves
    connect( this, &Audio::formatChanged, this, &Audio::handleFormatChanged );
}

void Audio::start() {
    thread.start( QThread::TimeCriticalPriority );
}

/* This needs to be called on the audio thread*/
void Audio::setFormat( QAudioFormat _afmt ) {

    /*
    #ifdef Q_OS_LINUX
    qCDebug(phxAudio, "setFormat(%iHz %ibits)", _afmt.sampleRate(), _afmt.sampleSize());
    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());

    afmt_in = _afmt;
    afmt_out = info.nearestFormat(_afmt); // try using the nearest supported format
    if (afmt_out.sampleRate() < afmt_in.sampleRate()) {
        // if that got us a format with a worse sample rate, use preferred format
        afmt_out = info.preferredFormat();
    }
    qCDebug(phxAudio) << afmt_out;
    qCDebug(phxAudio, "Using nearest format supported by sound card: %iHz %ibits",
                      afmt_out.sampleRate(), afmt_out.sampleSize());

    soxr_error_t error;
    soxr_quality_spec_t q_spec = soxr_quality_spec(SOXR_HQ, SOXR_VR);
    soxr_io_spec_t io_spec = soxr_io_spec(SOXR_INT16_I, SOXR_INT16_I);
    soxr = soxr_create(
        afmt_in.sampleRate(), afmt_out.sampleRate(), 2,
        &error,
        &io_spec, &q_spec, NULL
    );
    soxr_io_ratio = (double)afmt_in.sampleRate() / afmt_out.sampleRate();
    if (error) {}

    #else
    */
    qCDebug( phxAudio, "setFormat(%iHz %ibits)", _afmt.sampleRate(), _afmt.sampleSize() );
    /*    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
        if (!info.isFormatSupported(_afmt)) {
            qCWarning(phxAudio) << "Audio format not supported by output device !";
            return;
        }*/
    afmt_out = _afmt;
    //#endif
    emit formatChanged();


}

void Audio::handleFormatChanged() {
    if( aout ) {
        aout->stop();
        delete aout;
    }

    aout = new QAudioOutput( afmt_out );
    //    aout->moveToThread(&thread);
    Q_CHECK_PTR( aout );
    aout->moveToThread( &thread );
    connect( aout, SIGNAL( stateChanged( QAudio::State ) ), SLOT( stateChanged( QAudio::State ) ) );
    aio = aout->start();

    if( !isRunning ) {
        aout->suspend();
    }

    aio->moveToThread( &thread );
    timer.setInterval( afmt_out.durationForBytes( aout->periodSize() * 1.5 ) / 1000 );

}

void Audio::threadStarted() {
    if( !afmt_in.isValid() ) {
        // we don't have a valid audio format yet...
        qCDebug( phxAudio ) << "afmt is not valid";
        return;
    }

    handleFormatChanged();
}

void Audio::handlePeriodTimer() {
    Q_ASSERT( QThread::currentThread() == &thread );

    if( !aio ) {
        static bool error_msg = true;

        if( error_msg ) {
            qCDebug( phxAudio ) << "Audio device was not found, stopping all audio writes.";
            error_msg = false;
        }

        return;
    }

    int toWrite = aout->bytesFree();

    if( !toWrite ) {
        return;
    }

    /*
    #ifdef Q_OS_LINUX
        int half_size = aout->bufferSize() / 2;
            int delta_mid = toWrite - half_size;
            qreal direction = (qreal)delta_mid / half_size;
            qreal adjust = 1.0 + deviation * -direction;

            // Compute the exact number of bytes to read from the circular buffer to
            // produce toWrite bytes of output; Taking resampling and DRC into account
            double new_ratio = soxr_io_ratio * adjust;
            auto afmt_tmp = afmt_in;
            afmt_tmp.setSampleRate(afmt_out.sampleRate() * new_ratio);
            int toRead = afmt_tmp.bytesForDuration(afmt_out.durationForBytes(toWrite));

        //    qCDebug(phxAudio) << (((double)(aout->bufferSize() - toWrite) / aout->bufferSize()) * 100) << "% full ; DRC:" << adjust
        //                      << ";" << toRead << afmt_in.bytesForDuration(afmt_out.durationForBytes(toWrite));

            QVarLengthArray<char, 4096*4> tmpbuf(toRead);
            int read = m_abuf->read(tmpbuf.data(), tmpbuf.size());

            int samplesToWrite = afmt_out.framesForBytes(toWrite);
            soxr_set_io_ratio(soxr, new_ratio, 0);

            char *obuf = new char[toWrite];
            size_t odone;
            soxr_error_t error = soxr_process(
                soxr,
                tmpbuf.data(), afmt_in.framesForBytes(read),
                NULL,
                obuf, samplesToWrite,
                &odone
            );

            if (error) {
                qCDebug(phxAudio) << "Resampling Error";
            }

            int wrote = aio->write(obuf, afmt_out.bytesForFrames(odone));
            Q_UNUSED(wrote);

    #else
    */
    QVarLengthArray<char, 4096 * 4> tmpbuf( toWrite );
    int read = m_abuf->read( tmpbuf.data(), toWrite );
    int wrote = aio->write( tmpbuf.data(), read );
    Q_UNUSED( wrote );
    //#endif


}

void Audio::runChanged( bool _isRunning ) {
    isRunning = _isRunning;

    if( !aout ) {
        return;
    }

    if( !isRunning ) {
        if( aout->state() != QAudio::SuspendedState ) {
            qCDebug( phxAudio ) << "Paused";
            aout->suspend();
            timer.stop();
        }
    } else {
        if( aout->state() != QAudio::ActiveState ) {
            qCDebug( phxAudio ) << "Started";
            aout->resume();
            timer.start();
        }
    }
}

void Audio::stateChanged( QAudio::State s ) {
    if( s == QAudio::IdleState && aout->error() == QAudio::UnderrunError ) {
        qCDebug( phxAudio ) << "Underrun";
        aio = aout->start();
    }

    if( s != QAudio::IdleState && s != QAudio::ActiveState ) {
        qCDebug( phxAudio ) << "State changed:" << s;
    }
}

void Audio::setVolume( qreal level ) {
    if( aout ) {
        aout->setVolume( level );
    }
}

Audio::~Audio() {
    if( aout ) {
        delete aout;
    }

    if( aio ) {
        delete aio;
    }

    if( m_abuf ) {
        delete m_abuf;
    }
}
