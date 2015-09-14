#include "platform.h"
#include <QSettings>
#include <QDebug>

using namespace Library;

Platform::Platform(QObject *parent)
    : QObject( parent )
{

}

QString Platform::systemName() const
{
    return qmlSystemName;
}

QString Platform::coreName() const {
    return qmlCoreName;
}

QString Platform::absoluteFilePath() const {
    return qmlAbsoluteFilePath;
}

QString Library::Platform::toString (const Library::Platform::Platforms &platform ) {
    QString stringValue;

    switch( platform ) {
    case Platforms::UNKNOWN:
        break;

    case Platforms::GB:
        stringValue = QStringLiteral( "Game Boy" );
        break;

    case Platforms::GBC:
        stringValue = QStringLiteral( "Game Boy Color" );
        break;

    case Platforms::GBA:
        stringValue = QStringLiteral( "Game Boy Advance" );
        break;

    case Platforms::NES:
        stringValue = QStringLiteral( "Nintendo Entertainment System" );
        break;

    case Platforms::SNES:
        stringValue = QStringLiteral( "Super Nintendo" );
        break;

    case Platforms::N64:
        stringValue = QStringLiteral( "Nintendo 64" );
        break;

    case Platforms::PSX:
        stringValue = QStringLiteral( "Sony PlayStation" );
        break;

    /*
    case Platforms::GENESIS:
        stringValue = QStringLiteral( "Sega Genesis" );
        break;

    case Platforms::LYNX:
        stringValue = QStringLiteral( "Atari Lynx" );
        break;

    case Platforms::WII:
        stringValue = QStringLiteral( "Nintendo Wii" );
        break;

    case Platforms::GAMECUBE:
        stringValue = QStringLiteral( "Nintendo Gamecube" );
        break;

    case Platforms::NEOGEO:
        stringValue = QStringLiteral( "Neo Geo ");
        break;
    */
    default:
        break;
    }

    return std::move( stringValue );
}

Platform::AvailableCores Platform::toCore( const Platform::Platforms &platform ) {
    auto core = AvailableCores::INVALID;

    switch( platform ) {
    case GB:
    case GBC:
        core = GAMBATTE;
        break;
    case GBA:
        core = VBAM;
        break;
    case NES:
        core = FCEUMM;
        break;
    case SNES:
        core = SNES9X;
        break;
    case PSX:
        core = MEDNAFEN_PSX;
        break;
    case N64:
        core = MUPEN64PLUS;
        break;

    default:
        break;
    }

    return std::move( core );
}

QString Platform::toCoreName( const Platform::Platforms &platform ) {

    auto core = Platform::toCore( platform );

    Q_ASSERT( core != AvailableCores::INVALID );

    auto mode = DisplayMode::Fancy;
    return toString( core, mode );

}

Platform::Platforms Platform::toPlatform( const QString &extension ) {
    auto platform = Platform::Platforms::UNKNOWN;

    if ( extension == QStringLiteral( "sfc" )
         || extension == QStringLiteral( "smc" )
         || extension == QStringLiteral( "bml" ) ) {
        platform = Platform::Platforms::SNES;
    } else if ( extension == QStringLiteral( "nes" )
                || extension == QStringLiteral( "fds" )
                || extension == QStringLiteral( "unif" ) ) {
        platform = Platform::Platforms::NES;
    } else if ( extension == QStringLiteral( "gba" )
                || extension == QStringLiteral( "agb" )
                || extension == QStringLiteral( "gbz" )  ) {
        platform = Platform::Platforms::GBA;
    } else if ( extension == QStringLiteral( "bin" ) ) {
        platform == Platform::Platforms::BIOS;
    }

    Q_ASSERT( platform != Platform::Platforms::UNKNOWN );
    return platform;


}

