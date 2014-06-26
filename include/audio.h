#ifndef AUDIO_H
#define AUDIO_H

#include <QByteArray>
#include <QAudioOutput>
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QIODevice>

#include "core.h"

class Audio {
public:
    Audio();
    void play( QByteArray data );
    void play( int16_t left, int16_t right );
    void play( const int16_t *data, size_t frames );
    
    bool setSampleRate( double );
    void unload();
    bool running();
    bool underrun;
    
private:
    QAudioOutput *audio_output;
    QIODevice *io_device;
    double sample_rate;
};

#endif // AUDIO_H
