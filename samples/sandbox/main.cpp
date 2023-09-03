#include "src/log/public/logger.h"
#include <iostream>
#include <cstdio>

int main()
{
    LOG_INFO_FILE(Cat, "./log.txt", "Hello, my reaction to this information");
    LOG_WARN_FILE(Cat, "./log.txt", "Hello, my reaction to this information");
    LOG_ERROR_FILE(Cat, "./log.txt", "Hello, my reaction to this information");
    return 0;
}