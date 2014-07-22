

#include "audio.h"


Audio::Audio(QObject * parent /*= 0*/)
            : QObject(parent), isRunning(false),
              aout(0), aio(0), timer(this) {
        this->moveToThread(&thread);
        connect(&thread, SIGNAL(started()), SLOT(threadStarted()));
        thread.setObjectName("phoenix-audio");

        m_abuf = new AudioBuffer();
        Q_CHECK_PTR(m_abuf);
        //connect(m_abuf, SIGNAL(hasPeriodSize()), this, SLOT(handleHasPeriodSize()));

        timer.moveToThread(&thread);
        connect(&timer, SIGNAL(timeout()), this, SLOT(handlePeriodTimer()));

        // we need send this signal to ourselves
        connect(this, SIGNAL(formatChanged()), this, SLOT(handleFormatChanged()));
}

void Audio::start() {
    thread.start(QThread::HighestPriority);
}

/* This needs to be called on the audio thread*/
void Audio::setFormat(QAudioFormat _afmt) {
    qCDebug(phxAudio, "setFormat(%iHz %ibits)", _afmt.sampleRate(), _afmt.sampleSize());
/*    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(_afmt)) {
        qCWarning(phxAudio) << "Audio format not supported by output device !";
        return;
    }*/
    afmt = _afmt;
    emit formatChanged();
}

void Audio::handleFormatChanged() {
    if(aout) {
        aout->stop();
        delete aout;
    }
    aout = new QAudioOutput(afmt);
//    aout->moveToThread(&thread);
    Q_CHECK_PTR(aout);
    connect(aout, SIGNAL(stateChanged(QAudio::State)), SLOT(stateChanged(QAudio::State)));
    aio = aout->start();
    timer.setInterval(afmt.durationForBytes(aout->periodSize() * 1.5) / 1000);
    if(!isRunning) {
        aout->suspend();
    }
}

void Audio::threadStarted() {
//    qDebug() << QThread::currentThread() << &thread;
    if(!afmt.isValid()) {
        // we don't have a valid audio format yet...
        return;
    }
    aout = new QAudioOutput(afmt);
    Q_CHECK_PTR(aout);
    aout->moveToThread(&thread);
    connect(aout, SIGNAL(stateChanged(QAudio::State)), SLOT(stateChanged(QAudio::State)));
    aio = aout->start();
    aout->suspend(); // always start suspended
    timer.setInterval(afmt.durationForBytes(aout->periodSize() * 1.5) / 1000);
    aio->moveToThread(&thread);
}

void Audio::handlePeriodTimer() {
    Q_ASSERT(QThread::currentThread() == &thread);
    Q_ASSERT(aio);
    int toWrite = aout->bytesFree();
    if(!toWrite)
        return;

    QVarLengthArray<char, 4096*4> tmpbuf(toWrite);
    int read = m_abuf->read(tmpbuf.data(), toWrite);
    int wrote = aio->write(tmpbuf.data(), read);
    Q_UNUSED(wrote);
}

void Audio::runChanged( bool _isRunning ) {
    isRunning = _isRunning;
    if(!aout)
        return;
    if(!isRunning) {
        if(aout->state() != QAudio::SuspendedState) {
            qCDebug(phxAudio) << "Paused";
            aout->suspend();
            timer.stop();
        }
    } else {
        if(aout->state() != QAudio::ActiveState) {
            qCDebug(phxAudio) << "Started";
            aout->resume();
            timer.start();
        }
    }
}

void Audio::setVolume(qreal level) {
    if (aout)
        aout->setVolume(level);
}

Audio::~Audio() {
    if(aout)
        delete aout;
    if(aio)
        delete aio;
    if(m_abuf)
        delete m_abuf;
}
