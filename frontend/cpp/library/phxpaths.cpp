#include "phxpaths.h"

using namespace Library;

QString PhxPaths::mBiosLocation = QStringLiteral( "" );
QString PhxPaths::mSaveLocation = QStringLiteral( "" );
QString PhxPaths::mArtworkLocation = QStringLiteral( "" );
QString PhxPaths::mBinLocation = QStringLiteral( "" );
QString PhxPaths::mCoreLocation = QStringLiteral( "" );
QString PhxPaths::mResourceLocation = QStringLiteral( "" );
QString PhxPaths::mUserDataLocation = QStringLiteral( "" );
QString PhxPaths::mMetadataLocation = QStringLiteral( "" );

QString PhxPaths::biosLocation() {
    return mBiosLocation;
}

QString PhxPaths::saveLocation() {
    return mSaveLocation;
}

QString PhxPaths::artworkLocation() {
    return mArtworkLocation;
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

void PhxPaths::createAllPaths() {

    PhxPaths::mBinLocation = QCoreApplication::applicationDirPath();

    // Forward slash works on all platforms, Qt APIs convert if on Windows automatically
    QString sep = QStringLiteral( "/" );

    // Determine whether or not we're portable
    QString portableFilename = PhxPaths::mBinLocation % sep % "PHOENIX-PORTABLE";
    QFile file( portableFilename );
    bool portableMode = file.exists();

    if( portableMode ) {
        qCDebug( phxLibrary ) << "Portable mode";
        PhxPaths::mCoreLocation = PhxPaths::mBinLocation % sep % QStringLiteral( "cores" );
        PhxPaths::mResourceLocation = PhxPaths::mBinLocation;
        PhxPaths::mUserDataLocation = PhxPaths::mBinLocation % sep % QStringLiteral( "userdata" );
    }

    else {
        qCDebug( phxLibrary ) << "Installed mode";
#ifdef Q_OS_WIN32
        PhxPaths::mCoreLocation = QStringLiteral( "C:/Program Files/Libretro/Cores" );
        PhxPaths::mResourceLocation = PhxPaths::mBinLocation;
#endif
#ifdef Q_OS_MACX
        PhxPaths::mCoreLocation = QStringLiteral( "/usr/local/lib/libretro" );
        PhxPaths::mResourceLocation = PhxPaths::mBinLocation;
#endif

#ifdef Q_OS_LINUX
        PhxPaths::mCoreLocation = QStringLiteral( "/usr/lib/libretro" );
        PhxPaths::mResourceLocation = QStringLiteral( "/usr/local/share/Phoenix" );
#endif
        PhxPaths::mUserDataLocation = QStandardPaths::writableLocation( QStandardPaths::GenericDataLocation ) % QApplication::applicationName();
    }

    PhxPaths::mBiosLocation = PhxPaths::mResourceLocation % sep % QStringLiteral( "bios" ) % sep;
    PhxPaths::mSaveLocation = PhxPaths::mUserDataLocation % sep % QStringLiteral( "saves" ) % sep;
    PhxPaths::mArtworkLocation = PhxPaths::mResourceLocation % sep % QStringLiteral( "artwork" ) % sep;
    PhxPaths::mMetadataLocation = PhxPaths::mResourceLocation % sep % QStringLiteral( "metadata" ) % sep;

    QDir coreDir( PhxPaths::mCoreLocation );
    QDir biosDir( PhxPaths::mBiosLocation );
    QDir saveDir( PhxPaths::mSaveLocation );
    QDir artworkDir( PhxPaths::mArtworkLocation );
    QDir databaseDir( PhxPaths::mMetadataLocation );

    if( !coreDir.exists() ) {
        coreDir.mkpath( PhxPaths::mCoreLocation );
    }

    if( !biosDir.exists() ) {
        biosDir.mkpath( PhxPaths::mBiosLocation );
    }

    if( !saveDir.exists() ) {
        saveDir.mkpath( PhxPaths::mSaveLocation );
    }

    if( !artworkDir.exists() ) {
        artworkDir.mkpath( PhxPaths::mArtworkLocation );
    }

    if( !databaseDir.exists() ) {
        databaseDir.mkpath( PhxPaths::mMetadataLocation );
    }

}
