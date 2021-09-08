#include "logger.h"
#include <ostream>
#include <mutex>
#undef _GNU_SOURCE
#define _GNU_SOURCE
#include <unistd.h>

static std::mutex mutex;
logger::logger(std::ostream& out)
:d_out(out), d_tid(gettid())
{
    mutex.lock();
    d_out << d_tid << " ";
}

logger::~logger()
{
    d_out.flush();
    mutex.unlock();
}
