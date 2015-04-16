#include "core.h"
#include "phoenixglobals.h"

extern InputManager input_manager;
extern PhoenixGlobals phxGlobals;

//  ________________________
// |                        |
// |        Globals         |
// |________________________|

QDebug operator<<( QDebug debug, const Core::Variable &var ) {
    // join a QVector of std::strings. (Really, C++ ?)
    auto &choices = var.choices();
    std::string joinedchoices;

    foreach( auto &choice, choices ) {
        joinedchoices.append( choice );

        if( &choice != &choices.last() ) {
            joinedchoices.append( ", " );
        }
    }

    auto QStr = QString::fromStdString; // shorter alias

    debug << qPrintable( QString( "Core::Variable(%1=%2, description=\"%3\", choices=[%4])" ).
                         arg( QStr( var.key() ) ).arg( QStr( var.value( "<not set>" ) ) ).
                         arg( QStr( var.description() ) ).arg( QStr( joinedchoices ) ) );
    return debug;
}

//  ________________________
// |                        |
// |    Static variables    |
// |________________________|


// Must always point to the last Core instance used
Core *Core::core = nullptr;

//  ________________________
// |                        |
// |      Constructors      |
// |________________________|

LibretroSymbols::LibretroSymbols() {
    retro_audio = nullptr;
    retro_audio_set_state = nullptr;
    retro_frame_time = nullptr;
    retro_keyboard_event = nullptr;
}

Core::Core() {
    libretro_core = nullptr;
    audio_buf = nullptr;
    system_av_info = new retro_system_av_info();
    system_info = new retro_system_info();
    symbols = new LibretroSymbols;

    video_height = 0;
    video_data = nullptr;
    video_pitch = 0;
    video_width = 0;
    pixel_format = RETRO_PIXEL_FORMAT_UNKNOWN;

    left_channel = 0;
    right_channel = 0;

    is_dupe_frame = false;
    m_sram = nullptr;

    Core::core = this;

    setSaveDirectory( phxGlobals.savePath() );
    setSystemDirectory( phxGlobals.biosPath() );

} // Core::Core()

Core::~Core() {

} // Core::~Core()

//  ________________________
// |                        |
// |     Public methods     |
// |________________________|

//
// Control methods
//

bool Core::loadCore( const char *path ) {
    libretro_core = new QLibrary( path );
    libretro_core->load();

    if( libretro_core->isLoaded() ) {

        library_name = libretro_core->fileName().toLocal8Bit();

        // Resolve symbols
        resolved_sym( retro_set_environment );
        resolved_sym( retro_set_video_refresh );
        resolved_sym( retro_set_audio_sample );
        resolved_sym( retro_set_audio_sample_batch );
        resolved_sym( retro_set_input_poll );
        resolved_sym( retro_set_input_state );
        resolved_sym( retro_init );
        resolved_sym( retro_deinit );
        resolved_sym( retro_api_version );
        resolved_sym( retro_get_system_info );
        resolved_sym( retro_get_system_av_info );
        resolved_sym( retro_set_controller_port_device );
        resolved_sym( retro_reset );
        resolved_sym( retro_run );
        resolved_sym( retro_serialize );
        resolved_sym( retro_serialize_size );
        resolved_sym( retro_unserialize );
        resolved_sym( retro_cheat_reset );
        resolved_sym( retro_cheat_set );
        resolved_sym( retro_load_game );
        resolved_sym( retro_load_game_special );
        resolved_sym( retro_unload_game );
        resolved_sym( retro_get_region );
        resolved_sym( retro_get_memory_data );
        resolved_sym( retro_get_memory_size );

        // Set callbacks
        symbols->retro_set_environment( environmentCallback );
        symbols->retro_set_audio_sample( audioSampleCallback );
        symbols->retro_set_audio_sample_batch( audioSampleBatchCallback );
        symbols->retro_set_input_poll( inputPollCallback );
        symbols->retro_set_input_state( inputStateCallback );
        symbols->retro_set_video_refresh( videoRefreshCallback );
        //symbols->retro_get_memory_data(getMemoryData);
        //symbols->retro_get_memory_size(getMemorySize);

        // Init the core
        symbols->retro_init();

        // Get some info about the game
        symbols->retro_get_system_info( system_info );
        full_path_needed = system_info->need_fullpath;

        return true;

    }

    return false;

} // Core::loadCore()

