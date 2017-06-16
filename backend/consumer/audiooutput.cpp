#include "audiooutput.h"
#include "logging.h"

#include <QAudioOutput>
#include <QTimer>

// FIXME: Stop assuming stereo?
const auto samplesPerFrame = 2;
// const auto framesPerSample = 0.5;

AudioOutput::AudioOutput( Node *parent ): Node( parent ),
    outputBuffer( this ) {

    outputBuffer.start();
}

AudioOutput::~AudioOutput() {
    if( outputAudioInterface != nullptr || outputDataFloat != nullptr || outputDataShort != nullptr ) {
        shutdown();
    }
}

// Public slots

void AudioOutput::commandIn( Node::Command command, QVariant data, qint64 timeStamp ) {
    emit commandOut( command, data, timeStamp );

    switch( command ) {
        case Command::Play: {
            qCDebug( phxAudioOutput ) << command;
            state = State::Playing;
            pipelineStateChanged();
            break;
        }

        case Command::Stop: {
            qCDebug( phxAudioOutput ) << command;
            state = State::Stopped;
            pipelineStateChanged();
            break;
        }

        case Command::Load: {
            qCDebug( phxAudioOutput ) << command;
            state = State::Loading;
            pipelineStateChanged();
            break;
        }

        case Command::Pause: {
            qCDebug( phxAudioOutput ) << command;
            state = State::Paused;
            pipelineStateChanged();
            break;
        }

        case Command::Unload: {
            qCDebug( phxAudioOutput ) << command;
            state = State::Unloading;
            pipelineStateChanged();
            shutdown();
            break;
        }

        // For debugging. Prints number of heartbeats it gets each second (it can only check how much time has passed
        // each time it gets a heartbeat)
        case Command::Heartbeat: {
            static int counter = 0;
            static qint64 secondLater = QDateTime::currentMSecsSinceEpoch() + 1000;

            counter++;

            if( timeStamp > secondLater ) {
                //qCDebug( phxAudioOutput ) << counter;
                counter = 0;
                secondLater = timeStamp + 1000;
            }

            break;
        }

        case Command::SetHostFPS: {
            hostFPS = data.toReal();
            qCDebug( phxAudioOutput ).nospace() << "hostFPS = " << hostFPS << "fps, vsync: " << vsync;
            break;
        }

        case Command::SetCoreFPS: {
            coreFPS = data.toReal();
            qCDebug( phxAudioOutput ).nospace() << "coreFPS = " << coreFPS << "fps, vsync: " << vsync;
            break;
        }

        case Command::SetVolume: {
            if( outputAudioInterface ) {
                outputAudioInterface->setVolume( data.toReal() );
            }

            break;
        }

        case Command::SetVsync: {
            vsync = data.toBool();
            qCDebug( phxAudioOutput ).nospace() << "vsync: " << vsync << ", coreFPS: " << coreFPS << "Hz, hostFPS: " << hostFPS << "Hz";
            break;
        }

        // Attempt to find suitable output format for given input sample rate, reload audio output and reset buffers
        case Command::SetSampleRate: {
            sampleRate = data.toInt();

            // qCDebug( phxAudioOutput, "Init audio: %i Hz, %ffps (core), %ffps (host)", sampleRate, coreFPS, hostFPS );
            qCDebug( phxAudioOutput ) << "sampleRate" << sampleRate;

            inputAudioFormat.setSampleSize( 16 );
            inputAudioFormat.setSampleRate( sampleRate );
            inputAudioFormat.setChannelCount( 2 );
            inputAudioFormat.setSampleType( QAudioFormat::SignedInt );
            inputAudioFormat.setByteOrder( QAudioFormat::LittleEndian );
            inputAudioFormat.setCodec( "audio/pcm" );

            // Try using the nearest supported format
            QAudioDeviceInfo info( QAudioDeviceInfo::defaultOutputDevice() );
            outputAudioFormat = info.nearestFormat( inputAudioFormat );

            // If that got us a format with a worse sample rate or different endianness, use preferred format
            if( ( outputAudioFormat.sampleRate() <= inputAudioFormat.sampleRate() )
                || ( outputAudioFormat.byteOrder() != QAudioFormat::LittleEndian ) ) {
                outputAudioFormat = info.preferredFormat();
            }

            // Force 16-bit audio (for now)
            outputAudioFormat.setSampleSize( 16 );

            sampleRateRatio = static_cast<qreal>( outputAudioFormat.sampleRate() ) / inputAudioFormat.sampleRate();

            qCDebug( phxAudioOutput ) << "audioFormatIn" << inputAudioFormat;
            qCDebug( phxAudioOutput ) << "audioFormatOut" << outputAudioFormat;
            qCDebug( phxAudioOutput ) << "sampleRateRatio" << sampleRateRatio;
            qCDebug( phxAudioOutput, "Using nearest format supported by sound card: %iHz %ibits",
                     outputAudioFormat.sampleRate(), outputAudioFormat.sampleSize() );

            resetAudio();
            allocateMemory();

            outputLengthMs = static_cast<int>( outputAudioFormat.durationForBytes( outputAudioInterface->bufferSize() ) ) / 1000;
            break;
        }

        default:
            break;
    }
}

