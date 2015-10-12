#ifndef PHXPATHS_H
#define PHXPATHS_H

#include <QtCore>
#include <QtWidgets>

#include "logging.h"

namespace Library {

    class PhxPaths {
        public:
            static QString biosLocation();
            static QString saveLocation();
            static QString artworkLocation();
            static QString binLocation();
            static QString coreLocation();
            static QString resourceLocation();
            static QString userDataLocation();
            static QString metadataLocation();

            static void createAllPaths();

        private:
            PhxPaths();

            static QString mBiosLocation;
            static QString mSaveLocation;
            static QString mArtworkLocation;
            static QString mBinLocation;
            static QString mCoreLocation;
            static QString mResourceLocation;
            static QString mUserDataLocation;
            static QString mMetadataLocation;
    };

}

#endif // PHXPATHS_H
