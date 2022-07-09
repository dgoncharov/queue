#ifndef POSIXQUEUE_H
#define POSIXQUEUE_H

#include <vector>
#include <pthread.h>

class queue {
public:
    queue();
    ~queue();
    int push(int value);
    int pop(std::vector<int>* result);

private:
    std::vector<int> d_buf;
    mutable pthread_mutex_t d_mutex;
    pthread_cond_t d_cond;
};

#endif
