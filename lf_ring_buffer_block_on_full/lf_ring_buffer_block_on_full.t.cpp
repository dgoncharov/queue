#include "lf_ring_buffer_block_on_full.h"
#include "logger.h"
#include <iostream>
#include <thread>
#include <assert.h>
#include <string.h>
//#include <sys/types.h>
//#undef _GNU_SOURCE
//#define _GNU_SOURCE
//#include <unistd.h>


enum {bufsz = 16};

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
int producer_main(lf_ring_buffer_block_on_full* q)
{
    logger(std::cout) << __func__ << '\n';
    for (int k = 2; ; ++k) {
        static int count = 0;
        thread_local int id = count++;
        char* value = generate(id, k);
        q->push(value, k);
        // value cannot be accessed after push.
    }
    return 0;
}

static
int consumer_main(lf_ring_buffer_block_on_full* q)
{
    logger(std::cout) << __func__ << '\n';
    for (;;) {
//        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        char* value = q->pop();
        logger(std::cout) << "poped " << value << " at " << (void*) value << '\n';
        thread_local int lastvalue[64] = {};
        int id, k, rc;
        char s[64];
        rc = sscanf(value, "%s %d %d", s,  &id, &k);
        logger(std::cout) << "id = " << id << ", k = " << k << ", rc = " << rc << ", lastvalue[" << id << "] = " << lastvalue[id] << '\n';
        assert(rc == 3);
        assert(k > lastvalue[id]);
        lastvalue[id] = k;
        free(value);
    }
    return 0;
}

int main(int, char* [])
{
    lf_ring_buffer_block_on_full q(bufsz);
    for (int k = 0; k < 4; ++k) {
        std::thread t1(producer_main, &q);
        t1.detach();
        std::thread t2(consumer_main, &q);
        t2.detach();
    }
    for (;;)
        std::this_thread::sleep_for(std::chrono::seconds(1024));
    return 0;
}
