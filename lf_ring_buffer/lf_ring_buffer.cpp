#include "lf_ring_buffer.h"
#include "logger.h"
#include <thread>
#include <iostream>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#undef _GNU_SOURCE
#define _GNU_SOURCE
#include <unistd.h>

lf_ring_buffer::lf_ring_buffer(size_t size)
: d_size(size), d_begin((buffer*) malloc(size * sizeof(buffer))), d_wpos(d_begin),
d_rpos(d_begin), d_wcnt(0), d_rcnt(0)
{
    assert(size);
    logger(std::cout) << "allocated " << (void*) d_begin << '\n';
    assert(d_begin);

    for (size_t k = 0; k < d_size-1; ++k)
        d_begin[k].next = &d_begin[k+1];
    d_begin[d_size-1].next = d_begin;

    for (size_t k = 0; k < d_size; ++k)
        d_begin[k].value = 0, d_begin[k].owner = 0;
}

lf_ring_buffer::~lf_ring_buffer()
{
    logger(std::cout) << "freeing " << (void*) d_begin << '\n';
    free(d_begin);
}

void lf_ring_buffer::push(char* value)
{
    assert(value);
    assert(d_rcnt <= d_wcnt);

    logger(std::cout) << "pushing " << value << '\n';
    for (;;) {
        // Advance the pointer to the next slot.
        buffer* pos = d_wpos.load();
        logger(std::cout) << "advancing " << pos << '\n';
        while (d_wpos.compare_exchange_weak(pos, d_wpos.load()->next) == false)
            std::this_thread::yield();

        // Occupy the chosen slot.
        logger(std::cout) << "advanced, wpos = " << d_wpos.load()<< '\n';
        logger(std::cout) << "locking " << pos << '\n';
        static const thread_local pid_t mypid = gettid();
        assert(mypid > 0);
        pid_t pid = pos->owner.load();
        logger(std::cout) << "slot = " << pos << ", pid = " << pid << " mypid = " << mypid << '\n';
        assert(pid != mypid);
        pid = 0;
        for (; pos->owner.compare_exchange_weak(pid, mypid) == false; pid = 0)
            if (pid > 0)
                break;
            else
                std::this_thread::yield();
        // This is a critical section. If the scheduler decides to sleep this
        // thread now, the threads spinning on pos->owner will be burning cpu.
        // This whole owner based synchronization resembles a lame impl of a
        // mutex.
        if (pid > 0) {
            // This slot is occupied by another producer.
            // Move on to the next slot.
            logger(std::cout) << pos << " is occupied by another producer\n";
            continue;
        }
        const int rcnt = d_rcnt.load(), wcnt = d_wcnt.load();
        logger(std::cout) << "locked " << pos
            << ", rcnt = " << rcnt << ", wcnt = " << wcnt << '\n';
        assert(pid == 0);
        assert(rcnt <= wcnt);
        assert(pos->owner == mypid);
        if (pos->value) {
            logger(std::cout) << "full slot\n";
            pos->owner = 0;
            logger(std::cout) << "advancing " << pos << '\n';
            while (d_wpos.compare_exchange_weak(pos, d_wpos.load()->next) == false)
                std::this_thread::yield();
            continue;
        }
        ++d_wcnt;
        assert(pos->value == 0);
        pos->value = value;
        logger(std::cout) << "pushed " << value << ", pos = " << pos << '\n';
        pos->owner = 0;
        // value cannot be accessed below this line.
        // A consumer may have already consumed and freed this value.
        logger(std::cout) << "released " << pos << ", pos->owner = " << pos->owner.load() << '\n';
        break;
    }
}


char* lf_ring_buffer::pop()
{
    assert(d_rcnt <= d_wcnt);
    char* result;
    logger(std::cout) << "popping\n";
    for (;;) {
        // Advance the pointer to the next slot.
        buffer* pos = d_rpos.load();
        logger(std::cout) << "advancing " << pos << '\n';
        while (d_rpos.compare_exchange_weak(pos, d_rpos.load()->next) == false)
            std::this_thread::yield();

        // Occupy the chosen slot.
        logger(std::cout) << "locking " << pos << '\n';
        static const thread_local pid_t mypid = gettid();
        assert(mypid > 0);
        pid_t pid = pos->owner.load();
        assert(mypid != pid);
        pid = 0;
        for (; pos->owner.compare_exchange_weak(pid, -mypid) == false; pid = 0)
            if (pid < 0)
                break;
            else
                std::this_thread::yield();
        if (pid < 0) {
            // This slot is occupied by another consumer.
            // Move on to the next slot.
            logger(std::cout) << pos << " is occupied by another consumer\n";
            continue;
        }
        const int rcnt = d_rcnt.load(), wcnt = d_wcnt.load();
        logger(std::cout)
            << "locked " << pos
            << ", rcnt = " << rcnt << ", wcnt = " << wcnt << '\n';
        assert(pid == 0);
        assert(rcnt <= wcnt);
        assert(pid != -mypid);
        assert(pos->owner == -mypid);

        result = pos->value;
        logger(std::cout) << "read " << (result ? result : "null") << '\n';
        pos->value = 0;
        logger(std::cout) << "releasing " << pos << '\n';
        pos->owner = 0; // Release the slot.
        if (result == 0) {
            logger(std::cout) << "empty slot\n";
            logger(std::cout) << "advancing " << pos << '\n';
            while (d_rpos.compare_exchange_weak(pos, d_rpos.load()->next) == false)
                std::this_thread::yield();
            continue;
        }
        ++d_rcnt;
        break;
    }
    assert(result);
    return result;
}
