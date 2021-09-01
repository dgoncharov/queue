#include "posixqueue.h"
#include <iostream>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>

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
void* producer_main(void* que)
{
    queue* q = (queue*) que;
    std::cout << __func__ << std::endl;
    for (int k = 0; k < 1024; ++k) {
        q->push(k);
        std::cout << "tid = " << pthread_self() << " pushed " << k << std::endl;
    }
    return 0;
}

static
void* consumer_main(void* que)
{
    queue* q = (queue*) que;
    std::vector<int> buf;
    buf.reserve(1024);
    std::cout << __func__ << std::endl;
    for (;;) {
        buf.clear();
        q->pop(&buf);
        std::cout << "tid = " << pthread_self() << " poped " << buf << std::endl;
    }
    return 0;
}

int main(int, char* [])
{
    int rc;
    enum { nthreads = 64 };
    queue q;
    pthread_t idp[nthreads];
    pthread_t idc[nthreads];
    for (int k = 0; k < nthreads; ++k) {
        rc = pthread_create(&idp[k], 0, producer_main, &q);
        if (rc) {
            perror("pthread_create");
            return 1;
        }
        rc = pthread_create(&idc[k], 0, consumer_main, &q);
        if (rc) {
            perror("pthread_create");
            return 1;
        }
    }
    std::cout << "joining on producer threads" << std::endl;
    for (int k = 0; k < nthreads; ++k) {
        rc = pthread_join(idp[k], 0);
        assert(rc == 0);
    }
    std::cout << "detaching consumer threads" << std::endl;
    for (int k = 0; k < nthreads; ++k) {
        rc = pthread_detach(idc[k]);
        assert(rc == 0);
    }

    for (;;)
        usleep(1024 * 1000 * 1000);
    return 0;
}
