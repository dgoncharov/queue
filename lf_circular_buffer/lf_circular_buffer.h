#ifndef LF_CIRCULAR_BUFFER_H
#define LF_CIRCULAR_BUFFER_H

#include <atomic>
#include <vector>

typedef std::vector<char*> ring_buffer_t;
class lf_circular_buffer {
public:
    lf_circular_buffer(ring_buffer_t* buf);
    ring_buffer_t* push(ring_buffer_t* buf);
    ring_buffer_t* pop(ring_buffer_t* buf);

private:
    std::atomic<ring_buffer_t*> d_buf;
};

#endif
