#include "cmdlineargs.h"
#include "logging.h"

#include <QFile>
#include <QFileInfo>

QVariantMap parseCommandLine( QCoreApplication &app ) {
    QCommandLineParser parser;
    parser.addOptions( {
        { "test", "Runs the UI in test mode" },
        { "libretro", "Use a Libretro core. Choose a core with -c and a game with -g" },
        { { "c", "core" }, "Set the Libretro core", "core path" },
        { { "g", "game" }, "Set the Libretro game", "game path" },
    } );
    parser.addVersionOption();
    parser.addHelpOption();
    parser.setApplicationDescription( "Phoenix - A multi-system emulator" );
    parser.process( app );

    QVariantMap map;

    if( parser.isSet( "test" ) ) {
        map[ "mainSrc" ] = "TestUI.qml";
    } else {
        map[ "mainSrc" ] = "Phoenix.qml";
    }

    // Libretro: Set up the map for Libretro if the option is passed
    if( parser.isSet( "libretro" ) ) {
        map[ "type" ] = "libretro";

        if( !parser.value( "core" ).isEmpty() ) {
            // Clean up the file path given from the user
            map[ "core" ] = QFileInfo( parser.value( "core" ) ).canonicalFilePath();
        } else {
            qWarning() << "ERROR: A game must be specified";
            parser.showHelp();
        }

        if( !parser.value( "game" ).isEmpty() ) {
            // Clean up the file path given from the user
            map[ "game" ] = QFileInfo( parser.value( "game" ) ).canonicalFilePath();
        } else {
            qWarning() << "ERROR: A game must be specified";
            parser.showHelp();
        }

        if( !QFileInfo::exists( parser.value( "core" ) ) ) {
            qWarning() << "ERROR: Core file does not exist!";
            parser.showHelp();
        }

        if( !QFileInfo::exists( parser.value( "game" ) ) ) {
            qWarning() << "ERROR: Game file does not exist!";
            parser.showHelp();
        }

        // Grab filename, use as title
        QFileInfo fileInfo( parser.value( "game" ) );
        map[ "title" ] = fileInfo.completeBaseName();
    } else if( parser.isSet( "core" ) || parser.isSet( "game" ) ) {
        // Libretro: Quit if there are Libretro arguments but the core type was not set to Libretro
        qWarning() << "ERROR: --libretro must be specified if you intend to use a Libretro core";
        parser.showHelp();
    }

    return map;
}
