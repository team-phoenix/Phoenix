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

    libretro_core = NULL;
    video_data = NULL;
    audio_buf = NULL;
    system_av_info = new retro_system_av_info();
    system_info = new retro_system_info();
    symbols = new LibretroSymbols;

    video_height = 0;
    video_data = NULL;
    video_pitch = 0;
    video_width = 0;
    pixel_format = RETRO_PIXEL_FORMAT_UNKNOWN;

    audio_data = NULL;
    audio_frames = 0;
    left_channel = 0;
    right_channel = 0;

    Core::core = this;

} // Core::Core()

Core::~Core() {

    delete libretro_core;
    delete system_av_info;
    delete system_info;
    delete symbols;
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
    symbols->retro_run();

    if (symbols->retro_audio)
        symbols->retro_audio();
    
} // void doFrame()

// Getters

// Video
// [1]

retro_hw_render_callback Core::getHWData() {

    return hw_callback;

} // retro_hw_render_callback Core::getHWData()

const void *Core::getImageData() {

    return video_data;

} // const void *Core::getImageData()

unsigned Core::getBaseHeight() {

    return video_height;

    
} // unsigned Core::getBaseHeight()

unsigned Core::getBaseWidth() {

    return video_width;

} // Core::getBaseWidth()

unsigned Core::getMaxHeight() {

    return system_av_info->geometry.max_height;

} // unsigned Core::getMaxHeight()

unsigned Core::getMaxWidth() {

    return system_av_info->geometry.max_width;

} // unsigned Core::getMaxWidth();

float Core::getAspectRatio() {

    if (system_av_info->geometry.aspect_ratio)
        return system_av_info->geometry.aspect_ratio;
    return (float )system_av_info->geometry.base_width / system_av_info->geometry.base_height;

} // float Core::getAspectRatio();

double Core::getFps() {

    return system_av_info->timing.fps;

} // double Core::getFps()

double Core::getSampleRate() {

    return system_av_info->timing.sample_rate;

} // double Core::getSampleRate()

size_t Core::getPitch() {

    return video_pitch;
    
} // size_t Core::getPitch()


retro_pixel_format Core::getPixelFormat() {

    return pixel_format;

} // Core::getPixelFormat()

// ~[1]

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

} // Core::getRightChannel()

// ~[2]

// Input
// [3]

void Core::setInputStateCallBack(bool is_pressed, unsigned port, unsigned device, unsigned index, unsigned id) {

    joypad[id] = is_pressed;
    inputStateCallback(port, device, index, id);

} // Core::setInputStateCallBack(unsigned port, unsigned device, unsigned index, unsigned id)

// ~[3]

// System
// [4]
void Core::setSystemDirectory(QString system_dir) {

    system_directory = system_dir;

} // Core::setSystemDirectory(QString system_dir)

//~[4]

LibretroSymbols* Core::getSymbols() {

    return symbols;

} // LibretroSymbols *RasterWindow::getSymbols()

