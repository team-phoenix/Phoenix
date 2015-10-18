#include "cryptohash.h"
#include "zlib.h"

using namespace Library;

CryptoHash::CryptoHash( const Hash hash )
    : mHash( hash ),
      mCrc32Result( 0 )
{
}


void CryptoHash::addData( const char *data, const int length ) {
    switch( mHash ) {
    case Hash::Crc32: {
        auto newCalc = crc32( crc32( 0L, Z_NULL, 0 ), ( const uchar * )data, length );
        mCrc32Result = crc32_combine( mCrc32Result, newCalc, length );
        break;
    }
    case Hash::Sha1:
        break;
    default:
        Q_UNREACHABLE();
        break;
    }
}

void CryptoHash::addData(const QByteArray &data) {
    addData( data.constData(), data.length() );
}


bool CryptoHash::addData(QIODevice *device) {
    if (!device->isReadable() || !device->isOpen() ) {
        return false;
    }

    char buffer[1024];
    int length;

    while ((length = device->read(buffer, sizeof(buffer))) > 0) {
        addData(buffer, length);
    }

    return device->atEnd();
}

QByteArray CryptoHash::result() const
{

    QByteArray _result;
    switch( mHash ) {
    case Hash::Crc32:
        _result = QByteArray::number( static_cast<qulonglong>( mCrc32Result ), 16 );
        break;
    case Hash::Sha1:
        break;
    default:
        Q_UNREACHABLE();
        break;
    }

    return _result.toUpper();
}
