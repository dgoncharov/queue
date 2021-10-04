#include "lf_circular_buffer.h"
#include "ring_buffer.h"
#include "logger.h"
#include "assert.h"
#include <thread>

#define ASSERT(c) do { if ((c) == 0) { logger(std::cerr) << "assert faield "; assert(c); } } while (0)

lf_circular_buffer::lf_circular_buffer(ring_buffer_t *buf)
: d_buf(buf)
{
}

ring_buffer_t* lf_circular_buffer::push(ring_buffer_t* newbuf)
{
    ASSERT(newbuf->full());

    ASSERT(d_buf.load() != newbuf);
    ring_buffer_t* buf = d_buf.load(std::memory_order_relaxed);
    for (;;) {
        // Wait till the current buffer is empty.
        logger(std::cout) << "pushing " << newbuf << ", buf = " << buf << '\n';
        while (buf->empty() == 0)
            std::this_thread::yield();
        ring_buffer_t* b = buf;
        if (d_buf.compare_exchange_weak(buf, newbuf) == false) {
            // Another producer or consumer swapped the buffer after the empty
            // check.
            logger(std::cout) << "another producer took " << b << ", new buf = " << buf << '\n';
            continue;
        }
        if (buf->empty() == 0) {
            // This thread was able to successfully exchange buf and newbuf.
            // However, between buf->empty check and the following compare_exchange another
            // producer called lf_circular_buffer::push and also found the same buf empty
            // and replaced it with its own newbuf and then filled up buf and came back and
            // pushed buf back successfully.
            // This thread then resumed and found that buf is the same and thus
            // compare_exchange succeeded, but the buf is now full.
            // This is unfortunate, because this means that the data in this
            // buf cannot be consumed. It has to be discarded to guarantee the
            // fifo property of the ring buffer. If this data is not discarded,
            // but rather pushed again for consumption, it is possible that the
            // producer which originally filled this buf already pushed some
            // later data and that the later data were already consumed.
            logger(std::cout) << buf << " still not empty\n";
            return buf;
        }
        break;
    }
    logger(std::cout) << "pushed " << newbuf << ", retrieved " << buf << '\n';
    ASSERT(buf->empty());
    return buf;
}

ring_buffer_t* lf_circular_buffer::pop(ring_buffer_t* newbuf)
{
    ASSERT(newbuf->empty());

    ring_buffer_t* buf = d_buf.load(std::memory_order_relaxed);
    for (;;) {
        logger(std::cout) << "pop pushing " << newbuf << ", buf = " << buf << '\n';
        ASSERT(buf != newbuf);
        // Wait till the current buffer has data.
        while (buf->empty())
            std::this_thread::yield();
        ring_buffer_t* b = buf;
        if (d_buf.compare_exchange_weak(buf, newbuf) == false) {
            // Another producer or consumer swapped the buffer after the empty
            // check.
            logger(std::cout) << "another consumer took " << b << ", new buf = " << buf << '\n';
            continue;
        }
        logger(std::cout) << "poped " << buf << '\n';
        if (buf->empty()) {
            // Another consumer took, emptied and returned the buffer between
            // buf->empty check and subsequent compare_exchange.
            // Then this thread resumed and discovered that compare_exchange
            // succeeded, but buf is empty.
            logger(std::cout) << buf << " still empty, trying again\n";
            newbuf = buf;
            buf = d_buf.load(std::memory_order_relaxed);
            continue;
        }
        break;
    }
    logger(std::cout) << "pushed " << newbuf << ", poped buf = " << buf << " " << *buf << '\n';
    ASSERT(buf->full()); // This ASSERT fires somehow misteriosly.
    return buf;
}
