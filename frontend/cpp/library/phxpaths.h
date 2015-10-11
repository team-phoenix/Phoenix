#ifndef PHXPATHS_H
#define PHXPATHS_H

#include <QStandardPaths>
#include <QDir>

namespace Library {

    class PhxPaths {
        public:

            enum Locations {
                ExecutableLocation,
                SaveLocation,
                ArtworkLocation,
                LibLocation,
                ShareLocation,
                BiosLocation,
                DatabasesLocation,
            };
            Q_ENUMS( Locations )

            static QString path( const Locations location );

            static QString biosLocation();
            static QString saveLocation();
            static QString artworkLocation();
            static QString binLocation();
            static QString libLocation();
            static QString shareLocation();

            static void createAllPaths();


        private:
            PhxPaths();

            static QString mBiosLocation;
            static QString mSaveLocation;
            static QString mArtworkLocation;
            static QString mBinLocation;
            static QString mCoreLocation;
            static QString mResourceLocation;
            static QString mDatabaseLocation;
    };

}

#endif // PHXPATHS_H
