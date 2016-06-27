import qbs 1.0

QtApplication {

/*
 * Qt Settings
 */

    name: "frontend";

    Depends { name: "backend" }
    Depends {
        name: "Qt";
        submodules: {
            var modules = [ "qml", "quick", "sql", "multimedia", "network", "concurrent", "svg" ];
            if ( qbs.targetOS.contains( "osx" ) ) {
                modules.push( "platformsupport-private" );
            }
            return modules;
        }
    }

    files: [
        "cpp/*.cpp", "cpp/*.h", "cpp/*/*.cpp", "cpp/*/*.h", "cpp/*/*/*.cpp", "cpp/*/*/*.h",
        //"qml/*.qrc",
    ]

/*
 * Compiler Settings
 */

    cpp.defines: [ "QT_MESSAGELOGCONTEXT" ];

    cpp.cxxLanguageVersion: "c++14";
    cpp.warningLevel: "all";
    cpp.treatWarningsAsErrors: false;
    cpp.optimization: qbs.buildVariant == "debug" ? "none" : "fast"

    // Include our stuff
    cpp.includePaths: {
        var paths = [ "cpp", "cpp/library", "cpp/library/database",
                        "cpp/library/fileinfo", "cpp/library/model",
                        "cpp/library/scanner", "cpp/theme" ];

        if ( qbs.targetOS.contains( "osx" ) ) {
            paths = paths.concat( [ "/usr/local/include/quazip" ] );
        }
        return paths
    }

/*
 * Linker Settings
 */

    cpp.libraryPaths: {
        var paths = [ "../backend" ];
        if ( qbs.targetOS.contains( "osx" ) ) {
            paths = paths.concat( [ "/usr/local/lib", "/opt/local/lib"] ); // Homebrew, MacPorts
        }
        return paths;
    }

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

    //cpp.staticLibraries: [ "backend" ]

}
