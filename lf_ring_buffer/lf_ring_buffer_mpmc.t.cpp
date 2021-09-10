#include "lf_ring_buffer.h"
#include "logger.h"
#include <iostream>
#include <thread>
#include <assert.h>
#include <string.h>

enum {bufsz = 8};

static
int producer_main(lf_ring_buffer* q)
{
    logger(std::cout) << __func__ << '\n';
    for (int k = 1; ; ++k) {
        if (k > 1023)
            k = 1;
        q->push(k);
        logger(std::cout) << "pushed " << k << '\n';
    }
    return 0;
}

static
int consumer_main(lf_ring_buffer* q)
{
    logger(std::cout) << __func__ << '\n';
    for (;;) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        const int value = q->pop();
        logger(std::cout) << "poped " << value << '\n';
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
