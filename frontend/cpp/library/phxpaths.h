#ifndef PHXPATHS_H
#define PHXPATHS_H

#include "frontendcommon.h"

#include "logging.h"

namespace Library {

    class PhxPaths: public QObject {
            Q_OBJECT

        public:
            explicit PhxPaths( QObject *parent = 0 );

            static bool portableMode();

            static QString binLocation();
            static QString resourceLocation();
            static QString coreLocation();
            static QString metadataLocation();

            static QString userDataLocation();
            static QString saveLocation();
            static QString firmwareLocation();
            static QString coverArtCacheLocation();

            Q_INVOKABLE bool qmlPortableMode();

            Q_INVOKABLE QString qmlBinLocation();
            Q_INVOKABLE QString qmlResourceLocation();
            Q_INVOKABLE QString qmlCoreLocation();
            Q_INVOKABLE QString qmlMetadataLocation();

            Q_INVOKABLE QString qmlUserDataLocation();
            Q_INVOKABLE QString qmlSaveLocation();
            Q_INVOKABLE QString qmlFirmwareLocation();
            Q_INVOKABLE QString qmlCoverArtCacheLocation();

            static void initPaths();

        private:

            // Portable mode, store ROM paths as relative paths
            static bool mPortableMode;

            // Read-only locations
            // These must already exist
            // Their contents can be written over when Phoenix needs to be updated
            static QString mBinLocation;
            static QString mResourceLocation;
            static QString mCoreLocation;
            static QString mMetadataLocation;

            // User data
            // These will be created if not already there
            // These can be written to and read from freely
            static QString mUserDataLocation;
            static QString mSaveLocation;
            static QString mFirmwareLocation;
            static QString mCoverArtCacheLocation;
    };

}

// Instantiate an singleton instance of PhxPaths for QML's use (needed to call static members)
QObject *PhxPathsSingletonProviderCallback( QQmlEngine *engine, QJSEngine *scriptEngine );

#endif // PHXPATHS_H
