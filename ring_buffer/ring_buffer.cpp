#include "ring_buffer.h"
#include <iostream>
#include <assert.h>

ring_buffer::ring_buffer(int* buf, int size)
: d_buf(buf), d_cap(size), d_head(0), d_tail(0), d_wrapped(0)
{
}

void ring_buffer::push(int value)
{
    integrity();
    assert(value >= 0); // To let -1 be used as an error code from pop.
    const int oldhead = d_head;
    d_buf[d_head] = value;
    if (d_wrapped && d_head >= d_tail)
        d_tail = (d_tail + 1) % d_cap;
    d_head = (d_head + 1) % d_cap;
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
    d_tail = (d_tail + 1) % d_cap;
    if (d_tail < oldtail)
        d_wrapped = 0;
    integrity();
    return result;
}

void ring_buffer::clear()
{
    d_wrapped = d_tail = d_head = 0;
}

int ring_buffer::capacity() const
{
    return d_cap;
}
 
int ring_buffer::size() const
{
    if (d_wrapped)
        return d_cap - (d_tail - d_head);
    return d_head - d_tail;
}

int ring_buffer::full() const
{
    return d_cap == size();
}
 
int ring_buffer::empty() const
{
    if (d_wrapped)
        return false;
    return d_tail == d_head;
}

std::ostream& ring_buffer::print(std::ostream& out) const
{
    out << "size = " << d_cap
        << ", head = " << d_head
        << ", tail = " << d_tail
        << ", wrapped = " << d_wrapped
        << ", values = ";

    int k = d_tail;
    const char* comma = "";
    if (d_wrapped) {
        for (; k < d_cap; ++k, comma = ", ")
            out << comma << d_buf[k];
        k = 0;
    }
    for (; k < d_head; ++k, comma = ", ")
        out << comma << d_buf[k];
    out << '\n';
    return out;
}

void ring_buffer::integrity() const
{
    if (d_wrapped)
        assert(d_head <= d_tail);
    else
        assert(d_tail <= d_head);
}

std::ostream& operator<<(std::ostream& out, const ring_buffer& buf)
{
    return buf.print(out);
}
