#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <mutex>

struct logger {
    explicit logger(std::ostream& out, int loglevel = 1);
    ~logger();

    std::ostream& d_out;
    int d_loglevel;
};

template <class T>
inline
const logger& operator<<(const logger& log, T x)
{
    if (log.d_loglevel == 0)
        return log;
    log.d_out << x;
    return log;
}

#endif

/* Copyright (c) 2021 Dmitry Goncharov
 *
 * Distributed under the BSD License.
 * (See accompanying file COPYING).
 */
