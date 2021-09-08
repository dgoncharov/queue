#ifndef LF_CIRCULAR_BUFFER_H
#define LF_CIRCULAR_BUFFER_H

#include <iosfwd>
#include <atomic>
#include <stddef.h>

struct buffer {
    explicit buffer(size_t size);
    ~buffer();
    size_t push(int k);
    void clear();
    std::ostream& print(std::ostream& out) const;

private:
    buffer(const buffer&);
    size_t d_size;
    int* d_begin;
    int* d_pos;
};

class lf_circular_buffer {
public:
    lf_circular_buffer(buffer* buf);
    buffer* exchange(buffer* buf);

private:
    std::atomic<buffer*> d_buf;
};

#endif
