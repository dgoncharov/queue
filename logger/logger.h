#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <mutex>
#include <sys/types.h>

struct logger {
    explicit logger(std::ostream& out);
    ~logger();

    std::ostream& d_out;
    pid_t d_tid;
};

template <class T>
inline
const logger& operator<<(const logger& log, T x)
{
    log.d_out << x;
    return log;
}

#endif
