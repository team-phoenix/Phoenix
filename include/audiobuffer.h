
#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H

#include <atomic>
#include <cstddef>

#include "logging.h"


// Circular buffer holding audio data.
// Should only be accessed by one consumer and one producer thread.
class AudioBuffer {

    char *m_buffer;
    std::atomic<size_t> m_head; // newest item in queue
    std::atomic<size_t> m_tail; // oldest item in queue
    const size_t m_size;

    inline size_t next(size_t current)
    {
        return (current + 1) % m_size;
    }

    size_t size_impl(size_t head, size_t tail) const;

public:
    AudioBuffer(size_t size = 4096*4);
 
    virtual ~AudioBuffer();
 
    size_t write(const char *data, size_t size);
 
    size_t read(char *object, size_t size);

    size_t size() const;

    void clear();
};

#endif
