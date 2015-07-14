#include "phxpaths.h"

#include <QSettings>

using namespace Library;

QString PhxPaths::mArtworkLocation = QStringLiteral( "" );
QString PhxPaths::mBiosLocation = QStringLiteral( "" );
QString PhxPaths::mSaveLocation = QStringLiteral( "" );

QString PhxPaths::biosLocation() {
    return mBiosLocation;
}

QString PhxPaths::saveLocation() {
    return mSaveLocation;
}

QString PhxPaths::artworkLocation() {
    return mArtworkLocation;
}

void PhxPaths::CreateAllPaths() {
    auto cacheLocation = QStandardPaths::writableLocation( QStandardPaths::CacheLocation )
                         + QDir::separator();

    PhxPaths::mBiosLocation = cacheLocation + QStringLiteral( "Bios" ) + QDir::separator();
    PhxPaths::mSaveLocation = cacheLocation + QStringLiteral( "Saves" ) + QDir::separator();
    PhxPaths::mArtworkLocation = cacheLocation + QStringLiteral( "Artwork" ) + QDir::separator();

    QDir biosDir( PhxPaths::mBiosLocation );
    QDir saveDir( PhxPaths::mSaveLocation );
    QDir artworkDir( PhxPaths::mArtworkLocation );

    if( !biosDir.exists() ) {
        biosDir.mkpath( PhxPaths::mBiosLocation );
    }

    if( !saveDir.exists() ) {
        saveDir.mkpath( PhxPaths::mSaveLocation );
    }

    if( !artworkDir.exists() ) {
        artworkDir.mkpath( PhxPaths::mArtworkLocation );
    }

}
