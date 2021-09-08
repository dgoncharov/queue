#include "lf_circular_buffer.h"
#include "logger.h"
#include <thread>
#include <iostream>
#include <assert.h>
#include <string.h>

buffer::buffer(size_t size)
: d_size(size), d_begin((int*) malloc(size * sizeof(int))), d_pos(d_begin)
{
    assert(size);
    logger(std::cout) << "allocated " << (void*) d_begin << '\n';
    assert(d_begin);
}

buffer::~buffer()
{
    logger(std::cout) << "freeing " << (void*) d_begin << '\n';
    free(d_begin);
}

size_t buffer::push(int k)
{
    const int* end = d_begin + d_size;
    assert(d_pos < end);
    *d_pos = k;
    ++d_pos;
    if (d_pos >= end)
        d_pos = d_begin;
    assert(d_pos < end);
    logger(std::cout) << "pushed " << k << " to " << (void*) d_begin << '\n';
    return d_pos - d_begin;
}

void buffer::clear()
{
    d_pos = d_begin;
    memset(d_begin, 0, d_size*sizeof(int));
}

std::ostream& buffer::print(std::ostream& out) const
{
    out << "begin = " << d_begin
        << ", pos = " << d_pos
        << ", d_pos - d_begin = " << d_pos - d_begin
        << ", *d_begin = " << *d_begin
        << ", values = ";
    const int* end = d_begin + d_size;
    assert(d_pos < end);
    const char* comma = "";
    if (d_pos > d_begin || *d_begin)
        for (const int* p = d_pos; p < end; ++p, comma = ", ")
            out << comma << *p;
    for (const int* p = d_begin; p < d_pos; ++p, comma = ", ")
        out << comma << *p;
    out << '\n';
    return out;
}

lf_circular_buffer::lf_circular_buffer(buffer *buf)
: d_buf(buf)
{
}

buffer* lf_circular_buffer::exchange(buffer* newbuf)
{
    buffer* buf = d_buf.load(std::memory_order_relaxed);
    while (d_buf.compare_exchange_weak(buf, newbuf) == false)
        ;
    return buf;
}
