

#include "audio.h"


Audio::Audio( QObject *parent )
    : QObject( parent ),
      isCoreRunning( false ),
      audioOut( nullptr ),
      audioOutIODev( nullptr ),
      audioTimer( this ),
      audioBuf( new AudioBuffer ) {

    // All future calls done in the context of this new thread
    this->moveToThread( &audioThread );
    connect( &audioThread, &QThread::started, this, &Audio::slotThreadStarted );
    audioThread.setObjectName( "phoenix-audio" );

    Q_CHECK_PTR( audioBuf );

    audioTimer.moveToThread( &audioThread );
    connect( &audioTimer, &QTimer::timeout, this, &Audio::slotHandlePeriodTimer );

    // We need send this signal to ourselves
    connect( this, &Audio::signalFormatChanged, this, &Audio::slotHandleFormatChanged );
}

AudioBuffer *Audio::getAudioBuf() const {
    return audioBuf.get();
}
void Audio::start() {
    audioThread.start( QThread::TimeCriticalPriority );
}

void Audio::setInFormat( QAudioFormat newInFormat ) {

    qCDebug( phxAudio, "setFormat(%iHz %ibits)", newInFormat.sampleRate(), newInFormat.sampleSize() );

    audioFormatOut = newInFormat;

    emit signalFormatChanged();

}

void Audio::slotHandleFormatChanged() {
    if( audioOut ) {
        audioOut->stop();
        delete audioOut;
    }

    audioOut = new QAudioOutput( audioFormatOut );
    Q_CHECK_PTR( audioOut );
    audioOut->moveToThread( &audioThread );

    connect( audioOut, &QAudioOutput::stateChanged, this, &Audio::slotStateChanged );
    audioOutIODev = audioOut->start();

    if( !isCoreRunning ) {
        audioOut->suspend();
    }

    audioOutIODev->moveToThread( &audioThread );
    audioTimer.setInterval( audioFormatOut.durationForBytes( audioOut->periodSize() * 1.5 ) / 1000 );

}

void Audio::slotThreadStarted() {
    if( !audioFormatIn.isValid() ) {
        // We don't have a valid audio format yet...
        qCDebug( phxAudio ) << "afmt is not valid";
        return;
    }

    slotHandleFormatChanged();
}

void Audio::slotHandlePeriodTimer() {
    Q_ASSERT( QThread::currentThread() == &audioThread );

    if( !audioOutIODev ) {
        static bool error_msg = true;

        if( error_msg ) {
            qCDebug( phxAudio ) << "Audio device was not found, stopping all audio writes.";
            error_msg = false;
        }

        return;
    }

    int toWrite = audioOut->bytesFree();

    if( !toWrite ) {
        return;
    }

    QVarLengthArray<char, 4096 * 4> tmpbuf( toWrite );
    int read = audioBuf->read( tmpbuf.data(), toWrite );
    int wrote = audioOutIODev->write( tmpbuf.data(), read );
    Q_UNUSED( wrote );

}

void Audio::slotRunChanged( bool _isCoreRunning ) {
    isCoreRunning = _isCoreRunning;

    if( !audioOut ) {
        return;
    }

    if( !isCoreRunning ) {
        if( audioOut->state() != QAudio::SuspendedState ) {
            qCDebug( phxAudio ) << "Paused";
            audioOut->suspend();
            audioTimer.stop();
        }
    } else {
        if( audioOut->state() != QAudio::ActiveState ) {
            qCDebug( phxAudio ) << "Started";
            audioOut->resume();
            audioTimer.start();
        }
    }
}

void Audio::slotStateChanged( QAudio::State s ) {
    if( s == QAudio::IdleState && audioOut->error() == QAudio::UnderrunError ) {
        qCDebug( phxAudio ) << "Underrun";
        audioOutIODev = audioOut->start();
    }

    if( s != QAudio::IdleState && s != QAudio::ActiveState ) {
        qCDebug( phxAudio ) << "State changed:" << s;
    }
}

void Audio::slotSetVolume( qreal level ) {
    if( audioOut ) {
        audioOut->setVolume( level );
    }
}

Audio::~Audio() {
    if( audioOut ) {
        delete audioOut;
    }

    if( audioOutIODev ) {
        delete audioOutIODev;
    }
}
