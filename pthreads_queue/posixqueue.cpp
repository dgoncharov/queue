#include "posixqueue.h"
#include <assert.h>

class guard {
public:
    explicit guard(pthread_mutex_t& m)
    : d_mutex(m)
    {
        const int rc = pthread_mutex_lock(&d_mutex);
        assert(rc == 0);
    }

    ~guard()
    {
        const int rc = pthread_mutex_unlock(&d_mutex);
        assert(rc == 0);
    }

private:
    pthread_mutex_t& d_mutex;
};

queue::queue()
{
    d_buf.reserve(1024);
    int rc = pthread_mutex_init(&d_mutex, 0);
    assert(rc == 0);
    rc = pthread_cond_init(&d_cond, 0);
    assert(rc == 0);
}

queue::~queue()
{
    int rc = pthread_mutex_destroy(&d_mutex);
    assert(rc == 0);
    rc = pthread_cond_destroy(&d_cond);
    assert(rc == 0);
}

int queue::push(int value)
{
    {
        class guard guard(d_mutex);
        d_buf.push_back(value);
    }
    const int rc = pthread_cond_signal(&d_cond);
    assert(rc == 0);
    return rc;
}

int queue::pop(std::vector<int>* result)
{
    assert(result->empty());
    class guard guard(d_mutex);
    while (d_buf.empty()) {
        const int rc = pthread_cond_wait(&d_cond, &d_mutex);
        assert(rc == 0);
    }
    assert(d_buf.size());
    d_buf.swap(*result);
    return 0;
}
