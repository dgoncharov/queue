#ifndef RING_BUFFER
#define RING_BUFFER

#include <iosfwd>

struct ring_buffer {
    ring_buffer(int* buf, int size);
    void push(int value);
    int pop();
    void clear();
    int capacity() const;
    int size() const;
    int full() const;
    int empty() const;
    std::ostream& print(std::ostream& out) const;

private:
    void integrity() const;

    int* d_buf;
    int d_cap;
    int d_head;
    int d_tail;
    unsigned d_wrapped:1;
};


std::ostream& operator<<(std::ostream& out, const ring_buffer& buf);

#endif
