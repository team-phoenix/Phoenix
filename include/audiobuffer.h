
#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H

#include <atomic>
#include <cstddef>

#include "logging.h"


/* This AudioBuffer class is a circular buffer that holds audio data.
 * This class should only be accessed by one consumer and one producer thread.
 *
 * This AudioBuffer class is instantiated inside of the Audio class, which lives in audio.cpp.
 * Before each write, the Audio class must fill up this temporary buffer so that we are writing an entire frame worth of audio data
 * to the audio output.
 *
 * This class uses atomic types in order to be thread safe.
 */
class AudioBuffer {

        char *m_buffer;
        std::atomic<size_t> m_head; // newest item in queue
        std::atomic<size_t> m_tail; // oldest item in queue
        const size_t m_size;

        inline size_t next( size_t current ) {
            return ( current + 1 ) % m_size;
        }

        size_t size_impl( size_t head, size_t tail ) const;

    public:
        AudioBuffer( size_t size = 4096 * 4 );

        virtual ~AudioBuffer();

        size_t write( const char *data, size_t size );

        size_t read( char *object, size_t size );

        size_t size() const;

        void clear();
};

#endif
