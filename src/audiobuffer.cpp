
#include <audiobuffer.h>


AudioBuffer::AudioBuffer(size_t size)
                        : m_head(0), m_tail(0), m_size(size) {
    m_buffer = new char[m_size];
}

AudioBuffer::~AudioBuffer() {
    delete [] m_buffer;
}

size_t AudioBuffer::write(const char *data, size_t size) {
    size_t wrote = 0;
    size_t head, tail, nextHead;
    while(wrote < size) {
        head = m_head.load(std::memory_order_relaxed);
        nextHead = next(head);
        tail = m_tail.load(std::memory_order_acquire);
        if (nextHead == tail) {
            // buffer is full. let's just clear it.
            // It probably means the core produces frames too fast (not
            // clocked right) or audio backend stopped reading frames.
            // In the first case, it might cause audio to skip a bit.
            qCDebug(phxAudio, "Buffer full, dropping samples");
            clear();
        }
        m_buffer[head] = data[wrote++];
        m_head.store(nextHead, std::memory_order_release);
    }

    return wrote;
}

size_t AudioBuffer::read(char *data, size_t size) {
    size_t read = 0;
    size_t head, tail;
    while(read < size) {
        tail = m_tail.load(std::memory_order_relaxed);
        head = m_head.load(std::memory_order_acquire);
        if (tail == head) {
            break;
        }
        data[read++] = m_buffer[tail];
        m_tail.store(next(tail), std::memory_order_release);
    }

    return read;
}

size_t AudioBuffer::size_impl(size_t tail, size_t head) const {
    if(head < tail) {
        return head + (m_size - tail);
    } else {
        return head - tail;
    }
}

size_t AudioBuffer::size() const {
    size_t head = m_head.load(std::memory_order_relaxed);
    size_t tail = m_tail.load(std::memory_order_relaxed);
    return size_impl(tail, head);
}

void AudioBuffer::clear() {
    m_head.store(m_tail.load(std::memory_order_relaxed), std::memory_order_relaxed);
}