bool Core::loadGame( const char *path ) {
    // create a retro_game_info struct, load with data (created on stack)
    retro_game_info game_info;

    // full path needed, pass this file path to the core

    QFileInfo info( path );

    //QString file_name = info.baseName() + "." + info.suffix();
    //qDebug() << "suffix: " << file_name;

    /*if (info.suffix() == "cue") {
        full_path_needed = false;
        QFile cue_file(info.canonicalFilePath());
        if (cue_file.open(QIODevice::Text | QIODevice::ReadOnly)) {
            while (!cue_file.atEnd()) {
                QString line = cue_file.readLine().toLower();
                if (line.contains("file"))
                    file_name = line.split(" ").at(1).remove('"');
                qCDebug(phxLibrary) << line;
            }
            cue_file.close();
        }
    }*/

    //info.setFile(info.canonicalPath() + file_name);

    if( full_path_needed ) {
        game_info.path = path;
        game_info.data = nullptr;
        game_info.size = 0;
        game_info.meta = "";
    }

    else {
        // full path not needed, read the file to a buffer and pass that to the core


        QFile game( info.canonicalFilePath() );

        if( !game.open( QIODevice::ReadOnly ) ) {
            return false;
        }

        // read into memory
        game_data = game.readAll();

        game_info.path = nullptr;
        game_info.data = game_data.data();
        game_info.size = game.size();
        game_info.meta = "";

    }

    bool ret = symbols->retro_load_game( &game_info );

    // Get some info about the game
    if( !ret ) {
        return false;
    }

    symbols->retro_get_system_av_info( system_av_info );
    game_geometry = system_av_info->geometry;
    system_timing = system_av_info->timing;
    video_width = game_geometry.max_width;
    video_height = game_geometry.max_height;

    loadSRAM();

    return true;

} // Core::loadGame()

void Core::doFrame() {
    // Update the static pointer
    core = this;

    // Tell the core to run a frame
    symbols->retro_run();

    if( symbols->retro_audio ) {
        symbols->retro_audio();
    }

} // void doFrame()

void Core::unload() {
    saveSRAM();
    symbols->retro_unload_game();
    symbols->retro_deinit();
    libretro_core->unload();
    game_data.clear();
    library_name.clear();

    delete libretro_core;
    delete system_av_info;
    delete symbols;
    delete system_info;
    qCDebug( phxCore ) << "Finished unloading core";

} // Core::unload()

//
// Misc
//

LibretroSymbols *Core::getSymbols() {
    return symbols;

} // Core::getSymbols()


bool Core::saveGameState( QString path, QString name ) {
    Q_UNUSED( path );
    Q_UNUSED( name );

    size_t size = core->getSymbols()->retro_serialize_size();

    if( !size ) {
        return false;
    }

    char *data = new char[size];
    bool loaded = false;

    if( symbols->retro_serialize( data, size ) ) {
        QFile *file = new QFile( phxGlobals.savePath() + phxGlobals.selectedGame().baseName() + "_STATE.sav" );
        qCDebug( phxCore ) << file->fileName();

        if( file->open( QIODevice::WriteOnly ) ) {
            file->write( QByteArray( static_cast<char *>( data ), static_cast<int>( size ) ) );
            qCDebug( phxCore ) << "Save State wrote to " << file->fileName();
            file->close();
            loaded = true;
        }

        delete file;

    }

    delete[] data;
    return loaded;

} // Core::saveGameState(QString path, char *data, int size)

bool Core::loadGameState( QString path, QString name ) {
    Q_UNUSED( path );
    Q_UNUSED( name );

    QFile file( phxGlobals.savePath() + phxGlobals.selectedGame().baseName() + "_STATE.sav" );

    bool loaded = false;

    if( file.open( QIODevice::ReadOnly ) ) {
        QByteArray state = file.readAll();
        void *data = state.data();
        size_t size = static_cast<int>( state.size() );

        file.close();

        if( symbols->retro_unserialize( data, size ) ) {
            qCDebug( phxCore ) << "Save State loaded";
            loaded = true;
        }
    }

    return loaded;

} // Core::loadGameState(QString path)

//
// Video
//

//
// Audio
//

//
// System
//

void Core::setSystemDirectory( QString system_dir ) {
    system_directory = system_dir.toLocal8Bit();

} // Core::setSystemDirectory(QString system_dir)

void Core::setSaveDirectory( QString save_dir ) {
    save_directory = save_dir.toLocal8Bit();

} // Core::setSaveDirectory()

//  ________________________
// |                        |
// |    Private methods     |
// |________________________|

