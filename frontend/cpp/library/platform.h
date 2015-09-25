#ifndef PLATFORM_H
#define PLATFORM_H

#include <QString>
#include <QDir>
#include <QObject>
#include <QHash>

namespace Library {

    class Platform : public QObject {
        Q_OBJECT
    public:

        enum Platforms {
            BIOS = -1,
            UNKNOWN = 0,

            GB = 1,
            GBC = 2,

            GBA = 3,
            NES = 4,
            SNES = 5,
            N64 = 6,
            PSX = 7,

            NEOGEO,

          /*  GENESIS = 8,

            WII = 9,
            GAMECUBE = 10,
            LYNX = 11,
            NEOGEO = 12,
            */
            MAX,
        };

        enum AvailableCores {
            INVALID = -1,

            NESTOPIA,
            FCEUMM,
            BNES,

            SNES9X,
            BSNES_PERFORMANCE,
            BSNES_BALANCED,
            BSNES_ACCURACY,

            GAMBATTE,

            VBA_NEXT,
            VBAM,
            MGBA,

            MEDNAFEN_PSX,

            MUPEN64PLUS,

            MAX_CORE

        };

        enum DisplayMode {
            Fancy = 0,
            Ugly,
        };

        explicit Platform( QObject *parent = 0 );

        // Getters
        QString systemName() const;
        QString coreName() const;
        QString absoluteFilePath() const;

        // Static conversion functions
        static QString toString( const Platforms &platform );
        static QString toString( const AvailableCores &core, const DisplayMode &mode );
        static QString getCoreFilePath( const AvailableCores &core = INVALID );
        static QString toCoreName( const Platforms &platform );

        static Platform::AvailableCores toCore( const Platforms &platform );
        static Platform::Platforms toPlatform( const QString &extension );
        static Platform::Platforms checkHeaderString( const QString &headerString );

    private:
        QString qmlSystemName;
        QString qmlCoreName;
        QString qmlAbsoluteFilePath;

        QStringList mExtensionsList;

    };

}

#endif // PLATFORM_H
