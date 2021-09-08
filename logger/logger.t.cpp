#include "logger.h"
#include <iostream>

int main(int, char* [])
{
    logger(std::cout) << __func__ << '\n';

    return 0;
}
