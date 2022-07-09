#include "lf_circular_buffer.h"
#include "logger.h"
#include <iostream>
#include <thread>
#include <assert.h>
#include "string.h"


enum {bufsz = 8};

static
char* generate(int id, int k)
{
    enum {bufsz = 64};
    char* result = (char*) malloc(bufsz);
    assert(result);
    const int rc = sprintf(result, "hello %d %d", id, k);
    assert(rc < bufsz);
    return result;
}

static
int producer_main(lf_circular_buffer* q, int loglevel)
{
    static std::atomic<int> count = 0;
    thread_local int id = count++;
    logger(std::cout, loglevel) << __func__ << '\n';
    buffer_t mybuf;
    mybuf.reserve(bufsz);
    buffer_t* buf = &mybuf;
    logger(std::cout, loglevel) << "producer starting with buf at " << buf << '\n';
    for (int k = 1; k > 0; ++k) {
        char* value = generate(id, k);
        buf->push_back(value);
        logger(std::cout, loglevel) << "pushed " << value << " to " << buf << '\n';
        if (buf->size() >= bufsz) {
            buf = q->push(buf);
            // Have to clear, because buf may not be empty.
            buf->clear();
            // value cannot be used after push.
        }
    }
    logger(std::cout, loglevel) << "producer " << id << " exiting\n";
    return 0;
}

static
int consumer_main(lf_circular_buffer* q, int loglevel)
{
    logger(std::cout, loglevel) << __func__ << '\n';
    buffer_t mybuf;
    mybuf.reserve(bufsz);
    buffer_t* buf = &mybuf;
    logger(std::cout, loglevel) << "consumer starting with buf at " << buf << '\n';
    for (;;) {
        buf = q->pop(buf);
        assert(buf->size() == bufsz);
        for (char* value: *buf) {
            assert(value);
            char s[64];
            int id, newval;
            sscanf(value, "%s %d %d", s, &id, &newval);
            free(value);
            thread_local int lastvalue[64] = {};
            logger(std::cout, loglevel)
                << "id = " << id
                << ", newval = " << newval
                << ", lastvalue = " << lastvalue[id]
                << '\n';
            assert(lastvalue[id] < newval);
            lastvalue[id] = newval;
        }
        buf->clear();
        assert(buf->empty());
    }
    return 0;
}

int main(int argc, char* argv[])
{
    if (argc > 1 && memcmp(argv[1], "-h", 2) == 0) {
        std::cout << argv[0] << " [loglevel=0] [impl loglevel=0]" << std::endl;
        return 0;
    }
    const int loglevel = argc > 1 ? atoi(argv[1]) : 0;
    const int impl_loglevel = argc > 2 ? atoi(argv[2]) : 0;
    buffer_t mybuf;
    mybuf.reserve(bufsz);
    logger(std::cout, loglevel) << "main starting with buf at " << &mybuf << '\n';
    lf_circular_buffer q(&mybuf, impl_loglevel);
    for (int k = 0; k < 8; ++k) {
        std::thread t1(producer_main, &q, loglevel);
        t1.detach();
        std::thread t2(consumer_main, &q, loglevel);
        t2.detach();
    }
    for (;;)
        std::this_thread::sleep_for(std::chrono::seconds(1024));
    return 0;
}

/* Copyright (c) 2021 Dmitry Goncharov
 *
 * Distributed under the BSD License.
 * (See accompanying file COPYING).
 */
