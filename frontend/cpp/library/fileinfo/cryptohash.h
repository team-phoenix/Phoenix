#ifndef CRYPTOHASH_H
#define CRYPTOHASH_H

#include "frontendcommon.h"

namespace Library {

    class CryptoHash {
        public:
            enum Hash {
                Crc32 = 0,
                Sha1,
            };

            CryptoHash( const Hash hash );

            void addData( const char *data, const int length );

            void addData( const QByteArray &data );

            bool addData( QIODevice *device );

            bool addData( const QString &filePath ) {
                QFile file( filePath );
                if ( file.open( QIODevice::ReadOnly ) ) {
                    bool _result = addData( &file );
                    file.close();

                    return _result;
                }

                return false;
            }

            QByteArray result() const;

        private:
            Hash mHash;
            ulong mCrc32Result;
            QByteArray mResult;
    };

}


#endif // CRYPTOHASH_H
