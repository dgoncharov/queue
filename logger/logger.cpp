#include "logger.h"
#include <ostream>
#include <mutex>
#include <sys/types.h>
#undef _GNU_SOURCE
#define _GNU_SOURCE
#include <unistd.h>

static std::mutex mutex;
logger::logger(std::ostream& out, int loglevel)
: d_out(out)
, d_loglevel(loglevel)
{
    if (d_loglevel == 0)
        return;
    static const thread_local pid_t pid = gettid();
    mutex.lock();
    d_out << pid << " ";
}

logger::~logger()
{
    if (d_loglevel == 0)
        return;
    d_out.flush();
    mutex.unlock();
}

/* Copyright (c) 2021 Dmitry Goncharov
 *
 * Distributed under the BSD License.
 * (See accompanying file COPYING).
 */
