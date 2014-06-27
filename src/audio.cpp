
#include <unistd.h>

#include "audio.h"


Audio::Audio( QAudioFormat afmt, QObject * parent /*= 0*/ )
                        : QObject(parent), afmt(afmt), m_aio(0) {

        m_aio = new AudioIO();
        Q_CHECK_PTR(m_aio);
        this->moveToThread(&thread);
        connect(&thread, SIGNAL(started()), SLOT(threadStarted()));
}

void Audio::start( ) {
    thread.start();
}

void Audio::threadStarted( ) {
    aout = new QAudioOutput(afmt);
    aout->moveToThread(&thread);
    m_aio->moveToThread(&thread);
    connect(aout, SIGNAL(stateChanged(QAudio::State)), SLOT(stateChanged(QAudio::State)));
    usleep(700);
    aout->start(m_aio);
}

Audio::~Audio() {
    if(aout)
        delete aout;
    if(m_aio)
        delete m_aio;
}