void AudioOutput::dataIn( Node::DataType type, QMutex *mutex, void *data, size_t bytes, qint64 timeStamp ) {
    if( type == DataType::Audio ) {
        if( state == State::Playing ) {
            qint64 currentTime = QDateTime::currentMSecsSinceEpoch();

            // Discard data that's too far from the past to matter anymore
            if( currentTime - timeStamp > 500 ) {
                static qint64 lastMessage = QDateTime::currentMSecsSinceEpoch();

                if( currentTime - lastMessage > 1000 ) {
                    lastMessage = currentTime;
                    // qCWarning( phxAudioOutput ) << "Discarding" << bytes << "bytes of old audio data from" <<
                    //   currentTime - timestamp << "ms ago";
                }

                return;
            }

            // Make a copy so the data won't be changed later
            mutex->lock();

            short *inputData = *reinterpret_cast<short **>( data );

            if( inputData ) {
                memcpy( inputDataShort, inputData, bytes );
            }

            mutex->unlock();

            // Handle the situation where there is an error opening the audio device
            if( outputAudioInterface->error() == QAudio::OpenError ) {
                // qWarning( phxAudioOutput ) << "QAudio::OpenError, attempting reset...";
                resetAudio();
            }

            int inputBytes = static_cast<int>( bytes );
            int inputFrames = inputAudioFormat.framesForBytes( inputBytes );
            int inputSamples = inputFrames * samplesPerFrame;

            // What do we have to work with?
            int outputTotalBytes = outputAudioFormat.bytesForDuration( outputLengthMs * 1000 );
            outputCurrentByte = static_cast<int>( outputBuffer.bytesAvailable() );
            int outputFreeBytes = outputTotalBytes - outputCurrentByte;
            int outputFreeFrames = outputAudioFormat.framesForBytes( outputFreeBytes );
            int outputFreeSamples = outputFreeFrames * samplesPerFrame;
            Q_UNUSED( outputTotalBytes );
            Q_UNUSED( outputFreeSamples );

            // Calculate how much the read data should be scaled (shrunk or stretched) to keep the buffer on target
            // The vector is negative if beyond the target and positive if behind
            int outputTargetByte = outputAudioFormat.bytesForDuration( outputTargetMs * 1000 );
            int outputEstimatedBytes = outputAudioFormat.bytesForDuration( inputAudioFormat.durationForBytes( inputBytes ) );
            int outputVectorTargetToCurrent = outputTargetByte - outputCurrentByte + outputEstimatedBytes;
            // double unclampedDRCScale = ( double )outputVectorTargetToCurrent / outputTargetByte;
            double unclampedDRCScale = ( static_cast<double>( outputVectorTargetToCurrent ) + outputEstimatedBytes ) / outputEstimatedBytes;

            // Calculate the final DRC ratio
            double DRCScale = qMax( -maxDeviation, qMin( unclampedDRCScale, maxDeviation ) );
            double hostRatio = vsync ? hostFPS / coreFPS : 1.0;
            double adjustedSampleRateRatio = sampleRateRatio * ( 1.0 + DRCScale ) * hostRatio;

            // libsamplerate works in floats, must convert to floats for processing
            src_short_to_float_array( inputDataShort, inputDataFloat, inputSamples );

            // Set up a struct containing parameters for the resampler
            SRC_DATA srcData;
            srcData.data_in = inputDataFloat;
            srcData.data_out = outputDataFloat;
            srcData.end_of_input = 0;
            srcData.input_frames = inputFrames;
            srcData.output_frames = outputFreeFrames; // Max size
            srcData.src_ratio = adjustedSampleRateRatio;

            // Perform resample
            src_set_ratio( resamplerState, adjustedSampleRateRatio );
            int errorCode = src_process( resamplerState, &srcData );

            if( errorCode ) {
                qCWarning( phxAudioOutput ) << "libresample error: " << src_strerror( errorCode ) ;
            }

            int outputFramesConverted = static_cast<int>( srcData.output_frames_gen );
            int outputBytesConverted = outputAudioFormat.bytesForFrames( outputFramesConverted );
            int outputSamplesConverted = outputFramesConverted * samplesPerFrame;

            // Convert float data back to shorts
            src_float_to_short_array( outputDataFloat, outputDataShort, outputSamplesConverted );

            // Send the converted data out
            int outputBytesWritten = static_cast<int>( outputBuffer.write( reinterpret_cast<char *>( outputDataShort ), outputBytesConverted ) );
            outputCurrentByte += outputBytesWritten;
            //#define DRC_LOGGING
#if defined( DRC_LOGGING )
            static qint64 lastMessage = 0;

            qint64 inputBytesPerMSec = inputAudioFormat.bytesForDuration( 1000 );
            qint64 outputBytesPerMSec = outputAudioFormat.bytesForDuration( 1000 );

            if( currentTime - lastMessage > 1000 ) {
                lastMessage = currentTime;
                qCDebug( phxAudioOutput ) << "Input:" << inputBytes / inputBytesPerMSec << "ms";
                qCDebug( phxAudioOutput ) << "hostFps:" << hostFPS << "coreFPS:" << coreFPS;
                qCDebug( phxAudioOutput ) << "Output is" << ( ( ( double )( ( outputTotalBytes - outputFreeBytes ) ) /
                                          outputTotalBytes ) * 100 )
                                          << "% full," << ( outputTotalBytes - outputFreeBytes ) / outputBytesPerMSec << "ms (target:" << outputTargetMs << "ms /"
                                          << ( ( double )outputTargetMs / outputLengthMs ) * 100 << "%)";
                qCDebug( phxAudioOutput ) << "\tunclampedDRCScale =" << unclampedDRCScale << "DRCScale =" << DRCScale
                                          << "outputAudioInterface->bufferSize()" << outputAudioInterface->bufferSize() / outputBytesPerMSec << "ms";
                qCDebug( phxAudioOutput ) << "\toutputTotalBytes =" << outputTotalBytes / outputBytesPerMSec << "ms"
                                          << "outputCurrentByte =" << outputCurrentByte / outputBytesPerMSec << "ms"
                                          << " outputFreeBytes =" << outputFreeBytes / outputBytesPerMSec << "ms";
                qCDebug( phxAudioOutput ) << "\tOutput buffer is" << outputLengthMs
                                          << "ms, target =" << outputTargetMs
                                          << "ms (" << ( double )100.0 * ( ( double )outputTargetMs / (
                                                      ( double )( outputAudioFormat.durationForBytes(
                                                              outputTotalBytes ) ) / 1000.0 ) )
                                          << "%)";
                qCDebug( phxAudioOutput ) << "\tOutput: needed" << outputVectorTargetToCurrent / outputBytesPerMSec << "ms, wrote"
                                          << outputBytesWritten / outputBytesPerMSec << "ms";
                qCDebug( phxAudioOutput ) << "\toutputTargetByte =" << outputTargetByte / outputBytesPerMSec << "ms"
                                          << "outputVectorTargetToCurrent =" << outputVectorTargetToCurrent / outputBytesPerMSec << "ms";
                qCDebug( phxAudioOutput ) << "\toutputAudioInterface->bufferSize() =" << outputAudioInterface->bufferSize() / outputBytesPerMSec << "ms"
                                          << "outputAudioInterface->bytesFree() =" << outputAudioInterface->bytesFree() / outputBytesPerMSec << "ms"
                                          << "outputBuffer.bytesToWrite() =" << outputBuffer.bytesToWrite() / outputBytesPerMSec << "ms";
                qCDebug( phxAudioOutput ) << "\toutputBuffer.bytesAvailable() =" << outputBuffer.bytesAvailable() / outputBytesPerMSec << "ms"
                                          << "outputBuffer.size() =" << outputBuffer.size() / outputBytesPerMSec << "ms"
                                          << "outputBuffer.pos() =" << outputBuffer.pos() / outputBytesPerMSec << "ms";
                // qCDebug( phxAudioOutput ) << "\tState:" << outputAudioInterface->state()
                //   << " error: " << outputAudioInterface->error();
            }

#endif
        }
    }

    emit dataOut( type, mutex, data, bytes, timeStamp );
}

