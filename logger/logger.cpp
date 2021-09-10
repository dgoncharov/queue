#include "logger.h"
#include <ostream>
#include <mutex>
#include <sys/types.h>
#undef _GNU_SOURCE
#define _GNU_SOURCE
#include <unistd.h>

static std::mutex mutex;
logger::logger(std::ostream& out)
:d_out(out)
{
    static const thread_local pid_t pid = gettid();
    mutex.lock();
    d_out << pid << " ";
}

logger::~logger()
{
    d_out.flush();
    mutex.unlock();
}
