// Phoenix
// Core - A libretro core encapsulated in a C++ object
// Author: athairus
// Created: May 31, 2014

#include "core.h"

LibretroSymbols::LibretroSymbols() {
    retro_audio = NULL;
    retro_audio_set_state = NULL;
    retro_frame_time = NULL;
    retro_keyboard_event = NULL;
}

//  ________________________
// |                        |
// |    Static variables    |
// |________________________|


// Must always point to the current Core
Core* Core::core = NULL;

//  ________________________
// |                        |
// |      Constructors      |
// |________________________|

Core::Core() {

    mLibretroCore = NULL;
    mImageData = NULL;
    mRetroSystemAVInfo = new retro_system_av_info;
    mRetroSystemInfo = new retro_system_info;
    mSymbols = new LibretroSymbols;

    mHeight = 0;
    mImageData = NULL;
    mPitch = 0;
    mWidth = 0;
    mPixelFormat = RETRO_PIXEL_FORMAT_UNKNOWN;

    audio_data = NULL;
    audio_frames = 0;
    left_channel = 0;
    right_channel = 0;

    Core::core = this;

} // Core::Core()

Core::~Core() {

    if (mLibretroCore)
        mLibretroCore->deleteLater();
    if (mRetroSystemAVInfo)
        delete mRetroSystemAVInfo;
    if (mRetroSystemInfo)
        delete mRetroSystemInfo;
    if (mSymbols)
        delete mSymbols;
    Core::core = NULL;

} // Core::~Core()

//  ________________________
// |                        |
// |     Public methods     |
// |________________________|

// Run core for one frame
void Core::doFrame() {

	// Update the static pointer
	core = this;

	// Tell the core to run a frame
    mSymbols->retro_run();

    if (mSymbols->retro_audio)
        mSymbols->retro_audio();
    
} // void doFrame()

// Getters

QMap<unsigned, bool> Core::getKeyBoard() {

    return mKeyboard;

}

// Video
// [1]

const void *Core::getImageData() {

    return mImageData;

} // const void *Core::getImageData()

unsigned Core::getMinHeight() {

    return mHeight;

    
} // unsigned Core::getMinHeight()

unsigned Core::getMinWidth() {

    return mWidth;

} // Core::getMinWidth()

unsigned Core::getMaxHeight() {

    return mRetroSystemAVInfo->geometry.max_height;

} // unsigned Core::getMaxHeight()

unsigned Core::getMaxWidth() {

    return mRetroSystemAVInfo->geometry.max_width;

} // unsigned Core::getMaxWidth();

float Core::getAspectRatio() {

    return mRetroSystemAVInfo->geometry.aspect_ratio;

} // unsigned Core::getAspectRatio();

double Core::getFps() {

    return mRetroSystemAVInfo->timing.fps;

} // double Core::getFps()

double Core::getSampleRate() {

    return mRetroSystemAVInfo->timing.sample_rate;

} // double Core::getSampleRate()

size_t Core::getPitch() {

    return mPitch;
    
} // size_t Core::getPitch()


retro_pixel_format Core::getPixelFormat() {

    return mPixelFormat;

} // Core::getPixelFormat()

// [1]

// Audio
// [2]

const int16_t *Core::getAudioData() {

    return audio_data;

} // Core::getAudioData()

size_t Core::getAudioFrames() {

    return audio_frames;

} // Core::getAudioFrames()

int16_t Core::getLeftChannel() {

    return left_channel;

} // Core::getLeftChannel()

int16_t Core::getRightChannel() {

    return right_channel;

} // Core::getLeftChannel()

// [2]

LibretroSymbols* Core::getSymbols() {

    return mSymbols;

} // LibretroSymbols *RasterWindow::getSymbols()

