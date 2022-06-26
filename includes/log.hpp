#pragma once
#ifndef LOG_HPP
#define LOG_HPP

#include <ctime>
#include <iostream>

namespace log
{

namespace time
{

#define BUFFER_SIZE 32
static time_t raw;
static char   buffer[BUFFER_SIZE];

} // namespace time

void
    timestamp();
std::ostream&
    print();
std::ostream&
    endl(std::ostream&);

} // namespace log
#endif /* LOG_HPP */
