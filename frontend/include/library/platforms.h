#ifndef PLATFORMS
#define PLATFORMS

#include <QHash>
#include <QList>
#include <QPair>
#include <QString>

namespace Library {

    enum class Platforms {
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
        { "sfc" ,  platformToString( Platforms::SNES ) }, // BSNES
        { "smc" , platformToString( Platforms::SNES )   }, // BSNES
        { "bml" , platformToString( Platforms::SNES )   }, // BSNES

        { "nes" , platformToString( Platforms::NES )   },  // Nestopia
        { "fds" , platformToString( Platforms::NES )   },  // Nestopia
        { "unif" , platformToString( Platforms::NES )   }, // Fceumm

        // Don't use these until it's actually supported by the frontend.
        //{ "n64" , platformToString( Platforms::N64 )   }, // Mupen64plus
        //{ "z64" , platformToString( Platforms::N64 )  }, // Mupen64plus
        //{ "v64" , platformToString( Platforms::N64 )   }, // Mupen64plus

        { "gba" , platformToString( Platforms::GBA )   }, // MGBA
        { "agb" , platformToString( Platforms::GBA )  }, // MGBA
        { "gbz" , platformToString( Platforms::GBA ) }, // MGBA

        { "ngp" , platformToString( Platforms::NEOGEO ) }, // Mednafen NGP
        { "ngc" , platformToString( Platforms::NEOGEO ) }, // Mednafen NGP

        { "lnx" , platformToString( Platforms::LYNX ) }, // Handy

        { "cue", platformToString( Platforms::Unknown ) }, // Mednafen PSX, and others...

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
    const QHash<QString, QList<BinaryHeaderOffset>> headerOffsets    {
        {
            "bin", {
                BinaryHeaderOffset( 37664, 11 ),       // PlayStation Header
                BinaryHeaderOffset( 256, 15 ),         // Sega Genesis Header
            }
        },

        {
            "iso", {
                BinaryHeaderOffset( 24 , 4 ),             // Wii / GameCube Header
            }
        }
    };

    QString platformForHeaderString( const QByteArray &headerString );

}
#endif // PLATFORMS

