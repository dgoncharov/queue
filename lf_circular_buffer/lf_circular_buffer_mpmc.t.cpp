#include "lf_circular_buffer.h"
#include "logger.h"
#include <iostream>
#include <thread>
#include <assert.h>
#include <string.h>

enum {bufsz = 8};
static
std::ostream& operator<<(std::ostream& out, const buffer* buf)
{
    return buf->print(out);
}

static
int producer_main(lf_circular_buffer* q)
{
    logger(std::cout) << __func__ << '\n';
    buffer mybuf(bufsz);
    buffer* buf = &mybuf;
    for (int k = 1; ; ++k) {
        if (k > 1023)
            k = 1;
        if (buf->push(k) == 0)
            // Buffer is full.
            buf = q->exchange(buf);
    }
    return 0;
}

static
int consumer_main(lf_circular_buffer* q)
{
    logger(std::cout) << __func__ << '\n';
    buffer mybuf(bufsz);
    buffer* buf = &mybuf;
    for (;;) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        buf = q->exchange(buf);
        logger(std::cout) << "poped " << buf << '\n';
        buf->clear();
    }
    return 0;
}

int main(int, char* [])
{
    buffer buf(bufsz);
    lf_circular_buffer q(&buf);
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
