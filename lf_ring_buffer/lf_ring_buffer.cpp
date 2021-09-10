#include "lf_ring_buffer.h"
#include "logger.h"
#include <thread>
#include <iostream>
#include <assert.h>
#include <string.h>
#undef _GNU_SOURCE
#define _GNU_SOURCE
#include <unistd.h>

lf_ring_buffer::lf_ring_buffer(size_t size)
: d_size(size), d_begin((buffer*) malloc(size * sizeof(buffer))), d_wpos(d_begin),
d_rpos(d_begin)
{
    assert(size);
    logger(std::cout) << "allocated " << (void*) d_begin << '\n';
    assert(d_begin);

    for (size_t k = 0; k < d_size-1; ++k)
        d_begin[k].next = &d_begin[k+1];
    d_begin[d_size-1].next = d_begin;

    for (size_t k = 0; k < d_size; ++k)
        d_begin[k].value = d_begin[k].owner = 0;
}

lf_ring_buffer::~lf_ring_buffer()
{
    logger(std::cout) << "freeing " << (void*) d_begin << '\n';
    free(d_begin);
}

void lf_ring_buffer::push(int value)
{
    assert(value);

    for (;;) {
        // Advance the pointer to the next slot.
        buffer* pos = d_wpos.load(std::memory_order_relaxed);
        while (d_wpos.compare_exchange_weak(pos, d_wpos.load(std::memory_order_relaxed)->next) == false)
            ;

        // Occupy the chosen slot.
        static const thread_local pid_t mypid = gettid();
        assert(mypid > 0);
        pid_t pid = pos->owner.load(std::memory_order_relaxed);
        assert(pid != mypid);
        pid = 0;
        while (pos->owner.compare_exchange_weak(pid, mypid) == false)
            if (pid > 0)
                break;
            else
                pid = 0;
        if (pid > 0)
            // This slot is occupied by another producer.
            // Move on to the next slot.
            continue;
        logger(std::cout) << " producer occupied slot " << pos << '\n';
        assert(pid != mypid);
        assert(pos->owner == mypid);
        pos->value = value;
        logger(std::cout) << " producer releasing slot " << pos << '\n';
        pos->owner = 0;
        logger(std::cout) << " producer released slot " << pos << '\n';
        break;
    }
}


int lf_ring_buffer::pop()
{
    int result;
    for (;;) {
        // Advance the pointer to the next slot.
        buffer* pos = d_rpos.load(std::memory_order_relaxed);
        while (d_rpos.compare_exchange_weak(pos, d_rpos.load(std::memory_order_relaxed)->next) == false)
            ;

        // Occupy the chosen slot.
        static const thread_local pid_t mypid = gettid();
        assert(mypid > 0);
        pid_t pid = pos->owner.load(std::memory_order_relaxed);
        assert(mypid != pid);
        pid = 0;
        while (pos->owner.compare_exchange_weak(pid, -mypid) == false)
            if (pid < 0)
                break;
            else
                pid = 0;
        if (pid < 0)
            // This slot is occupied by another consumer.
            // Move on to the next slot.
            continue;
        logger(std::cout) << " consumer occupied slot " << pos << '\n';
        assert(pid != -mypid);
        assert(pos->owner == -mypid);

        result = pos->value;
        if (result == 0)
            continue;
        pos->value = 0;
        logger(std::cout) << " consumer releasing slot " << pos << '\n';
        pos->owner = 0;
        logger(std::cout) << " consumer released slot " << pos << '\n';
        break;
    }
    assert(result);
    return result;
}
