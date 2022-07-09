#ifndef LF_CIRCULAR_BUFFER_H
#define LF_CIRCULAR_BUFFER_H

#include <atomic>
#include <vector>

typedef std::vector<char*> buffer_t;
class lf_circular_buffer {
public:
    lf_circular_buffer(buffer_t* buf, int loglevel);
    buffer_t* push(buffer_t* buf);
    buffer_t* pop(buffer_t* buf);

private:
    std::atomic<buffer_t*> d_buf;
    int d_loglevel;
};

#endif

/* Copyright (c) 2021 Dmitry Goncharov
 *
 * Distributed under the BSD License.
 * (See accompanying file COPYING).
 */
