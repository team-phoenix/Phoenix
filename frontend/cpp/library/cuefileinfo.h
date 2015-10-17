#ifndef CUEFILEINFO_H
#define CUEFILEINFO_H

#include "frontendcommon.h"

#include "gamefileinfo.h"

namespace Library {

    class CueFileInfo : public GameFileInfo {
    public:

        CueFileInfo( const QString &file );
        CueFileInfo( GameFileInfo &gameInfo );
        
        bool isValid();

    private:
        QStringList files();
    };

}


#endif // CUEFILEINFO_H
