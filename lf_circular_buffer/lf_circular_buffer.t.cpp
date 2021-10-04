#include "lf_circular_buffer.h"
#include "ring_buffer.h"
#include "logger.h"
#include <iostream>
#include <thread>
#include <assert.h>


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
int producer_main(lf_circular_buffer* q)
{
    static std::atomic<int> count = 0;
    thread_local int id = count++;
    logger(std::cout) << __func__ << '\n';
    char* buffer[bufsz];
    ring_buffer<char*> mybuf(buffer, bufsz);
    assert(mybuf.empty());
    ring_buffer<char*>* buf = &mybuf;
    logger(std::cout) << "producer starting with buf at " << buf << '\n';
    for (int k = 1; k > 0; ++k) {
        char* value = generate(id, k);
        buf->push(value);
//        logger(std::cout) << "pushed " << value << " to " << buf << '\n';
        if (buf->full()) {
            buf = q->push(buf);
            // Have to clear, because buf may not be empty.
            buf->clear();
            // value cannot be used after push.
        }
    }
    logger(std::cout) << "producer " << id << " exiting\n";
    return 0;
}

static
int consumer_main(lf_circular_buffer* q)
{
    logger(std::cout) << __func__ << '\n';
    char* buffer[bufsz];
    ring_buffer<char*> mybuf(buffer, bufsz);
    assert(mybuf.empty());
    ring_buffer<char*>* buf = &mybuf;
    logger(std::cout) << "consumer starting with buf at " << buf << '\n';
    for (;;) {
        buf = q->pop(buf);
        if (buf->empty())
            continue;
        assert(buf->full());
        for (int k = 0, len = buf->size(); k < len; ++k) {
            char* value = buf->pop();
            assert(value);
            char s[64];
            int id, newval;
            sscanf(value, "%s %d %d", s, &id, &newval);
            free(value);
            thread_local int lastvalue[64] = {};
//            logger(std::cout)
//                << "id = " << id
//                << ", newval = " << newval
//                << ", lastvalue = " << lastvalue[id]
//                << '\n';
            assert(lastvalue[id] < newval);
            lastvalue[id] = newval;
        }
        buf->clear();
        assert(buf->empty());
    }
    return 0;
}

int main(int, char* [])
{
    char* buffer[bufsz];
    ring_buffer<char*> mybuf(buffer, bufsz);
    logger(std::cout) << "main starting with buf at " << &mybuf << '\n';
    lf_circular_buffer q(&mybuf);
    for (int k = 0; k < 8; ++k) {
        std::thread t1(producer_main, &q);
        t1.detach();
        std::thread t2(consumer_main, &q);
        t2.detach();
    }
    for (;;)
        std::this_thread::sleep_for(std::chrono::seconds(1024));
    return 0;
}
