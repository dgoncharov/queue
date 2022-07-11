#include "lf_ring_buffer.h"
#include "logger.h"
#include <iostream>
#include <thread>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#undef _GNU_SOURCE
#define _GNU_SOURCE
#include <unistd.h>


enum {bufsz = 256};

static
char* generate(pid_t pid, int k)
{
    enum {bufsz = 64};
    char* result = (char*) malloc(bufsz);
    assert(result);
    const int rc = sprintf(result, "hello %d %d", pid, k);
    assert(rc < bufsz);
    return result;
}

static
int producer_main(lf_ring_buffer* q)
{
    static const thread_local pid_t pid = gettid();
    logger(std::cout) << __func__ << '\n';
    for (int k = 0; ; ++k) {
        char* value = generate(pid, k);
        q->push(value);
        // value cannot be accessed after push.
    }
    return 0;
}

static
int consumer_main(lf_ring_buffer* q)
{
    logger(std::cout) << __func__ << '\n';
    for (;;) {
//        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        char* value = q->pop();
        logger(std::cout) << "poped " << value << '\n';
        free(value);
    }
    return 0;
}

int main(int, char* [])
{
    lf_ring_buffer q(bufsz);
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
