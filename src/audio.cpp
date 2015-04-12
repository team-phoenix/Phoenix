
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

    QAudioDeviceInfo info( QAudioDeviceInfo::defaultOutputDevice() );

    audioFormatIn = newInFormat;
    audioFormatOut = info.nearestFormat( newInFormat ); // try using the nearest supported format

    if( audioFormatOut.sampleRate() < audioFormatIn.sampleRate() ) {
        // If that got us a format with a worse sample rate, use preferred format
        audioFormatOut = info.preferredFormat();
    }

    qCDebug( phxAudio ) << audioFormatOut;
    qCDebug( phxAudio, "Using nearest format supported by sound card: %iHz %ibits",
             audioFormatOut.sampleRate(), audioFormatOut.sampleSize() );

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
    audioTimer.setInterval( audioFormatOut.durationForBytes( audioOut->periodSize() ) / 1000 );

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
    int audioOutBufBytesFree = audioOut->bytesFree();

    if( !audioOutBufBytesFree ) {
        return;
    }

    // Calculate DRC info
    qreal deviation = 0.005;
    int audioOutBufMidPoint = audioOut->bufferSize() / 2;

    // Positive if buffer is more than half-full, negative if less
    int drift = audioOutBufBytesFree - audioOutBufMidPoint;
    qreal direction = ( qreal )drift / audioOutBufMidPoint;
    qreal pitchStretch = 1.0 + deviation * -direction;

    // Compute the exact number of bytes to read from the circular buffer to
    // produce toWrite bytes of output; Taking resampling and DRC into account
    double outDRCRatio = sampleRateRatio * pitchStretch;
    auto audioFormatDRC = audioFormatIn;
    audioFormatDRC.setSampleRate( audioFormatOut.sampleRate() * outDRCRatio );

    // Take the delta between the output buffer's mid point and its current size from the input buffer,
    // and resample it to slightly more or slightly less than the output sample rate
    audioInBytesNeeded = audioFormatDRC.bytesForDuration( audioFormatOut.durationForBytes( drift ) );

    qCDebug( phxAudio ) << ( ( ( double )( audioOut->bufferSize() - audioOutBufBytesFree ) / audioOut->bufferSize() ) * 100 ) << "% full ; DRC:" << pitchStretch
                        << ";" << audioInBytesNeeded << audioFormatIn.bytesForDuration( audioFormatOut.durationForBytes( audioOutBufBytesFree ) );

    // Preform conversion
    long framesConverted = src_callback_read( resamplerState, outDRCRatio, audioInBytesNeeded / 4,  convertedDataFloat );

    // Convert float data back to shorts
    src_float_to_short_array( convertedDataFloat, convertedDataShort, 4096 );

    int errorCode;

    if( !framesConverted && ( errorCode = src_error( resamplerState ) ) ) {

        qCWarning( phxAudio ) << "libresample error: " << src_strerror( errorCode ) ;
    }

    // Send audio data to output device
    int bytesWritten = audioOutIODev->write( ( char * )convertedDataShort, audioInBytesNeeded );
    qCDebug( phxAudio ) << "Wrote" << bytesWritten << "bytes to output.";
    //Q_UNUSED( bytesWritten );

}

long Audio::getAudioData( void *callbackData, float **outBuf ) {
    // 16-bit stereo PCM
    // 4096 frames
    // 8192 samples (interleaved)
    // 16kb

    // Convert the first parameter to avoid having to typecast all the time
    Audio *audioClass = ( ( Audio * )callbackData );

    // Figure out how many samples we need to grab
    int outSamplesToWrite = audioClass->audioInBytesNeeded / 2;

    // Read frames from the audio buffer
    QVarLengthArray<short, 4096 * 2> inData( outSamplesToWrite );
    int framesRead = audioClass->audioBuf->read( ( char * )inData.data(), outSamplesToWrite * 2 ) / 4 ;
    qCDebug( phxAudio ) << "Read" << framesRead * 4 << "bytes from input.";

    // libsamplerate works in floats, must convert to floats for processing
    src_short_to_float_array( inData.data(), audioClass->tempDataFloat, framesRead );
    *outBuf = audioClass->tempDataFloat;
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
