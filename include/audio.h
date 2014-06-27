
#ifndef AUDIO_H
#define AUDIO_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QAudioOutput>
#include "audioio.h"

class Audio : public QObject {
    Q_OBJECT
public:
    Audio( QAudioFormat, QObject * = 0 );
    ~Audio();

    void start( );

    AudioIO* aio() const {
        return m_aio;
    }

public slots:
    void stateChanged(QAudio::State s) {
        qDebug() << "state:" << s << " " <<aout->error();
        if(aout->error() == QAudio::UnderrunError) {
            aout->start(m_aio);
        }
    }

private slots:
    void threadStarted( );

private:
    QAudioFormat afmt; // owned
    QAudioOutput *aout;
    AudioIO *m_aio;
    QThread thread;
};

#endif
