
#include "audioio.h"

#include <cstring>
#include <QThread>
#include <QDebug>


inline void AudioIO::cleanBuffer( QByteArray *ba, int size_read ) {
    if(ba->size() == size_read) {
        ba->clear();
    } else {
        // much less efficient, but we should mostly get complete reads
        ba->remove(0, size_read);
    }
}

qint64 AudioIO::readData( char *data, qint64 maxSize ) {
//    qDebug() << QThread::currentThread() << inbuf << outbuf;
    qint64 r;
    qint64 leftToRead = maxSize;;

    /* If there's any data left from output buffer, give it first */
    if(outbuf->size() > 0) {
//        qDebug() << QThread::currentThread() << "READ" << maxSize << outbuf->size();
        r = qMin<qint64>(outbuf->size(), leftToRead);
        if(r) {
            memcpy(data, outbuf->constData(), r);
            data += r;
            leftToRead -= r;
            cleanBuffer(outbuf, r);
        }
        /* If we gave as much data to consumer as possible, return */
        if(maxSize <= 0)
            return maxSize;
    }

    /* Else, Swap buffers and continue serving data from there */
    bufmutex.lock();
    qSwap(inbuf, outbuf);
    bufmutex.unlock();
//    qDebug() << QThread::currentThread() << "READ2" << maxSize << outbuf->size();
    r = qMin<qint64>(outbuf->size(), leftToRead);
    if(r) {
        memcpy(data, outbuf->constData(), r);
        leftToRead -= r;
        cleanBuffer(outbuf, r);
    }
    return maxSize-leftToRead;
}

qint64 AudioIO::writeData( const char *data, qint64 maxSize ) {
//    qDebug() << QThread::currentThread() << "WRITE" << maxSize << inbuf->size();
    QMutexLocker _l(&bufmutex);
    inbuf->append(data, maxSize);
    return maxSize;
}