Platform::Platforms Platform::checkHeaderString(const QString &headerString) {
    auto platform = Platform::Platforms::UNKNOWN;

    if( headerString == "504c415953544154494f4e" ) { // PLAYSTATION
        platform = Platform::Platforms::PSX;
    }

    else if( headerString == "53454741204d454741204452495645"  // SEGA MEGA DRIVE
             || headerString == "534547412047454e45534953" ) {  // SEGA GENESIS
        //platform = Platforms::GENESIS;
    }

    else if( headerString == "5d1c9ea3" ) {                    // Wii ID
        //platform = platformToString( Platforms::WII );
    } else if( headerString == "C2339F3D" ) {                  // Gamecube ID
        //platform = platformToString( Platforms::GAMECUBE );
    }

    Q_ASSERT( platform != Platform::Platforms::UNKNOWN );

    return std::move( platform );
}

QString Platform::getCoreFilePath( const Platform::AvailableCores &core )
{
    if ( core == Platform::AvailableCores::INVALID ) {
        return QStringLiteral( "" );
    }

    QString path;
    QString extension;
#ifdef Q_OS_MACX
    path = QStringLiteral( "/usr/local/lib/libretro" );
    extension = QStringLiteral( ".dylib" );
#endif

#ifdef Q_OS_LINUX
    path = QStringLiteral( "/usr/lib/libretro" );
    extension = QStringLiteral( ".so" );
#endif

#ifdef Q_OS_WIN32
    path = QStringLiteral( "C:/Program Files/Libretro/Cores" );
    extension = QStringLiteral( ".dll" );
#endif

    path += QDir::separator() + toString( core, Platform::DisplayMode::Ugly ) + extension;

    return path;

}

QString Platform::toString( const Platform::AvailableCores &core, const Platform::DisplayMode &mode ) {
    QString coreName;

    const bool isFancyMode = (mode == Platform::DisplayMode::Fancy);

    switch( core ) {
    case NESTOPIA:
        coreName = isFancyMode ? QStringLiteral( "Nestopia" ) : QStringLiteral( "nestopia_libretro" );
        break;
    case BNES:
        coreName = isFancyMode ? QStringLiteral( "bNES" ) : QStringLiteral( "bnes_libretro" );
        break;
    case FCEUMM:
        coreName = isFancyMode ? QStringLiteral( "Fceumm" ) : QStringLiteral( "fceumm_libretro" );
        break;
    case SNES9X:
        coreName = isFancyMode ? QStringLiteral( "Snes9x" ) : QStringLiteral( "snes9x_libretro" );
        break;
    case BSNES_PERFORMANCE:
        coreName = isFancyMode ? QStringLiteral( "bSNES Performance" ) : QStringLiteral( "bsnes_performance_libretro" );
        break;
    case BSNES_BALANCED:
        coreName = isFancyMode ? QStringLiteral( "bSNES Balanced" ) : QStringLiteral( "bsnes_balanced_libretro" );
        break;
    case BSNES_ACCURACY:
        coreName = isFancyMode ? QStringLiteral( "bSNES Accuracy" ) : QStringLiteral( "bsnes_accuracy_libretro" );
        break;
    case GAMBATTE:
        coreName = isFancyMode ? QStringLiteral( "Gambatte" ) : QStringLiteral( "gambatte_libretro" );
        break;
    case VBA_NEXT:
        coreName = isFancyMode ? QStringLiteral( "Vba Next" ) : QStringLiteral( "vba_next_libretro" );
        break;
    case VBAM:
        coreName = isFancyMode ? QStringLiteral( "Vbam" ) : QStringLiteral( "vbam_libretro" );
        break;
    case MGBA:
        coreName = isFancyMode ? QStringLiteral( "mGba" ) : QStringLiteral( "mgba_libretro" );
        break;
    case MEDNAFEN_PSX:
        coreName = isFancyMode ? QStringLiteral( "Mednafen PlayStation" ) : QStringLiteral( "mednafen_psx_Libretro" );
        break;
    case MUPEN64PLUS:
        coreName = isFancyMode ? QStringLiteral( "Mupen64Plus" ) : QStringLiteral( "mupen64plus_libretro" );
        break;
    default:
        break;
    }

    return std::move( coreName );
}

