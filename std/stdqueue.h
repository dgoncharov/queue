#ifndef STDQUEUE_H
#define STDQUEUE_H

#include <vector>
#include <mutex>
#include <condition_variable>

class queue {
public:
    queue();
    int push(int value);
    int pop(std::vector<int>* result);

private:
    std::vector<int> d_buf;
    mutable std::mutex d_mutex;
    std::condition_variable d_cond;
};

#endif
