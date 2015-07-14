#ifndef PLATFORMS
#define PLATFORMS

#include <QHash>
#include <QList>
#include <QPair>
#include <QString>
#include <QObject>

namespace Library {

    enum class Platforms {
        Bios = -1,
        Unknown = 0,

        GB = 1,
        GBC = 2,

        GBA = 3,
        NES = 4,
        SNES = 5,
        N64 = 6,
        PSX = 7,

        GENESIS = 8,

        WII = 9,
        GAMECUBE = 10,
        LYNX = 11,
        NEOGEO = 12,
        MAX = 13,
    };

    QString platformToString( const Platforms &value );

    // If ambiguous files extensions are used, use Platforms::Unknown.
    static const QHash<QString, QString> platformMap {
        { QStringLiteral( "sfc" ), platformToString( Platforms::SNES ) }, // BSNES
        { QStringLiteral( "smc" ), platformToString( Platforms::SNES )   }, // BSNES
        { QStringLiteral( "bml" ), platformToString( Platforms::SNES )   }, // BSNES

        { QStringLiteral( "nes" ), platformToString( Platforms::NES )   }, // Nestopia
        { QStringLiteral( "fds" ), platformToString( Platforms::NES )   }, // Nestopia
        { QStringLiteral( "unif" ), platformToString( Platforms::NES )   }, // Fceumm

        // Don't use these until it's actually supported by the frontend.
        //{ QStringLiteral( "n64" ), platformToString( Platforms::N64 )   }, // Mupen64plus
        //{ QStringLiteral( "z64" ), platformToString( Platforms::N64 )  }, // Mupen64plus
        //{ QStringLiteral( "v64" ), platformToString( Platforms::N64 )   }, // Mupen64plus

        { QStringLiteral( "gba" ), platformToString( Platforms::GBA )   }, // MGBA
        { QStringLiteral( "agb" ), platformToString( Platforms::GBA )  }, // MGBA
        { QStringLiteral( "gbz" ), platformToString( Platforms::GBA ) }, // MGBA

        { QStringLiteral( "ngp" ), platformToString( Platforms::NEOGEO ) }, // Mednafen NGP
        { QStringLiteral( "ngc" ), platformToString( Platforms::NEOGEO ) }, // Mednafen NGP

        { QStringLiteral( "lnx" ), platformToString( Platforms::LYNX ) }, // Handy

        { QStringLiteral( "cue" ), platformToString( Platforms::Unknown ) }, // Mednafen PSX, and others...

        { QStringLiteral( "bin" ), platformToString( Platforms::Unknown ) }, // Needed for bios files
    };

    class BinaryHeaderOffset {
        public:
            BinaryHeaderOffset( int _offset, int _length );

            int offset;
            int length;
    };

    // These offset values are obtained book looking at rom hacking sources.
    // Just google something like 'Sega Genesis rom header' and you'll
    // find some good sources.
    const QHash<QString, QList<BinaryHeaderOffset>> headerOffsets {
        {
            QStringLiteral( "bin" ), {
                BinaryHeaderOffset( 37664, 11 ),       // PlayStation Header
                BinaryHeaderOffset( 256, 15 ),         // Sega Genesis Header
            }
        },

        {
            QStringLiteral( "iso" ), {
                BinaryHeaderOffset( 24 , 4 ),             // Wii / GameCube Header
            }
        }
    };

    QString platformForHeaderString( const QByteArray &headerString );

}
#endif // PLATFORMS

