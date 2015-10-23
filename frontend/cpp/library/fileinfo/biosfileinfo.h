#ifndef BIOSFILEINFO_H
#define BIOSFILEINFO_H

#include "frontendcommon.h"

#include "libretrodatabase.h"
#include "gamefileinfo.h"

namespace Library {

    class BiosFileInfo : public GameFileInfo {
        public:

            BiosFileInfo( const QString &file );
            BiosFileInfo( GameFileInfo &gameInfo );

        private:
            QString mBiosName;

    };

}

#endif // BIOSFILEINFO_H
