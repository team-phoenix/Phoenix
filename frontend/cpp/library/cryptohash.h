#ifndef CRYPTOHASH_H
#define CRYPTOHASH_H

#include <QIODevice>

namespace Library {

    class CryptoHash
    {
    public:
        enum Hash {
            Crc32 = 0,
            Sha1,
        };

        CryptoHash( const Hash hash );

        void addData( const char *data, const int length );

        void addData( const QByteArray &data );

        bool addData( QIODevice *device );

        QByteArray result() const;

    private:
        Hash mHash;
        ulong mCrc32Result;
        QByteArray mResult;
    };

}


#endif // CRYPTOHASH_H
