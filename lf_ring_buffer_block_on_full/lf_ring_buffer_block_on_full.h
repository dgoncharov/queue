#ifndef lf_ring_buffer_block_on_full_block_ON_FULL_H
#define lf_ring_buffer_block_on_full_block_ON_FULL_H

#include <atomic>
#include <stddef.h>
#include <sys/types.h>

struct buffer {
    std::atomic<char*> value;
    std::atomic<buffer*> next;
};

class lf_ring_buffer_block_on_full {
public:
    explicit lf_ring_buffer_block_on_full(size_t size);
    ~lf_ring_buffer_block_on_full();
    void push(char* value, int k);
    char* pop();

private:
    size_t d_size;
    buffer* d_begin;
    std::atomic<buffer*> d_wpos;
    std::atomic<buffer*> d_rpos;
};

#endif
