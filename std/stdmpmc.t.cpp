#include "stdqueue.h"
#include <iostream>
#include <thread>
#include <assert.h>

static
std::ostream& operator<<(std::ostream& out, const std::vector<int>& x)
{
    out << x.size() << " elements ";
    const char* comma = "";
    for (size_t k = 0, len = x.size(); k < len; ++k, comma = ", ")
        out << comma << x[k];
    out << std::endl;
    return out;
}

static
int producer_main(queue* q)
{
    std::cout << __func__ << std::endl;
    for (int k = 0; k < 1024; ++k) {
        q->push(k);
        std::cout << "tid = " << std::this_thread::get_id() << " pushed " << k << std::endl;
    }
    return 0;
}

static
int consumer_main(queue* q)
{
    std::vector<int> buf;
    buf.reserve(1024);
    std::cout << __func__ << std::endl;
    for (;;) {
        buf.clear();
        q->pop(&buf);
        std::cout << "tid = " << std::this_thread::get_id() << " poped " << buf << std::endl;
    }
    return 0;
}

int main(int, char* [])
{
    queue q;
//    std::thread t1(producer_main, &q);
//    std::thread t2(consumer_main, &q);
//    t1.join();
//    t2.join();
    for (int k = 0; k < 64; ++k) {
        std::thread t1(producer_main, &q);
        t1.detach();
        std::thread t2(consumer_main, &q);
        t2.detach();
    }
    for (;;)
        std::this_thread::sleep_for(std::chrono::seconds(1024));
    return 0;
}
