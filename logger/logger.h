#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <mutex>

struct logger {
    explicit logger(std::ostream& out);
    ~logger();

    std::ostream& d_out;
};

template <class T>
inline
const logger& operator<<(const logger& log, T x)
{
    log.d_out << x;
    return log;
}

#endif
