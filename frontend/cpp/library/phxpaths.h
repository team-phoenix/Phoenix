#ifndef PHXPATHS_H
#define PHXPATHS_H

#include <QtCore>
#include <QtWidgets>
#include <QtQml>

#include "logging.h"

namespace Library {

    class PhxPaths: public QObject {
        Q_OBJECT

        public:
            explicit PhxPaths( QObject *parent = 0 );
            static QString biosLocation();
            static QString saveLocation();
            static QString artworkLocation();
            static QString binLocation();
            static QString coreLocation();
            static QString resourceLocation();
            static QString userDataLocation();
            static QString metadataLocation();

            Q_INVOKABLE QString qmlBiosLocation();
            Q_INVOKABLE QString qmlSaveLocation();
            Q_INVOKABLE QString qmlArtworkLocation();
            Q_INVOKABLE QString qmlBinLocation();
            Q_INVOKABLE QString qmlCoreLocation();
            Q_INVOKABLE QString qmlResourceLocation();
            Q_INVOKABLE QString qmlUserDataLocation();
            Q_INVOKABLE QString qmlMetadataLocation();

            static void createAllPaths();

        private:

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

QObject *PhxPathsSingletonProviderCallback( QQmlEngine *engine, QJSEngine *scriptEngine );

#endif // PHXPATHS_H