// Load a libretro core at the given path
// Returns: true if successful, false otherwise
bool Core::loadCore( const char *path ) {

    qDebug() << "Core::loadCore(" << path << ")";
    
    libretro_core = new QLibrary( path );
    libretro_core->load();

    if( libretro_core->isLoaded() ) {
    
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
        symbols->retro_set_environment( environmentCallback );
        symbols->retro_set_audio_sample( audioSampleCallback );
        symbols->retro_set_audio_sample_batch( audioSampleBatchCallback );
        symbols->retro_set_input_poll( inputPollCallback );
        symbols->retro_set_input_state( inputStateCallback );
        symbols->retro_set_video_refresh( videoRefreshCallback );
        
        // Init the core
        symbols->retro_init();
        
        // Get some info about the game
        symbols->retro_get_system_info( system_info );
        full_path_needed = system_info -> need_fullpath;
        
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

    if( full_path_needed ) {
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
        game_data = game.readAll();
        
        game_info.path = NULL;
        game_info.data = game_data.data();
        game_info.size = game.size();
        game_info.meta = "";
        
    }
    
    // attempt to load the game
    bool ret = symbols->retro_load_game( &game_info );
    
    // Get some info about the game
    if( ret ) {
    
        symbols->retro_get_system_av_info( system_av_info );
        game_geometry = system_av_info->geometry;
        system_timing = system_av_info->timing;
        video_width = game_geometry.max_width;
        video_height = game_geometry.max_height;
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
    if(core->audio_buf) {
        uint32_t sample = ((uint16_t) left << 16) | (uint16_t) right;
        core->audio_buf->write((const char*)&sample, sizeof(int16_t) * 2);
    }

} // Core::an udioSampleCallback()

size_t Core::audioSampleBatchCallback( const int16_t *data, size_t frames ) {

    core->audio_data = data;
    core->audio_frames = frames;
    if(core->audio_buf)
        core->audio_buf->write((const char *)data, frames * sizeof(int16_t) * 2);

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
            (*(const char **)data) = Core::core->system_directory.toStdString().c_str();
            return true;
            
        case RETRO_ENVIRONMENT_SET_PIXEL_FORMAT: { // 10
            qDebug() << "\tRETRO_ENVIRONMENT_SET_PIXEL_FORMAT (10) (handled)";
            
            retro_pixel_format *pixelformat = ( enum retro_pixel_format * )data;
            Core::core->pixel_format = *pixelformat;

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
            Core::core->input_descriptor = *( retro_input_descriptor * )data;
            return true;
            break;
            
        case RETRO_ENVIRONMENT_SET_KEYBOARD_CALLBACK: // 12
            qDebug() << "\tRETRO_ENVIRONMENT_SET_KEYBOARD_CALLBACK (12) (handled)";
            Core::core->symbols->retro_keyboard_event = ( decltype( symbols->retro_keyboard_event ) )data;
            break;
            
        case RETRO_ENVIRONMENT_SET_DISK_CONTROL_INTERFACE: // 13
            qDebug() << "\tRETRO_ENVIRONMENT_SET_DISK_CONTROL_INTERFACE (13)";
            break;
            
        case RETRO_ENVIRONMENT_SET_HW_RENDER: // 14
            qDebug() << "\tRETRO_ENVIRONMENT_SET_HW_RENDER (14)";
            Core::core->hw_callback = *( retro_hw_render_callback *)data;
            switch (Core::core->hw_callback.context_type) {
                case RETRO_HW_CONTEXT_NONE:
                    qDebug() << "No hardware context was selected";
                    break;
                case RETRO_HW_CONTEXT_OPENGL:
                    qDebug() << "OpenGL 2 context was selected";
                    break;
                case RETRO_HW_CONTEXT_OPENGLES2:
                    qDebug() << "OpenGL ES 2 context was selected";
                    Core::core->hw_callback.context_type = RETRO_HW_CONTEXT_OPENGLES2;\
                    break;
                 case RETRO_HW_CONTEXT_OPENGLES3:
                    qDebug() << "OpenGL 3 context was selected";
                    break;
                default:
                    qCritical() << "RETRO_HW_CONTEXT: " << Core::core->hw_callback.context_type << " was not handled";
                    return false;
            }

            break;
            
        case RETRO_ENVIRONMENT_GET_VARIABLE: // 15
            qDebug() << "\tRETRO_ENVIRONMENT_GET_VARIABLE (15)";
            break;
            
        case RETRO_ENVIRONMENT_SET_VARIABLES: // 16
            qDebug() << "\tRETRO_ENVIRONMENT_SET_VARIABLES (16)";
            break;
            
        case RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE: // 17
//            qDebug() << "\tRETRO_ENVIRONMENT_GET_VARIABLE_UPDATE (17)";
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
            Core::core->symbols->retro_frame_time = ( decltype( symbols->retro_frame_time ) )data;
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

    if(port != 0 || index != 0)
        return 0;

    switch(device) {
        case RETRO_DEVICE_JOYPAD:
            if (id >= 16)
                return false;

            return Core::core->joypad[id];

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

    core->video_data = data;
    core->video_width = width;
    core->video_height = height;
    core->video_pitch = pitch;

    return;
    
} // Core::videoRefreshCallback()
