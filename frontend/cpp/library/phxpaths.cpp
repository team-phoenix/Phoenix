#include "phxpaths.h"

#include <QSettings>
#include <QStringBuilder>
#include <QDebug>

using namespace Library;

QString PhxPaths::mArtworkLocation = QStringLiteral( "" );
QString PhxPaths::mBiosLocation = QStringLiteral( "" );
QString PhxPaths::mSaveLocation = QStringLiteral( "" );

QString PhxPaths::mBinLocation = QStringLiteral( "" );
QString PhxPaths::mCoreLocation = QStringLiteral( "" );
QString PhxPaths::mResourceLocation = QStringLiteral( "" );
QString PhxPaths::mDatabaseLocation = QStringLiteral( "" );

QString PhxPaths::path( const Locations location ) {
    QString p;

    switch( location ) {
        case Locations::ArtworkLocation:
            p = PhxPaths::mArtworkLocation;
            break;

        case Locations::DatabasesLocation:
            p = PhxPaths::mDatabaseLocation;
            break;

        case Locations::BiosLocation:
            p = PhxPaths::mBiosLocation;
            break;

        case Locations::ExecutableLocation:
            p = PhxPaths::mBinLocation;
            break;

        case Locations::LibLocation:
            p = PhxPaths::mCoreLocation;
            break;

        case Locations::SaveLocation:
            p = PhxPaths::mSaveLocation;
            break;

        case Locations::ShareLocation:
            p = PhxPaths::mResourceLocation;
            break;

        default:
            break;
    }

    return std::move( p );
}

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

QString PhxPaths::libLocation() {
    return mCoreLocation;
}

QString PhxPaths::shareLocation() {
    return mResourceLocation;
}

void PhxPaths::createAllPaths() {

    PhxPaths::mBinLocation = QDir::currentPath();

#if defined( PHX_PORTABLE )
    PhxPaths::mCoreLocation = PhxPaths::mBinLocation % QDir::separator() % QStringLiteral( "lib" );
    PhxPaths::mResourceLocation = PhxPaths::mBinLocation % QDir::separator() % QStringLiteral( "resources" );

#elif defined( PHX_INSTALLED )
#ifdef Q_OS_WIN32
    PhxPaths::mCoreLocation = QStringLiteral( "C:/Program Files/Libretro/Cores" );
#endif
#ifdef Q_OS_MACX
    PhxPaths::mCoreLocation = QStringLiteral( "/usr/local/lib/libretro" );
#endif

#ifdef Q_OS_LINUX
    PhxPaths::mCoreLocation = QStringLiteral( "/usr/lib/libretro" );
    PhxPaths::mResourceLocation = QStringLiteral( "/usr/local/share/Phoenix" );
#endif
#endif

    PhxPaths::mBiosLocation = PhxPaths::mResourceLocation % QDir::separator() % QStringLiteral( "bios" ) % QDir::separator();
    PhxPaths::mSaveLocation = PhxPaths::mResourceLocation % QDir::separator() % QStringLiteral( "saves" ) % QDir::separator();
    PhxPaths::mArtworkLocation = PhxPaths::mResourceLocation % QDir::separator() % QStringLiteral( "artwork" ) % QDir::separator();
    PhxPaths::mDatabaseLocation = PhxPaths::mResourceLocation % QDir::separator() % QStringLiteral( "databases" ) % QDir::separator();

    QDir biosDir( PhxPaths::mBiosLocation );
    QDir saveDir( PhxPaths::mSaveLocation );
    QDir artworkDir( PhxPaths::mArtworkLocation );
    QDir databaseDir( PhxPaths::mDatabaseLocation );

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
        databaseDir.mkpath( PhxPaths::mDatabaseLocation );
    }

}
