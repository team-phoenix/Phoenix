
#ifndef AUDIOIO_H
#define AUDIOIO_H

#include <QIODevice>
#include <QMutex>


class AudioIO : public QIODevice {
    Q_OBJECT

public:
    AudioIO( ) {
        open(QIODevice::ReadWrite);
        inbuf = new QByteArray;
        outbuf = new QByteArray;
        Q_CHECK_PTR(inbuf);
        Q_CHECK_PTR(outbuf);
    };
    virtual qint64 readData( char *data, qint64 maxSize ) Q_DECL_OVERRIDE;
    virtual qint64 writeData( const char *data, qint64 maxSize ) Q_DECL_OVERRIDE;

private:
    inline void cleanBuffer( QByteArray *, int sizeRead );
    QByteArray *inbuf;
    QByteArray *outbuf;
    QMutex bufmutex;
};

#endif
