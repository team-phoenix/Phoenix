import qbs 1.0

/*Project {
    name: "phoenix_backend";

    StaticLibrary {
        name: "phoenix_backend"
        Depends { name: "cpp" }
        Depends { name: "Qt"; submodules: [ "qml", "quick", "multimedia" ]; }


        Group { qbs.install: true; fileTagsFilter: product.type;}

        files: [

            // C++ HEADERS
            "backendplugin.h",
            "consumer/audiobuffer.h",
            "consumer/audiooutput.h",
            "consumer/videooutput.h",
            "consumer/videooutputnode.h",
            "control/controloutput.h",
            "control/gameconsole.h",
            "core/core.h",
            "core/libretro.h",
            "core/libretrocore.h",
            "core/libretroloader.h",
            "core/libretrorunner.h",
            "core/libretrosymbols.h",
            "core/libretrovariable.h",
            "core/libretrovariablemodel.h",
            "core/libretrovariableforwarder.h",
            "input/gamepadstate.h",
            "input/globalgamepad.h",
            "input/keyboardstate.h",
            "input/mousestate.h",
            "input/remapper.h",
            "input/remappermodel.h",
            "input/sdlmanager.h",
            "input/sdlunloader.h",
            "pipeline/node.h",
            "pipeline/pipelinecommon.h",
            "util/logging.h",
            "util/microtimer.h",
            "util/phoenixwindow.h",
            "util/phoenixwindownode.h",

            // C++ SOURCES
            "backendplugin.cpp",
            "consumer/audiobuffer.cpp",
            "consumer/audiooutput.cpp",
            "consumer/videooutput.cpp",
            "consumer/videooutputnode.cpp",
            "control/controloutput.cpp",
            "control/gameconsole.cpp",
            "core/core.cpp",
            "core/libretrocore.cpp",
            "core/libretroloader.cpp",
            "core/libretrorunner.cpp",
            "core/libretrosymbols.cpp",
            "core/libretrovariable.cpp",
            "core/libretrovariablemodel.cpp",
            "core/libretrovariableforwarder.cpp",
            "input/gamepadstate.cpp",
            "input/globalgamepad.cpp",
            "input/keyboardstate.cpp",
            "input/mousestate.cpp",
            "input/remapper.cpp",
            "input/remappermodel.cpp",
            "input/sdlmanager.cpp",
            "input/sdlunloader.cpp",
            "pipeline/node.cpp",
            "util/logging.cpp",
            "util/microtimer.cpp",
            "util/phoenixwindow.cpp",
            "util/phoenixwindownode.cpp",

            // OBJECTIVE_SOURCES
            "util/osxhelper.mm",

            // QRC FILES
            "input/controllerdb.qrc",


        ];

        //!msvc {
            // SDL 2

        Properties {
            condition: qbs.targetOS.contains( "windows" );
            cpp.dynamicLibraries: [ "SDL2main" ];
        }

        cpp.dynamicLibraries: [ "quazip", "SDL2", "samplerate", "z" ];


        //}


        cpp.cxxLanguageVersion: "c++11";
        cpp.warningLevel: "all";
        cpp.treatWarningsAsErrors: true;

        cpp.debugInformation: qbs.buildVariant == "debug" ? true : false;
        cpp.optimization: qbs.buildVariant == "debug" ? "none" : "fast";

        cpp.defines: [ "QT_MESSAGELOGCONTEXT", "QUAZIP_STATIC" ];

        // Externals
        cpp.staticLibraries: [ "./externals/quazip/quazip" ];

        // SDL2
        Properties {
            condition: qbs.targetOS.contains( "macx" );
            cpp.dynamicLibraries: [ "/usr/local/lib", "/opt/local/lib" ]; // Homebrew, MacPorts
            cpp.libraryPaths: [ "/usr/local/lib", "/opt/local/lib"]; // Homebrew, MacPorts


        }
    }*/

    StaticLibrary {
        name: "backend"

        Depends { name: "cpp" }
        Depends {
            name: "Qt"
            submodules: {
                var modules = [ "qml", "quick", "multimedia" ];
                if ( qbs.targetOS.contains( "osx" ) ) {
                    modules.push( "platformsupport-private" );
                }
                return modules;
            }
        }

        Group { qbs.install: true; fileTagsFilter: product.type;}

        Export { Depends { name: "cpp" } cpp.includePaths: [ "." ] }

        cpp.includePaths: {
            var paths = [ ".", ".." ]

            // Include out stuff
            paths = paths.concat( [ "consumer", "control", "core", "input", "pipeline", "util" ] )

            if ( qbs.targetOS.contains( "osx" ) ) {
                paths = paths.concat( [ "/usr/local/include",

                                "/usr/local/include/SDL2", // Homebrew
                                "/opt/local/include/SDL2", // MacPorts

                                ".",
                                "..",
                                "../externals/quazip/quazip",

                                // Include our stuff
                                "consumer", "control", "core", "input", "pipeline", "util" ] )

            }
            return paths;
        }

        cpp.libraryPaths: {
            var paths = [];
            if ( qbs.targetOS.contains( "osx" ) ) {
                paths = paths.concat( [ "/usr/local/lib" ] );
            }
            return paths;
        }

        files: [
            "*.h", "*/*.h",
            "*.cpp", "*/*.cpp",
            "util/*.mm",

            "input/*.qrc",
        ];

        cpp.dynamicLibraries: {
            var libs = [];
            if ( qbs.toolchain.contains( "msvc" ) ) {
                libs.push( "libsamplerate-0.lib" )
            } else {
                libs = libs.concat( [ "quazip", "SDL2", "samplerate", "z" ] );
                if ( qbs.targetOS.contains( "windows" ) ) {
                    libs = libs.push( "SDL2main" );
                }
                return libs;
            }
        }

        cpp.cxxLanguageVersion: "c++11";
        cpp.warningLevel: "all";
        cpp.treatWarningsAsErrors: true;

        cpp.optimization: qbs.buildVariant == "debug" ? "none" : "fast";

        cpp.defines: [ "QT_MESSAGELOGCONTEXT" ];
    }
//}
