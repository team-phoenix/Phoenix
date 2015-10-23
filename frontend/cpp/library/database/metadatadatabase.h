#ifndef METADATADATABASE_H
#define METADATADATABASE_H

#include "frontendcommon.h"

#include "phxpaths.h"
#include "logging.h"

namespace Library {

    // GameMetaData is used to set in metadata for any game during;
    // this usually used after the append process.
    struct GameMetaData {
        QString artworkUrl;
        QString goodToolsCode;
        QString region;
        QString developer;
        QString releaseDate;
        QString genre;
        QString description;
        QString title;
        QString filePath;
        QString identifier;
        qreal progress;
        bool updated;
    };

    class MetaDataDatabase {
        public:
            static const QString tableRoms;
            static const QString tableSystems;
            static const QString tableReleases;
            static const QString tableRegions;


            static QSqlDatabase database();

            static void open();

        private:
            MetaDataDatabase();

    };

}

#endif // METADATADATABASE_H
