#ifndef LF_RING_BUFFER_H
#define LF_RING_BUFFER_H

#include <atomic>
#include <stddef.h>
#include <sys/types.h>

struct buffer {
    char* value;
    std::atomic<buffer*> next;
    std::atomic<pid_t> owner;
};

class lf_ring_buffer {
public:
    explicit lf_ring_buffer(size_t size);
    ~lf_ring_buffer();
    void push(char* value);
    char* pop();

private:
    size_t d_size;
    buffer* d_begin;
    std::atomic<buffer*> d_wpos;
    std::atomic<buffer*> d_rpos;
};

#endif
