#include "phxpaths.h"

#include <QSettings>
#include <QStringBuilder>
#include <QDebug>

using namespace Library;

QString PhxPaths::mArtworkLocation = QStringLiteral( "" );
QString PhxPaths::mBiosLocation = QStringLiteral( "" );
QString PhxPaths::mSaveLocation = QStringLiteral( "" );

QString PhxPaths::mBinLocation = QStringLiteral( "" );
QString PhxPaths::mLibLocation = QStringLiteral( "" );
QString PhxPaths::mShareLocation = QStringLiteral( "" );
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
            p = PhxPaths::mLibLocation;
            break;
        case Locations::SaveLocation:
            p = PhxPaths::mSaveLocation;
            break;
        case Locations::ShareLocation:
            p = PhxPaths::mShareLocation;
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

QString PhxPaths::binLocation()
{
    return mBinLocation;
}

QString PhxPaths::libLocation()
{
    return mLibLocation;
}

QString PhxPaths::shareLocation()
{
    return mShareLocation;
}

void PhxPaths::createAllPaths() {

    PhxPaths::mBinLocation = QDir::currentPath();

#if defined( PHX_PORTABLE )
    PhxPaths::mLibLocation = PhxPaths::mBinLocation % QDir::separator() % QStringLiteral( "lib" );
    PhxPaths::mShareLocation = PhxPaths::mBinLocation % QDir::separator() % QStringLiteral( "share" );
#elif defined( PHX_INSTALLED )
    PhxPaths::mLibLocation = QStringLiteral( "/usr/local/lib/Phoenix" );
    PhxPaths::mShareLocation = QStringLiteral( "/usr/local/share/Phoenix" );
#endif

    PhxPaths::mBiosLocation = PhxPaths::mShareLocation % QDir::separator() % QStringLiteral( "bios" ) % QDir::separator();
    PhxPaths::mSaveLocation = PhxPaths::mShareLocation % QDir::separator() % QStringLiteral( "saves" ) % QDir::separator();
    PhxPaths::mArtworkLocation = PhxPaths::mShareLocation % QDir::separator() % QStringLiteral( "artwork" ) % QDir::separator();
    PhxPaths::mDatabaseLocation = PhxPaths::mShareLocation % QDir::separator() % QStringLiteral( "databases" ) % QDir::separator();

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

    if ( !databaseDir.exists() ) {
        databaseDir.mkpath( PhxPaths::mDatabaseLocation );
    }

}
