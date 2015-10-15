#ifndef CUEFILEINFO_H
#define CUEFILEINFO_H

#include "gamefileinfo.h"

#include <QFile>

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
