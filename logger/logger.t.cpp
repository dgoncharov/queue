#include "logger.h"
#include <iostream>

int main(int, char* [])
{
    logger(std::cout) << __func__ << '\n';

    return 0;
}

/* Copyright (c) 2021 Dmitry Goncharov
 *
 * Distributed under the BSD License.
 * (See accompanying file COPYING).
 */