void Core::saveSRAM() {
    if( m_sram == nullptr ) {
        return;
    }

    QFile file( save_directory + phxGlobals.selectedGame().baseName() + ".srm" );
    qCDebug( phxCore ) << "Saving SRAM to: " << file.fileName();

    if( file.open( QIODevice::WriteOnly ) ) {
        char *data = static_cast<char *>( m_sram );
        size_t size = symbols->retro_get_memory_size( RETRO_MEMORY_SAVE_RAM );
        file.write( data, size );
        file.close();
    }
} // Core::saveSRAM()

void Core::loadSRAM() {
    m_sram = symbols->retro_get_memory_data( RETRO_MEMORY_SAVE_RAM );

    QFile file( save_directory + phxGlobals.selectedGame().baseName() + ".srm" );

    if( file.open( QIODevice::ReadOnly ) ) {
        QByteArray data = file.readAll();
        memcpy( m_sram, data.data(), data.size() );

        qCDebug( phxCore ) << "Loading SRAM from: " << file.fileName();
        file.close();
    }

} // Core::loadSRAM()

//  ________________________
// |                        |
// |       Callbacks        |
// |________________________|

void Core::audioSampleCallback( int16_t left, int16_t right ) {
    if( core->audio_buf ) {
        uint32_t sample = ( ( uint16_t ) left << 16 ) | ( uint16_t ) right;
        core->audio_buf->write( ( const char * )&sample, sizeof( int16_t ) * 2 );
    }

} // Core::audioSampleCallback()

size_t Core::audioSampleBatchCallback( const int16_t *data, size_t frames ) {
    if( core->audio_buf ) {
        core->audio_buf->write( ( const char * )data, frames * sizeof( int16_t ) * 2 );
    }

    return frames;
    
} // Core::audioSampleBatchCallback()

