#ifndef RING_BUFFER
#define RING_BUFFER

#include <iostream>
#include <assert.h>

template <typename T>
struct ring_buffer {
    ring_buffer(T* buf, int size);
    void push(const T& value);
    int checked_pop(T* result);
    const T& pop();
    void clear();
    int capacity() const;
    int size() const;
    int full() const;
    int empty() const;
    std::ostream& print(std::ostream& out) const;

private:
    void integrity() const;

    T* d_buf;
    int d_cap;
    int d_head;
    int d_tail;
    unsigned d_wrapped:1;
};

template <typename T>
ring_buffer<T>::ring_buffer(T* buf, int size)
: d_buf(buf), d_cap(size), d_head(0), d_tail(0), d_wrapped(0)
{
}

template <typename T>
void ring_buffer<T>::push(const T& value)
{
    integrity();
    const int oldhead = d_head;
    d_buf[d_head] = value;
    if (d_wrapped && d_head >= d_tail)
        d_tail = (d_tail + 1) % d_cap;
    d_head = (d_head + 1) % d_cap;
    if (d_head < oldhead)
        d_wrapped = 1;
    integrity();
}

template <typename T>
int ring_buffer<T>::checked_pop(T* result)
{
    if (empty())
        return -1;
    *result = pop();
    return 0;
}

template <typename T>
const T& ring_buffer<T>::pop()
{
    integrity();
    assert(empty() == 0);
    const int oldtail = d_tail;
    const T& result = d_buf[d_tail];
    d_tail = (d_tail + 1) % d_cap;
    if (d_tail < oldtail)
        d_wrapped = 0;
    integrity();
    return result;
}

template <typename T>
void ring_buffer<T>::clear()
{
    d_wrapped = d_tail = d_head = 0;
}

template <typename T>
int ring_buffer<T>::capacity() const
{
    return d_cap;
}

template <typename T>
int ring_buffer<T>::size() const
{
    if (d_wrapped)
        return d_cap - (d_tail - d_head);
    return d_head - d_tail;
}

template <typename T>
int ring_buffer<T>::full() const
{
    return d_cap == size();
}

template <typename T>
int ring_buffer<T>::empty() const
{
    return d_tail == d_head && d_wrapped == 0;
}

template <typename T>
void ring_buffer<T>::integrity() const
{
    const int size = this->size();
    const int empty = this->empty();
    const int full = this->full();
    const int cap = capacity();
    assert(size <= cap);
    if (full) {
        assert(size == cap);
        assert(empty == 0);
    }
    if (empty) {
        assert(size == 0);
        assert(full == 0);
    }
    if (size == 0)
        assert(empty);
    if (d_wrapped)
        assert(d_head <= d_tail);
    else
        assert(d_tail <= d_head);
}

template <typename T>
std::ostream& ring_buffer<T>::print(std::ostream& out) const
{
    out << "capacity = " << d_cap
        << ", head = " << d_head
        << ", tail = " << d_tail
        << ", wrapped = " << d_wrapped
        << ", empty = " << empty()
        << ", full = " << full()
        << ", size = " << size()
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


template <typename T>
std::ostream& operator<<(std::ostream& out, const ring_buffer<T>& buf)
{
    return buf.print(out);
}

#endif

/* Copyright (c) 2021 Dmitry Goncharov
 *
 * Distributed under the BSD License.
 * (See accompanying file COPYING).
 */
