
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

    this->moveToThread( &audioThread );
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

    qCDebug( phxAudio ) << "audioFormatIn" << audioFormatIn;
    qCDebug( phxAudio ) << "audioFormatOut" << audioFormatOut;
    qCDebug( phxAudio, "Using nearest format supported by sound card: %iHz %ibits",
             audioFormatOut.sampleRate(), audioFormatOut.sampleSize() );

    sampleRateRatio = ( double )audioFormatIn.sampleRate()  / audioFormatOut.sampleRate();

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
    qint64 durationInMs = audioFormatOut.durationForBytes( audioOut->periodSize() * 1.5 ) / 1000;
    qCDebug( phxAudio ) << "Timer interval set to" << durationInMs << "ms, Period size" << audioOut->periodSize() << "bytes, buffer size" << audioOut->bufferSize() << "bytes";
    audioTimer.setInterval( durationInMs );

    if( resamplerState ) {
        src_delete( resamplerState );
    }

    int errorCode;
    resamplerState = src_new( SRC_SINC_BEST_QUALITY, 2, &errorCode );

    if( !resamplerState ) {
        qCWarning( phxAudio ) << "libresample could not init: " << src_strerror( errorCode ) ;
    }

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

    // Handle the situation where there is no output to output to
    if( !audioOutIODev ) {
        static bool audioDevErrHandled = false;

        if( !audioDevErrHandled ) {
            qCDebug( phxAudio ) << "Audio device was not found, attempting reset...";
            emit signalFormatChanged();
            audioDevErrHandled = true;
        }

        return;
    }

    auto samplesPerFrame = 2;

    // Max number of bytes/frames we can write to the output
    auto outputBytesToWrite = audioOut->bytesFree();
    auto outputFramesToWrite = audioFormatOut.framesForBytes( outputBytesToWrite );
    auto outputSamplesToWrite = outputFramesToWrite * samplesPerFrame;

    // If output buffer is somehow full despite DRC, empty it
    if( !outputBytesToWrite ) {
        qWarning( phxAudio ) << "Output buffer full, resetting...";
        emit signalFormatChanged();
        return;
    }

    double maxDeviation = 0.005;
    auto outputBufferMidPoint = audioOut->bufferSize() / 2;
    auto distanceFromCenter = outputBytesToWrite - outputBufferMidPoint;
    double direction = ( double )distanceFromCenter / outputBufferMidPoint;
    double adjust = 1.0 + maxDeviation * -direction;

    // Compute the exact number of bytes to read from the circular buffer to
    // produce outputBytesToWrite bytes of output; Taking resampling and DRC into account
    double adjustedSampleRateRatio = sampleRateRatio * adjust;
    auto audioFormatTemp = audioFormatIn;
    audioFormatTemp.setSampleRate( audioFormatOut.sampleRate() * adjustedSampleRateRatio );
    auto inputBytesToRead = audioFormatTemp.bytesForDuration( audioFormatOut.durationForBytes( outputBytesToWrite ) );

    // qCDebug( phxAudio ) << "Input is" << ( audioBuf->size() * 100 / audioFormatIn.bytesForFrames( 4096 ) ) << "% full, output is"
    //                     << ( ( ( double )( audioOut->bufferSize() - outputBytesToWrite ) / audioOut->bufferSize() ) * 100 )  << "% full ; DRC:" << adjust
    //                     << ";" << inputBytesToRead << audioFormatIn.bytesForDuration( audioFormatOut.durationForBytes( outputBytesToWrite ) ) << sampleRateRatio << adjustedSampleRateRatio;

    // Read the input data
    QVarLengthArray<char, 4096 * 4> inputDataChar( inputBytesToRead );
    auto inputBytesRead = audioBuf->read( inputDataChar.data(), inputDataChar.size() );
    auto inputFramesRead = audioFormatIn.framesForBytes( inputBytesRead );
    auto inputSamplesRead = inputFramesRead * samplesPerFrame;

    // libsamplerate works in floats, must convert to floats for processing
    src_short_to_float_array( ( short * )inputDataChar.data(), inputDataFloat, inputSamplesRead );

    // Set up a struct containing parameters for the resampler
    float *outputDataFloat = new float[outputSamplesToWrite];
    SRC_DATA srcData;
    srcData.data_in = inputDataFloat;
    srcData.data_out = outputDataFloat;
    srcData.end_of_input = 0;
    srcData.input_frames = inputFramesRead;
    // Max size
    srcData.output_frames = outputFramesToWrite;
    srcData.src_ratio = adjustedSampleRateRatio;

    // Perform resample
    src_set_ratio( resamplerState, adjustedSampleRateRatio );
    auto errorCode = src_process( resamplerState, &srcData );

    if( errorCode ) {
        qCWarning( phxAudio ) << "libresample error: " << src_strerror( errorCode ) ;
    }

    auto outputFramesConverted = srcData.output_frames_gen;
    auto outputBytesConverted = audioFormatOut.bytesForFrames( outputFramesConverted );
    auto outputSamplesConverted = outputFramesConverted * samplesPerFrame;

    // Convert float data back to shorts
    short *outputDataShort = new short[outputSamplesConverted];
    src_float_to_short_array( outputDataFloat, outputDataShort, outputSamplesConverted );

    int outputBytesWritten = audioOutIODev->write( ( char * ) outputDataShort, outputBytesConverted );
    Q_UNUSED( outputBytesWritten );

    // qCDebug( phxAudio ) << "\tInput: needed" << inputBytesToRead << "bytes, read" << inputBytesRead << "bytes";
    // qCDebug( phxAudio ) << "\tOutput: needed" << outputBytesToWrite << "bytes, converted" << outputBytesConverted << "bytes, wrote" << outputBytesWritten << "bytes";
    // qCDebug( phxAudio ) << "Input: needed" << audioFormatIn.framesForBytes( inputBytesToRead ) << "frames, read" << audioFormatIn.framesForBytes( inputBytesRead ) << "frames";
    // qCDebug( phxAudio ) << "Output: needed" << audioFormatOut.framesForBytes( outputBytesToWrite ) << "frames, wrote" << audioFormatOut.framesForBytes( outputBytesWritten ) << "frames";

    // Cleanup
    delete outputDataFloat;
    delete outputDataShort;

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
        qWarning( phxAudio ) << "audioOut underrun";
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
    moveToThread( &audioThread );

    audioTimer.stop();

    if( audioOut ) {
        delete audioOut;
    }

    if( audioOutIODev ) {
        delete audioOutIODev;
    }
}
