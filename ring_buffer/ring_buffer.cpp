#include "ring_buffer.h"
#include <iostream>
#include <assert.h>

ring_buffer::ring_buffer(char* buf, int size)
: d_buf(buf), d_size(size), d_head(0), d_tail(0), d_wrapped(0)
{
}

void ring_buffer::push(int value)
{
    integrity();
    assert(value >= 0); // To let -1 be used as an error code from pop.
    const int oldhead = d_head;
    d_buf[d_head] = value;
    if (d_wrapped && d_head >= d_tail)
        d_tail = (d_tail + 1) % d_size;
    d_head = (d_head + 1) % d_size;
    if (d_head < oldhead)
        d_wrapped = 1;
    integrity();
}

int ring_buffer::pop()
{
    integrity();
    const int oldtail = d_tail;
    if (empty())
        return -1;
    const int result = d_buf[d_tail];
    d_tail = (d_tail + 1) % d_size;
    if (d_tail < oldtail)
        d_wrapped = 0;
    integrity();
    return result;
}

int ring_buffer::empty() const
{
    if (d_wrapped)
        return false;
    return d_tail == d_head;
}

std::ostream& ring_buffer::print(std::ostream& out) const
{
    out << "size = " << d_size
        << ", head = " << d_head
        << ", tail = " << d_tail
        << ", wrapped = " << d_wrapped;
    return out;
}

void ring_buffer::integrity() const
{
    if (d_wrapped)
        assert(d_head <= d_tail);
    else
        assert(d_tail <= d_head);
}
