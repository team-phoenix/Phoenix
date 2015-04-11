
#include "audio.h"

Audio::Audio( QObject *parent )
    : QObject( parent ),
      isCoreRunning( false ),
      audioOut( nullptr ),
      audioOutIODev( nullptr ),
      audioTimer( this ),
      audioBuf( new AudioBuffer ) {

    Q_CHECK_PTR( audioBuf );

    audioThread.setObjectName( "phoenix-audio" );

    resamplerState = nullptr;

    moveToThread( &audioThread );
    connect( &audioThread, &QThread::started, this, &Audio::slotThreadStarted );
    connect( &audioTimer, &QTimer::timeout, this, &Audio::slotHandlePeriodTimer );

    // We need to send this signal to ourselves
    connect( this, &Audio::signalFormatChanged, this, &Audio::slotHandleFormatChanged );
}

AudioBuffer *Audio::getAudioBuf() const {
    return audioBuf.get();
}
void Audio::startAudioThread() {
    audioThread.start( QThread::TimeCriticalPriority );
}

void Audio::setInFormat( QAudioFormat newInFormat ) {

    qCDebug( phxAudio, "setInFormat(%iHz %ibits)", newInFormat.sampleRate(), newInFormat.sampleSize() );

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

    if( resamplerState ) {
        src_delete( resamplerState );
    }

    int errorCode;
    resamplerState = src_callback_new( ( src_callback_t )&Audio::getAudioData, SRC_SINC_BEST_QUALITY, 2, &errorCode, this );

    if( !resamplerState ) {
        qCWarning( phxAudio ) << "libresample could not init: " << src_strerror( errorCode ) ;
    }

    sampleRateRatio = ( double )audioFormatOut.sampleRate() / audioFormatIn.sampleRate();

}

void Audio::slotThreadStarted() {
    if( !audioFormatIn.isValid() ) {
        // We don't have a valid audio format yet...
        qCDebug( phxAudio ) << "audioFormatIn is not valid";
        return;
    }

    slotHandleFormatChanged();
}

void Audio::slotHandlePeriodTimer() {
    Q_ASSERT( QThread::currentThread() == &audioThread );

    if( !audioOutIODev ) {
        static bool audioDevErrHandled = false;

        if( !audioDevErrHandled ) {
            qCDebug( phxAudio ) << "Audio device was not found, stopping all audio writes.";
            audioDevErrHandled = true;
        }

        return;
    }

    // Nothing to write means the output buffer if full, a duplicate frame may occur
    int outBytesToWrite = audioOut->bytesFree();

    if( !outBytesToWrite ) {
        return;
    }

    // Calculate DRC info
    qreal deviation = 0.005;
    int outBufMidPoint = audioOut->bufferSize() / 2;
    int drift = outBytesToWrite - outBufMidPoint;
    qreal direction = ( qreal )drift / outBufMidPoint;
    qreal adjust = 1.0 + deviation * -direction;

    // Compute the exact number of bytes to read from the circular buffer to
    // produce toWrite bytes of output; Taking resampling and DRC into account
    double outDRCRatio = sampleRateRatio * adjust;
    auto audioFormatDRC = audioFormatIn;
    audioFormatDRC.setSampleRate( audioFormatOut.sampleRate() * outDRCRatio );
    bytesToRead = audioFormatDRC.bytesForDuration( audioFormatOut.durationForBytes( outBytesToWrite ) );

    qCDebug( phxAudio ) << ( ( ( double )( audioOut->bufferSize() - outBytesToWrite ) / audioOut->bufferSize() ) * 100 ) << "% full ; DRC:" << adjust
                        << ";" << bytesToRead << audioFormatIn.bytesForDuration( audioFormatOut.durationForBytes( outBytesToWrite ) );

    // Preform conversion
    long framesConverted = src_callback_read( resamplerState, outDRCRatio, bytesToRead / 4,  convertedDataFloat );

    // Convert float data back to shorts
    src_float_to_short_array( convertedDataFloat, convertedDataShort, 4096 );

    int errorCode;

    if( !framesConverted && ( errorCode = src_error( resamplerState ) ) ) {

        qCWarning( phxAudio ) << "libresample error: " << src_strerror( errorCode ) ;
    }

    // Send audio data to output device
    int bytesWritten = audioOutIODev->write( ( char * )convertedDataShort, bytesToRead );
    qCDebug( phxAudio ) << "Wrote " << bytesWritten << "bytes to output.";
    //Q_UNUSED( bytesWritten );

}

long Audio::getAudioData( void *callbackData, float **outBuf ) {
    // 16-bit stereo PCM
    // 4096 frames
    // 8192 samples (interleaved)
    // 16kb

    int outSamplesToWrite = ( ( Audio * )callbackData )->bytesToRead / 2;
    QVarLengthArray<short, 4096 * 2> inData( outSamplesToWrite );
    int framesRead = ( ( Audio * )callbackData )->audioBuf->read( ( char * )inData.data(), outSamplesToWrite * 2 ) / 4 ;
    qCDebug( phxAudio ) << "Read " << framesRead * 4 << "bytes from input.";
    src_short_to_float_array( inData.data(), *outBuf, framesRead );
    return framesRead;
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
