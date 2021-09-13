#include "lf_ring_buffer_block_on_full.h"
#include "logger.h"
#include <thread>
#include <iostream>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#undef _GNU_SOURCE
#define _GNU_SOURCE
#include <unistd.h>

lf_ring_buffer_block_on_full::lf_ring_buffer_block_on_full(size_t size)
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
        d_begin[k].value = 0;

    for (size_t k = 0; k < d_size; ++k)
        assert(d_begin[k].next);
}

lf_ring_buffer_block_on_full::~lf_ring_buffer_block_on_full()
{
    logger(std::cout) << "freeing " << (void*) d_begin << '\n';
    free(d_begin);
}

void lf_ring_buffer_block_on_full::push(char* value, int k)
{
    assert(value);
    thread_local int lastvalue = 1;
    assert(k == lastvalue + 1);
    lastvalue = k;

    // Advance the pointer to the next slot.
    buffer* pos = d_wpos.load(std::memory_order_relaxed);
    logger(std::cout) << "advancing " << pos << '\n';
    while (d_wpos.compare_exchange_weak(pos, d_wpos.load(std::memory_order_relaxed)->next) == false)
        ;
////        std::this_thread::yield();

    buffer* next = pos->next.load(std::memory_order_relaxed);
    while (next == d_rpos.load(std::memory_order_relaxed)) {
//        std::this_thread::yield();
        logger(std::cout) << "buffer is full\n";
    }

    logger(std::cout) << "pushing " << value << " at " << (void*) value << '\n';
    char* zero = 0;
    while (pos->value.compare_exchange_weak(zero, value) == false) {
        zero = 0;
//        std::this_thread::yield();
    }
    // value cannot be accessed below this line.
    // A consumer may have already consumed and freed this value.
}


char* lf_ring_buffer_block_on_full::pop()
{
    char* result;
    for (;;) {
        // Advance the pointer to the next slot.
        buffer* pos = d_rpos.load(std::memory_order_relaxed);
        logger(std::cout) << "advancing " << pos << '\n';
        while (d_rpos.compare_exchange_weak(pos, d_rpos.load(std::memory_order_relaxed)->next) == false)
            ;
//////            std::this_thread::yield();

        logger(std::cout) << "comparing pos = " << pos << ", wpos = " << d_wpos.load() << '\n';
        logger(std::cout) << "pos = " << pos
            << ", begin = " << d_begin
            << ", slot = " << pos - d_begin << '\n';
        while (pos == d_wpos.load(std::memory_order_relaxed)) {
////            std::this_thread::yield();
            logger(std::cout) << "buffer is empty\n";
        }
        // 2 consumers can be reading this slot at the same time.
        // Both will attempt to set pos->value to 0.
        // One will succeed and take the value.
        // The other will have to look for another slot.
        char* v = pos->value.load(std::memory_order_relaxed);
        while (pos->value.compare_exchange_weak(v, 0) == false)
            ;
//                std::this_thread::yield();
        if (v == 0) {
            logger(std::cout) << "The other consumer took and reset this value.\n";
            continue; // The other consumer took and reset this value.
        }
        result = v;
        break;
    }
    assert(result);
    return result;
}
