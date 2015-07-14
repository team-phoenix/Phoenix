#ifndef PHXPATHS_H
#define PHXPATHS_H

#include <QStandardPaths>
#include <QDir>

namespace Library {

    class PhxPaths {
        public:
            static QString biosLocation();

            static QString saveLocation();

            static QString artworkLocation();

            static void CreateAllPaths();

        private:
            PhxPaths();

            static QString mBiosLocation;
            static QString mSaveLocation;
            static QString mArtworkLocation;


    };

}

#endif // PHXPATHS_H
