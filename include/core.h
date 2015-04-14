#ifndef CORE_H
#define CORE_H

#include <QtCore>
#include <QFile>
#include <QString>
#include <QByteArray>
#include <QImage>
#include <QMap>
#include <QLibrary>
#include <QObject>

#include "libretro.h"
#include "audiobuffer.h"
#include "logging.h"
#include "inputmanager.h"
#include "keyboard.h"

/* The Core class is a wrapper around any given libretro core.
 * The general functionality for this class is to load the core into memory,
 * connect to all of the core's callbacks, such as video and audio rendering,
 * and generate frames of video and audio data in the raw memory format.
 *
 * The Core class is instantiated inside of the VideoItem class, which lives in the videoitem.cpp file.
 * Currently this approach only supports loading a single core and game at any one time.
 *
 * Check out the static callbacks in order to see how data is passed from the core, to the screen.
 *
 */

// Helper for resolving libretro methods
#define resolved_sym( name ) symbols->name = ( decltype( symbols->name ) )libretro_core->resolve( #name );

struct LibretroSymbols {

    LibretroSymbols();
    
    // Libretro core functions
    unsigned( *retro_api_version )( void );
    void ( *retro_cheat_reset )( void );
    void ( *retro_cheat_set )( unsigned , bool , const char * );
    void ( *retro_deinit )( void );
    void *( *retro_get_memory_data )( unsigned );
    size_t ( *retro_get_memory_size )( unsigned );
    unsigned( *retro_get_region )( void );
    void ( *retro_get_system_av_info )( struct retro_system_av_info * );
    void ( *retro_get_system_info )( struct retro_system_info * );
    void ( *retro_init )( void );
    bool ( *retro_load_game )( const struct retro_game_info * );
    bool ( *retro_load_game_special )( unsigned , const struct retro_game_info *, size_t );
    void ( *retro_reset )( void );
    void ( *retro_run )( void );
    bool ( *retro_serialize )( void *, size_t );
    size_t ( *retro_serialize_size )( void );
    void ( *retro_unload_game )( void );
    bool ( *retro_unserialize )( const void *, size_t );
    
    // Frontend-defined callbacks
    void ( *retro_set_audio_sample )( retro_audio_sample_t );
    void ( *retro_set_audio_sample_batch )( retro_audio_sample_batch_t );
    void ( *retro_set_controller_port_device )( unsigned, unsigned );
    void ( *retro_set_environment )( retro_environment_t );
    void ( *retro_set_input_poll )( retro_input_poll_t );
    void ( *retro_set_input_state )( retro_input_state_t );
    void ( *retro_set_video_refresh )( retro_video_refresh_t );
    
    // Optional core-defined callbacks
    void ( *retro_audio )();
    void ( *retro_audio_set_state )( bool enabled );
    void ( *retro_frame_time )( retro_usec_t delta );
    void ( *retro_keyboard_event )( bool down, unsigned keycode, uint32_t character, uint16_t key_modifiers );
    
};

class Core {

    public:

        Core();
        ~Core();

        //
        // Control methods
        //

        // Load a libretro core at the given path
        // Returns: true if successful, false otherwise
        bool loadCore( const char *path );

        // Load a game with the given path
        // Returns: true if the game was successfully loaded, false otherwise
        bool loadGame( const char *path );

        // Run core for one frame
        void doFrame();

        // Unload
        void unload();

        //
        // Misc
        //

        // A pointer to the last instance of the class used
        static Core *core;

        LibretroSymbols *getSymbols();
        QByteArray getLibraryName() {
            return library_name;
        }

        bool saveGameState( QString save_path, QString game_name );
        bool loadGameState( QString save_path, QString game_name );

        //
        // Video
        //

        retro_hw_render_callback getHWData() const {
            return hw_callback;
        }
        const void *getImageData() const {
            return video_data;
        }
        unsigned getBaseWidth() const {
            return video_width;
        }
        unsigned getBaseHeight() const {
            return video_height;
        }
        unsigned getMaxWidth() const {
            return system_av_info->geometry.max_width;
        }
        unsigned getMaxHeight() const {
            return system_av_info->geometry.max_height;
        }
        size_t getPitch() const {
            return video_pitch;
        }
        retro_pixel_format getPixelFormat() const {
            return pixel_format;
        }
        const retro_system_info *getSystemInfo() const {
            return system_info;
        }
        float getAspectRatio() const {
            if( system_av_info->geometry.aspect_ratio ) {
                return system_av_info->geometry.aspect_ratio;
            }

            return ( float )system_av_info->geometry.base_width / system_av_info->geometry.base_height;
        }

