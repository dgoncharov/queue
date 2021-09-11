#ifndef RING_BUFFER
#define RING_BUFFER

#include <iosfwd>

struct ring_buffer {
    ring_buffer(char* buf, int size);
    void push(int value);
    int pop();
    int empty() const;
    std::ostream& print(std::ostream& out) const;

private:
    void integrity() const;

    char* d_buf;
    int d_size;
    int d_head;
    int d_tail;
    int d_wrapped:1;
};

#endif
