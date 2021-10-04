#ifndef LF_CIRCULAR_BUFFER_H
#define LF_CIRCULAR_BUFFER_H

#include <atomic>
#include <vector>

typedef std::vector<char*> buffer_t;
class lf_circular_buffer {
public:
    lf_circular_buffer(buffer_t* buf);
    buffer_t* push(buffer_t* buf);
    buffer_t* pop(buffer_t* buf);

private:
    std::atomic<buffer_t*> d_buf;
};

#endif