bool Core::environmentCallback( unsigned cmd, void *data ) {
    switch( cmd ) {
        case RETRO_ENVIRONMENT_SET_ROTATION: // 1
            qDebug() << "\tRETRO_ENVIRONMENT_SET_ROTATION (1)";
            break;

        case RETRO_ENVIRONMENT_GET_OVERSCAN: // 2
            qDebug() << "\tRETRO_ENVIRONMENT_GET_OVERSCAN (2) (handled)";
            // Crop away overscan
            return true;

        case RETRO_ENVIRONMENT_GET_CAN_DUPE: // 3
            *( bool * )data = true;
            return true;

        // 4 and 5 have been deprecated
        
        case RETRO_ENVIRONMENT_SET_MESSAGE: // 6
            qDebug() << "\tRETRO_ENVIRONMENT_SET_MESSAGE (6)";
            break;

        case RETRO_ENVIRONMENT_SHUTDOWN: // 7
            qDebug() << "\tRETRO_ENVIRONMENT_SHUTDOWN (7)";
            break;

        case RETRO_ENVIRONMENT_SET_PERFORMANCE_LEVEL: // 8
            qDebug() << "\tRETRO_ENVIRONMENT_SET_PERFORMANCE_LEVEL (8)";
            break;

        case RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY: // 9
            qCDebug( phxCore ) << "\tRETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY (9)";
            *static_cast<const char **>( data ) = core->system_directory.constData();
            return true;

        case RETRO_ENVIRONMENT_SET_PIXEL_FORMAT: { // 10
            qDebug() << "\tRETRO_ENVIRONMENT_SET_PIXEL_FORMAT (10) (handled)";

            retro_pixel_format *pixelformat = ( enum retro_pixel_format * )data;
            Core::core->pixel_format = *pixelformat;

            switch( *pixelformat ) {
                case RETRO_PIXEL_FORMAT_0RGB1555:
                    qDebug() << "\tPixel format: 0RGB1555\n";
                    return true;
                    
                case RETRO_PIXEL_FORMAT_RGB565:
                    qDebug() << "\tPixel format: RGB565\n";
                    return true;
                    
                case RETRO_PIXEL_FORMAT_XRGB8888:
                    qDebug() << "\tPixel format: XRGB8888\n";
                    return true;
                    
                default:
                    qDebug() << "\tError: Pixel format is not supported. (" << pixelformat << ")";
                    break;
            }

            return false;
        }

        case RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS: // 11
            qDebug() << "\tRETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS (11) (handled)";
            Core::core->input_descriptor = *( retro_input_descriptor * )data;
            return true;

        case RETRO_ENVIRONMENT_SET_KEYBOARD_CALLBACK: // 12
            qDebug() << "\tRETRO_ENVIRONMENT_SET_KEYBOARD_CALLBACK (12) (handled)";
            Core::core->symbols->retro_keyboard_event = ( decltype( LibretroSymbols::retro_keyboard_event ) )data;
            break;

        case RETRO_ENVIRONMENT_SET_DISK_CONTROL_INTERFACE: // 13
            qDebug() << "\tRETRO_ENVIRONMENT_SET_DISK_CONTROL_INTERFACE (13)";
            break;

        case RETRO_ENVIRONMENT_SET_HW_RENDER: // 14
            qDebug() << "\tRETRO_ENVIRONMENT_SET_HW_RENDER (14)";
            Core::core->hw_callback = *( retro_hw_render_callback * )data;

            switch( Core::core->hw_callback.context_type ) {
                case RETRO_HW_CONTEXT_NONE:
                    qDebug() << "No hardware context was selected";
                    break;

                case RETRO_HW_CONTEXT_OPENGL:
                    qDebug() << "OpenGL 2 context was selected";
                    break;

                case RETRO_HW_CONTEXT_OPENGLES2:
                    qDebug() << "OpenGL ES 2 context was selected";
                    Core::core->hw_callback.context_type = RETRO_HW_CONTEXT_OPENGLES2;
                    break;

                case RETRO_HW_CONTEXT_OPENGLES3:
                    qDebug() << "OpenGL 3 context was selected";
                    break;

                default:
                    qCritical() << "RETRO_HW_CONTEXT: " << Core::core->hw_callback.context_type << " was not handled";
                    break;
            }

            break;

        case RETRO_ENVIRONMENT_GET_VARIABLE: { // 15
            auto *rv = static_cast<struct retro_variable *>( data );

            if( core->variables.contains( rv->key ) ) {
                const auto &var = core->variables[rv->key];

                if( var.isValid() ) {
                    rv->value = var.value().c_str();
                }
            }

            break;
        }

        case RETRO_ENVIRONMENT_SET_VARIABLES: { // 16
            qCDebug( phxCore ) << "SET_VARIABLES:";
            auto *rv = static_cast<const struct retro_variable *>( data );

            for( ; rv->key != NULL; rv++ ) {
                Core::Variable v( rv );
                core->variables.insert( v.key(), v );
                qCDebug( phxCore ) << "\t" << v;
            }

            break;
        }

        case RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE: // 17
            //            qDebug() << "\tRETRO_ENVIRONMENT_GET_VARIABLE_UPDATE (17)";
            break;

        case RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME: // 18
            qDebug() << "\tRETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME (18)";
            break;

        case RETRO_ENVIRONMENT_GET_LIBRETRO_PATH: // 19
            *static_cast<const char **>( data ) = core->getLibraryName().constData();
            break;

        // 20 has been deprecated
        
        case RETRO_ENVIRONMENT_SET_FRAME_TIME_CALLBACK: // 21
            qDebug() << "RETRO_ENVIRONMENT_SET_FRAME_TIME_CALLBACK (21)";
            Core::core->symbols->retro_frame_time = ( decltype( LibretroSymbols::retro_frame_time ) )data;
            break;

        case RETRO_ENVIRONMENT_SET_AUDIO_CALLBACK: // 22
            qDebug() << "\tRETRO_ENVIRONMENT_SET_AUDIO_CALLBACK (22)";
            break;

        case RETRO_ENVIRONMENT_GET_RUMBLE_INTERFACE: // 23
            qDebug() << "\tRETRO_ENVIRONMENT_GET_RUMBLE_INTERFACE (23)";
            break;

        case RETRO_ENVIRONMENT_GET_INPUT_DEVICE_CAPABILITIES: // 24
            qDebug() << "\tRETRO_ENVIRONMENT_GET_INPUT_DEVICE_CAPABILITIES (24)";
            break;

        case RETRO_ENVIRONMENT_GET_SENSOR_INTERFACE: // 25
            qDebug() << "\tRETRO_ENVIRONMENT_GET_SENSOR_INTERFACE (25)";
            break;

        case RETRO_ENVIRONMENT_GET_CAMERA_INTERFACE: // 26
            qDebug() << "\tRETRO_ENVIRONMENT_GET_CAMERA_INTERFACE (26)";
            break;

        case RETRO_ENVIRONMENT_GET_LOG_INTERFACE: {// 27
            struct retro_log_callback *logcb = ( struct retro_log_callback * )data;
            logcb->log = logCallback;
            return true;
        }

        case RETRO_ENVIRONMENT_GET_PERF_INTERFACE: // 28
            qDebug() << "\tRETRO_ENVIRONMENT_GET_PERF_INTERFACE (28)";
            break;

        case RETRO_ENVIRONMENT_GET_LOCATION_INTERFACE: // 29
            qDebug() << "\tRETRO_ENVIRONMENT_GET_LOCATION_INTERFACE (29)";
            break;

        case RETRO_ENVIRONMENT_GET_CONTENT_DIRECTORY: // 30
            qDebug() << "\tRETRO_ENVIRONMENT_GET_CONTENT_DIRECTORY (30)";
            break;

        case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY: // 31
            qCDebug( phxCore ) << "\tRETRO_ENVIRONMENT_GET_SAVE_DIRECTORY (31)";
            *static_cast<const char **>( data ) = core->save_directory.constData();
            qCDebug( phxCore ) << "Save Directory: " << core->save_directory;
            break;

        case RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO: // 32
            qDebug() << "\tRETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO (32)";
            break;

        case RETRO_ENVIRONMENT_SET_PROC_ADDRESS_CALLBACK: // 33
            qDebug() << "\tRETRO_ENVIRONMENT_SET_PROC_ADDRESS_CALLBACK (33)";
            break;

        case RETRO_ENVIRONMENT_SET_SUBSYSTEM_INFO: // 34
            qDebug() << "\tRETRO_ENVIRONMENT_SET_SUBSYSTEM_INFO (34)";
            break;

        case RETRO_ENVIRONMENT_SET_CONTROLLER_INFO: // 35
            qDebug() << "\tRETRO_ENVIRONMENT_SET_CONTROLLER_INFO (35)";
            break;

        default:
            qDebug() << "Error: Environment command " << cmd << " is not defined in the frontend's libretro.h!.";
            return false;
    }
    
    // Command was not handled
    return false;
    
} // Core::environmentCallback()

