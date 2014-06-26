#include "audio.h"

Audio::Audio() {

    audio_output = NULL ;
    io_device = NULL ;
    sample_rate = 0;

}

void Audio::play( QByteArray data ) {

    if( !io_device )
        io_device = audio_output->start();
        
    QAudio::Error error = audio_output->error();
    
    switch( error ) {
        case QAudio::OpenError:
            qCritical() << "failed to open audio stream";
            break;
            
        case QAudio::IOError:
            qCritical() << "failed to read audio stream";
            break;
            
        case QAudio::FatalError:
            qFatal( "fatal audio error." );
            break;
            
        default:
            io_device->write( data );
            //qDebug() << "write data";
            if( error == QAudio::UnderrunError )
                underrun = true;
    }
}

void Audio::play( int16_t left, int16_t right ) {

    int16_t data[] = {left, right};
    play( QByteArray( ( const char * )data, sizeof( data ) ) );

}

void Audio::play( const int16_t *data, size_t frames ) {

    play( QByteArray::fromRawData( ( const char * )data, frames * sizeof( int16_t ) * 2 ) );

}

bool Audio::setSampleRate( double rate ) {

    QAudioFormat audio_format;
    qDebug() << "rate: " << rate;
    
    if( rate == sample_rate ) {
        qDebug() << "Audio Sample Rate: " << rate;
        return true;
    }
    
    sample_rate = rate;
    
    audio_format.setSampleSize( 16 );
    audio_format.setSampleRate( rate );
    audio_format.setChannelCount( 2 );
    audio_format.setSampleType( QAudioFormat::SignedInt );
    audio_format.setByteOrder( QAudioFormat::LittleEndian );
    audio_format.setCodec( "audio/pcm" );
    
    if( audio_output ) {
        audio_output->stop();
        audio_output->deleteLater();
        audio_output = NULL;
        io_device = NULL;
    }
    
    qDebug() << "default format" << QAudioDeviceInfo::defaultOutputDevice().supportedSampleRates();
    
    if( QAudioDeviceInfo::defaultOutputDevice().isFormatSupported( audio_format ) ) {
        audio_output = new QAudioOutput( audio_format );
        Q_CHECK_PTR( audio_output );
        qDebug() << "Sample rate set to" << rate;
    }
    else {
        double new_rate = QAudioDeviceInfo::defaultOutputDevice().preferredFormat().sampleRate();
        audio_format.setSampleRate( new_rate );
        audio_output = new QAudioOutput( audio_format /*Qobject parent*/ );
        Q_CHECK_PTR( audio_output );
        qDebug() << audio_format << "was not supported, defaulted to " << new_rate;
    }
    
    qCritical() << "16bit little endian audio/pcm @ " << rate << "hz not supported";
    underrun = false;
    return true;

}

bool Audio::running() {

    return true;

}

void Audio::unload() {

    if( audio_output ) {
        audio_output->stop();
        audio_output->deleteLater();
        audio_output = NULL;
        io_device = NULL; \
        qDebug() << "Audio closed";
        return;
    }
    
    qDebug() << "Audio wasn't loaded";

}
