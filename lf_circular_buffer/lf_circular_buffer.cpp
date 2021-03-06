#include "lf_circular_buffer.h"
#include "logger.h"
#include "assert.h"
#include <thread>

#define ASSERT(c) do { if ((c) == 0) { logger(std::cerr) << "assert faield "; assert(c); } } while (0)

static
std::ostream& operator<<(std::ostream& out, const buffer_t& x)
{
    const char* comma = "";
    for (size_t k = 0, len = x.size(); k < len; ++k, comma = ", ")
        out << comma << x[k];
    return out;
}

lf_circular_buffer::lf_circular_buffer(buffer_t *buf, int loglevel)
: d_buf(buf)
, d_loglevel(loglevel)
{
}

buffer_t* lf_circular_buffer::push(buffer_t* newbuf)
{
    ASSERT(newbuf->size());

    buffer_t* buf = d_buf.load(std::memory_order_relaxed);
    for (;;) {
        logger(std::cout, d_loglevel) << "pushing " << newbuf << ", buf = " << buf << '\n';
        ASSERT(buf != newbuf);
        // Wait till the current buffer is empty.
        while (buf->size())
            std::this_thread::yield();
        buffer_t* b = buf;
        if (d_buf.compare_exchange_weak(buf, newbuf) == false) {
            // Another producer or consumer swapped the buffer after the empty
            // check.
            logger(std::cout, d_loglevel) << "another producer took " << b << ", new buf = " << buf << '\n';
            continue;
        }
        if (buf->size()) {
            // This thread was able to successfully exchange buf and newbuf.
            // However, between buf->size check and the following compare_exchange another
            // producer called lf_circular_buffer::push and also found the same buf empty
            // and replaced it with its own newbuf and then filled it up and came back and
            // pushed buf back successfully.
            // This thread then resumed and found that buf is the same and thus
            // compare_exchange succeeded, but the buf is now full.
            // This is unfortunate, because this means that the data in this
            // buf cannot be consumed. It has to be discarded to guarantee the
            // fifo property of the ring buffer. If this data is not discarded,
            // but rather pushed again for consumption, it is possible that the
            // producer which originally filled this buf already pushed some
            // later data and that the later data were already consumed.
            logger(std::cout, d_loglevel) << buf << " still not empty\n";
            return buf;
        }
        break;
    }
    logger(std::cout, d_loglevel) << "pushed " << newbuf << ", retrieved " << buf << '\n';
    ASSERT(buf->empty());
    return buf;
}

buffer_t* lf_circular_buffer::pop(buffer_t* newbuf)
{
    ASSERT(newbuf->empty());

    buffer_t* buf = d_buf.load(std::memory_order_relaxed);
    for (;;) {
        logger(std::cout, d_loglevel) << "pop pushing " << newbuf << ", buf = " << buf << '\n';
        ASSERT(buf != newbuf);
        // Wait till the current buffer has data.
        while (buf->empty())
            std::this_thread::yield();
        buffer_t* b = buf;
        if (d_buf.compare_exchange_weak(buf, newbuf) == false) {
            // Another producer or consumer swapped the buffer after the empty
            // check.
            logger(std::cout, d_loglevel) << "another consumer took " << b << ", new buf = " << buf << '\n';
            continue;
        }
        logger(std::cout, d_loglevel) << "poped " << buf << '\n';
        if (buf->empty()) {
            // Another consumer took, emptied and returned the buffer between
            // buf->empty check and subsequent compare_exchange.
            // Then this thread resumed and discovered that compare_exchange
            // succeeded, but buf is empty.
            logger(std::cout, d_loglevel) << buf << " still empty, trying again\n";
            newbuf = buf;
            buf = d_buf.load(std::memory_order_relaxed);
            continue;
        }
        break;
    }
    logger(std::cout, d_loglevel) << "pushed " << newbuf << ", poped buf = " << buf << " " << *buf << '\n';
    ASSERT(buf->size());
    return buf;
}

/* Copyright (c) 2021 Dmitry Goncharov
 *
 * Distributed under the BSD License.
 * (See accompanying file COPYING).
 */
