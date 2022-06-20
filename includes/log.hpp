#ifndef LOG_HPP
#define LOG_HPP

#include <ctime>
#include <iostream>

namespace log
{

namespace time
{

#define BUFFER_SIZE 32
time_t raw;
char   buffer[BUFFER_SIZE];

} // namespace time

void
    timestamp()
{
    std::time(&time::raw);
    if (std::strftime(time::buffer, BUFFER_SIZE, "%Y-%m-%d %H:%M:%S",
                      std::localtime(&time::raw)))
        std::cout << time::buffer << ": ";
}

std::ostream&
    print()
{
    log::timestamp();
    return std::cout;
}

std::ostream&
    endl(std::ostream& os)
{
    os.put(os.widen('\n'));
    os.flush();
    return os;
}

} // namespace log
#endif /* LOG_HPP */
