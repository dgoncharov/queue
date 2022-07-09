#include "stdqueue.h"
#include <assert.h>

queue::queue()
{
    d_buf.reserve(1024);
}

int queue::push(int value)
{
    {
        std::lock_guard<std::mutex> guard(d_mutex);
        d_buf.push_back(value);
    }
    d_cond.notify_one();
    return 0;
}

int queue::pop(std::vector<int>* result)
{
    assert(result->empty());
    std::unique_lock<std::mutex> lock(d_mutex);
    while (d_buf.empty())
        d_cond.wait(lock);
    assert(d_buf.size());
    d_buf.swap(*result);
    return 0;
}

