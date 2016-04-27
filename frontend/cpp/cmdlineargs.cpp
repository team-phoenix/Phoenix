#include "cmdlineargs.h"

#include <QFile>
#include <QFileInfo>
#include <QDebug>

QVariantMap parseCommandLine( QCoreApplication &app ) {
    QCommandLineParser parser;
    parser.addOptions( {
        { { "c", "core" }, "Set the Libretro core", "Core path" },
        { { "g", "game" }, "Set the Libretro game", "Game path" },
        { "libretro", "Run a game in Libretro mode. Choose a core with -c and a game with -g" }
    } );
    parser.addVersionOption();
    parser.addHelpOption();
    parser.setApplicationDescription( "Phoenix - A multi-system emulator" );
    parser.process( app );

    QVariantMap map;

    // Set source based on type given, quit if none is set
    if( parser.isSet( "libretro" ) ) {
        map[ "type" ] = "libretro";

        if( !parser.value( "core" ).isEmpty() ) {
            map[ "core" ] = parser.value( "core" );
        } else {
            qWarning() << "A game must be specified";
            parser.showHelp();
        }

        if( !parser.value( "game" ).isEmpty() ) {
            map[ "game" ] = parser.value( "game" );
        } else {
            qWarning() << "A game must be specified";
            parser.showHelp();
        }

        if( !QFile::exists( parser.value( "core" ) ) ) {
            qWarning() << "Core file does not exist!";
            parser.showHelp();
        }

        if( !QFile::exists( parser.value( "game" ) ) ) {
            qWarning() << "Game file does not exist!";
            parser.showHelp();
        }

        // Grab filename, use as title
        QFileInfo fileInfo( parser.value( "game" ) );
        map[ "title" ] = fileInfo.completeBaseName();
    } else {
        qWarning() << "A mode must be specified";
        parser.showHelp();
    }

    return map;
}