// Private slots

void AudioOutput::outputStateChanged( QAudio::State outputState ) {
    if( outputState == QAudio::IdleState && outputAudioInterface->error() == QAudio::UnderrunError ) {
        // Schedule the audio to be reset at some point in the future, giving the buffer some time to fill
        QTimer::singleShot( 50, this, &AudioOutput::outputUnderflow );
    }

    if( outputState == QAudio::SuspendedState ) {
        qCDebug( phxAudioOutput ) << "Output state changed:" << outputState;
    }
}

void AudioOutput::outputUnderflow() {
    if( outputAudioInterface && state == State::Playing ) {
        // qCWarning( phxAudioOutput ) << "Audio buffer underflow";
        outputAudioInterface->start( &outputBuffer );
    }

    else if( !outputAudioInterface ) {
        qCWarning( phxAudioOutput ) << "Audio buffer underflow (resetting)";
        resetAudio();
        allocateMemory();
    }
}

// Private

void AudioOutput::pipelineStateChanged() {
    if( !outputAudioInterface ) {
        return;
    }

    if( !( state == State::Playing ) ) {
        if( outputAudioInterface->state() != QAudio::SuspendedState ) {
            qCDebug( phxAudioOutput ) << "Paused";
            outputAudioInterface->suspend();
        }
    }

    else {
        if( outputAudioInterface->state() != QAudio::ActiveState ) {
            qCDebug( phxAudioOutput ) << "Started";
            outputAudioInterface->resume();
        }
    }
}

