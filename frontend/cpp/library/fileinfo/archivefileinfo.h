#ifndef ARCHIEVEFILEINFO_H
#define ARCHIEVEFILEINFO_H

#include "frontendcommon.h"

#include "gamefileinfo.h"
#include "libretrodatabase.h"
#include "metadatadatabase.h"

namespace Library {

    class ArchiveFileInfo : public GameFileInfo {
        public:

            ArchiveFileInfo( const QString &file );
            ArchiveFileInfo( GameFileInfo &gameInfo );

            bool firstFile();
            bool nextFile();
            bool isValid() const;
            bool open( QuaZip::Mode mode );


            void close();

            QString nextFileName() const;
            static QString delimiter();

        private:
            QStringList mChecksums;
            QuaZip *mZipFile;
            QSqlQuery mLibretroQuery;
            bool mIsValid;

            void update();


    };

}

#endif // ARCHIEVEFILEINFO_H