// Load a libretro core at the given path
// Returns: true if successful, false otherwise
bool Core::loadCore( const char *path ) {

    qDebug() << "Core::loadCore(" << path << ")";
    
    mLibretroCore = new QLibrary( path );
    mLibretroCore->load();

    if( mLibretroCore->isLoaded() ) {
    
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
        mSymbols->retro_set_environment( environmentCallback );
        mSymbols->retro_set_audio_sample( audioSampleCallback );
        mSymbols->retro_set_audio_sample_batch( audioSampleBatchCallback );
        mSymbols->retro_set_input_poll( inputPollCallback );
        mSymbols->retro_set_input_state( inputStateCallback );
        mSymbols->retro_set_video_refresh( videoRefreshCallback );
        
        // Init the core
        mSymbols->retro_init();
        
        // Get some info about the game
        mSymbols->retro_get_system_info( mRetroSystemInfo );
        mRetroFullpathNeeded = mRetroSystemInfo -> need_fullpath;
        
        return true;
        
    }
    
    else {
        qDebug() << "Core is not loaded!";
        return false;
    }
    
    return false;
    
} // Core::loadCore()

// Load a game with the given path
// Returns: true if the game was successfully loaded, false otherwise
bool Core::loadGame( const char *path ) {

    qDebug() << "Core::loadGame(" << path << ")";
    
    // create a retro_game_info struct, load with data (created on stack)
    retro_game_info game_info;
    
    // full path needed, pass this file path to the core

    if( mRetroFullpathNeeded ) {
        game_info.path = path;
        game_info.data = NULL;
        game_info.size = 0;
        game_info.meta = "";

    }

    // full path not needed, read the file to a buffer and pass that to the core
    else {
        // attempt to open file
        QFile game( path );
        
        if( !game.open( QIODevice::ReadOnly ) ) {
            return false;
        }

        // read into memory
        mGameData = game.readAll();
        
        game_info.path = NULL;
        game_info.data = mGameData.data();
        game_info.size = game.size();
        game_info.meta = "";
        
    }
    
    // attempt to load the game
    bool ret = mSymbols->retro_load_game( &game_info );
    
    // Get some info about the game
    if( ret ) {
    
        mSymbols->retro_get_system_av_info( mRetroSystemAVInfo );
        mRetroGameGeometry = mRetroSystemAVInfo->geometry;
        mRetroSystemTiming = mRetroSystemAVInfo->timing;
        //mWidth = mRetroGameGeometry.max_width;
        //mHeight = mRetroGameGeometry.max_height;
        return true;
        
    }
    else {
        return false;
    }
    
} // Core::load_game()

//  ________________________
// |                        |
// |       Callbacks        |
// |________________________|

void Core::audioSampleCallback( int16_t left, int16_t right ) {

    Core::core->left_channel = left;
    Core::core->right_channel = right;

    qDebug() << "Core::audioSampleCallback";
    return;
    
} // Core::an udioSampleCallback()

size_t Core::audioSampleBatchCallback( const int16_t *data, size_t frames ) {

    core->audio_data = data;
    core->audio_frames = frames;
    //qDebug() << "frames: " << frames;
    //qDebug() << "Core::audioSampleBatchCallback";

    return frames;
    
} // Core::audioSampleBatchCallback()