void AudioOutput::shutdown() {
    qCDebug( phxAudioOutput ) << "shutdown() start";

    if( outputAudioInterface ) {
        outputAudioInterface->stop();
        delete outputAudioInterface;
        outputAudioInterface = nullptr;
    }

    if( inputDataShort ) {
        delete [] inputDataShort;
        inputDataShort = nullptr;
    }

    if( inputDataFloat ) {
        delete [] inputDataFloat;
        inputDataFloat = nullptr;
    }

    if( outputDataFloat ) {
        delete [] outputDataFloat;
        outputDataFloat = nullptr;
    }

    if( outputDataShort ) {
        delete [] outputDataShort;
        outputDataShort = nullptr;
    }

    outputCurrentByte = 0;
    outputBuffer.clear();

    qCDebug( phxAudioOutput ) << "shutdown() end";
}

void AudioOutput::resetAudio() {
    // Reset the resampler

    if( resamplerState ) {
        src_delete( resamplerState );
        resamplerState = nullptr;
    }

    int errorCode;
    resamplerState = src_new( SRC_SINC_BEST_QUALITY, 2, &errorCode );

    if( !resamplerState ) {
        qCWarning( phxAudioOutput ) << "libresample could not init: " << src_strerror( errorCode ) ;
    }

    // Reset the output interface object

    if( outputAudioInterface ) {
        outputAudioInterface->stop();
        delete outputAudioInterface;
        outputAudioInterface = nullptr;
    }

    outputAudioInterface = new QAudioOutput( outputAudioFormat, this );
    Q_CHECK_PTR( outputAudioInterface );

    connect( outputAudioInterface, &QAudioOutput::stateChanged, this, &AudioOutput::outputStateChanged );
    outputAudioInterface->start( &outputBuffer );
    // QAudio::State state = outputAudioInterface->state();
    // qCDebug( phxAudioOutput ) << state;

    if( !( state == State::Playing ) ) {
        outputAudioInterface->suspend();
    }
}

void AudioOutput::allocateMemory() {
    // Some cores may give as much as 4 video frames' worth of audio data in a single video frame period, so we need to one-up them
    const static int bufferSizeInVideoFrames = 30;
    int inputBufferSamples = samplesPerFrame * inputAudioFormat.framesForDuration( static_cast<qint64>( ( 1000.0 / coreFPS ) * bufferSizeInVideoFrames * 1000.0 ) );
    int outputBufferSamples = samplesPerFrame * outputAudioFormat.framesForDuration( outputLengthMs * 1000 );

    qCDebug( phxAudioOutput ) << "Allocating" <<
                              static_cast<double>(
                                  static_cast<int>( sizeof( float ) ) * ( inputBufferSamples + outputBufferSamples ) +
                                  static_cast<int>( sizeof( short ) ) * ( inputBufferSamples + outputBufferSamples )
                              )
                              / 1024.0 << "KB for resampling";
    qCDebug( phxAudioOutput ).nospace() << "Input buffer samples: " << inputBufferSamples <<
                                        ", output buffer samples: " << outputBufferSamples;

    if( inputDataShort ) {
        delete [] inputDataShort;
        inputDataShort = nullptr;
    }

    if( inputDataFloat ) {
        delete [] inputDataFloat;
        inputDataFloat = nullptr;
    }

    if( outputDataFloat ) {
        delete [] outputDataFloat;
        outputDataFloat = nullptr;
    }

    if( outputDataShort ) {
        delete [] outputDataShort;
        outputDataShort = nullptr;
    }

    inputDataShort = new short[ inputBufferSamples ]();
    inputDataFloat = new float[ inputBufferSamples ]();
    outputDataFloat = new float[ outputBufferSamples ]();
    outputDataShort = new short[ outputBufferSamples ]();
}

