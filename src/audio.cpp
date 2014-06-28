
#include "audio.h"


Audio::Audio( QObject * parent /*= 0*/ )
                        : QObject(parent),
                          isRunning(false),
                          aout(0),
                          m_aio(0) {

        m_aio = new AudioIO();
        Q_CHECK_PTR(m_aio);
        this->moveToThread(&thread);
        connect(&thread, SIGNAL(started()), SLOT(threadStarted()));
        // we need send this signal to ourselves
        connect(this, SIGNAL(formatChanged()), this, SLOT(handleFormatChanged()));
}

void Audio::start( ) {
    thread.start();
}

/* This needs to be called on the audio thread*/
void Audio::setFormat( QAudioFormat _afmt ) {
    // TODO: perform checking...
    afmt = _afmt;
    emit formatChanged();
}

void Audio::handleFormatChanged( ) {
    if(aout) {
        aout->stop();
        delete aout;
    }
    aout = new QAudioOutput(afmt);
//    aout->moveToThread(&thread);
    Q_CHECK_PTR(aout);
    connect(aout, SIGNAL(stateChanged(QAudio::State)), SLOT(stateChanged(QAudio::State)));
    aout->start(m_aio);
    if(!isRunning) {
        aout->suspend();
    }
}

void Audio::threadStarted( ) {
//    qDebug() << QThread::currentThread() << &thread;
    if(!afmt.isValid()) {
        // we don't have a valid audio format yet...
        return;
    }
    aout = new QAudioOutput(afmt);
    Q_CHECK_PTR(aout);
    aout->moveToThread(&thread);
    m_aio->moveToThread(&thread);
    connect(aout, SIGNAL(stateChanged(QAudio::State)), SLOT(stateChanged(QAudio::State)));
    aout->start(m_aio);
    aout->suspend(); // always start suspended
}

void Audio::runChanged( bool _isRunning ) {
    isRunning = _isRunning;
    if(!aout)
        return;
    if(!isRunning) {
        if(aout->state() != QAudio::SuspendedState)
            aout->suspend();
    } else {
        if(aout->state() != QAudio::ActiveState)
            aout->resume();
    }
}

Audio::~Audio() {
    if(aout)
        delete aout;
    if(m_aio)
        delete m_aio;
}