void Core::inputPollCallback( void ) {
    // qDebug() << "Core::inputPollCallback";
    return;
    
} // Core::inputPollCallback()

int16_t Core::inputStateCallback( unsigned port, unsigned device, unsigned index, unsigned id ) {
    Q_UNUSED( index )

    if( static_cast<int>( port ) >= input_manager.getDevices().size() ) {
        return 0;
    }

    InputDevice *deviceobj = input_manager.getDevice( port );

    // make sure the InputDevice was configured
    // to map to the requested RETRO_DEVICE.


    if( deviceobj->mapping()->deviceType() != device ) {
        return 0;
    }

    // we don't handle index for now...
    return deviceobj->state( id );

} // Core::inputStateCallback()

void Core::logCallback( enum retro_log_level level, const char *fmt, ... ) {
    QVarLengthArray<char, 1024> outbuf( 1024 );
    va_list args;
    va_start( args, fmt );
    int ret = vsnprintf( outbuf.data(), outbuf.size(), fmt, args );

    if( ret < 0 ) {
        qCDebug( phxCore ) << "logCallback: could not format string";
        return;
    } else if( ( ret + 1 ) > outbuf.size() ) {
        outbuf.resize( ret + 1 );
        ret = vsnprintf( outbuf.data(), outbuf.size(), fmt, args );

        if( ret < 0 ) {
            qCDebug( phxCore ) << "logCallback: could not format string";
            return;
        }
    }

    va_end( args );

    // remove trailing newline, which are already added by qCDebug
    if( outbuf.value( ret - 1 ) == '\n' ) {
        outbuf[ret - 1] = '\0';

        if( outbuf.value( ret - 2 ) == '\r' ) {
            outbuf[ret - 2] = '\0';
        }
    }

    switch( level ) {
        case RETRO_LOG_DEBUG:
            qCDebug( phxCore ) << outbuf.data();
            break;

        case RETRO_LOG_INFO:
            qCDebug( phxCore ) << outbuf.data();
            break;

        case RETRO_LOG_WARN:
            qCWarning( phxCore ) << outbuf.data();
            break;

        case RETRO_LOG_ERROR:
            qCCritical( phxCore ) << outbuf.data();
            break;

        default:
            qCWarning( phxCore ) << outbuf.data();
            break;
    }

} // Core::retro_log()

void Core::videoRefreshCallback( const void *data, unsigned width, unsigned height, size_t pitch ) {
    if( data ) {
        core->video_data = data;
        core->is_dupe_frame = false;
    } else {
        core->is_dupe_frame = true;
    }

    core->video_width = width;
    core->video_height = height;
    core->video_pitch = pitch;

    return;
    
} // Core::videoRefreshCallback()