bool Core::environmentCallback( unsigned cmd, void *data ) {

    qDebug() << "Core::environmentCallback:";
    
    switch( cmd ) {
    
        case RETRO_ENVIRONMENT_SET_ROTATION: // 1
            qDebug() << "\tRETRO_ENVIRONMENT_SET_ROTATION (1)";
            break;
            
        case RETRO_ENVIRONMENT_GET_OVERSCAN: // 2
            qDebug() << "\tRETRO_ENVIRONMENT_GET_OVERSCAN (2) (handled)";
            // Crop away overscan
            return true;
            break;
            
        case RETRO_ENVIRONMENT_GET_CAN_DUPE: // 3
            qDebug() << "\tRETRO_ENVIRONMENT_GET_CAN_DUPE (3) (handled)";
            *( bool * )data = true;
            return true;
            break;
            
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
            qDebug() << "\tRETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY (9)";
            break;
            
        case RETRO_ENVIRONMENT_SET_PIXEL_FORMAT: { // 10
            qDebug() << "\tRETRO_ENVIRONMENT_SET_PIXEL_FORMAT (10) (handled)";
            
            retro_pixel_format *pixelformat = ( enum retro_pixel_format * )data;
            Core::core->mPixelFormat = *pixelformat;

            switch( *pixelformat ) {
                case RETRO_PIXEL_FORMAT_0RGB1555:
                    qDebug() << "\tPixel format: 0RGB1555\n";
                    return true;
                    break;
                    
                case RETRO_PIXEL_FORMAT_RGB565:
                    qDebug() << "\tPixel format: RGB565\n";
                    return true;
                    break;
                    
                case RETRO_PIXEL_FORMAT_XRGB8888:
                    qDebug() << "\tPixel format: XRGB8888\n";
                    return true;
                    break;
                    
                default:
                    qDebug() << "\tError: Pixel format is not supported. (" << pixelformat << ")";
                    return false;
            }

            break;
        }
        
        case RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS: // 11
            qDebug() << "\tRETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS (11) (handled)";
            Core::core->mRetroInputDescriptor = *( retro_input_descriptor * )data;
            return true;
            break;
            
        case RETRO_ENVIRONMENT_SET_KEYBOARD_CALLBACK: // 12
            qDebug() << "\tRETRO_ENVIRONMENT_SET_KEYBOARD_CALLBACK (12) (handled)";
            Core::core->mSymbols->retro_keyboard_event = ( typeof( mSymbols->retro_keyboard_event ) )data;
            break;
            
        case RETRO_ENVIRONMENT_SET_DISK_CONTROL_INTERFACE: // 13
            qDebug() << "\tRETRO_ENVIRONMENT_SET_DISK_CONTROL_INTERFACE (13)";
            break;
            
        case RETRO_ENVIRONMENT_SET_HW_RENDER: // 14
            qDebug() << "\tRETRO_ENVIRONMENT_SET_HW_RENDER (14)";
            break;
            
        case RETRO_ENVIRONMENT_GET_VARIABLE: // 15
            qDebug() << "\tRETRO_ENVIRONMENT_GET_VARIABLE (15)";
            break;
            
        case RETRO_ENVIRONMENT_SET_VARIABLES: // 16
            qDebug() << "\tRETRO_ENVIRONMENT_SET_VARIABLES (16)";
            break;
            
        case RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE: // 17
            qDebug() << "\tRETRO_ENVIRONMENT_GET_VARIABLE_UPDATE (17)";
            break;
            
        case RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME: // 18
            qDebug() << "\tRETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME (18)";
            break;
            
        case RETRO_ENVIRONMENT_GET_LIBRETRO_PATH: // 19
            qDebug() << "\tRETRO_ENVIRONMENT_GET_LIBRETRO_PATH (19)";
            break;
            
        // 20 has been deprecated
        
        case RETRO_ENVIRONMENT_SET_FRAME_TIME_CALLBACK: // 21
            qDebug() << "RETRO_ENVIRONMENT_SET_FRAME_TIME_CALLBACK (21)";
            Core::core->mSymbols->retro_frame_time = ( typeof( mSymbols->retro_frame_time ) )data;
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
            
        case RETRO_ENVIRONMENT_GET_LOG_INTERFACE: { // 27
            qDebug() << "\tRETRO_ENVIRONMENT_GET_LOG_INTERFACE (27) (handled)";
            struct retro_log_callback *logcb = ( struct retro_log_callback * )data;
            logcb->log = logCallback;
            return true;
            break;
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
            qDebug() << "\tRETRO_ENVIRONMENT_GET_SAVE_DIRECTORY (31)";
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

   // qDebug() << "input port: " << port;
    //qDebug() << "input device: " << device;
    //qDebug() << "input index: " << index;
    //qDebug() << "input id: " << id;

    // If device is already being read from, ignore
    if (port != 0 || index != 0)
        return 0;

    switch(device) {
        case RETRO_DEVICE_JOYPAD:
            return 0;
        case RETRO_DEVICE_KEYBOARD:
            // If key press id is a proper key, return true
            qDebug() << "keyboard is true";
            return true;
        default:
            break;
    }
    return 0;

    // qDebug() << "Core::inputStateCallback";
} // Core::inputStateCallback()

void Core::logCallback( enum retro_log_level level, const char *fmt, ... ) {

    Q_UNUSED( level );
    va_list args;
    va_start( args, fmt );
    printf( "[CORE]: " );
    vprintf( fmt, args );
    va_end( args );
    
} // Core::retro_log()

void Core::videoRefreshCallback( const void *data, unsigned width, unsigned height, size_t pitch ) {

    core->mImageData = data;
    core->mWidth = width;
    core->mHeight = height;
    core->mPitch = pitch;

    return;
    
} // Core::videoRefreshCallback()
