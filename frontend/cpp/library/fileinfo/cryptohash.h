#pragma once

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

            bool addData( const QString &filePath );

            QByteArray result() const;

        private:
            Hash mHash;
            ulong mCrc32Result;
            QByteArray mResult;
    };

}
