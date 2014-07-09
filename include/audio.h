
#ifndef AUDIO_H
#define AUDIO_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QAudioOutput>
#include <QDebug>

#include "audiobuffer.h"
#include "logging.h"


class Audio : public QObject {
    Q_OBJECT
public:
    Audio(QObject * = 0);
    virtual ~Audio();

    void start();
    void setFormat(QAudioFormat _afmt);

    AudioBuffer* abuf() const {
        return m_abuf;
    }

signals:
    void formatChanged();

public slots:
    void stateChanged(QAudio::State s) {
        if(s == QAudio::IdleState && aout->error() == QAudio::UnderrunError) {
            aio = aout->start();
        }
        if(s != QAudio::IdleState && s != QAudio::ActiveState) {
            qCDebug(phxAudio) << "State changed:" << s;
        }
    }

    void runChanged(bool isRunning);

private slots:
    void threadStarted();
    void handleFormatChanged();
    void handlePeriodTimer();

private:
    bool isRunning; // is core running
    QAudioFormat afmt;
    QAudioOutput *aout;
    QIODevice *aio;
    AudioBuffer *m_abuf;
    QThread thread;
    QTimer timer;
};

#endif