        //
        // Audio
        //

        AudioBuffer *audio_buf;
        //const int16_t *getAudioData() const { return audio_data; };
        //size_t getAudioFrames() const { return audio_frames; };
        //int16_t getLeftChannel() const { return left_channel; };
        //int16_t getRightChannel() const { return right_channel; };

        //
        // System
        //

        void setSystemDirectory( QString system_directory );
        void setSaveDirectory( QString save_directory );

        //
        // Timing
        //

        double getFps() const {
            return system_av_info->timing.fps;
        }
        double getSampleRate() const {
            return system_av_info->timing.sample_rate;
        }
        bool isDupeFrame() const {
            return is_dupe_frame;
        }

        // Container class for a libretro core variable
        class Variable {
            public:
                Variable() {}; // default constructor

                Variable( const retro_variable *var ) {
                    m_key = var->key;

                    // "Text before first ';' is description. This ';' must be followed by a space,
                    // and followed by a list of possible values split up with '|'."
                    QString valdesc( var->value );
                    int splitidx = valdesc.indexOf( "; " );

                    if( splitidx != -1 ) {
                        m_description = valdesc.mid( 0, splitidx ).toStdString();
                        auto _choices = valdesc.mid( splitidx + 2 ).split( '|' );

                        foreach( auto &choice, _choices ) {
                            m_choices.append( choice.toStdString() );
                        }
                    } else {
                        // unknown value
                    }
                };
                virtual ~Variable() {};

                const std::string &key() const {
                    return m_key;
                };

                const std::string &value( const std::string &default_ ) const {
                    if( m_value.empty() ) {
                        return default_;
                    }

                    return m_value;
                };

                const std::string &value() const {
                    static std::string default_( "" );
                    return value( default_ );
                }

                const std::string &description() const {
                    return m_description;
                };

                const QVector<std::string> &choices() const {
                    return m_choices;
                };

                bool isValid() const {
                    return !m_key.empty();
                };

            private:
                // use std::strings instead of QStrings, since the later store data as 16bit chars
                // while cores probably use ASCII/utf-8 internally..
                std::string m_key;
                std::string m_value; // XXX: value should not be modified from the UI while a retro_run() call happens
                std::string m_description;
                QVector<std::string> m_choices;

        };

    private:

        // Handle to the libretro core
        QLibrary *libretro_core;
        QByteArray library_name;

        // Struct containing libretro methods
        LibretroSymbols *symbols;

        // Information about the core
        retro_system_av_info *system_av_info;
        retro_system_info *system_info;
        QMap<std::string, Core::Variable> variables;

        // Do something with retro_variable
        retro_input_descriptor input_descriptor;
        retro_game_geometry game_geometry;
        retro_system_timing system_timing;
        retro_hw_render_callback hw_callback;
        bool full_path_needed;
        QByteArray system_directory;
        QByteArray save_directory;

        // Game
        QByteArray game_data;

        // Video
        unsigned video_height;
        const void *video_data;
        size_t video_pitch;
        unsigned video_width;
        retro_pixel_format pixel_format;

        // Audio
        int16_t left_channel;
        int16_t right_channel;

        // Timing
        bool is_dupe_frame;

        // Misc
        void *m_sram;
        void saveSRAM();
        void loadSRAM();

        // Callbacks
        static void audioSampleCallback( int16_t left, int16_t right );
        static size_t audioSampleBatchCallback( const int16_t *data, size_t frames );
        static bool environmentCallback( unsigned cmd, void *data );
        static void inputPollCallback( void );
        static void logCallback( enum retro_log_level level, const char *fmt, ... );
        static int16_t inputStateCallback( unsigned port, unsigned device, unsigned index, unsigned id );
        static void videoRefreshCallback( const void *data, unsigned width, unsigned height, size_t pitch );
};

// Do not scope this globally anymore, it is not thread-safe
// QDebug operator<<( QDebug, const Core::Variable & );

#endif
