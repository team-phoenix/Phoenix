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
                stringValue = "Game Boy";
                break;

            case Platforms::GBC:
                stringValue = "Game Boy Color";
                break;

            case Platforms::GBA:
                stringValue = "Game Boy Advance";
                break;

            case Platforms::NES:
                stringValue = "Nintendo Entertainment System";
                break;

            case Platforms::SNES:
                stringValue = "Super Nintendo";
                break;

            case Platforms::N64:
                stringValue = "Nintendo 64";
                break;

            case Platforms::PSX:
                stringValue = "Sony PlayStation";
                break;

            case Platforms::GENESIS:
                stringValue = "Sega Genesis";
                break;

            case Platforms::LYNX:
                stringValue = "Atari Lynx";
                break;

            case Platforms::WII:
                stringValue = "Nintendo Wii";
                break;

            case Platforms::GAMECUBE:
                stringValue = "Nintendo Gamecube";
                break;

            default:
                break;
        }

        return std::move( stringValue );
    }

}
