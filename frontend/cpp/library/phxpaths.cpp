#include "phxpaths.h"

using namespace Library;

bool PhxPaths::mPortableMode = false;
QString PhxPaths::mBiosLocation = QStringLiteral( "" );
QString PhxPaths::mSaveLocation = QStringLiteral( "" );
QString PhxPaths::mCoverArtCacheLocation = QStringLiteral( "" );
QString PhxPaths::mBinLocation = QStringLiteral( "" );
QString PhxPaths::mCoreLocation = QStringLiteral( "" );
QString PhxPaths::mResourceLocation = QStringLiteral( "" );
QString PhxPaths::mUserDataLocation = QStringLiteral( "" );
QString PhxPaths::mMetadataLocation = QStringLiteral( "" );

PhxPaths::PhxPaths( QObject *parent ): QObject( parent ) {
    setObjectName( "PhxPaths" );
}

bool PhxPaths::portableMode() {
    return mPortableMode;
}

QString PhxPaths::biosLocation() {
    return mBiosLocation;
}

QString PhxPaths::saveLocation() {
    return mSaveLocation;
}

QString PhxPaths::coverArtCacheLocation() {
    return mCoverArtCacheLocation;
}

QString PhxPaths::binLocation() {
    return mBinLocation;
}

QString PhxPaths::coreLocation() {
    return mCoreLocation;
}

QString PhxPaths::resourceLocation() {
    return mResourceLocation;
}

QString PhxPaths::userDataLocation() {
    return mUserDataLocation;
}

QString PhxPaths::metadataLocation() {
    return mMetadataLocation;
}

bool PhxPaths::qmlPortableMode() {
    return portableMode();
}

QString PhxPaths::qmlBiosLocation() {
    return PhxPaths::biosLocation();
}

QString PhxPaths::qmlSaveLocation() {
    return PhxPaths::saveLocation();
}

QString PhxPaths::qmlCoverArtCacheLocation() {
    return PhxPaths::coverArtCacheLocation();
}

QString PhxPaths::qmlBinLocation() {
    return PhxPaths::binLocation();
}

QString PhxPaths::qmlCoreLocation() {
    return PhxPaths::coreLocation();
}

QString PhxPaths::qmlResourceLocation() {
    return PhxPaths::resourceLocation();
}

QString PhxPaths::qmlUserDataLocation() {
    return PhxPaths::userDataLocation();
}

QString PhxPaths::qmlMetadataLocation() {
    return PhxPaths::metadataLocation();
}

void PhxPaths::createAllPaths() {

    PhxPaths::mBinLocation = QCoreApplication::applicationDirPath();

    // Determine whether or not we're portable
    QString portableFilename = PhxPaths::mBinLocation % '/' % "PHOENIX-PORTABLE";
    QFile file( portableFilename );
    mPortableMode = file.exists();

    if( mPortableMode ) {
        qCDebug( phxLibrary ) << "Portable mode";
        PhxPaths::mResourceLocation = PhxPaths::mBinLocation;
        PhxPaths::mCoreLocation = PhxPaths::mBinLocation % '/' % QStringLiteral( "Cores" );
        PhxPaths::mMetadataLocation = PhxPaths::mBinLocation % '/' % QStringLiteral( "Metadata" ) % '/';

        PhxPaths::mUserDataLocation = PhxPaths::mBinLocation % '/' % QStringLiteral( "User Data" );
        PhxPaths::mCoverArtCacheLocation = PhxPaths::mUserDataLocation % '/' % QStringLiteral( "Cover Art Cache" );

        // Create a core and game folder if they do not exist
        QDir coreDir( PhxPaths::mCoreLocation );

        if( !coreDir.exists() ) {
            coreDir.mkpath( coreDir.path() );
        }

        // Create a game folder if one does not exist
        QDir gameDir( PhxPaths::mUserDataLocation % '/' % QStringLiteral( "Games" ) );

        if( !gameDir.exists() ) {
            gameDir.mkpath( gameDir.path() );
        }
    }

    else {
        qCDebug( phxLibrary ) << "Installed mode";

#ifdef Q_OS_WIN
        PhxPaths::mResourceLocation = PhxPaths::mBinLocation;
        // TODO: Make this work on all languages
        PhxPaths::mCoreLocation = QStringLiteral( "C:/Program Files/Libretro/Cores" );
        PhxPaths::mMetadataLocation = PhxPaths::mResourceLocation % '/' % QStringLiteral( "Metadata" ) % '/';

        PhxPaths::mUserDataLocation = QStandardPaths::writableLocation( QStandardPaths::AppLocalDataLocation ) % '/' % QApplication::applicationName();
#endif
#ifdef Q_OS_MACX
        PhxPaths::mResourceLocation = PhxPaths::mBinLocation;
        PhxPaths::mCoreLocation = QStringLiteral( "/usr/local/lib/libretro" );
        PhxPaths::mMetadataLocation = PhxPaths::mResourceLocation % '/' % QStringLiteral( "Metadata" ) % '/';

        PhxPaths::mUserDataLocation = QStandardPaths::writableLocation( QStandardPaths::AppLocalDataLocation );
#endif

#ifdef Q_OS_LINUX
        PhxPaths::mResourceLocation = PhxPaths::mBinLocation % '/' % QStringLiteral( ".." ) % '/' % QStringLiteral( "share/phoenix" );
        PhxPaths::mCoreLocation = QStringLiteral( "/usr/lib/libretro" );
        PhxPaths::mMetadataLocation = PhxPaths::mResourceLocation % '/' % QStringLiteral( "Metadata" ) % '/';

        PhxPaths::mUserDataLocation = QStandardPaths::writableLocation( QStandardPaths::AppLocalDataLocation );
#endif

    }

    PhxPaths::mSaveLocation = PhxPaths::mUserDataLocation % '/' % QStringLiteral( "Saves" ) % '/';
    PhxPaths::mBiosLocation = PhxPaths::mUserDataLocation % '/' % QStringLiteral( "BIOS" ) % '/';

    QDir userDir( PhxPaths::mUserDataLocation );
    QDir saveDir( PhxPaths::mSaveLocation );
    QDir biosDir( PhxPaths::mBiosLocation );
    QDir coverArtCacheDir( PhxPaths::mCoverArtCacheLocation );

    if( !userDir.exists() ) {
        userDir.mkpath( userDir.path() );
    }

    if( !saveDir.exists() ) {
        saveDir.mkpath( saveDir.path() );
    }

    if( !biosDir.exists() ) {
        biosDir.mkpath( biosDir.path() );
    }

    if( !coverArtCacheDir.exists() ) {
        coverArtCacheDir.mkpath( coverArtCacheDir.path() );
    }

    // Create the log directory, too
    QDir logDir( PhxPaths::userDataLocation() % '/' % QStringLiteral( "Logs" ) );

    if( !logDir.exists() ) {
        logDir.mkpath( logDir.path() );
    }

}

// Instantiate PhxPaths for QML use as a singleton object
// FIXME: Need to destroy it somehow?
QObject *PhxPathsSingletonProviderCallback( QQmlEngine *engine, QJSEngine *scriptEngine ) {
    Q_UNUSED( scriptEngine )

    Library::PhxPaths *phxPaths = new Library::PhxPaths( engine );

    return phxPaths;
}
