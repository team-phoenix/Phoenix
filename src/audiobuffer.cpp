
#include <QDebug>
#include <audiobuffer.h>

AudioBuffer::AudioBuffer() {
    b1 = new QByteArray;
    b2 = new QByteArray;
    this->open(QIODevice::ReadWrite);
}

AudioBuffer::~AudioBuffer() {
    delete b1;
    delete b2;
}

qint64 AudioBuffer::readData( char *data, qint64 maxlen ) {
    if(b2->size() == 0 && b1->size() > 0) {
        std::swap(b1, b2);
        b1->clear();
    }
    qint64 s_read = std::min<qint64>(b2->size(), maxlen);
    qDebug() << "hit1" << s_read << "sur" << maxlen;
    memcpy(data, b2->data(), s_read);
    b2->remove(0, s_read);
    if(b2->size() == 0) {
        std::swap(b1, b2);
        b1->clear();
    }
    return s_read;
}

qint64 AudioBuffer::writeData( const char *data, qint64 len ) {
//    qDebug() << "hit2" << len << b1->size();
    b1->append(data, len);
    return len;
}

