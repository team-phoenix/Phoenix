#include <platforms.h>

namespace Library {

    QString platformForHeaderString( const QByteArray &headerString ) {
        QString platform;

        if( headerString == "504c415953544154494f4e" ) { // PLAYSTATION
            platform = platformToString( Platforms::PSX );
        }

        else if( headerString == "53454741204d454741204452495645"  // SEGA MEGA DRIVE
                 || headerString == "534547412047454e45534953" ) {  // SEGA GENESIS
            platform = platformToString( Platforms::GENESIS );
        }

        else if( headerString == "5d1c9ea3" ) {                    // Wii ID
            platform = platformToString( Platforms::WII );
        } else if( headerString == "C2339F3D" ) {                  // Gamecube ID
            platform = platformToString( Platforms::GAMECUBE );
        }

        return std::move( platform );
    }

    BinaryHeaderOffset::BinaryHeaderOffset( int _offset, int _length ) {
        offset = _offset;
        length = _length;
    }

    QString platformToString( const Platforms &value ) {

        QString stringValue;

        switch( value ) {
            case Platforms::Unknown:
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

            default:
                break;
        }

        return std::move( stringValue );
    }

}
